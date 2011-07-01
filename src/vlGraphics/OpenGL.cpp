/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi                                             */
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

#include <vlGraphics/OpenGL.hpp>
#include <vlCore/String.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <algorithm>

#if defined(VL_OPENGL_ES1) || defined(VL_OPENGL_ES2)
  #include <EGL/egl.h> // for eglGetProcAddress()
#endif

// OpenGL implmentation
namespace vl
{
  bool Has_GL_Version_1_1 = false;
  bool Has_GL_Version_1_2 = false;
  bool Has_GL_Version_1_3 = false;
  bool Has_GL_Version_1_4 = false;
  bool Has_GL_Version_1_5 = false;
  bool Has_GL_Version_2_0 = false;
  bool Has_GL_Version_2_1 = false;
  bool Has_GL_Version_3_0 = false;
  bool Has_GL_Version_3_1 = false;
  bool Has_GL_Version_3_2 = false;
  bool Has_GL_Version_3_3 = false;
  bool Has_GL_Version_4_0 = false;
  bool Has_GL_Version_4_1 = false;

  bool Has_Fixed_Function_Pipeline = false;

  // GLES defines

  bool Has_GLES_Version_1 = false;
  bool Has_GLES_Version_2 = false;
  bool Has_GLES = false;

  // Helper defines

  bool Has_GLSL = false;
  bool Has_GLSL_120_Or_More = false;
  bool Has_GLSL_130_Or_More = false;
  bool Has_GLSL_140_Or_More = false;
  bool Has_GLSL_150_Or_More = false;
  bool Has_GLSL_330_Or_More = false;
  bool Has_GLSL_400_Or_More = false;
  bool Has_GLSL_410_Or_More = false;
  bool Has_Geometry_Shader = false;
  bool Has_VBO = false;
  bool Has_FBO = false;
  bool Has_PBO = false;
  bool Has_FBO_Multisample = false;
  bool Has_Cubemap_Textures = false;
  bool Has_Texture_Rectangle = false;
  bool Has_Texture_Array = false;
  bool Has_Texture_Buffer = false;
  bool Has_Texture_Multisample = false;
  bool Has_Texture_3D = false;
  bool Has_Multitexture = false;
  bool Has_Primitive_Restart = false;
  bool Has_Occlusion_Query = false;
  bool Has_Transform_Feedback = false;
  bool Has_glGenerateMipmaps = false;
  bool Has_GL_GENERATE_MIPMAP = false;
  bool Has_Point_Sprite = false;

  #define VL_EXTENSION(extension) bool Has_##extension = false;
  #include "GLExtensionList.hpp"
  #undef VL_EXTENSION

  #define VL_GLES_EXTENSION(extension) bool Has_##extension = false;
  #include "GLESExtensionList.hpp"
  #undef VL_GLES_EXTENSION

  #if defined(VL_OPENGL)
    #define VL_GL_FUNCTION(TYPE, NAME) TYPE NAME = NULL;
    #include "GLFunctionList.hpp"
    #undef VL_GL_FUNCTION
  #endif

  #if defined(VL_OPENGL_ES1)
    #define VL_GL_FUNCTION(TYPE, NAME) TYPE NAME = NULL;
    #include "GLES1FunctionList.hpp"
    #undef VL_GL_FUNCTION
  #endif

  #if defined(VL_OPENGL_ES2)
    #define VL_GL_FUNCTION(TYPE, NAME) TYPE NAME = NULL;
    #include "GLES2FunctionList.hpp"
    #undef VL_GL_FUNCTION
  #endif

}

bool vl::initializeOpenGL()
{
    // clear errors and check OpenGL context is present
    if (glGetError() != GL_NO_ERROR)
      return false;

    // opengl function pointer initialization
    #if defined(VL_OPENGL)
      #define VL_GL_FUNCTION(TYPE, NAME) NAME = (TYPE)getGLProcAddress((const GLubyte*)#NAME);
      #include "GLFunctionList.hpp"
    #endif

    // opengl function pointer initialization
    #if defined(VL_OPENGL_ES1)
      #define VL_GL_FUNCTION(TYPE, NAME) NAME = (TYPE)getGLProcAddress((const GLubyte*)#NAME);
      #include "GLES1FunctionList.hpp"
    #endif

    // opengl function pointer initialization
    #if defined(VL_OPENGL_ES2)
      #define VL_GL_FUNCTION(TYPE, NAME) NAME = (TYPE)getGLProcAddress((const GLubyte*)#NAME);
      #include "GLES2FunctionList.hpp"
    #endif

    // Check fixed function pipeline
    glDisable(GL_LIGHTING);
    Has_Fixed_Function_Pipeline = glGetError() == GL_NO_ERROR;

    // GLES detect
    #if defined(VL_OPENGL_ES1)
      Has_GLES = Has_GLES_Version_1 = true;
    #endif

    #if defined(VL_OPENGL_ES2)
      Has_GLES = Has_GLES_Version_2 = true;
    #endif

    // GL versions
    const char* version_string = (const char*)glGetString(GL_VERSION);
    // These stay zero for GLES
    int vmaj = 0;
    int vmin = 0;
    // Format returned by GLES is "OpenGL ES-XX N.M"
    if (!Has_GLES)
    {
      vmaj = version_string[0] - '0';
      vmin = version_string[2] - '0';
    }

    Has_GL_Version_1_1 = (vmaj == 1 && vmin >= 1) || (vmaj > 1 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_1_2 = (vmaj == 1 && vmin >= 2) || (vmaj > 1 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_1_3 = (vmaj == 1 && vmin >= 3) || (vmaj > 1 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_1_4 = (vmaj == 1 && vmin >= 4) || (vmaj > 1 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_1_5 = (vmaj == 1 && vmin >= 5) || (vmaj > 1 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_2_0 = (vmaj == 2 && vmin >= 0) || (vmaj > 2 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_2_1 = (vmaj == 2 && vmin >= 1) || (vmaj > 2 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_3_0 = (vmaj == 3 && vmin >= 0) || (vmaj > 3 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_3_1 = (vmaj == 3 && vmin >= 1) || (vmaj > 3 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_3_2 = (vmaj == 3 && vmin >= 2) || (vmaj > 3 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_3_3 = (vmaj == 3 && vmin >= 3) || (vmaj > 3 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_4_0 = (vmaj == 4 && vmin >= 0) || (vmaj > 4 && Has_Fixed_Function_Pipeline);
    Has_GL_Version_4_1 = (vmaj == 4 && vmin >= 1) || (vmaj > 4 && Has_Fixed_Function_Pipeline);

    // opengl extension strings init
    std::string extensions = getOpenGLExtensions();

    #define VL_EXTENSION(extension) Has_##extension = strstr(extensions.c_str(), #extension" ") != NULL;
    #include "GLExtensionList.hpp"
    #undef VL_EXTENSION

    #define VL_GLES_EXTENSION(extension) Has_##extension = strstr(extensions.c_str(), #extension" ") != NULL;
    #include "GLESExtensionList.hpp"
    #undef VL_GLES_EXTENSION

#if defined(VL_OPENGL_ES1)
    // mic fixme: http://www.imgtec.com/forum/forum_posts.asp?TID=1379
    // POWERVR emulator bugs:
    if (Has_GL_OES_texture_cube_map && glTexGenfOES == 0)
    {
      Has_GL_OES_texture_cube_map = false;
      Has_Cubemap_Textures = false;
      Log::error("GL_OES_texture_cube_map exposed but glTexGenfOES == NULL!\n"); /*VL_TRAP();*/
    }
    if(Has_GL_OES_blend_func_separate && glBlendFuncSeparateOES == 0)
    {
      Has_GL_OES_blend_func_separate = false;
      Log::error("GL_OES_blend_func_separate exposed but glBlendFuncSeparateOES == NULL!\n"); /*VL_TRAP();*/
    }
    if (Has_GL_OES_fixed_point && glAlphaFuncxOES == NULL)
    {
      Log::warning("GL_OES_fixed_point functions are not exposed with their OES suffix!\n");
    }
    if (Has_GL_OES_single_precision && glDepthRangefOES == NULL)
    {
      Log::warning("GL_OES_single_precision functions are not exposed with their OES suffix!\n");
    }
#endif

    // Helper defines

    // Note that GL extensions pertaining to deprecated features seem to be exposed under Core profiles even if they are not supported (like Has_GL_SGIS_generate_mipmap)

    Has_GLSL = Has_GL_ARB_shading_language_100||Has_GL_Version_2_0||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GLES_Version_2;
    Has_GLSL_120_Or_More = Has_GL_Version_2_1||Has_GL_Version_3_0||Has_GL_Version_4_0;
    Has_GLSL_130_Or_More = Has_GL_Version_3_0||Has_GL_Version_4_0;
    Has_GLSL_140_Or_More = Has_GL_Version_3_1||Has_GL_Version_4_0;
    Has_GLSL_150_Or_More = Has_GL_Version_3_2||Has_GL_Version_4_0;
    Has_GLSL_330_Or_More = Has_GL_Version_3_3||Has_GL_Version_4_0;
    Has_GLSL_400_Or_More = Has_GL_Version_4_0;
    Has_GLSL_410_Or_More = Has_GL_Version_4_1;
    Has_Geometry_Shader  = Has_GL_NV_geometry_shader4||Has_GL_EXT_geometry_shader4||Has_GL_ARB_geometry_shader4||Has_GL_Version_3_2||Has_GL_Version_4_0;
    Has_VBO = Has_GL_ARB_vertex_buffer_object||Has_GL_Version_1_5||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GLES;
    Has_FBO = Has_GL_EXT_framebuffer_object||Has_GL_ARB_framebuffer_object||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GL_OES_framebuffer_object||Has_GLES_Version_2;
    Has_PBO = Has_GL_ARB_pixel_buffer_object||Has_GL_EXT_pixel_buffer_object||Has_GL_Version_2_1||Has_GL_Version_3_0||Has_GL_Version_4_0;
    // We only support GL_ANGLE_framebuffer_blit for GLES, see also:
    // http://www.khronos.org/registry/gles/extensions/IMG/IMG_multisampled_render_to_texture.txt
    // http://www.khronos.org/registry/gles/extensions/APPLE/APPLE_framebuffer_multisample.txt
    Has_FBO_Multisample = Has_GL_Version_4_0||Has_GL_Version_3_0||Has_GL_ARB_framebuffer_object||Has_GL_EXT_framebuffer_multisample||Has_GL_ANGLE_framebuffer_multisample;
    Has_Cubemap_Textures = Has_GL_ARB_texture_cube_map||Has_GL_Version_1_3||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GL_OES_texture_cube_map||Has_GLES_Version_2;
    Has_Texture_Rectangle = Has_GL_ARB_texture_rectangle||Has_GL_NV_texture_rectangle||Has_GL_Version_3_1||Has_GL_Version_4_0;
    Has_Texture_Array = Has_GL_EXT_texture_array||Has_GL_Version_3_0||Has_GL_Version_4_0;
    Has_Texture_Buffer = Has_GL_ARB_texture_buffer_object||Has_GL_EXT_texture_buffer_object||Has_GL_Version_3_1||Has_GL_Version_4_0;
    Has_Texture_Multisample = Has_GL_ARB_texture_multisample||Has_GL_Version_3_2||Has_GL_Version_4_0;
    Has_Texture_3D = Has_GL_EXT_texture3D||Has_GL_Version_1_2||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GL_OES_texture_3D;
    Has_Multitexture = Has_GL_ARB_multitexture||Has_GL_Version_1_3||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GLES;
    Has_Primitive_Restart = Has_GL_Version_3_1||Has_GL_Version_4_0;
    Has_Occlusion_Query = Has_GL_ARB_occlusion_query||Has_GL_Version_1_5||Has_GL_Version_3_0||Has_GL_Version_4_0;
    Has_Transform_Feedback = Has_GL_NV_transform_feedback||Has_GL_EXT_transform_feedback||Has_GL_Version_3_0||Has_GL_Version_4_0;
    Has_glGenerateMipmaps = Has_GL_ARB_framebuffer_object||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GLES_Version_2;
    Has_GL_GENERATE_MIPMAP = (Has_GL_SGIS_generate_mipmap && Has_Fixed_Function_Pipeline)||Has_GL_Version_1_4||Has_GLES_Version_1;
    Has_Point_Sprite = Has_GL_NV_point_sprite || Has_GL_ARB_point_sprite || Has_GLSL || Has_GLES_Version_1;

    return glGetError() == GL_NO_ERROR;
}

const char* vl::getGLErrorString(int err)
{
  switch(err)
  {
  case GL_INVALID_ENUM:      return "Invalid enum";
  case GL_INVALID_VALUE:     return "Invalid value";
  case GL_INVALID_OPERATION: return "Invalid operation";
  case GL_STACK_OVERFLOW:    return "Stack overflow";
  case GL_STACK_UNDERFLOW:   return "Stack underflow";
  case GL_OUT_OF_MEMORY:     return "Out of memory";
  default:
    return "";
  }
}

//------------------------------------------------------------------------------
int vl::glcheck(const char* file, int line)
{
  unsigned int glerr = glGetError();
  // if an OpenGL context is available this must be clear!
  if ( glGetError() )
  {
    Log::bug( Say("%s:%n: NO OPENGL CONTEXT ACTIVE!\n") << file << line );
  }
  else
  if (glerr != GL_NO_ERROR)
  {
    String msg( (char*)getGLErrorString(glerr) );
    Log::bug( Say("glGetError() [%s:%n]: %s\n") << file << line << msg );
  }
  return glerr;
}
//------------------------------------------------------------------------------
// vl::getGLProcAddress() implementation based on GLEW's
//------------------------------------------------------------------------------
#if defined(VL_OPENGL_ES1) || defined(VL_OPENGL_ES2)
void* vl::getGLProcAddress(const GLubyte* name)
{
  void* func = (void*)eglGetProcAddress( (const char*)name );
  /*if (func)
    Log::warning( String().printf("+ %s\n", name) );
  else
    Log::error( String().printf("- %s\n", name) );*/
  return func;
}
#elif defined(VL_PLATFORM_WINDOWS)
void* vl::getGLProcAddress(const GLubyte* name)
{
  return (void*)wglGetProcAddress((LPCSTR)name);
}
#elif defined(VL_PLATFORM_LINUX)
void* vl::getGLProcAddress(const GLubyte* name)
{
  return (void*)(*glXGetProcAddress)(name);
}
#elif defined(__APPLE__)
#include <stdlib.h>
#include <string.h>
#include <AvailabilityMacros.h>

#ifdef MAC_OS_X_VERSION_10_3

#include <dlfcn.h>

void* vl::getGLProcAddress(const GLubyte *name)
{
  static void* image = NULL;
  if (NULL == image)
  {
    image = dlopen("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", RTLD_LAZY);
  }
  return image ? dlsym(image, (const char*)name) : NULL;
}

#else

#include <mach-o/dyld.h>

void* vl::getGLProcAddress(const GLubyte *name)
{
  static const struct mach_header* image = NULL;
  NSSymbol symbol;
  char* symbolName;
  if (NULL == image)
  {
    image = NSAddImage("/System/Library/Frameworks/OpenGL.framework/Versions/Current/OpenGL", NSADDIMAGE_OPTION_RETURN_ON_ERROR);
  }
  /* prepend a '_' for the Unix C symbol mangling convention */
  symbolName = malloc(strlen((const char*)name) + 2);
  strcpy(symbolName+1, (const char*)name);
  symbolName[0] = '_';
  symbol = NULL;
  /* if (NSIsSymbolNameDefined(symbolName))
	 symbol = NSLookupAndBindSymbol(symbolName); */
  symbol = image ? NSLookupSymbolInImage(image, symbolName, NSLOOKUPSYMBOLINIMAGE_OPTION_BIND | NSLOOKUPSYMBOLINIMAGE_OPTION_RETURN_ON_ERROR) : NULL;
  free(symbolName);
  return symbol ? NSAddressOfSymbol(symbol) : NULL;
}

#endif /* MAC_OS_X_VERSION_10_3 */

/* __APPLE__ end */

#elif defined(__sgi) || defined (__sun)

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

void* vl::getGLProcAddress(const GLubyte* name)
{
  static void* h = NULL;
  static void* gpa;

  if (h == NULL)
  {
    if ((h = dlopen(NULL, RTLD_LAZY | RTLD_LOCAL)) == NULL) return NULL;
    gpa = dlsym(h, "glXGetProcAddress");
  }

  if (gpa != NULL)
    return ((void*(*)(const GLubyte*))gpa)(name);
  else
    return dlsym(h, (const char*)name);
}

/* __sgi || __sun end */

#else
void* vl::getGLProcAddress(const GLubyte* name)
{
  return NULL;
}
#endif
//------------------------------------------------------------------------------
