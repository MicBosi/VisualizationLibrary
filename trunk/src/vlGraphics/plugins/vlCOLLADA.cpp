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
#include <strstream>
#include <set>
#include <dae.h>
#include <dom.h>
#include <dom/domCOLLADA.h>

using namespace vl;

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
struct DaeVert
{
  // mic fixme: comment or configure this.
  static const int MAX_ATTRIBS = 12;

  DaeVert()
  {
    memset(mAttribIndex, 0xFF, sizeof(mAttribIndex));
    mIndex = -1;
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
EInputSemantic getSemantic(const char* semantic)
{
  if(strcmp(semantic, "BINORMAL") == 0)
    return IS_BINORMAL;
  else if(strcmp(semantic, "COLOR") == 0)
    return IS_COLOR;
  else if(strcmp(semantic, "CONTINUITY") == 0)
    return IS_CONTINUITY;
  else if(strcmp(semantic, "IMAGE") == 0)
    return IS_IMAGE;
  else if(strcmp(semantic, "INPUT") == 0)
    return IS_INPUT;
  else if(strcmp(semantic, "IN_TANGENT") == 0)
    return IS_IN_TANGENT;
  else if(strcmp(semantic, "INTERPOLATION") == 0)
    return IS_INTERPOLATION;
  else if(strcmp(semantic, "INV_BIND_MATRIX") == 0)
    return IS_INV_BIND_MATRIX;
  else if(strcmp(semantic, "JOINT") == 0)
    return IS_JOINT;
  else if(strcmp(semantic, "LINEAR_STEPS") == 0)
    return IS_LINEAR_STEPS;
  else if(strcmp(semantic, "MORPHS_TARGET") == 0)
    return IS_MORPHS_TARGET;
  else if(strcmp(semantic, "MORPH_WEIGHT") == 0)
    return IS_MORPH_WEIGHT;
  else if(strcmp(semantic, "NORMAL") == 0)
    return IS_NORMAL;
  else if(strcmp(semantic, "OUTPUT") == 0)
    return IS_OUTPUT;
  else if(strcmp(semantic, "OUT_TANGENT") == 0)
    return IS_OUT_TANGENT;
  else if(strcmp(semantic, "POSITION") == 0)
    return IS_POSITION;
  else if(strcmp(semantic, "TANGENT") == 0)
    return IS_TANGENT;
  else if(strcmp(semantic, "TEXBINORMAL") == 0)
    return IS_TEXBINORMAL;
  else if(strcmp(semantic, "TEXCOORD") == 0)
    return IS_TEXCOORD;
  else if(strcmp(semantic, "TEXTANGENT") == 0)
    return IS_TEXTANGENT;
  else if(strcmp(semantic, "UV") == 0)
    return IS_UV;
  else if(strcmp(semantic, "VERTEX") == 0)
    return IS_VERTEX;
  else if(strcmp(semantic, "WEIGHT") == 0)
    return IS_WEIGHT;
  else
  {
    VL_TRAP()
    return IS_UNKNOWN;
  }
}
//-----------------------------------------------------------------------------
// mic fixme: support sources of other types: int_array, name_array, bool_array, idref_array
class DaeSource: public Object
{
public:
  DaeSource()
  {
    mRawSource    = NULL;
    mFieldsMask = 0;
    mStride     = 0;
    mOffset     = 0;
    mCount      = 0;
    mDataSize   = 0;
  }

  //! Initializes an accessor. An accessor can read only up to 32 floats.
  void init(domFloat_arrayRef data_src, domUint count, domUint stride, domUint offset, size_t fields_mask)
  {
    mRawSource  = data_src;
    mCount      = (size_t)count;
    mStride     = (size_t)stride;
    mOffset     = (size_t)offset;
    mFieldsMask = fields_mask;

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

    if ( !((n < mCount) || (read_pos < mRawSource->getValue().getCount() - mDataSize)) )
    {
      Log::error("DaeSource::readData() out of bounds!\n");
      VL_TRAP();
    }
    else
    {
      size_t pos = 0;
      for(size_t i=0; i<32 && i<mStride; ++i)
        if (mFieldsMask & (1<<i))
          output[pos++] = (float)mRawSource->getValue()[read_pos+i];
    }
  }

  //! The number of elements in the source.
  size_t count() const { return mCount; }

  //! The number of elements written by readData().
  size_t dataSize() const { return mDataSize; }

  //! Returns the raw data source
  domFloat_arrayRef rawSource() { return mRawSource; }

protected:
  size_t mFieldsMask;
  size_t mDataSize;
  domFloat_arrayRef mRawSource;
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
  domUint mOffset;
  domUint mSet;
};
//-----------------------------------------------------------------------------
struct DaeMaterial: public Object
{
  // mic fixme:

  ref<Effect> mEffect;
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

  ref<DaeMaterial> mMaterial;
  std::vector< ref<DaeInput> > mChannels;
  size_t mCount;
  std::vector<domPRef> mP;
  size_t mIndexStride;
  ref<Geometry> mGeometry;

  void generateGeometry()
  {
    VL_CHECK(mIndexStride);

    mGeometry = new Geometry;

    // mic fixme: we should do it directly in the DrawElementsUInt
    std::vector<unsigned int> index_buffer;
    index_buffer.reserve(1000);

    // generate index buffers & DrawElements
    std::set<DaeVert> vert_set;
    for(size_t ip=0; ip<mP.size(); ++ip)
    {
      index_buffer.clear();

      for(size_t ivert=0; ivert<mP[ip]->getValue().getCount(); ivert+=mIndexStride)
      {
        DaeVert vert;

        // fill vertex info
        for(size_t ichannel=0; ichannel<mChannels.size(); ++ichannel)
          vert.mAttribIndex[ichannel] = mP[ip]->getValue()[ivert + mChannels[ichannel]->mOffset];

        // retrieve/insert the vertex
        std::set<DaeVert>::iterator it = vert_set.find(vert);
        if (it == vert_set.end())
        {
          vert.mIndex = vert_set.size();
          vert_set.insert(vert);
        }
        else
          vert.mIndex = it->mIndex;
        
        // this is the actual index
        index_buffer.push_back( vert.mIndex );
      }

      // fill the DrawElementsUInt
      ref<DrawElementsUInt> de;
      switch(mType)
      {
        case LINES: de = new DrawElementsUInt( PT_LINES ); break;
        case LINE_STRIP: de = new DrawElementsUInt( PT_LINE_STRIP ); break;
        case POLYGONS: de = new DrawElementsUInt( PT_POLYGON ); break;
        case TRIFANS: de = new DrawElementsUInt( PT_TRIANGLE_FAN ); break;
        case TRIANGLES: de = new DrawElementsUInt( PT_TRIANGLES ); VL_CHECK(mCount*3 == mP[ip]->getValue().getCount() / mIndexStride); break;
        case TRISTRIPS: de = new DrawElementsUInt( PT_TRIANGLE_STRIP ); break;
        default:
          // mic fixme: issue warning
          VL_TRAP()
          continue;
      }
      de->indices()->resize( index_buffer.size() );
      VL_CHECK(de->indices()->bytesUsed() == sizeof(index_buffer[0])*index_buffer.size());
      memcpy(de->indices()->ptr(), &index_buffer[0], sizeof(index_buffer[0])*index_buffer.size());
      mGeometry->drawCalls()->push_back(de.get());
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
          ptr_end = ptr + vert_set.size() * 1;
          memset(ptr, 0xFF, array_f1->bytesUsed());
          break;
        }
        case 2:
        {
          ref<ArrayFloat2> array_f2 = new ArrayFloat2;
          vert_attrib = array_f2;
          array_f2->resize( vert_set.size() );
          ptr = array_f2->at(0).ptr();
          ptr_end = ptr + vert_set.size() * 2;
          memset(ptr, 0xFF, array_f2->bytesUsed());
          break;
        }
        case 3:
        {
          ref<ArrayFloat3> array_f3 = new ArrayFloat3;
          vert_attrib = array_f3;
          array_f3->resize( vert_set.size() );
          ptr = array_f3->at(0).ptr();
          ptr_end = ptr + vert_set.size() * 3;
          memset(ptr, 0xFF, array_f3->bytesUsed());
          break;
        }
        case 4:
        {
          ref<ArrayFloat4> array_f4 = new ArrayFloat4;
          vert_attrib = array_f4;
          array_f4->resize( vert_set.size() );
          ptr = array_f4->at(0).ptr();
          ptr_end = ptr + vert_set.size() * 4;
          memset(ptr, 0xFF, array_f4->bytesUsed());
          break;
        }
        default:
          // mic fixme: issue error
          VL_TRAP();
          continue;
      }

      // install vertex attribute array
      switch(mChannels[ich]->mSemantic)
      {
        // mic fixme: support tangent, binormal and uv at least.
      case IS_POSITION: mGeometry->setVertexArray( vert_attrib.get() ); break;
      case IS_NORMAL: mGeometry->setNormalArray( vert_attrib.get() ); break;
      case IS_COLOR: mGeometry->setColorArray( vert_attrib.get() ); break;
      case IS_TEXCOORD: mGeometry->setTexCoordArray( tex_unit++, vert_attrib.get() ); break;
      default:
        // mic fixme: issue warning
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

      // mic fixme: ifdef-out and remove memset()s prima
      // debug: check that all the buffer has been filled.
      for(float* pos=ptr; pos<ptr_end; ++pos)
      {
        unsigned int FF = 0xFFFFFFFF;
        VL_CHECK( memcmp(&FF, pos, sizeof(FF)) != 0 );
      }
    }

    // mGeometry->computeNormals();
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

  void generateActors()
  {
    for(size_t imesh=0; imesh<mMesh.size(); ++imesh)
    {
      for(size_t iprim=0; iprim<mMesh[imesh]->mPrimitives.size(); ++iprim)
      {
        ref<DaeMaterial> dae_material = mMesh[imesh]->mPrimitives[iprim]->mMaterial;
        VL_CHECK(dae_material->mEffect)

        ref<Actor> actor = new Actor( mMesh[imesh]->mPrimitives[iprim]->mGeometry.get(), dae_material->mEffect.get(), mTransform.get() );
        mActors.push_back( actor );
      }
    }
  }

};
//-----------------------------------------------------------------------------
class COLLADALoader
{
public:
  COLLADALoader()
  {
    reset();

    // default material

    // parse material
    mDefaultMaterial = new DaeMaterial;
    mDefaultMaterial->mEffect = new Effect;
    mDefaultMaterial->mEffect->shader()->enable(EN_DEPTH_TEST);
    mDefaultMaterial->mEffect->shader()->enable(EN_LIGHTING);
    mDefaultMaterial->mEffect->shader()->gocLightModel()->setTwoSide(true);
    // mDefaultMaterial->mEffect->shader()->gocPolygonMode()->set(PM_LINE, PM_LINE);
    mDefaultMaterial->mEffect->shader()->setRenderState( new Light(0) );
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
        // mic fixme: issue warning
        VL_CHECK(!vertex_inputs.empty())
        for(size_t ivert=0; ivert<vertex_inputs.size(); ++ivert)
        {
          ref<DaeInput> dae_input = new DaeInput;
          dae_input->mSemantic = vertex_inputs[ivert]->mSemantic;
          dae_input->mSource   = vertex_inputs[ivert]->mSource;
          dae_input->mOffset   = input->getOffset();
          dae_input->mSet      = input->getSet();
          dae_primitive->mChannels.push_back(dae_input);
          // mic fixme: check error
          VL_CHECK(dae_input->mSource);
          VL_CHECK(dae_input->mSemantic != IS_UNKNOWN);

          dae_primitive->mIndexStride = dae_primitive->mIndexStride > dae_input->mOffset ? dae_primitive->mIndexStride : dae_input->mOffset;
        }
      }
      else
      {
          ref<DaeInput> dae_input = new DaeInput;
          dae_input->mSemantic = getSemantic(input->getSemantic());
          dae_input->mSource   = getSource( input->getSource().getElement() );
          dae_input->mOffset   = input->getOffset();
          dae_input->mSet      = input->getSet();
          dae_primitive->mChannels.push_back( dae_input );
          // mic fixme: check error
          VL_CHECK(dae_input->mSource);
          VL_CHECK(dae_input->mSemantic != IS_UNKNOWN);

          dae_primitive->mIndexStride = dae_primitive->mIndexStride > dae_input->mOffset ? dae_primitive->mIndexStride : dae_input->mOffset;
      }
    }
    
    dae_primitive->mIndexStride += 1;
  }

  ref<DaeMesh> parseGeometry(daeElement* geometry)
  {
    // mic fixme: issue warning
    // we need IDs
    VL_CHECK(geometry->getID())
    if (!geometry->getID())
      return NULL;

    // try to reuse the geometry in the library
    std::map< std::string, ref<DaeMesh> >::iterator it = mMeshes.find( geometry->getID() );
    if (it != mMeshes.end())
      return it->second;

    if (!geometry->getChild("mesh"))
      return NULL;

    domMesh* mesh = static_cast<domMesh*>(geometry->getChild("mesh"));

    // add to dictionary
    ref<DaeMesh> dae_mesh = new DaeMesh;
    mMeshes[geometry->getID()] = dae_mesh;

    // vertices
    domVerticesRef vertices = mesh->getVertices();
    domInputLocal_Array input_array = vertices->getInput_array();
    for(size_t i=0; i<input_array.getCount(); ++i)
    {
      ref<DaeInput> dae_input = new DaeInput;

      dae_input->mSemantic = getSemantic(input_array[i]->getSemantic());
      if (dae_input->mSemantic == IS_UNKNOWN)
      {
        // mic fixme: issue warning
        VL_TRAP()
        continue;
      }

      dae_input->mSource = getSource( input_array[i]->getSource().getElement() );
      if (!dae_input->mSource)
      {
        // mic fixme: issue warning
        VL_TRAP()
        continue;
      }

      dae_mesh->mVertexInputs.push_back(dae_input);
    }

    // --- --- ---- primitives ---- --- ---

    // mic fixme: for the moment we generate one Geometry for each primitive but we should try to generate
    // one single set of vertex attribute array for each input semantic and recycle it.

    // --- ---- triangles ---- ---
    domTriangles_Array triangles_arr = mesh->getTriangles_array();
    for(size_t itri=0; itri< triangles_arr.getCount(); ++itri)
    {
      domTrianglesRef triangles = triangles_arr.get(itri);

      ref<DaePrimitive> dae_primitive = new DaePrimitive;
      dae_mesh->mPrimitives.push_back(dae_primitive);
      dae_primitive->mType = DaePrimitive::TRIANGLES;
      dae_primitive->mCount = triangles->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = triangles->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      dae_primitive->mP.push_back( triangles->getP() );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( triangles->getMaterial() );
      
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
      dae_primitive->mCount = trifan->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = trifan->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<trifan->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( trifan->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( trifan->getMaterial() );

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
      dae_primitive->mCount = tristrip->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = tristrip->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<tristrip->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( tristrip->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( tristrip->getMaterial() );
      
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
      dae_primitive->mCount = polygon->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = polygon->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<polygon->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( polygon->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( polygon->getMaterial() );
      
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
      dae_primitive->mCount = polylist->getVcount()->getValue().getCount();

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
        size_t vcount = polylist->getVcount()->getValue()[ivc];
        p->getValue().setCount(vcount * dae_primitive->mIndexStride);
        for(size_t i=0; i<p->getValue().getCount(); ++i)
          p->getValue().set(i, polylist->getP()->getValue()[ip++]);
      }

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( polylist->getMaterial() );
      
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
      dae_primitive->mCount = linestrip->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = linestrip->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      for(size_t ip=0; ip<linestrip->getP_array().getCount(); ++ip)
        dae_primitive->mP.push_back( linestrip->getP_array().get(ip) );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( linestrip->getMaterial() );
      
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
      dae_primitive->mCount = line->getCount();

      // --- input ---
      domInputLocalOffset_Array input_arr = line->getInput_array();
      parseInputs(dae_primitive.get(), input_arr, dae_mesh->mVertexInputs);

      // --- ---- p ---- ---
      dae_primitive->mP.push_back( line->getP() );

      // --- ---- material ---- ---
      dae_primitive->mMaterial = getMaterial( line->getMaterial() );
      
      // --- ---- generates the geometry ---- ---
      dae_primitive->generateGeometry();
    }

    return dae_mesh;
  }

  DaeSource* getSource(daeElement* source_el)
  {
    std::map< std::string, ref<DaeSource> >::iterator it = mSources.find(source_el->getID());
    if (it != mSources.end())
      return it->second.get();
    else
    {
      VL_CHECK(source_el->typeID() == domSource::ID())
      domSourceRef source = static_cast<domSource*>(source_el);

      // mic fixme: issue warning
      if(!source->getID() || !strlen(source->getID()))
        return NULL;

      domSource::domTechnique_commonRef tech_common = source->getTechnique_common(); VL_CHECK(tech_common)
      domAccessorRef accessor = tech_common->getAccessor();
      
      size_t mask = 0;
      // we support only up to 32 parameters for a single accessor
      domParam_Array param_array = accessor->getParam_array();
      size_t attr_count = param_array.getCount() <= 32 ? param_array.getCount() : 32;
      for(size_t ipar=0; ipar<attr_count; ++ipar)
      {
        if (param_array[ipar]->getName() && strlen(param_array[ipar]->getName()))
          mask |= 1<<ipar;
      }

      ref<DaeSource> dae_source = new DaeSource;
      domFloat_arrayRef float_array = source->getFloat_array(); VL_CHECK(float_array); // mic fixme: support other source types.
      dae_source->init(float_array, accessor->getCount(), accessor->getStride(), accessor->getOffset(), mask);
      
      // add to source library for quick access later
      mSources[source->getID()] = dae_source;

      return dae_source.get();
    }
  }

  DaeMaterial* getMaterial(const char* name)
  {
    if (!name)
    {
      // mic fixme: warning
      return mDefaultMaterial.get();
    }

    std::map< std::string, ref<DaeMaterial> >::iterator it = mMaterials.find(name);
    if(it != mMaterials.end())
      return it->second.get();
    {
      // mic fixme: todo

      // parse material
      ref<DaeMaterial> material = new DaeMaterial;

      // mic fixme: do we really have to lookup materials by their name instead of ID???
      // mic fixme: generate from the material library
      material->mEffect = new Effect;
      material->mEffect->shader()->enable(EN_DEPTH_TEST);
      material->mEffect->shader()->enable(EN_LIGHTING);
      material->mEffect->shader()->gocLightModel()->setTwoSide(true);
      // material->mEffect->shader()->gocPolygonMode()->set(PM_LINE, PM_LINE);
      material->mEffect->shader()->setRenderState( new Light(0) );

      // insert new material
      mMaterials[name] = material;

      // return newly created material
      return material.get();
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
      }

      // generate the Actors belonging to this node
      this_node->generateActors();

      // mic fixme: support skew

      // note: transforms are accumulated in the order in which they are specified
      for(size_t transf=0; transf<node->getChildren().getCount(); ++transf)
      {
        daeElement* tranf_el = node->getChildren()[transf];
      
        if ( 0 == strcmp(tranf_el->getElementName(), "matrix") )
        {
          domMatrix* matrix = static_cast<domMatrix*>(tranf_el);
          fmat4 local_matrix;
          for(int i=0; i<16; ++i)
            local_matrix.ptr()[i] = (float)matrix->getValue().get(i);
          local_matrix.transpose();
          this_node->mTransform->preMultiply(local_matrix);
        }
        else
        if ( 0 == strcmp(tranf_el->getElementName(), "translate") )
        {
          domTranslate* tr = static_cast<domTranslate*>(tranf_el);
          this_node->mTransform->translate((Real)tr->getValue()[0], (Real)tr->getValue()[1], (Real)tr->getValue()[2]);
        }
        else
        if ( 0 == strcmp(tranf_el->getElementName(), "rotate") )
        {
          domRotate* rot = static_cast<domRotate*>(tranf_el);
          this_node->mTransform->rotate((Real)rot->getValue()[3], (Real)rot->getValue()[0], (Real)rot->getValue()[1], (Real)rot->getValue()[2]);
        }
        else
        if ( 0 == strcmp(tranf_el->getElementName(), "scale") )
        {
          domScale* sc = static_cast<domScale*>(tranf_el);
          this_node->mTransform->scale((Real)sc->getValue()[0], (Real)sc->getValue()[1], (Real)sc->getValue()[2]);
        }
        else
        if ( 0 == strcmp(tranf_el->getElementName(), "lookat") )
        {
          domLookat* lookat = static_cast<domLookat*>(tranf_el);
          vec3 eye ((Real)lookat->getValue()[0], (Real)lookat->getValue()[1], (Real)lookat->getValue()[2]);
          vec3 look((Real)lookat->getValue()[3], (Real)lookat->getValue()[4], (Real)lookat->getValue()[5]);
          vec3 up  ((Real)lookat->getValue()[6], (Real)lookat->getValue()[7], (Real)lookat->getValue()[8]);
          this_node->mTransform->preMultiply( mat4::getLookAt(eye, look, up).invert() );
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

  bool loadCOLLADA(VirtualFile* file)
  {
    reset();

    std::vector<char> buffer;
    file->load(buffer);
    if (buffer.empty())
      return false;
    buffer.push_back(0);
    
    daeElement* root = mDAE.openFromMemory(file->path().toStdString(), (char*)&buffer[0]);
    // mic fixme: issue warning
    if (!root)
      return false;

    daeElement* visual_scene = root->getDescendant("visual_scene");
    // mic fixme: issue warning
    if (!visual_scene)
      return false;

    // parse children and build the visual scene tree
    mScene = new DaeNode;
    daeTArray< daeSmartRef<daeElement> > children = visual_scene->getChildren();
    for(size_t i=0; i<children.getCount(); ++i)
      parseNode(children[i], mScene.get());

    mResources->resources().push_back( mScene->mTransform );
    mScene->mTransform->computeWorldMatrixRecursive();

    // return the Actors
    for( size_t inode=0; inode<mNodes.size(); ++inode )
    {
      for(size_t i=0; i<mNodes[inode]->mActors.size(); ++i)
      {
        mResources->resources().push_back( mNodes[inode]->mActors[i].get() );

        // mic fixme: ifdef-out
        #if 1
          printf("---\n");
          mat4 m = mNodes[inode]->mActors[i]->transform()->worldMatrix();
          // m = mat4::getTranslation(10,20,30);
          printf("%f %f %f %f\n", m.e(0,0), m.e(0,1), m.e(0,2), m.e(0,3) );
          printf("%f %f %f %f\n", m.e(1,0), m.e(1,1), m.e(1,2), m.e(1,3) );
          printf("%f %f %f %f\n", m.e(2,0), m.e(2,1), m.e(2,2), m.e(2,3) );
          printf("%f %f %f %f\n", m.e(3,0), m.e(3,1), m.e(3,2), m.e(3,3) );        
        #endif
      }
    }

    return true;
  }

  ResourceDatabase* resources() { return mResources.get(); }
  
  const ResourceDatabase* resources() const { return mResources.get(); }

protected:
  ref<ResourceDatabase> mResources;
  std::map< std::string, ref<DaeMaterial> > mMaterials;
  std::map< std::string, ref<DaeMesh> > mMeshes;
  std::vector< ref<DaeNode> > mNodes;
  std::map< std::string, ref<DaeSource> > mSources;
  ref<DaeMaterial> mDefaultMaterial;
  ref<DaeNode> mScene;
  DAE mDAE;
};
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadCOLLADA(const String& path)
{
  ref<VirtualFile> file = defFileSystem()->locateFile(path);

  if (file)
    return loadCOLLADA( file.get() );
  else
  {
    Log::error( Say("Could not locate '%s'.\n") << path );
    return NULL;
  }
}
//-----------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadCOLLADA(VirtualFile* file)
{
  COLLADALoader loader;
  loader.loadCOLLADA(file);
  return loader.resources();
}
//-----------------------------------------------------------------------------
