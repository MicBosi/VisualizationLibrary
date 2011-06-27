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

#ifndef OpenGL_INCLUDE_ONCE
#define OpenGL_INCLUDE_ONCE

#include <vlCore/OpenGLDefs.hpp>
#include <vlGraphics/link_config.hpp>
#include <vlCore/Log.hpp>

// Helper macros

#define Has_GLSL (::vl::Has_GL_ARB_shading_language_100||::vl::Has_GL_Version_2_0||::vl::Has_GL_Version_3_0||::vl::Has_GL_Version_4_0)
#define Has_GLSL_120_Or_More (::vl::Has_GL_Version_2_1||::vl::Has_GL_Version_3_0||::vl::Has_GL_Version_4_0)
#define Has_GLSL_130_Or_More (::vl::Has_GL_Version_3_0||::vl::Has_GL_Version_4_0)
#define Has_GLSL_140_Or_More (::vl::Has_GL_Version_3_1||::vl::Has_GL_Version_4_0)
#define Has_GLSL_150_Or_More (::vl::Has_GL_Version_3_2||::vl::Has_GL_Version_4_0)
#define Has_GLSL_330_Or_More (::vl::Has_GL_Version_3_3||::vl::Has_GL_Version_4_0)
#define Has_GLSL_400_Or_More (::vl::Has_GL_Version_4_0)
#define Has_GLSL_410_Or_More (::vl::Has_GL_Version_4_1)
#define Has_Geometry_Shader (::vl::Has_GL_NV_geometry_shader4||::vl::Has_GL_EXT_geometry_shader4||::vl::Has_GL_ARB_geometry_shader4||::vl::Has_GL_Version_3_2||::vl::Has_GL_Version_4_0)
#define Has_Framebuffer_Object (::vl::Has_GL_EXT_framebuffer_object||::vl::Has_GL_ARB_framebuffer_object||::vl::Has_GL_Version_3_0||::vl::Has_GL_Version_4_0)
#define Has_Framebuffer_Object_Multisample (::vl::Has_GL_Version_4_0||::vl::Has_GL_Version_3_0||::vl::Has_GL_ARB_framebuffer_object||::vl::Has_GL_EXT_framebuffer_multisample)
#define Has_Texture_Rectangle (vl::Has_GL_ARB_texture_rectangle||vl::Has_GL_NV_texture_rectangle||vl::Has_GL_Version_3_1||vl::Has_GL_Version_4_0)

namespace vl
{
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_2;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_3;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_4;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_1_5;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_2_0;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_2_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_0;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_1;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_2;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_3_3;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_0;
  VLGRAPHICS_EXPORT extern bool Has_GL_Version_4_1;

  #define VL_EXTENSION(extension) VLGRAPHICS_EXPORT extern bool Has_##extension;
  #include "GLExtensionList.hpp"
  #undef VL_EXTENSION

  #define VL_GLES_EXTENSION(extension) VLGRAPHICS_EXPORT extern bool Has_##extension;
  #include "GLESExtensionList.hpp"
  #undef VL_GLES_EXTENSION

  #if defined(VL_OPENGL)
    #define VL_GL_FUNCTION(TYPE, NAME) VLGRAPHICS_EXPORT extern TYPE NAME;
    #include "GLFunctionList.hpp"
    #undef VL_GL_FUNCTION
  #endif

  #if defined(VL_OPENGL_ES1)
    #define VL_GL_FUNCTION(TYPE, NAME) VLGRAPHICS_EXPORT extern TYPE NAME;
    #include "GLES1FunctionList.hpp"
    #include "GLES1FunctionList_missing.hpp"
    #undef VL_GL_FUNCTION
  #endif

  #if defined(VL_OPENGL_ES2)
    #define VL_GL_FUNCTION(TYPE, NAME) VLGRAPHICS_EXPORT extern TYPE NAME;
    #include "GLES2FunctionList.hpp"
    #undef VL_GL_FUNCTION
  #endif

  VLGRAPHICS_EXPORT bool initializeOpenGL();

  VLGRAPHICS_EXPORT void* getGLProcAddress(const GLubyte* name);
  
  VLGRAPHICS_EXPORT const char* getGLErrorString(int err);

  //-----------------------------------------------------------------------------
  // VL_CHECK_OGL
  //-----------------------------------------------------------------------------

  VLGRAPHICS_EXPORT int glcheck( const char* file, int line );

  #if defined( _DEBUG ) || !defined( NDEBUG ) || VL_FORCE_CHECKS == 1
    #define VL_CHECK_OGL( ) { if ( ::vl::glcheck( __FILE__, __LINE__ ) ) { VL_TRAP( ) } }
  #else
    #define VL_CHECK_OGL( );
  #endif

  //------------------------------------------------------------------------------
  
}

//-----------------------------------------------------------------------------
// OpenGL functions wrappers
//-----------------------------------------------------------------------------
#if defined(VL_OPENGL)
  #include "VL_Functions_GL.hpp"
#endif
  
#if defined(VL_OPENGL_ES1)
  #include "VL_Functions_GLES1.hpp"
#endif

#if defined(VL_OPENGL_ES2)
  #include "VL_Functions_GLES2.hpp"
#endif
//-----------------------------------------------------------------------------

#endif
