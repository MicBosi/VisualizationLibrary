/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
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

/**
 * \file vlnamespace.hpp
 * Visualization Library's enums in the 'vl' namespace.
*/

#ifndef vlnamespace_INCLUDE_ONCE
#define vlnamespace_INCLUDE_ONCE

#include <vl/OpenGL.hpp>
#include <vl/config.hpp>
#include <vl/version.hpp>

namespace vl
{
  typedef enum 
  {
    TF_ALPHA   = GL_ALPHA, 
    TF_ALPHA4  = GL_ALPHA4, 
    TF_ALPHA8  = GL_ALPHA8, 
    TF_ALPHA12 = GL_ALPHA12, 
    TF_ALPHA16 = GL_ALPHA16, 

    TF_INTENSITY   = GL_INTENSITY, 
    TF_INTENSITY4  = GL_INTENSITY4, 
    TF_INTENSITY8  = GL_INTENSITY8, 
    TF_INTENSITY12 = GL_INTENSITY12, 
    TF_INTENSITY16 = GL_INTENSITY16, 
    TF_LUMINANCE   = GL_LUMINANCE, 
    TF_LUMINANCE4  = GL_LUMINANCE4, 
    TF_LUMINANCE8  = GL_LUMINANCE8, 
    TF_LUMINANCE12 = GL_LUMINANCE12, 
    TF_LUMINANCE16 = GL_LUMINANCE16, 
    TF_LUMINANCE_ALPHA     = GL_LUMINANCE_ALPHA, 
    TF_LUMINANCE4_ALPHA4   = GL_LUMINANCE4_ALPHA4, 
    TF_LUMINANCE6_ALPHA2   = GL_LUMINANCE6_ALPHA2, 
    TF_LUMINANCE8_ALPHA8   = GL_LUMINANCE8_ALPHA8, 
    TF_LUMINANCE12_ALPHA4  = GL_LUMINANCE12_ALPHA4, 
    TF_LUMINANCE12_ALPHA12 = GL_LUMINANCE12_ALPHA12, 
    TF_LUMINANCE16_ALPHA16 = GL_LUMINANCE16_ALPHA16, 
    TF_R3_G3_B2 = GL_R3_G3_B2, 
    TF_RGB      = GL_RGB, 
    TF_RGB4     = GL_RGB4, 
    TF_RGB5     = GL_RGB5, 
    TF_RGB8     = GL_RGB8, 
    TF_RGB10    = GL_RGB10, 
    TF_RGB12    = GL_RGB12, 
    TF_RGB16    = GL_RGB16, 
    TF_RGBA     = GL_RGBA, 
    TF_RGBA2    = GL_RGBA2, 
    TF_RGBA4    = GL_RGBA4, 
    TF_RGB5_A1  = GL_RGB5_A1, 
    TF_RGBA8    = GL_RGBA8, 
    TF_RGB10_A2 = GL_RGB10_A2, 
    TF_RGBA12   = GL_RGBA12, 
    TF_RGBA16   = GL_RGBA16,

    // ARB_texture_float
    TF_RGBA32F = GL_RGBA32F_ARB,                      
    TF_RGB32F = GL_RGB32F_ARB,                       
    TF_ALPHA32F = GL_ALPHA32F_ARB,                     
    TF_INTENSITY32F = GL_INTENSITY32F_ARB,                 
    TF_LUMINANCE32F = GL_LUMINANCE32F_ARB,                 
    TF_LUMINANCE_ALPHA32F = GL_LUMINANCE_ALPHA32F_ARB,           
    TF_RGBA16F = GL_RGBA16F_ARB,                      
    TF_RGB16F = GL_RGB16F_ARB,                      
    TF_ALPHA16F = GL_ALPHA16F_ARB,                     
    TF_INTENSITY16F = GL_INTENSITY16F_ARB,                 
    TF_LUMINANCE16F = GL_LUMINANCE16F_ARB,                 
    TF_LUMINANCE_ALPHA16F = GL_LUMINANCE_ALPHA16F_ARB,           

    // ATI_texture_float (the enums are the same as ARB_texture_float)
    TF_RGBA_FLOAT32_ATI = GL_RGBA_FLOAT32_ATI,
    TF_RGB_FLOAT32_ATI = GL_RGB_FLOAT32_ATI,
    TF_ALPHA_FLOAT32_ATI = GL_ALPHA_FLOAT32_ATI,
    TF_INTENSITY_FLOAT32_ATI = GL_INTENSITY_FLOAT32_ATI,
    TF_LUMINANCE_FLOAT32_ATI = GL_LUMINANCE_FLOAT32_ATI,
    TF_LUMINANCE_ALPHA_FLOAT32_ATI = GL_LUMINANCE_ALPHA_FLOAT32_ATI,
    TF_RGBA_FLOAT16_ATI = GL_RGBA_FLOAT16_ATI,
    TF_RGB_FLOAT16_ATI = GL_RGB_FLOAT16_ATI,
    TF_ALPHA_FLOAT16_ATI = GL_ALPHA_FLOAT16_ATI,
    TF_INTENSITY_FLOAT16_ATI = GL_INTENSITY_FLOAT16_ATI,
    TF_LUMINANCE_FLOAT16_ATI = GL_LUMINANCE_FLOAT16_ATI,
    TF_LUMINANCE_ALPHA_FLOAT16_ATI = GL_LUMINANCE_ALPHA_FLOAT16_ATI,

    // EXT_texture_shared_exponent
    TF_RGB9_E5 = GL_RGB9_E5_EXT,

    // EXT_packed_float
    TF_11F_G11F_B10F = GL_R11F_G11F_B10F_EXT,

    // EXT_packed_depth_stencil
    TF_DEPTH_STENCIL = GL_DEPTH_STENCIL_EXT,

    // ARB_depth_buffer_float
    TF_DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F_NV,
    TF_DEPTH32F_STENCIL8  = GL_DEPTH32F_STENCIL8_NV,

    // ARB_depth_texture
    TF_DEPTH_COMPONENT   = GL_DEPTH_COMPONENT,
    TF_DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
    TF_DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,
    TF_DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32,

    // ARB_texture_compression
    TF_COMPRESSED_ALPHA           = GL_COMPRESSED_ALPHA_ARB,        
    TF_COMPRESSED_INTENSITY       = GL_COMPRESSED_INTENSITY_ARB,
    TF_COMPRESSED_LUMINANCE       = GL_COMPRESSED_LUMINANCE_ARB,      
    TF_COMPRESSED_LUMINANCE_ALPHA = GL_COMPRESSED_LUMINANCE_ALPHA_ARB,      
    TF_COMPRESSED_RGB             = GL_COMPRESSED_RGB_ARB,        
    TF_COMPRESSED_RGBA            = GL_COMPRESSED_RGBA_ARB,

    // 3DFX_texture_compression_FXT1
    TF_COMPRESSED_RGB_FXT1_3DFX  = GL_COMPRESSED_RGB_FXT1_3DFX,        
    TF_COMPRESSED_RGBA_FXT1_3DFX = GL_COMPRESSED_RGBA_FXT1_3DFX,        

    // EXT_texture_compression_s3tc
    TF_COMPRESSED_RGB_S3TC_DXT1  = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,                   
    TF_COMPRESSED_RGBA_S3TC_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,               
    TF_COMPRESSED_RGBA_S3TC_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,                  
    TF_COMPRESSED_RGBA_S3TC_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,                 

    // EXT_texture_compression_latc
    TF_COMPRESSED_LUMINANCE_LATC1        = GL_COMPRESSED_LUMINANCE_LATC1_EXT,                 
    TF_COMPRESSED_SIGNED_LUMINANCE_LATC1 = GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT,      
    TF_COMPRESSED_LUMINANCE_ALPHA_LATC2  = GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,        
    TF_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2 = GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT,

    // EXT_texture_compression_rgtc
    TF_COMPRESSED_RED_RGTC1        = GL_COMPRESSED_RED_RGTC1_EXT,                       
    TF_COMPRESSED_SIGNED_RED_RGTC1 = GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,                
    TF_COMPRESSED_RED_GREEN_RGTC2  = GL_COMPRESSED_RED_GREEN_RGTC2_EXT,                 
    TF_COMPRESSED_SIGNED_RED_GREEN_RGTC2 = GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT,

    // EXT_texture_integer
    TF_RGBA32UI = GL_RGBA32UI_EXT,           
    TF_RGB32UI = GL_RGB32UI_EXT,            
    TF_ALPHA32UI = GL_ALPHA32UI_EXT,          
    TF_INTENSITY32UI = GL_INTENSITY32UI_EXT,      
    TF_LUMINANCE32UI = GL_LUMINANCE32UI_EXT,      
    TF_LUMINANCE_ALPHA32UI = GL_LUMINANCE_ALPHA32UI_EXT,

    TF_RGBA16UI = GL_RGBA16UI_EXT,           
    TF_RGB16UI = GL_RGB16UI_EXT,            
    TF_ALPHA16UI = GL_ALPHA16UI_EXT,          
    TF_INTENSITY16UI = GL_INTENSITY16UI_EXT,      
    TF_LUMINANCE16UI = GL_LUMINANCE16UI_EXT,      
    TF_LUMINANCE_ALPHA16UI = GL_LUMINANCE_ALPHA16UI_EXT, 

    TF_RGBA8UI = GL_RGBA8UI_EXT,           
    TF_RGB8UI = GL_RGB8UI_EXT,            
    TF_ALPHA8UI = GL_ALPHA8UI_EXT,          
    TF_INTENSITY8UI = GL_INTENSITY8UI_EXT,      
    TF_LUMINANCE8UI = GL_LUMINANCE8UI_EXT,      
    TF_LUMINANCE_ALPHA8UI = GL_LUMINANCE_ALPHA8UI_EXT,

    TF_RGBA32I = GL_RGBA32I_EXT,           
    TF_RGB32I = GL_RGB32I_EXT,            
    TF_ALPHA32I = GL_ALPHA32I_EXT,          
    TF_INTENSITY32I = GL_INTENSITY32I_EXT,      
    TF_LUMINANCE32I = GL_LUMINANCE32I_EXT,      
    TF_LUMINANCE_ALPHA32I = GL_LUMINANCE_ALPHA32I_EXT,

    TF_RGBA16I = GL_RGBA16I_EXT,           
    TF_RGB16I = GL_RGB16I_EXT,            
    TF_ALPHA16I = GL_ALPHA16I_EXT,          
    TF_INTENSITY16I = GL_INTENSITY16I_EXT,      
    TF_LUMINANCE16I = GL_LUMINANCE16I_EXT,      
    TF_LUMINANCE_ALPHA16I = GL_LUMINANCE_ALPHA16I_EXT,

    TF_RGBA8I = GL_RGBA8I_EXT,
    TF_RGB8I = GL_RGB8I_EXT,
    TF_ALPHA8I = GL_ALPHA8I_EXT,
    TF_INTENSITY8I = GL_INTENSITY8I_EXT,
    TF_LUMINANCE8I = GL_LUMINANCE8I_EXT,
    TF_LUMINANCE_ALPHA8I = GL_LUMINANCE_ALPHA8I_EXT,

  } ETextureFormat;
    
  typedef enum
  {
    CBF_RGB   = GL_RGB,
    CBF_RGBA  = GL_RGBA,
    CBF_R3_G3_B2 = GL_R3_G3_B2,
    CBF_RGB4 = GL_RGB4,
    CBF_RGB5 = GL_RGB5,
    CBF_RGB8 = GL_RGB8,
    CBF_RGB10 = GL_RGB10, 
    CBF_RGB12 = GL_RGB12,
    CBF_RGB16 = GL_RGB16, 
    CBF_RGBA2 = GL_RGBA2, 
    CBF_RGBA4 = GL_RGBA4,
    CBF_RGB5_A1 = GL_RGB5_A1, 
    CBF_RGBA8 = GL_RGBA8, 
    CBF_RGB10_A2 = GL_RGB10_A2, 
    CBF_RGBA12 = GL_RGBA12,
    CBF_RGBA16 = GL_RGBA16,

    // GL_NV_float_buffer
    CBF_FLOAT_R_NV = GL_FLOAT_R_NV, 
    CBF_FLOAT_RG_NV = GL_FLOAT_RG_NV,
    CBF_FLOAT_RGB_NV = GL_FLOAT_RGB_NV, 
    CBF_FLOAT_RGBA_NV = GL_FLOAT_RGBA_NV,
    CBF_FLOAT_R16_NV = GL_FLOAT_R16_NV, 
    CBF_FLOAT_RG16_NV = GL_FLOAT_RG16_NV,
    CBF_FLOAT_RGB16_NV = GL_FLOAT_RGB16_NV, 
    CBF_FLOAT_RGBA16_NV = GL_FLOAT_RGBA16_NV,
    CBF_FLOAT_R32_NV = GL_FLOAT_R32_NV, 
    CBF_FLOAT_RG32_NV = GL_FLOAT_RG32_NV,
    CBF_FLOAT_RGB32_NV = GL_FLOAT_RGB32_NV, 
    CBF_FLOAT_RGBA32_NV = GL_FLOAT_RGBA32_NV,

    // EXT_texture_shared_exponent
    CBF_RGB9_E5 = GL_RGB9_E5_EXT,

    // EXT_packed_float
    CBF_11F_G11F_B10F = GL_R11F_G11F_B10F_EXT,

  } EColorBufferFormat;

  typedef enum
  {
    AP_NO_ATTACHMENT = 0,

    AP_COLOR_ATTACHMENT0  = GL_COLOR_ATTACHMENT0_EXT,
    AP_COLOR_ATTACHMENT1  = GL_COLOR_ATTACHMENT1_EXT,
    AP_COLOR_ATTACHMENT2  = GL_COLOR_ATTACHMENT2_EXT,
    AP_COLOR_ATTACHMENT3  = GL_COLOR_ATTACHMENT3_EXT,
    AP_COLOR_ATTACHMENT4  = GL_COLOR_ATTACHMENT4_EXT,
    AP_COLOR_ATTACHMENT5  = GL_COLOR_ATTACHMENT5_EXT,
    AP_COLOR_ATTACHMENT6  = GL_COLOR_ATTACHMENT6_EXT,
    AP_COLOR_ATTACHMENT7  = GL_COLOR_ATTACHMENT7_EXT,
    AP_COLOR_ATTACHMENT8  = GL_COLOR_ATTACHMENT8_EXT,
    AP_COLOR_ATTACHMENT9  = GL_COLOR_ATTACHMENT9_EXT,
    AP_COLOR_ATTACHMENT10 = GL_COLOR_ATTACHMENT10_EXT,
    AP_COLOR_ATTACHMENT11 = GL_COLOR_ATTACHMENT11_EXT,
    AP_COLOR_ATTACHMENT12 = GL_COLOR_ATTACHMENT12_EXT,
    AP_COLOR_ATTACHMENT13 = GL_COLOR_ATTACHMENT13_EXT,
    AP_COLOR_ATTACHMENT14 = GL_COLOR_ATTACHMENT14_EXT,
    AP_COLOR_ATTACHMENT15 = GL_COLOR_ATTACHMENT15_EXT,

    AP_DEPTH_ATTACHMENT   = GL_DEPTH_ATTACHMENT_EXT,

    AP_STENCIL_ATTACHMENT = GL_STENCIL_ATTACHMENT_EXT

  } EAttachmentPoint;

  typedef enum
  {
    T2DT_TEXTURE_2D = GL_TEXTURE_2D,
    T2DT_TEXTURE_CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    T2DT_TEXTURE_CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    T2DT_TEXTURE_CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    T2DT_TEXTURE_CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    T2DT_TEXTURE_CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    T2DT_TEXTURE_CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
    T2DT_TEXTURE_RECTANGLE           = GL_TEXTURE_RECTANGLE_ARB,
    T2DT_TEXTURE_1D_ARRAY            = GL_TEXTURE_1D_ARRAY_EXT
  } ETex2DTarget;

  typedef enum
  {
    IF_RGB   = GL_RGB,
    IF_RGBA  = GL_RGBA,
    IF_BGR   = GL_BGR,
    IF_BGRA  = GL_BGRA,
    IF_RED   = GL_RED,
    IF_GREEN = GL_GREEN,
    IF_BLUE  = GL_BLUE,
    IF_ALPHA = GL_ALPHA,
    IF_LUMINANCE       = GL_LUMINANCE,
    IF_LUMINANCE_ALPHA = GL_LUMINANCE_ALPHA,
    IF_DEPTH_COMPONENT = GL_DEPTH_COMPONENT,
    IF_STENCIL_INDEX   = GL_STENCIL_INDEX,

    // EXT_packed_depth_stencil
    IF_DEPTH_STENCIL = GL_DEPTH_STENCIL_EXT,

    // compressed formats
    // note: with these format the type must be IT_COMPRESSED_TYPE

    IF_COMPRESSED_RGB_S3TC_DXT1  = GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    IF_COMPRESSED_RGBA_S3TC_DXT1 = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    IF_COMPRESSED_RGBA_S3TC_DXT3 = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    IF_COMPRESSED_RGBA_S3TC_DXT5 = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

    // EXT_texture_integer
    IF_RED_INTEGER = GL_RED_INTEGER_EXT,
    IF_GREEN_INTEGER = GL_GREEN_INTEGER_EXT,
    IF_BLUE_INTEGER = GL_BLUE_INTEGER_EXT,
    IF_ALPHA_INTEGER = GL_ALPHA_INTEGER_EXT,
    IF_RGB_INTEGER = GL_RGB_INTEGER_EXT,
    IF_RGBA_INTEGER = GL_RGBA_INTEGER_EXT,
    IF_BGR_INTEGER = GL_BGR_INTEGER_EXT,
    IF_BGRA_INTEGER = GL_BGRA_INTEGER_EXT,
    IF_LUMINANCE_INTEGER = GL_LUMINANCE_INTEGER_EXT,
    IF_LUMINANCE_ALPHA_INTEGER = GL_LUMINANCE_ALPHA_INTEGER_EXT,

  } EImageFormat;

  typedef enum
  {
    DT_DEPTH_COMPONENT   = GL_DEPTH_COMPONENT,
    DT_DEPTH_COMPONENT16 = GL_DEPTH_COMPONENT16,
    DT_DEPTH_COMPONENT24 = GL_DEPTH_COMPONENT24,
    DT_DEPTH_COMPONENT32 = GL_DEPTH_COMPONENT32,
    DT_DEPTH_COMPONENT32F = GL_DEPTH_COMPONENT32F_NV,
  } EDepthType;

  typedef enum
  {
    ST_STENCIL_INDEX1  = GL_STENCIL_INDEX1_EXT,
    ST_STENCIL_INDEX4  = GL_STENCIL_INDEX4_EXT,
    ST_STENCIL_INDEX8  = GL_STENCIL_INDEX8_EXT,
    ST_STENCIL_INDEX16 = GL_STENCIL_INDEX16_EXT
  } EStencilType;

  typedef enum
  {
    DST_DEPTH_STENCIL     = GL_DEPTH_STENCIL_EXT,
    DST_DEPTH24_STENCIL8  = GL_DEPTH_STENCIL_EXT,
    DST_DEPTH32F_STENCIL8 = GL_DEPTH32F_STENCIL8_NV,
  } EDepthStencilType;

  typedef enum
  {
    CF_DO_NOT_CLEAR        = 0,
    CF_CLEAR_COLOR         = GL_COLOR_BUFFER_BIT,
    CF_CLEAR_DEPTH         = GL_DEPTH_BUFFER_BIT,
    CF_CLEAR_STENCIL       = GL_STENCIL_BUFFER_BIT,
    CF_CLEAR_COLOR_DEPTH   = CF_CLEAR_COLOR + CF_CLEAR_DEPTH,
    CF_CLEAR_COLOR_STENCIL = CF_CLEAR_COLOR + CF_CLEAR_STENCIL,
    CF_CLEAR_DEPTH_STENCIL = CF_CLEAR_DEPTH + CF_CLEAR_STENCIL,
    CF_CLEAR_COLOR_DEPTH_STENCIL = CF_CLEAR_COLOR + CF_CLEAR_DEPTH + CF_CLEAR_STENCIL,
  } EClearFlags;
  
  typedef enum
  {
    CCM_Float,
    CCM_Int,
    CCM_UInt
  } EClearColorMode;

  typedef enum 
  { 
    BF_ZERO=GL_ZERO, 
    BF_ONE=GL_ONE, 
    BF_SRC_COLOR=GL_SRC_COLOR, 
    BF_ONE_MINUS_SRC_COLOR=GL_ONE_MINUS_SRC_COLOR, 
    BF_DST_COLOR=GL_DST_COLOR, 
    BF_ONE_MINUS_DST_COLOR=GL_ONE_MINUS_DST_COLOR, 
    BF_SRC_ALPHA=GL_SRC_ALPHA, 
    BF_ONE_MINUS_SRC_ALPHA=GL_ONE_MINUS_SRC_ALPHA, 
    BF_DST_ALPHA=GL_DST_ALPHA, 
    BF_ONE_MINUS_DST_ALPHA=GL_ONE_MINUS_DST_ALPHA, 
    BF_CONSTANT_COLOR=GL_CONSTANT_COLOR, 
    BF_ONE_MINUS_CONSTANT_COLOR=GL_ONE_MINUS_CONSTANT_COLOR, 
    BF_CONSTANT_ALPHA=GL_CONSTANT_ALPHA, 
    BF_ONE_MINUS_CONSTANT_ALPHA=GL_ONE_MINUS_CONSTANT_ALPHA, 
    BF_SRC_ALPHA_SATURATE=GL_SRC_ALPHA_SATURATE
  } EBlendFactor;

  typedef enum 
  { 
    TD_TEXTURE_UNKNOWN = 0,
    TD_TEXTURE_1D = GL_TEXTURE_1D, 
    TD_TEXTURE_2D = GL_TEXTURE_2D, 
    TD_TEXTURE_3D = GL_TEXTURE_3D, 
    TD_TEXTURE_CUBE_MAP  = GL_TEXTURE_CUBE_MAP,
    TD_TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE_ARB,
    TD_TEXTURE_1D_ARRAY  = GL_TEXTURE_1D_ARRAY_EXT, 
    TD_TEXTURE_2D_ARRAY  = GL_TEXTURE_2D_ARRAY_EXT, 
  } ETextureDimension;

  typedef enum
  {
    TCM_NONE = GL_NONE,
    TCM_COMPARE_R_TO_TEXTURE = GL_COMPARE_R_TO_TEXTURE,
    TCM_COMPARE_REF_DEPTH_TO_TEXTURE = GL_COMPARE_REF_DEPTH_TO_TEXTURE_EXT
  } ETexCompareMode;

  typedef enum
  { 
    TCF_LEQUAL   = GL_LEQUAL,
    TCF_GEQUAL   = GL_GEQUAL,
    TCF_LESS     = GL_LESS,
    TCF_GREATER  = GL_GREATER,
    TCF_EQUAL    = GL_EQUAL,
    TCF_NOTEQUAL = GL_NOTEQUAL,
    TCF_ALWAYS   = GL_ALWAYS,
    TCF_NEVER    = GL_NEVER
  } ETexCompareFunc;

  typedef enum 
  {
    DTM_LUMINANCE = GL_LUMINANCE, 
    DTM_INTENSITY = GL_INTENSITY, 
    DTM_ALPHA     = GL_ALPHA
  } EDepthTextureMode;

  typedef enum
  {
    RDB_NONE        = GL_NONE,
    RDB_FRONT_LEFT  = GL_FRONT_LEFT,
    RDB_FRONT_RIGHT = GL_FRONT_RIGHT,
    RDB_BACK_LEFT   = GL_BACK_LEFT,
    RDB_BACK_RIGHT  = GL_BACK_RIGHT,
    RDB_AUX0        = GL_AUX0,
    RDB_AUX1        = GL_AUX1,
    RDB_AUX2        = GL_AUX2,
    RDB_AUX3        = GL_AUX3,
    RDB_COLOR_ATTACHMENT0  = GL_COLOR_ATTACHMENT0_EXT,
    RDB_COLOR_ATTACHMENT1  = GL_COLOR_ATTACHMENT1_EXT,
    RDB_COLOR_ATTACHMENT2  = GL_COLOR_ATTACHMENT2_EXT,
    RDB_COLOR_ATTACHMENT3  = GL_COLOR_ATTACHMENT3_EXT,
    RDB_COLOR_ATTACHMENT4  = GL_COLOR_ATTACHMENT4_EXT,
    RDB_COLOR_ATTACHMENT5  = GL_COLOR_ATTACHMENT5_EXT,
    RDB_COLOR_ATTACHMENT6  = GL_COLOR_ATTACHMENT6_EXT,
    RDB_COLOR_ATTACHMENT7  = GL_COLOR_ATTACHMENT7_EXT,
    RDB_COLOR_ATTACHMENT8  = GL_COLOR_ATTACHMENT8_EXT,
    RDB_COLOR_ATTACHMENT9  = GL_COLOR_ATTACHMENT9_EXT,
    RDB_COLOR_ATTACHMENT10 = GL_COLOR_ATTACHMENT10_EXT,
    RDB_COLOR_ATTACHMENT11 = GL_COLOR_ATTACHMENT11_EXT,
    RDB_COLOR_ATTACHMENT12 = GL_COLOR_ATTACHMENT12_EXT,
    RDB_COLOR_ATTACHMENT13 = GL_COLOR_ATTACHMENT13_EXT,
    RDB_COLOR_ATTACHMENT14 = GL_COLOR_ATTACHMENT14_EXT,
    RDB_COLOR_ATTACHMENT15 = GL_COLOR_ATTACHMENT15_EXT
  } EReadDrawBuffer;

  typedef enum
  {
    IT_COMPRESSED_TYPE = 0,

    IT_UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
    IT_BYTE           = GL_BYTE,
    IT_UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
    IT_SHORT          = GL_SHORT,
    IT_UNSIGNED_INT   = GL_UNSIGNED_INT,
    IT_INT            = GL_INT,
    IT_FLOAT          = GL_FLOAT,
    IT_UNSIGNED_BYTE_3_3_2         = GL_UNSIGNED_BYTE_3_3_2,
    IT_UNSIGNED_BYTE_2_3_3_REV     = GL_UNSIGNED_BYTE_2_3_3_REV,
    IT_UNSIGNED_SHORT_5_6_5        = GL_UNSIGNED_SHORT_5_6_5,
    IT_UNSIGNED_SHORT_5_6_5_REV    = GL_UNSIGNED_SHORT_5_6_5_REV,
    IT_UNSIGNED_SHORT_4_4_4_4      = GL_UNSIGNED_SHORT_4_4_4_4,
    IT_UNSIGNED_SHORT_4_4_4_4_REV  = GL_UNSIGNED_SHORT_4_4_4_4_REV,
    IT_UNSIGNED_SHORT_5_5_5_1      = GL_UNSIGNED_SHORT_5_5_5_1,
    IT_UNSIGNED_SHORT_1_5_5_5_REV  = GL_UNSIGNED_SHORT_1_5_5_5_REV,
    IT_UNSIGNED_INT_8_8_8_8        = GL_UNSIGNED_INT_8_8_8_8,
    IT_UNSIGNED_INT_8_8_8_8_REV    = GL_UNSIGNED_INT_8_8_8_8_REV,
    IT_UNSIGNED_INT_10_10_10_2     = GL_UNSIGNED_INT_10_10_10_2,
    IT_UNSIGNED_INT_2_10_10_10_REV = GL_UNSIGNED_INT_2_10_10_10_REV,

    IT_UNSIGNED_INT_5_9_9_9_REV       = GL_UNSIGNED_INT_5_9_9_9_REV_EXT,     /* EXT_texture_shared_exponent, supports only GL_RGB */
    IT_UNSIGNED_INT_10F_11F_11F_REV   = GL_UNSIGNED_INT_10F_11F_11F_REV_EXT, /* EXT_packed_float, supports only GL_RGB */
    IT_UNSIGNED_INT_24_8              = GL_UNSIGNED_INT_24_8_EXT,            /* EXT_packed_depth_stencil*/
    IT_FLOAT_32_UNSIGNED_INT_24_8_REV = GL_FLOAT_32_UNSIGNED_INT_24_8_REV_NV /* ARB_depth_buffer_float */

  } EImageType;

  typedef enum
  {
    PT_POINTS         = GL_POINTS,
    PT_LINES          = GL_LINES,
    PT_LINE_LOOP      = GL_LINE_LOOP,
    PT_LINE_STRIP     = GL_LINE_STRIP,
    PT_TRIANGLES      = GL_TRIANGLES,
    PT_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    PT_TRIANGLE_FAN   = GL_TRIANGLE_FAN,
    PT_QUADS          = GL_QUADS,
    PT_QUAD_STRIP     = GL_QUAD_STRIP,
    PT_POLYGON        = GL_POLYGON,

    // geometry shader
    PT_LINES_ADJACENCY          = GL_LINES_ADJACENCY_EXT,
    PT_LINE_STRIP_ADJACENCY     = GL_LINE_STRIP_ADJACENCY_EXT,
    PT_TRIANGLES_ADJACENCY      = GL_TRIANGLES_ADJACENCY_EXT,
    PT_TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY_EXT
  } EPrimitiveType;

  typedef enum 
  { 
    PF_FRONT=GL_FRONT, 
    PF_BACK=GL_BACK, 
    PF_FRONT_AND_BACK=GL_FRONT_AND_BACK 
  } EPolygonFace;

  typedef enum 
  { 
    HM_FASTEST=GL_FASTEST, 
    HM_NICEST=GL_NICEST, 
    HM_DONT_CARE=GL_DONT_CARE 
  } EHintMode;

  typedef enum 
  { 
    FF_CW=GL_CW, 
    FF_CCW=GL_CCW 
  } EFrontFace;

  typedef enum 
  { 
    FU_NEVER=GL_NEVER, 
    FU_LESS=GL_LESS, 
    FU_EQUAL=GL_EQUAL, 
    FU_LEQUAL=GL_LEQUAL, 
    FU_GREATER=GL_GREATER, 
    FU_NOTEQUAL=GL_NOTEQUAL, 
    FU_GEQUAL=GL_GEQUAL, 
    FU_ALWAYS=GL_ALWAYS 
  } EFunction;

  typedef enum 
  { 
    PM_FILL=GL_FILL, 
    PM_LINE=GL_LINE, 
    PM_POINT=GL_POINT 
  } EPolygonMode;

  typedef enum 
  { 
    SM_FLAT=GL_FLAT, 
    SM_SMOOTH=GL_SMOOTH 
  } EShadeModel;

  typedef enum 
  {
    BE_FUNC_ADD=GL_FUNC_ADD, 
    BE_FUNC_SUBTRACT=GL_FUNC_SUBTRACT, 
    BE_FUNC_REVERSE_SUBTRACT=GL_FUNC_REVERSE_SUBTRACT, 
    BE_MIN=GL_MIN, 
    BE_MAX=GL_MAX 
  } EBlendEquation;

  typedef enum 
  { 
    CM_EMISSION=GL_EMISSION, 
    CM_AMBIENT=GL_AMBIENT, 
    CM_DIFFUSE=GL_DIFFUSE, 
    CM_SPECULAR=GL_SPECULAR, 
    CM_AMBIENT_AND_DIFFUSE=GL_AMBIENT_AND_DIFFUSE 
  } EColorMaterial;

  typedef enum 
  { 
    CC_SEPARATE_SPECULAR_COLOR=GL_SEPARATE_SPECULAR_COLOR, 
    CC_SINGLE_COLOR=GL_SINGLE_COLOR 
  } EColorControl;

  typedef enum 
  {
    FM_LINEAR=GL_LINEAR, 
    FM_EXP=GL_EXP, 
    FM_EXP2=GL_EXP2 
  } EFogMode;

  typedef enum 
  { 
    LO_CLEAR=GL_CLEAR, 
    LO_SET=GL_SET, 
    LO_COPY=GL_COPY, 
    LO_COPY_INVERTED=GL_COPY_INVERTED, 
    LO_NOOP=GL_NOOP, 
    LO_INVERT=GL_INVERT, 
    LO_AND=GL_AND, 
    LO_NAND=GL_NAND, 
    LO_OR=GL_OR, 
    LO_NOR=GL_NOR, 
    LO_XOR=GL_XOR, 
    LO_EQUIV=GL_EQUIV, 
    LO_AND_REVERSE=GL_AND_REVERSE, 
    LO_AND_INVERTED=GL_AND_INVERTED, 
    LO_OR_REVERSE=GL_OR_REVERSE, 
    LO_OR_INVERTED=GL_OR_INVERTED 
  } ELogicOp;

  typedef enum 
  { 
    SO_KEEP=GL_KEEP, 
    SO_ZERO=GL_ZERO, 
    SO_REPLACE=GL_REPLACE, 
    SO_INCR=GL_INCR, 
    SO_INCR_WRAP=GL_INCR_WRAP, 
    SO_DECR=GL_DECR, 
    SO_DECR_WRAP=GL_DECR_WRAP, 
    SO_INVERT=GL_INVERT 
  } EStencilOp;

 typedef enum 
  { 
    TPF_NEAREST=GL_NEAREST, 
    TPF_LINEAR=GL_LINEAR, 
    TPF_NEAREST_MIPMAP_NEAREST=GL_NEAREST_MIPMAP_NEAREST, 
    TPF_LINEAR_MIPMAP_NEAREST=GL_LINEAR_MIPMAP_NEAREST, 
    TPF_NEAREST_MIPMAP_LINEAR=GL_NEAREST_MIPMAP_LINEAR, 
    TPF_LINEAR_MIPMAP_LINEAR=GL_LINEAR_MIPMAP_LINEAR 
  } ETexParamFilter;

  typedef enum 
  { 
    TPW_CLAMP           = GL_CLAMP, 
    TPW_CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
    TPW_CLAMP_TO_EDGE   = GL_CLAMP_TO_EDGE,
    TPW_MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
    TPW_REPEAT          = GL_REPEAT
  } ETexParamWrap;

  typedef enum 
  {
    TEM_DECAL=GL_DECAL, 
    TEM_MODULATE=GL_MODULATE, 
    TEM_ADD=GL_ADD, 
    TEM_BLEND=GL_BLEND, 
    TEM_REPLACE=GL_REPLACE, 
    TEM_COMBINE=GL_COMBINE, 
    TEM_ADD_SIGN=GL_ADD_SIGNED, 
    TEM_INTERPOLATE=GL_INTERPOLATE, 
    TEM_SUBTRACT=GL_SUBTRACT, 
    TEM_DOT3_RGB=GL_DOT3_RGB, 
    TEM_DOT3_RGBA=GL_DOT3_RGBA 
  } ETexEnvMode;

  typedef enum 
  {
    TES_TEXTURE=GL_TEXTURE, 
    TES_TEXTURE0=GL_TEXTURE0, 
    TES_TEXTURE1=GL_TEXTURE1, 
    TES_TEXTURE2=GL_TEXTURE2, 
    TES_TEXTURE3=GL_TEXTURE3, 
    TES_TEXTURE4=GL_TEXTURE4, 
    TES_TEXTURE5=GL_TEXTURE5, 
    TES_TEXTURE6=GL_TEXTURE6, 
    TES_TEXTURE7=GL_TEXTURE7, 
    TES_CONSTANT=GL_CONSTANT,
    TES_PRIMARY_COLOR=GL_PRIMARY_COLOR, 
    TES_PREVIOUS=GL_PREVIOUS
  } ETexEnvSource;

  typedef enum 
  {
    TEO_SRC_COLOR=GL_SRC_COLOR, 
    TEO_ONE_MINUS_SRC_COLOR=GL_ONE_MINUS_SRC_COLOR,
    TEO_SRC_ALPHA=GL_SRC_ALPHA, 
    TEO_ONE_MINUS_SRC_ALPHA=GL_ONE_MINUS_SRC_ALPHA
  } ETexEnvOperand;

  //! Texture generation modes, see http://www.opengl.org/sdk/docs/man/xhtml/glTexGen.xml for more information.
  typedef enum 
  {
    TGM_DISABLED = 0, 
    TGM_EYE_LINEAR=GL_EYE_LINEAR, 
    TGM_OBJECT_LINEAR=GL_OBJECT_LINEAR, 
    TGM_SPHERE_MAP=GL_SPHERE_MAP, 
    TGM_REFLECTION_MAP=GL_REFLECTION_MAP, 
    TGM_NORMAL_MAP=GL_NORMAL_MAP
  } ETexGenMode;

  //! Constant that enable/disable a specific OpenGL feature, see also Shader, Shader::enable(), Shader::disable(), Shader::isEnabled()
  typedef enum 
  {
    EN_ALPHA_TEST, //!< If enabled, performs alpha testing, see also AlphaFunc for more information.
    EN_BLEND, //!< If enabled, blend the incoming RGBA color values with the values in the color buffers, see also BlendFunc for more information.
    EN_COLOR_LOGIC_OP, //!< If enabled, apply the currently selected logical operation to the incoming RGBA color and color buffer values, see also LogicOp.
    EN_LIGHTING, //!< If enabled, use the current lighting parameters to compute the vertex color; Otherwise, simply associate the current color with each vertex, see also Material, LightModel, and Light.
    EN_COLOR_SUM, //!< If enabled, add the secondary color value to the computed fragment color. 
    EN_CULL_FACE, //!< If enabled, cull polygons based on their winding in window coordinates, see also CullFace. 
    EN_DEPTH_TEST, //!< If enabled, do depth comparisons and update the depth buffer; Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled, see also DepthFunc and DepthRange.
    EN_FOG, //!< If enabled, blend a fog color into the post-texturing color, see also Fog.
    EN_LINE_SMOOTH, //!< IIf enabled, draw lines with correct filtering; Otherwise, draw aliased lines, see also LineWidth.
    EN_LINE_STIPPLE, //!< If enabled, use the current line stipple pattern when drawing lines, see also LineStipple.
    EN_POLYGON_STIPPLE, //!< If enabled, use the current polygon stipple pattern when rendering polygons, see also PolygonStipple.
    EN_NORMALIZE, //!< If enabled, normal vectors are scaled to unit length after transformation, see also vl::EN_RESCALE_NORMAL.
    EN_POINT_SMOOTH, //!< If enabled, draw points with proper filtering; Otherwise, draw aliased points, see also PointSize.
    EN_POINT_SPRITE, //!< If enabled, calculate texture coordinates for points based on texture environment and point parameter settings; Otherwise texture coordinates are constant across points.
    EN_POLYGON_SMOOTH, //!< If enabled, draw polygons with proper filtering; Otherwise, draw aliased polygons; For correct antialiased polygons, an alpha buffer is needed and the polygons must be sorted front to back.
    EN_POLYGON_OFFSET_FILL, //!< If enabled, and if the polygon is rendered in GL_FILL mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed, see also PolygonOffset.
    EN_POLYGON_OFFSET_LINE, //!< If enabled, and if the polygon is rendered in GL_LINE mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed, see also PolygonOffset.
    EN_POLYGON_OFFSET_POINT, //!< If enabled, an offset is added to depth values of a polygon's fragments before the depth comparison is performed, if the polygon is rendered in GL_POINT mode, see also PolygonOffset.
    EN_RESCALE_NORMAL, //!< If enabled, normals are scaled by a scaling factor derived from the modelview matrix; vl::EN_RESCALE_NORMAL requires that the originally specified normals were of unit length, and that the modelview matrix contain only uniform scales for proper results, see also vl::EN_NORMALIZE.
    EN_STENCIL_TEST, //!< If enabled, do stencil testing and update the stencil buffer, see also StencilFunc and StencilOp.
    EN_VERTEX_PROGRAM_POINT_SIZE, //!< If enabled, and a vertex shader is active, then the derived point size is taken from the (potentially clipped) shader builtin \p gl_PointSize and clamped to the implementation-dependent point size range|
    EN_VERTEX_PROGRAM_TWO_SIDE, //!< If enabled, and a vertex shader is active, it specifies that the GL will choose between front and back colors based on the polygon's face direction of which the vertex being shaded is a part; It has no effect on points or lines.

    // OpenGL 3
    EN_TEXTURE_CUBE_MAP_SEAMLESS,
    EN_GL_CLIP_DISTANCE0,
    EN_GL_CLIP_DISTANCE1,
    EN_GL_CLIP_DISTANCE2,
    EN_GL_CLIP_DISTANCE3,
    EN_GL_CLIP_DISTANCE4,
    EN_GL_CLIP_DISTANCE5,

    // multisampling
    EN_MULTISAMPLE, //!< If enabled, use multiple fragment samples in computing the final color of a pixel, see also SampleCoverage.
    EN_SAMPLE_ALPHA_TO_COVERAGE, //!< If enabled, compute a temporary coverage value where each bit is determined by the alpha value at the corresponding sample location; The temporary coverage value is then ANDed with the fragment coverage value.
    EN_SAMPLE_ALPHA_TO_ONE, //!< If enabled, each sample alpha value is replaced by the maximum representable alpha value.
    EN_SAMPLE_COVERAGE, //!< If enabled, the fragment's coverage is ANDed with the temporary coverage value; If GL_SAMPLE_COVERAGE_INVERT is set to GL_TRUE, invert the coverage value, see also SampleCoverage.

    EN_EnableCount, //!< For internal use only.

    EN_UnknownEnable //!< For internal use only.

  } EEnable;

  typedef enum
  {
    GBU_STREAM_DRAW = GL_STREAM_DRAW, 
    GBU_STREAM_READ = GL_STREAM_READ, 
    GBU_STREAM_COPY = GL_STREAM_COPY, 
    GBU_STATIC_DRAW = GL_STATIC_DRAW, 
    GBU_STATIC_READ = GL_STATIC_READ, 
    GBU_STATIC_COPY = GL_STATIC_COPY, 
    GBU_DYNAMIC_DRAW = GL_DYNAMIC_DRAW, 
    GBU_DYNAMIC_READ = GL_DYNAMIC_READ, 
    GBU_DYNAMIC_COPY = GL_DYNAMIC_COPY
  } EGLBufferUsage;

  typedef enum
  {
    GBA_READ_ONLY  = GL_READ_ONLY,
    GBA_WRITE_ONLY = GL_WRITE_ONLY, 
    GBA_READ_WRITE = GL_READ_WRITE
  } EGLBufferAccess;

  typedef enum
  {
    AlignLeft    = 1,
    AlignHCenter = 2,
    AlignRight   = 4,
    AlignTop     = 8,
    AlignVCenter = 16,
    AlignBottom  = 32
  } EAlign;

  typedef enum
  {
    TextAlignLeft,
    TextAlignRight,
    TextAlignCenter,
    TextAlignJustify,
  } ETextAlign;

  typedef enum
  {
    Text2D = 1,
    Text3D = 2
  } ETextMode;

  typedef enum
  {
    LeftToRightText,
    RightToLeftText,
    TopToBottomText
  } ETextLayout;

  typedef enum
  {
    NeverDepthSort,
    AlwaysDepthSort,
    AlphaDepthSort
  } EDepthSortMode;

  typedef enum
  {
    ID_Error = 0,
    ID_1D = 1,
    ID_2D = 2,
    ID_3D = 3,
    ID_Cubemap = 4
  } EImageDimension;

  typedef enum
  {
    ST_RenderStates = 1,
    ST_Enables      = 2,
    ST_TextureUnits = 4,
    ST_Lights       = 8,
    ST_ClipPlanes   = 16
  } EStateType;

  typedef enum
  {
    RS_AlphaFunc,
    RS_BlendColor,
    RS_BlendEquation,
    RS_BlendFunc,
    RS_ColorMask,
    RS_CullFace,
    RS_DepthFunc,
    RS_DepthMask,
    RS_DepthRange,
    RS_Fog,
    RS_FrontFace,
    RS_PolygonMode,
    RS_Hint,
    RS_LightModel,
    RS_LineStipple,
    RS_LineWidth,
    RS_LogicOp,
    RS_Material,
    RS_PixelTransfer,
    RS_PointParameter,
    RS_PointSize,
    RS_PolygonOffset,
    RS_PolygonStipple,
    RS_SampleCoverage,
    RS_ShadeModel,
    RS_StencilFunc,
    RS_StencilMask,
    RS_StencilOp,
    RS_GLSLProgram,

    RS_Light0,
    RS_Light1,
    RS_Light2,
    RS_Light3,
    RS_Light4,
    RS_Light5,
    RS_Light6,
    RS_Light7,

    RS_ClipPlane0,
    RS_ClipPlane1,
    RS_ClipPlane2,
    RS_ClipPlane3,
    RS_ClipPlane4,
    RS_ClipPlane5,

    RS_TextureUnit0,
    /* ... */

    RS_TexGen0 = RS_TextureUnit0 + VL_MAX_TEXTURE_UNITS*1,
    /* ... */

    RS_TexEnv0 = RS_TextureUnit0 + VL_MAX_TEXTURE_UNITS*2,
    /* ... */

    RS_TextureMatrix0 = RS_TextureUnit0 + VL_MAX_TEXTURE_UNITS*3,
    /* ... */

    RS_COUNT = RS_TextureUnit0 + VL_MAX_TEXTURE_UNITS*4,
    RS_NONE,

  } ERenderState;

  typedef enum
  {
    RC_PreRendering, //!< The RenderingCallback is being called because a new rendering is going to start.
    RC_PostRendering  //!< The RenderingCallback is being called because a rendering has just been completed.
  } ERenderingCallback;

  typedef enum
  {
    GIT_POINTS              = GL_POINTS, 
    GIT_LINES               = GL_LINES,
    GIT_LINES_ADJACENCY     = GL_LINES_ADJACENCY_EXT, 
    GIT_TRIANGLES           = GL_TRIANGLES,
    GIT_TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY_EXT
  } EGeometryInputType;

  typedef enum
  {
    GOT_POINTS = GL_POINTS, 
    GOT_LINE_STRIP = GL_LINE_STRIP,
    GOT_TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
  } EGeometryOutputType;

  typedef enum
  {
    BB_COLOR_BUFFER_BIT   = GL_COLOR_BUFFER_BIT,
    BB_DEPTH_BUFFER_BIT   = GL_DEPTH_BUFFER_BIT, 
    BB_STENCIL_BUFFER_BIT = GL_STENCIL_BUFFER_BIT
  } EBufferBits;

  typedef enum
  {
    BT_AxisAlignedBillboard = 1,
    BT_SphericalBillboard = 2
  } EBillboardType;

  typedef enum
  {
    SM_SortBackToFront,
    SM_SortFrontToBack
  } ESortMode;

  typedef enum
  {
    GM_GetOrCreate,
    GM_DontCreate
  } EGetMode;

  typedef enum
  {
    SE_Unknown,
    SE_ASCII,
    SE_UTF8,
    SE_UTF16_BE,
    SE_UTF16_LE,
    SE_UTF32_BE,
    SE_UTF32_LE,
    SE_LATIN1,
  } EStringEncoding;

  typedef enum
  {
    OM_ReadOnly,
    OM_WriteOnly,
  } EOpenMode;

  typedef enum
  {
    Key_None = 0,

    // unicode keys

    Key_0,
    Key_1,
    Key_2,
    Key_3,
    Key_4,
    Key_5,
    Key_6,
    Key_7,
    Key_8,
    Key_9,

    Key_A,
    Key_B,
    Key_C,
    Key_D,
    Key_E,
    Key_F,
    Key_G,
    Key_H,
    Key_I,
    Key_J,
    Key_K,
    Key_L,
    Key_M,
    Key_N,
    Key_O,
    Key_P,
    Key_Q,
    Key_R,
    Key_S,
    Key_T,
    Key_U,
    Key_V,
    Key_W,
    Key_X,
    Key_Y,
    Key_Z,

    Key_Return,
    Key_BackSpace,
    Key_Tab,
    Key_Space,

    Key_Clear,
    Key_Escape,
    Key_Exclam,
    Key_QuoteDbl,
    Key_Hash,
    Key_Dollar,
    Key_Ampersand,
    Key_Quote,
    Key_LeftParen,
    Key_RightParen,
    Key_Asterisk,
    Key_Plus,
    Key_Comma,
    Key_Minus,
    Key_Period,
    Key_Slash,
    Key_Colon,
    Key_Semicolon,
    Key_Less,
    Key_Equal,
    Key_Greater,
    Key_Question,
    Key_At,
    Key_LeftBracket,
    Key_BackSlash,
    Key_RightBracket,
    Key_Caret,
    Key_Underscore,
    Key_QuoteLeft,

    // non unicode keys

    Key_Ctrl,
    Key_LeftCtrl,
    Key_RightCtrl,
    Key_Alt,
    Key_LeftAlt,
    Key_RightAlt,
    Key_Shift,
    Key_LeftShift,
    Key_RightShift,
    Key_Insert,
    Key_Delete,
    Key_Home,
    Key_End,
    Key_Print,
    Key_Pause,
    Key_PageUp,
    Key_PageDown,
    Key_Left,
    Key_Right,
    Key_Up,
    Key_Down,
    Key_F1,
    Key_F2,
    Key_F3,
    Key_F4,
    Key_F5,
    Key_F6,
    Key_F7,
    Key_F8,
    Key_F9,
    Key_F10,
    Key_F11,
    Key_F12,

    Key_Unknown,

    Key_NumberOfKeys

  } EKey;

  typedef enum
  {
    NoButton = 0,

    LeftButton   = 1,
    RightButton  = 2,
    MiddleButton = 4,

    UnknownButton,
  } EMouseButton;

  typedef enum
  {
    IN_Local                      = 0x00, //!< Does not propagates to children (thus cannot override children settings); can be overridden.
    IN_Propagate                  = 0x01, //!< Propagates to children; does not override children settings; can be overridden.
    IN_Sticky                     = 0x04, //!< Does not propagates to children (thus cannot override children settings); cannot be overridden.
    IN_Propagate_Overrides_Sticky = 0x01 | 0x02 | 0x04, //!< Propagates to children; overrides children settings; cannot be overridden.
    IN_Propagate_Overrides        = 0x01 | 0x02, //!< Propagates to children; overrides children settings; can be overridden.
    IN_Propagate_Sticky           = 0x01 | 0x04, //!< Propagates to children; does not override children settings; cannot be overridden.
  } EInheritance;

  typedef enum
  {
    ST_VERTEX_SHADER   = GL_VERTEX_SHADER,
    ST_FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    ST_GEOMETRY_SHADER = GL_GEOMETRY_SHADER_EXT
  } EShaderType;

  typedef enum
  {
    TW_TESS_WINDING_ODD         = GLU_TESS_WINDING_ODD,
    TW_TESS_WINDING_NONZERO     = GLU_TESS_WINDING_NONZERO,
    TW_TESS_WINDING_POSITIVE    = GLU_TESS_WINDING_POSITIVE,
    TW_TESS_WINDING_NEGATIVE    = GLU_TESS_WINDING_NEGATIVE,
    TW_TESS_WINDING_ABS_GEQ_TWO = GLU_TESS_WINDING_ABS_GEQ_TWO
  } ETessellationWinding;

  typedef enum { 
    VEL_VERBOSITY_SILENT, //!<< No log information is generated.
    VEL_VERBOSITY_ERROR,  //!<< Outputs critical and runtime error messages.
    VEL_VERBOSITY_NORMAL, //!<< Outputs normal information messages, plus all error messages.
    VEL_VERBOSITY_DEBUG   //!<< Outputs extra information messages useful for debugging, plus all normal and error messages.
  } EVerbosityLevel;

}


#endif
