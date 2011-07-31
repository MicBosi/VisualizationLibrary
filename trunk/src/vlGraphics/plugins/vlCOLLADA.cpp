/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#include <vlGraphics/plugins/vlCOLLADA.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Light.hpp>
#include <vlCore/Time.hpp>
#include <set>
#include <dae.h>
#include <dom.h>
#include <dom/domCOLLADA.h>
#include <dom/domProfile_COMMON.h>

using namespace vl;

// mic fixme:

// CODE: put everything in the COLLADA loader class and cleanup

// TODO
// - support blending/transparency
// - support two sides
// - default material values
// - parse and use lights
// ---
// - output more diagnostic messages with debug verbosity.
// - parse and export cameras.
// - materials should be reused as much as possible from their parent-effect,
//   for profile_COMMON we should be able to share GLSLProgram and Materials.
// - proper TEXTURE and input semantic binding logic.

// --- mic fixme: remove debug code ---
bool debug_Wireframe = false;
// ---
int debug_IndentLevel = 0;
// ---
mat4 debug_WorldMatrix;
// ---
std::vector< ref<Actor> > debug_Actors;
// ---
float debug_GeometryTime = 0;
// ---
Time debug_Timer;
// ---
void debug_PrintIndent(const String& str)
{
  for(int i=0; i<debug_IndentLevel; ++i)
    Log::print("  ");
  Log::print(str);
}
// ---
bool debug_IsMatrixSane(const mat4& m)
{
  for(int i=0; i<16; i++)
    if( m.ptr()[i] != m.ptr()[i] )
      return false;
  return true;
}
// ---
void debug_PrintMatrix(const fmat4& m)
{
  debug_PrintIndent("<matrix>\n");
  debug_PrintIndent( Say("%n %n %n %n\n") << m.e(0, 0) << m.e(0, 1) << m.e(0, 2) << m.e(0, 3) );
  debug_PrintIndent( Say("%n %n %n %n\n") << m.e(1, 0) << m.e(1, 1) << m.e(1, 2) << m.e(1, 3) );
  debug_PrintIndent( Say("%n %n %n %n\n") << m.e(2, 0) << m.e(2, 1) << m.e(2, 2) << m.e(2, 3) );
  debug_PrintIndent( Say("%n %n %n %n\n") << m.e(3, 0) << m.e(3, 1) << m.e(3, 2) << m.e(3, 3) );
}
// ---
int debug_ExtractGeometry = false;
// ---
void debug_PrintMatrices(const Transform* tr)
{
  debug_IndentLevel++;

  debug_PrintMatrix( tr->worldMatrix() );
  for(size_t i=0; i<tr->childrenCount(); ++i)
  {
    debug_PrintMatrices( tr->children()[i].get() );
  }

  debug_IndentLevel--;
}
// ---
void debug_PrintGometry(Actor* actor)
{
  Geometry* geom = actor->lod(0)->as<Geometry>();

  debug_PrintIndent( "[VERTICES]\n" );
  for(size_t i=0; i<geom->vertexArray()->size(); ++i)
  {
    vec3 v = geom->vertexArray()->getAsVec3(i);
    debug_PrintIndent( Say("    %n %n %n \n") << v.x() << v.y() << v.z() );
  }
  debug_PrintIndent( "[/VERTICES]\n" );

  debug_PrintIndent( "[NORMALS]\n" );
  for(size_t i=0; i<geom->normalArray()->size(); ++i)
  {
    vec3 v = geom->normalArray()->getAsVec3(i);
    debug_PrintIndent( Say("    %n %n %n \n") << v.x() << v.y() << v.z() );
  }
  debug_PrintIndent( "[/NORMALS]\n" );

  debug_PrintIndent( "[TRIANGLES]\n" );
  for(int i=0; i<geom->drawCalls()->size(); ++i)
  {
    DrawCall* dc = geom->drawCalls()->at(i);
    TriangleIterator it = dc->triangleIterator();
    for( ; it.hasNext(); it.next() )
      debug_PrintIndent( Say("  %n %n %n \n") << it.a() << it.b() << it.c() );
  }
  debug_PrintIndent( "[/TRIANGLES]\n" );
}
// ---

//-----------------------------------------------------------------------------
const char* NO_MATERIAL_SPECIFIED = "<NO_MATERIAL_SPECIFIED>";
//-----------------------------------------------------------------------------
typedef enum
{
  IS_UNKNOWN,
  IS_BINORMAL,
  IS_COLOR,
  IS_CONTINUITY,
  IS_IMAGE,
  IS_INPUT,
  IS_IN_TANGENT,
  IS_INTERPOLATION,
  IS_INV_BIND_MATRIX,
  IS_JOINT,
  IS_LINEAR_STEPS,
  IS_MORPHS_TARGET,
  IS_MORPH_WEIGHT,
  IS_NORMAL,
  IS_OUTPUT,
  IS_OUT_TANGENT,
  IS_POSITION,
  IS_TANGENT,
  IS_TEXBINORMAL,
  IS_TEXCOORD,
  IS_TEXTANGENT,
  IS_UV,
  IS_VERTEX,
  IS_WEIGHT
} EInputSemantic;
//-----------------------------------------------------------------------------
struct 
{
  EInputSemantic mSemantic;
  const char* mSemanticString;
} SemanticTable[] = 
  {
    { IS_UNKNOWN,         "UNKNOWN"         },
    { IS_BINORMAL,        "BINORMAL"        },
    { IS_COLOR,           "COLOR"           },
    { IS_CONTINUITY,      "CONTINUITY"      },
    { IS_IMAGE,           "IMAGE"           },
    { IS_INPUT,           "INPUT"           },
    { IS_IN_TANGENT,      "IN_TANGENT"      },
    { IS_INTERPOLATION,   "INTERPOLATION"   },
    { IS_INV_BIND_MATRIX, "INV_BIND_MATRIX" },
    { IS_JOINT,           "JOINT"           },
    { IS_LINEAR_STEPS,    "LINEAR_STEPS"    },
    { IS_MORPHS_TARGET,   "MORPHS_TARGET"   },
    { IS_MORPH_WEIGHT,    "MORPH_WEIGHT"    },
    { IS_NORMAL,          "NORMAL"          },
    { IS_OUTPUT,          "OUTPUT"          },
    { IS_OUT_TANGENT,     "OUT_TANGENT"     },
    { IS_POSITION,        "POSITION"        },
    { IS_TANGENT,         "TANGENT"         },
    { IS_TEXBINORMAL,     "TEXBINORMAL"     },
    { IS_TEXCOORD,        "TEXCOORD"        },
    { IS_TEXTANGENT,      "TEXTANGENT"      },
    { IS_UV,              "UV"              },
    { IS_VERTEX,          "VERTEX"          },
    { IS_WEIGHT,          "WEIGHT"          },
    { IS_UNKNOWN,          NULL             }
  };
//-----------------------------------------------------------------------------
EInputSemantic getSemantic(const char* semantic)
{
  for(int i=0; SemanticTable[i].mSemanticString; ++i)
  {
    if (strcmp(semantic, SemanticTable[i].mSemanticString) == 0)
      return SemanticTable[i].mSemantic;
  }

  return IS_UNKNOWN;
}
//-----------------------------------------------------------------------------
const char* getSemanticString(EInputSemantic semantic)
{
  for(int i=0; SemanticTable[i].mSemanticString; ++i)
  {
    if ( semantic == SemanticTable[i].mSemantic )
      return SemanticTable[i].mSemanticString;
  }

  return NULL;
}
//-----------------------------------------------------------------------------
ETexParamFilter translateSampleFilter(domFx_sampler_filter_common filter)
{
  switch(filter)
  {
	  case FX_SAMPLER_FILTER_COMMON_NEAREST:                return TPF_NEAREST;
	  case FX_SAMPLER_FILTER_COMMON_LINEAR:                 return TPF_LINEAR;
	  case FX_SAMPLER_FILTER_COMMON_NEAREST_MIPMAP_NEAREST: return TPF_NEAREST_MIPMAP_NEAREST;
	  case FX_SAMPLER_FILTER_COMMON_LINEAR_MIPMAP_NEAREST:  return TPF_LINEAR_MIPMAP_NEAREST;
	  case FX_SAMPLER_FILTER_COMMON_NEAREST_MIPMAP_LINEAR:  return TPF_NEAREST_MIPMAP_LINEAR;
	  case FX_SAMPLER_FILTER_COMMON_LINEAR_MIPMAP_LINEAR:   return TPF_LINEAR_MIPMAP_LINEAR;
	  default:                                              return (ETexParamFilter)0;
  }
}
//-----------------------------------------------------------------------------
ETexParamWrap translateWrapMode(domFx_sampler_wrap_common wrap)
{
  switch(wrap)
  {
	  case FX_SAMPLER_WRAP_COMMON_WRAP:   return TPW_REPEAT;
	  case FX_SAMPLER_WRAP_COMMON_MIRROR: return TPW_MIRRORED_REPEAT;
	  case FX_SAMPLER_WRAP_COMMON_CLAMP:  return TPW_CLAMP;
	  case FX_SAMPLER_WRAP_COMMON_BORDER: return TPW_CLAMP_TO_BORDER;
	  default:                            return (ETexParamWrap)0;
  }
}
//-----------------------------------------------------------------------------
void parseColor(domCommon_color_or_texture_typeRef color_or_texture, fvec4* out_col)
{
  if (color_or_texture && color_or_texture->getColor())
  {
    domFx_color_common& col = color_or_texture->getColor()->getValue();
    *out_col = fvec4( (float)col[0], (float)col[1], (float)col[2], (float)col[3] );
  }
}
//-----------------------------------------------------------------------------
struct DaeVert
{
  static const int MAX_ATTRIBS = 8;

  DaeVert()
  {
    memset(mAttribIndex, 0xFF, sizeof(mAttribIndex));
    mIndex = (size_t)-1;
  }

  bool operator<(const DaeVert& other) const
  {
    for(int i=0; i<MAX_ATTRIBS; ++i)
    {
      if (mAttribIndex[i] != other.mAttribIndex[i])
        return mAttribIndex[i] < other.mAttribIndex[i];
    }
    return false;
  }

  size_t mAttribIndex[MAX_ATTRIBS];
  size_t mIndex;
};
//-----------------------------------------------------------------------------
class DaeSource: public Object
{
public:
  DaeSource()
  {
    mFloatSource = NULL;
    mIntSource   = NULL;
    mBoolSource  = NULL;
    mFieldsMask = 0;
    mStride     = 0;
    mOffset     = 0;
    mCount      = 0;
    mDataSize   = 0;
  }

  //! Initializes an accessor. An accessor can read only up to 32 floats.
  void init(domFloat_arrayRef data_src, domUint count, domUint stride, domUint offset, size_t fields_mask)
  {
    mFloatSource = data_src;
    mIntSource   = NULL;
    mBoolSource  = NULL;
    mCount       = (size_t)count;
    mStride      = (size_t)stride;
    mOffset      = (size_t)offset;
    mFieldsMask  = fields_mask;

    // count the number of scalars that will be read.
    mDataSize = 0;
    for(size_t i=0; i<32; ++i)
      if (mFieldsMask & (1<<i))
        mDataSize++;
  }

  //! Initializes an accessor. An accessor can read only up to 32 floats.
  void init(domInt_arrayRef data_src, domUint count, domUint stride, domUint offset, size_t fields_mask)
  {
    mFloatSource = NULL;
    mIntSource   = data_src;
    mBoolSource  = NULL;
    mCount       = (size_t)count;
    mStride      = (size_t)stride;
    mOffset      = (size_t)offset;
    mFieldsMask  = fields_mask;

    // count the number of scalars that will be read.
    mDataSize = 0;
    for(size_t i=0; i<32; ++i)
      if (mFieldsMask & (1<<i))
        mDataSize++;
  }

  //! Initializes an accessor. An accessor can read only up to 32 floats.
  void init(domBool_arrayRef data_src, domUint count, domUint stride, domUint offset, size_t fields_mask)
  {
    mFloatSource = NULL;
    mIntSource   = NULL;
    mBoolSource  = data_src;
    mCount       = (size_t)count;
    mStride      = (size_t)stride;
    mOffset      = (size_t)offset;
    mFieldsMask  = fields_mask;

    // count the number of scalars that will be read.
    mDataSize = 0;
    for(size_t i=0; i<32; ++i)
      if (mFieldsMask & (1<<i))
        mDataSize++;
  }

  //! Reads an element of data at the n-th position and writes it into 'output'. The number of elements that will be written can be queried by calling dataSize().
  void readData(size_t n, float* output)
  {
    size_t read_pos = mOffset + n * mStride;

    size_t pos = 0;

    if(mFloatSource)
    {
      VL_CHECK( (n < mCount) || (read_pos < mFloatSource->getValue().getCount() - mDataSize) )
      for(size_t i=0; i<32 && i<mStride; ++i)
        if (mFieldsMask & (1<<i))
          output[pos++] = (float)mFloatSource->getValue()[read_pos+i];
    }
    else
    if(mIntSource)
    {
      VL_CHECK( (n < mCount) || (read_pos < mIntSource->getValue().getCount() - mDataSize) )
      for(size_t i=0; i<32 && i<mStride; ++i)
        if (mFieldsMask & (1<<i))
          output[pos++] = (float)mIntSource->getValue()[read_pos+i];
    }
    else
    if(mBoolSource)
    {
      VL_CHECK( (n < mCount) || (read_pos < mBoolSource->getValue().getCount() - mDataSize) )
      for(size_t i=0; i<32 && i<mStride; ++i)
        if (mFieldsMask & (1<<i))
          output[pos++] = (float)mBoolSource->getValue()[read_pos+i];
    }
  }

  //! The number of elements in the source.
  size_t count() const { return mCount; }

  //! The number of elements written by readData().
  size_t dataSize() const { return mDataSize; }

protected:
  size_t mFieldsMask;
  size_t mDataSize;
  domFloat_arrayRef mFloatSource;
  domInt_arrayRef  mIntSource;
  domBool_arrayRef mBoolSource;
  size_t mStride;
  size_t mOffset;
  size_t mCount;
};
//-----------------------------------------------------------------------------
struct DaeInput: public Object
{
  DaeInput()
  {
    mSemantic = IS_UNKNOWN;
    mOffset = 0;
    mSet = 0;
  }

  ref<DaeSource> mSource;
  EInputSemantic mSemantic;
  size_t mOffset;
  size_t mSet;
};
//-----------------------------------------------------------------------------
struct DaePrimitive: public Object
{
  DaePrimitive()
  {
    mIndexStride = 0;
    mCount = 0;
    mType = UNKNOWN;
  }

  enum { UNKNOWN, LINES, LINE_STRIP, POLYGONS, POLYLIST, TRIANGLES, TRIFANS, TRISTRIPS } mType;

  std::string mMaterial;
  std::vector< ref<DaeInput> > mChannels;
  size_t mCount;
  std::vector<domPRef> mP;
  size_t mIndexStride;
  ref<Geometry> mGeometry;

  void generateGeometry()
  {
    VL_CHECK(mIndexStride);

    mGeometry = new Geometry;

    // generate index buffers & DrawElements
    std::set<DaeVert> vert_set;
    for(size_t ip=0; ip<mP.size(); ++ip)
    {
      ref<ArrayUInt1> index_buffer = new ArrayUInt1;
      index_buffer->resize( mP[ip]->getValue().getCount() / mIndexStride );

      for(size_t ivert=0, iidx=0; ivert<mP[ip]->getValue().getCount(); ivert+=mIndexStride, ++iidx)
      {
        DaeVert vert;

        const domListOfUInts& p = mP[ip]->getValue();

        // fill vertex info
        for(size_t ichannel=0; ichannel<mChannels.size(); ++ichannel)
          vert.mAttribIndex[ichannel] = (size_t)p[ivert + mChannels[ichannel]->mOffset];

        size_t final_index = 0xFFFFFFFF;
        // retrieve/insert the vertex
        std::set<DaeVert>::iterator it = vert_set.find(vert);
        if (it == vert_set.end())
        {
          vert.mIndex = final_index = vert_set.size();
          vert_set.insert(vert);
        }
        else
          final_index = it->mIndex;
        
        // this is the actual index
        (*index_buffer)[iidx] = final_index;
      }

      // fill the DrawElementsUInt
      ref<DrawElementsUInt> de;
      switch(mType)
      {
        case LINES:      de = new DrawElementsUInt( PT_LINES ); break;
        case LINE_STRIP: de = new DrawElementsUInt( PT_LINE_STRIP ); break;
        case POLYGONS:   de = new DrawElementsUInt( PT_POLYGON ); break;
        case TRIFANS:    de = new DrawElementsUInt( PT_TRIANGLE_FAN ); break;
        case TRIANGLES:  de = new DrawElementsUInt( PT_TRIANGLES ); break;
        case TRISTRIPS:  de = new DrawElementsUInt( PT_TRIANGLE_STRIP ); break;
        default:
          VL_TRAP()
          continue;
      }

      mGeometry->drawCalls()->push_back(de.get());
      de->setIndices( index_buffer.get() );
    }

    // generate new vertex attrib info and install data
    size_t tex_unit = 0;
    for( size_t ich=0; ich<mChannels.size(); ++ich )
    {
      // init data storage for this channel
      ref<ArrayAbstract> vert_attrib;
      float* ptr = NULL;
      float* ptr_end = NULL;
      switch(mChannels[ich]->mSource->dataSize())
      {
        case 1:
        {
          ref<ArrayFloat1> array_f1 = new ArrayFloat1;
          vert_attrib = array_f1;
          array_f1->resize( vert_set.size() );
          ptr = array_f1->begin();
          // debug
          ptr_end = ptr + vert_set.size() * 1;
          break;
        }
        case 2:
        {
          ref<ArrayFloat2> array_f2 = new ArrayFloat2;
          vert_attrib = array_f2;
          array_f2->resize( vert_set.size() );
          ptr = array_f2->at(0).ptr();
          // debug
          ptr_end = ptr + vert_set.size() * 2;
          break;
        }
        case 3:
        {
          ref<ArrayFloat3> array_f3 = new ArrayFloat3;
          vert_attrib = array_f3;
          array_f3->resize( vert_set.size() );
          ptr = array_f3->at(0).ptr();
          // debug
          ptr_end = ptr + vert_set.size() * 3;
          break;
        }
        case 4:
        {
          ref<ArrayFloat4> array_f4 = new ArrayFloat4;
          vert_attrib = array_f4;
          array_f4->resize( vert_set.size() );
          ptr = array_f4->at(0).ptr();
          // debug
          ptr_end = ptr + vert_set.size() * 4;
          break;
        }
        default:
          Log::warning( Say("LoadWriterCOLLADA: input '%s' skipped because parameter count is more than 4.\n") << getSemanticString(mChannels[ich]->mSemantic) );
          continue;
      }

      // install vertex attribute
      switch(mChannels[ich]->mSemantic)
      {
      case IS_POSITION: mGeometry->setVertexArray( vert_attrib.get() ); break;
      case IS_NORMAL:   mGeometry->setNormalArray( vert_attrib.get() ); break;
      case IS_COLOR:    mGeometry->setColorArray( vert_attrib.get() ); break;
      case IS_TEXCOORD: mGeometry->setTexCoordArray( tex_unit++, vert_attrib.get() ); break;
      default:
        Log::warning( Say("LoadWriterCOLLADA: input semantic '%s' not supported.\n") << getSemanticString(mChannels[ich]->mSemantic) );
        continue;
      }

      // fill the vertex attribute array
      for(std::set<DaeVert>::iterator it = vert_set.begin(); it != vert_set.end(); ++it)
      {
        const DaeVert& vert = *it;
        size_t idx = vert.mAttribIndex[ich];
        VL_CHECK(ptr + mChannels[ich]->mSource->dataSize()*vert.mIndex < ptr_end);
        mChannels[ich]->mSource->readData(idx, ptr + mChannels[ich]->mSource->dataSize()*vert.mIndex);
      }
    }

    // mic fixme:
    if (!mGeometry->normalArray())
      mGeometry->computeNormals();
  }
};
//-----------------------------------------------------------------------------
struct DaeMesh: public Object
{
  std::vector< ref<DaeInput> > mVertexInputs;
  std::vector< ref<DaePrimitive> > mPrimitives;
};
//-----------------------------------------------------------------------------
struct DaeNode: public Object
{
  DaeNode()
  {
    mTransform = new Transform;
  }

  ref<Transform> mTransform;
  std::vector< ref<DaeNode> > mChildren;
  std::vector< ref<DaeMesh> > mMesh;
  std::vector< ref<Actor> > mActors;
};
//-----------------------------------------------------------------------------
struct DaeSurface: public Object
{
  // mic fixme: for the moment we only support 2D images.
  ref<Image> mImage; // <init_from>
};
//-----------------------------------------------------------------------------
struct DaeSampler2D: public Object
{
  DaeSampler2D()
  {
    // default values if no tags are found.
    mMinFilter = TPF_LINEAR;
    mMagFilter = TPF_LINEAR;
    mWrapS     = TPW_REPEAT;
    mWrapT     = TPW_REPEAT;
  }

  ref<DaeSurface> mDaeSurface;    // <source>
  vl::ETexParamFilter mMinFilter; // <minfilter>
  vl::ETexParamFilter mMagFilter; // <magfilter>
  vl::ETexParamWrap   mWrapS;     // <wrap_s>
  vl::ETexParamWrap   mWrapT;     // <wrap_t>

  ref<Texture> mTexture; // actual VL texture implementing the sampler

  void prepareTexture2D()
  {
    if (mDaeSurface && mDaeSurface->mImage)
    {
      bool use_mipmaps = true;
      switch(mMinFilter)
      {
      case TPF_LINEAR:
      case TPF_NEAREST:
        use_mipmaps = false;
      }

      mTexture = new Texture;
      mTexture->prepareTexture2D( mDaeSurface->mImage.get(), TF_UNKNOWN, use_mipmaps, false);
      mTexture->getTexParameter()->setWrapS(mWrapS);
      mTexture->getTexParameter()->setWrapT(mWrapT);
      mTexture->getTexParameter()->setMinFilter(mMinFilter);
      mTexture->getTexParameter()->setMagFilter(mMagFilter);
    }
  }
};
//-----------------------------------------------------------------------------
struct DaeNewParam: public Object
{
  ref<DaeSampler2D> mDaeSampler2D;
  ref<DaeSurface> mDaeSurface;
  fvec4 mFloat4;
};
//-----------------------------------------------------------------------------
struct DaeTechniqueCOMMON: public Object
{
  DaeTechniqueCOMMON()
  {
    mMode = Unknown;

    mEmission     = fvec4(0, 0, 0, 1);
    mAmbient      = fvec4(0, 0, 0, 1);
    mDiffuse      = fvec4(1, 1, 1, 1);
    mSpecular     = fvec4(0, 0, 0, 1);
    mShininess    = 40;

    mReflective   = fvec4(1, 1, 1, 1);
    mReflectivity = 0;

    mTransparent  = fvec4(1, 1, 1, 1);
    mOpaqueMode   = Opaque_A_ONE;
    mTransparency = 1;

    mIndexOfRefraction = 0;
  }

  enum { Unknown, Blinn, Phong, Lambert } mMode;

  fvec4 mEmission;
  fvec4 mAmbient;
  fvec4 mDiffuse;
  fvec4 mSpecular;
  float mShininess;
  fvec4 mReflective;
  float mReflectivity;
  fvec4 mTransparent;
  float mTransparency;
  float mIndexOfRefraction;

  // mic fixme: for the moment we support the <texture> tag only in <diffuse>
  struct { ref<DaeSampler2D> mSampler; std::string mTexCoord; } mDiffuseTexture;

  enum { Opaque_A_ONE, Opaque_RGB_ZERO } mOpaqueMode;
};
//-----------------------------------------------------------------------------
struct DaeEffect: public Object
{
  std::vector<ref<DaeNewParam> > mNewParams;
  ref<DaeTechniqueCOMMON> mDaeTechniqueCOMMON;
};
//-----------------------------------------------------------------------------
struct DaeMaterial: public Object
{
  ref<DaeEffect> mDaeEffect;
};
//-----------------------------------------------------------------------------
ref<Effect> setupMaterial( DaeMaterial* mat )
{
  VL_CHECK(mat)
  VL_CHECK(mat->mDaeEffect)
  // VL_CHECK(mat->mDaeEffect->mDaeTechniqueCOMMON)

  ref<Effect> fx = new Effect;
  fx->shader()->enable(EN_LIGHTING);
  fx->shader()->setRenderState( new Light(0) ); // mic fixme: these should be all removed and add the ones present in the scene.
  fx->shader()->enable(EN_DEPTH_TEST);

  // very basic material setup
  if (mat->mDaeEffect->mDaeTechniqueCOMMON)
  {
    DaeTechniqueCOMMON* common_tech =mat->mDaeEffect->mDaeTechniqueCOMMON.get();

    // mic fixme: this vl::Material can be put in mDaeTechniqueCOMMON and shared among all the materials that use it.
    fx->shader()->gocMaterial()->setDiffuse( common_tech->mDiffuse );
    fx->shader()->gocMaterial()->setAmbient( common_tech->mAmbient );
    fx->shader()->gocMaterial()->setEmission( common_tech->mEmission );
    fx->shader()->gocMaterial()->setSpecular( common_tech->mSpecular );
    fx->shader()->gocMaterial()->setShininess( common_tech->mShininess );

    // mic fixme: 
    // for the moment we ignore things like <bind_vertex_input semantic="UVSET0" input_semantic="TEXCOORD" input_set="0"/>
    // instead we use the first texture with the first texture coordinate set installed.
    if ( common_tech->mDiffuseTexture.mSampler && common_tech->mDiffuseTexture.mSampler->mTexture )
    {
      fx->shader()->gocTextureSampler(0)->setTexture( common_tech->mDiffuseTexture.mSampler->mTexture.get() );
    }
  }
  else
  {
    Log::error("LoadWriterCOLLADA: technique or profile not supported.\n");
    fx->shader()->gocMaterial()->setDiffuse( vl::fuchsia );
  }

  return fx;
}
//-----------------------------------------------------------------------------
class COLLADALoader
{
public:
  COLLADALoader()
  {
    reset();

    // default material

    mDefaultFX = new Effect;
    mDefaultFX->setObjectName("<COLLADA_Default_Material>");
    mDefaultFX->shader()->enable(EN_LIGHTING);
    mDefaultFX->shader()->setRenderState( new Light(0) );
    mDefaultFX->shader()->gocMaterial()->setFlatColor( vl::fuchsia );
    mDefaultFX->shader()->gocPolygonMode()->set(PM_LINE, PM_LINE);
  }

  void reset()
  {
    mResources = new ResourceDatabase;
    mScene = NULL;
  }

  void parseInputs(DaePrimitive* dae_primitive, const domInputLocalOffset_Array& input_arr, const std::vector< ref<DaeInput> >& vertex_inputs)
  {
    dae_primitive->mIndexStride = 0;

    for(size_t iinp=0; iinp<input_arr.getCount(); ++iinp)
    {
      domInputLocalOffsetRef input = input_arr.get(iinp);

      // copy over VERTEX inputs with the current offset and set
      if ( getSemantic(input->getSemantic()) ==  IS_VERTEX )
      {
        VL_CHECK(!vertex_inputs.empty())
        for(size_t ivert=0; ivert<vertex_inputs.size(); ++ivert)
        {
          ref<DaeInput> dae_input = new DaeInput;
          dae_input->mSemantic = vertex_inputs[ivert]->mSemantic;
          dae_input->mSource   = vertex_inputs[ivert]->mSource;
          dae_input->mOffset   = (size_t)input->getOffset();
          dae_input->mSet      = (size_t)input->getSet();
          dae_primitive->mChannels.push_back(dae_input);

          VL_CHECK(dae_input->mSource);
          VL_CHECK(dae_input->mSemantic != IS_UNKNOWN);

          
          dae_primitive->mIndexStride = std::max(dae_primitive->mIndexStride, dae_input->mOffset);
        }
      }
      else
      {
          ref<DaeInput> dae_input = new DaeInput;
          dae_input->mSemantic = getSemantic(input->getSemantic());
          dae_input->mSource   = getSource( input->getSource().getElement() );
          dae_input->mOffset   = (size_t)input->getOffset();
          dae_input->mSet      = (size_t)input->getSet();

          // if the source is NULL getSource() has already issued an error.
          if (dae_input->mSource)
            dae_primitive->mChannels.push_back( dae_input );

          VL_CHECK(dae_input->mSource);
          VL_CHECK(dae_input->mSemantic != IS_UNKNOWN);

          dae_primitive->mIndexStride = std::max(dae_primitive->mIndexStride, dae_input->mOffset);
      }
    }
    
    dae_primitive->mIndexStride += 1;
  }

  ref<DaeMesh> parseGeometry(daeElement* geometry)
  {
    // try to reuse the geometry in the library
    std::map< daeElement*, ref<DaeMesh> >::iterator it = mMeshes.find( geometry );
    if (it != mMeshes.end())
      return it->second;

    if (!geometry->getChild("mesh"))
      return NULL;

    domMesh* mesh = static_cast<domMesh*>(geometry->getChild("mesh"));

    // add to dictionary
    ref<DaeMesh> dae_mesh = new DaeMesh;
    mMeshes[geometry] = dae_mesh;

    // vertices
    domVerticesRef vertices = mesh->getVertices();
    domInputLocal_Array input_array = vertices->getInput_array();
    for(size_t i=0; i<input_array.getCount(); ++i)
    {
      ref<DaeInput> dae_input = new DaeInput;

      dae_input->mSemantic = getSemantic(input_array[i]->getSemantic());
      if (dae_input->mSemantic == IS_UNKNOWN)
      {
        Log::error( Say("LoadWriterCOLLADA: the following semantic is unknown: %s\n") << input_array[i]->getSemantic() );
        continue;
      }

      dae_input->mSource = getSource( input_array[i]->getSource().getElement() );
      // if the source is NULL getSource() already issued an error.
      if (!dae_input->mSource)
        continue;

      dae_mesh->mVertexInputs.push_back(dae_input);
    }

    // --- --- ---- primitives ---- --- ---

    // NOTE: for the moment we generate one Geometry for each primitive but we should try to generate
    // one single set of vertex attribute array for each input semantic and recycle it if possible.
    // Unfortunately COLLADA makes this trivial task impossible to achieve.

    // --- ---- triangles ---- ---
    domTriangles_Array triangles_arr = mesh->getTriangles_array();
    for(size_t itri=0; itri< triangles_arr.getCount(); ++itri)
    {
      domTrianglesRef triangles = triangles_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::TRIANGLES;
      dae_primitive->mCount = (size_t)triangles->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = triangles->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      dae_primitive->mP.push_back( triangles->getP() );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = triangles->getMaterial() ? triangles->getMaterial() : NO_MATERIAL_SPECIFIED;
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    // --- ---- triangles fan ---- ---
    domTrifans_Array trifan_arr = mesh->getTrifans_array();
    for(size_t itri=0; itri< trifan_arr.getCount(); ++itri)
    {
      domTrifansRef trifan = trifan_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::TRIFANS;
      dae_primitive->mCount = (size_t)trifan->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = trifan->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<trifan->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( trifan->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = trifan->getMaterial() ? trifan->getMaterial() : NO_MATERIAL_SPECIFIED;

      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    // --- ---- triangle strip ---- ---
    domTristrips_Array tristrip_arr = mesh->getTristrips_array();
    for(size_t itri=0; itri< tristrip_arr.getCount(); ++itri)
    {
      domTristripsRef tristrip = tristrip_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::TRISTRIPS;
      dae_primitive->mCount = (size_t)tristrip->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = tristrip->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<tristrip->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( tristrip->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = tristrip->getMaterial() ? tristrip->getMaterial() : NO_MATERIAL_SPECIFIED;
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    // --- ---- polygons ---- ---
    domPolygons_Array polygon_arr = mesh->getPolygons_array();
    for(size_t itri=0; itri< polygon_arr.getCount(); ++itri)
    {
      domPolygonsRef polygon = polygon_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::POLYGONS;
      dae_primitive->mCount = (size_t)polygon->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = polygon->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<polygon->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( polygon->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = polygon->getMaterial() ? polygon->getMaterial() : NO_MATERIAL_SPECIFIED;
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    // --- ---- polylists ---- ---
    domPolylist_Array polylist_arr = mesh->getPolylist_array();
    for(size_t itri=0; itri< polylist_arr.getCount(); ++itri)
    {
      domPolylistRef polylist = polylist_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::POLYGONS;
      dae_primitive->mCount = (size_t)polylist->getVcount()->getValue().getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = polylist->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      size_t ip=0;
      for(size_t ivc=0; ivc<polylist->getVcount()->getValue().getCount(); ++ivc)
      {
        domPRef p = static_cast<domP*>(domP::create(mDAE).cast());
        VL_CHECK(p->typeID() == domP::ID());
        dae_primitive->mP.push_back( p );
        size_t vcount = (size_t)polylist->getVcount()->getValue()[ivc];
        p->getValue().setCount(vcount * dae_primitive->mIndexStride);
        for(size_t i=0; i<p->getValue().getCount(); ++i)
          p->getValue().set(i, polylist->getP()->getValue()[ip++]);
      }

      // --- ---- material ---- ---
      dae_primitive->mMaterial = polylist->getMaterial() ? polylist->getMaterial() : NO_MATERIAL_SPECIFIED;
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    // --- ---- linestrips ---- ---
    domLinestrips_Array linestrip_arr = mesh->getLinestrips_array();
    for(size_t itri=0; itri< linestrip_arr.getCount(); ++itri)
    {
      domLinestripsRef linestrip = linestrip_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::LINE_STRIP;
      dae_primitive->mCount = (size_t)linestrip->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = linestrip->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<linestrip->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( linestrip->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = linestrip->getMaterial() ? linestrip->getMaterial() : NO_MATERIAL_SPECIFIED;
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    // --- ---- lines ---- ---
    domLines_Array line_arr = mesh->getLines_array();
    for(size_t itri=0; itri< line_arr.getCount(); ++itri)
    {
      domLinesRef line = line_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::LINES;
      dae_primitive->mCount = (size_t)line->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = line->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      dae_primitive->mP.push_back( line->getP() );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = line->getMaterial() ? line->getMaterial() : NO_MATERIAL_SPECIFIED;
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    return dae_mesh;
  }

  DaeSource* getSource(daeElement* source_el)
  {
    std::map< daeElement*, ref<DaeSource> >::iterator it = mSources.find(source_el);
    if (it != mSources.end())
      return it->second.get();
    else
    {
      VL_CHECK(source_el->typeID() == domSource::ID())
      domSourceRef source = static_cast<domSource*>(source_el);

      domSource::domTechnique_commonRef tech_common = source->getTechnique_common(); VL_CHECK(tech_common)
      domAccessorRef accessor = tech_common->getAccessor();
      
      size_t mask = 0;
      // we support up to 32 parameters for a single accessor
      domParam_Array param_array = accessor->getParam_array();
      size_t attr_count = param_array.getCount() <= 32 ? param_array.getCount() : 32;
      for(size_t ipar=0; ipar<attr_count; ++ipar)
      {
        if (param_array[ipar]->getName() && strlen(param_array[ipar]->getName()))
          mask |= 1<<ipar;
      }

      ref<DaeSource> dae_source = new DaeSource;

      if (source->getFloat_array())
        dae_source->init(source->getFloat_array(), accessor->getCount(), accessor->getStride(), accessor->getOffset(), mask);
      else
      if (source->getInt_array())
        dae_source->init(source->getInt_array(), accessor->getCount(), accessor->getStride(), accessor->getOffset(), mask);
      else
      if (source->getBool_array())
        dae_source->init(source->getBool_array(), accessor->getCount(), accessor->getStride(), accessor->getOffset(), mask);
      else
      {
        Log::error("LoadWriterCOLLADA: no supported source data found. Only Float_array, Int_array and Bool_array are supported as source data.\n");
        return NULL;
      }

      // add to source library for quick access later
      mSources[source] = dae_source;

      return dae_source.get();
    }
  }

  void bindMaterials(DaeNode* dae_node, DaeMesh* dae_mesh, domBind_materialRef bind_material)
  {
    // map symbols to actual materials
    std::map< std::string, DaeMaterial* > material_map;

    if ( bind_material )
    {
      if (bind_material->getTechnique_common())
      {
        domInstance_material_Array& material_instances = bind_material->getTechnique_common()->getInstance_material_array();
        for(size_t i=0; i<material_instances.getCount(); ++i)
        {
          daeElement* material = material_instances[i]->getTarget().getElement();
          VL_CHECK(material)
          std::map< daeElement*, ref<DaeMaterial> >::iterator it = mMaterials.find( material );
          if (it != mMaterials.end())
          {
            material_map[ material_instances[i]->getSymbol() ] = it->second.get();
          }
          else
          {
            // mic fixme: error
            continue;
          }
        }
      }
      else
      {
        // mic fixme issue error
      }
    }

    // now we need to instance the material
    for(size_t iprim=0; iprim<dae_mesh->mPrimitives.size(); ++iprim)
    {
      ref<DaeMaterial> dae_material; 

      if (!dae_mesh->mPrimitives[iprim]->mMaterial.empty())
      {
        std::map< std::string, DaeMaterial* >::iterator it = material_map.find(  dae_mesh->mPrimitives[iprim]->mMaterial );
        if (it != material_map.end())
        {
          dae_material = it->second;
        }
        else
        {
          Log::warning( Say("LoadWriterCOLLADA: material symbol %s could not be resolved.\n") << dae_mesh->mPrimitives[iprim]->mMaterial );
        }
      }

      ref<Effect> fx = dae_material ? setupMaterial(dae_material.get()) : mDefaultFX;

      ref<Actor> actor = new Actor( dae_mesh->mPrimitives[iprim]->mGeometry.get(), fx.get(), dae_node->mTransform.get() );
      dae_node->mActors.push_back( actor );
    }
  }

  void parseNode(daeElement* el, DaeNode* parent)
  {
    if (el->typeID() == domNode::ID())
    {
      // --- --- --- parse this node --- --- ---

      // create new node and add it to the library
      ref<DaeNode> this_node = new DaeNode;
      mNodes.push_back(this_node);
      parent->mChildren.push_back( this_node );
      parent->mTransform->addChild( this_node->mTransform.get() );

      domNode* node = static_cast<domNode*>(el);

      // parse geometries
      domInstance_geometry_Array geometries = node->getInstance_geometry_array();
      for(size_t i=0; i<geometries.getCount(); ++i)
      {
        VL_CHECK(geometries[i]->getUrl().getElement()->typeID() == domGeometry::ID())
        daeElement* geometry = geometries[i]->getUrl().getElement();
        ref<DaeMesh> dae_mesh = parseGeometry(geometry);
        if (dae_mesh)
          this_node->mMesh.push_back(dae_mesh.get());
        
        // generate the Actors belonging to this node with their own material
        bindMaterials(this_node.get(), dae_mesh.get(), geometries[i]->getBind_material());
      }

      // note: transforms are post-multiplied in the order in which they are specified (as if they were sub-nodes)
      for(size_t ichild=0; ichild<node->getChildren().getCount(); ++ichild)
      {
        daeElement* child = node->getChildren()[ichild];
      
        if ( 0 == strcmp(child->getElementName(), "matrix") )
        {
          domMatrix* matrix = static_cast<domMatrix*>(child);
          mat4 local_matrix;
          for(int i=0; i<16; ++i)
            local_matrix.ptr()[i] = (Real)matrix->getValue().get(i);
          local_matrix.transpose();
          this_node->mTransform->postMultiply(local_matrix);
        }
        else
        if ( 0 == strcmp(child->getElementName(), "translate") )
        {
          domTranslate* tr = static_cast<domTranslate*>(child);
          mat4 m = mat4::getTranslation((Real)tr->getValue()[0], (Real)tr->getValue()[1], (Real)tr->getValue()[2]);
          this_node->mTransform->postMultiply( m );
        }
        else
        if ( 0 == strcmp(child->getElementName(), "rotate") )
        {
          domRotate* rot = static_cast<domRotate*>(child);
          mat4 m = mat4::getRotation((Real)rot->getValue()[3], (Real)rot->getValue()[0], (Real)rot->getValue()[1], (Real)rot->getValue()[2]);
          this_node->mTransform->postMultiply( m );
        }
        else
        if ( 0 == strcmp(child->getElementName(), "scale") )
        {
          domScale* sc = static_cast<domScale*>(child);
          mat4 m = mat4::getScaling((Real)sc->getValue()[0], (Real)sc->getValue()[1], (Real)sc->getValue()[2]);
          this_node->mTransform->postMultiply( m );
        }
        else
        if ( 0 == strcmp(child->getElementName(), "lookat") )
        {
          domLookat* lookat = static_cast<domLookat*>(child);
          vec3 eye ((Real)lookat->getValue()[0], (Real)lookat->getValue()[1], (Real)lookat->getValue()[2]);
          vec3 look((Real)lookat->getValue()[3], (Real)lookat->getValue()[4], (Real)lookat->getValue()[5]);
          vec3 up  ((Real)lookat->getValue()[6], (Real)lookat->getValue()[7], (Real)lookat->getValue()[8]);
          this_node->mTransform->preMultiply( mat4::getLookAt(eye, look, up).invert() );
        }
        else
        if ( 0 == strcmp(child->getElementName(), "skew") )
        {
          // mic fixme: support skew
          // domSkew* skew = static_cast<domSkew*>(child);
          Log::error("LoadWriterCOLLADA: <skew> transform not supported yet. Call me if you know how to compute it.\n");
        }
      }

      // --- --- --- parse children --- --- ---

      // parse instance nodes
      domInstance_node_Array nodes = node->getInstance_node_array();
      for(size_t i=0; i<nodes.getCount(); ++i)
      {
        daeElement* node = nodes[i]->getUrl().getElement();
        VL_CHECK(node->typeID() == domNode::ID())
        parseNode(node, this_node.get());
      }

      // parse proper children
      daeTArray< daeSmartRef<daeElement> > children = node->getChildren();
      for(size_t i=0; i<children.getCount(); ++i)
        parseNode(children[i], this_node.get());
    }
  }

  bool load(VirtualFile* file)
  {
    reset();

    mFilePath = file->path();

    // load COLLADA file as a string.
    std::vector<char> buffer;
    file->load(buffer);
    if (buffer.empty())
      return false;
    buffer.push_back(0);

    daeElement* root = mDAE.openFromMemory(file->path().toStdString(), (char*)&buffer[0]);
    if (!root)
    {
      Log::error( "LoadWriterCOLLADA: failed to open COLLADA document.\n" );
      return false;
    }

    parseImages(root->getDescendant("library_images"));

    parseEffects(root->getDescendant("library_effects"));

    parseMaterials(root->getDescendant("library_materials"));

    daeElement* visual_scene = root->getDescendant("visual_scene");
    if (!visual_scene)
    {
      Log::error( "LoadWriterCOLLADA: <visual_scene> not found!\n" );
      return false;
    }

    // --- parse the visual scene ---

    mScene = new DaeNode;
    daeTArray< daeSmartRef<daeElement> > children = visual_scene->getChildren();
    for(size_t i=0; i<children.getCount(); ++i)
      parseNode(children[i], mScene.get());

    // --- fill the resource database and final setup ---

    mScene->mTransform->computeWorldMatrixRecursive();

    // return the Actors
    for( size_t inode=0; inode<mNodes.size(); ++inode )
    {
      for(size_t i=0; i<mNodes[inode]->mActors.size(); ++i)
      {
        Actor* actor = mNodes[inode]->mActors[i].get();

        // add actor to the resources
        mResources->resources().push_back( actor );

        // *** flatten transform hierarchy ***
        if ( loadOptions()->flattenTransformHierarchy() )
          actor->transform()->removeFromParent();

        // *** check for transforms that require normal rescaling ***
        mat4 nmatrix = actor->transform()->worldMatrix().as3x3().invert().transpose();
        Real len_x = nmatrix.getX().length();
        Real len_y = nmatrix.getY().length();
        Real len_z = nmatrix.getZ().length();
        if ( fabs(len_x - 1) > 0.1 || fabs(len_y - 1) > 0.1 || fabs(len_z - 1) > 0.1 )
        {
          // Log::warning("Detected mesh with scaled transform: enabled normal renormalization.\n");
          if ( actor->effect()->shader()->isEnabled(vl::EN_LIGHTING) )
            actor->effect()->shader()->enable(vl::EN_NORMALIZE);
            // actor->effect()->shader()->enable(vl::EN_RESCALE_NORMAL);
        }
      }
    }

    if ( loadOptions()->flattenTransformHierarchy() )
      mScene->mTransform->eraseAllChildrenRecursive();
    else
      mResources->resources().push_back( mScene->mTransform );

    return true;
  }

  const ResourceDatabase* resources() const { return mResources.get(); }

  ResourceDatabase* resources() { return mResources.get(); }

  // --- options ---

  void setLoadOptions(const LoadWriterCOLLADA::LoadOptions* options) { mLoadOptions = options; }

  const LoadWriterCOLLADA::LoadOptions* loadOptions() const { return mLoadOptions; }

public:
  void loadImages(const domImage_Array& images)
  {
    for(size_t i=0; i<images.getCount(); ++i)
    {
      String complete_path = mFilePath.extractPath() + images[i]->getInit_from()->getValue().getOriginalURI();
      complete_path.normalizeSlashes();
      ref<Image> image = loadImage( complete_path );
      mImages[ images[i].cast() ] = image;
    }
  }

  void parseImages(daeElement* library)
  {
    if (!library)
      return;
    domLibrary_images* library_images = static_cast<domLibrary_images*>(library);
    const domImage_Array& images = library_images->getImage_array();
    loadImages(images);
  }

  void parseEffects(daeElement* library)
  {
    if (!library)
      return;
    domLibrary_effects* library_effects = static_cast<domLibrary_effects*>(library);
    const domEffect_Array& effects = library_effects->getEffect_array();
    for(size_t i=0; i<effects.getCount(); ++i)
    {
      domEffect* effect = effects[i].cast();

      ref<DaeEffect> dae_effect = new DaeEffect;

      mEffects[effect] = dae_effect;

      // load images
      loadImages(effect->getImage_array());

      const domFx_profile_abstract_Array& profiles = effect->getFx_profile_abstract_array();
      for(size_t i=0; i<profiles.getCount(); ++i)
      {
        if ( profiles[i]->typeID() == domProfile_COMMON::ID() )
        {
          domProfile_COMMON* common = static_cast<domProfile_COMMON*>(profiles[i].cast());

          // --- parse <newparam> ---
          for(size_t ipar=0; ipar<common->getNewparam_array().getCount(); ++ipar)
          {
            domCommon_newparam_typeRef newparam = common->getNewparam_array()[ipar];

            ref<DaeNewParam> dae_newparam = new DaeNewParam;
            dae_effect->mNewParams.push_back( dae_newparam );

            // insert in the map se can resolve references to <sampler2D> and <surface>
            mDaeNewParams[newparam.cast()] = dae_newparam;

            // --- <surface> ---
            // mic fixme: for the moment we support only single image 2D surfaces
            if (newparam->getSurface())
            {
              domFx_surface_commonRef surface = newparam->getSurface();

              if ( !surface->getFx_surface_init_common()->getInit_from_array().getCount() )
              {
                // mic fixme: issue error
                continue;
              }

              dae_newparam->mDaeSurface = new DaeSurface;
              daeElement* ref_image = surface->getFx_surface_init_common()->getInit_from_array()[0]->getValue().getElement();
              if (!ref_image)
              {
                // mic fixme report error
                continue;
              }

              std::map< daeElement*, ref<Image> >::iterator it = mImages.find( ref_image );
              if (it != mImages.end())
                dae_newparam->mDaeSurface->mImage = it->second.get();
              else
              {
                // mic fixme: issue error
                continue;
              }
            }

            // --- <sampler2D> ---
            if (newparam->getSampler2D())
            {
              domFx_sampler2D_commonRef sampler2D = newparam->getSampler2D();
              
              dae_newparam->mDaeSampler2D = new DaeSampler2D;

              // --- <source> ---
              daeSIDResolver sid_res( effect, sampler2D->getSource()->getValue() );
              domElement* surface_newparam = sid_res.getElement();
              // mic fixme issue error
              VL_CHECK(surface_newparam);

              std::map<domElement*, ref<DaeNewParam> >::iterator it = mDaeNewParams.find(surface_newparam);
              if ( it != mDaeNewParams.end() )
              {
                dae_newparam->mDaeSampler2D->mDaeSurface = it->second->mDaeSurface;
              }
              else
              {
                // mic fixme: issue error
                continue;
              }
              
              // --- <minfilter> ---
              if( sampler2D->getMinfilter() )
              {
                dae_newparam->mDaeSampler2D->mMinFilter = translateSampleFilter( sampler2D->getMinfilter()->getValue() );
              }
                

              // --- <magfilter> ---
              if( sampler2D->getMagfilter() )
              {
                dae_newparam->mDaeSampler2D->mMagFilter = translateSampleFilter( sampler2D->getMagfilter()->getValue() );
              }

              // --- <wrap_s> ---
              if (sampler2D->getWrap_s())
              {
                dae_newparam->mDaeSampler2D->mWrapS = translateWrapMode( sampler2D->getWrap_s()->getValue() );
              }

              // --- <wrap_t> ---
              if (sampler2D->getWrap_t())
              {
                dae_newparam->mDaeSampler2D->mWrapT = translateWrapMode( sampler2D->getWrap_t()->getValue() );
              }

              // prepare vl::Texture for creation with the given parameters
              dae_newparam->mDaeSampler2D->prepareTexture2D();
            }

            // --- <float>, <float2>, <float3>, <floa4> ---
            if ( newparam->getFloat() )
            {
              dae_newparam->mFloat4 = fvec4((float)newparam->getFloat()->getValue(), 0, 0, 0);
            }
            else
            if ( newparam->getFloat2() )
            {
              daeDouble* fptr = &newparam->getFloat2()->getValue()[0];
              dae_newparam->mFloat4 = fvec4((float)fptr[0], (float)fptr[1], 0, 0);
            }
            else
            if ( newparam->getFloat3() )
            {
              daeDouble* fptr = &newparam->getFloat3()->getValue()[0];
              dae_newparam->mFloat4 = fvec4((float)fptr[0], (float)fptr[1], (float)fptr[2], 0);
            }
            else
            if ( newparam->getFloat4() )
            {
              daeDouble* fptr = &newparam->getFloat4()->getValue()[0];
              dae_newparam->mFloat4 = fvec4((float)fptr[0], (float)fptr[1], (float)fptr[2], (float)fptr[3]);
            }
          }

          // --- parse technique ---
          if (common->getTechnique()->getBlinn())
          {
            domProfile_COMMON::domTechnique::domBlinnRef blinn = common->getTechnique()->getBlinn();

            dae_effect->mDaeTechniqueCOMMON = new DaeTechniqueCOMMON;
            parseColor( blinn->getEmission(), &dae_effect->mDaeTechniqueCOMMON->mEmission );
            parseColor( blinn->getAmbient(),  &dae_effect->mDaeTechniqueCOMMON->mAmbient );
            parseColor( blinn->getDiffuse(),  &dae_effect->mDaeTechniqueCOMMON->mDiffuse );
            parseColor( blinn->getSpecular(), &dae_effect->mDaeTechniqueCOMMON->mSpecular );
            if (blinn->getShininess())
              dae_effect->mDaeTechniqueCOMMON->mShininess = (float)blinn->getShininess()->getFloat()->getValue();

            // --- <diffuse><texture texture="..." texcoord="..." /></diffuse> ---
            if ( blinn->getDiffuse()->getTexture() )
            {
              // <texture texture="...">
              daeSIDResolver sid_res( common, blinn->getDiffuse()->getTexture()->getTexture() );
              domElement* sampler2D_newparam = sid_res.getElement();
              // mic fixme: issue error
              VL_CHECK(sampler2D_newparam)

              std::map<domElement*, ref<DaeNewParam> >::iterator it = mDaeNewParams.find(sampler2D_newparam);
              if ( it != mDaeNewParams.end() )
              {
                VL_CHECK(it->second->mDaeSampler2D)
                dae_effect->mDaeTechniqueCOMMON->mDiffuseTexture.mSampler = it->second->mDaeSampler2D;
              }
              else
              {
                // mic fixme: issue error
                continue;
              }

              // <texture texcoord="...">
              dae_effect->mDaeTechniqueCOMMON->mDiffuseTexture.mTexCoord = blinn->getDiffuse()->getTexture()->getTexcoord();
            }

            parseColor( blinn->getReflective(), &dae_effect->mDaeTechniqueCOMMON->mReflective );
            if (blinn->getReflectivity())
              dae_effect->mDaeTechniqueCOMMON->mReflectivity = (float)blinn->getReflectivity()->getFloat()->getValue();

            if (blinn->getTransparent())
            {
              if (blinn->getTransparent()->getColor())
              {
                domFloat4 tr_col = blinn->getTransparent()->getColor()->getValue();
                dae_effect->mDaeTechniqueCOMMON->mTransparent = fvec4((float)tr_col[0], (float)tr_col[1], (float)tr_col[2], (float)tr_col[3]);
              }
              dae_effect->mDaeTechniqueCOMMON->mOpaqueMode = blinn->getTransparent()->getOpaque() == FX_OPAQUE_ENUM_A_ONE ? DaeTechniqueCOMMON::Opaque_A_ONE : DaeTechniqueCOMMON::Opaque_RGB_ZERO;
            }
            if (blinn->getTransparency())
              dae_effect->mDaeTechniqueCOMMON->mTransparency = (float)blinn->getTransparency()->getFloat()->getValue();
          }
          else
          if (common->getTechnique()->getPhong())
          {
            domProfile_COMMON::domTechnique::domPhongRef phong = common->getTechnique()->getPhong();

            dae_effect->mDaeTechniqueCOMMON = new DaeTechniqueCOMMON;
            parseColor( phong->getEmission(), &dae_effect->mDaeTechniqueCOMMON->mEmission );
            parseColor( phong->getAmbient(),  &dae_effect->mDaeTechniqueCOMMON->mAmbient );
            parseColor( phong->getDiffuse(),  &dae_effect->mDaeTechniqueCOMMON->mDiffuse );
            parseColor( phong->getSpecular(), &dae_effect->mDaeTechniqueCOMMON->mSpecular );
            if (phong->getShininess())
              dae_effect->mDaeTechniqueCOMMON->mShininess = (float)phong->getShininess()->getFloat()->getValue();

            // --- <diffuse><texture texture="..." texcoord="..." /></diffuse> ---
            if ( phong->getDiffuse()->getTexture() )
            {
              // <texture texture="...">
              daeSIDResolver sid_res( common, phong->getDiffuse()->getTexture()->getTexture() );
              domElement* sampler2D_newparam = sid_res.getElement();
              // mic fixme: issue error
              VL_CHECK(sampler2D_newparam)

              std::map<domElement*, ref<DaeNewParam> >::iterator it = mDaeNewParams.find(sampler2D_newparam);
              if ( it != mDaeNewParams.end() )
              {
                VL_CHECK(it->second->mDaeSampler2D)
                dae_effect->mDaeTechniqueCOMMON->mDiffuseTexture.mSampler = it->second->mDaeSampler2D;
              }
              else
              {
                // mic fixme: issue error
                continue;
              }

              // <texture texcoord="...">
              dae_effect->mDaeTechniqueCOMMON->mDiffuseTexture.mTexCoord = phong->getDiffuse()->getTexture()->getTexcoord();
            }

            parseColor( phong->getReflective(), &dae_effect->mDaeTechniqueCOMMON->mReflective );
            if (phong->getReflectivity())
              dae_effect->mDaeTechniqueCOMMON->mReflectivity = (float)phong->getReflectivity()->getFloat()->getValue();

            if (phong->getTransparent())
            {
              if (phong->getTransparent()->getColor())
              {
                domFloat4 tr_col = phong->getTransparent()->getColor()->getValue();
                dae_effect->mDaeTechniqueCOMMON->mTransparent = fvec4((float)tr_col[0], (float)tr_col[1], (float)tr_col[2], (float)tr_col[3]);
              }
              dae_effect->mDaeTechniqueCOMMON->mOpaqueMode = phong->getTransparent()->getOpaque() == FX_OPAQUE_ENUM_A_ONE ? DaeTechniqueCOMMON::Opaque_A_ONE : DaeTechniqueCOMMON::Opaque_RGB_ZERO;
            }
            if (phong->getTransparency())
              dae_effect->mDaeTechniqueCOMMON->mTransparency = (float)phong->getTransparency()->getFloat()->getValue();
          }
          else
          if (common->getTechnique()->getLambert())
          {
            domProfile_COMMON::domTechnique::domLambertRef lambert = common->getTechnique()->getLambert();

            dae_effect->mDaeTechniqueCOMMON = new DaeTechniqueCOMMON;
            parseColor( lambert->getEmission(), &dae_effect->mDaeTechniqueCOMMON->mEmission );
            parseColor( lambert->getAmbient(),  &dae_effect->mDaeTechniqueCOMMON->mAmbient );
            parseColor( lambert->getDiffuse(),  &dae_effect->mDaeTechniqueCOMMON->mDiffuse );
            // parseColor( lambert->getSpecular(), &dae_effect->mDaeTechniqueCOMMON->mSpecular );
            //if (lambert->getShininess())
            //  dae_effect->mDaeTechniqueCOMMON->mShininess = (float)lambert->getShininess()->getFloat()->getValue();

            // --- <diffuse><texture texture="..." texcoord="..." /></diffuse> ---
            if ( lambert->getDiffuse()->getTexture() )
            {
              // <texture texture="...">
              daeSIDResolver sid_res( common, lambert->getDiffuse()->getTexture()->getTexture() );
              domElement* sampler2D_newparam = sid_res.getElement();
              // mic fixme: issue error
              VL_CHECK(sampler2D_newparam)

              std::map<domElement*, ref<DaeNewParam> >::iterator it = mDaeNewParams.find(sampler2D_newparam);
              if ( it != mDaeNewParams.end() )
              {
                VL_CHECK(it->second->mDaeSampler2D)
                dae_effect->mDaeTechniqueCOMMON->mDiffuseTexture.mSampler = it->second->mDaeSampler2D;
              }
              else
              {
                // mic fixme: issue error
                continue;
              }

              // <texture texcoord="...">
              dae_effect->mDaeTechniqueCOMMON->mDiffuseTexture.mTexCoord = lambert->getDiffuse()->getTexture()->getTexcoord();
            }

            parseColor( lambert->getReflective(), &dae_effect->mDaeTechniqueCOMMON->mReflective );
            if (lambert->getReflectivity())
              dae_effect->mDaeTechniqueCOMMON->mReflectivity = (float)lambert->getReflectivity()->getFloat()->getValue();

            if (lambert->getTransparent())
            {
              if (lambert->getTransparent()->getColor())
              {
                domFloat4 tr_col = lambert->getTransparent()->getColor()->getValue();
                dae_effect->mDaeTechniqueCOMMON->mTransparent = fvec4((float)tr_col[0], (float)tr_col[1], (float)tr_col[2], (float)tr_col[3]);
              }
              dae_effect->mDaeTechniqueCOMMON->mOpaqueMode = lambert->getTransparent()->getOpaque() == FX_OPAQUE_ENUM_A_ONE ? DaeTechniqueCOMMON::Opaque_A_ONE : DaeTechniqueCOMMON::Opaque_RGB_ZERO;
            }
            if (lambert->getTransparency())
              dae_effect->mDaeTechniqueCOMMON->mTransparency = (float)lambert->getTransparency()->getFloat()->getValue();
          }
        }
      }
    }
  }

  void parseMaterials(daeElement* library)
  {
    if (!library)
      return;
    domLibrary_materials* library_materials = static_cast<domLibrary_materials*>(library);
    const domMaterial_Array& materials = library_materials->getMaterial_array();
    for(size_t i=0; i<materials.getCount(); ++i)
    {
      domElement* effect = materials[i]->getInstance_effect()->getUrl().getElement();
      if (!effect)
      {
        // mic fixme: issue warning
        continue;
      }

      std::map< daeElement*, ref<DaeEffect> >::iterator it = mEffects.find(effect);
      if (it != mEffects.end())
      {
        domMaterial* material = materials[i].cast();
        ref<DaeMaterial> dae_material = new DaeMaterial;
        dae_material->mDaeEffect = it->second;
        mMaterials[ material ] = dae_material;
      }
      else
      {
        // mic fixme: issue warning
        continue;
      }
    }
  }

protected:
  const LoadWriterCOLLADA::LoadOptions* mLoadOptions;

protected:
  ref<ResourceDatabase> mResources;
  std::map< daeElement*, ref<DaeMaterial> > mMaterials;
  std::map< daeElement*, ref<DaeEffect> > mEffects;
  std::map< daeElement*, ref<DaeMesh> > mMeshes; // daeElement* -> <geometry>
  std::vector< ref<DaeNode> > mNodes;
  std::map< daeElement*, ref<DaeSource> > mSources; // daeElement* -> <source>
  std::map< daeElement*, ref<Image> > mImages;
  std::map< daeElement*, ref<DaeNewParam> > mDaeNewParams;
  ref<Effect> mDefaultFX;
  ref<DaeNode> mScene;
  DAE mDAE;
  String mFilePath;
};
//-----------------------------------------------------------------------------
ref<ResourceDatabase> LoadWriterCOLLADA::load(const String& path, const LoadOptions* options)
{
  ref<VirtualFile> file = defFileSystem()->locateFile(path);

  if (file)
    return load( file.get(), options );
  else
  {
    Log::error( Say("Could not locate '%s'.\n") << path );
    return NULL;
  }
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> LoadWriterCOLLADA::load(VirtualFile* file, const LoadOptions* options)
{
  COLLADALoader loader;
  loader.setLoadOptions(options);
  loader.load(file);
  return loader.resources();
}
//-----------------------------------------------------------------------------
