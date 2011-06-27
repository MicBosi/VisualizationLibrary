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

#include <vlGraphics/OpenGL.hpp>
#include <vlCore/String.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <algorithm>

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
    #include "GLES1FunctionList_missing.hpp"
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
      #include "GLES1FunctionList_missing.hpp"
    #endif

    // opengl function pointer initialization
    #if defined(VL_OPENGL_ES2)
      #define VL_GLES_FUNCTION(TYPE, NAME) NAME = (TYPE)getGLProcAddress((const GLubyte*)#NAME);
      #include "GLES2FunctionList.hpp"
    #endif

    // check fixed function pipeline present
    glDisable(GL_LIGHTING);
    bool compatible = glGetError() == GL_NO_ERROR;

    // opengl version detect
    const char* version_str = (const char*)glGetString(GL_VERSION);
    int vmaj = version_str[0] - '0';
    int vmin = version_str[2] - '0';
    int version = vmaj*10 + vmin;

    Has_GL_Version_1_1 = (vmaj == 1 && vmin >= 1) || vmaj == 2 || version == 30 || (vmaj > 1 && compatible);
    Has_GL_Version_1_2 = (vmaj == 1 && vmin >= 2) || vmaj == 2 || version == 30 || (vmaj > 1 && compatible);
    Has_GL_Version_1_3 = (vmaj == 1 && vmin >= 3) || vmaj == 2 || version == 30 || (vmaj > 1 && compatible);
    Has_GL_Version_1_4 = (vmaj == 1 && vmin >= 4) || vmaj == 2 || version == 30 || (vmaj > 1 && compatible);
    Has_GL_Version_1_5 = (vmaj == 1 && vmin >= 5) || vmaj == 2 || version == 30 || (vmaj > 1 && compatible);
    Has_GL_Version_2_0 = (vmaj == 2 && vmin >= 0) || version == 30 || (vmaj > 2 && compatible);
    Has_GL_Version_2_1 = (vmaj == 2 && vmin >= 1) || version == 30 || (vmaj > 2 && compatible);
    Has_GL_Version_3_0 = (vmaj == 3 && vmin >= 0) || (vmaj > 3 && compatible);
    Has_GL_Version_3_1 = (vmaj == 3 && vmin >= 1) || (vmaj > 3 && compatible);
    Has_GL_Version_3_2 = (vmaj == 3 && vmin >= 2) || (vmaj > 3 && compatible);
    Has_GL_Version_3_3 = (vmaj == 3 && vmin >= 3) || (vmaj > 3 && compatible);
    Has_GL_Version_4_0 = (vmaj == 4 && vmin >= 0) || (vmaj > 4 && compatible);
    Has_GL_Version_4_1 = (vmaj == 4 && vmin >= 1) || (vmaj > 4 && compatible);

    // opengl extension strings init
    std::string extensions = getOpenGLExtensions();

    // mic fixme: check that it works!
    #define VL_EXTENSION(extension) Has_##extension = strstr(extensions.c_str(), #extension" ") != NULL;
    #include "GLExtensionList.hpp"
    #undef VL_EXTENSION

    #define VL_GLES_EXTENSION(extension) Has_##extension = strstr(extensions.c_str(), #extension" ") != NULL;
    #include "GLESExtensionList.hpp"
    #undef VL_GLES_EXTENSION

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
  // mic fixme: use EGL
  return NULL;
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
