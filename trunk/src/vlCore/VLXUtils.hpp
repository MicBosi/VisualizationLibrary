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

#ifndef VLXUtils_INCLUDE_ONCE
#define VLXUtils_INCLUDE_ONCE

#include <vlCore/VLXValue.hpp>
#include <vlCore/Matrix4.hpp>
#include <vlCore/Vector4.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>

namespace vl
{
  inline std::string vlx_makeTag(const Object* obj) { return std::string("<") + obj->classType()->name() + ">"; }

  inline VLXValue vlx_Identifier(const std::string& str) { return VLXValue(str.c_str(), VLXValue::Identifier); }

  inline VLXValue vlx_ID(const std::string& str)        { return VLXValue(str.c_str(), VLXValue::ID); }

  inline VLXValue vlx_String(const std::string& str)     { return VLXValue(str.c_str(), VLXValue::String); }

  inline VLXValue vlx_Rawtext(const std::string& str)    { return VLXValue( new VLXRawtextBlock(NULL, str.c_str()) ); }

  inline fvec2 vlx_fvec2(const VLXArrayReal* arr) { VL_CHECK(arr->value().size() == 2); fvec2 v; arr->copyTo(v.ptr()); return v;  }

  inline fvec3 vlx_fvec3(const VLXArrayReal* arr) { VL_CHECK(arr->value().size() == 3); fvec3 v; arr->copyTo(v.ptr()); return v;  }

  inline fvec4 vlx_fvec4(const VLXArrayReal* arr) { VL_CHECK(arr->value().size() == 4); fvec4 v; arr->copyTo(v.ptr()); return v;  }

  inline ivec4 vlx_ivec4(const VLXArrayInteger* arr) { VL_CHECK(arr->value().size() == 4); ivec4 v; arr->copyTo(v.ptr()); return v; }

  inline uvec4 vlx_uivec4(const VLXArrayInteger* arr) { VL_CHECK(arr->value().size() == 4); uvec4 v; arr->copyTo(v.ptr()); return v; }

  inline VLXValue vlx_toValue(const std::vector<int>& vec)
  {
    VLXValue value;
    value.setArray( new VLXArrayInteger );
    value.getArrayInteger()->value().resize( vec.size() );
    if (vec.size())
      value.getArrayInteger()->copyFrom(&vec[0]);
    return value;
  }

  inline VLXValue vlx_toValue(const vec4& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLXValue vlx_toValue(const ivec4& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLXValue vlx_toValue(const uvec4& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLXValue vlx_toValue(const vec3& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(3);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    return val;
  }

  inline VLXValue vlx_toValue(const vec2& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(2);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    return val;
  }

  inline bool vlx_isTranslation(const fmat4& mat)
  {
    fmat4 tmp = mat;
    tmp.setT( fvec3(0,0,0) );
    return tmp.isIdentity();
  }

  inline bool vlx_isScaling(const fmat4& mat)
  {
    fmat4 tmp = mat;
    tmp.e(0,0) = 1;
    tmp.e(1,1) = 1;
    tmp.e(2,2) = 1;
    return tmp.isIdentity();
  }

  inline VLXValue vlx_toValue(const fmat4& mat)
  {
    VLXValue matrix_list( new VLXList );

    if (vlx_isTranslation(mat))
    {
      VLXValue value( new VLXArrayReal("<Translate>") );
      value.getArrayReal()->value().resize(3);
      value.getArrayReal()->value()[0] = mat.getT().x();
      value.getArrayReal()->value()[1] = mat.getT().y();
      value.getArrayReal()->value()[2] = mat.getT().z();
      matrix_list.getList()->value().push_back( value );
    }
    else
    if (vlx_isScaling(mat))
    {
      VLXValue value( new VLXArrayReal("<Scale>") );
      value.getArrayReal()->value().resize(3);
      value.getArrayReal()->value()[0] = mat.e(0,0);
      value.getArrayReal()->value()[1] = mat.e(1,1);
      value.getArrayReal()->value()[2] = mat.e(2,2);
      matrix_list.getList()->value().push_back( value );
    }
    else
    {
      VLXValue value( new VLXArrayReal("<Matrix>") );
      value.getArrayReal()->value().resize(4*4);
      // if we transpose this we have to transpose also the uniform matrices
      value.getArrayReal()->copyFrom(mat.ptr());
      matrix_list.getList()->value().push_back( value );
    }

    return matrix_list;
  }

  inline fmat4 vlx_fmat4( const VLXArrayReal* arr )
  {
    fmat4 mat;
    arr->copyTo(mat.ptr());
    return mat;
  }

  inline fmat4 vlx_fmat4( const VLXList* list )
  {
    fmat4 mat;

    for(size_t i=0; i<list->value().size(); ++i)
    {
      const VLXValue& value = list->value()[i];
      if (value.type() != VLXValue::ArrayReal)
      {
        Log::error( Say("Line %n : parse error during matrix import.\n") << value.lineNumber() );
        return mat4::getNull();
      }
      // composition of subtransforms is made by post multiplication like for COLLADA.
      const VLXArrayReal* arr = value.getArrayReal();
      if (arr->tag() == "<Translate>")
      {
        fvec3 tr = vlx_fvec3( arr );
        mat = mat * fmat4::getTranslation(tr);
      }
      else
      if (arr->tag() == "<Scale>")
      {
        fvec3 sc = vlx_fvec3( arr );
        mat = mat * fmat4::getScaling(sc);
      }
      else
      if (arr->tag() == "<Matrix>")
      {
        fmat4 m = vlx_fmat4( arr );
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

  inline const char* vlx_EProjectionMatrixType(EProjectionMatrixType pt)
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

  inline EProjectionMatrixType vlx_EProjectionMatrixType(const char* str)
  {
    if (strcmp(str, "PMT_OrthographicProjection") == 0) return PMT_OrthographicProjection;
    if (strcmp(str, "PMT_PerspectiveProjection") == 0) return PMT_PerspectiveProjection;
    if (strcmp(str, "PMT_PerspectiveProjectionFrustum") == 0) return PMT_PerspectiveProjectionFrustum;
    /*if (strcmp(str, "PMT_UserProjection") == 0)*/ return PMT_UserProjection;
  }

  inline const char* vlx_EClearColorMode(EClearColorMode ccm)
  {
    switch(ccm)
    {
    default:
    case CCM_Float: return "CCM_Float";
    case CCM_Int: return "CCM_Int";
    case CCM_UInt: return "CCM_UInt";
    }
  }

  inline EClearColorMode vlx_EClearColorMode(const char* str)
  {
    if (strcmp(str, "CCM_Int") == 0) return CCM_Int;
    if (strcmp(str, "CCM_UInt") == 0) return CCM_UInt;
    /*if (strcmp(str, "CCM_Float") == 0)*/ return CCM_Float;
  }

  inline const char* vlx_EClearFlags(EClearFlags cf)
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

  inline EClearFlags vlx_EClearFlags(const char* str)
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

  inline const char* vlx_EPolygonFace(EPolygonFace pf)
  {
    switch(pf)
    {
    default:
    case PF_FRONT_AND_BACK: return "PF_FRONT_AND_BACK";
    case PF_FRONT: return "PF_FRONT";
    case PF_BACK:  return "PF_BACK";
    }
  }

  inline EPolygonFace vlx_EPolygonFace(const char* str)
  {
    if (strcmp(str, "PF_FRONT") == 0) return PF_FRONT;
    if (strcmp(str, "PF_BACK") == 0) return PF_BACK;
    /*if (strcmp(str, "PF_FRONT_AND_BACK") == 0)*/ return PF_FRONT_AND_BACK;
  }

  inline const char* vlx_EColorMaterial(EColorMaterial cm)
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

  inline EColorMaterial vlx_EColorMaterial(const char* str)
  {
    if (strcmp(str, "CM_EMISSION") == 0) return CM_EMISSION;
    if (strcmp(str, "CM_AMBIENT") == 0) return CM_AMBIENT;
    if (strcmp(str, "CM_DIFFUSE") == 0) return CM_DIFFUSE;
    if (strcmp(str, "CM_SPECULAR") == 0) return CM_SPECULAR;
    /*if (strcmp(str, "CM_AMBIENT_AND_DIFFUSE") == 0) */return CM_AMBIENT_AND_DIFFUSE;
  }

  inline const char* vlx_ETextureFormat(ETextureFormat tf)
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

  inline ETextureFormat vlx_ETextureFormat(const char* str)
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

  inline const char* vlx_EUniformType(EUniformType type)
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

  inline EUniformType vlx_EUniformType(const char* type)
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

  inline const char* vlx_EDepthTextureMode(EDepthTextureMode dtm)
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

  inline EDepthTextureMode vlx_EDepthTextureMode(const char* str)
  {
    if (strcmp(str, "DTM_LUMINANCE") == 0) return DTM_LUMINANCE;
    if (strcmp(str, "DTM_INTENSITY") == 0) return DTM_INTENSITY;
    if (strcmp(str, "DTM_ALPHA") == 0) return DTM_ALPHA;
    /*if (strcmp(str, "DTM_RED") == 0)*/ return DTM_RED;
  }

  inline const char* vlx_ETexCompareMode(ETexCompareMode tcm)
  {
    switch(tcm)
    {
    default:
    case TCM_NONE: return "TCM_NONE";
    // case TCM_COMPARE_R_TO_TEXTURE: return "TCM_COMPARE_R_TO_TEXTURE";
    case TCM_COMPARE_REF_DEPTH_TO_TEXTURE: return "TCM_COMPARE_REF_DEPTH_TO_TEXTURE";
    }
  }

  inline ETexCompareMode vlx_ETexCompareMode(const char* str)
  {
    if (strcmp(str, "TCM_COMPARE_R_TO_TEXTURE") == 0) return TCM_COMPARE_R_TO_TEXTURE;
    if (strcmp(str, "TCM_COMPARE_REF_DEPTH_TO_TEXTURE") == 0) return TCM_COMPARE_REF_DEPTH_TO_TEXTURE;
    /*if (strcmp(str, "TCM_NONE") == 0)*/ return TCM_NONE;
  }

  inline const char* vlx_ETexCompareFunc(ETexCompareFunc tcf)
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

  inline ETexCompareFunc vlx_ETexCompareFunc(const char* str)
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

  inline const char* vlx_ETexParamFilter(ETexParamFilter tpf)
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

  inline ETexParamFilter vlx_ETexParamFilter(const char* str)
  {
    if (strcmp(str, "TPF_LINEAR") == 0) return TPF_LINEAR;
    if (strcmp(str, "TPF_NEAREST_MIPMAP_NEAREST") == 0) return TPF_NEAREST_MIPMAP_NEAREST;
    if (strcmp(str, "TPF_LINEAR_MIPMAP_NEAREST") == 0) return TPF_LINEAR_MIPMAP_NEAREST;
    if (strcmp(str, "TPF_NEAREST_MIPMAP_LINEAR") == 0) return TPF_NEAREST_MIPMAP_LINEAR;
    if (strcmp(str, "TPF_LINEAR_MIPMAP_LINEAR") == 0) return TPF_LINEAR_MIPMAP_LINEAR;
    /*if (strcmp(str, "TPF_NEAREST") == 0)*/ return TPF_NEAREST;
  }

  inline const char* vlx_ETexParamWrap(ETexParamWrap tpw)
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

  inline ETexParamWrap vlx_ETexParamWrap(const char* str)
  {
    if (strcmp(str, "TPW_CLAMP") == 0) return TPW_CLAMP;
    if (strcmp(str, "TPW_CLAMP_TO_BORDER") == 0) return TPW_CLAMP_TO_BORDER;
    if (strcmp(str, "TPW_CLAMP_TO_EDGE") == 0) return TPW_CLAMP_TO_EDGE;
    if (strcmp(str, "TPW_MIRRORED_REPEAT") == 0) return TPW_MIRRORED_REPEAT;
    /*if (strcmp(str, "TPW_REPEAT") == 0)*/ return TPW_REPEAT;
  }

  inline const char* vlx_EEnable(EEnable en)
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

  inline EEnable vlx_EEnable(const char* str)
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

  inline EPrimitiveType vlx_EPrimitiveType(const std::string& str, int line_num)
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
}

#endif
