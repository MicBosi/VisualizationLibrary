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

#ifndef LoadWriterVLX_INCLUDE_ONCE
#define LoadWriterVLX_INCLUDE_ONCE

#include <vlCore/VLX_Tools.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/Shader.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/ClipPlane.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlGraphics/DrawElements.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <vlGraphics/DrawArrays.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/DistanceLODEvaluator.hpp>
#include <vlGraphics/PixelLODEvaluator.hpp>
#include <vlGraphics/DepthSortCallback.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/DiskFile.hpp>
#include <sstream>

#define VLX_IMPORT_CHECK_RETURN(Condition, Obj)                                                        \
  if (!(Condition))                                                                                    \
  {                                                                                                    \
    signalImportError( Say("Line %n : condition failed : %s\n") << (Obj).lineNumber() << #Condition ); \
    return;                                                                                            \
  }

#define VLX_IMPORT_CHECK_RETURN_NULL(Condition, Obj)                                                   \
  if (!(Condition))                                                                                    \
  {                                                                                                    \
    signalImportError( Say("Line %n : condition failed : %s\n") << (Obj).lineNumber() << #Condition ); \
    return NULL;                                                                                       \
  }

namespace vl
{
  #define VL_SERIALIZER_VERSION 100
  #define VLX_UNKNOWN_OBJECT "<VLX_UNKNOWN_OBJECT>"

  class VLX_Registry;

  //---------------------------------------------------------------------------
  // EXPORT TOOLS
  //---------------------------------------------------------------------------

  inline VLX_Value toValue(const std::vector<int>& vec)
  {
    VLX_Value value;
    value.setArray( new VLX_ArrayInteger );
    value.getArrayInteger()->value().resize( vec.size() );
    if (vec.size())
      value.getArrayInteger()->copyFrom(&vec[0]);
    return value;
  }

  inline VLX_Value toIdentifier(const std::string& str) { return VLX_Value(str.c_str(), VLX_Value::Identifier); }

  inline VLX_Value toUID(const std::string& str)        { return VLX_Value(str.c_str(), VLX_Value::UID); }

  inline VLX_Value toString(const std::string& str)     { return VLX_Value(str.c_str(), VLX_Value::String); }

  inline VLX_Value toRawtext(const std::string& str)    { return VLX_Value( new VLX_RawtextBlock(NULL, str.c_str()) ); }

  inline fvec2 to_fvec2(const VLX_ArrayReal* arr) { VL_CHECK(arr->value().size() == 2); fvec2 v; arr->copyTo(v.ptr()); return v;  }

  inline fvec3 to_fvec3(const VLX_ArrayReal* arr) { VL_CHECK(arr->value().size() == 3); fvec3 v; arr->copyTo(v.ptr()); return v;  }

  inline fvec4 to_fvec4(const VLX_ArrayReal* arr) { VL_CHECK(arr->value().size() == 4); fvec4 v; arr->copyTo(v.ptr()); return v;  }

  inline ivec4 to_ivec4(const VLX_ArrayInteger* arr) { VL_CHECK(arr->value().size() == 4); ivec4 v; arr->copyTo(v.ptr()); return v; }

  inline uvec4 to_uivec4(const VLX_ArrayInteger* arr) { VL_CHECK(arr->value().size() == 4); uvec4 v; arr->copyTo(v.ptr()); return v; }

  inline VLX_Value toValue(const vec4& vec)
  {
    VLX_Value val( new VLX_ArrayReal );
    VLX_ArrayReal* arr = val.getArrayReal();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLX_Value toValue(const ivec4& vec)
  {
    VLX_Value val( new VLX_ArrayReal );
    VLX_ArrayReal* arr = val.getArrayReal();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLX_Value toValue(const uvec4& vec)
  {
    VLX_Value val( new VLX_ArrayReal );
    VLX_ArrayReal* arr = val.getArrayReal();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLX_Value toValue(const vec3& vec)
  {
    VLX_Value val( new VLX_ArrayReal );
    VLX_ArrayReal* arr = val.getArrayReal();
    arr->value().resize(3);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    return val;
  }

  inline VLX_Value toValue(const vec2& vec)
  {
    VLX_Value val( new VLX_ArrayReal );
    VLX_ArrayReal* arr = val.getArrayReal();
    arr->value().resize(2);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    return val;
  }

  inline bool isTranslation(const fmat4& mat)
  {
    fmat4 tmp = mat;
    tmp.setT( fvec3(0,0,0) );
    return tmp.isIdentity();
  }

  inline bool isScaling(const fmat4& mat)
  {
    fmat4 tmp = mat;
    tmp.e(0,0) = 1;
    tmp.e(1,1) = 1;
    tmp.e(2,2) = 1;
    return tmp.isIdentity();
  }

  inline VLX_Value toValue(const fmat4& mat)
  {
    VLX_Value matrix_list( new VLX_List );

    if (isTranslation(mat))
    {
      VLX_Value value( new VLX_ArrayReal("<Translate>") );
      value.getArrayReal()->value().resize(3);
      value.getArrayReal()->value()[0] = mat.getT().x();
      value.getArrayReal()->value()[1] = mat.getT().y();
      value.getArrayReal()->value()[2] = mat.getT().z();
      matrix_list.getList()->value().push_back( value );
    }
    else
    if (isScaling(mat))
    {
      VLX_Value value( new VLX_ArrayReal("<Scale>") );
      value.getArrayReal()->value().resize(3);
      value.getArrayReal()->value()[0] = mat.e(0,0);
      value.getArrayReal()->value()[1] = mat.e(1,1);
      value.getArrayReal()->value()[2] = mat.e(2,2);
      matrix_list.getList()->value().push_back( value );
    }
    else
    {
      VLX_Value value( new VLX_ArrayReal("<Matrix>") );
      value.getArrayReal()->value().resize(4*4);
      // if we transpose this we have to transpose also the uniform matrices
      value.getArrayReal()->copyFrom(mat.ptr());
      matrix_list.getList()->value().push_back( value );
    }

    return matrix_list;
  }

  inline fmat4 to_fmat4( const VLX_ArrayReal* arr )
  {
    fmat4 mat;
    arr->copyTo(mat.ptr());
    return mat;
  }

  inline fmat4 to_fmat4( const VLX_List* list )
  {
    fmat4 mat;

    for(size_t i=0; i<list->value().size(); ++i)
    {
      const VLX_Value& value = list->value()[i];
      if (value.type() != VLX_Value::ArrayReal)
      {
        Log::error( Say("Line %n : parse error during matrix import.\n") << value.lineNumber() );
        return mat4::getNull();
      }
      // composition of subtransforms is made by post multiplication like for COLLADA.
      const VLX_ArrayReal* arr = value.getArrayReal();
      if (arr->tag() == "<Translate>")
      {
        fvec3 tr = to_fvec3( arr );
        mat = mat * fmat4::getTranslation(tr);
      }
      else
      if (arr->tag() == "<Scale>")
      {
        fvec3 sc = to_fvec3( arr );
        mat = mat * fmat4::getScaling(sc);
      }
      else
      if (arr->tag() == "<Matrix>")
      {
        fmat4 m = to_fmat4( arr );
        mat = mat * m;
      }
      else
      if (arr->tag() == "<LookAt>")
      {
        // implements the camera's view-matrix look-at as specified by COLLADA
        if (arr->value().size() != 9)
        {
          Log::error( Say("Line %n : <LookAt> must have 9 floats, 3 for 'eye', 3 for 'look' and 3 for 'up'.\n") << arr->lineNumber() << arr->tag() ); // mic fixme: test
        }
        else
        {
          // mic fixme: test this
          fvec3 eye, look, up;
          eye.x()  = (float)arr->value()[0];
          eye.y()  = (float)arr->value()[1];
          eye.z()  = (float)arr->value()[2];
          look.x() = (float)arr->value()[3];
          look.y() = (float)arr->value()[4];
          look.z() = (float)arr->value()[5];
          up.x()   = (float)arr->value()[6];
          up.y()   = (float)arr->value()[7];
          up.z()   = (float)arr->value()[8];
          mat = mat * fmat4::getLookAt(eye, look, up).invert();
        }
      }
      else
      if (arr->tag() == "<Skew>")
      {
        Log::error("<Skew> tag not yet supported.\n"); // mic fixme: test these two
      }
      else
      {
        Log::error( Say("Line %n : unknown tag '%s' ignored.\n") << arr->lineNumber() << arr->tag() );
      }
    }

    return mat;
  }

  inline const char* stringfy_EProjectionMatrixType(EProjectionMatrixType pt)
  {
    switch(pt)
    {
    default:
    case PMT_UserProjection: return "PMT_UserProjection";
    case PMT_OrthographicProjection: return "PMT_OrthographicProjection";
    case PMT_PerspectiveProjection: return "PMT_PerspectiveProjection";
    case PMT_PerspectiveProjectionFrustum: return "PMT_PerspectiveProjectionFrustum";
    }
  }

  inline EProjectionMatrixType destringfy_EProjectionMatrixType(const char* str)
  {
    if (strcmp(str, "PMT_OrthographicProjection") == 0) return PMT_OrthographicProjection;
    if (strcmp(str, "PMT_PerspectiveProjection") == 0) return PMT_PerspectiveProjection;
    if (strcmp(str, "PMT_PerspectiveProjectionFrustum") == 0) return PMT_PerspectiveProjectionFrustum;
    /*if (strcmp(str, "PMT_UserProjection") == 0)*/ return PMT_UserProjection;
  }

  inline const char* stringfy_EClearColorMode(EClearColorMode ccm)
  {
    switch(ccm)
    {
    default:
    case CCM_Float: return "CCM_Float";
    case CCM_Int: return "CCM_Int";
    case CCM_UInt: return "CCM_UInt";
    }
  }

  inline EClearColorMode destringfy_EClearColorMode(const char* str)
  {
    if (strcmp(str, "CCM_Int") == 0) return CCM_Int;
    if (strcmp(str, "CCM_UInt") == 0) return CCM_UInt;
    /*if (strcmp(str, "CCM_Float") == 0)*/ return CCM_Float;
  }

  inline const char* stringfy_EClearFlags(EClearFlags cf)
  {
    switch(cf)
    {
    default:
    case CF_CLEAR_COLOR_DEPTH_STENCIL: return "CF_CLEAR_COLOR_DEPTH_STENCIL";
    case CF_DO_NOT_CLEAR: return "CF_DO_NOT_CLEAR";
    case CF_CLEAR_COLOR: return "CF_CLEAR_COLOR";
    case CF_CLEAR_DEPTH: return "CF_CLEAR_DEPTH";
    case CF_CLEAR_STENCIL: return "CF_CLEAR_STENCIL";
    case CF_CLEAR_COLOR_DEPTH: return "CF_CLEAR_COLOR_DEPTH";
    case CF_CLEAR_COLOR_STENCIL: return "CF_CLEAR_COLOR_STENCIL";
    case CF_CLEAR_DEPTH_STENCIL: return "CF_CLEAR_DEPTH_STENCIL";
    }
  }

  inline EClearFlags destringfy_EClearFlags(const char* str)
  {
    if (strcmp(str, "CF_DO_NOT_CLEAR") == 0) return CF_DO_NOT_CLEAR;
    if (strcmp(str, "CF_CLEAR_COLOR") == 0) return CF_CLEAR_COLOR;
    if (strcmp(str, "CF_CLEAR_DEPTH") == 0) return CF_CLEAR_DEPTH;
    if (strcmp(str, "CF_CLEAR_STENCIL") == 0) return CF_CLEAR_STENCIL;
    if (strcmp(str, "CF_CLEAR_COLOR_DEPTH") == 0) return CF_CLEAR_COLOR_DEPTH;
    if (strcmp(str, "CF_CLEAR_COLOR_STENCIL") == 0) return CF_CLEAR_COLOR_STENCIL;
    if (strcmp(str, "CF_CLEAR_DEPTH_STENCIL") == 0) return CF_CLEAR_DEPTH_STENCIL;
    /*if (strcmp(str, "CF_CLEAR_COLOR_DEPTH_STENCIL") == 0)*/ return CF_CLEAR_COLOR_DEPTH_STENCIL;
  }

  inline const char* stringfy_EPolygonFace(EPolygonFace pf)
  {
    switch(pf)
    {
    default:
    case PF_FRONT_AND_BACK: return "PF_FRONT_AND_BACK";
    case PF_FRONT: return "PF_FRONT";
    case PF_BACK:  return "PF_BACK";
    }
  }

  inline EPolygonFace destringfy_EPolygonFace(const char* str)
  {
    if (strcmp(str, "PF_FRONT") == 0) return PF_FRONT;
    if (strcmp(str, "PF_BACK") == 0) return PF_BACK;
    /*if (strcmp(str, "PF_FRONT_AND_BACK") == 0)*/ return PF_FRONT_AND_BACK;
  }

  inline const char* stringfy_EColorMaterial(EColorMaterial cm)
  {
    switch(cm)
    {
    default:
    case CM_AMBIENT_AND_DIFFUSE: return "CM_AMBIENT_AND_DIFFUSE";
    case CM_EMISSION: return "CM_EMISSION";
    case CM_AMBIENT: return "CM_AMBIENT";
    case CM_DIFFUSE: return "CM_DIFFUSE";
    case CM_SPECULAR: return "CM_SPECULAR";
    }
  }

  inline EColorMaterial destringfy_EColorMaterial(const char* str)
  {
    if (strcmp(str, "CM_EMISSION") == 0) return CM_EMISSION;
    if (strcmp(str, "CM_AMBIENT") == 0) return CM_AMBIENT;
    if (strcmp(str, "CM_DIFFUSE") == 0) return CM_DIFFUSE;
    if (strcmp(str, "CM_SPECULAR") == 0) return CM_SPECULAR;
    /*if (strcmp(str, "CM_AMBIENT_AND_DIFFUSE") == 0) */return CM_AMBIENT_AND_DIFFUSE;
  }

  inline const char* stringfy_ETextureFormat(ETextureFormat tf)
  {
    switch(tf)
    {
    default:
    case TF_UNKNOWN: return "TF_UNKNOWN";

    case TF_ALPHA  : return "ALPHA";
    case TF_ALPHA4 : return "ALPHA4";
    case TF_ALPHA8 : return "ALPHA8";
    case TF_ALPHA12: return "ALPHA12";
    case TF_ALPHA16: return "ALPHA16";

    case TF_INTENSITY  : return "INTENSITY";
    case TF_INTENSITY4 : return "INTENSITY4";
    case TF_INTENSITY8 : return "INTENSITY8";
    case TF_INTENSITY12: return "INTENSITY12";
    case TF_INTENSITY16: return "INTENSITY16";
    case TF_LUMINANCE  : return "LUMINANCE";
    case TF_LUMINANCE4 : return "LUMINANCE4";
    case TF_LUMINANCE8 : return "LUMINANCE8";
    case TF_LUMINANCE12: return "LUMINANCE12";
    case TF_LUMINANCE16: return "LUMINANCE16";
    case TF_LUMINANCE_ALPHA    : return "LUMINANCE_ALPHA";
    case TF_LUMINANCE4_ALPHA4  : return "LUMINANCE4_ALPHA4";
    case TF_LUMINANCE6_ALPHA2  : return "LUMINANCE6_ALPHA2";
    case TF_LUMINANCE8_ALPHA8  : return "LUMINANCE8_ALPHA8";
    case TF_LUMINANCE12_ALPHA4 : return "LUMINANCE12_ALPHA4";
    case TF_LUMINANCE12_ALPHA12: return "LUMINANCE12_ALPHA12";
    case TF_LUMINANCE16_ALPHA16: return "LUMINANCE16_ALPHA16";
    case TF_R3_G3_B2: return "R3_G3_B2";
    case TF_RGB     : return "RGB";
    case TF_RGB4    : return "RGB4";
    case TF_RGB5    : return "RGB5";
    case TF_RGB8    : return "RGB8";
    case TF_RGB10   : return "RGB10";
    case TF_RGB12   : return "RGB12";
    case TF_RGB16   : return "RGB16";
    case TF_RGBA    : return "RGBA";
    case TF_RGBA2   : return "RGBA2";
    case TF_RGBA4   : return "RGBA4";
    case TF_RGB5_A1 : return "RGB5_A1";
    case TF_RGBA8   : return "RGBA8";
    case TF_RGB10_A2: return "RGB10_A2";
    case TF_RGBA12  : return "RGBA12";
    case TF_RGBA16  : return "RGBA16";

    // ARB_texture_float / OpenGL 3
    case TF_RGBA32F: return "RGBA32F";
    case TF_RGB32F: return "RGB32F";
    case TF_ALPHA32F: return "ALPHA32F";
    case TF_INTENSITY32F: return "INTENSITY32F";
    case TF_LUMINANCE32F: return "LUMINANCE32F";
    case TF_LUMINANCE_ALPHA32F: return "LUMINANCE_ALPHA32F";
    case TF_RGBA16F: return "RGBA16F";
    case TF_RGB16F: return "RGB16F";
    case TF_ALPHA16F: return "ALPHA16F";
    case TF_INTENSITY16F: return "INTENSITY16F";
    case TF_LUMINANCE16F: return "LUMINANCE16F";
    case TF_LUMINANCE_ALPHA16F: return "LUMINANCE_ALPHA16F";

    // from table 3.12 opengl api specs 4.1
    case TF_R8_SNORM: return "R8_SNORM";
    case TF_R16_SNORM: return "R16_SNORM";
    case TF_RG8_SNORM: return "RG8_SNORM";
    case TF_RG16_SNORM: return "RG16_SNORM";
    case TF_RGB8_SNORM: return "RGB8_SNORM";
    case TF_RGBA8_SNORM: return "RGBA8_SNORM";
    case TF_RGB10_A2UI: return "RGB10_A2UI";
    case TF_RGBA16_SNORM: return "RGBA16_SNORM";
    case TF_R11F_G11F_B10F: return "R11F_G11F_B10F";
    case TF_RGB9_E5: return "RGB9_E5";
    case TF_RGB8I: return "RGB8I";
    case TF_RGB8UI: return "RGB8UI";
    case TF_RGB16I: return "RGB16I";
    case TF_RGB16UI: return "RGB16UI";
    case TF_RGB32I: return "RGB32I";
    case TF_RGB32UI: return "RGB32UI";
    case TF_RGBA8I: return "RGBA8I";
    case TF_RGBA8UI: return "RGBA8UI";
    case TF_RGBA16I: return "RGBA16I";
    case TF_RGBA16UI: return "RGBA16UI";
    case TF_RGBA32I: return "RGBA32I";
    case TF_RGBA32UI: return "TF_RGBA32UI";

    // ATI_texture_float (the enums are the same as ARB_texture_float)
    //case TF_RGBA_FLOAT32_ATI: return "RGBA_FLOAT32_ATI";
    //case TF_RGB_FLOAT32_ATI: return "RGB_FLOAT32_ATI";
    //case TF_ALPHA_FLOAT32_ATI: return "ALPHA_FLOAT32_ATI";
    //case TF_INTENSITY_FLOAT32_ATI: return "INTENSITY_FLOAT32_ATI";
    //case TF_LUMINANCE_FLOAT32_ATI: return "LUMINANCE_FLOAT32_ATI";
    //case TF_LUMINANCE_ALPHA_FLOAT32_ATI: return "LUMINANCE_ALPHA_FLOAT32_ATI";
    //case TF_RGBA_FLOAT16_ATI: return "RGBA_FLOAT16_ATI";
    //case TF_RGB_FLOAT16_ATI: return "RGB_FLOAT16_ATI";
    //case TF_ALPHA_FLOAT16_ATI: return "ALPHA_FLOAT16_ATI";
    //case TF_INTENSITY_FLOAT16_ATI: return "INTENSITY_FLOAT16_ATI";
    //case TF_LUMINANCE_FLOAT16_ATI: return "LUMINANCE_FLOAT16_ATI";
    //case TF_LUMINANCE_ALPHA_FLOAT16_ATI: return "LUMINANCE_ALPHA_FLOAT16_ATI";

    // EXT_texture_shared_exponent
    // case TF_RGB9_E5_EXT: return "RGB9_E5_EXT";

    // EXT_packed_float
    // case TF_11F_G11F_B10F_EXT: return "11F_G11F_B10F_EXT";

    // EXT_packed_depth_stencil / GL_ARB_framebuffer_object
    case TF_DEPTH_STENCIL   : return "DEPTH_STENCIL";
    case TF_DEPTH24_STENCIL8: return "DEPTH24_STENCIL8";

    // ARB_depth_buffer_float
    case TF_DEPTH_COMPONENT32F: return "DEPTH_COMPONENT32F";
    case TF_DEPTH32F_STENCIL8 : return "DEPTH32F_STENCIL8";

    // ARB_depth_texture
    case TF_DEPTH_COMPONENT  : return "DEPTH_COMPONENT";
    case TF_DEPTH_COMPONENT16: return "DEPTH_COMPONENT16";
    case TF_DEPTH_COMPONENT24: return "DEPTH_COMPONENT24";
    case TF_DEPTH_COMPONENT32: return "DEPTH_COMPONENT32";

    // ARB_texture_compression
    case TF_COMPRESSED_ALPHA          : return "COMPRESSED_ALPHA";
    case TF_COMPRESSED_INTENSITY      : return "COMPRESSED_INTENSITY";
    case TF_COMPRESSED_LUMINANCE      : return "COMPRESSED_LUMINANCE";
    case TF_COMPRESSED_LUMINANCE_ALPHA: return "COMPRESSED_LUMINANCE_ALPHA";
    case TF_COMPRESSED_RGB            : return "COMPRESSED_RGB";
    case TF_COMPRESSED_RGBA           : return "COMPRESSED_RGBA";

    // 3DFX_texture_compression_FXT1
    case TF_COMPRESSED_RGB_FXT1_3DFX : return "COMPRESSED_RGB_FXT1_3DFX";
    case TF_COMPRESSED_RGBA_FXT1_3DFX: return "COMPRESSED_RGBA_FXT1_3DFX";

    // EXT_texture_compression_s3tc
    case TF_COMPRESSED_RGB_S3TC_DXT1_EXT : return "COMPRESSED_RGB_S3TC_DXT1_EXT";
    case TF_COMPRESSED_RGBA_S3TC_DXT1_EXT: return "COMPRESSED_RGBA_S3TC_DXT1_EXT";
    case TF_COMPRESSED_RGBA_S3TC_DXT3_EXT: return "COMPRESSED_RGBA_S3TC_DXT3_EXT";
    case TF_COMPRESSED_RGBA_S3TC_DXT5_EXT: return "COMPRESSED_RGBA_S3TC_DXT5_EXT";

    // EXT_texture_compression_latc
    case TF_COMPRESSED_LUMINANCE_LATC1_EXT             : return "COMPRESSED_LUMINANCE_LATC1_EXT";
    case TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT      : return "COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT";
    case TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT       : return "COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT";
    case TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT: return "COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT";

    // EXT_texture_compression_rgtc
    case TF_COMPRESSED_RED_RGTC1_EXT             : return "COMPRESSED_RED_RGTC1_EXT";
    case TF_COMPRESSED_SIGNED_RED_RGTC1_EXT      : return "COMPRESSED_SIGNED_RED_RGTC1_EXT";
    case TF_COMPRESSED_RED_GREEN_RGTC2_EXT       : return "COMPRESSED_RED_GREEN_RGTC2_EXT";
    case TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT: return "COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT";

    // EXT_texture_integer
    // case TF_RGBA32UI_EXT: return "RGBA32UI_EXT";
    // case TF_RGB32UI_EXT: return "RGB32UI_EXT";
    case TF_ALPHA32UI_EXT: return "ALPHA32UI_EXT";
    case TF_INTENSITY32UI_EXT: return "INTENSITY32UI_EXT";
    case TF_LUMINANCE32UI_EXT: return "LUMINANCE32UI_EXT";
    case TF_LUMINANCE_ALPHA32UI_EXT: return "LUMINANCE_ALPHA32UI_EXT";

    // case TF_RGBA16UI_EXT: return "RGBA16UI_EXT";
    // case TF_RGB16UI_EXT: return "RGB16UI_EXT";
    case TF_ALPHA16UI_EXT: return "ALPHA16UI_EXT";
    case TF_INTENSITY16UI_EXT: return "INTENSITY16UI_EXT";
    case TF_LUMINANCE16UI_EXT: return "LUMINANCE16UI_EXT";
    case TF_LUMINANCE_ALPHA16UI_EXT: return "LUMINANCE_ALPHA16UI_EXT";

    // case TF_RGBA8UI_EXT: return "RGBA8UI_EXT";
    // case TF_RGB8UI_EXT: return "RGB8UI_EXT";
    case TF_ALPHA8UI_EXT: return "ALPHA8UI_EXT";
    case TF_INTENSITY8UI_EXT: return "INTENSITY8UI_EXT";
    case TF_LUMINANCE8UI_EXT: return "LUMINANCE8UI_EXT";
    case TF_LUMINANCE_ALPHA8UI_EXT: return "LUMINANCE_ALPHA8UI_EXT";

    // case TF_RGBA32I_EXT: return "RGBA32I_EXT";
    // case TF_RGB32I_EXT: return "RGB32I_EXT";
    case TF_ALPHA32I_EXT: return "ALPHA32I_EXT";
    case TF_INTENSITY32I_EXT: return "INTENSITY32I_EXT";
    case TF_LUMINANCE32I_EXT: return "LUMINANCE32I_EXT";
    case TF_LUMINANCE_ALPHA32I_EXT: return "LUMINANCE_ALPHA32I_EXT";

    // case TF_RGBA16I_EXT: return "RGBA16I_EXT";
    // case TF_RGB16I_EXT: return "RGB16I_EXT";
    case TF_ALPHA16I_EXT: return "ALPHA16I_EXT";
    case TF_INTENSITY16I_EXT: return "INTENSITY16I_EXT";
    case TF_LUMINANCE16I_EXT: return "LUMINANCE16I_EXT";
    case TF_LUMINANCE_ALPHA16I_EXT: return "LUMINANCE_ALPHA16I_EXT";

    // case TF_RGBA8I_EXT: return "RGBA8I_EXT";
    // case TF_RGB8I_EXT: return "RGB8I_EXT";
    case TF_ALPHA8I_EXT: return "ALPHA8I_EXT";
    case TF_INTENSITY8I_EXT: return "INTENSITY8I_EXT";
    case TF_LUMINANCE8I_EXT: return "LUMINANCE8I_EXT";
    case TF_LUMINANCE_ALPHA8I_EXT: return "LUMINANCE_ALPHA8I_EXT";

    // GL_ARB_texture_rg
    case TF_RED: return "RED";
    case TF_COMPRESSED_RED: return "COMPRESSED_RED";
    case TF_COMPRESSED_RG: return "COMPRESSED_RG";
    case TF_RG: return "RG";
    case TF_R8: return "R8";
    case TF_R16: return "R16";
    case TF_RG8: return "RG8";
    case TF_RG16: return "RG16";
    case TF_R16F: return "R16F";
    case TF_R32F: return "R32F";
    case TF_RG16F: return "RG16F";
    case TF_RG32F: return "RG32F";
    case TF_R8I: return "R8I";
    case TF_R8UI: return "R8UI";
    case TF_R16I: return "R16I";
    case TF_R16UI: return "R16UI";
    case TF_R32I: return "R32I";
    case TF_R32UI: return "R32UI";
    case TF_RG8I: return "RG8I";
    case TF_RG8UI: return "RG8UI";
    case TF_RG16I: return "RG16I";
    case TF_RG16UI: return "RG16UI";
    case TF_RG32I: return "RG32I";
    case TF_RG32UI: return "RG32UI";

    // sRGB OpenGL 2.1
    case TF_SLUMINANCE_ALPHA: return "SLUMINANCE_ALPHA";
    case TF_SLUMINANCE8_ALPHA8: return "SLUMINANCE8_ALPHA8";
    case TF_SLUMINANCE: return "SLUMINANCE";
    case TF_SLUMINANCE8: return "SLUMINANCE8";
    case TF_COMPRESSED_SLUMINANCE: return "COMPRESSED_SLUMINANCE";
    case TF_COMPRESSED_SLUMINANCE_ALPHA: return "COMPRESSED_SLUMINANCE_ALPHA";

    // sRGB OpenGL 2.1 / 3.x
    case TF_SRGB: return "SRGB";
    case TF_SRGB8: return "SRGB8";
    case TF_SRGB_ALPHA: return "SRGB_ALPHA";
    case TF_SRGB8_ALPHA8: return "SRGB8_ALPHA8";
    case TF_COMPRESSED_SRGB: return "COMPRESSED_SRGB";
    case TF_COMPRESSED_SRGB_ALPHA: return "COMPRESSED_SRGB_ALPHA";

    // GL_EXT_texture_sRGB compressed formats
    case TF_COMPRESSED_SRGB_S3TC_DXT1_EXT: return "COMPRESSED_SRGB_S3TC_DXT1_EXT";
    case TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
    case TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
    case TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT: return "COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
    }
  }

  inline ETextureFormat destringfy_ETextureFormat(const char* str)
  {
    if (strcmp(str, "TF_ALPHA  ") == 0) return TF_ALPHA  ;
    if (strcmp(str, "TF_ALPHA4 ") == 0) return TF_ALPHA4 ;
    if (strcmp(str, "TF_ALPHA8 ") == 0) return TF_ALPHA8 ;
    if (strcmp(str, "TF_ALPHA12") == 0) return TF_ALPHA12;
    if (strcmp(str, "TF_ALPHA16") == 0) return TF_ALPHA16;

    if (strcmp(str, "TF_INTENSITY  ") == 0) return TF_INTENSITY  ;
    if (strcmp(str, "TF_INTENSITY4 ") == 0) return TF_INTENSITY4 ;
    if (strcmp(str, "TF_INTENSITY8 ") == 0) return TF_INTENSITY8 ;
    if (strcmp(str, "TF_INTENSITY12") == 0) return TF_INTENSITY12;
    if (strcmp(str, "TF_INTENSITY16") == 0) return TF_INTENSITY16;
    if (strcmp(str, "TF_LUMINANCE  ") == 0) return TF_LUMINANCE  ;
    if (strcmp(str, "TF_LUMINANCE4 ") == 0) return TF_LUMINANCE4 ;
    if (strcmp(str, "TF_LUMINANCE8 ") == 0) return TF_LUMINANCE8 ;
    if (strcmp(str, "TF_LUMINANCE12") == 0) return TF_LUMINANCE12;
    if (strcmp(str, "TF_LUMINANCE16") == 0) return TF_LUMINANCE16;
    if (strcmp(str, "TF_LUMINANCE_ALPHA    ") == 0) return TF_LUMINANCE_ALPHA    ;
    if (strcmp(str, "TF_LUMINANCE4_ALPHA4  ") == 0) return TF_LUMINANCE4_ALPHA4  ;
    if (strcmp(str, "TF_LUMINANCE6_ALPHA2  ") == 0) return TF_LUMINANCE6_ALPHA2  ;
    if (strcmp(str, "TF_LUMINANCE8_ALPHA8  ") == 0) return TF_LUMINANCE8_ALPHA8  ;
    if (strcmp(str, "TF_LUMINANCE12_ALPHA4 ") == 0) return TF_LUMINANCE12_ALPHA4 ;
    if (strcmp(str, "TF_LUMINANCE12_ALPHA12") == 0) return TF_LUMINANCE12_ALPHA12;
    if (strcmp(str, "TF_LUMINANCE16_ALPHA16") == 0) return TF_LUMINANCE16_ALPHA16;
    if (strcmp(str, "TF_R3_G3_B2") == 0) return TF_R3_G3_B2;
    if (strcmp(str, "TF_RGB     ") == 0) return TF_RGB     ;
    if (strcmp(str, "TF_RGB4    ") == 0) return TF_RGB4    ;
    if (strcmp(str, "TF_RGB5    ") == 0) return TF_RGB5    ;
    if (strcmp(str, "TF_RGB8    ") == 0) return TF_RGB8    ;
    if (strcmp(str, "TF_RGB10   ") == 0) return TF_RGB10   ;
    if (strcmp(str, "TF_RGB12   ") == 0) return TF_RGB12   ;
    if (strcmp(str, "TF_RGB16   ") == 0) return TF_RGB16   ;
    if (strcmp(str, "TF_RGBA    ") == 0) return TF_RGBA    ;
    if (strcmp(str, "TF_RGBA2   ") == 0) return TF_RGBA2   ;
    if (strcmp(str, "TF_RGBA4   ") == 0) return TF_RGBA4   ;
    if (strcmp(str, "TF_RGB5_A1 ") == 0) return TF_RGB5_A1 ;
    if (strcmp(str, "TF_RGBA8   ") == 0) return TF_RGBA8   ;
    if (strcmp(str, "TF_RGB10_A2") == 0) return TF_RGB10_A2;
    if (strcmp(str, "TF_RGBA12  ") == 0) return TF_RGBA12  ;
    if (strcmp(str, "TF_RGBA16  ") == 0) return TF_RGBA16  ;

    // ARB_texture_float / OpenGL 3
    if (strcmp(str, "TF_RGBA32F") == 0) return TF_RGBA32F;
    if (strcmp(str, "TF_RGB32F") == 0) return TF_RGB32F;
    if (strcmp(str, "TF_ALPHA32F") == 0) return TF_ALPHA32F;
    if (strcmp(str, "TF_INTENSITY32F") == 0) return TF_INTENSITY32F;
    if (strcmp(str, "TF_LUMINANCE32F") == 0) return TF_LUMINANCE32F;
    if (strcmp(str, "TF_LUMINANCE_ALPHA32F") == 0) return TF_LUMINANCE_ALPHA32F;
    if (strcmp(str, "TF_RGBA16F") == 0) return TF_RGBA16F;
    if (strcmp(str, "TF_RGB16F") == 0) return TF_RGB16F;
    if (strcmp(str, "TF_ALPHA16F") == 0) return TF_ALPHA16F;
    if (strcmp(str, "TF_INTENSITY16F") == 0) return TF_INTENSITY16F;
    if (strcmp(str, "TF_LUMINANCE16F") == 0) return TF_LUMINANCE16F;
    if (strcmp(str, "TF_LUMINANCE_ALPHA16F") == 0) return TF_LUMINANCE_ALPHA16F;

    // from table 3.12 opengl api specs 4.1
    if (strcmp(str, "TF_R8_SNORM") == 0) return TF_R8_SNORM;
    if (strcmp(str, "TF_R16_SNORM") == 0) return TF_R16_SNORM;
    if (strcmp(str, "TF_RG8_SNORM") == 0) return TF_RG8_SNORM;
    if (strcmp(str, "TF_RG16_SNORM") == 0) return TF_RG16_SNORM;
    if (strcmp(str, "TF_RGB8_SNORM") == 0) return TF_RGB8_SNORM;
    if (strcmp(str, "TF_RGBA8_SNORM") == 0) return TF_RGBA8_SNORM;
    if (strcmp(str, "TF_RGB10_A2UI") == 0) return TF_RGB10_A2UI;
    if (strcmp(str, "TF_RGBA16_SNORM") == 0) return TF_RGBA16_SNORM;
    if (strcmp(str, "TF_R11F_G11F_B10F") == 0) return TF_R11F_G11F_B10F;
    if (strcmp(str, "TF_RGB9_E5") == 0) return TF_RGB9_E5;
    if (strcmp(str, "TF_RGB8I") == 0) return TF_RGB8I;
    if (strcmp(str, "TF_RGB8UI") == 0) return TF_RGB8UI;
    if (strcmp(str, "TF_RGB16I") == 0) return TF_RGB16I;
    if (strcmp(str, "TF_RGB16UI") == 0) return TF_RGB16UI;
    if (strcmp(str, "TF_RGB32I") == 0) return TF_RGB32I;
    if (strcmp(str, "TF_RGB32UI") == 0) return TF_RGB32UI;
    if (strcmp(str, "TF_RGBA8I") == 0) return TF_RGBA8I;
    if (strcmp(str, "TF_RGBA8UI") == 0) return TF_RGBA8UI;
    if (strcmp(str, "TF_RGBA16I") == 0) return TF_RGBA16I;
    if (strcmp(str, "TF_RGBA16UI") == 0) return TF_RGBA16UI;
    if (strcmp(str, "TF_RGBA32I") == 0) return TF_RGBA32I;
    if (strcmp(str, "TF_RGBA32UI") == 0) return TF_RGBA32UI;

    // ATI_texture_float (the enums are the same as ARB_texture_float)
    if (strcmp(str, "TF_RGBA_FLOAT32_ATI") == 0) return TF_RGBA_FLOAT32_ATI;
    if (strcmp(str, "TF_RGB_FLOAT32_ATI") == 0) return TF_RGB_FLOAT32_ATI;
    if (strcmp(str, "TF_ALPHA_FLOAT32_ATI") == 0) return TF_ALPHA_FLOAT32_ATI;
    if (strcmp(str, "TF_INTENSITY_FLOAT32_ATI") == 0) return TF_INTENSITY_FLOAT32_ATI;
    if (strcmp(str, "TF_LUMINANCE_FLOAT32_ATI") == 0) return TF_LUMINANCE_FLOAT32_ATI;
    if (strcmp(str, "TF_LUMINANCE_ALPHA_FLOAT32_ATI") == 0) return TF_LUMINANCE_ALPHA_FLOAT32_ATI;
    if (strcmp(str, "TF_RGBA_FLOAT16_ATI") == 0) return TF_RGBA_FLOAT16_ATI;
    if (strcmp(str, "TF_RGB_FLOAT16_ATI") == 0) return TF_RGB_FLOAT16_ATI;
    if (strcmp(str, "TF_ALPHA_FLOAT16_ATI") == 0) return TF_ALPHA_FLOAT16_ATI;
    if (strcmp(str, "TF_INTENSITY_FLOAT16_ATI") == 0) return TF_INTENSITY_FLOAT16_ATI;
    if (strcmp(str, "TF_LUMINANCE_FLOAT16_ATI") == 0) return TF_LUMINANCE_FLOAT16_ATI;
    if (strcmp(str, "TF_LUMINANCE_ALPHA_FLOAT16_ATI") == 0) return TF_LUMINANCE_ALPHA_FLOAT16_ATI;

    // EXT_texture_shared_exponent
    if (strcmp(str, "TF_RGB9_E5_EXT") == 0) return TF_RGB9_E5_EXT;

    // EXT_packed_float
    if (strcmp(str, "TF_11F_G11F_B10F_EXT") == 0) return TF_11F_G11F_B10F_EXT;

    // EXT_packed_depth_stencil / GL_ARB_framebuffer_object
    if (strcmp(str, "TF_DEPTH_STENCIL   ") == 0) return TF_DEPTH_STENCIL   ;
    if (strcmp(str, "TF_DEPTH24_STENCIL8") == 0) return TF_DEPTH24_STENCIL8;

    // ARB_depth_buffer_float
    if (strcmp(str, "TF_DEPTH_COMPONENT32F") == 0) return TF_DEPTH_COMPONENT32F;
    if (strcmp(str, "TF_DEPTH32F_STENCIL8 ") == 0) return TF_DEPTH32F_STENCIL8 ;

    // ARB_depth_texture
    if (strcmp(str, "TF_DEPTH_COMPONENT  ") == 0) return TF_DEPTH_COMPONENT  ;
    if (strcmp(str, "TF_DEPTH_COMPONENT16") == 0) return TF_DEPTH_COMPONENT16;
    if (strcmp(str, "TF_DEPTH_COMPONENT24") == 0) return TF_DEPTH_COMPONENT24;
    if (strcmp(str, "TF_DEPTH_COMPONENT32") == 0) return TF_DEPTH_COMPONENT32;

    // ARB_texture_compression
    if (strcmp(str, "TF_COMPRESSED_ALPHA          ") == 0) return TF_COMPRESSED_ALPHA          ;
    if (strcmp(str, "TF_COMPRESSED_INTENSITY      ") == 0) return TF_COMPRESSED_INTENSITY      ;
    if (strcmp(str, "TF_COMPRESSED_LUMINANCE      ") == 0) return TF_COMPRESSED_LUMINANCE      ;
    if (strcmp(str, "TF_COMPRESSED_LUMINANCE_ALPHA") == 0) return TF_COMPRESSED_LUMINANCE_ALPHA;
    if (strcmp(str, "TF_COMPRESSED_RGB            ") == 0) return TF_COMPRESSED_RGB            ;
    if (strcmp(str, "TF_COMPRESSED_RGBA           ") == 0) return TF_COMPRESSED_RGBA           ;

    // 3DFX_texture_compression_FXT1
    if (strcmp(str, "TF_COMPRESSED_RGB_FXT1_3DFX ") == 0) return TF_COMPRESSED_RGB_FXT1_3DFX ;
    if (strcmp(str, "TF_COMPRESSED_RGBA_FXT1_3DFX") == 0) return TF_COMPRESSED_RGBA_FXT1_3DFX;

    // EXT_texture_compression_s3tc
    if (strcmp(str, "TF_COMPRESSED_RGB_S3TC_DXT1_EXT ") == 0) return TF_COMPRESSED_RGB_S3TC_DXT1_EXT ;
    if (strcmp(str, "TF_COMPRESSED_RGBA_S3TC_DXT1_EXT") == 0) return TF_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    if (strcmp(str, "TF_COMPRESSED_RGBA_S3TC_DXT3_EXT") == 0) return TF_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    if (strcmp(str, "TF_COMPRESSED_RGBA_S3TC_DXT5_EXT") == 0) return TF_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    // EXT_texture_compression_latc
    if (strcmp(str, "TF_COMPRESSED_LUMINANCE_LATC1_EXT             ") == 0) return TF_COMPRESSED_LUMINANCE_LATC1_EXT             ;
    if (strcmp(str, "TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT      ") == 0) return TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT      ;
    if (strcmp(str, "TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT       ") == 0) return TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT       ;
    if (strcmp(str, "TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT") == 0) return TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT;

    // EXT_texture_compression_rgtc
    if (strcmp(str, "TF_COMPRESSED_RED_RGTC1_EXT             ") == 0) return TF_COMPRESSED_RED_RGTC1_EXT             ;
    if (strcmp(str, "TF_COMPRESSED_SIGNED_RED_RGTC1_EXT      ") == 0) return TF_COMPRESSED_SIGNED_RED_RGTC1_EXT      ;
    if (strcmp(str, "TF_COMPRESSED_RED_GREEN_RGTC2_EXT       ") == 0) return TF_COMPRESSED_RED_GREEN_RGTC2_EXT       ;
    if (strcmp(str, "TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT") == 0) return TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT;

    // EXT_texture_integer
    if (strcmp(str, "TF_RGBA32UI_EXT") == 0) return TF_RGBA32UI_EXT;
    if (strcmp(str, "TF_RGB32UI_EXT") == 0) return TF_RGB32UI_EXT;
    if (strcmp(str, "TF_ALPHA32UI_EXT") == 0) return TF_ALPHA32UI_EXT;
    if (strcmp(str, "TF_INTENSITY32UI_EXT") == 0) return TF_INTENSITY32UI_EXT;
    if (strcmp(str, "TF_LUMINANCE32UI_EXT") == 0) return TF_LUMINANCE32UI_EXT;
    if (strcmp(str, "TF_LUMINANCE_ALPHA32UI_EXT") == 0) return TF_LUMINANCE_ALPHA32UI_EXT;

    if (strcmp(str, "TF_RGBA16UI_EXT") == 0) return TF_RGBA16UI_EXT;
    if (strcmp(str, "TF_RGB16UI_EXT") == 0) return TF_RGB16UI_EXT;
    if (strcmp(str, "TF_ALPHA16UI_EXT") == 0) return TF_ALPHA16UI_EXT;
    if (strcmp(str, "TF_INTENSITY16UI_EXT") == 0) return TF_INTENSITY16UI_EXT;
    if (strcmp(str, "TF_LUMINANCE16UI_EXT") == 0) return TF_LUMINANCE16UI_EXT;
    if (strcmp(str, "TF_LUMINANCE_ALPHA16UI_EXT") == 0) return TF_LUMINANCE_ALPHA16UI_EXT;

    if (strcmp(str, "TF_RGBA8UI_EXT") == 0) return TF_RGBA8UI_EXT;
    if (strcmp(str, "TF_RGB8UI_EXT") == 0) return TF_RGB8UI_EXT;
    if (strcmp(str, "TF_ALPHA8UI_EXT") == 0) return TF_ALPHA8UI_EXT;
    if (strcmp(str, "TF_INTENSITY8UI_EXT") == 0) return TF_INTENSITY8UI_EXT;
    if (strcmp(str, "TF_LUMINANCE8UI_EXT") == 0) return TF_LUMINANCE8UI_EXT;
    if (strcmp(str, "TF_LUMINANCE_ALPHA8UI_EXT") == 0) return TF_LUMINANCE_ALPHA8UI_EXT;

    if (strcmp(str, "TF_RGBA32I_EXT") == 0) return TF_RGBA32I_EXT;
    if (strcmp(str, "TF_RGB32I_EXT") == 0) return TF_RGB32I_EXT;
    if (strcmp(str, "TF_ALPHA32I_EXT") == 0) return TF_ALPHA32I_EXT;
    if (strcmp(str, "TF_INTENSITY32I_EXT") == 0) return TF_INTENSITY32I_EXT;
    if (strcmp(str, "TF_LUMINANCE32I_EXT") == 0) return TF_LUMINANCE32I_EXT;
    if (strcmp(str, "TF_LUMINANCE_ALPHA32I_EXT") == 0) return TF_LUMINANCE_ALPHA32I_EXT;

    if (strcmp(str, "TF_RGBA16I_EXT") == 0) return TF_RGBA16I_EXT;
    if (strcmp(str, "TF_RGB16I_EXT") == 0) return TF_RGB16I_EXT;
    if (strcmp(str, "TF_ALPHA16I_EXT") == 0) return TF_ALPHA16I_EXT;
    if (strcmp(str, "TF_INTENSITY16I_EXT") == 0) return TF_INTENSITY16I_EXT;
    if (strcmp(str, "TF_LUMINANCE16I_EXT") == 0) return TF_LUMINANCE16I_EXT;
    if (strcmp(str, "TF_LUMINANCE_ALPHA16I_EXT") == 0) return TF_LUMINANCE_ALPHA16I_EXT;

    if (strcmp(str, "TF_RGBA8I_EXT") == 0) return TF_RGBA8I_EXT;
    if (strcmp(str, "TF_RGB8I_EXT") == 0) return TF_RGB8I_EXT;
    if (strcmp(str, "TF_ALPHA8I_EXT") == 0) return TF_ALPHA8I_EXT;
    if (strcmp(str, "TF_INTENSITY8I_EXT") == 0) return TF_INTENSITY8I_EXT;
    if (strcmp(str, "TF_LUMINANCE8I_EXT") == 0) return TF_LUMINANCE8I_EXT;
    if (strcmp(str, "TF_LUMINANCE_ALPHA8I_EXT") == 0) return TF_LUMINANCE_ALPHA8I_EXT;

    // GL_ARB_texture_rg
    if (strcmp(str, "TF_RED") == 0) return TF_RED;
    if (strcmp(str, "TF_COMPRESSED_RED") == 0) return TF_COMPRESSED_RED;
    if (strcmp(str, "TF_COMPRESSED_RG") == 0) return TF_COMPRESSED_RG;
    if (strcmp(str, "TF_RG") == 0) return TF_RG;
    if (strcmp(str, "TF_R8") == 0) return TF_R8;
    if (strcmp(str, "TF_R16") == 0) return TF_R16;
    if (strcmp(str, "TF_RG8") == 0) return TF_RG8;
    if (strcmp(str, "TF_RG16") == 0) return TF_RG16;
    if (strcmp(str, "TF_R16F") == 0) return TF_R16F;
    if (strcmp(str, "TF_R32F") == 0) return TF_R32F;
    if (strcmp(str, "TF_RG16F") == 0) return TF_RG16F;
    if (strcmp(str, "TF_RG32F") == 0) return TF_RG32F;
    if (strcmp(str, "TF_R8I") == 0) return TF_R8I;
    if (strcmp(str, "TF_R8UI") == 0) return TF_R8UI;
    if (strcmp(str, "TF_R16I") == 0) return TF_R16I;
    if (strcmp(str, "TF_R16UI") == 0) return TF_R16UI;
    if (strcmp(str, "TF_R32I") == 0) return TF_R32I;
    if (strcmp(str, "TF_R32UI") == 0) return TF_R32UI;
    if (strcmp(str, "TF_RG8I") == 0) return TF_RG8I;
    if (strcmp(str, "TF_RG8UI") == 0) return TF_RG8UI;
    if (strcmp(str, "TF_RG16I") == 0) return TF_RG16I;
    if (strcmp(str, "TF_RG16UI") == 0) return TF_RG16UI;
    if (strcmp(str, "TF_RG32I") == 0) return TF_RG32I;
    if (strcmp(str, "TF_RG32UI") == 0) return TF_RG32UI;

    // sRGB OpenGL 2.1
    if (strcmp(str, "TF_SLUMINANCE_ALPHA") == 0) return TF_SLUMINANCE_ALPHA;
    if (strcmp(str, "TF_SLUMINANCE8_ALPHA8") == 0) return TF_SLUMINANCE8_ALPHA8;
    if (strcmp(str, "TF_SLUMINANCE") == 0) return TF_SLUMINANCE;
    if (strcmp(str, "TF_SLUMINANCE8") == 0) return TF_SLUMINANCE8;
    if (strcmp(str, "TF_COMPRESSED_SLUMINANCE") == 0) return TF_COMPRESSED_SLUMINANCE;
    if (strcmp(str, "TF_COMPRESSED_SLUMINANCE_ALPHA") == 0) return TF_COMPRESSED_SLUMINANCE_ALPHA;

    // sRGB OpenGL 2.1 / 3.x
    if (strcmp(str, "TF_SRGB") == 0) return TF_SRGB;
    if (strcmp(str, "TF_SRGB8") == 0) return TF_SRGB8;
    if (strcmp(str, "TF_SRGB_ALPHA") == 0) return TF_SRGB_ALPHA;
    if (strcmp(str, "TF_SRGB8_ALPHA8") == 0) return TF_SRGB8_ALPHA8;
    if (strcmp(str, "TF_COMPRESSED_SRGB") == 0) return TF_COMPRESSED_SRGB;
    if (strcmp(str, "TF_COMPRESSED_SRGB_ALPHA") == 0) return TF_COMPRESSED_SRGB_ALPHA;

    // GL_EXT_texture_sRGB compressed formats
    if (strcmp(str, "TF_COMPRESSED_SRGB_S3TC_DXT1_EXT") == 0) return TF_COMPRESSED_SRGB_S3TC_DXT1_EXT;
    if (strcmp(str, "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT") == 0) return TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
    if (strcmp(str, "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT") == 0) return TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
    if (strcmp(str, "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT") == 0) return TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

    /*if (strcmp(str, "TF_UNKNOWN") == 0)*/ return TF_UNKNOWN;
  }

  inline const char* stringfy_EUniformType(EUniformType type)
  {
    switch(type)
    {
      default: 
        return "UT_NONE";

      case UT_INT:      return "UT_INT";
      case UT_INT_VEC2: return "UT_INT_VEC2";
      case UT_INT_VEC3: return "UT_INT_VEC3";
      case UT_INT_VEC4: return "UT_INT_VEC4";

      case UT_UNSIGNED_INT:      return "UT_UNSIGNED_INT";
      case UT_UNSIGNED_INT_VEC2: return "UT_UNSIGNED_INT_VEC2";
      case UT_UNSIGNED_INT_VEC3: return "UT_UNSIGNED_INT_VEC3";
      case UT_UNSIGNED_INT_VEC4: return "UT_UNSIGNED_INT_VEC4";

      case UT_FLOAT:      return "UT_FLOAT";
      case UT_FLOAT_VEC2: return "UT_FLOAT_VEC2";
      case UT_FLOAT_VEC3: return "UT_FLOAT_VEC3";
      case UT_FLOAT_VEC4: return "UT_FLOAT_VEC4";

      case UT_FLOAT_MAT2: return "UT_FLOAT_MAT2";
      case UT_FLOAT_MAT3: return "UT_FLOAT_MAT3";
      case UT_FLOAT_MAT4: return "UT_FLOAT_MAT4";

      case UT_FLOAT_MAT2x3: return "UT_FLOAT_MAT2x3";
      case UT_FLOAT_MAT3x2: return "UT_FLOAT_MAT3x2";
      case UT_FLOAT_MAT2x4: return "UT_FLOAT_MAT2x4";
      case UT_FLOAT_MAT4x2: return "UT_FLOAT_MAT4x2";
      case UT_FLOAT_MAT3x4: return "UT_FLOAT_MAT3x4";
      case UT_FLOAT_MAT4x3: return "UT_FLOAT_MAT4x3";

      case UT_DOUBLE:      return "UT_DOUBLE";
      case UT_DOUBLE_VEC2: return "UT_DOUBLE_VEC2";
      case UT_DOUBLE_VEC3: return "UT_DOUBLE_VEC3";
      case UT_DOUBLE_VEC4: return "UT_DOUBLE_VEC4";

      case UT_DOUBLE_MAT2: return "UT_DOUBLE_MAT2";
      case UT_DOUBLE_MAT3: return "UT_DOUBLE_MAT3";
      case UT_DOUBLE_MAT4: return "UT_DOUBLE_MAT4";

      case UT_DOUBLE_MAT2x3: return "UT_DOUBLE_MAT2x3";
      case UT_DOUBLE_MAT3x2: return "UT_DOUBLE_MAT3x2";
      case UT_DOUBLE_MAT2x4: return "UT_DOUBLE_MAT2x4";
      case UT_DOUBLE_MAT4x2: return "UT_DOUBLE_MAT4x2";
      case UT_DOUBLE_MAT3x4: return "UT_DOUBLE_MAT3x4";
      case UT_DOUBLE_MAT4x3: return "UT_DOUBLE_MAT4x3";
    }
  }

  inline EUniformType destringfy_EUniformType(const char* type)
  {
    if (strcmp(type, "UT_INT") == 0) return UT_INT;
    if (strcmp(type, "UT_INT_VEC2") == 0) return UT_INT_VEC2;
    if (strcmp(type, "UT_INT_VEC3") == 0) return UT_INT_VEC3;
    if (strcmp(type, "UT_INT_VEC4") == 0) return UT_INT_VEC4;

    if (strcmp(type, "UT_UNSIGNED_INT") == 0) return UT_UNSIGNED_INT;
    if (strcmp(type, "UT_UNSIGNED_INT_VEC2") == 0) return UT_UNSIGNED_INT_VEC2;
    if (strcmp(type, "UT_UNSIGNED_INT_VEC3") == 0) return UT_UNSIGNED_INT_VEC3;
    if (strcmp(type, "UT_UNSIGNED_INT_VEC4") == 0) return UT_UNSIGNED_INT_VEC4;

    if (strcmp(type, "UT_FLOAT") == 0) return UT_FLOAT;
    if (strcmp(type, "UT_FLOAT_VEC2") == 0) return UT_FLOAT_VEC2;
    if (strcmp(type, "UT_FLOAT_VEC3") == 0) return UT_FLOAT_VEC3;
    if (strcmp(type, "UT_FLOAT_VEC4") == 0) return UT_FLOAT_VEC4;

    if (strcmp(type, "UT_FLOAT_MAT2") == 0) return UT_FLOAT_MAT2;
    if (strcmp(type, "UT_FLOAT_MAT3") == 0) return UT_FLOAT_MAT3;
    if (strcmp(type, "UT_FLOAT_MAT4") == 0) return UT_FLOAT_MAT4;

    if (strcmp(type, "UT_FLOAT_MAT2x3") == 0) return UT_FLOAT_MAT2x3;
    if (strcmp(type, "UT_FLOAT_MAT3x2") == 0) return UT_FLOAT_MAT3x2;
    if (strcmp(type, "UT_FLOAT_MAT2x4") == 0) return UT_FLOAT_MAT2x4;
    if (strcmp(type, "UT_FLOAT_MAT4x2") == 0) return UT_FLOAT_MAT4x2;
    if (strcmp(type, "UT_FLOAT_MAT3x4") == 0) return UT_FLOAT_MAT3x4;
    if (strcmp(type, "UT_FLOAT_MAT4x3") == 0) return UT_FLOAT_MAT4x3;

    if (strcmp(type, "UT_DOUBLE") == 0) return UT_DOUBLE;
    if (strcmp(type, "UT_DOUBLE_VEC2") == 0) return UT_DOUBLE_VEC2;
    if (strcmp(type, "UT_DOUBLE_VEC3") == 0) return UT_DOUBLE_VEC3;
    if (strcmp(type, "UT_DOUBLE_VEC4") == 0) return UT_DOUBLE_VEC4;

    if (strcmp(type, "UT_DOUBLE_MAT2") == 0) return UT_DOUBLE_MAT2;
    if (strcmp(type, "UT_DOUBLE_MAT3") == 0) return UT_DOUBLE_MAT3;
    if (strcmp(type, "UT_DOUBLE_MAT4") == 0) return UT_DOUBLE_MAT4;

    if (strcmp(type, "UT_DOUBLE_MAT2x3") == 0) return UT_DOUBLE_MAT2x3;
    if (strcmp(type, "UT_DOUBLE_MAT3x2") == 0) return UT_DOUBLE_MAT3x2;
    if (strcmp(type, "UT_DOUBLE_MAT2x4") == 0) return UT_DOUBLE_MAT2x4;
    if (strcmp(type, "UT_DOUBLE_MAT4x2") == 0) return UT_DOUBLE_MAT4x2;
    if (strcmp(type, "UT_DOUBLE_MAT3x4") == 0) return UT_DOUBLE_MAT3x4;
    if (strcmp(type, "UT_DOUBLE_MAT4x3") == 0) return UT_DOUBLE_MAT4x3;

    /*if (strcmp(type, "UT_NONE") == 0)*/ return UT_NONE;
  }

  inline const char* stringfy_EDepthTextureMode(EDepthTextureMode dtm)
  {
    switch(dtm)
    {
    default:
    case DTM_RED: return "DTM_RED";
    case DTM_LUMINANCE: return "DTM_LUMINANCE";
    case DTM_INTENSITY: return "DTM_INTENSITY";
    case DTM_ALPHA: return "DTM_ALPHA";
    }
  }

  inline EDepthTextureMode destringfy_EDepthTextureMode(const char* str)
  {
    if (strcmp(str, "DTM_LUMINANCE") == 0) return DTM_LUMINANCE;
    if (strcmp(str, "DTM_INTENSITY") == 0) return DTM_INTENSITY;
    if (strcmp(str, "DTM_ALPHA") == 0) return DTM_ALPHA;
    /*if (strcmp(str, "DTM_RED") == 0)*/ return DTM_RED;
  }

  inline const char* stringfy_ETexCompareMode(ETexCompareMode tcm)
  {
    switch(tcm)
    {
    default:
    case TCM_NONE: return "TCM_NONE";
    // case TCM_COMPARE_R_TO_TEXTURE: return "TCM_COMPARE_R_TO_TEXTURE";
    case TCM_COMPARE_REF_DEPTH_TO_TEXTURE: return "TCM_COMPARE_REF_DEPTH_TO_TEXTURE";
    }
  }

  inline ETexCompareMode destringfy_ETexCompareMode(const char* str)
  {
    if (strcmp(str, "TCM_COMPARE_R_TO_TEXTURE") == 0) return TCM_COMPARE_R_TO_TEXTURE;
    if (strcmp(str, "TCM_COMPARE_REF_DEPTH_TO_TEXTURE") == 0) return TCM_COMPARE_REF_DEPTH_TO_TEXTURE;
    /*if (strcmp(str, "TCM_NONE") == 0)*/ return TCM_NONE;
  }

  inline const char* stringfy_ETexCompareFunc(ETexCompareFunc tcf)
  {
    switch(tcf)
    {
    default:
    case TCF_LEQUAL: return "TCF_LEQUAL";
    case TCF_GEQUAL: return "TCF_GEQUAL";
    case TCF_LESS: return "TCF_LESS";
    case TCF_GREATER: return "TCF_GREATER";
    case TCF_EQUAL: return "TCF_EQUAL";
    case TCF_NOTEQUAL: return "TCF_NOTEQUAL";
    case TCF_ALWAYS: return "TCF_ALWAYS";
    case TCF_NEVER: return "TCF_NEVER";
    }
  }

  inline ETexCompareFunc destringfy_ETexCompareFunc(const char* str)
  {
    if (strcmp(str, "TCF_GEQUAL") == 0) return TCF_GEQUAL;
    if (strcmp(str, "TCF_LESS") == 0) return TCF_LESS;
    if (strcmp(str, "TCF_GREATER") == 0) return TCF_GREATER;
    if (strcmp(str, "TCF_EQUAL") == 0) return TCF_EQUAL;
    if (strcmp(str, "TCF_NOTEQUAL") == 0) return TCF_NOTEQUAL;
    if (strcmp(str, "TCF_ALWAYS") == 0) return TCF_ALWAYS;
    if (strcmp(str, "TCF_NEVER") == 0) return TCF_NEVER;
    /*if (strcmp(str, "TCF_LEQUAL") == 0)*/ return TCF_LEQUAL;
  }

  inline const char* stringfy_ETexParamFilter(ETexParamFilter tpf)
  {
    switch(tpf)
    {
    default:
    case TPF_NEAREST: return "TPF_NEAREST";
    case TPF_LINEAR: return "TPF_LINEAR";
    case TPF_NEAREST_MIPMAP_NEAREST: return "TPF_NEAREST_MIPMAP_NEAREST";
    case TPF_LINEAR_MIPMAP_NEAREST: return "TPF_LINEAR_MIPMAP_NEAREST";
    case TPF_NEAREST_MIPMAP_LINEAR: return "TPF_NEAREST_MIPMAP_LINEAR";
    case TPF_LINEAR_MIPMAP_LINEAR: return "TPF_LINEAR_MIPMAP_LINEAR";
    }
  }

  inline ETexParamFilter destringfy_ETexParamFilter(const char* str)
  {
    if (strcmp(str, "TPF_LINEAR") == 0) return TPF_LINEAR;
    if (strcmp(str, "TPF_NEAREST_MIPMAP_NEAREST") == 0) return TPF_NEAREST_MIPMAP_NEAREST;
    if (strcmp(str, "TPF_LINEAR_MIPMAP_NEAREST") == 0) return TPF_LINEAR_MIPMAP_NEAREST;
    if (strcmp(str, "TPF_NEAREST_MIPMAP_LINEAR") == 0) return TPF_NEAREST_MIPMAP_LINEAR;
    if (strcmp(str, "TPF_LINEAR_MIPMAP_LINEAR") == 0) return TPF_LINEAR_MIPMAP_LINEAR;
    /*if (strcmp(str, "TPF_NEAREST") == 0)*/ return TPF_NEAREST;
  }

  inline const char* stringfy_ETexParamWrap(ETexParamWrap tpw)
  {
    switch(tpw)
    {
    default:
    case TPW_REPEAT: return "TPW_REPEAT";
    case TPW_CLAMP: return "TPW_CLAMP";
    case TPW_CLAMP_TO_BORDER: return "TPW_CLAMP_TO_BORDER";
    case TPW_CLAMP_TO_EDGE: return "TPW_CLAMP_TO_EDGE";
    case TPW_MIRRORED_REPEAT: return "TPW_MIRRORED_REPEAT";
    }
  }

  inline ETexParamWrap destringfy_ETexParamWrap(const char* str)
  {
    if (strcmp(str, "TPW_CLAMP") == 0) return TPW_CLAMP;
    if (strcmp(str, "TPW_CLAMP_TO_BORDER") == 0) return TPW_CLAMP_TO_BORDER;
    if (strcmp(str, "TPW_CLAMP_TO_EDGE") == 0) return TPW_CLAMP_TO_EDGE;
    if (strcmp(str, "TPW_MIRRORED_REPEAT") == 0) return TPW_MIRRORED_REPEAT;
    /*if (strcmp(str, "TPW_REPEAT") == 0)*/ return TPW_REPEAT;
  }

  inline const char* stringfy_EEnable(EEnable en)
  {
    switch(en)
    {
    default:
      return "EN_UnknownEnable";
    case EN_BLEND: return "EN_BLEND";
    case EN_CULL_FACE: return "EN_CULL_FACE";
    case EN_DEPTH_TEST: return "EN_DEPTH_TEST";
    case EN_STENCIL_TEST: return "EN_STENCIL_TEST";
    case EN_DITHER: return "EN_DITHER";
    case EN_POLYGON_OFFSET_FILL: return "EN_POLYGON_OFFSET_FILL";
    case EN_POLYGON_OFFSET_LINE: return "EN_POLYGON_OFFSET_LINE";
    case EN_POLYGON_OFFSET_POINT: return "EN_POLYGON_OFFSET_POINT";
    case EN_COLOR_LOGIC_OP: return "EN_COLOR_LOGIC_OP";
    case EN_MULTISAMPLE: return "EN_MULTISAMPLE";
    case EN_POINT_SMOOTH: return "EN_POINT_SMOOTH";
    case EN_LINE_SMOOTH: return "EN_LINE_SMOOTH";
    case EN_POLYGON_SMOOTH: return "EN_POLYGON_SMOOTH";
    case EN_LINE_STIPPLE: return "EN_LINE_STIPPLE";
    case EN_POLYGON_STIPPLE: return "EN_POLYGON_STIPPLE";
    case EN_POINT_SPRITE: return "EN_POINT_SPRITE";
    case EN_PROGRAM_POINT_SIZE: return "EN_PROGRAM_POINT_SIZE";
    case EN_ALPHA_TEST: return "EN_ALPHA_TEST";
    case EN_LIGHTING: return "EN_LIGHTING";
    case EN_COLOR_SUM: return "EN_COLOR_SUM";
    case EN_FOG: return "EN_FOG";
    case EN_NORMALIZE: return "EN_NORMALIZE";
    case EN_RESCALE_NORMAL: return "EN_RESCALE_NORMAL";
    case EN_VERTEX_PROGRAM_TWO_SIDE: return "EN_VERTEX_PROGRAM_TWO_SIDE";
    case EN_TEXTURE_CUBE_MAP_SEAMLESS: return "EN_TEXTURE_CUBE_MAP_SEAMLESS";
    case EN_CLIP_DISTANCE0: return "EN_CLIP_DISTANCE0";
    case EN_CLIP_DISTANCE1: return "EN_CLIP_DISTANCE1";
    case EN_CLIP_DISTANCE2: return "EN_CLIP_DISTANCE2";
    case EN_CLIP_DISTANCE3: return "EN_CLIP_DISTANCE3";
    case EN_CLIP_DISTANCE4: return "EN_CLIP_DISTANCE4";
    case EN_CLIP_DISTANCE5: return "EN_CLIP_DISTANCE5";
    case EN_CLIP_DISTANCE6: return "EN_CLIP_DISTANCE6";
    case EN_CLIP_DISTANCE7: return "EN_CLIP_DISTANCE7";
    case EN_SAMPLE_ALPHA_TO_COVERAGE: return "EN_SAMPLE_ALPHA_TO_COVERAGE";
    case EN_SAMPLE_ALPHA_TO_ONE: return "EN_SAMPLE_ALPHA_TO_ONE";
    case EN_SAMPLE_COVERAGE: return "EN_SAMPLE_COVERAGE";
    }
  }

  inline EEnable destringfy_EEnable(const char* str)
  {
    if (strcmp(str, "EN_BLEND") == 0) return EN_BLEND;
    if (strcmp(str, "EN_CULL_FACE") == 0) return EN_CULL_FACE;
    if (strcmp(str, "EN_DEPTH_TEST") == 0) return EN_DEPTH_TEST;
    if (strcmp(str, "EN_STENCIL_TEST") == 0) return EN_STENCIL_TEST;
    if (strcmp(str, "EN_DITHER") == 0) return EN_DITHER;
    if (strcmp(str, "EN_POLYGON_OFFSET_FILL") == 0) return EN_POLYGON_OFFSET_FILL;
    if (strcmp(str, "EN_POLYGON_OFFSET_LINE") == 0) return EN_POLYGON_OFFSET_LINE;
    if (strcmp(str, "EN_POLYGON_OFFSET_POINT") == 0) return EN_POLYGON_OFFSET_POINT;
    if (strcmp(str, "EN_COLOR_LOGIC_OP") == 0) return EN_COLOR_LOGIC_OP;
    if (strcmp(str, "EN_MULTISAMPLE") == 0) return EN_MULTISAMPLE;
    if (strcmp(str, "EN_POINT_SMOOTH") == 0) return EN_POINT_SMOOTH;
    if (strcmp(str, "EN_LINE_SMOOTH") == 0) return EN_LINE_SMOOTH;
    if (strcmp(str, "EN_POLYGON_SMOOTH") == 0) return EN_POLYGON_SMOOTH;
    if (strcmp(str, "EN_LINE_STIPPLE") == 0) return EN_LINE_STIPPLE;
    if (strcmp(str, "EN_POLYGON_STIPPLE") == 0) return EN_POLYGON_STIPPLE;
    if (strcmp(str, "EN_POINT_SPRITE") == 0) return EN_POINT_SPRITE;
    if (strcmp(str, "EN_PROGRAM_POINT_SIZE") == 0) return EN_PROGRAM_POINT_SIZE;
    if (strcmp(str, "EN_ALPHA_TEST") == 0) return EN_ALPHA_TEST;
    if (strcmp(str, "EN_LIGHTING") == 0) return EN_LIGHTING;
    if (strcmp(str, "EN_COLOR_SUM") == 0) return EN_COLOR_SUM;
    if (strcmp(str, "EN_FOG") == 0) return EN_FOG;
    if (strcmp(str, "EN_NORMALIZE") == 0) return EN_NORMALIZE;
    if (strcmp(str, "EN_RESCALE_NORMAL") == 0) return EN_RESCALE_NORMAL;
    if (strcmp(str, "EN_VERTEX_PROGRAM_TWO_SIDE") == 0) return EN_VERTEX_PROGRAM_TWO_SIDE;
    if (strcmp(str, "EN_TEXTURE_CUBE_MAP_SEAMLESS") == 0) return EN_TEXTURE_CUBE_MAP_SEAMLESS;
    if (strcmp(str, "EN_CLIP_DISTANCE0") == 0) return EN_CLIP_DISTANCE0;
    if (strcmp(str, "EN_CLIP_DISTANCE1") == 0) return EN_CLIP_DISTANCE1;
    if (strcmp(str, "EN_CLIP_DISTANCE2") == 0) return EN_CLIP_DISTANCE2;
    if (strcmp(str, "EN_CLIP_DISTANCE3") == 0) return EN_CLIP_DISTANCE3;
    if (strcmp(str, "EN_CLIP_DISTANCE4") == 0) return EN_CLIP_DISTANCE4;
    if (strcmp(str, "EN_CLIP_DISTANCE5") == 0) return EN_CLIP_DISTANCE5;
    if (strcmp(str, "EN_CLIP_DISTANCE6") == 0) return EN_CLIP_DISTANCE6;
    if (strcmp(str, "EN_CLIP_DISTANCE7") == 0) return EN_CLIP_DISTANCE7;
    if (strcmp(str, "EN_SAMPLE_ALPHA_TO_COVERAGE") == 0) return EN_SAMPLE_ALPHA_TO_COVERAGE;
    if (strcmp(str, "EN_SAMPLE_ALPHA_TO_ONE") == 0) return EN_SAMPLE_ALPHA_TO_ONE;
    if (strcmp(str, "EN_SAMPLE_COVERAGE") == 0) return EN_SAMPLE_COVERAGE;

    return EN_UnknownEnable;
  }

  inline EPrimitiveType destringfy_EPrimitiveType(const std::string& str, int line_num)
  {
    if ("PT_POINTS" == str) return PT_POINTS;
    if ("PT_LINES" == str)  return PT_LINES;
    if ("PT_LINE_LOOP" == str) return PT_LINE_LOOP;
    if ("PT_LINE_STRIP" == str) return PT_LINE_STRIP;
    if ("PT_TRIANGLES" == str) return PT_TRIANGLES;
    if ("PT_TRIANGLE_STRIP" == str) return PT_TRIANGLE_STRIP;
    if ("PT_TRIANGLE_FAN" == str) return PT_TRIANGLE_FAN;
    if ("PT_QUADS" == str) return PT_QUADS;
    if ("PT_QUAD_STRIP" == str) return PT_QUAD_STRIP;
    if ("PT_POLYGON" == str) return PT_POLYGON;
    if ("PT_LINES_ADJACENCY" == str) return PT_LINES_ADJACENCY;
    if ("PT_LINE_STRIP_ADJACENCY" == str) return PT_LINE_STRIP_ADJACENCY;
    if ("PT_TRIANGLES_ADJACENCY" == str) return PT_TRIANGLES_ADJACENCY;
    if ("PT_TRIANGLE_STRIP_ADJACENCY" == str) return PT_TRIANGLES_ADJACENCY;
    if ("PT_PATCHES" == str) return PT_PATCHES;
    
    Log::error( Say("Line %n : error : unknown primitive type '%s'\n") << line_num << str);
    /*if ("PT_UNKNOWN" == str)*/ return PT_UNKNOWN;
  }

  inline VLX_Value export_AABB(const AABB& aabb)
  {
    VLX_Value value ( new VLX_Structure("<vl::AABB>") );
    *value.getStructure() << "MinCorner" << toValue(aabb.minCorner());
    *value.getStructure() << "MaxCorner" << toValue(aabb.maxCorner());
    return value;
  }

  inline AABB import_AABB(const VLX_Structure* vlx)
  {
    AABB aabb;

    VL_CHECK( vlx->tag() == "<vl::AABB>" )

    for(size_t i=0; i<vlx->value().size(); ++i)
    {
      const std::string& key = vlx->value()[i].key();
      const VLX_Value& value = vlx->value()[i].value();
      if (key == "MinCorner")
      {
        VL_CHECK(value.type() == VLX_Value::ArrayReal)
        aabb.setMinCorner( to_fvec3(value.getArrayReal()) );
      }
      else
      if (key == "MaxCorner")
      {
        VL_CHECK(value.type() == VLX_Value::ArrayReal)
        aabb.setMaxCorner( to_fvec3(value.getArrayReal()) );
      }
    }

    return aabb;
  }

  inline VLX_Value export_Sphere(const Sphere& sphere)
  {
    VLX_Value value ( new VLX_Structure("<vl::Sphere>") );
    *value.getStructure() << "Center" << toValue(sphere.center());
    *value.getStructure() << "Radius" << sphere.radius();
    return value;
  }

  inline Sphere import_Sphere(const VLX_Structure* vlx)
  {
    Sphere sphere;

    VL_CHECK( vlx->tag() == "<vl::Sphere>" )

    for(size_t i=0; i<vlx->value().size(); ++i)
    {
      const std::string& key = vlx->value()[i].key();
      const VLX_Value& value = vlx->value()[i].value();
      if (key == "Center")
      {
        VL_CHECK(value.type() == VLX_Value::ArrayReal)
        sphere.setCenter( to_fvec3(value.getArrayReal()) );
      }
      else
      if (key == "Radius")
      {
        VL_CHECK(value.type() == VLX_Value::Real)
        sphere.setRadius( (Real)value.getReal() );
      }
    }

    return sphere;
  }

  //---------------------------------------------------------------------------

  class VLX_IO: public Object
  {
  public:
    VLX_IO(): mRegistry(NULL) {}

    virtual ref<Object> importVLX(const VLX_Structure* st) = 0;

    virtual ref<VLX_Structure> exportVLX(const Object* obj) = 0;

    void setRegistry(VLX_Registry* reg) { mRegistry = reg; }

    VLX_Registry* registry() { return mRegistry; }

    VLX_Structure* do_export(const Object* obj);

    Object* do_import(const VLX_Structure* obj);

    void signalImportError(const String& str);

    void signalExportError(const String& str);

    const VLX_Registry* registry() const { return mRegistry; }

    std::string generateUID(const char* prefix);

    static std::string makeObjectTag(const Object* obj)
    {
      return std::string("<") + obj->classType()->name() + ">";
    }

  private:
    VLX_Registry* mRegistry;
  };

  //---------------------------------------------------------------------------

  class VLX_Registry: public Object
  {
  public:
    typedef enum { NoError, ImportError, ExportError } EError;

  public:
    VLX_Registry(): mVersion(100), mUIDCounter(0), mError(NoError) {}

    Object* importVLX(const VLX_Structure* st)
    {
      if (error())
        return NULL;

      Object* obj = getImportedStructure(st);
      if (obj)
        return obj;
      else
      {
        std::map< std::string, ref<VLX_IO> >::iterator it = mImportRegistry.find(st->tag());
        if (it != mImportRegistry.end())
        {
          VLX_IO* serializer = it->second.get_writable();
          VL_CHECK(serializer);
          // import structure
          ref<Object> obj = serializer->importVLX(st);
          if (!obj)
          {
            setError(ImportError);
            Log::error( Say("Error importing structure '%s'.") << st->tag() );
            VL_TRAP()
          }
          return obj.get();
        }
        else
        {
          setError(ImportError);
          Log::error( Say("No serializer found for structure '%s'.") << st->tag() );
          VL_TRAP();
          return NULL;
        }
      }
    }

    VLX_Structure* exportVLX(const Object* obj)
    {
      if (error())
        return NULL;

      VLX_Structure* st = getExportedObject(obj);
      if (st)
        return st;
      else
      {
        std::map< const TypeInfo*, ref<VLX_IO> >::iterator it = mExportRegistry.find(obj->classType());
        if (it != mExportRegistry.end())
        {
          VLX_IO* serializer = it->second.get_writable();
          VL_CHECK(serializer);
          // export object
          ref<VLX_Structure> st = serializer->exportVLX(obj);
          if (!st)
          {
            setError(ExportError);
            Log::error( Say("Error exporting '%s'.") << obj->classType()->name() );
            VL_TRAP()
          }
          return st.get();
        }
        else
        {
          setError(ExportError);
          Log::error( Say("No serializer found for '%s'.") << obj->classType()->name() );
          VL_TRAP()
          return NULL;
        }
      }
    }

    bool canExport(const Object* obj) const { return mExportRegistry.find(obj->classType()) != mExportRegistry.end(); }

    bool canImport(const VLX_Structure* st) const { return mImportRegistry.find(st->tag()) != mImportRegistry.end(); }

    void addSerializer(const TypeInfo* type, VLX_IO* serializer)
    {
      std::string tag = std::string("<") + type->name() + ">";
      mExportRegistry[type] = serializer; 
      mImportRegistry[tag]  = serializer; 
      serializer->setRegistry(this);
    }

    std::map< std::string, ref<VLX_IO> >& importRegistry() { return mImportRegistry; }
    std::map< const TypeInfo*, ref<VLX_IO> >& exportRegistry() { return mExportRegistry; }

    const std::map< std::string, ref<VLX_IO> >& importRegistry() const { return mImportRegistry; }
    const std::map< const TypeInfo*, ref<VLX_IO> >& exportRegistry() const { return mExportRegistry; }

    void registerImportedStructure(const VLX_Structure* st, Object* obj) 
    {
      VL_CHECK( mImportedStructures.find(st) == mImportedStructures.end() )
      mImportedStructures[st] = obj;
    }

    void registerExportedObject(const Object* obj, VLX_Structure* st)
    {
      VL_CHECK(mExportedObjects.find(obj) == mExportedObjects.end())
      mExportedObjects[obj] = st;
    }

    Object* getImportedStructure(const VLX_Structure* st)
    {
      std::map< ref<VLX_Structure>, ref<Object> >::iterator it = mImportedStructures.find(st);
      if (it == mImportedStructures.end())
        return NULL;
      else
      {
        VL_CHECK(it->second.get_writable() != NULL)
        return it->second.get_writable();
      }
    }

    VLX_Structure* getExportedObject(const Object* obj)
    {
      std::map< ref<Object>, ref<VLX_Structure> >::iterator it = mExportedObjects.find(obj);
      if (it == mExportedObjects.end())
        return NULL;
      else
      {
        VL_CHECK(it->second.get_writable() != NULL)
        return it->second.get_writable();
      }
    }
    
    int getNewUID() { return ++mUIDCounter; }

    int version() const { return mVersion; }
    void setVersion(int version) { mVersion = version; }

    EError error() const { return mError; }
    void setError(EError err) { mError = err; }

  private:
    EError mError;
    int mVersion;
    int mUIDCounter;
    std::map< std::string, ref<VLX_IO> > mImportRegistry;     // <tag> --> VLX_IO
    std::map< const TypeInfo*, ref<VLX_IO> > mExportRegistry; // TypeInfo --> VLX_IO

    std::map< ref<VLX_Structure>, ref<Object> > mImportedStructures; // structure --> object
    std::map< ref<Object>, ref<VLX_Structure> > mExportedObjects;    // object --> structure
  };

  //---------------------------------------------------------------------------

  inline void VLX_IO::signalImportError(const String& str) 
  { 
    // signal only the first one
    if (!registry()->error())
    {
      Log::error( str );
      registry()->setError( VLX_Registry::ImportError );
    }
  }

  inline void VLX_IO::signalExportError(const String& str)
  { 
    // signal only the first one
    if (!registry()->error())
    {
      Log::error( str );
      registry()->setError( VLX_Registry::ExportError ); 
    }
  }

  inline VLX_Structure* VLX_IO::do_export(const Object* obj) { return mRegistry->exportVLX(obj); }

  inline Object* VLX_IO::do_import(const VLX_Structure* obj) { return mRegistry->importVLX(obj); }

  inline std::string VLX_IO::generateUID(const char* prefix)
  {
    std::stringstream strstr;
    strstr << "#" << prefix << "id" << registry()->getNewUID();
    return strstr.str();
  }

  //---------------------------------------------------------------------------

  struct VLX_IO_Array: public VLX_IO
  {
    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      if (!vlx->getValue("Value"))
      {
        Log::error( Say("Line %n : error. 'Value' expected in object '%s'. \n") << vlx->lineNumber() << vlx->tag() );
        return NULL;
      }

      const VLX_Value& value = *vlx->getValue("Value");

      ref<ArrayAbstract> arr_abstract;

      if (vlx->tag() == "<vl::ArrayFloat1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_float = value.getArrayReal();
        ref<ArrayFloat1> arr_float1 = new ArrayFloat1; arr_abstract = arr_float1;
        arr_float1->resize( vlx_arr_float->value().size() );
        vlx_arr_float->copyTo((float*)arr_float1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayFloat2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_float = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_float->value().size() % 2 == 0, value)
        ref<ArrayFloat2> arr_float2 = new ArrayFloat2; arr_abstract = arr_float2;
        arr_float2->resize( vlx_arr_float->value().size() / 2 );
        vlx_arr_float->copyTo((float*)arr_float2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayFloat3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_float = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_float->value().size() % 3 == 0, value)
        ref<ArrayFloat3> arr_float3 = new ArrayFloat3; arr_abstract = arr_float3;
        arr_float3->resize( vlx_arr_float->value().size() / 3 );
        vlx_arr_float->copyTo((float*)arr_float3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayFloat4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_float = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_float->value().size() % 4 == 0, value)
        ref<ArrayFloat4> arr_float4 = new ArrayFloat4; arr_abstract = arr_float4;
        arr_float4->resize( vlx_arr_float->value().size() / 4 );
        vlx_arr_float->copyTo((float*)arr_float4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_floating = value.getArrayReal();
        ref<ArrayDouble1> arr_floating1 = new ArrayDouble1; arr_abstract = arr_floating1;
        arr_floating1->resize( vlx_arr_floating->value().size() );
        vlx_arr_floating->copyTo((double*)arr_floating1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_floating = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_floating->value().size() % 2 == 0, value)
        ref<ArrayDouble2> arr_floating2 = new ArrayDouble2; arr_abstract = arr_floating2;
        arr_floating2->resize( vlx_arr_floating->value().size() / 2 );
        vlx_arr_floating->copyTo((double*)arr_floating2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_floating = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_floating->value().size() % 3 == 0, value)
        ref<ArrayDouble3> arr_floating3 = new ArrayDouble3; arr_abstract = arr_floating3;
        arr_floating3->resize( vlx_arr_floating->value().size() / 3 );
        vlx_arr_floating->copyTo((double*)arr_floating3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayDouble4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayReal, value);
        const VLX_ArrayReal* vlx_arr_floating = value.getArrayReal();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_floating->value().size() % 4 == 0, value)
        ref<ArrayDouble4> arr_floating4 = new ArrayDouble4; arr_abstract = arr_floating4;
        arr_floating4->resize( vlx_arr_floating->value().size() / 4 );
        vlx_arr_floating->copyTo((double*)arr_floating4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        ref<ArrayInt1> arr_int1 = new ArrayInt1; arr_abstract = arr_int1;
        arr_int1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((int*)arr_int1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        ref<ArrayInt2> arr_int2 = new ArrayInt2; arr_abstract = arr_int2;
        arr_int2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((int*)arr_int2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        ref<ArrayInt3> arr_int3 = new ArrayInt3; arr_abstract = arr_int3;
        arr_int3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((int*)arr_int3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayInt4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        ref<ArrayInt4> arr_int4 = new ArrayInt4; arr_abstract = arr_int4;
        arr_int4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((int*)arr_int4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        ref<ArrayUInt1> arr_int1 = new ArrayUInt1; arr_abstract = arr_int1;
        arr_int1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((unsigned int*)arr_int1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        ref<ArrayUInt2> arr_int2 = new ArrayUInt2; arr_abstract = arr_int2;
        arr_int2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((unsigned int*)arr_int2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        ref<ArrayUInt3> arr_int3 = new ArrayUInt3; arr_abstract = arr_int3;
        arr_int3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((unsigned int*)arr_int3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUInt4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        ref<ArrayUInt4> arr_int4 = new ArrayUInt4; arr_abstract = arr_int4;
        arr_int4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((unsigned int*)arr_int4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        ref<ArrayShort1> arr_short1 = new ArrayShort1; arr_abstract = arr_short1;
        arr_short1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((short*)arr_short1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        ref<ArrayShort2> arr_short2 = new ArrayShort2; arr_abstract = arr_short2;
        arr_short2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((short*)arr_short2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        ref<ArrayShort3> arr_short3 = new ArrayShort3; arr_abstract = arr_short3;
        arr_short3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((short*)arr_short3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayShort4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        ref<ArrayShort4> arr_short4 = new ArrayShort4; arr_abstract = arr_short4;
        arr_short4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((short*)arr_short4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        ref<ArrayUShort1> arr_short1 = new ArrayUShort1; arr_abstract = arr_short1;
        arr_short1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((unsigned short*)arr_short1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        ref<ArrayUShort2> arr_short2 = new ArrayUShort2; arr_abstract = arr_short2;
        arr_short2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((unsigned short*)arr_short2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        ref<ArrayUShort3> arr_short3 = new ArrayUShort3; arr_abstract = arr_short3;
        arr_short3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((unsigned short*)arr_short3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUShort4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        ref<ArrayUShort4> arr_short4 = new ArrayUShort4; arr_abstract = arr_short4;
        arr_short4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((unsigned short*)arr_short4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        ref<ArrayByte1> arr_byte1 = new ArrayByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((char*)arr_byte1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        ref<ArrayByte2> arr_byte2 = new ArrayByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((char*)arr_byte2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        ref<ArrayByte3> arr_byte3 = new ArrayByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((char*)arr_byte3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayByte4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        ref<ArrayByte4> arr_byte4 = new ArrayByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((char*)arr_byte4->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte1>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        ref<ArrayUByte1> arr_byte1 = new ArrayUByte1; arr_abstract = arr_byte1;
        arr_byte1->resize( vlx_arr_int->value().size() );
        vlx_arr_int->copyTo((unsigned char*)arr_byte1->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte2>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 2 == 0, value)
        ref<ArrayUByte2> arr_byte2 = new ArrayUByte2; arr_abstract = arr_byte2;
        arr_byte2->resize( vlx_arr_int->value().size() / 2 );
        vlx_arr_int->copyTo((unsigned char*)arr_byte2->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte3>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 3 == 0, value)
        ref<ArrayUByte3> arr_byte3 = new ArrayUByte3; arr_abstract = arr_byte3;
        arr_byte3->resize( vlx_arr_int->value().size() / 3 );
        vlx_arr_int->copyTo((unsigned char*)arr_byte3->ptr());
      }
      else
      if (vlx->tag() == "<vl::ArrayUByte4>")
      {
        VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::ArrayInteger, value);
        const VLX_ArrayInteger* vlx_arr_int = value.getArrayInteger();
        VLX_IMPORT_CHECK_RETURN_NULL( vlx_arr_int->value().size() % 4 == 0, value)
        ref<ArrayUByte4> arr_byte4 = new ArrayUByte4; arr_abstract = arr_byte4;
        arr_byte4->resize( vlx_arr_int->value().size() / 4 );
        vlx_arr_int->copyTo((unsigned char*)arr_byte4->ptr());
      }
      else
      {
        signalImportError( Say("Line %n : unknown array '%s'.\n") << vlx->lineNumber() << vlx->tag() );
      }

      // register imported structure asap
      registry()->registerImportedStructure(vlx, arr_abstract.get());
      return arr_abstract.get();
    }

    template<typename T_Array, typename T_VLX_Array>
    ref<VLX_Structure> export_ArrayT(const Object* arr_abstract)
    {
      const T_Array* arr = arr_abstract->as<T_Array>();
      ref<VLX_Structure> st =new VLX_Structure(makeObjectTag(arr_abstract).c_str(), generateUID("array_"));
      ref<T_VLX_Array> vlx_array = new T_VLX_Array;
      if (arr->size())
      {
        vlx_array->value().resize( arr->size() * arr->glSize() );
        typename T_VLX_Array::scalar_type* dst = &vlx_array->value()[0];
        const typename T_Array::scalar_type* src = (const typename T_Array::scalar_type*)arr->begin();
        const typename T_Array::scalar_type* end = (const typename T_Array::scalar_type*)arr->end();
        for(; src<end; ++src, ++dst)
          *dst = (typename T_VLX_Array::scalar_type)*src;
      }
      st->value().push_back( VLX_Structure::Value("Value", vlx_array.get() ) );
      return st;
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      ref<VLX_Structure> vlx;
      if(obj->classType() == ArrayUInt1::Type())
        vlx = export_ArrayT<ArrayUInt1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUInt2::Type())
        vlx = export_ArrayT<ArrayUInt2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUInt3::Type())
        vlx = export_ArrayT<ArrayUInt3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUInt4::Type())
        vlx = export_ArrayT<ArrayUInt4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayInt1::Type())
        vlx = export_ArrayT<ArrayInt1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayInt2::Type())
        vlx = export_ArrayT<ArrayInt2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayInt3::Type())
        vlx = export_ArrayT<ArrayInt3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayInt4::Type())
        vlx = export_ArrayT<ArrayInt4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayUShort1::Type())
        vlx = export_ArrayT<ArrayUShort1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUShort2::Type())
        vlx = export_ArrayT<ArrayUShort2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUShort3::Type())
        vlx = export_ArrayT<ArrayUShort3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUShort4::Type())
        vlx = export_ArrayT<ArrayUShort4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayUShort1::Type())
        vlx = export_ArrayT<ArrayUShort1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUShort2::Type())
        vlx = export_ArrayT<ArrayUShort2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUShort3::Type())
        vlx = export_ArrayT<ArrayUShort3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUShort4::Type())
        vlx = export_ArrayT<ArrayUShort4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayShort1::Type())
        vlx = export_ArrayT<ArrayShort1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayShort2::Type())
        vlx = export_ArrayT<ArrayShort2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayShort3::Type())
        vlx = export_ArrayT<ArrayShort3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayShort4::Type())
        vlx = export_ArrayT<ArrayShort4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayUByte1::Type())
        vlx = export_ArrayT<ArrayUByte1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUByte2::Type())
        vlx = export_ArrayT<ArrayUByte2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUByte3::Type())
        vlx = export_ArrayT<ArrayUByte3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayUByte4::Type())
        vlx = export_ArrayT<ArrayUByte4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayByte1::Type())
        vlx = export_ArrayT<ArrayByte1, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayByte2::Type())
        vlx = export_ArrayT<ArrayByte2, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayByte3::Type())
        vlx = export_ArrayT<ArrayByte3, VLX_ArrayInteger>(obj);
      else
      if(obj->classType() == ArrayByte4::Type())
        vlx = export_ArrayT<ArrayByte4, VLX_ArrayInteger>(obj);
      else

      if(obj->classType() == ArrayFloat1::Type())
        vlx = export_ArrayT<ArrayFloat1, VLX_ArrayReal>(obj);
      else
      if(obj->classType() == ArrayFloat2::Type())
        vlx = export_ArrayT<ArrayFloat2, VLX_ArrayReal>(obj);
      else
      if(obj->classType() == ArrayFloat3::Type())
        vlx = export_ArrayT<ArrayFloat3, VLX_ArrayReal>(obj);
      else
      if(obj->classType() == ArrayFloat4::Type())
        vlx = export_ArrayT<ArrayFloat4, VLX_ArrayReal>(obj);
      else

      if(obj->classType() == ArrayDouble1::Type())
        vlx = export_ArrayT<ArrayDouble1, VLX_ArrayReal>(obj);
      else
      if(obj->classType() == ArrayDouble2::Type())
        vlx = export_ArrayT<ArrayDouble2, VLX_ArrayReal>(obj);
      else
      if(obj->classType() == ArrayDouble3::Type())
        vlx = export_ArrayT<ArrayDouble3, VLX_ArrayReal>(obj);
      else
      if(obj->classType() == ArrayDouble4::Type())
        vlx = export_ArrayT<ArrayDouble4, VLX_ArrayReal>(obj);
      else
      {
        signalExportError("Array type not supported for export.\n");
      }

      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Renderable: public VLX_IO
  {
    virtual void exportRenderable(const Renderable* ren, VLX_Structure* vlx)
    {
      *vlx << "VBOEnabled" << ren->isVBOEnabled();
      *vlx << "DisplayListEnabled" << ren->isDisplayListEnabled();
      *vlx << "AABB" << export_AABB(ren->boundingBox());
      *vlx << "Sphere" << export_Sphere(ren->boundingSphere());
    }

    virtual void importRenderable(const VLX_Structure* vlx, Renderable* ren)
    {
      const std::vector<VLX_Structure::Value>& values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        if (key == "VBOEnabled")
        {
          ren->setVBOEnabled( values[i].value().getBool() );
        }
        else
        if (key == "DisplayListEnabled")
        {
          ren->setDisplayListEnabled( values[i].value().getBool() );
        }
        else
        if (key == "AABB")
        {
          ren->setBoundingBox( import_AABB(values[i].value().getStructure()) );
        }
        else
        if (key == "Sphere")
        {
          ren->setBoundingSphere( import_Sphere(values[i].value().getStructure()) );
        }
      }
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Geometry: public VLX_IO_Renderable
  {
    void importGeometry(const VLX_Structure* vlx, Geometry* geom)
    {
      VLX_IO_Renderable::importRenderable(vlx, geom);

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();

        if (key == "VertexArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import(value.getStructure())->as<ArrayAbstract>();
          if (arr)
            geom->setVertexArray(arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "NormalArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import(value.getStructure())->as<ArrayAbstract>();
          if (arr)
            geom->setNormalArray(arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "ColorArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import(value.getStructure())->as<ArrayAbstract>();
          if (arr)
            geom->setColorArray(arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "SecondaryColorArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import(value.getStructure())->as<ArrayAbstract>();
          if (arr)
            geom->setSecondaryColorArray(arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "FogCoordArray")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import(value.getStructure())->as<ArrayAbstract>();
          if (arr)
            geom->setFogCoordArray(arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (strstr(key.c_str(), "TexCoordArray") == key.c_str())
        {
          const char* ch = key.c_str() + 13/*strlen("TexCoordArray")*/;
          int tex_unit = 0;
          for(; *ch; ++ch)
          {
            if (*ch>='0' && *ch<='9')
              tex_unit = tex_unit*10 + (*ch - '0');
            else
            {
              Log::error( Say("Line %n : error. ") << value.lineNumber() );
              Log::error( "TexCoordArray must end with a number!\n" );
              signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
            }
          }

          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import(value.getStructure())->as<ArrayAbstract>();
          if (arr)
            geom->setTexCoordArray(tex_unit, arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (strstr(key.c_str(), "VertexAttribArray") == key.c_str())
        {
          const char* ch = key.c_str() + 17/*strlen("VertexAttribArray")*/;
          int attrib_location = 0;
          for(; *ch; ++ch)
          {
            if (*ch>='0' && *ch<='9')
              attrib_location = attrib_location*10 + (*ch - '0');
            else
            {
              Log::error( Say("Line %n : error. ") << value.lineNumber() );
              Log::error( "VertexAttribArray must end with a number!\n" );
              signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
            }
          }
        
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value)
          VertexAttribInfo* info_ptr = do_import(value.getStructure())->as<VertexAttribInfo>();
          if (info_ptr)
          {
            VertexAttribInfo info = *info_ptr;
            info.setAttribLocation(attrib_location);
            geom->setVertexAttribArray(info);
          }
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "DrawCall")
        {
          VLX_IMPORT_CHECK_RETURN(value.type() == VLX_Value::Structure, value) 
          DrawCall* draw_call = do_import(value.getStructure())->as<DrawCall>();
          if (draw_call)
            geom->drawCalls()->push_back(draw_call);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Geometry> geom = new Geometry;
      // registration must be done here to avoid loops
      registry()->registerImportedStructure(vlx, geom.get());
      importGeometry(vlx, geom.get());
      return geom;
    }

    void exportGeometry(const Geometry* geom, VLX_Structure* vlx)
    {
      // Renderable
      VLX_IO_Renderable::exportRenderable(geom, vlx);

      // Geometry
      if (geom->vertexArray()) 
        *vlx << "VertexArray" << do_export(geom->vertexArray());
    
      if (geom->normalArray()) 
        *vlx << "NormalArray" << do_export(geom->normalArray());
    
      if (geom->colorArray()) 
        *vlx << "ColorArray" << do_export(geom->colorArray());
    
      if (geom->secondaryColorArray()) 
        *vlx << "SecondaryColorArray" << do_export(geom->secondaryColorArray());
    
      if (geom->fogCoordArray()) 
        *vlx << "FogCoordArray" << do_export(geom->fogCoordArray());

      for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      {
        if (geom->texCoordArray(i)) 
        {
          std::string tex_coord_array = String::printf("TexCoordArray%d", i).toStdString();
          *vlx << tex_coord_array.c_str() << do_export(geom->texCoordArray(i)); 
        }
      }

      for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      {
        if (geom->vertexAttribArray(i))
        {
          std::string vertex_attrib_array = String::printf("VertexAttribArray%d", i).toStdString();
          *vlx << vertex_attrib_array.c_str() << do_export(geom->vertexAttribArray(i));
        }
      }

      for(int i=0; i<geom->drawCalls()->size(); ++i)
        *vlx << "DrawCall" << do_export(geom->drawCalls()->at(i));
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Geometry* cast_obj = obj->as<Geometry>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("geometry_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportGeometry(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_VertexAttribInfo: public VLX_IO
  {
    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      if (vlx->tag() != "<vl::VertexAttribInfo>")
      {
        Log::error( Say("Line %n : <vl::VertexAttribInfo> expected.\n") << vlx->lineNumber() );
        return NULL;
      }

      // link the VLX to the VL object
      ref<VertexAttribInfo> info = new VertexAttribInfo;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, info.get());

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();

        if (key == "Data")
        {
          VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::Structure, value) 
          ArrayAbstract* arr = do_import( value.getStructure() )->as<ArrayAbstract>();
          if(arr)
            info->setData(arr);
          else
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
        }
        else
        if (key == "Normalize")
        {
          VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::Bool, value) 
          info->setNormalize( value.getBool() );
        }
        else
        if (key == "Interpretation")
        {
          VLX_IMPORT_CHECK_RETURN_NULL(value.type() == VLX_Value::Identifier, value) 
          if (strcmp(value.getIdentifier(), "VAI_NORMAL") == 0)
            info->setInterpretation(VAI_NORMAL);
          else
          if (strcmp(value.getIdentifier(), "VAI_INTEGER") == 0)
            info->setInterpretation(VAI_INTEGER);
          else
          if (strcmp(value.getIdentifier(), "VAI_DOUBLE") == 0)
            info->setInterpretation(VAI_DOUBLE);
          else
          {
            signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );
          }
        }
      }
    
      return info.get();
    }

    void exportVertexAttribInfo(const VertexAttribInfo* info, VLX_Structure* vlx)
    {
      *vlx << "Data" << do_export(info->data());
      *vlx << "Normalize" << info->normalize();
      std::string interpretation;
      switch(info->interpretation())
      {
      case VAI_NORMAL:  interpretation = "VAI_NORMAL";  break;
      case VAI_INTEGER: interpretation = "VAI_INTEGER"; break;
      case VAI_DOUBLE:  interpretation = "VAI_DOUBLE";  break;
      }
      *vlx << "Interpretation" << toIdentifier(interpretation);
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const VertexAttribInfo* cast_obj = obj->as<VertexAttribInfo>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("vertattrinfo_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportVertexAttribInfo(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_DrawCall: public VLX_IO
  {
    void importDrawCall(const VLX_Structure* vlx, DrawCall* draw_call)
    {
      if(draw_call->isOfType(DrawElementsBase::Type()))
      {
        DrawElementsBase* de= draw_call->as<DrawElementsBase>();

        for(size_t i=0; i<vlx->value().size(); ++i)
        {
          const std::string& key = vlx->value()[i].key();
          const VLX_Value& value = vlx->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier , value)
            de->setPrimitiveType( destringfy_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VLX_IMPORT_CHECK_RETURN( de->primitiveType() != PT_UNKNOWN , value);
          }
          else
          if( key == "Enabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool , value)
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer , value)
            de->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool , value)
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertex" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer , value)
            de->setBaseVertex( (int)value.getInteger() );
          }
          else
          if( key == "IndexBuffer" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure , value)
            ArrayAbstract* arr_abstract = do_import(value.getStructure())->as<ArrayAbstract>();
            if(!arr_abstract)
              signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );

            if ( de->isOfType(DrawElementsUInt::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == ArrayUInt1::Type(), value);
              de->as<DrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
            }
            else
            if ( de->isOfType(DrawElementsUShort::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == ArrayUShort1::Type(), value);
              de->as<DrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
            }
            else
            if ( de->isOfType(DrawElementsUByte::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == ArrayUByte1::Type(), value);
              de->as<DrawElementsUByte>()->setIndexBuffer( arr_abstract->as<ArrayUByte1>() );
            }
          }
        }
      }
      else
      if(draw_call->isOfType( MultiDrawElementsBase::Type() ))
      {
        MultiDrawElementsBase* de = draw_call->as<MultiDrawElementsBase>();

        VL_CHECK(de)
        VL_CHECK(draw_call)

        for(size_t i=0; i<vlx->value().size(); ++i)
        {
          const std::string& key = vlx->value()[i].key();
          const VLX_Value& value = vlx->value()[i].value();
          if( key == "PrimitiveType" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier , value)
            de->setPrimitiveType( destringfy_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VLX_IMPORT_CHECK_RETURN( de->primitiveType() != PT_UNKNOWN , value);
          }
          else
          if( key == "Enabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool , value)
            de->setEnabled( value.getBool() );
          }
          else
          if( key == "PrimitiveRestartEnabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool , value)
            de->setPrimitiveRestartEnabled( value.getBool() );
          }
          else
          if( key == "BaseVertices" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayInteger , value)
            de->baseVertices().resize( value.getArrayInteger()->value().size() );
            if (de->baseVertices().size())
              value.getArrayInteger()->copyTo( &de->baseVertices()[0] );
            // de->setBaseVertices( value.getArrayInt32()->value() );
          }
          else
          if( key == "CountVector" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayInteger , value)
            de->countVector().resize( value.getArrayInteger()->value().size() );
            if (de->countVector().size())
              value.getArrayInteger()->copyTo( &de->countVector()[0] );
            // de->countVector() = value.getArrayInt32()->value();
          }
          else
          if( key == "IndexBuffer" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure , value)
            ArrayAbstract* arr_abstract = do_import(value.getStructure())->as<ArrayAbstract>();
            if( !arr_abstract )
              signalImportError( Say("Line %n : import error.\n") << value.lineNumber() );

            if ( de->isOfType(MultiDrawElementsUInt::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == ArrayUInt1::Type(), value);
              de->as<MultiDrawElementsUInt>()->setIndexBuffer( arr_abstract->as<ArrayUInt1>() );
            }
            else
            if ( de->isOfType(MultiDrawElementsUShort::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == ArrayUShort1::Type(), value);
              de->as<MultiDrawElementsUShort>()->setIndexBuffer( arr_abstract->as<ArrayUShort1>() );
            }
            else
            if ( de->isOfType(MultiDrawElementsUByte::Type()) )
            {
              VLX_IMPORT_CHECK_RETURN(arr_abstract->classType() == ArrayUByte1::Type(), value);
              de->as<MultiDrawElementsUByte>()->setIndexBuffer( arr_abstract->as<ArrayUByte1>() );
            }
          }
        }

        // finalize setup
        de->computePointerVector();
        de->computeVBOPointerVector();
        if ( de->baseVertices().size() != de->countVector().size() )
          de->baseVertices().resize( de->countVector().size() );
      }
      else
      if( draw_call->isOfType( DrawArrays::Type() ) )
      {
        ref<DrawArrays> da = draw_call->as<DrawArrays>();

        for(size_t i=0; i<vlx->value().size(); ++i)
        {
          const std::string& key = vlx->value()[i].key();
          const VLX_Value& value = vlx->value()[i].value();

          if( key == "PrimitiveType" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier , value)
            da->setPrimitiveType( destringfy_EPrimitiveType( value.getIdentifier(), value.lineNumber() ) );
            VLX_IMPORT_CHECK_RETURN( da->primitiveType() != PT_UNKNOWN , value);
          }
          else
          if( key == "Enabled" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool , value)
            da->setEnabled( value.getBool() );
          }
          else
          if( key == "Instances" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer , value)
            da->setInstances( (int)value.getInteger() );
          }
          else
          if( key == "Start" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer , value)
            da->setStart( (int)value.getInteger() );
          }
          else
          if( key == "Count" )
          {
            VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer , value)
            da->setCount( (int)value.getInteger() );
          }
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<DrawCall> dc;
      if (vlx->tag() == "<vl::DrawElementsUInt>")
        dc = new DrawElementsUInt;
      else
      if (vlx->tag() == "<vl::DrawElementsUShort>")
        dc = new DrawElementsUShort;
      else
      if (vlx->tag() == "<vl::DrawElementsUByte>")
        dc = new DrawElementsUByte;
      else
      if (vlx->tag() == "<vl::MultiDrawElementsUInt>")
        dc = new MultiDrawElementsUInt;
      else
      if (vlx->tag() == "<vl::MultiDrawElementsUShort>")
        dc = new MultiDrawElementsUShort;
      else
      if (vlx->tag() == "<vl::MultiDrawElementsUByte>")
        dc = new MultiDrawElementsUByte;
      else
      if (vlx->tag() == "<vl::DrawArrays>")
        dc = new DrawArrays;
      else
        signalImportError( Say("Line %n : error. Unknown draw call.\n") << vlx->lineNumber() );
      // register imported structure asap
      registry()->registerImportedStructure(vlx, dc.get());
      importDrawCall(vlx, dc.get());
      return dc;
    }

    void exportDrawCallBase(const DrawCall* dcall, VLX_Structure* vlx)
    {
      std::string primitive_type = "PRIMITIVE_TYPE_ERROR";
      switch(dcall->primitiveType())
      {
        case PT_POINTS:                   primitive_type = "PT_POINTS"; break;
        case PT_LINES:                    primitive_type = "PT_LINES"; break;
        case PT_LINE_LOOP:                primitive_type = "PT_LINE_LOOP"; break;
        case PT_LINE_STRIP:               primitive_type = "PT_LINE_STRIP"; break;
        case PT_TRIANGLES:                primitive_type = "PT_TRIANGLES"; break;
        case PT_TRIANGLE_STRIP:           primitive_type = "PT_TRIANGLE_STRIP"; break;
        case PT_TRIANGLE_FAN:             primitive_type = "PT_TRIANGLE_FAN"; break;
        case PT_QUADS:                    primitive_type = "PT_QUADS"; break;
        case PT_QUAD_STRIP:               primitive_type = "PT_QUAD_STRIP"; break;
        case PT_POLYGON:                  primitive_type = "PT_POLYGON"; break;
        case PT_LINES_ADJACENCY:          primitive_type = "PT_LINES_ADJACENCY"; break;
        case PT_LINE_STRIP_ADJACENCY:     primitive_type = "PT_LINE_STRIP_ADJACENCY"; break;
        case PT_TRIANGLES_ADJACENCY:      primitive_type = "PT_TRIANGLES_ADJACENCY"; break;
        case PT_TRIANGLE_STRIP_ADJACENCY: primitive_type = "PT_TRIANGLE_STRIP_ADJACENCY"; break;
        case PT_PATCHES:                  primitive_type = "PT_PATCHES"; break;
        case PT_UNKNOWN:                  primitive_type = "PT_UNKNOWN"; break;
      }

      *vlx << "PrimitiveType" << toIdentifier(primitive_type);
      *vlx << "Enabled" << dcall->isEnabled();
      if (dcall->patchParameter())
        *vlx << "PatchParameter" << do_export(dcall->patchParameter());
    }

    void exportDrawCall(const DrawCall* dcall, VLX_Structure* vlx)
    {
      exportDrawCallBase(dcall, vlx);

      if (dcall->isOfType(DrawArrays::Type()))
      {
        const DrawArrays* da = dcall->as<DrawArrays>();
        *vlx << "Instances" << (long long)da->instances();
        *vlx << "Start" << (long long)da->start();
        *vlx << "Count" << (long long)da->count();
      }
      else
      if (dcall->isOfType(DrawElementsUInt::Type()))
      {
        const DrawElementsUInt* de = dcall->as<DrawElementsUInt>();
        *vlx << "Instances" << (long long)de->instances();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertex" << (long long)de->baseVertex();
        *vlx << "IndexBuffer" << do_export(de->indexBuffer());
      }
      else
      if (dcall->isOfType(DrawElementsUShort::Type()))
      {
        const DrawElementsUShort* de = dcall->as<DrawElementsUShort>();
        *vlx << "Instances" << (long long)de->instances();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertex" << (long long)de->baseVertex();
        *vlx << "IndexBuffer" << do_export(de->indexBuffer());
      }
      else
      if (dcall->isOfType(DrawElementsUByte::Type()))
      {
        const DrawElementsUByte* de = dcall->as<DrawElementsUByte>();
        *vlx << "Instances" << (long long)de->instances();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertex" << (long long)de->baseVertex();
        *vlx << "IndexBuffer" << do_export(de->indexBuffer());
      }
      else
      if (dcall->isOfType(MultiDrawElementsUInt::Type()))
      {
        const MultiDrawElementsUInt* de = dcall->as<MultiDrawElementsUInt>();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertices" << toValue(de->baseVertices());
        *vlx << "CountVector" << toValue(de->countVector());
        *vlx << "IndexBuffer" << do_export(de->indexBuffer());
      }
      else
      if (dcall->isOfType(MultiDrawElementsUShort::Type()))
      {
        const MultiDrawElementsUShort* de = dcall->as<MultiDrawElementsUShort>();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertices" << toValue(de->baseVertices());
        *vlx << "CountVector" << toValue(de->countVector());
        *vlx << "IndexBuffer" << do_export(de->indexBuffer());
      }
      else
      if (dcall->isOfType(MultiDrawElementsUByte::Type()))
      {
        const MultiDrawElementsUByte* de = dcall->as<MultiDrawElementsUByte>();
        *vlx << "PrimitiveRestartEnabled" << de->primitiveRestartEnabled();
        *vlx << "BaseVertices" << toValue(de->baseVertices());
        *vlx << "CountVector" << toValue(de->countVector());
        *vlx << "IndexBuffer" << do_export(de->indexBuffer());
      }
      else
      {
        Log::error("DrawCall type not supported for export.\n");
      }
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const DrawCall* cast_obj = obj->as<DrawCall>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("drawcall_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportDrawCall(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_PatchParameter: public VLX_IO
  {
    void importPatchParameter(const VLX_Structure* vlx, PatchParameter* pp)
    {
      std::vector<VLX_Structure::Value> values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        if (key == "PatchVertices")
        {
          pp->setPatchVertices( (int)values[i].value().getInteger() );
        }
        else
        if (key == "PatchDefaultOuterLevel")
        {
          pp->setPatchDefaultOuterLevel( to_fvec4(values[i].value().getArrayReal()) );
        }
        else
        if (key == "PatchDefaultInnerLevel")
        {
          pp->setPatchDefaultInnerLevel( to_fvec2(values[i].value().getArrayReal()) );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<PatchParameter> pp = new PatchParameter;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, pp.get());
      importPatchParameter(vlx, pp.get());
      return pp;
    }

    void exportPatchParameter(const PatchParameter* pp, VLX_Structure* vlx)
    {
      *vlx << "PatchVertices" << (long long)pp->patchVertices();
      *vlx << "PatchDefaultOuterLevel" << toValue(pp->patchDefaultOuterLevel());
      *vlx << "PatchDefaultInnerLevel" << toValue(pp->patchDefaultInnerLevel());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const PatchParameter* cast_obj = obj->as<PatchParameter>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("patchparam_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportPatchParameter(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_ResourceDatabase: public VLX_IO
  {
    void importResourceDatabase(const VLX_Structure* vlx, ResourceDatabase* resdb)
    {
      // <vl::ResourceDatabase> must have at least one "SerializerVersion" and one "Resources" keys in this order.
      if (vlx->value().size() < 2)
      {
        signalImportError("<vl::ResourceDatabase> must have at least one \"SerializerVersion\" and one \"Resources\" keys (in this order).\n");
        return;
      }
      else
      {
        if (vlx->value()[0].key() != "SerializerVersion" || vlx->value()[0].value().type() != VLX_Value::Integer)
        {
          signalImportError( Say("Line %n : no serializer version found.\n") << vlx->value()[0].value().lineNumber() );
          return;
        }
        else
        if (vlx->value()[0].value().getInteger() != VL_SERIALIZER_VERSION )
        {
          signalImportError("Unsupported serializer version.\n");
          return;
        }

        if (vlx->value()[1].key() != "Resources" || vlx->value()[1].value().type() != VLX_Value::List)
        {
          signalImportError( Say("Line %n : 'Resources' key/value expected.\n") << vlx->value()[1].value().lineNumber() );
          return;
        }
      }

      // get the list
      const VLX_List* list = vlx->value()[1].value().getList();
      for(size_t i=0; i<list->value().size(); ++i)
      {
        const VLX_Value& value = list->value()[i];

        // the member of this list must be all structures, unknown ones are silently skipped

        if (value.type() != VLX_Value::Structure)
        {
          signalImportError( Say("Line %n : structure expected.\n") << value.lineNumber() );
          return;
        }

        resdb->resources().push_back( do_import(value.getStructure()) );
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<ResourceDatabase> resdb = new ResourceDatabase;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, resdb.get());
      importResourceDatabase(vlx, resdb.get());
      return resdb;
    }

    void exportResourceDatabase(const ResourceDatabase* resdb, VLX_Structure* vlx)
    {
      *vlx << "SerializerVersion" << (long long)VL_SERIALIZER_VERSION;

      ref<VLX_List> list = new VLX_List;
      *vlx << "Resources" << new VLX_List;

      for(size_t i=0; i<resdb->resources().size(); ++i)
        *list << do_export(resdb->resources().at(i).get());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const ResourceDatabase* cast_obj = obj->as<ResourceDatabase>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("resdb_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportResourceDatabase(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Uniform: public VLX_IO
  {
    void importUniform(const VLX_Structure* vlx, Uniform* uniform)
    {
      const VLX_Value* val = vlx->getValue("Name");
      if (val)
      {
        VL_CHECK( val->type() == VLX_Value::Identifier );
        uniform->setName( val->getIdentifier() );
      }
      else
      {
        signalImportError( Say("Line %d : uniform without 'Name'.\n") << vlx->lineNumber() );
        return;
      }

      // 'Count' is optional
      int count = 1;
      val = vlx->getValue("Count");
      if (val)
      {
        VL_CHECK( val->type() == VLX_Value::Integer );
        count = (int)val->getInteger();
      }

      EUniformType type = UT_NONE;
      val = vlx->getValue("Type");
      if (val)
      {
        VL_CHECK( val->type() == VLX_Value::Identifier );
        type= destringfy_EUniformType( val->getIdentifier() );
      }
      else
      {
        signalImportError( Say("Line %d : uniform without 'Type'.\n") << vlx->lineNumber() );
        return;
      }

      val = vlx->getValue("Data");
      const VLX_ArrayReal* arr_real = NULL;
      const VLX_ArrayInteger* arr_int = NULL;
      if (!val)
      {
        signalImportError( Say("Line %d : uniform without 'Data'.\n") << vlx->lineNumber() );
        return;
      }
      else
      {
        if (val->type() == VLX_Value::ArrayReal)
          arr_real = val->getArrayReal();
        else
        if (val->type() == VLX_Value::ArrayInteger)
          arr_int = val->getArrayInteger();
      }

      std::vector<int> int_vec;
      std::vector<unsigned int> uint_vec;
      std::vector<float> float_vec;
      std::vector<double> double_vec;

      // mic fixme: gli scalar versions

      switch(type)
      {
      case UT_INT:
        int_vec.resize(count*1); if (arr_int) arr_int->copyTo(&int_vec[0]); else int_vec[0] = (int)val->getInteger();
        uniform->setUniform1i(count, &int_vec[0]);
        break; // mic fixme: test this
      case UT_INT_VEC2: 
        int_vec.resize(count*2); arr_int->copyTo(&int_vec[0]); VLX_IMPORT_CHECK_RETURN(int_vec.size() == arr_int->value().size(), *val); 
        uniform->setUniform2i(count, &int_vec[0]);
        break;
      case UT_INT_VEC3: 
        int_vec.resize(count*3); arr_int->copyTo(&int_vec[0]); VLX_IMPORT_CHECK_RETURN(int_vec.size() == arr_int->value().size(), *val); 
        uniform->setUniform3i(count, &int_vec[0]);
        break;
      case UT_INT_VEC4: 
        int_vec.resize(count*4); arr_int->copyTo(&int_vec[0]); VLX_IMPORT_CHECK_RETURN(int_vec.size() == arr_int->value().size(), *val); 
        uniform->setUniform4i(count, &int_vec[0]);
        break;

      case UT_UNSIGNED_INT:
        uint_vec.resize(count*1); if (arr_int) arr_int->copyTo(&uint_vec[0]); else uint_vec[0] = (unsigned int)val->getInteger(); 
        uniform->setUniform1ui(count, &uint_vec[0]);
        break; // mic fixme: test this
      case UT_UNSIGNED_INT_VEC2: 
        uint_vec.resize(count*2); arr_int->copyTo(&uint_vec[0]); VLX_IMPORT_CHECK_RETURN(uint_vec.size() == arr_int->value().size(), *val); 
        uniform->setUniform2ui(count, &uint_vec[0]);
        break;
      case UT_UNSIGNED_INT_VEC3: 
        uint_vec.resize(count*3); arr_int->copyTo(&uint_vec[0]); VLX_IMPORT_CHECK_RETURN(uint_vec.size() == arr_int->value().size(), *val); 
        uniform->setUniform3ui(count, &uint_vec[0]);
        break;
      case UT_UNSIGNED_INT_VEC4: 
        uint_vec.resize(count*4); arr_int->copyTo(&uint_vec[0]); VLX_IMPORT_CHECK_RETURN(uint_vec.size() == arr_int->value().size(), *val); 
        uniform->setUniform4ui(count, &uint_vec[0]);
        break;

      case UT_FLOAT: 
        float_vec.resize(count*1); if (arr_real) arr_real->copyTo(&float_vec[0]); else float_vec[0] = (float)val->getReal(); 
        uniform->setUniform1f(count, &float_vec[0]);
        break; // mic fixme: test this
      case UT_FLOAT_VEC2: 
        float_vec.resize(count*2); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniform2f(count, &float_vec[0]);
        break;
      case UT_FLOAT_VEC3: 
        float_vec.resize(count*3); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniform3f(count, &float_vec[0]);
        break;
      case UT_FLOAT_VEC4: 
        float_vec.resize(count*4); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniform4f(count, &float_vec[0]);
        break;

      case UT_FLOAT_MAT2: 
        float_vec.resize(count*2*2); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT3: 
        float_vec.resize(count*3*3); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT4: 
        float_vec.resize(count*4*4); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4f(count, &float_vec[0]);
        break;

      case UT_FLOAT_MAT2x3: 
        float_vec.resize(count*2*3); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix2x3f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT3x2: 
        float_vec.resize(count*3*2); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix3x2f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT2x4: 
        float_vec.resize(count*2*4); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix2x4f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT4x2: 
        float_vec.resize(count*4*2); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix4x2f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT3x4: 
        float_vec.resize(count*3*4); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix3x4f(count, &float_vec[0]);
        break;
      case UT_FLOAT_MAT4x3: 
        float_vec.resize(count*4*3); arr_int->copyTo(&float_vec[0]); VLX_IMPORT_CHECK_RETURN(float_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix4x3f(count, &float_vec[0]);
        break;

      case UT_DOUBLE: 
        double_vec.resize(count*1); if (arr_real) arr_real->copyTo(&double_vec[0]); else double_vec[0] = (double)val->getReal(); 
        uniform->setUniform1d(count, &double_vec[0]);
        break; // mic fixme: test this
      case UT_DOUBLE_VEC2: 
        double_vec.resize(count*2); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniform2d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_VEC3: 
        double_vec.resize(count*3); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniform3d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_VEC4:
        double_vec.resize(count*4); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniform4d(count, &double_vec[0]);
        break;

      case UT_DOUBLE_MAT2: 
        double_vec.resize(count*2*2); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix2d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT3: 
        double_vec.resize(count*3*3); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix3d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT4: 
        double_vec.resize(count*4*4); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val);
        uniform->setUniformMatrix4d(count, &double_vec[0]);
        break;

      case UT_DOUBLE_MAT2x3:
        double_vec.resize(count*2*3); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix2x3d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT3x2:
        double_vec.resize(count*3*2); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix3x2d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT2x4:
        double_vec.resize(count*2*4); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix2x4d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT4x2:
        double_vec.resize(count*4*2); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix4x2d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT3x4:
        double_vec.resize(count*3*4); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix3x4d(count, &double_vec[0]);
        break;
      case UT_DOUBLE_MAT4x3:
        double_vec.resize(count*4*3); arr_int->copyTo(&double_vec[0]); VLX_IMPORT_CHECK_RETURN(double_vec.size() == arr_real->value().size(), *val); 
        uniform->setUniformMatrix4x3d(count, &double_vec[0]);
        break;

      case UT_NONE:
        Log::warning("Error importing uniform : uninitialized uniform.\n");
        break;

      default:
        Log::warning("Error importing uniform : illegal uniform type.\n");
        break;
      }

    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Uniform> obj = new Uniform;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importUniform(vlx, obj.get());
      return obj;
    }

    void exportUniform(const Uniform* uniform, VLX_Structure* vlx)
    {
      *vlx << "Name" << toIdentifier(uniform->name());
      *vlx << "Type" << toIdentifier(stringfy_EUniformType(uniform->type()));
      *vlx << "Count" << (long long)uniform->count();

      const int count = uniform->count();
      ref<VLX_ArrayInteger> arr_int = new VLX_ArrayInteger;
      ref<VLX_ArrayReal> arr_real = new VLX_ArrayReal;

      switch(uniform->type())
      {
      case UT_INT:
        {
          if (count == 1)
            { int val = 0; uniform->getUniform(&val); *vlx << "Data" << (long long)val; break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case UT_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case UT_UNSIGNED_INT:
        {
          if (count == 1)
            { unsigned int val = 0; uniform->getUniform(&val); *vlx << "Data" << (long long)val; break; }
          else
            { arr_int->value().resize(count*1); arr_int->copyFrom( (int*)uniform->rawData() ); break; }
        }
      case UT_UNSIGNED_INT_VEC2: arr_int->value().resize(count*2); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_UNSIGNED_INT_VEC3: arr_int->value().resize(count*3); arr_int->copyFrom( (int*)uniform->rawData() ); break;
      case UT_UNSIGNED_INT_VEC4: arr_int->value().resize(count*4); arr_int->copyFrom( (int*)uniform->rawData() ); break;

      case UT_FLOAT:
        {
          if (count == 1)
            { float val = 0; uniform->getUniform(&val); *vlx << "Data" << (double)val; break; }
          else
            { arr_real->value().resize(count*1); arr_real->copyFrom( (float*)uniform->rawData() ); break; }
        }
      case UT_FLOAT_VEC2: arr_real->value().resize(count*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_VEC3: arr_real->value().resize(count*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_VEC4: arr_real->value().resize(count*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case UT_FLOAT_MAT2: arr_real->value().resize(count*2*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT3: arr_real->value().resize(count*3*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT4: arr_real->value().resize(count*4*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case UT_FLOAT_MAT2x3: arr_real->value().resize(count*2*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT3x2: arr_real->value().resize(count*3*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT2x4: arr_real->value().resize(count*2*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT4x2: arr_real->value().resize(count*4*2); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT3x4: arr_real->value().resize(count*3*4); arr_real->copyFrom( (float*)uniform->rawData() ); break;
      case UT_FLOAT_MAT4x3: arr_real->value().resize(count*4*3); arr_real->copyFrom( (float*)uniform->rawData() ); break;

      case UT_DOUBLE:
        {
          if (count == 1)
            { double val = 0; uniform->getUniform(&val); *vlx << "Data" << (double)val; break; }
          else
            { arr_real->value().resize(count*1); arr_real->copyFrom( (double*)uniform->rawData() ); break; }
        }
      case UT_DOUBLE_VEC2: arr_real->value().resize(count*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_VEC3: arr_real->value().resize(count*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_VEC4: arr_real->value().resize(count*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case UT_DOUBLE_MAT2: arr_real->value().resize(count*2*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT3: arr_real->value().resize(count*3*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT4: arr_real->value().resize(count*4*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case UT_DOUBLE_MAT2x3: arr_real->value().resize(count*2*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT3x2: arr_real->value().resize(count*3*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT2x4: arr_real->value().resize(count*2*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT4x2: arr_real->value().resize(count*4*2); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT3x4: arr_real->value().resize(count*3*4); arr_real->copyFrom( (double*)uniform->rawData() ); break;
      case UT_DOUBLE_MAT4x3: arr_real->value().resize(count*4*3); arr_real->copyFrom( (double*)uniform->rawData() ); break;

      case UT_NONE:
        Log::warning("Error exporting uniform : uninitialized uniform.\n");
        break;

      default:
        Log::warning("Error exporting uniform : illegal uniform type.\n");
        break;
      }

      if (!arr_int->value().empty())
        *vlx << "Data" << arr_int.get();
      else
      if (!arr_real->value().empty())
        *vlx << "Data" << arr_real.get();
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Uniform* cast_obj = obj->as<Uniform>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("uniform_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportUniform(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Shader: public VLX_IO
  {
    void importShader(const VLX_Structure* vlx, Shader* sh)
    {
      // enables
      const VLX_Value* enables = vlx->getValue("Enables");
      if (enables)
      {
        // mic fixme: check these errors
        VLX_IMPORT_CHECK_RETURN( enables->type() == VLX_Value::List, *enables )
        const VLX_List* list = enables->getList();
        for(size_t i=0; i<list->value().size(); ++i)
        {
          VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLX_Value::Identifier, list->value()[i] );
          EEnable en = destringfy_EEnable( list->value()[i].getIdentifier() );
          VLX_IMPORT_CHECK_RETURN( en != EN_UnknownEnable, list->value()[i] );
          sh->enable(en);
        }
      }

      // render states
      const VLX_Value* renderstates = vlx->getValue("RenderStates");
      if (renderstates)
      {
        VLX_IMPORT_CHECK_RETURN( renderstates->type() == VLX_Value::List, *renderstates )
        const VLX_List* list = renderstates->getList();
        int index = -1;
        for(size_t i=0; i<list->value().size(); ++i)
        {
          VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLX_Value::Structure || list->value()[i].type() == VLX_Value::Integer, list->value()[i] );
          if (list->value()[i].type() == VLX_Value::Integer)
          {
            VLX_IMPORT_CHECK_RETURN( index == -1, list->value()[i] );
            index = (int)list->value()[i].getInteger();
          }
          else
          {
            RenderState* renderstate = do_import( list->value()[i].getStructure() )->as<RenderState>();
            VLX_IMPORT_CHECK_RETURN( renderstate != NULL, list->value()[i] )
            VLX_IMPORT_CHECK_RETURN( index == -1 || renderstate->isOfType(RenderStateIndexed::Type()), list->value()[i] ) // mic fixme: check this
            sh->setRenderState(renderstate, index);
            // consume index in any case
            index = -1;
          }
        }
      }
      
      // uniforms
      const VLX_Value* uniforms = vlx->getValue("Uniforms");
      if (uniforms)
      {
        VLX_IMPORT_CHECK_RETURN( uniforms->type() == VLX_Value::List, *uniforms )
        const VLX_List* list = uniforms->getList();
        for(size_t i=0; i<list->value().size(); ++i)
        {
          VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLX_Value::Structure, list->value()[i] );
          Uniform* uniform = do_import( list->value()[i].getStructure() )->as<Uniform>();
          VLX_IMPORT_CHECK_RETURN( uniform != NULL, list->value()[i] )
          sh->setUniform(uniform);
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Shader> obj = new Shader;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importShader(vlx, obj.get());
      return obj;
    }

    void exportShader(const Shader* sh, VLX_Structure* vlx)
    {
      // uniforms
      VLX_Value uniforms;
      uniforms.setList( new VLX_List );
      if (sh->getUniformSet())
      {
        for(size_t i=0; i<sh->uniforms().size(); ++i)
          *uniforms.getList() << do_export(sh->uniforms()[i].get());
      }
      *vlx << "Uniforms" << uniforms;

      // enables
      ref<VLX_List> enables = new VLX_List;
      if (sh->getEnableSet() )
      {
        for(size_t i=0; i<sh->getEnableSet()->enables().size(); ++i)
          *enables << toIdentifier(stringfy_EEnable(sh->getEnableSet()->enables()[i]));
      }
      *vlx << "Enables" << enables.get();

      // renderstates
      VLX_Value renderstates;
      renderstates.setList( new VLX_List );
      if (sh->getRenderStateSet())
      {
        for(size_t i=0; i<sh->getRenderStateSet()->renderStatesCount(); ++i)
        {
          int index = sh->getRenderStateSet()->renderStates()[i].mIndex;
          if (index != -1)
            *renderstates.getList() << (long long)index;
          const RenderState* rs = sh->getRenderStateSet()->renderStates()[i].mRS.get();
          *renderstates.getList() << do_export(rs);
        }
      }
      *vlx << "RenderStates" << renderstates;
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Shader* cast_obj = obj->as<Shader>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("shader_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportShader(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_LODEvaluator: public VLX_IO
  {
    void importLODEvaluator(const VLX_Structure* vlx, LODEvaluator* obj)
    {
      if (obj->isOfType(DistanceLODEvaluator::Type()))
      {
        DistanceLODEvaluator* lod = obj->as<DistanceLODEvaluator>();
        const VLX_Value* vlx_distances = vlx->getValue("DistanceRageSet");
        VLX_IMPORT_CHECK_RETURN( vlx_distances != NULL, *vlx );
        VLX_IMPORT_CHECK_RETURN( vlx_distances->type() == VLX_Value::ArrayReal, *vlx_distances );
        const VLX_ArrayReal* arr = vlx_distances->getArrayReal();
        if (arr->value().size())
        {
          lod->distanceRangeSet().resize( arr->value().size() );
          arr->copyTo( &lod->distanceRangeSet()[0] );
        }
      }
      else
      if (obj->isOfType(PixelLODEvaluator::Type()))
      {
        PixelLODEvaluator* lod = obj->as<PixelLODEvaluator>();
        const VLX_Value* vlx_pixels = vlx->getValue("PixelRageSet");
        VLX_IMPORT_CHECK_RETURN( vlx_pixels != NULL, *vlx );
        VLX_IMPORT_CHECK_RETURN( vlx_pixels->type() == VLX_Value::ArrayReal, *vlx_pixels );
        const VLX_ArrayReal* arr = vlx_pixels->getArrayReal();
        if (arr->value().size())
        {
          lod->pixelRangeSet().resize( arr->value().size() );
          arr->copyTo( &lod->pixelRangeSet()[0] );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      if (vlx->tag() == "<vl::DistanceLODEvaluator>")
      {
        ref<LODEvaluator> obj = new DistanceLODEvaluator;
        // register imported structure asap
        registry()->registerImportedStructure(vlx, obj.get());
        importLODEvaluator(vlx, obj.get());
        return obj;
      }
      else
      if (vlx->tag() == "<vl::PixelLODEvaluator>")
      {
        ref<LODEvaluator> obj = new PixelLODEvaluator;
        // register imported structure asap
        registry()->registerImportedStructure(vlx, obj.get());
        importLODEvaluator(vlx, obj.get());
        return obj;
      }
      else
      {
        return NULL;
      }
    }

    void exportLODEvaluator(const LODEvaluator* obj, VLX_Structure* vlx)
    {
      if (obj->classType() == DistanceLODEvaluator::Type())
      {
        const DistanceLODEvaluator* lod = obj->as<DistanceLODEvaluator>();
        VLX_Value distances( new VLX_ArrayReal );
        distances.getArrayReal()->value().resize( lod->distanceRangeSet().size() );
        if (lod->distanceRangeSet().size() != 0)
          distances.getArrayReal()->copyFrom( &lod->distanceRangeSet()[0] );
        *vlx << "DistanceRageSet" << distances;
      }
      else
      if (obj->classType() == PixelLODEvaluator::Type())
      {
        const PixelLODEvaluator* lod = obj->as<PixelLODEvaluator>();
        VLX_Value pixels( new VLX_ArrayReal );
        pixels.getArrayReal()->value().resize( lod->pixelRangeSet().size() );
        if (lod->pixelRangeSet().size() != 0)
          pixels.getArrayReal()->copyFrom( &lod->pixelRangeSet()[0] );
        *vlx << "PixelRageSet" << pixels;
      }
      else
      {
        signalExportError("LODEvaluator type not supported for export.\n");
      }
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const LODEvaluator* cast_obj = obj->as<LODEvaluator>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("lodeval_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportLODEvaluator(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Effect: public VLX_IO
  {
    void importEffect(const VLX_Structure* vlx, Effect* obj)
    {
      const std::vector<VLX_Structure::Value>& values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        const VLX_Value& value = values[i].value();
        if (key == "RenderRank")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value )
          obj->setRenderRank( (int)value.getInteger() );
        }
        else
        if (key == "EnableMask")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value )
          obj->setEnableMask( (int)value.getInteger() );
        }
        else
        if (key == "ActiveLod")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value )
          obj->setActiveLod( (int)value.getInteger() );
        }
        else
        if (key == "LODEvaluator")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value )
          LODEvaluator* lod_eval = do_import( value.getStructure() )->as<LODEvaluator>();
          VLX_IMPORT_CHECK_RETURN( lod_eval, value )
          obj->setLODEvaluator(lod_eval);
        }
        else
        if (key == "Lods")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* lod_list = value.getList();
          for(size_t ilod=0; ilod< lod_list->value().size(); ++ilod)
          {
            const VLX_Value& lod_shaders = lod_list->value()[ilod];
            VLX_IMPORT_CHECK_RETURN( lod_shaders.type() == VLX_Value::List, lod_shaders )
            obj->lod(ilod) = new ShaderPasses;
            for( size_t ish=0; ish<lod_shaders.getList()->value().size(); ++ish)
            {
              const VLX_Value& vlx_sh = lod_shaders.getList()->value()[ish];
              VLX_IMPORT_CHECK_RETURN( vlx_sh.type() == VLX_Value::Structure, vlx_sh )
              Shader* shader = do_import( vlx_sh.getStructure() )->as<Shader>();
              VLX_IMPORT_CHECK_RETURN( shader, vlx_sh )
              obj->lod(ilod)->push_back( shader );
            }
          }
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Effect> obj = new Effect;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importEffect(vlx, obj.get());
      return obj;
    }

    VLX_Value export_ShaderPasses(const ShaderPasses* sh_seq)
    {
      VLX_Value value( new VLX_List(makeObjectTag(sh_seq).c_str()) );
      for(int i=0; i<sh_seq->size(); ++i)
        *value.getList() << do_export(sh_seq->at(i));
      return value;
    }

    void exportEffect(const Effect* fx, VLX_Structure* vlx)
    {
      *vlx << "RenderRank" << (long long)fx->renderRank();
      *vlx << "EnableMask" << (long long)fx->enableMask();
      *vlx << "ActiveLod" << (long long)fx->activeLod();

      if (fx->lodEvaluator())
        *vlx << "LODEvaluator" << do_export(fx->lodEvaluator());

      // shaders
      ref<VLX_List> lod_list = new VLX_List;
      for(int i=0; fx->lod(i) && i<VL_MAX_EFFECT_LOD; ++i)
        *lod_list << export_ShaderPasses(fx->lod(i).get());
      *vlx << "Lods" << lod_list.get();
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Effect* cast_obj = obj->as<Effect>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("effect_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportEffect(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Actor: public VLX_IO
  {
    void importActor(const VLX_Structure* vlx, Actor* obj)
    {
      const std::vector<VLX_Structure::Value>& values = vlx->value();
      for(size_t i=0; i<values.size(); ++i)
      {
        const std::string& key = values[i].key();
        const VLX_Value& value = values[i].value();
        if (key == "RenderRank")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value )
          obj->setRenderRank( (int)value.getInteger() );
        }
        else
        if (key == "RenderBlock")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value )
          obj->setRenderBlock( (int)value.getInteger() );
        }
        else
        if (key == "EnableMask")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value )
          obj->setEnableMask( (int)value.getInteger() );
        }
        else
        if (key == "IsOccludee")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool, value )
          obj->setOccludee( value.getBool() );
        }
        else
        if (key == "Lods")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            const VLX_Value& lod = list->value()[i];
            VLX_IMPORT_CHECK_RETURN( lod.type() == VLX_Value::Structure, lod )
            Renderable* rend = do_import( lod.getStructure() )->as<Renderable>();
            VLX_IMPORT_CHECK_RETURN( rend != NULL, lod )
            obj->setLod(i, rend);
          }
        }
        else
        if (key == "Effect")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value )
          Effect* fx = do_import(value.getStructure())->as<Effect>();
          VLX_IMPORT_CHECK_RETURN( fx != NULL, value )
          obj->setEffect(fx);
        }
        else
        if (key == "Transform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value )
          Transform* tr = do_import(value.getStructure())->as<Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->setTransform(tr);
        }
        else
        if (key == "Uniforms")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLX_Value::Structure, list->value()[i] )
            Uniform* uniform = do_import( list->value()[i].getStructure() )->as<Uniform>();
            VLX_IMPORT_CHECK_RETURN( uniform != NULL, list->value()[i] )
            obj->setUniform(uniform);
          }
        }
        // bounding volumes are not serialized, they are computed based on the geometry's bounds
        // else if (key == "AABB") {}
        // else if (key == "Sphere") {}
        else
        if (key == "LODEvaluator")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value )
          if (registry()->canImport( value.getStructure() ) )
          {
            LODEvaluator* lod = do_import( value.getStructure() )->as<LODEvaluator>();
            VLX_IMPORT_CHECK_RETURN( lod != NULL, value )
            obj->setLODEvaluator(lod);
          }
        }
        else
        if (key == "ActorEventCallbacks")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            const VLX_Value& elem = list->value()[i];
            VLX_IMPORT_CHECK_RETURN( elem.type() == VLX_Value::Structure, elem )
            if (registry()->canImport(elem.getStructure()))
            {
              ActorEventCallback* cb = do_import( elem.getStructure() )->as<ActorEventCallback>();
              VLX_IMPORT_CHECK_RETURN( cb != NULL, elem )
              obj->actorEventCallbacks()->push_back(cb);
            }
          }
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Actor> obj = new Actor;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importActor(vlx, obj.get());
      return obj;
    }

    void exportActor(const Actor* act, VLX_Structure* vlx)
    {
      *vlx << "EnableMask" << (long long)act->enableMask();
      *vlx << "RenderBlock" << (long long)act->renderBlock();
      *vlx << "RenderRank" << (long long)act->renderRank();
      *vlx << "IsOccludee" << act->isOccludee();

      VLX_Value renderables;
      renderables.setList( new VLX_List );
      for(size_t i=0; i<VL_MAX_ACTOR_LOD && act->lod(i); ++i)
        *renderables.getList() << do_export(act->lod(i));
      *vlx << "Lods" << renderables;

      // bounding volumes are not serialized, they are computed based on the geometry's bounds
      // *vlx << "AABB" << export_AABB(act->boundingBox());
      // *vlx << "Sphere" << export_Sphere(act->boundingSphere());

      if (act->effect())
        *vlx << "Effect" << do_export(act->effect());
      if (act->transform())
        *vlx << "Transform" << do_export(act->transform());

      VLX_Value uniforms;
      uniforms.setList( new VLX_List );
      for(size_t i=0; act->getUniformSet() && i<act->uniforms().size(); ++i)
        *uniforms.getList() << do_export(act->uniforms()[i].get());
      *vlx << "Uniforms" << uniforms;

      if (act->lodEvaluator())
        *vlx << "LODEvaluator" << do_export(act->lodEvaluator());

      // mic fixme:
      // Scissor: scissors might go away from the Actor

      VLX_Value callbacks;
      callbacks.setList( new VLX_List );
      for(int i=0; i<act->actorEventCallbacks()->size(); ++i)
        *callbacks.getList() << do_export(act->actorEventCallbacks()->at(i));
      *vlx << "ActorEventCallbacks" << callbacks;
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Actor* cast_obj = obj->as<Actor>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("actor_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportActor(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Camera: public VLX_IO
  {
    void importCamera(const VLX_Structure* vlx, Camera* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "ViewMatrix")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value);
          obj->setViewMatrix( to_fmat4(value.getList()) );
          // VLX_IMPORT_CHECK_RETURN( !obj->viewMatrix().isNull(), value )
        }
        else
        if (key == "ProjectionMatrix")
        {
          EProjectionMatrixType ptype = PMT_UserProjection;
          const VLX_Value* pmtype = vlx->getValue("ProjectionMatrixType");
          if ( pmtype )
          {
            VLX_IMPORT_CHECK_RETURN( pmtype->type() == VLX_Value::Identifier, *pmtype );
            ptype = destringfy_EProjectionMatrixType( pmtype->getIdentifier() );
          }

          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value);
          obj->setProjectionMatrix( to_fmat4(value.getList()), ptype );
          // VLX_IMPORT_CHECK_RETURN( !obj->projectionMatrix().isNull(), value )
        }
        else
        if (key == "Viewport")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value)
          Viewport* viewp = do_import( value.getStructure() )->as<Viewport>();
          VLX_IMPORT_CHECK_RETURN( viewp != NULL, value )
          obj->setViewport(viewp);
        }
        else
        if (key == "FOV")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setFOV( (float)value.getReal() );
        }
        else
        if (key == "NearPlane")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setNearPlane( (float)value.getReal() );
        }
        else
        if (key == "FarPlane")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setFarPlane( (float)value.getReal() );
        }
        else
        if (key == "Left")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setLeft( (float)value.getReal() );
        }
        else
        if (key == "Right")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setRight( (float)value.getReal() );
        }
        else
        if (key == "Bottom")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setBottom( (float)value.getReal() );
        }
        else
        if (key == "Top")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setTop( (float)value.getReal() );
        }
        else
        if (key == "BoundTransform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value)
          Transform* tr= do_import( value.getStructure() )->as<Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->bindTransform(tr);
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Camera> obj = new Camera;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importCamera(vlx, obj.get());
      return obj;
    }

    void exportCamera(const Camera* cam, VLX_Structure* vlx)
    {
      *vlx << "ViewMatrix" << toValue(cam->viewMatrix());
      *vlx << "ProjectionMatrix" << toValue(cam->projectionMatrix());
      *vlx << "ProjectionMatrixType" << toIdentifier(stringfy_EProjectionMatrixType(cam->projectionMatrixType()));
      *vlx << "Viewport" << do_export(cam->viewport());
      *vlx << "NearPlane" << (double)cam->nearPlane();
      *vlx << "FarPlane" << (double)cam->farPlane();
      *vlx << "FOV" << (double)cam->fov();
      *vlx << "Left" << (double)cam->left();
      *vlx << "Right" << (double)cam->right();
      *vlx << "Bottom" << (double)cam->bottom();
      *vlx << "Top" << (double)cam->top();
      if (cam->boundTransform())
        *vlx << "BoundTransfrm" << do_export(cam->boundTransform());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Camera* cast_obj = obj->as<Camera>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("camera_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportCamera(cast_obj, vlx.get());
      return vlx;
    }
  };


  //---------------------------------------------------------------------------

  struct VLX_IO_Viewport: public VLX_IO
  {
    void importViewport(const VLX_Structure* vlx, Viewport* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "ClearColor")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setClearColor( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "ClearColorInt")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayInteger, value );
          obj->setClearColorInt( to_ivec4( value.getArrayInteger() ) );
        }
        else
        if (key == "ClearColorUInt")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayInteger, value );
          obj->setClearColorUInt( to_uivec4( value.getArrayInteger() ) );
        }
        else
        if (key == "ClearDepth")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setClearDepth( (float)value.getReal() );
        }
        else
        if (key == "ClearStecil")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayInteger, value );
          obj->setClearStencil( (int)value.getInteger() );
        }
        else
        if (key == "ClearColorMode")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setClearColorMode( destringfy_EClearColorMode(value.getIdentifier()) );
        }
        else
        if (key == "ClearFlags")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setClearFlags( destringfy_EClearFlags(value.getIdentifier()) );
        }
        else
        if (key == "X")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setX( (int)value.getInteger()  );
        }
        else
        if (key == "Y")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setY( (int)value.getInteger()  );
        }
        else
        if (key == "Width")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setWidth( (int)value.getInteger()  );
        }
        else
        if (key == "Height")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setHeight( (int)value.getInteger()  );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Viewport> obj = new Viewport;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importViewport(vlx, obj.get());
      return obj;
    }

    void exportViewport(const Viewport* viewp, VLX_Structure* vlx)
    {
      *vlx << "ClearColor" << toValue(viewp->clearColor());
      *vlx << "ClearColorInt" << toValue(viewp->clearColorInt());
      *vlx << "ClearColorUInt" << toValue(viewp->clearColorUInt());
      *vlx << "ClearDepth" << (double)viewp->clearDepth();
      *vlx << "ClearStecil" << (long long)viewp->clearStencil();
      *vlx << "ClearColorMode" << toIdentifier(stringfy_EClearColorMode(viewp->clearColorMode()));
      *vlx << "ClearFlags" << toIdentifier(stringfy_EClearFlags(viewp->clearFlags()));
      *vlx << "X" << (long long)viewp->x();
      *vlx << "Y" << (long long)viewp->y();
      *vlx << "Width" << (long long)viewp->width();
      *vlx << "Height" << (long long)viewp->height();
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Viewport* cast_obj = obj->as<Viewport>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("viewport_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportViewport(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Transform: public VLX_IO
  {
    void importTransform(const VLX_Structure* vlx, Transform* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "LocalMatrix")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          obj->setLocalAndWorldMatrix( to_fmat4( value.getList() ) );
        }
        else
        // let the "Children" property take care of children binding
        /*
        if (key == "Parent")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value )
          Transform* tr = do_import( value.getStructure() )->as<Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          tr->addChild(obj);
        }
        else
        */
        if (key == "Children")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          for(size_t ich=0; ich<list->value().size(); ++ich)
          {
            VLX_IMPORT_CHECK_RETURN( list->value()[ich].type() == VLX_Value::Structure, list->value()[ich] )
            const VLX_Structure* vlx_tr = list->value()[ich].getStructure();
            Transform* child = do_import( vlx_tr )->as<Transform>();
            VLX_IMPORT_CHECK_RETURN( child != NULL, *vlx_tr )
            obj->addChild(child);
          }
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Transform> obj = new Transform;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importTransform(vlx, obj.get());
      return obj;
    }

    void exportTransform(const Transform* tr, VLX_Structure* vlx)
    {
      *vlx << "LocalMatrix" << toValue(tr->localMatrix());

      // not needed
      /*if (tr->parent())
        *vlx << "Parent" << do_export(tr->parent());*/

      VLX_Value childs;
      childs.setList( new VLX_List );
      for(size_t i=0; i<tr->childrenCount(); ++i)
        childs.getList()->value().push_back( do_export(tr->children()[i].get()) );
      *vlx << "Children" << childs;
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Transform* cast_obj = obj->as<Transform>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("transform_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportTransform(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Light: public VLX_IO
  {
    void importLight(const VLX_Structure* vlx, Light* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "Ambient")
        {
          // mic fixme: what if the user specifies ( 1 0 0 0 ) -> becomes a ArrayInteger!!!
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value )
          obj->setAmbient( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "Diffuse")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value )
          obj->setDiffuse( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "Specular")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value )
          obj->setSpecular( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "Position")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value )
          obj->setPosition( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "SpotDirection")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value )
          obj->setSpotDirection( to_fvec3( value.getArrayReal() ) );
        }
        else
        if (key == "SpotExponent")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setSpotExponent( (float)value.getReal() );
        }
        else
        if (key == "SpotCutoff")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setSpotCutoff( (float)value.getReal() );
        }
        else
        if (key == "ConstantAttenuation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setConstantAttenuation( (float)value.getReal() );
        }
        else
        if (key == "LinearAttenuation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setLinearAttenuation( (float)value.getReal() );
        }
        else
        if (key == "QuadraticAttenuation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->setQuadraticAttenuation( (float)value.getReal() );
        }
        else
        if (key == "BoundTransform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value)
          Transform* tr= do_import( value.getStructure() )->as<Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->bindTransform(tr);
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Light> obj = new Light;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importLight(vlx, obj.get());
      return obj;
    }

    void exportLight(const Light* light, VLX_Structure* vlx)
    {
      *vlx << "Ambient" << toValue(light->ambient());
      *vlx << "Diffuse" << toValue(light->diffuse());
      *vlx << "Specular" << toValue(light->specular());
      *vlx << "Position" << toValue(light->position());
      *vlx << "SpotDirection" << toValue(light->spotDirection());
      *vlx << "SpotExponent" << light->spotExponent();
      *vlx << "SpotCutoff" << light->spotCutoff();
      *vlx << "ConstantAttenuation" << light->constantAttenuation();
      *vlx << "LinearAttenuation" << light->linearAttenuation();
      *vlx << "QuadraticAttenuation" << light->quadraticAttenuation();
      if (light->boundTransform())
        *vlx << "BoundTransform" << do_export(light->boundTransform());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Light* cast_obj = obj->as<Light>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("light_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportLight(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_ClipPlane: public VLX_IO
  {
    void importClipPlane(const VLX_Structure* vlx, ClipPlane* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "PlaneNormal")
        {
          // mic fixme: what if the user specifies ( 1 0 0 0 ) -> becomes a ArrayInteger!!!
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value )
          obj->plane().setNormal( to_fvec3( value.getArrayReal() ) );
        }
        else
        if (key == "PlaneOrigin")
        {
          // mic fixme: what if the user specifies ( 1 0 0 0 ) -> becomes a ArrayInteger!!!
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value )
          obj->plane().setOrigin( (float)value.getReal() );
        }
        else
        if (key == "BoundTransform")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value)
          Transform* tr= do_import( value.getStructure() )->as<Transform>();
          VLX_IMPORT_CHECK_RETURN( tr != NULL, value )
          obj->bindTransform(tr);
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<ClipPlane> obj = new ClipPlane;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importClipPlane(vlx, obj.get());
      return obj;
    }

    void exportClipPlane(const ClipPlane* clip, VLX_Structure* vlx)
    {
      *vlx << "PlaneNormal" << toValue(clip->plane().normal());
      *vlx << "PlaneOrigin" << clip->plane().origin();
      if (clip->boundTransform())
        *vlx << "BoundTransform" << do_export(clip->boundTransform());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const ClipPlane* cast_obj = obj->as<ClipPlane>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("clipplane_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportClipPlane(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_GLSLProgram: public VLX_IO
  {
    void importGLSLProgram(const VLX_Structure* vlx, GLSLProgram* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "AttachShader")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value )
          const VLX_Structure* st = value.getStructure();
          GLSLShader* glsl_sh = do_import(st)->as<GLSLShader>();
          VLX_IMPORT_CHECK_RETURN( glsl_sh != NULL, *st )
          obj->attachShader(glsl_sh);
        }
        else
        if (key == "FragDataLocation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          VLX_IMPORT_CHECK_RETURN( list->value().size() == 2, *list )
          VLX_IMPORT_CHECK_RETURN( list->value()[0].type() == VLX_Value::Identifier, *list )
          VLX_IMPORT_CHECK_RETURN( list->value()[1].type() == VLX_Value::Integer, *list )
          const char* name = list->value()[0].getIdentifier();
          int index = (int)list->value()[1].getInteger();
          obj->bindFragDataLocation(index, name);
        }
        else
        if (key == "AttribLocation")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          VLX_IMPORT_CHECK_RETURN( list->value().size() == 2, *list )
          VLX_IMPORT_CHECK_RETURN( list->value()[0].type() == VLX_Value::Identifier, *list )
          VLX_IMPORT_CHECK_RETURN( list->value()[1].type() == VLX_Value::Integer, *list )
          const char* name = list->value()[0].getIdentifier();
          int index = (int)list->value()[1].getInteger();
          obj->addAutoAttribLocation(index, name);
        }
        else
        if (key == "Uniforms")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::List, value )
          const VLX_List* list = value.getList();
          for(size_t i=0; i<list->value().size(); ++i)
          {
            VLX_IMPORT_CHECK_RETURN( list->value()[i].type() == VLX_Value::Structure, list->value()[i] )
            Uniform* uniform = do_import( list->value()[i].getStructure() )->as<Uniform>();
            VLX_IMPORT_CHECK_RETURN( uniform != NULL, list->value()[i] )
            obj->setUniform(uniform);
          }
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<GLSLProgram> obj = new GLSLProgram;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importGLSLProgram(vlx, obj.get());
      return obj;
    }

    void exportGLSLProgram(const GLSLProgram* glsl, VLX_Structure* vlx)
    {
      // export glsl shaders
      for(int i=0; i<glsl->shaderCount(); ++i)
        *vlx << "AttachShader" << do_export(glsl->shader(i));

      // export uniforms
      VLX_Value uniforms;
      uniforms.setList( new VLX_List );
      for(size_t i=0; glsl->getUniformSet() && i<glsl->getUniformSet()->uniforms().size(); ++i)
        *uniforms.getList() << do_export(glsl->getUniformSet()->uniforms()[i].get());
      *vlx << "Uniforms" << uniforms;

      // frag data location
      for(std::map<std::string, int>::const_iterator it = glsl->fragDataLocations().begin(); it != glsl->fragDataLocations().end(); ++it)
      {
        VLX_List* location = new VLX_List;
        *location << toIdentifier(it->first); // Name
        *location << (long long)it->second;   // Location
        *vlx << "FragDataLocation" << location;
      }

      // auto attrib locations
      for(std::map<std::string, int>::const_iterator it = glsl->autoAttribLocations().begin(); it != glsl->autoAttribLocations().end(); ++it)
      {
        VLX_List* location = new VLX_List;
        *location << toIdentifier(it->first); // Name
        *location << (long long)it->second;   // Location
        *vlx << "AttribLocation" << location;
      }
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const GLSLProgram* cast_obj = obj->as<GLSLProgram>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("glslprog_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportGLSLProgram(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_GLSLShader: public VLX_IO
  {
    void importGLSLShader(const VLX_Structure* vlx, GLSLShader* obj)
    {
      const VLX_Value* path   = vlx->getValue("Path");
      const VLX_Value* source = vlx->getValue("Source");
      VLX_IMPORT_CHECK_RETURN( path != NULL || source != NULL, *vlx )
      if (path)
      {
        VLX_IMPORT_CHECK_RETURN( path->type() == VLX_Value::String, *path )
        obj->setSource(path->getString()); // this automatically loads the source and sets the path
      }
      else
      if (source)
      {
        VLX_IMPORT_CHECK_RETURN( source->type() == VLX_Value::RawtextBlock, *source )
        obj->setSource(source->getRawtextBlock()->value().c_str()); // mic fixme: check this
      }
      else
      {
        Log::warning( Say("Line %n : no source or path specified for glsl shader.\n") << vlx->lineNumber() );
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<GLSLShader> obj = NULL;
      if (vlx->tag() == "<vl::GLSLVertexShader>")
        obj = new GLSLVertexShader;
      else
      if (vlx->tag() == "<vl::GLSLFragmentShader>")
        obj = new GLSLFragmentShader;
      else
      if (vlx->tag() == "<vl::GLSLGeometryShader>")
        obj = new GLSLGeometryShader;
      else
      if (vlx->tag() == "<vl::GLSLTessControlShader>")
        obj = new GLSLTessControlShader;
      else
      if (vlx->tag() == "<vl::GLSLTessEvaluationShader>")
        obj = new GLSLTessEvaluationShader;
      else
      {
        signalImportError( Say("Line %n : shader type '%s' not supported.\n") << vlx->tag() );
        return NULL;
      }

      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importGLSLShader(vlx, obj.get());
      return obj;
    }

    void exportGLSLShader(const GLSLShader* glslsh, VLX_Structure* vlx)
    {
      if (!glslsh->path().empty())
        *vlx << "Path" << toString(glslsh->path());
      else
      if (!glslsh->source().empty())
        *vlx << "Source" << toRawtext(glslsh->source());
      else
      if (glslsh->handle())
        *vlx << "Source" << toRawtext(glslsh->getShaderSource());
      else
        *vlx << "Source" << toIdentifier("NO_SOURCE_FOUND");
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const GLSLShader* cast_obj = obj->as<GLSLShader>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("glslsh_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportGLSLShader(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_VertexAttrib: public VLX_IO
  {
    void importVertexAttrib(const VLX_Structure* vlx, VertexAttrib* obj)
    {
      const VLX_Value* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLX_Value::ArrayReal, *value )
      obj->setValue( to_fvec4( value->getArrayReal() ) );
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<VertexAttrib> obj = new VertexAttrib;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importVertexAttrib(vlx, obj.get());
      return obj;
    }

    void exportVertexAttrib(const VertexAttrib* obj, VLX_Structure* vlx)
    {
      *vlx << "Value" << toValue(obj->value());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const VertexAttrib* cast_obj = obj->as<VertexAttrib>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("vertexattrib_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportVertexAttrib(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Color: public VLX_IO
  {
    void importColor(const VLX_Structure* vlx, Color* obj)
    {
      const VLX_Value* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLX_Value::ArrayReal, *value )
      obj->setValue( to_fvec4( value->getArrayReal() ) );
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Color> obj = new Color;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importColor(vlx, obj.get());
      return obj;
    }

    void exportColor(const Color* obj, VLX_Structure* vlx)
    {
      *vlx << "Value" << toValue(obj->value());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Color* cast_obj = obj->as<Color>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("color_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportColor(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_SecondaryColor: public VLX_IO
  {
    void importSecondaryColor(const VLX_Structure* vlx, SecondaryColor* obj)
    {
      const VLX_Value* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLX_Value::ArrayReal, *value )
      obj->setValue( to_fvec3( value->getArrayReal() ) );
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<SecondaryColor> obj = new SecondaryColor;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importSecondaryColor(vlx, obj.get());
      return obj;
    }

    void exportSecondaryColor(const SecondaryColor* obj, VLX_Structure* vlx)
    {
      *vlx << "Value" << toValue(obj->value());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const SecondaryColor* cast_obj = obj->as<SecondaryColor>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("seccolor_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportSecondaryColor(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Normal: public VLX_IO
  {
    void importNormal(const VLX_Structure* vlx, Normal* obj)
    {
      const VLX_Value* value = vlx->getValue("Value");
      VLX_IMPORT_CHECK_RETURN( value->type() == VLX_Value::ArrayReal, *value )
      obj->setValue( to_fvec3( value->getArrayReal() ) );
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Normal> obj = new Normal;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importNormal(vlx, obj.get());
      return obj;
    }

    void exportNormal(const Normal* obj, VLX_Structure* vlx)
    {
      *vlx << "Value" << toValue(obj->value());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Normal* cast_obj = obj->as<Normal>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("normal_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportNormal(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Material: public VLX_IO
  {
    void importMaterial(const VLX_Structure* vlx, Material* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();
        if (key == "FrontAmbient")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setFrontAmbient( to_fvec4( value.getArrayReal() ) ); // mic fixme: what if this guy is interpreted as integer!!!
        }
        else
        if (key == "FrontDiffuse")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setFrontDiffuse( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontEmission")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setFrontEmission( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontSpecular")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setFrontSpecular( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "FrontShininess")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value );
          obj->setFrontShininess( (float)value.getReal() ); 
        }
        else
        if (key == "BackAmbient")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setBackAmbient( to_fvec4( value.getArrayReal() ) ); // mic fixme: what if this guy is interpreted as integer!!!
        }
        else
        if (key == "BackDiffuse")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setBackDiffuse( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackEmission")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setBackEmission( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackSpecular")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setBackSpecular( to_fvec4( value.getArrayReal() ) );
        }
        else
        if (key == "BackShininess")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value );
          obj->setBackShininess( (float)value.getReal() ); 
        }
        else
        if (key == "ColorMaterialEnabled")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool, value );
          obj->setColorMaterialEnabled( value.getBool() ); 
        }
      }

      // mic fixme check these all

      EColorMaterial col_mat = CM_AMBIENT_AND_DIFFUSE;
      const VLX_Value* vlx_col_mat = vlx->getValue("ColorMaterial");
      if (vlx_col_mat)
      {
        VLX_IMPORT_CHECK_RETURN( vlx_col_mat->type() == VLX_Value::Identifier, *vlx_col_mat );
        col_mat = destringfy_EColorMaterial( vlx_col_mat->getIdentifier() );
      }

      EPolygonFace poly_face = PF_FRONT_AND_BACK;
      const VLX_Value* vlx_poly_mat = vlx->getValue("ColorMaterialFace");
      if (vlx_poly_mat)
      {
        VLX_IMPORT_CHECK_RETURN( vlx_poly_mat->type() == VLX_Value::Identifier, *vlx_poly_mat );
        poly_face = destringfy_EPolygonFace( vlx_poly_mat->getIdentifier() );
      }

      obj->setColorMaterial( poly_face, col_mat );
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Material> obj = new Material;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importMaterial(vlx, obj.get());
      return obj;
    }

    void exportMaterial(const Material* mat, VLX_Structure* vlx)
    {
      *vlx << "FrontAmbient" << toValue(mat->frontAmbient());
      *vlx << "FrontDiffuse" << toValue(mat->frontDiffuse());
      *vlx << "FrontEmission" << toValue(mat->frontEmission());
      *vlx << "FrontSpecular" << toValue(mat->frontSpecular());
      *vlx << "FrontShininess" << (double)mat->frontShininess();

      *vlx << "BackAmbient" << toValue(mat->backAmbient());
      *vlx << "BackDiffuse" << toValue(mat->backDiffuse());
      *vlx << "BackEmission" << toValue(mat->backEmission());
      *vlx << "BackSpecular" << toValue(mat->backSpecular());
      *vlx << "BackShininess" << (double)mat->backShininess();

      *vlx << "ColorMaterial" << toIdentifier(stringfy_EColorMaterial(mat->colorMaterial()));
      *vlx << "ColorMaterialFace" << toIdentifier(stringfy_EPolygonFace(mat->colorMaterialFace()));

      *vlx << "ColorMaterialEnabled" << mat->colorMaterialEnabled();
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Material* cast_obj = obj->as<Material>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("material_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportMaterial(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_ActorEventCallback: public VLX_IO
  {
    void importActorEventCallback(const VLX_Structure* vlx, ActorEventCallback* obj)
    {
      if (obj->isOfType(DepthSortCallback::Type()))
      {
        ESortMode sm = SM_SortBackToFront;
        const VLX_Value* vlx_sm = vlx->getValue("SortMode");
        VLX_IMPORT_CHECK_RETURN( vlx_sm->type() == VLX_Value::Identifier, *vlx_sm )
        if (vlx_sm)
        {
          if ( strcmp(vlx_sm->getIdentifier(), "SM_SortBackToFront") == 0 )
            sm = SM_SortBackToFront;
          else
          if ( strcmp(vlx_sm->getIdentifier(), "SM_SortFrontToBack") == 0 )
            sm = SM_SortFrontToBack;
          else
            signalImportError( Say("Line %n : unknown sort mode '%s'.\n") << vlx_sm->lineNumber() << vlx_sm->getIdentifier() );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<ActorEventCallback> obj = NULL;

      if (vlx->tag() == "<vl::DepthSortCallback>") // mic fixme: support more stuff
        obj = new DepthSortCallback;
      else
      {
        signalImportError( Say("Line %n : ActorEventCallback type not supported for import.\n") << vlx->lineNumber() );
        return NULL;
      }

      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importActorEventCallback(vlx, obj.get());
      return obj;
    }

    void exportActorEventCallback(const ActorEventCallback* cb, VLX_Structure* vlx)
    {
      if (cb->classType() == DepthSortCallback::Type())
      {
        const DepthSortCallback* dsc = cb->as<DepthSortCallback>();

        if (dsc->sortMode() == SM_SortBackToFront)
          *vlx << "SortMode" << toIdentifier("SM_SortBackToFront");
        else
          *vlx << "SortMode" << toIdentifier("SM_SortFrontToBack");
      }
      else
      {
        signalExportError("ActorEventCallback type not supported for export.\n");
      }
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const ActorEventCallback* cast_obj = obj->as<ActorEventCallback>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("actorcallback_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportActorEventCallback(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_Texture: public VLX_IO
  {
    void importTexture(const VLX_Structure* vlx, Texture* obj)
    {
      obj->setSetupParams( new Texture::SetupParams );

      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();

        // mic fixme: document how these guys are to be used by examples

        if (key == "TexParameter")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Structure, value );
          TexParameter* tex_param = do_import( value.getStructure() )->as<TexParameter>();
          VLX_IMPORT_CHECK_RETURN( tex_param != NULL, value );
          // copy the content over
          *obj->getTexParameter() = *tex_param;
        }
        else
        if (key == "ImagePath")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::String, value );
          obj->setupParams()->setImagePath( value.getString() );
        }
        else
        if (key == "Format")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setupParams()->setFormat( destringfy_ETextureFormat(value.getIdentifier()) );
        }
        else
        if (key == "Width")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setupParams()->setWidth( (int)value.getInteger() );
        }
        else
        if (key == "Height")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setupParams()->setHeight( (int)value.getInteger() );
        }
        else
        if (key == "Depth")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setupParams()->setDepth( (int)value.getInteger() );
        }
        else
        if (key == "GenMipmaps")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool, value );
          obj->setupParams()->setGenMipmaps( (int)value.getBool() );
        }
        else
        if (key == "Samples")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Integer, value );
          obj->setupParams()->setSamples( (int)value.getInteger() );
        }
        else
        if (key == "FixedSamplesLocations")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool, value );
          obj->setupParams()->setFixedSamplesLocations( (int)value.getBool() );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<Texture> obj = new Texture;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importTexture(vlx, obj.get());
      return obj;
    }

    void exportTexture(const Texture* tex, VLX_Structure* vlx)
    {
      // mic fixme:
      // - we should allow patterns such as initializing a texture from an image filled by a shader or procedure.
      // - we should allow avoid loading twice the same image or shader source or any externa resource etc. time for a resource manager?

      if (tex->getTexParameter())
        *vlx << "TexParameter" << do_export(tex->getTexParameter());

      if (tex->setupParams())
      {
        const Texture::SetupParams* par = tex->setupParams();

        if (!par->imagePath().empty())
          *vlx << "ImagePath" << toString(par->imagePath().toStdString());
        else
        if (par->image())
          *vlx << "ImagePath" << toString(par->image()->filePath().toStdString());

        *vlx << "Format" << toIdentifier(stringfy_ETextureFormat(par->format()));

        if (par->width())
          *vlx << "Width" << (long long)par->width();

        if (par->height())
          *vlx << "Height" << (long long)par->height();

        if (par->depth())
          *vlx << "Depth" << (long long)par->depth();

        *vlx << "GenMipmaps" << par->genMipmaps();

        if(par->samples())
        {
          *vlx << "Samples" << (long long)par->samples();
          *vlx << "FixedSamplesLocations" << par->fixedSamplesLocations();
        }
      }
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const Texture* cast_obj = obj->as<Texture>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("texture_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportTexture(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_TexParameter: public VLX_IO
  {
    void importTexParameter(const VLX_Structure* vlx, TexParameter* obj)
    {
      for(size_t i=0; i<vlx->value().size(); ++i)
      {
        const std::string& key = vlx->value()[i].key();
        const VLX_Value& value = vlx->value()[i].value();

        if (key == "MinFilter")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setMinFilter( destringfy_ETexParamFilter( value.getIdentifier() ) );
        }
        else
        if (key == "MagFilter")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setMagFilter( destringfy_ETexParamFilter( value.getIdentifier() ) );
        }
        else
        if (key == "WrapS")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setWrapS( destringfy_ETexParamWrap( value.getIdentifier() ) );
        }
        else
        if (key == "WrapT")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setWrapT( destringfy_ETexParamWrap( value.getIdentifier() ) );
        }
        else
        if (key == "WrapR")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setWrapR( destringfy_ETexParamWrap( value.getIdentifier() ) );
        }
        else
        if (key == "CompareMode")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setCompareMode( destringfy_ETexCompareMode( value.getIdentifier() ) );
        }
        else
        if (key == "CompareFunc")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setCompareFunc( destringfy_ETexCompareFunc( value.getIdentifier() ) );
        }
        else
        if (key == "DepthTextureMode")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Identifier, value );
          obj->setDepthTextureMode( destringfy_EDepthTextureMode( value.getIdentifier() ) );
        }
        else
        if (key == "BorderColor")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::ArrayReal, value );
          obj->setBorderColor( to_fvec4(value.getArrayReal()) );
        }
        else
        if (key == "Anisotropy")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Real, value );
          obj->setAnisotropy( (float)value.getReal() );
        }
        else
        if (key == "GenerateMipmap")
        {
          VLX_IMPORT_CHECK_RETURN( value.type() == VLX_Value::Bool, value );
          obj->setGenerateMipmap( value.getBool() );
        }
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<TexParameter> obj = new TexParameter;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importTexParameter(vlx, obj.get());
      return obj;
    }

    void exportTexParameter(const TexParameter* texparam, VLX_Structure* vlx)
    {
      *vlx << "MinFilter" << toIdentifier(stringfy_ETexParamFilter(texparam->minFilter()));
      *vlx << "MagFilter" << toIdentifier(stringfy_ETexParamFilter(texparam->magFilter()));
      *vlx << "WrapS" << toIdentifier(stringfy_ETexParamWrap(texparam->wrapS()));
      *vlx << "WrapT" << toIdentifier(stringfy_ETexParamWrap(texparam->wrapT()));
      *vlx << "WrapR" << toIdentifier(stringfy_ETexParamWrap(texparam->wrapR()));
      *vlx << "CompareMode" << toIdentifier(stringfy_ETexCompareMode(texparam->compareMode()));
      *vlx << "CompareFunc" << toIdentifier(stringfy_ETexCompareFunc(texparam->compareFunc()));
      *vlx << "DepthTextureMode" << toIdentifier(stringfy_EDepthTextureMode(texparam->depthTextureMode()));
      *vlx << "BorderColor" << toValue(texparam->borderColor());
      *vlx << "Anisotropy" << texparam->anisotropy();
      *vlx << "GenerateMipmap" << texparam->generateMipmap();
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const TexParameter* cast_obj = obj->as<TexParameter>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("texparam_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportTexParameter(cast_obj, vlx.get());
      return vlx;
    }
  };

  //---------------------------------------------------------------------------

  struct VLX_IO_TextureSampler: public VLX_IO
  {
    void importTextureSampler(const VLX_Structure* vlx, TextureSampler* obj)
    {
      const VLX_Value* vlx_texture = vlx->getValue("Texture");
      if (vlx_texture)
      {
        VLX_IMPORT_CHECK_RETURN(vlx_texture->type() == VLX_Value::Structure, *vlx_texture);
        Texture* texture = do_import(vlx_texture->getStructure())->as<Texture>();
        VLX_IMPORT_CHECK_RETURN( texture != NULL , *vlx_texture);
        obj->setTexture(texture);
      }

      const VLX_Value* vlx_texp = vlx->getValue("TexParameter");
      if (vlx_texp)
      {
        VLX_IMPORT_CHECK_RETURN(vlx_texp->type() == VLX_Value::Structure, *vlx_texp);
        TexParameter* texp = do_import(vlx_texp->getStructure())->as<TexParameter>();
        VLX_IMPORT_CHECK_RETURN( texp != NULL , *vlx_texp);
        obj->setTexParameter(texp);
      }
    }

    virtual ref<Object> importVLX(const VLX_Structure* vlx)
    {
      ref<TextureSampler> obj = new TextureSampler;
      // register imported structure asap
      registry()->registerImportedStructure(vlx, obj.get());
      importTextureSampler(vlx, obj.get());
      return obj;
    }

    void exportTextureSampler(const TextureSampler* tex_sampler, VLX_Structure* vlx)
    {
      if (tex_sampler->texture())
        *vlx << "Texture" << do_export(tex_sampler->texture());
      if (tex_sampler->getTexParameter())
        *vlx << "TexParameter" << do_export(tex_sampler->getTexParameter());
    }

    virtual ref<VLX_Structure> exportVLX(const Object* obj)
    {
      const TextureSampler* cast_obj = obj->as<TextureSampler>(); VL_CHECK(cast_obj)
      ref<VLX_Structure> vlx = new VLX_Structure(makeObjectTag(obj).c_str(), generateUID("texsampler_"));
      // register exported object asap
      registry()->registerExportedObject(obj, vlx.get());
      exportTextureSampler(cast_obj, vlx.get());
      return vlx;
    }
  };

  //-----------------------------------------------------------------------------
  // IMPORTER
  //-----------------------------------------------------------------------------

  class VLX_Importer
  {
  public:
    typedef enum
    {
      NoError,
      ImportError
    } EError;

    VLX_Importer()
    {
      mError = NoError;
    }

    EError error() const { return mError; }

    void resetError() { mError = NoError; }

    //-----------------------------------------------------------------------------

    void registerImportedStructure(const VLX_Structure* vlx, const Object* vl_obj)
    {
      VL_CHECK( mVLX_To_VL.find(vlx) == mVLX_To_VL.end() );
      mVLX_To_VL[vlx] = vl_obj;
    }

    Object* vlxToVL(const VLX_Structure* vlx)
    {
      std::map< ref<VLX_Structure>, ref<Object> >::iterator it = mVLX_To_VL.find(vlx);
      if (it != mVLX_To_VL.end())
        return it->second.get();
      else
        return NULL;
    }

    const std::map< ref<VLX_Structure>, ref<Object> >& vlxToVL() const { return mVLX_To_VL; }

    //-----------------------------------------------------------------------------

    ref<ResourceDatabase> importFromText(VirtualFile* file)
    {
      VLX_Parser parser;
      parser.tokenizer()->setInputFile( file );

      if (!parser.parse())
        return NULL;

      if (!parser.link())
        return NULL;

      ref<ResourceDatabase> res_db /*= import_ResourceDatabase( parser.root() )*/; // mic fixme
      return res_db;
    }

  private:
    std::map< ref<VLX_Structure>, ref<Object> > mVLX_To_VL;
    EError mError;
  };

  //-----------------------------------------------------------------------------
  // EXPORT
  //-----------------------------------------------------------------------------

  class VLX_Exporter
  {
  public:
    typedef enum
    {
      NoError,
      ExportError,
    } EError;

    VLX_Exporter()
    {
      mError = NoError;
      mUIDCounter = 0;
    }

    EError error() const { return mError; }

    void resetError() { mError = NoError; }

    //-----------------------------------------------------------------------------

    std::string exportToText(const ResourceDatabase* res_db)
    {
      VLX_Value value /*= export_ResourceDatabase(res_db)*/; // mic fixme

      // do not link!

      // collect the UIDs that need to be visible
      std::map< std::string, int > uid_set;
      VLX_UIDCollectorVisitor uid_collector;
      uid_collector.setUIDSet(&uid_set);
      value.getStructure()->acceptVisitor(&uid_collector);

      VLX_TextExportVisitor text_export_visitor;
      text_export_visitor.setUIDSet(&uid_set);
      value.getStructure()->acceptVisitor(&text_export_visitor);

      return text_export_visitor.text();
    }

  protected:
    std::map< ref<Object>, ref<VLX_Structure> > mVL_To_VLX;
    int mUIDCounter;
    EError mError;
  };
//-----------------------------------------------------------------------------
  VLGRAPHICS_EXPORT ref<ResourceDatabase> loadVLX(VirtualFile* file);
  VLGRAPHICS_EXPORT ref<ResourceDatabase> loadVLX(const String& path);
  VLGRAPHICS_EXPORT bool writeVLX(VirtualFile* file, const ResourceDatabase*);
  VLGRAPHICS_EXPORT bool writeVLX(const String& file, const ResourceDatabase*);
//---------------------------------------------------------------------------
// LoadWriterVLX
//---------------------------------------------------------------------------
  /**
   * The LoadWriterVLX class is a ResourceLoadWriter capable of reading Visualization Library's VLX files.
   */
  class LoadWriterVLX: public ResourceLoadWriter
  {
    VL_INSTRUMENT_CLASS(vl::LoadWriterVLX, ResourceLoadWriter)

  public:
    LoadWriterVLX(): ResourceLoadWriter("|vlx|", "|vlx|") {}

    ref<ResourceDatabase> loadResource(const String& path) const 
    {
      return loadVLX(path);
    }

    ref<ResourceDatabase> loadResource(VirtualFile* file) const
    {
      return loadVLX(file);
    }

    bool writeResource(const String& path, ResourceDatabase* res_db) const
    {
      return writeVLX(path, res_db);
    }

    bool writeResource(VirtualFile* file, ResourceDatabase* res_db) const
    {
      return writeVLX(file, res_db);
    }
  };
//-----------------------------------------------------------------------------
}

#undef VLX_IMPORT_CHECK_RETURN
#undef VLX_IMPORT_CHECK_RETURN_NULL

#endif
