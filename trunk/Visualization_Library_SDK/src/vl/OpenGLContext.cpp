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

#include <vl/OpenGLContext.hpp>
#include <vl/OpenGL.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>
#include <algorithm>

// include GLEW for the OpenGL extensions and to implement getProcAddress

extern "C"
{
  #include "../3rdparty/glew/src/glew.c"
}

using namespace vl;

//-----------------------------------------------------------------------------
// UIEventListener
//-----------------------------------------------------------------------------
void UIEventListener::setRank(int rank)
{
  mRank = rank;
  if ( openglContext() )
    openglContext()->sortEventListeners();
}
//-----------------------------------------------------------------------------
int UIEventListener::rank() const
{
  return mRank;
}
//-----------------------------------------------------------------------------
OpenGLContext* UIEventListener::openglContext() { return mOpenGLContext; }
//-----------------------------------------------------------------------------
// OpenGLContext
//-----------------------------------------------------------------------------
void OpenGLContext::sortEventListeners()
{
  std::sort(mEventListeners.begin(), mEventListeners.end());
}
//-----------------------------------------------------------------------------
//! \note An \p UIEventListener can be associated only to one OpenGLContext at a time.
void OpenGLContext::addEventListener(UIEventListener* el)
{
  VL_CHECK( el->mOpenGLContext == NULL );
  mEventListeners.push_back(el);
  el->mOpenGLContext = this;
  el->openglContextBoundEvent(this);
  sortEventListeners();
}
//-----------------------------------------------------------------------------
void OpenGLContext::removeEventListener(UIEventListener* el)
{
  VL_CHECK( el->mOpenGLContext != NULL );
  std::vector< ref<UIEventListener> >::iterator pos = std::find(mEventListeners.begin(), mEventListeners.end(), el);
  if( pos != mEventListeners.end() )
  {
    mEventListeners.erase( pos );
    el->mOpenGLContext = NULL;
  }
}
//-----------------------------------------------------------------------------
void OpenGLContext::eraseAllEventListeners()
{
  std::vector< ref<UIEventListener> >::iterator it = mEventListeners.begin();
  for(; it != mEventListeners.end(); it++)
    it->get()->mOpenGLContext = NULL;
  mEventListeners.clear();
}
//-----------------------------------------------------------------------------
void OpenGLContext::setVSyncEnabled(bool enable)
{
#ifdef _WIN32
  makeCurrent();
  if (WGLEW_EXT_swap_control)
    wglSwapIntervalEXT(enable?1:0);
#else
  // Mac and Linux?
#endif
}
//-----------------------------------------------------------------------------
bool OpenGLContext::vsyncEnabled() const
{
#ifdef _WIN32
  if (WGLEW_EXT_swap_control)
    return wglGetSwapIntervalEXT() != 0;
  else
    return false;
#else
  return false;
#endif
}
//-----------------------------------------------------------------------------
void OpenGLContext::initGLContext(bool log)
{
  makeCurrent();

  // init glew for each rendering context
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    fprintf(stderr, "Error calling glewInit(): %s\n", glewGetErrorString(err));
    VL_TRAP()
  }

  if (log)
    logOpenGLInfo();

  // clears eventual OpenGL errors generated by GLEW
  glGetError();

  mTextureUnitCount = 1;
  if (GLEW_ARB_multitexture||GLEW_VERSION_1_3)
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &mTextureUnitCount);
  mTextureUnitCount = mTextureUnitCount < VL_MAX_TEXTURE_UNIT_COUNT ? mTextureUnitCount : VL_MAX_TEXTURE_UNIT_COUNT;

  VL_CHECK_OGL();

  // test for double buffer availability
  glDrawBuffer(GL_BACK); 
  if ( glGetError() )
    mHasDoubleBuffer = false;
  else
    mHasDoubleBuffer = true;

  mIsInitialized = true;
}
//-----------------------------------------------------------------------------
bool OpenGLContext::isExtensionSupported(const char* ext_name)
{
  makeCurrent();
  size_t len = strlen(ext_name);
  const char* ext = (const char*)glGetString(GL_EXTENSIONS);
  const char* ext_end = ext + strlen(ext);

  for( const char* pos = strstr(ext,ext_name); pos && pos < ext_end; pos = strstr(pos,ext_name) )
  {
    if (pos[len] == ' ' || pos[len] == 0)
      return true;
    else
      pos += len;
  }

  return false;
}
//-----------------------------------------------------------------------------
void* OpenGLContext::getProcAddress(const char* function_name)
{
  makeCurrent();
  return (void*)glewGetProcAddress((const unsigned char*)function_name);
}
//-----------------------------------------------------------------------------
void OpenGLContext::logOpenGLInfo()
{
  makeCurrent();

  if (VL_VERBOSITY_LEVEL >= 1)
  {
    if (GLEW_VERSION_3_0)
      Log::print("OpenGL 3.0 detected\n");
    else
    if (GLEW_VERSION_2_1)
      Log::print("OpenGL 2.1 detected\n");
    else
    if (GLEW_VERSION_2_0)
      Log::print("OpenGL 2.0 detected\n");
    else
    if (GLEW_VERSION_1_5)
      Log::print("OpenGL 1.5 detected\n");
    else
    if (GLEW_VERSION_1_4)
      Log::print("OpenGL 1.4 detected\n");
    else
    if (GLEW_VERSION_1_3)
      Log::print("OpenGL 1.3 detected\n");
    else
    if (GLEW_VERSION_1_2)
      Log::print("OpenGL 1.2 detected\n");
    else
    if (GLEW_VERSION_1_1)
      Log::print("OpenGL 1.1 detected\n");
    else
      Log::print("Unknown OpenGL version supported\n");
    Log::print( Say("GL_VERSION = %s\n") << glGetString(GL_VERSION) );
    Log::print( Say("GL_VENDOR = %s\n") << glGetString(GL_VENDOR) );
    Log::print( Say("GL_RENDERER = %s\n") << glGetString(GL_RENDERER) );
    if (GLEW_VERSION_2_0)
      Log::print( Say("GL_SHADING_LANGUAGE_VERSION = %s\n")<<glGetString(GL_SHADING_LANGUAGE_VERSION) );
    Log::print( Say("GLEW_VERSION = %s\n")<<glewGetString(GLEW_VERSION) );
    int tex_max = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tex_max);
    Log::print( Say("Max texture size: %n\n")<<tex_max);
    tex_max = 1;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &tex_max);
    Log::print( Say("%s: Multitexturing, %n unit%s\n") << (GLEW_ARB_multitexture ? "OK" : "NO") << tex_max << (tex_max>1?"s":""));
    tex_max = 0;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3)
      glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &tex_max);
    Log::print( Say("%s: Anisotropic Texture Filter ") << (GLEW_EXT_texture_filter_anisotropic? "OK" : "NO") );
    GLEW_EXT_texture_filter_anisotropic ? Log::print( Say("%nX\n") << tex_max) : Log::print("\n");
    Log::print( Say("%s: S3 Texture Compression\n") << (GLEW_EXT_texture_compression_s3tc? "OK" : "NO") );
    Log::print( Say("%s: Vertex Buffer Object\n") << (GLEW_ARB_vertex_buffer_object ? "OK" : "NO"));
    Log::print( Say("%s: Pixel Buffer Object\n") << (GLEW_ARB_pixel_buffer_object ? "OK" : "NO"));
    Log::print( Say("%s: Framebuffer Object\n") << (GLEW_EXT_framebuffer_object ? "OK" : "NO"));
    Log::print( Say("%s: GLSL 1.0\n") << (GLEW_ARB_shading_language_100 ? "OK" : "NO"));
    int max_vertex_attribs = 0;
    if (GLEW_ARB_shading_language_100)
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS , &max_vertex_attribs);    
    Log::print( Say("GL_MAX_VERTEX_ATTRIBS: %n\n")<<max_vertex_attribs);

    if (VL_VERBOSITY_LEVEL >= 2)
    {
      Log::print("\nExtensions:\n");
      const char* ext_str = (const char*)glGetString(GL_EXTENSIONS);
      if (ext_str)
      {
        size_t max_offset = strlen( ext_str );
        for(size_t offset=0; offset<max_offset; )
        {
          char buffer[128];
          memset(buffer,0,128);
          sscanf(ext_str+offset,"%s",buffer);
          Log::print( Say("%s\n") << buffer );
          offset += strlen(buffer)+1;
        }
      }
    }
    Log::print("\n");
  }
}
//-----------------------------------------------------------------------------
