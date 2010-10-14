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
#include <vl/Shader.hpp>
#include <vl/GLSL.hpp>
#include <vl/Light.hpp>
#include <vl/ClipPlane.hpp>
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
OpenGLContext::OpenGLContext(int w, int h): 
mMouseVisible(true), mContinuousUpdate(true), mIgnoreNextMouseMoveEvent(false), mFullscreen(false), mHasDoubleBuffer(false), mIsInitialized(false)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mRenderTarget = new RenderTarget(this, w, h);

  // just for debugging purposes
  memset( mCurrentRenderState, 0xFF, sizeof(mCurrentRenderState[0]) * RS_COUNT );
  memset( mRenderStateTable,   0xFF, sizeof(int)                    * RS_COUNT );
  memset( mCurrentEnable,      0xFF, sizeof(mCurrentEnable[0])      * EN_EnableCount );
  memset( mEnableTable,        0xFF, sizeof(int)                    * EN_EnableCount );

}
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
  mIsInitialized = false;

  makeCurrent();

  // init glew for each rendering context
  GLenum err = glewInit();
  if (GLEW_OK != err)
  {
    fprintf(stderr, "Error calling glewInit(): %s\n", glewGetErrorString(err));
    VL_TRAP()
  }
  else
  {
    mIsInitialized = true;
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

  setupDefaultRenderStates();
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
    GLint max_elements_vertices=0, max_elements_indices=0;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_elements_vertices);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES,  &max_elements_indices );
    Log::print( Say("GL_MAX_ELEMENTS_VERTICES = %n\n") << max_elements_vertices );
    Log::print( Say("GL_MAX_ELEMENTS_INDICES  = %n\n") << max_elements_indices  );
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
//------------------------------------------------------------------------------
namespace
{
  const GLenum TranslateEnable[] =
  {
    GL_ALPHA_TEST,
    GL_BLEND,
    GL_COLOR_LOGIC_OP,
    GL_LIGHTING,
    GL_COLOR_SUM,
    GL_CULL_FACE,
    GL_DEPTH_TEST,
    GL_FOG,
    GL_LINE_SMOOTH,
    GL_LINE_STIPPLE,
    GL_POLYGON_STIPPLE,
    GL_NORMALIZE,
    GL_POINT_SMOOTH,
    GL_POINT_SPRITE,
    GL_POLYGON_SMOOTH,
    GL_POLYGON_OFFSET_FILL,
    GL_POLYGON_OFFSET_LINE,
    GL_POLYGON_OFFSET_POINT,
    GL_RESCALE_NORMAL,
    GL_STENCIL_TEST,
    GL_VERTEX_PROGRAM_POINT_SIZE,
    GL_VERTEX_PROGRAM_TWO_SIDE,

    // multisampling
    GL_MULTISAMPLE,
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_ALPHA_TO_ONE,
    GL_SAMPLE_COVERAGE
  };

  #ifndef NDEBUG
  const char* TranslateEnableString[] =
  {
    "EN_ALPHA_TEST",
    "EN_BLEND",
    "EN_COLOR_LOGIC_OP",
    "EN_LIGHTING",
    "EN_COLOR_SUM",
    "EN_CULL_FACE",
    "EN_DEPTH_TEST",
    "EN_FOG ",
    "EN_LINE_SMOOTH",
    "EN_LINE_STIPPLE",
    "EN_POLYGON_STIPPLE",
    "EN_NORMALIZE",
    "EN_POINT_SMOOTH",
    "EN_POINT_SPRITE",
    "EN_POLYGON_SMOOTH",
    "EN_POLYGON_OFFSET_FILL",
    "EN_POLYGON_OFFSET_LINE",
    "EN_POLYGON_OFFSET_POINT",
    "EN_RESCALE_NORMAL",
    "EN_STENCIL_TEST",
    "EN_VERTEX_PROGRAM_POINT_SIZE",
    "EN_VERTEX_PROGRAM_TWO_SIDE",

    // multisampling
    "EN_MULTISAMPLE",
    "EN_SAMPLE_ALPHA_TO_COVERAGE",
    "EN_SAMPLE_ALPHA_TO_ONE",
    "EN_SAMPLE_COVERAGE"
  };
  #endif
}
//------------------------------------------------------------------------------
void OpenGLContext::applyEnables( const EnableSet* prev, const EnableSet* cur )
{
  VL_CHECK_OGL()
  VL_CHECK(cur)
  if (prev == NULL)
    memset( mEnableTable, 0, sizeof(int) * EN_EnableCount );

  // iterate current: increment

  for( unsigned i=0; i<cur->enables().size(); ++i )
    mEnableTable[cur->enables()[i]] |= 2;

  // iterate on prev: reset to default only if it won't be overwritten by cur

  if (prev)
  {
    for( unsigned i=0; i<prev->enables().size(); ++i )
    {
      if ( mEnableTable[prev->enables()[i]] == 1 )
      {
        mEnableTable[prev->enables()[i]] = 0;
        mCurrentEnable[prev->enables()[i]] = false;
        glDisable( TranslateEnable[prev->enables()[i]] );
        #ifndef NDEBUG
          if (glGetError() != GL_NO_ERROR)
          {
            Log::error( Say("An unsupported enum has been disabled: %s.\n") << TranslateEnableString[prev->enables()[i]]);
            VL_TRAP()
          }
        #endif
      }
    }
  }
  else
  {
    memset(mCurrentEnable, 0, sizeof(mCurrentEnable[0])*EN_EnableCount);
  }

  // iterate current: apply only if needed

  for( unsigned i=0; i<cur->enables().size(); ++i )
  {
    mEnableTable[cur->enables()[i]] = 1;
    if ( !mCurrentEnable[cur->enables()[i]] )
    {
      glEnable( TranslateEnable[cur->enables()[i]] );
      mCurrentEnable[ cur->enables()[i] ] = true;
      #ifndef NDEBUG
        if (glGetError() != GL_NO_ERROR)
        {
          Log::error( Say("An unsupported enum has been enabled: %s.\n") << TranslateEnableString[cur->enables()[i]]);
          VL_TRAP()
        }
      #endif
    }
  }
}
//------------------------------------------------------------------------------
void OpenGLContext::applyRenderStates( const RenderStateSet* prev, const RenderStateSet* cur, const Camera* camera )
{
  VL_CHECK(cur)
  if (prev == NULL)
    memset( mRenderStateTable, 0, sizeof(int) * RS_COUNT );

  // iterate current: increment

  for( unsigned i=0; i<cur->renderStates().size(); ++i )
    mRenderStateTable[cur->renderStates()[i]->type()] |= 2;

  // iterate on prev: reset to default only if it won't be overwritten by cur

  if (prev)
  {
    for( unsigned i=0; i<prev->renderStates().size(); ++i )
    {
      if ( mRenderStateTable[prev->renderStates()[i]->type()] == 1 )
      {
        mRenderStateTable[prev->renderStates()[i]->type()] = 0;
        mCurrentRenderState[prev->renderStates()[i]->type()] = mDefaultRenderStates[prev->renderStates()[i]->type()].get();
        // if this fails you are using a render state that is not supported by the current OpenGL implementation (too old or Core profile)
        VL_CHECK(mCurrentRenderState[prev->renderStates()[i]->type()]);
        mDefaultRenderStates[prev->renderStates()[i]->type()]->disable();
        mDefaultRenderStates[prev->renderStates()[i]->type()]->apply(NULL);
      }
    }
  }
  else
  {
    memset(mCurrentRenderState, 0, sizeof(mCurrentRenderState[0])*RS_COUNT);
  }

  // iterate current: apply only if needed

  for( unsigned i=0; i<cur->renderStates().size(); ++i )
  {
    mRenderStateTable[cur->renderStates()[i]->type()] = 1;
    if ( mCurrentRenderState[cur->renderStates()[i]->type()] != cur->renderStates()[i] )
    {
      mCurrentRenderState[cur->renderStates()[i]->type()] = cur->renderStates()[i].get();
      VL_CHECK(cur->renderStates()[i]);
      cur->renderStates()[i]->enable();
      cur->renderStates()[i]->apply(camera);
    }
  }
}
//------------------------------------------------------------------------------
void OpenGLContext::setupDefaultRenderStates()
{
  mDefaultRenderStates[RS_AlphaFunc] = new AlphaFunc;
  mDefaultRenderStates[RS_BlendColor] = new BlendColor;
  mDefaultRenderStates[RS_BlendEquation] = new BlendEquation;
  mDefaultRenderStates[RS_BlendFunc] = new BlendFunc;
  mDefaultRenderStates[RS_ColorMask] = new ColorMask;
  mDefaultRenderStates[RS_CullFace] = new CullFace;
  mDefaultRenderStates[RS_DepthFunc] = new DepthFunc;
  mDefaultRenderStates[RS_DepthMask] = new DepthMask;
  mDefaultRenderStates[RS_DepthRange] = new DepthRange;
  mDefaultRenderStates[RS_Fog] = new Fog;
  mDefaultRenderStates[RS_FrontFace] = new FrontFace;
  mDefaultRenderStates[RS_PolygonMode] = new PolygonMode;
  mDefaultRenderStates[RS_Hint] = new Hint;
  mDefaultRenderStates[RS_LightModel] = new LightModel;
  mDefaultRenderStates[RS_LineStipple] = new LineStipple;
  mDefaultRenderStates[RS_LineWidth] = new LineWidth;
  mDefaultRenderStates[RS_LogicOp] = new LogicOp;
  mDefaultRenderStates[RS_Material] = new Material;
  mDefaultRenderStates[RS_PixelTransfer] = new PixelTransfer;
  mDefaultRenderStates[RS_PointParameter] = new PointParameter;
  mDefaultRenderStates[RS_PointSize] = new PointSize;
  mDefaultRenderStates[RS_PolygonOffset] = new PolygonOffset;
  mDefaultRenderStates[RS_PolygonStipple] = new PolygonStipple;
  mDefaultRenderStates[RS_SampleCoverage] = new SampleCoverage;
  mDefaultRenderStates[RS_ShadeModel] = new ShadeModel;
  mDefaultRenderStates[RS_StencilFunc] = new StencilFunc;
  mDefaultRenderStates[RS_StencilMask] = new StencilMask;
  mDefaultRenderStates[RS_StencilOp] = new StencilOp;
  mDefaultRenderStates[RS_GLSLProgram] = new GLSLProgram;

  mDefaultRenderStates[RS_Light0] = new Light(0);
  mDefaultRenderStates[RS_Light1] = new Light(1);
  mDefaultRenderStates[RS_Light2] = new Light(2);
  mDefaultRenderStates[RS_Light3] = new Light(3);
  mDefaultRenderStates[RS_Light4] = new Light(4);
  mDefaultRenderStates[RS_Light5] = new Light(5);
  mDefaultRenderStates[RS_Light6] = new Light(6);
  mDefaultRenderStates[RS_Light7] = new Light(7);

  mDefaultRenderStates[RS_ClipPlane0] = new ClipPlane(0);
  mDefaultRenderStates[RS_ClipPlane1] = new ClipPlane(1);
  mDefaultRenderStates[RS_ClipPlane2] = new ClipPlane(2);
  mDefaultRenderStates[RS_ClipPlane3] = new ClipPlane(3);
  mDefaultRenderStates[RS_ClipPlane4] = new ClipPlane(4);
  mDefaultRenderStates[RS_ClipPlane5] = new ClipPlane(5);

  for(unsigned int i=0; i<VL_MAX_TEXTURE_UNIT_COUNT; ++i)
  {
    if (i < textureUnitCount())
    {
      mDefaultRenderStates[RS_TextureUnit0   + i] = new TextureUnit(i);
      mDefaultRenderStates[RS_TexGen0        + i] = new TexGen(i);
      mDefaultRenderStates[RS_TexEnv0        + i] = new TexEnv(i);
      mDefaultRenderStates[RS_TextureMatrix0 + i] = new TextureMatrix(i);
    }
    else
    {
      mDefaultRenderStates[RS_TextureUnit0   + i] = NULL;
      mDefaultRenderStates[RS_TexGen0        + i] = NULL;
      mDefaultRenderStates[RS_TexEnv0        + i] = NULL;
      mDefaultRenderStates[RS_TextureMatrix0 + i] = NULL;
    }
  }
}
//-----------------------------------------------------------------------------
void OpenGLContext::resetRenderStates()
{
  VL_CHECK_OGL();
  memset( mCurrentRenderState, 0, sizeof(mCurrentRenderState[0]) * RS_COUNT );
  memset( mRenderStateTable,   0, sizeof(int)                    * RS_COUNT );
  // render states
  for( unsigned i=0; i<RS_COUNT; ++i )
  {
    // the empty ones are the ones that are not supported by the current OpenGL implementation (too old or Core profile)
    if (mDefaultRenderStates[i])
    {
       mDefaultRenderStates[i]->disable();
       mDefaultRenderStates[i]->apply(NULL);
    }
  }
  VL_CHECK_OGL();
}
//-----------------------------------------------------------------------------
void OpenGLContext::resetEnables()
{
  memset( mCurrentEnable, 0, sizeof(mCurrentEnable[0]) * EN_EnableCount );
  memset( mEnableTable,   0, sizeof(int)               * EN_EnableCount );
  for( unsigned i=0; i<EN_EnableCount; ++i )
    glDisable( TranslateEnable[i] );
  // clears errors due to unsupported enable flags
  while( glGetError() ) {}
}
//-----------------------------------------------------------------------------
