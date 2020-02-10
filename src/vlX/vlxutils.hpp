/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
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

#ifndef vlxutils_INCLUDE_ONCE
#define vlxutils_INCLUDE_ONCE

#include <vlX/Value.hpp>
#include <vlCore/Matrix4.hpp>
#include <vlCore/Vector4.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <vlX/Serializer.hpp>

namespace vlX
{
  inline std::string vlx_makeTag(const vl::Object* obj) { return std::string("<") + obj->classType().name() + ">"; }

  inline VLXValue vlx_Identifier(const std::string& str) { return VLXValue(str.c_str(), VLXValue::Identifier); }

  inline VLXValue vlx_ID(const std::string& str)        { return VLXValue(str.c_str(), VLXValue::ID); }

  inline VLXValue vlx_String(const std::string& str)     { return VLXValue(str.c_str(), VLXValue::String); }

  inline VLXValue vlx_Rawtext(const std::string& str)    { return VLXValue( new VLXRawtextBlock(NULL, str.c_str()) ); }

  inline vl::vec2 vlx_vec2(const VLXArrayReal* arr) { VL_CHECK(arr->value().size() == 2); vl::vec2 v; arr->copyTo(v.ptr()); return v;  }

  inline vl::vec3 vlx_vec3(const VLXArrayReal* arr) { VL_CHECK(arr->value().size() == 3); vl::vec3 v; arr->copyTo(v.ptr()); return v;  }

  inline vl::vec4 vlx_vec4(const VLXArrayReal* arr) { VL_CHECK(arr->value().size() == 4); vl::vec4 v; arr->copyTo(v.ptr()); return v;  }

  inline vl::ivec4 vlx_ivec4(const VLXArrayInteger* arr) { VL_CHECK(arr->value().size() == 4); vl::ivec4 v; arr->copyTo(v.ptr()); return v; }

  inline vl::uvec4 vlx_uivec4(const VLXArrayInteger* arr) { VL_CHECK(arr->value().size() == 4); vl::uvec4 v; arr->copyTo(v.ptr()); return v; }

  inline VLXValue vlx_toValue(const std::vector<int>& vec)
  {
    VLXValue value;
    value.setArray( new VLXArrayInteger );
    value.getArrayInteger()->value().resize( vec.size() );
    if (vec.size())
      value.getArrayInteger()->copyFrom(&vec[0]);
    return value;
  }

  inline VLXValue vlx_toValue(const vl::vec4& vec)
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

  inline VLXValue vlx_toValue(const vl::ivec4& vec)
  {
    VLXValue val( new VLXArrayInteger );
    VLXArrayInteger* arr = val.getArrayInteger();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLXValue vlx_toValue(const vl::uvec4& vec)
  {
    VLXValue val( new VLXArrayInteger );
    VLXArrayInteger* arr = val.getArrayInteger();
    arr->value().resize(4);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    arr->value()[3] = vec.w();
    return val;
  }

  inline VLXValue vlx_toValue(const vl::vec3& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(3);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    arr->value()[2] = vec.z();
    return val;
  }

  inline VLXValue vlx_toValue(const vl::vec2& vec)
  {
    VLXValue val( new VLXArrayReal );
    VLXArrayReal* arr = val.getArrayReal();
    arr->value().resize(2);
    arr->value()[0] = vec.x();
    arr->value()[1] = vec.y();
    return val;
  }

  inline bool vlx_isTranslation(const vl::mat4& mat)
  {
    vl::mat4 tmp = mat;
    tmp.setT( vl::vec3(0,0,0) );
    return tmp.isIdentity();
  }

  inline bool vlx_isScaling(const vl::mat4& mat)
  {
    vl::mat4 tmp = mat;
    tmp.e(0,0) = 1;
    tmp.e(1,1) = 1;
    tmp.e(2,2) = 1;
    return tmp.isIdentity();
  }

  inline VLXValue vlx_toValue(const vl::mat4& mat)
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

  inline vl::mat4 vlx_mat4( const VLXArrayReal* arr )
  {
    vl::mat4 mat;
    arr->copyTo(mat.ptr());
    return mat;
  }

  inline vl::mat4 vlx_mat4( const VLXList* list )
  {
    vl::mat4 mat;

    for(size_t i=0; i<list->value().size(); ++i)
    {
      const VLXValue& value = list->value()[i];
      if (value.type() != VLXValue::ArrayReal)
      {
        vl::Log::error( vl::Say("Line %n : parse error during matrix import.\n") << value.lineNumber() );
        return vl::mat4::getNull();
      }
      // composition of subtransforms is made by post multiplication like for COLLADA.
      const VLXArrayReal* arr = value.getArrayReal();
      if (arr->tag() == "<Translate>")
      {
        vl::vec3 tr = vlx_vec3( arr );
        mat = mat * vl::mat4::getTranslation(tr);
      }
      else
      if (arr->tag() == "<Scale>")
      {
        vl::vec3 sc = vlx_vec3( arr );
        mat = mat * vl::mat4::getScaling(sc);
      }
      else
      if (arr->tag() == "<Matrix>")
      {
        vl::mat4 m = vlx_mat4( arr );
        mat = mat * m;
      }
      else
      if (arr->tag() == "<LookAt>")
      {
        // implements the camera's view-matrix look-at as specified by COLLADA
        if (arr->value().size() != 9)
        {
          vl::Log::error( vl::Say("Line %n : <LookAt> must have 9 floats, 3 for 'eye', 3 for 'look' and 3 for 'up'.\n") << arr->lineNumber() << arr->tag() );
        }
        else
        {
          vl::vec3 eye, look, up;
          eye.x()  = (float)arr->value()[0];
          eye.y()  = (float)arr->value()[1];
          eye.z()  = (float)arr->value()[2];
          look.x() = (float)arr->value()[3];
          look.y() = (float)arr->value()[4];
          look.z() = (float)arr->value()[5];
          up.x()   = (float)arr->value()[6];
          up.y()   = (float)arr->value()[7];
          up.z()   = (float)arr->value()[8];
          mat = mat * vl::mat4::getLookAt(eye, look, up);
        }
      }
      else
      if (arr->tag() == "<Skew>")
      {
        vl::Log::error("<Skew> tag not yet supported.\n");
      }
      else
      {
        vl::Log::error( vl::Say("Line %n : unknown tag '%s' ignored.\n") << arr->lineNumber() << arr->tag() );
      }
    }

    return mat;
  }

  inline const char* vlx_EProjectionMatrixType(vl::EProjectionMatrixType pt)
  {
    switch(pt)
    {
    default:
    case vl::PMT_UserProjection: return "PMT_UserProjection";
    case vl::PMT_OrthographicProjection: return "PMT_OrthographicProjection";
    case vl::PMT_PerspectiveProjection: return "PMT_PerspectiveProjection";
    case vl::PMT_PerspectiveProjectionFrustum: return "PMT_PerspectiveProjectionFrustum";
    }
  }

  inline vl::EProjectionMatrixType vlx_EProjectionMatrixType(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "PMT_OrthographicProjection") return vl::PMT_OrthographicProjection;
    if( value.getIdentifier() == "PMT_PerspectiveProjection") return vl::PMT_PerspectiveProjection;
    if( value.getIdentifier() == "PMT_PerspectiveProjectionFrustum") return vl::PMT_PerspectiveProjectionFrustum;
    if( value.getIdentifier() == "PMT_UserProjection") return vl::PMT_UserProjection;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::PMT_UserProjection;
  }

  inline const char* vlx_EClearColorMode(vl::EClearColorMode ccm)
  {
    switch(ccm)
    {
    default:
    case vl::CCM_Float: return "CCM_Float";
    case vl::CCM_Int: return "CCM_Int";
    case vl::CCM_UInt: return "CCM_UInt";
    }
  }

  inline vl::EClearColorMode vlx_EClearColorMode(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "CCM_Int") return vl::CCM_Int;
    if( value.getIdentifier() == "CCM_UInt") return vl::CCM_UInt;
    if( value.getIdentifier() == "CCM_Float") return vl::CCM_Float;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::CCM_Float;
  }

  inline const char* vlx_EClearFlags(vl::EClearFlags cf)
  {
    switch(cf)
    {
    default:
    case vl::CF_CLEAR_COLOR_DEPTH_STENCIL: return "CF_CLEAR_COLOR_DEPTH_STENCIL";
    case vl::CF_DO_NOT_CLEAR: return "CF_DO_NOT_CLEAR";
    case vl::CF_CLEAR_COLOR: return "CF_CLEAR_COLOR";
    case vl::CF_CLEAR_DEPTH: return "CF_CLEAR_DEPTH";
    case vl::CF_CLEAR_STENCIL: return "CF_CLEAR_STENCIL";
    case vl::CF_CLEAR_COLOR_DEPTH: return "CF_CLEAR_COLOR_DEPTH";
    case vl::CF_CLEAR_COLOR_STENCIL: return "CF_CLEAR_COLOR_STENCIL";
    case vl::CF_CLEAR_DEPTH_STENCIL: return "CF_CLEAR_DEPTH_STENCIL";
    }
  }

  inline vl::EClearFlags vlx_EClearFlags(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "CF_DO_NOT_CLEAR") return vl::CF_DO_NOT_CLEAR;
    if( value.getIdentifier() == "CF_CLEAR_COLOR") return vl::CF_CLEAR_COLOR;
    if( value.getIdentifier() == "CF_CLEAR_DEPTH") return vl::CF_CLEAR_DEPTH;
    if( value.getIdentifier() == "CF_CLEAR_STENCIL") return vl::CF_CLEAR_STENCIL;
    if( value.getIdentifier() == "CF_CLEAR_COLOR_DEPTH") return vl::CF_CLEAR_COLOR_DEPTH;
    if( value.getIdentifier() == "CF_CLEAR_COLOR_STENCIL") return vl::CF_CLEAR_COLOR_STENCIL;
    if( value.getIdentifier() == "CF_CLEAR_DEPTH_STENCIL") return vl::CF_CLEAR_DEPTH_STENCIL;
    if( value.getIdentifier() == "CF_CLEAR_COLOR_DEPTH_STENCIL") return vl::CF_CLEAR_COLOR_DEPTH_STENCIL;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::CF_DO_NOT_CLEAR;
  }

  inline const char* vlx_EPolygonFace(vl::EPolygonFace pf)
  {
    switch(pf)
    {
    default:
    case vl::PF_FRONT_AND_BACK: return "PF_FRONT_AND_BACK";
    case vl::PF_FRONT: return "PF_FRONT";
    case vl::PF_BACK:  return "PF_BACK";
    }
  }

  inline vl::EPolygonFace vlx_EPolygonFace(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "PF_FRONT") return vl::PF_FRONT;
    if( value.getIdentifier() == "PF_BACK") return vl::PF_BACK;
    if( value.getIdentifier() == "PF_FRONT_AND_BACK") return vl::PF_FRONT_AND_BACK;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::PF_FRONT_AND_BACK;
  }

  inline const char* vlx_EColorMaterial(vl::EColorMaterial cm)
  {
    switch(cm)
    {
    default:
    case vl::CM_AMBIENT_AND_DIFFUSE: return "CM_AMBIENT_AND_DIFFUSE";
    case vl::CM_EMISSION: return "CM_EMISSION";
    case vl::CM_AMBIENT: return "CM_AMBIENT";
    case vl::CM_DIFFUSE: return "CM_DIFFUSE";
    case vl::CM_SPECULAR: return "CM_SPECULAR";
    }
  }

  inline vl::EColorMaterial vlx_EColorMaterial(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "CM_EMISSION") return vl::CM_EMISSION;
    if( value.getIdentifier() == "CM_AMBIENT") return vl::CM_AMBIENT;
    if( value.getIdentifier() == "CM_DIFFUSE") return vl::CM_DIFFUSE;
    if( value.getIdentifier() == "CM_SPECULAR") return vl::CM_SPECULAR;
    if( value.getIdentifier() == "CM_AMBIENT_AND_DIFFUSE") return vl::CM_AMBIENT_AND_DIFFUSE;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::CM_AMBIENT_AND_DIFFUSE;
  }

  inline const char* vlx_ETextureFormat(vl::ETextureFormat tf)
  {
    switch(tf)
    {
    default:
    case vl::TF_UNKNOWN: return "TF_UNKNOWN";

    case vl::TF_ALPHA  : return "TF_ALPHA";
    case vl::TF_ALPHA4 : return "TF_ALPHA4";
    case vl::TF_ALPHA8 : return "TF_ALPHA8";
    case vl::TF_ALPHA12: return "TF_ALPHA12";
    case vl::TF_ALPHA16: return "TF_ALPHA16";

    case vl::TF_INTENSITY  : return "TF_INTENSITY";
    case vl::TF_INTENSITY4 : return "TF_INTENSITY4";
    case vl::TF_INTENSITY8 : return "TF_INTENSITY8";
    case vl::TF_INTENSITY12: return "TF_INTENSITY12";
    case vl::TF_INTENSITY16: return "TF_INTENSITY16";
    case vl::TF_LUMINANCE  : return "TF_LUMINANCE";
    case vl::TF_LUMINANCE4 : return "TF_LUMINANCE4";
    case vl::TF_LUMINANCE8 : return "TF_LUMINANCE8";
    case vl::TF_LUMINANCE12: return "TF_LUMINANCE12";
    case vl::TF_LUMINANCE16: return "TF_LUMINANCE16";
    case vl::TF_LUMINANCE_ALPHA    : return "TF_LUMINANCE_ALPHA";
    case vl::TF_LUMINANCE4_ALPHA4  : return "TF_LUMINANCE4_ALPHA4";
    case vl::TF_LUMINANCE6_ALPHA2  : return "TF_LUMINANCE6_ALPHA2";
    case vl::TF_LUMINANCE8_ALPHA8  : return "TF_LUMINANCE8_ALPHA8";
    case vl::TF_LUMINANCE12_ALPHA4 : return "TF_LUMINANCE12_ALPHA4";
    case vl::TF_LUMINANCE12_ALPHA12: return "TF_LUMINANCE12_ALPHA12";
    case vl::TF_LUMINANCE16_ALPHA16: return "TF_LUMINANCE16_ALPHA16";
    case vl::TF_R3_G3_B2: return "TF_R3_G3_B2";
    case vl::TF_RGB     : return "TF_RGB";
    case vl::TF_RGB4    : return "TF_RGB4";
    case vl::TF_RGB5    : return "TF_RGB5";
    case vl::TF_RGB8    : return "TF_RGB8";
    case vl::TF_RGB10   : return "TF_RGB10";
    case vl::TF_RGB12   : return "TF_RGB12";
    case vl::TF_RGB16   : return "TF_RGB16";
    case vl::TF_RGBA    : return "TF_RGBA";
    case vl::TF_RGBA2   : return "TF_RGBA2";
    case vl::TF_RGBA4   : return "TF_RGBA4";
    case vl::TF_RGB5_A1 : return "TF_RGB5_A1";
    case vl::TF_RGBA8   : return "TF_RGBA8";
    case vl::TF_RGB10_A2: return "TF_RGB10_A2";
    case vl::TF_RGBA12  : return "TF_RGBA12";
    case vl::TF_RGBA16  : return "TF_RGBA16";

    // ARB_texture_float / OpenGL 3
    case vl::TF_RGBA32F: return "TF_RGBA32F";
    case vl::TF_RGB32F: return "TF_RGB32F";
    case vl::TF_ALPHA32F: return "TF_ALPHA32F";
    case vl::TF_INTENSITY32F: return "TF_INTENSITY32F";
    case vl::TF_LUMINANCE32F: return "TF_LUMINANCE32F";
    case vl::TF_LUMINANCE_ALPHA32F: return "TF_LUMINANCE_ALPHA32F";
    case vl::TF_RGBA16F: return "TF_RGBA16F";
    case vl::TF_RGB16F: return "TF_RGB16F";
    case vl::TF_ALPHA16F: return "TF_ALPHA16F";
    case vl::TF_INTENSITY16F: return "TF_INTENSITY16F";
    case vl::TF_LUMINANCE16F: return "TF_LUMINANCE16F";
    case vl::TF_LUMINANCE_ALPHA16F: return "TF_LUMINANCE_ALPHA16F";

    // from table 3.12 opengl api specs 4.1
    case vl::TF_R8_SNORM: return "TF_R8_SNORM";
    case vl::TF_R16_SNORM: return "TF_R16_SNORM";
    case vl::TF_RG8_SNORM: return "TF_RG8_SNORM";
    case vl::TF_RG16_SNORM: return "TF_RG16_SNORM";
    case vl::TF_RGB8_SNORM: return "TF_RGB8_SNORM";
    case vl::TF_RGBA8_SNORM: return "TF_RGBA8_SNORM";
    case vl::TF_RGB10_A2UI: return "TF_RGB10_A2UI";
    case vl::TF_RGBA16_SNORM: return "TF_RGBA16_SNORM";
    case vl::TF_R11F_G11F_B10F: return "TF_R11F_G11F_B10F";
    case vl::TF_RGB9_E5: return "TF_RGB9_E5";
    case vl::TF_RGB8I: return "TF_RGB8I";
    case vl::TF_RGB8UI: return "TF_RGB8UI";
    case vl::TF_RGB16I: return "TF_RGB16I";
    case vl::TF_RGB16UI: return "TF_RGB16UI";
    case vl::TF_RGB32I: return "TF_RGB32I";
    case vl::TF_RGB32UI: return "TF_RGB32UI";
    case vl::TF_RGBA8I: return "TF_RGBA8I";
    case vl::TF_RGBA8UI: return "TF_RGBA8UI";
    case vl::TF_RGBA16I: return "TF_RGBA16I";
    case vl::TF_RGBA16UI: return "TF_RGBA16UI";
    case vl::TF_RGBA32I: return "TF_RGBA32I";
    case vl::TF_RGBA32UI: return "TF_TF_RGBA32UI";

    // ATI_texture_float (the enums are the same as ARB_texture_float)
    //case vl::TF_RGBA_FLOAT32_ATI: return "TF_RGBA_FLOAT32_ATI";
    //case vl::TF_RGB_FLOAT32_ATI: return "TF_RGB_FLOAT32_ATI";
    //case vl::TF_ALPHA_FLOAT32_ATI: return "TF_ALPHA_FLOAT32_ATI";
    //case vl::TF_INTENSITY_FLOAT32_ATI: return "TF_INTENSITY_FLOAT32_ATI";
    //case vl::TF_LUMINANCE_FLOAT32_ATI: return "TF_LUMINANCE_FLOAT32_ATI";
    //case vl::TF_LUMINANCE_ALPHA_FLOAT32_ATI: return "TF_LUMINANCE_ALPHA_FLOAT32_ATI";
    //case vl::TF_RGBA_FLOAT16_ATI: return "TF_RGBA_FLOAT16_ATI";
    //case vl::TF_RGB_FLOAT16_ATI: return "TF_RGB_FLOAT16_ATI";
    //case vl::TF_ALPHA_FLOAT16_ATI: return "TF_ALPHA_FLOAT16_ATI";
    //case vl::TF_INTENSITY_FLOAT16_ATI: return "TF_INTENSITY_FLOAT16_ATI";
    //case vl::TF_LUMINANCE_FLOAT16_ATI: return "TF_LUMINANCE_FLOAT16_ATI";
    //case vl::TF_LUMINANCE_ALPHA_FLOAT16_ATI: return "TF_LUMINANCE_ALPHA_FLOAT16_ATI";

    // EXT_texture_shared_exponent
    // case vl::TF_RGB9_E5_EXT: return "TF_RGB9_E5_EXT";

    // EXT_packed_float
    // case vl::TF_11F_G11F_B10F_EXT: return "TF_11F_G11F_B10F_EXT";

    // EXT_packed_depth_stencil / GL_ARB_framebuffer_object
    case vl::TF_DEPTH_STENCIL   : return "TF_DEPTH_STENCIL";
    case vl::TF_DEPTH24_STENCIL8: return "TF_DEPTH24_STENCIL8";

    // ARB_depth_buffer_float
    case vl::TF_DEPTH_COMPONENT32F: return "TF_DEPTH_COMPONENT32F";
    case vl::TF_DEPTH32F_STENCIL8 : return "TF_DEPTH32F_STENCIL8";

    // ARB_depth_texture
    case vl::TF_DEPTH_COMPONENT  : return "TF_DEPTH_COMPONENT";
    case vl::TF_DEPTH_COMPONENT16: return "TF_DEPTH_COMPONENT16";
    case vl::TF_DEPTH_COMPONENT24: return "TF_DEPTH_COMPONENT24";
    case vl::TF_DEPTH_COMPONENT32: return "TF_DEPTH_COMPONENT32";

    // ARB_texture_compression
    case vl::TF_COMPRESSED_ALPHA          : return "TF_COMPRESSED_ALPHA";
    case vl::TF_COMPRESSED_INTENSITY      : return "TF_COMPRESSED_INTENSITY";
    case vl::TF_COMPRESSED_LUMINANCE      : return "TF_COMPRESSED_LUMINANCE";
    case vl::TF_COMPRESSED_LUMINANCE_ALPHA: return "TF_COMPRESSED_LUMINANCE_ALPHA";
    case vl::TF_COMPRESSED_RGB            : return "TF_COMPRESSED_RGB";
    case vl::TF_COMPRESSED_RGBA           : return "TF_COMPRESSED_RGBA";

    // 3DFX_texture_compression_FXT1
    case vl::TF_COMPRESSED_RGB_FXT1_3DFX : return "TF_COMPRESSED_RGB_FXT1_3DFX";
    case vl::TF_COMPRESSED_RGBA_FXT1_3DFX: return "TF_COMPRESSED_RGBA_FXT1_3DFX";

    // EXT_texture_compression_s3tc
    case vl::TF_COMPRESSED_RGB_S3TC_DXT1_EXT : return "TF_COMPRESSED_RGB_S3TC_DXT1_EXT";
    case vl::TF_COMPRESSED_RGBA_S3TC_DXT1_EXT: return "TF_COMPRESSED_RGBA_S3TC_DXT1_EXT";
    case vl::TF_COMPRESSED_RGBA_S3TC_DXT3_EXT: return "TF_COMPRESSED_RGBA_S3TC_DXT3_EXT";
    case vl::TF_COMPRESSED_RGBA_S3TC_DXT5_EXT: return "TF_COMPRESSED_RGBA_S3TC_DXT5_EXT";

    // EXT_texture_compression_latc
    case vl::TF_COMPRESSED_LUMINANCE_LATC1_EXT             : return "TF_COMPRESSED_LUMINANCE_LATC1_EXT";
    case vl::TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT      : return "TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT";
    case vl::TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT       : return "TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT";
    case vl::TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT: return "TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT";

    // EXT_texture_compression_rgtc
    case vl::TF_COMPRESSED_RED_RGTC1_EXT             : return "TF_COMPRESSED_RED_RGTC1_EXT";
    case vl::TF_COMPRESSED_SIGNED_RED_RGTC1_EXT      : return "TF_COMPRESSED_SIGNED_RED_RGTC1_EXT";
    case vl::TF_COMPRESSED_RED_GREEN_RGTC2_EXT       : return "TF_COMPRESSED_RED_GREEN_RGTC2_EXT";
    case vl::TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT: return "TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT";

    // EXT_texture_integer
    // case vl::TF_RGBA32UI_EXT: return "TF_RGBA32UI_EXT";
    // case vl::TF_RGB32UI_EXT: return "TF_RGB32UI_EXT";
    case vl::TF_ALPHA32UI_EXT: return "TF_ALPHA32UI_EXT";
    case vl::TF_INTENSITY32UI_EXT: return "TF_INTENSITY32UI_EXT";
    case vl::TF_LUMINANCE32UI_EXT: return "TF_LUMINANCE32UI_EXT";
    case vl::TF_LUMINANCE_ALPHA32UI_EXT: return "TF_LUMINANCE_ALPHA32UI_EXT";

    // case vl::TF_RGBA16UI_EXT: return "TF_RGBA16UI_EXT";
    // case vl::TF_RGB16UI_EXT: return "TF_RGB16UI_EXT";
    case vl::TF_ALPHA16UI_EXT: return "TF_ALPHA16UI_EXT";
    case vl::TF_INTENSITY16UI_EXT: return "TF_INTENSITY16UI_EXT";
    case vl::TF_LUMINANCE16UI_EXT: return "TF_LUMINANCE16UI_EXT";
    case vl::TF_LUMINANCE_ALPHA16UI_EXT: return "TF_LUMINANCE_ALPHA16UI_EXT";

    // case vl::TF_RGBA8UI_EXT: return "TF_RGBA8UI_EXT";
    // case vl::TF_RGB8UI_EXT: return "TF_RGB8UI_EXT";
    case vl::TF_ALPHA8UI_EXT: return "TF_ALPHA8UI_EXT";
    case vl::TF_INTENSITY8UI_EXT: return "TF_INTENSITY8UI_EXT";
    case vl::TF_LUMINANCE8UI_EXT: return "TF_LUMINANCE8UI_EXT";
    case vl::TF_LUMINANCE_ALPHA8UI_EXT: return "TF_LUMINANCE_ALPHA8UI_EXT";

    // case vl::TF_RGBA32I_EXT: return "TF_RGBA32I_EXT";
    // case vl::TF_RGB32I_EXT: return "TF_RGB32I_EXT";
    case vl::TF_ALPHA32I_EXT: return "TF_ALPHA32I_EXT";
    case vl::TF_INTENSITY32I_EXT: return "TF_INTENSITY32I_EXT";
    case vl::TF_LUMINANCE32I_EXT: return "TF_LUMINANCE32I_EXT";
    case vl::TF_LUMINANCE_ALPHA32I_EXT: return "TF_LUMINANCE_ALPHA32I_EXT";

    // case vl::TF_RGBA16I_EXT: return "TF_RGBA16I_EXT";
    // case vl::TF_RGB16I_EXT: return "TF_RGB16I_EXT";
    case vl::TF_ALPHA16I_EXT: return "TF_ALPHA16I_EXT";
    case vl::TF_INTENSITY16I_EXT: return "TF_INTENSITY16I_EXT";
    case vl::TF_LUMINANCE16I_EXT: return "TF_LUMINANCE16I_EXT";
    case vl::TF_LUMINANCE_ALPHA16I_EXT: return "TF_LUMINANCE_ALPHA16I_EXT";

    // case vl::TF_RGBA8I_EXT: return "TF_RGBA8I_EXT";
    // case vl::TF_RGB8I_EXT: return "TF_RGB8I_EXT";
    case vl::TF_ALPHA8I_EXT: return "TF_ALPHA8I_EXT";
    case vl::TF_INTENSITY8I_EXT: return "TF_INTENSITY8I_EXT";
    case vl::TF_LUMINANCE8I_EXT: return "TF_LUMINANCE8I_EXT";
    case vl::TF_LUMINANCE_ALPHA8I_EXT: return "TF_LUMINANCE_ALPHA8I_EXT";

    // GL_ARB_texture_rg
    case vl::TF_RED: return "TF_RED";
    case vl::TF_COMPRESSED_RED: return "TF_COMPRESSED_RED";
    case vl::TF_COMPRESSED_RG: return "TF_COMPRESSED_RG";
    case vl::TF_RG: return "TF_RG";
    case vl::TF_R8: return "TF_R8";
    case vl::TF_R16: return "TF_R16";
    case vl::TF_RG8: return "TF_RG8";
    case vl::TF_RG16: return "TF_RG16";
    case vl::TF_R16F: return "TF_R16F";
    case vl::TF_R32F: return "TF_R32F";
    case vl::TF_RG16F: return "TF_RG16F";
    case vl::TF_RG32F: return "TF_RG32F";
    case vl::TF_R8I: return "TF_R8I";
    case vl::TF_R8UI: return "TF_R8UI";
    case vl::TF_R16I: return "TF_R16I";
    case vl::TF_R16UI: return "TF_R16UI";
    case vl::TF_R32I: return "TF_R32I";
    case vl::TF_R32UI: return "TF_R32UI";
    case vl::TF_RG8I: return "TF_RG8I";
    case vl::TF_RG8UI: return "TF_RG8UI";
    case vl::TF_RG16I: return "TF_RG16I";
    case vl::TF_RG16UI: return "TF_RG16UI";
    case vl::TF_RG32I: return "TF_RG32I";
    case vl::TF_RG32UI: return "TF_RG32UI";

    // sRGB OpenGL 2.1
    case vl::TF_SLUMINANCE_ALPHA: return "TF_SLUMINANCE_ALPHA";
    case vl::TF_SLUMINANCE8_ALPHA8: return "TF_SLUMINANCE8_ALPHA8";
    case vl::TF_SLUMINANCE: return "TF_SLUMINANCE";
    case vl::TF_SLUMINANCE8: return "TF_SLUMINANCE8";
    case vl::TF_COMPRESSED_SLUMINANCE: return "TF_COMPRESSED_SLUMINANCE";
    case vl::TF_COMPRESSED_SLUMINANCE_ALPHA: return "TF_COMPRESSED_SLUMINANCE_ALPHA";

    // sRGB OpenGL 2.1 / 3.x
    case vl::TF_SRGB: return "TF_SRGB";
    case vl::TF_SRGB8: return "TF_SRGB8";
    case vl::TF_SRGB_ALPHA: return "TF_SRGB_ALPHA";
    case vl::TF_SRGB8_ALPHA8: return "TF_SRGB8_ALPHA8";
    case vl::TF_COMPRESSED_SRGB: return "TF_COMPRESSED_SRGB";
    case vl::TF_COMPRESSED_SRGB_ALPHA: return "TF_COMPRESSED_SRGB_ALPHA";

    // GL_EXT_texture_sRGB compressed formats
    case vl::TF_COMPRESSED_SRGB_S3TC_DXT1_EXT: return "TF_COMPRESSED_SRGB_S3TC_DXT1_EXT";
    case vl::TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT: return "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT";
    case vl::TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT: return "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT";
    case vl::TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT: return "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT";
    }
  }

  inline vl::ETextureFormat vlx_ETextureFormat(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "TF_UNKNOWN") return vl::TF_UNKNOWN;

    if( value.getIdentifier() == "TF_ALPHA") return vl::TF_ALPHA;
    if( value.getIdentifier() == "TF_ALPHA4") return vl::TF_ALPHA4;
    if( value.getIdentifier() == "TF_ALPHA8") return vl::TF_ALPHA8;
    if( value.getIdentifier() == "TF_ALPHA12") return vl::TF_ALPHA12;
    if( value.getIdentifier() == "TF_ALPHA16") return vl::TF_ALPHA16;

    if( value.getIdentifier() == "TF_INTENSITY") return vl::TF_INTENSITY;
    if( value.getIdentifier() == "TF_INTENSITY4") return vl::TF_INTENSITY4;
    if( value.getIdentifier() == "TF_INTENSITY8") return vl::TF_INTENSITY8;
    if( value.getIdentifier() == "TF_INTENSITY12") return vl::TF_INTENSITY12;
    if( value.getIdentifier() == "TF_INTENSITY16") return vl::TF_INTENSITY16;
    if( value.getIdentifier() == "TF_LUMINANCE") return vl::TF_LUMINANCE;
    if( value.getIdentifier() == "TF_LUMINANCE4") return vl::TF_LUMINANCE4;
    if( value.getIdentifier() == "TF_LUMINANCE8") return vl::TF_LUMINANCE8;
    if( value.getIdentifier() == "TF_LUMINANCE12") return vl::TF_LUMINANCE12;
    if( value.getIdentifier() == "TF_LUMINANCE16") return vl::TF_LUMINANCE16;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA") return vl::TF_LUMINANCE_ALPHA;
    if( value.getIdentifier() == "TF_LUMINANCE4_ALPHA4") return vl::TF_LUMINANCE4_ALPHA4;
    if( value.getIdentifier() == "TF_LUMINANCE6_ALPHA2") return vl::TF_LUMINANCE6_ALPHA2;
    if( value.getIdentifier() == "TF_LUMINANCE8_ALPHA8") return vl::TF_LUMINANCE8_ALPHA8;
    if( value.getIdentifier() == "TF_LUMINANCE12_ALPHA4") return vl::TF_LUMINANCE12_ALPHA4;
    if( value.getIdentifier() == "TF_LUMINANCE12_ALPHA12") return vl::TF_LUMINANCE12_ALPHA12;
    if( value.getIdentifier() == "TF_LUMINANCE16_ALPHA16") return vl::TF_LUMINANCE16_ALPHA16;
    if( value.getIdentifier() == "TF_R3_G3_B2") return vl::TF_R3_G3_B2;
    if( value.getIdentifier() == "TF_RGB") return vl::TF_RGB;
    if( value.getIdentifier() == "TF_RGB4") return vl::TF_RGB4;
    if( value.getIdentifier() == "TF_RGB5") return vl::TF_RGB5;
    if( value.getIdentifier() == "TF_RGB8") return vl::TF_RGB8;
    if( value.getIdentifier() == "TF_RGB10") return vl::TF_RGB10;
    if( value.getIdentifier() == "TF_RGB12") return vl::TF_RGB12;
    if( value.getIdentifier() == "TF_RGB16") return vl::TF_RGB16;
    if( value.getIdentifier() == "TF_RGBA") return vl::TF_RGBA;
    if( value.getIdentifier() == "TF_RGBA2") return vl::TF_RGBA2;
    if( value.getIdentifier() == "TF_RGBA4") return vl::TF_RGBA4;
    if( value.getIdentifier() == "TF_RGB5_A1") return vl::TF_RGB5_A1;
    if( value.getIdentifier() == "TF_RGBA8") return vl::TF_RGBA8;
    if( value.getIdentifier() == "TF_RGB10_A2") return vl::TF_RGB10_A2;
    if( value.getIdentifier() == "TF_RGBA12") return vl::TF_RGBA12;
    if( value.getIdentifier() == "TF_RGBA16") return vl::TF_RGBA16;

    // ARB_texture_float / OpenGL 3
    if( value.getIdentifier() == "TF_RGBA32F") return vl::TF_RGBA32F;
    if( value.getIdentifier() == "TF_RGB32F") return vl::TF_RGB32F;
    if( value.getIdentifier() == "TF_ALPHA32F") return vl::TF_ALPHA32F;
    if( value.getIdentifier() == "TF_INTENSITY32F") return vl::TF_INTENSITY32F;
    if( value.getIdentifier() == "TF_LUMINANCE32F") return vl::TF_LUMINANCE32F;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA32F") return vl::TF_LUMINANCE_ALPHA32F;
    if( value.getIdentifier() == "TF_RGBA16F") return vl::TF_RGBA16F;
    if( value.getIdentifier() == "TF_RGB16F") return vl::TF_RGB16F;
    if( value.getIdentifier() == "TF_ALPHA16F") return vl::TF_ALPHA16F;
    if( value.getIdentifier() == "TF_INTENSITY16F") return vl::TF_INTENSITY16F;
    if( value.getIdentifier() == "TF_LUMINANCE16F") return vl::TF_LUMINANCE16F;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA16F") return vl::TF_LUMINANCE_ALPHA16F;

    // from table 3.12 opengl api specs 4.1
    if( value.getIdentifier() == "TF_R8_SNORM") return vl::TF_R8_SNORM;
    if( value.getIdentifier() == "TF_R16_SNORM") return vl::TF_R16_SNORM;
    if( value.getIdentifier() == "TF_RG8_SNORM") return vl::TF_RG8_SNORM;
    if( value.getIdentifier() == "TF_RG16_SNORM") return vl::TF_RG16_SNORM;
    if( value.getIdentifier() == "TF_RGB8_SNORM") return vl::TF_RGB8_SNORM;
    if( value.getIdentifier() == "TF_RGBA8_SNORM") return vl::TF_RGBA8_SNORM;
    if( value.getIdentifier() == "TF_RGB10_A2UI") return vl::TF_RGB10_A2UI;
    if( value.getIdentifier() == "TF_RGBA16_SNORM") return vl::TF_RGBA16_SNORM;
    if( value.getIdentifier() == "TF_R11F_G11F_B10F") return vl::TF_R11F_G11F_B10F;
    if( value.getIdentifier() == "TF_RGB9_E5") return vl::TF_RGB9_E5;
    if( value.getIdentifier() == "TF_RGB8I") return vl::TF_RGB8I;
    if( value.getIdentifier() == "TF_RGB8UI") return vl::TF_RGB8UI;
    if( value.getIdentifier() == "TF_RGB16I") return vl::TF_RGB16I;
    if( value.getIdentifier() == "TF_RGB16UI") return vl::TF_RGB16UI;
    if( value.getIdentifier() == "TF_RGB32I") return vl::TF_RGB32I;
    if( value.getIdentifier() == "TF_RGB32UI") return vl::TF_RGB32UI;
    if( value.getIdentifier() == "TF_RGBA8I") return vl::TF_RGBA8I;
    if( value.getIdentifier() == "TF_RGBA8UI") return vl::TF_RGBA8UI;
    if( value.getIdentifier() == "TF_RGBA16I") return vl::TF_RGBA16I;
    if( value.getIdentifier() == "TF_RGBA16UI") return vl::TF_RGBA16UI;
    if( value.getIdentifier() == "TF_RGBA32I") return vl::TF_RGBA32I;
    if( value.getIdentifier() == "TF_RGBA32UI") return vl::TF_RGBA32UI;

    // ATI_texture_float (the enums are the same as ARB_texture_float)
    if( value.getIdentifier() == "TF_RGBA_FLOAT32_ATI") return vl::TF_RGBA_FLOAT32_ATI;
    if( value.getIdentifier() == "TF_RGB_FLOAT32_ATI") return vl::TF_RGB_FLOAT32_ATI;
    if( value.getIdentifier() == "TF_ALPHA_FLOAT32_ATI") return vl::TF_ALPHA_FLOAT32_ATI;
    if( value.getIdentifier() == "TF_INTENSITY_FLOAT32_ATI") return vl::TF_INTENSITY_FLOAT32_ATI;
    if( value.getIdentifier() == "TF_LUMINANCE_FLOAT32_ATI") return vl::TF_LUMINANCE_FLOAT32_ATI;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA_FLOAT32_ATI") return vl::TF_LUMINANCE_ALPHA_FLOAT32_ATI;
    if( value.getIdentifier() == "TF_RGBA_FLOAT16_ATI") return vl::TF_RGBA_FLOAT16_ATI;
    if( value.getIdentifier() == "TF_RGB_FLOAT16_ATI") return vl::TF_RGB_FLOAT16_ATI;
    if( value.getIdentifier() == "TF_ALPHA_FLOAT16_ATI") return vl::TF_ALPHA_FLOAT16_ATI;
    if( value.getIdentifier() == "TF_INTENSITY_FLOAT16_ATI") return vl::TF_INTENSITY_FLOAT16_ATI;
    if( value.getIdentifier() == "TF_LUMINANCE_FLOAT16_ATI") return vl::TF_LUMINANCE_FLOAT16_ATI;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA_FLOAT16_ATI") return vl::TF_LUMINANCE_ALPHA_FLOAT16_ATI;

    // EXT_texture_shared_exponent
    if( value.getIdentifier() == "TF_RGB9_E5_EXT") return vl::TF_RGB9_E5_EXT;

    // EXT_packed_float
    if( value.getIdentifier() == "TF_11F_G11F_B10F_EXT") return vl::TF_11F_G11F_B10F_EXT;

    // EXT_packed_depth_stencil / GL_ARB_framebuffer_object
    if( value.getIdentifier() == "TF_DEPTH_STENCIL") return vl::TF_DEPTH_STENCIL;
    if( value.getIdentifier() == "TF_DEPTH24_STENCIL8") return vl::TF_DEPTH24_STENCIL8;

    // ARB_depth_buffer_float
    if( value.getIdentifier() == "TF_DEPTH_COMPONENT32F") return vl::TF_DEPTH_COMPONENT32F;
    if( value.getIdentifier() == "TF_DEPTH32F_STENCIL8") return vl::TF_DEPTH32F_STENCIL8;

    // ARB_depth_texture
    if( value.getIdentifier() == "TF_DEPTH_COMPONENT") return vl::TF_DEPTH_COMPONENT;
    if( value.getIdentifier() == "TF_DEPTH_COMPONENT16") return vl::TF_DEPTH_COMPONENT16;
    if( value.getIdentifier() == "TF_DEPTH_COMPONENT24") return vl::TF_DEPTH_COMPONENT24;
    if( value.getIdentifier() == "TF_DEPTH_COMPONENT32") return vl::TF_DEPTH_COMPONENT32;

    // ARB_texture_compression
    if( value.getIdentifier() == "TF_COMPRESSED_ALPHA") return vl::TF_COMPRESSED_ALPHA;
    if( value.getIdentifier() == "TF_COMPRESSED_INTENSITY") return vl::TF_COMPRESSED_INTENSITY;
    if( value.getIdentifier() == "TF_COMPRESSED_LUMINANCE") return vl::TF_COMPRESSED_LUMINANCE;
    if( value.getIdentifier() == "TF_COMPRESSED_LUMINANCE_ALPHA") return vl::TF_COMPRESSED_LUMINANCE_ALPHA;
    if( value.getIdentifier() == "TF_COMPRESSED_RGB") return vl::TF_COMPRESSED_RGB;
    if( value.getIdentifier() == "TF_COMPRESSED_RGBA") return vl::TF_COMPRESSED_RGBA;

    // 3DFX_texture_compression_FXT1
    if( value.getIdentifier() == "TF_COMPRESSED_RGB_FXT1_3DFX") return vl::TF_COMPRESSED_RGB_FXT1_3DFX;
    if( value.getIdentifier() == "TF_COMPRESSED_RGBA_FXT1_3DFX") return vl::TF_COMPRESSED_RGBA_FXT1_3DFX;

    // EXT_texture_compression_s3tc
    if( value.getIdentifier() == "TF_COMPRESSED_RGB_S3TC_DXT1_EXT") return vl::TF_COMPRESSED_RGB_S3TC_DXT1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_RGBA_S3TC_DXT1_EXT") return vl::TF_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_RGBA_S3TC_DXT3_EXT") return vl::TF_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_RGBA_S3TC_DXT5_EXT") return vl::TF_COMPRESSED_RGBA_S3TC_DXT5_EXT;

    // EXT_texture_compression_latc
    if( value.getIdentifier() == "TF_COMPRESSED_LUMINANCE_LATC1_EXT") return vl::TF_COMPRESSED_LUMINANCE_LATC1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT") return vl::TF_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT") return vl::TF_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT") return vl::TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT;

    // EXT_texture_compression_rgtc
    if( value.getIdentifier() == "TF_COMPRESSED_RED_RGTC1_EXT") return vl::TF_COMPRESSED_RED_RGTC1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SIGNED_RED_RGTC1_EXT") return vl::TF_COMPRESSED_SIGNED_RED_RGTC1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_RED_GREEN_RGTC2_EXT") return vl::TF_COMPRESSED_RED_GREEN_RGTC2_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT") return vl::TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT;

    // EXT_texture_integer
    if( value.getIdentifier() == "TF_RGBA32UI_EXT") return vl::TF_RGBA32UI_EXT;
    if( value.getIdentifier() == "TF_RGB32UI_EXT") return vl::TF_RGB32UI_EXT;
    if( value.getIdentifier() == "TF_ALPHA32UI_EXT") return vl::TF_ALPHA32UI_EXT;
    if( value.getIdentifier() == "TF_INTENSITY32UI_EXT") return vl::TF_INTENSITY32UI_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE32UI_EXT") return vl::TF_LUMINANCE32UI_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA32UI_EXT") return vl::TF_LUMINANCE_ALPHA32UI_EXT;

    if( value.getIdentifier() == "TF_RGBA16UI_EXT") return vl::TF_RGBA16UI_EXT;
    if( value.getIdentifier() == "TF_RGB16UI_EXT") return vl::TF_RGB16UI_EXT;
    if( value.getIdentifier() == "TF_ALPHA16UI_EXT") return vl::TF_ALPHA16UI_EXT;
    if( value.getIdentifier() == "TF_INTENSITY16UI_EXT") return vl::TF_INTENSITY16UI_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE16UI_EXT") return vl::TF_LUMINANCE16UI_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA16UI_EXT") return vl::TF_LUMINANCE_ALPHA16UI_EXT;

    if( value.getIdentifier() == "TF_RGBA8UI_EXT") return vl::TF_RGBA8UI_EXT;
    if( value.getIdentifier() == "TF_RGB8UI_EXT") return vl::TF_RGB8UI_EXT;
    if( value.getIdentifier() == "TF_ALPHA8UI_EXT") return vl::TF_ALPHA8UI_EXT;
    if( value.getIdentifier() == "TF_INTENSITY8UI_EXT") return vl::TF_INTENSITY8UI_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE8UI_EXT") return vl::TF_LUMINANCE8UI_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA8UI_EXT") return vl::TF_LUMINANCE_ALPHA8UI_EXT;

    if( value.getIdentifier() == "TF_RGBA32I_EXT") return vl::TF_RGBA32I_EXT;
    if( value.getIdentifier() == "TF_RGB32I_EXT") return vl::TF_RGB32I_EXT;
    if( value.getIdentifier() == "TF_ALPHA32I_EXT") return vl::TF_ALPHA32I_EXT;
    if( value.getIdentifier() == "TF_INTENSITY32I_EXT") return vl::TF_INTENSITY32I_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE32I_EXT") return vl::TF_LUMINANCE32I_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA32I_EXT") return vl::TF_LUMINANCE_ALPHA32I_EXT;

    if( value.getIdentifier() == "TF_RGBA16I_EXT") return vl::TF_RGBA16I_EXT;
    if( value.getIdentifier() == "TF_RGB16I_EXT") return vl::TF_RGB16I_EXT;
    if( value.getIdentifier() == "TF_ALPHA16I_EXT") return vl::TF_ALPHA16I_EXT;
    if( value.getIdentifier() == "TF_INTENSITY16I_EXT") return vl::TF_INTENSITY16I_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE16I_EXT") return vl::TF_LUMINANCE16I_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA16I_EXT") return vl::TF_LUMINANCE_ALPHA16I_EXT;

    if( value.getIdentifier() == "TF_RGBA8I_EXT") return vl::TF_RGBA8I_EXT;
    if( value.getIdentifier() == "TF_RGB8I_EXT") return vl::TF_RGB8I_EXT;
    if( value.getIdentifier() == "TF_ALPHA8I_EXT") return vl::TF_ALPHA8I_EXT;
    if( value.getIdentifier() == "TF_INTENSITY8I_EXT") return vl::TF_INTENSITY8I_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE8I_EXT") return vl::TF_LUMINANCE8I_EXT;
    if( value.getIdentifier() == "TF_LUMINANCE_ALPHA8I_EXT") return vl::TF_LUMINANCE_ALPHA8I_EXT;

    // GL_ARB_texture_rg
    if( value.getIdentifier() == "TF_RED") return vl::TF_RED;
    if( value.getIdentifier() == "TF_COMPRESSED_RED") return vl::TF_COMPRESSED_RED;
    if( value.getIdentifier() == "TF_COMPRESSED_RG") return vl::TF_COMPRESSED_RG;
    if( value.getIdentifier() == "TF_RG") return vl::TF_RG;
    if( value.getIdentifier() == "TF_R8") return vl::TF_R8;
    if( value.getIdentifier() == "TF_R16") return vl::TF_R16;
    if( value.getIdentifier() == "TF_RG8") return vl::TF_RG8;
    if( value.getIdentifier() == "TF_RG16") return vl::TF_RG16;
    if( value.getIdentifier() == "TF_R16F") return vl::TF_R16F;
    if( value.getIdentifier() == "TF_R32F") return vl::TF_R32F;
    if( value.getIdentifier() == "TF_RG16F") return vl::TF_RG16F;
    if( value.getIdentifier() == "TF_RG32F") return vl::TF_RG32F;
    if( value.getIdentifier() == "TF_R8I") return vl::TF_R8I;
    if( value.getIdentifier() == "TF_R8UI") return vl::TF_R8UI;
    if( value.getIdentifier() == "TF_R16I") return vl::TF_R16I;
    if( value.getIdentifier() == "TF_R16UI") return vl::TF_R16UI;
    if( value.getIdentifier() == "TF_R32I") return vl::TF_R32I;
    if( value.getIdentifier() == "TF_R32UI") return vl::TF_R32UI;
    if( value.getIdentifier() == "TF_RG8I") return vl::TF_RG8I;
    if( value.getIdentifier() == "TF_RG8UI") return vl::TF_RG8UI;
    if( value.getIdentifier() == "TF_RG16I") return vl::TF_RG16I;
    if( value.getIdentifier() == "TF_RG16UI") return vl::TF_RG16UI;
    if( value.getIdentifier() == "TF_RG32I") return vl::TF_RG32I;
    if( value.getIdentifier() == "TF_RG32UI") return vl::TF_RG32UI;

    // sRGB OpenGL 2.1
    if( value.getIdentifier() == "TF_SLUMINANCE_ALPHA") return vl::TF_SLUMINANCE_ALPHA;
    if( value.getIdentifier() == "TF_SLUMINANCE8_ALPHA8") return vl::TF_SLUMINANCE8_ALPHA8;
    if( value.getIdentifier() == "TF_SLUMINANCE") return vl::TF_SLUMINANCE;
    if( value.getIdentifier() == "TF_SLUMINANCE8") return vl::TF_SLUMINANCE8;
    if( value.getIdentifier() == "TF_COMPRESSED_SLUMINANCE") return vl::TF_COMPRESSED_SLUMINANCE;
    if( value.getIdentifier() == "TF_COMPRESSED_SLUMINANCE_ALPHA") return vl::TF_COMPRESSED_SLUMINANCE_ALPHA;

    // sRGB OpenGL 2.1 / 3.x
    if( value.getIdentifier() == "TF_SRGB") return vl::TF_SRGB;
    if( value.getIdentifier() == "TF_SRGB8") return vl::TF_SRGB8;
    if( value.getIdentifier() == "TF_SRGB_ALPHA") return vl::TF_SRGB_ALPHA;
    if( value.getIdentifier() == "TF_SRGB8_ALPHA8") return vl::TF_SRGB8_ALPHA8;
    if( value.getIdentifier() == "TF_COMPRESSED_SRGB") return vl::TF_COMPRESSED_SRGB;
    if( value.getIdentifier() == "TF_COMPRESSED_SRGB_ALPHA") return vl::TF_COMPRESSED_SRGB_ALPHA;

    // GL_EXT_texture_sRGB compressed formats
    if( value.getIdentifier() == "TF_COMPRESSED_SRGB_S3TC_DXT1_EXT") return vl::TF_COMPRESSED_SRGB_S3TC_DXT1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT") return vl::TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT") return vl::TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
    if( value.getIdentifier() == "TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT") return vl::TF_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::TF_UNKNOWN;
  }

  inline const char* vlx_EUniformType(vl::EUniformType type)
  {
    switch(type)
    {
      default:
        return "UT_NONE";

      case vl::UT_INT:      return "UT_INT";
      case vl::UT_INT_VEC2: return "UT_INT_VEC2";
      case vl::UT_INT_VEC3: return "UT_INT_VEC3";
      case vl::UT_INT_VEC4: return "UT_INT_VEC4";

      case vl::UT_UNSIGNED_INT:      return "UT_UNSIGNED_INT";
      case vl::UT_UNSIGNED_INT_VEC2: return "UT_UNSIGNED_INT_VEC2";
      case vl::UT_UNSIGNED_INT_VEC3: return "UT_UNSIGNED_INT_VEC3";
      case vl::UT_UNSIGNED_INT_VEC4: return "UT_UNSIGNED_INT_VEC4";

      case vl::UT_FLOAT:      return "UT_FLOAT";
      case vl::UT_FLOAT_VEC2: return "UT_FLOAT_VEC2";
      case vl::UT_FLOAT_VEC3: return "UT_FLOAT_VEC3";
      case vl::UT_FLOAT_VEC4: return "UT_FLOAT_VEC4";

      case vl::UT_FLOAT_MAT2: return "UT_FLOAT_MAT2";
      case vl::UT_FLOAT_MAT3: return "UT_FLOAT_MAT3";
      case vl::UT_FLOAT_MAT4: return "UT_FLOAT_MAT4";

      case vl::UT_FLOAT_MAT2x3: return "UT_FLOAT_MAT2x3";
      case vl::UT_FLOAT_MAT3x2: return "UT_FLOAT_MAT3x2";
      case vl::UT_FLOAT_MAT2x4: return "UT_FLOAT_MAT2x4";
      case vl::UT_FLOAT_MAT4x2: return "UT_FLOAT_MAT4x2";
      case vl::UT_FLOAT_MAT3x4: return "UT_FLOAT_MAT3x4";
      case vl::UT_FLOAT_MAT4x3: return "UT_FLOAT_MAT4x3";

      case vl::UT_DOUBLE:      return "UT_DOUBLE";
      case vl::UT_DOUBLE_VEC2: return "UT_DOUBLE_VEC2";
      case vl::UT_DOUBLE_VEC3: return "UT_DOUBLE_VEC3";
      case vl::UT_DOUBLE_VEC4: return "UT_DOUBLE_VEC4";

      case vl::UT_DOUBLE_MAT2: return "UT_DOUBLE_MAT2";
      case vl::UT_DOUBLE_MAT3: return "UT_DOUBLE_MAT3";
      case vl::UT_DOUBLE_MAT4: return "UT_DOUBLE_MAT4";

      case vl::UT_DOUBLE_MAT2x3: return "UT_DOUBLE_MAT2x3";
      case vl::UT_DOUBLE_MAT3x2: return "UT_DOUBLE_MAT3x2";
      case vl::UT_DOUBLE_MAT2x4: return "UT_DOUBLE_MAT2x4";
      case vl::UT_DOUBLE_MAT4x2: return "UT_DOUBLE_MAT4x2";
      case vl::UT_DOUBLE_MAT3x4: return "UT_DOUBLE_MAT3x4";
      case vl::UT_DOUBLE_MAT4x3: return "UT_DOUBLE_MAT4x3";
    }
  }

  inline vl::EUniformType vlx_EUniformType(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "UT_INT") return vl::UT_INT;
    if( value.getIdentifier() == "UT_INT_VEC2") return vl::UT_INT_VEC2;
    if( value.getIdentifier() == "UT_INT_VEC3") return vl::UT_INT_VEC3;
    if( value.getIdentifier() == "UT_INT_VEC4") return vl::UT_INT_VEC4;

    if( value.getIdentifier() == "UT_UNSIGNED_INT") return vl::UT_UNSIGNED_INT;
    if( value.getIdentifier() == "UT_UNSIGNED_INT_VEC2") return vl::UT_UNSIGNED_INT_VEC2;
    if( value.getIdentifier() == "UT_UNSIGNED_INT_VEC3") return vl::UT_UNSIGNED_INT_VEC3;
    if( value.getIdentifier() == "UT_UNSIGNED_INT_VEC4") return vl::UT_UNSIGNED_INT_VEC4;

    if( value.getIdentifier() == "UT_FLOAT") return vl::UT_FLOAT;
    if( value.getIdentifier() == "UT_FLOAT_VEC2") return vl::UT_FLOAT_VEC2;
    if( value.getIdentifier() == "UT_FLOAT_VEC3") return vl::UT_FLOAT_VEC3;
    if( value.getIdentifier() == "UT_FLOAT_VEC4") return vl::UT_FLOAT_VEC4;

    if( value.getIdentifier() == "UT_FLOAT_MAT2") return vl::UT_FLOAT_MAT2;
    if( value.getIdentifier() == "UT_FLOAT_MAT3") return vl::UT_FLOAT_MAT3;
    if( value.getIdentifier() == "UT_FLOAT_MAT4") return vl::UT_FLOAT_MAT4;

    if( value.getIdentifier() == "UT_FLOAT_MAT2x3") return vl::UT_FLOAT_MAT2x3;
    if( value.getIdentifier() == "UT_FLOAT_MAT3x2") return vl::UT_FLOAT_MAT3x2;
    if( value.getIdentifier() == "UT_FLOAT_MAT2x4") return vl::UT_FLOAT_MAT2x4;
    if( value.getIdentifier() == "UT_FLOAT_MAT4x2") return vl::UT_FLOAT_MAT4x2;
    if( value.getIdentifier() == "UT_FLOAT_MAT3x4") return vl::UT_FLOAT_MAT3x4;
    if( value.getIdentifier() == "UT_FLOAT_MAT4x3") return vl::UT_FLOAT_MAT4x3;

    if( value.getIdentifier() == "UT_DOUBLE") return vl::UT_DOUBLE;
    if( value.getIdentifier() == "UT_DOUBLE_VEC2") return vl::UT_DOUBLE_VEC2;
    if( value.getIdentifier() == "UT_DOUBLE_VEC3") return vl::UT_DOUBLE_VEC3;
    if( value.getIdentifier() == "UT_DOUBLE_VEC4") return vl::UT_DOUBLE_VEC4;

    if( value.getIdentifier() == "UT_DOUBLE_MAT2") return vl::UT_DOUBLE_MAT2;
    if( value.getIdentifier() == "UT_DOUBLE_MAT3") return vl::UT_DOUBLE_MAT3;
    if( value.getIdentifier() == "UT_DOUBLE_MAT4") return vl::UT_DOUBLE_MAT4;

    if( value.getIdentifier() == "UT_DOUBLE_MAT2x3") return vl::UT_DOUBLE_MAT2x3;
    if( value.getIdentifier() == "UT_DOUBLE_MAT3x2") return vl::UT_DOUBLE_MAT3x2;
    if( value.getIdentifier() == "UT_DOUBLE_MAT2x4") return vl::UT_DOUBLE_MAT2x4;
    if( value.getIdentifier() == "UT_DOUBLE_MAT4x2") return vl::UT_DOUBLE_MAT4x2;
    if( value.getIdentifier() == "UT_DOUBLE_MAT3x4") return vl::UT_DOUBLE_MAT3x4;
    if( value.getIdentifier() == "UT_DOUBLE_MAT4x3") return vl::UT_DOUBLE_MAT4x3;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::UT_NONE;
  }

  inline const char* vlx_EDepthTextureMode(vl::EDepthTextureMode dtm)
  {
    switch(dtm)
    {
    default:
    case vl::DTM_RED: return "DTM_RED";
    case vl::DTM_LUMINANCE: return "DTM_LUMINANCE";
    case vl::DTM_INTENSITY: return "DTM_INTENSITY";
    case vl::DTM_ALPHA: return "DTM_ALPHA";
    }
  }

  inline vl::EDepthTextureMode vlx_EDepthTextureMode(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "DTM_LUMINANCE") return vl::DTM_LUMINANCE;
    if( value.getIdentifier() == "DTM_INTENSITY") return vl::DTM_INTENSITY;
    if( value.getIdentifier() == "DTM_ALPHA") return vl::DTM_ALPHA;
    if( value.getIdentifier() == "DTM_RED") return vl::DTM_RED;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::DTM_RED;
  }

  inline const char* vlx_ETexCompareMode(vl::ETexCompareMode tcm)
  {
    switch(tcm)
    {
    default:
    case vl::TCM_NONE: return "TCM_NONE";
    // case vl::TCM_COMPARE_R_TO_TEXTURE: return "TCM_COMPARE_R_TO_TEXTURE";
    case vl::TCM_COMPARE_REF_DEPTH_TO_TEXTURE: return "TCM_COMPARE_REF_DEPTH_TO_TEXTURE";
    }
  }

  inline vl::ETexCompareMode vlx_ETexCompareMode(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "TCM_COMPARE_R_TO_TEXTURE") return vl::TCM_COMPARE_R_TO_TEXTURE;
    if( value.getIdentifier() == "TCM_COMPARE_REF_DEPTH_TO_TEXTURE") return vl::TCM_COMPARE_REF_DEPTH_TO_TEXTURE;
    if( value.getIdentifier() == "TCM_NONE") return vl::TCM_NONE;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::TCM_NONE;
  }

  inline const char* vlx_ETexCompareFunc(vl::ETexCompareFunc tcf)
  {
    switch(tcf)
    {
    default:
    case vl::TCF_LEQUAL: return "TCF_LEQUAL";
    case vl::TCF_GEQUAL: return "TCF_GEQUAL";
    case vl::TCF_LESS: return "TCF_LESS";
    case vl::TCF_GREATER: return "TCF_GREATER";
    case vl::TCF_EQUAL: return "TCF_EQUAL";
    case vl::TCF_NOTEQUAL: return "TCF_NOTEQUAL";
    case vl::TCF_ALWAYS: return "TCF_ALWAYS";
    case vl::TCF_NEVER: return "TCF_NEVER";
    }
  }

  inline vl::ETexCompareFunc vlx_ETexCompareFunc(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "TCF_GEQUAL") return vl::TCF_GEQUAL;
    if( value.getIdentifier() == "TCF_LESS") return vl::TCF_LESS;
    if( value.getIdentifier() == "TCF_GREATER") return vl::TCF_GREATER;
    if( value.getIdentifier() == "TCF_EQUAL") return vl::TCF_EQUAL;
    if( value.getIdentifier() == "TCF_NOTEQUAL") return vl::TCF_NOTEQUAL;
    if( value.getIdentifier() == "TCF_ALWAYS") return vl::TCF_ALWAYS;
    if( value.getIdentifier() == "TCF_NEVER") return vl::TCF_NEVER;
    if( value.getIdentifier() == "TCF_LEQUAL") return vl::TCF_LEQUAL;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::TCF_LEQUAL;
  }

  inline const char* vlx_ETexParamFilter(vl::ETexParamFilter tpf)
  {
    switch(tpf)
    {
    default:
    case vl::TPF_NEAREST: return "TPF_NEAREST";
    case vl::TPF_LINEAR: return "TPF_LINEAR";
    case vl::TPF_NEAREST_MIPMAP_NEAREST: return "TPF_NEAREST_MIPMAP_NEAREST";
    case vl::TPF_LINEAR_MIPMAP_NEAREST: return "TPF_LINEAR_MIPMAP_NEAREST";
    case vl::TPF_NEAREST_MIPMAP_LINEAR: return "TPF_NEAREST_MIPMAP_LINEAR";
    case vl::TPF_LINEAR_MIPMAP_LINEAR: return "TPF_LINEAR_MIPMAP_LINEAR";
    }
  }

  inline vl::ETexParamFilter vlx_ETexParamFilter(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "TPF_LINEAR") return vl::TPF_LINEAR;
    if( value.getIdentifier() == "TPF_NEAREST_MIPMAP_NEAREST") return vl::TPF_NEAREST_MIPMAP_NEAREST;
    if( value.getIdentifier() == "TPF_LINEAR_MIPMAP_NEAREST") return vl::TPF_LINEAR_MIPMAP_NEAREST;
    if( value.getIdentifier() == "TPF_NEAREST_MIPMAP_LINEAR") return vl::TPF_NEAREST_MIPMAP_LINEAR;
    if( value.getIdentifier() == "TPF_LINEAR_MIPMAP_LINEAR") return vl::TPF_LINEAR_MIPMAP_LINEAR;
    if( value.getIdentifier() == "TPF_NEAREST") return vl::TPF_NEAREST;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::TPF_NEAREST;
  }

  inline const char* vlx_ETexParamWrap(vl::ETexParamWrap tpw)
  {
    switch(tpw)
    {
    default:
    case vl::TPW_REPEAT: return "TPW_REPEAT";
    case vl::TPW_CLAMP: return "TPW_CLAMP";
    case vl::TPW_CLAMP_TO_BORDER: return "TPW_CLAMP_TO_BORDER";
    case vl::TPW_CLAMP_TO_EDGE: return "TPW_CLAMP_TO_EDGE";
    case vl::TPW_MIRRORED_REPEAT: return "TPW_MIRRORED_REPEAT";
    }
  }

  inline vl::ETexParamWrap vlx_ETexParamWrap(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "TPW_CLAMP") return vl::TPW_CLAMP;
    if( value.getIdentifier() == "TPW_CLAMP_TO_BORDER") return vl::TPW_CLAMP_TO_BORDER;
    if( value.getIdentifier() == "TPW_CLAMP_TO_EDGE") return vl::TPW_CLAMP_TO_EDGE;
    if( value.getIdentifier() == "TPW_MIRRORED_REPEAT") return vl::TPW_MIRRORED_REPEAT;
    if( value.getIdentifier() == "TPW_REPEAT") return vl::TPW_REPEAT;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::TPW_REPEAT;
  }

  inline const char* vlx_EEnable(vl::EEnable en)
  {
    switch(en)
    {
    default:
      return "EN_UnknownEnable";
    case vl::EN_BLEND: return "EN_BLEND";
    case vl::EN_CULL_FACE: return "EN_CULL_FACE";
    case vl::EN_DEPTH_TEST: return "EN_DEPTH_TEST";
    case vl::EN_STENCIL_TEST: return "EN_STENCIL_TEST";
    case vl::EN_DITHER: return "EN_DITHER";
    case vl::EN_POLYGON_OFFSET_FILL: return "EN_POLYGON_OFFSET_FILL";
    case vl::EN_POLYGON_OFFSET_LINE: return "EN_POLYGON_OFFSET_LINE";
    case vl::EN_POLYGON_OFFSET_POINT: return "EN_POLYGON_OFFSET_POINT";
    case vl::EN_COLOR_LOGIC_OP: return "EN_COLOR_LOGIC_OP";
    case vl::EN_MULTISAMPLE: return "EN_MULTISAMPLE";
    case vl::EN_POINT_SMOOTH: return "EN_POINT_SMOOTH";
    case vl::EN_LINE_SMOOTH: return "EN_LINE_SMOOTH";
    case vl::EN_POLYGON_SMOOTH: return "EN_POLYGON_SMOOTH";
    case vl::EN_LINE_STIPPLE: return "EN_LINE_STIPPLE";
    case vl::EN_POLYGON_STIPPLE: return "EN_POLYGON_STIPPLE";
    case vl::EN_POINT_SPRITE: return "EN_POINT_SPRITE";
    case vl::EN_PROGRAM_POINT_SIZE: return "EN_PROGRAM_POINT_SIZE";
    case vl::EN_ALPHA_TEST: return "EN_ALPHA_TEST";
    case vl::EN_LIGHTING: return "EN_LIGHTING";
    case vl::EN_COLOR_SUM: return "EN_COLOR_SUM";
    case vl::EN_FOG: return "EN_FOG";
    case vl::EN_NORMALIZE: return "EN_NORMALIZE";
    case vl::EN_RESCALE_NORMAL: return "EN_RESCALE_NORMAL";
    case vl::EN_VERTEX_PROGRAM_TWO_SIDE: return "EN_VERTEX_PROGRAM_TWO_SIDE";
    case vl::EN_TEXTURE_CUBE_MAP_SEAMLESS: return "EN_TEXTURE_CUBE_MAP_SEAMLESS";
    case vl::EN_CLIP_DISTANCE0: return "EN_CLIP_DISTANCE0";
    case vl::EN_CLIP_DISTANCE1: return "EN_CLIP_DISTANCE1";
    case vl::EN_CLIP_DISTANCE2: return "EN_CLIP_DISTANCE2";
    case vl::EN_CLIP_DISTANCE3: return "EN_CLIP_DISTANCE3";
    case vl::EN_CLIP_DISTANCE4: return "EN_CLIP_DISTANCE4";
    case vl::EN_CLIP_DISTANCE5: return "EN_CLIP_DISTANCE5";
    case vl::EN_CLIP_DISTANCE6: return "EN_CLIP_DISTANCE6";
    case vl::EN_CLIP_DISTANCE7: return "EN_CLIP_DISTANCE7";
    case vl::EN_SAMPLE_ALPHA_TO_COVERAGE: return "EN_SAMPLE_ALPHA_TO_COVERAGE";
    case vl::EN_SAMPLE_ALPHA_TO_ONE: return "EN_SAMPLE_ALPHA_TO_ONE";
    case vl::EN_SAMPLE_COVERAGE: return "EN_SAMPLE_COVERAGE";
    }
  }

  inline vl::EEnable vlx_EEnable(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "EN_BLEND") return vl::EN_BLEND;
    if( value.getIdentifier() == "EN_CULL_FACE") return vl::EN_CULL_FACE;
    if( value.getIdentifier() == "EN_DEPTH_TEST") return vl::EN_DEPTH_TEST;
    if( value.getIdentifier() == "EN_STENCIL_TEST") return vl::EN_STENCIL_TEST;
    if( value.getIdentifier() == "EN_DITHER") return vl::EN_DITHER;
    if( value.getIdentifier() == "EN_POLYGON_OFFSET_FILL") return vl::EN_POLYGON_OFFSET_FILL;
    if( value.getIdentifier() == "EN_POLYGON_OFFSET_LINE") return vl::EN_POLYGON_OFFSET_LINE;
    if( value.getIdentifier() == "EN_POLYGON_OFFSET_POINT") return vl::EN_POLYGON_OFFSET_POINT;
    if( value.getIdentifier() == "EN_COLOR_LOGIC_OP") return vl::EN_COLOR_LOGIC_OP;
    if( value.getIdentifier() == "EN_MULTISAMPLE") return vl::EN_MULTISAMPLE;
    if( value.getIdentifier() == "EN_POINT_SMOOTH") return vl::EN_POINT_SMOOTH;
    if( value.getIdentifier() == "EN_LINE_SMOOTH") return vl::EN_LINE_SMOOTH;
    if( value.getIdentifier() == "EN_POLYGON_SMOOTH") return vl::EN_POLYGON_SMOOTH;
    if( value.getIdentifier() == "EN_LINE_STIPPLE") return vl::EN_LINE_STIPPLE;
    if( value.getIdentifier() == "EN_POLYGON_STIPPLE") return vl::EN_POLYGON_STIPPLE;
    if( value.getIdentifier() == "EN_POINT_SPRITE") return vl::EN_POINT_SPRITE;
    if( value.getIdentifier() == "EN_PROGRAM_POINT_SIZE") return vl::EN_PROGRAM_POINT_SIZE;
    if( value.getIdentifier() == "EN_ALPHA_TEST") return vl::EN_ALPHA_TEST;
    if( value.getIdentifier() == "EN_LIGHTING") return vl::EN_LIGHTING;
    if( value.getIdentifier() == "EN_COLOR_SUM") return vl::EN_COLOR_SUM;
    if( value.getIdentifier() == "EN_FOG") return vl::EN_FOG;
    if( value.getIdentifier() == "EN_NORMALIZE") return vl::EN_NORMALIZE;
    if( value.getIdentifier() == "EN_RESCALE_NORMAL") return vl::EN_RESCALE_NORMAL;
    if( value.getIdentifier() == "EN_VERTEX_PROGRAM_TWO_SIDE") return vl::EN_VERTEX_PROGRAM_TWO_SIDE;
    if( value.getIdentifier() == "EN_TEXTURE_CUBE_MAP_SEAMLESS") return vl::EN_TEXTURE_CUBE_MAP_SEAMLESS;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE0") return vl::EN_CLIP_DISTANCE0;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE1") return vl::EN_CLIP_DISTANCE1;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE2") return vl::EN_CLIP_DISTANCE2;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE3") return vl::EN_CLIP_DISTANCE3;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE4") return vl::EN_CLIP_DISTANCE4;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE5") return vl::EN_CLIP_DISTANCE5;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE6") return vl::EN_CLIP_DISTANCE6;
    if( value.getIdentifier() == "EN_CLIP_DISTANCE7") return vl::EN_CLIP_DISTANCE7;
    if( value.getIdentifier() == "EN_SAMPLE_ALPHA_TO_COVERAGE") return vl::EN_SAMPLE_ALPHA_TO_COVERAGE;
    if( value.getIdentifier() == "EN_SAMPLE_ALPHA_TO_ONE") return vl::EN_SAMPLE_ALPHA_TO_ONE;
    if( value.getIdentifier() == "EN_SAMPLE_COVERAGE") return vl::EN_SAMPLE_COVERAGE;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::EN_UnknownEnable;
  }

  inline vl::EPrimitiveType vlx_EPrimitiveType(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "PT_POINTS") return vl::PT_POINTS;
    if( value.getIdentifier() == "PT_LINES")  return vl::PT_LINES;
    if( value.getIdentifier() == "PT_LINE_LOOP") return vl::PT_LINE_LOOP;
    if( value.getIdentifier() == "PT_LINE_STRIP") return vl::PT_LINE_STRIP;
    if( value.getIdentifier() == "PT_TRIANGLES") return vl::PT_TRIANGLES;
    if( value.getIdentifier() == "PT_TRIANGLE_STRIP") return vl::PT_TRIANGLE_STRIP;
    if( value.getIdentifier() == "PT_TRIANGLE_FAN") return vl::PT_TRIANGLE_FAN;
    if( value.getIdentifier() == "PT_QUADS") return vl::PT_QUADS;
    if( value.getIdentifier() == "PT_QUAD_STRIP") return vl::PT_QUAD_STRIP;
    if( value.getIdentifier() == "PT_POLYGON") return vl::PT_POLYGON;
    if( value.getIdentifier() == "PT_LINES_ADJACENCY") return vl::PT_LINES_ADJACENCY;
    if( value.getIdentifier() == "PT_LINE_STRIP_ADJACENCY") return vl::PT_LINE_STRIP_ADJACENCY;
    if( value.getIdentifier() == "PT_TRIANGLES_ADJACENCY") return vl::PT_TRIANGLES_ADJACENCY;
    if( value.getIdentifier() == "PT_TRIANGLE_STRIP_ADJACENCY") return vl::PT_TRIANGLES_ADJACENCY;
    if( value.getIdentifier() == "PT_PATCHES") return vl::PT_PATCHES;
    if( value.getIdentifier() == "PT_UNKNOWN") return vl::PT_UNKNOWN;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::PT_UNKNOWN;
  }

  inline const char* vlx_EPrimitiveType(vl::EPrimitiveType type)
  {
    switch(type)
    {
      case vl::PT_POINTS:                   return "PT_POINTS"; break;
      case vl::PT_LINES:                    return "PT_LINES"; break;
      case vl::PT_LINE_LOOP:                return "PT_LINE_LOOP"; break;
      case vl::PT_LINE_STRIP:               return "PT_LINE_STRIP"; break;
      case vl::PT_TRIANGLES:                return "PT_TRIANGLES"; break;
      case vl::PT_TRIANGLE_STRIP:           return "PT_TRIANGLE_STRIP"; break;
      case vl::PT_TRIANGLE_FAN:             return "PT_TRIANGLE_FAN"; break;
      case vl::PT_QUADS:                    return "PT_QUADS"; break;
      case vl::PT_QUAD_STRIP:               return "PT_QUAD_STRIP"; break;
      case vl::PT_POLYGON:                  return "PT_POLYGON"; break;
      case vl::PT_LINES_ADJACENCY:          return "PT_LINES_ADJACENCY"; break;
      case vl::PT_LINE_STRIP_ADJACENCY:     return "PT_LINE_STRIP_ADJACENCY"; break;
      case vl::PT_TRIANGLES_ADJACENCY:      return "PT_TRIANGLES_ADJACENCY"; break;
      case vl::PT_TRIANGLE_STRIP_ADJACENCY: return "PT_TRIANGLE_STRIP_ADJACENCY"; break;
      case vl::PT_PATCHES:                  return "PT_PATCHES"; break;
      default:
      case vl::PT_UNKNOWN:                  return "PT_UNKNOWN"; break;
    }
  }

  inline vl::EVertexAttribInterpretation vlx_EVertexAttribInterpretation(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "VAI_NORMAL") return vl::VAI_NORMAL;
    if( value.getIdentifier() == "VAI_INTEGER")  return vl::VAI_INTEGER;
    if( value.getIdentifier() == "VAI_DOUBLE") return vl::VAI_DOUBLE;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::VAI_NORMAL;
  }

  inline const char* vlx_EVertexAttribInterpretation(vl::EVertexAttribInterpretation type)
  {
    switch(type)
    {
      default:
      case vl::VAI_NORMAL:  return "VAI_NORMAL";  break;
      case vl::VAI_INTEGER: return "VAI_INTEGER"; break;
      case vl::VAI_DOUBLE:  return "VAI_DOUBLE";  break;
    }
  }

  inline vl::ETextureDimension vlx_ETextureDimension(const VLXValue& value, VLXSerializer& s)
  {
    if( value.getIdentifier() == "TD_TEXTURE_1D") return vl::TD_TEXTURE_1D;
    if( value.getIdentifier() == "TD_TEXTURE_2D")  return vl::TD_TEXTURE_2D;
    if( value.getIdentifier() == "TD_TEXTURE_3D") return vl::TD_TEXTURE_3D;
    if( value.getIdentifier() == "TD_TEXTURE_CUBE_MAP") return vl::TD_TEXTURE_CUBE_MAP;
    if( value.getIdentifier() == "TD_TEXTURE_RECTANGLE") return vl::TD_TEXTURE_RECTANGLE;
    if( value.getIdentifier() == "TD_TEXTURE_1D_ARRAY") return vl::TD_TEXTURE_1D_ARRAY;
    if( value.getIdentifier() == "TD_TEXTURE_2D_ARRAY") return vl::TD_TEXTURE_2D_ARRAY;
    if( value.getIdentifier() == "TD_TEXTURE_BUFFER") return vl::TD_TEXTURE_BUFFER;
    if( value.getIdentifier() == "TD_TEXTURE_2D_MULTISAMPLE") return vl::TD_TEXTURE_2D_MULTISAMPLE;
    if( value.getIdentifier() == "TD_TEXTURE_2D_MULTISAMPLE_ARRAY") return vl::TD_TEXTURE_2D_MULTISAMPLE_ARRAY;
    if( value.getIdentifier() == "TD_TEXTURE_UNKNOWN") return vl::TD_TEXTURE_UNKNOWN;

    vl::Log::error( vl::Say("Line %n : unknown token '%s'.\n") << value.lineNumber() << value.getIdentifier() );
    s.setError(VLXSerializer::ImportError);
    return vl::TD_TEXTURE_UNKNOWN;
  }

  inline const char* vlx_ETextureDimension(vl::ETextureDimension td)
  {
    switch(td)
    {
    case vl::TD_TEXTURE_1D: return "TD_TEXTURE_1D";
    case vl::TD_TEXTURE_2D:  return "TD_TEXTURE_2D";
    case vl::TD_TEXTURE_3D: return "TD_TEXTURE_3D";
    case vl::TD_TEXTURE_CUBE_MAP: return "TD_TEXTURE_CUBE_MAP";
    case vl::TD_TEXTURE_RECTANGLE: return "TD_TEXTURE_RECTANGLE";
    case vl::TD_TEXTURE_1D_ARRAY: return "TD_TEXTURE_1D_ARRAY";
    case vl::TD_TEXTURE_2D_ARRAY: return "TD_TEXTURE_2D_ARRAY";
    case vl::TD_TEXTURE_BUFFER: return "TD_TEXTURE_BUFFER";
    case vl::TD_TEXTURE_2D_MULTISAMPLE: return "TD_TEXTURE_2D_MULTISAMPLE";
    case vl::TD_TEXTURE_2D_MULTISAMPLE_ARRAY: return "TD_TEXTURE_2D_MULTISAMPLE_ARRAY";
    case vl::TD_TEXTURE_UNKNOWN: return "TD_TEXTURE_UNKNOWN";
    default:
      vl::Log::error( vl::Say("Invalid texture dimension %n\n") << td );
      VL_TRAP()
      return "TD_TEXTURE_UNKNOWN";
    }
  }

}

#endif
