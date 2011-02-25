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
#include <sstream>

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
mMouseVisible(true), mContinuousUpdate(true), mIgnoreNextMouseMoveEvent(false), mFullscreen(false), 
mHasDoubleBuffer(false), mIsInitialized(false), mIsCompatible(false), mMajorVersion(0), mMinorVersion(0)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mRenderTarget = new RenderTarget(this, w, h);

  // set to unknown texture target
  memset( mTexUnitBinding, 0, sizeof(mTexUnitBinding) );

  // just for debugging purposes
  memset( mCurrentRenderState, 0xFF, sizeof(mCurrentRenderState) );
  memset( mRenderStateTable,   0xFF, sizeof(mRenderStateTable) );
  memset( mCurrentEnable,      0xFF, sizeof(mCurrentEnable) );
  memset( mEnableTable,        0xFF, sizeof(mEnableTable) );
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
  mIsCompatible  = false;
  mMajorVersion = 0;
  mMinorVersion = 0;

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

    char punto;
    std::stringstream stream;
    stream << glGetString(GL_VERSION);
    stream >> mMajorVersion >> punto >> mMinorVersion;

    // clear errors
    glGetError();
    // test if fixed function pipeline is supported.
    glDisable(GL_TEXTURE_2D);
    // check error code
    mIsCompatible  = glGetError() == GL_NO_ERROR;
  }

  if (log)
  {
    logOpenGLInfo();
  }

  // clears eventual OpenGL errors generated by GLEW
  glGetError();

  mTextureUnitCount = 1;
  if (GLEW_ARB_multitexture||GLEW_VERSION_1_3)
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &mTextureUnitCount);
  mTextureUnitCount = mTextureUnitCount < VL_MAX_TEXTURE_UNITS ? mTextureUnitCount : VL_MAX_TEXTURE_UNITS;

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
    Log::print( Say("OpenGL profile: %s\n") << (isCompatible() ? "Compatible" : "Core") );
    if (GLEW_VERSION_2_0||GLEW_VERSION_3_0)
      Log::print( Say("GL_SHADING_LANGUAGE_VERSION = %s\n")<<glGetString(GL_SHADING_LANGUAGE_VERSION) );
    Log::print( Say("GLEW_VERSION = %s\n")<<glewGetString(GLEW_VERSION) );
    int tex_max = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tex_max);
    Log::print( Say("Max texture size: %n\n")<<tex_max);
    tex_max = 1;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3||GLEW_VERSION_3_0)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &tex_max);
    Log::print( Say("Texture units: %n\n") << tex_max);
    tex_max = 1;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3||GLEW_VERSION_3_0)
      glGetIntegerv(GL_MAX_TEXTURE_COORDS, &tex_max);
    Log::print( Say("Texture coords: %n\n") << tex_max);
    tex_max = 0;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3)
      glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &tex_max);
    Log::print( Say("%s: Anisotropic Texture Filter ") << (GLEW_EXT_texture_filter_anisotropic? "OK" : "NO") );
    GLEW_EXT_texture_filter_anisotropic ? Log::print( Say("%nX\n") << tex_max) : Log::print("\n");
    Log::print( Say("%s: S3 Texture Compression\n") << (GLEW_EXT_texture_compression_s3tc? "OK" : "NO") );
    Log::print( Say("%s: Vertex Buffer Object\n") << (GLEW_ARB_vertex_buffer_object ? "OK" : "NO"));
    Log::print( Say("%s: Pixel Buffer Object\n") << (GLEW_ARB_pixel_buffer_object ? "OK" : "NO"));
    Log::print( Say("%s: Framebuffer Object\n") << (GLEW_EXT_framebuffer_object ? "OK" : "NO"));
    GLint max_elements_vertices=0, max_elements_indices=0;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_elements_vertices);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES,  &max_elements_indices );
    Log::print( Say("GL_MAX_ELEMENTS_VERTICES = %n\n") << max_elements_vertices );
    Log::print( Say("GL_MAX_ELEMENTS_INDICES  = %n\n") << max_elements_indices  );
    int max_vertex_attribs = 0;
    if(GLEW_VERSION_2_0||GLEW_VERSION_3_0)
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS , &max_vertex_attribs);    
    Log::print( Say("GL_MAX_VERTEX_ATTRIBS: %n\n")<<max_vertex_attribs);

    if (VL_VERBOSITY_LEVEL >= 2)
    {
      Log::print("\nExtensions:\n");
      if (GLEW_VERSION_3_0)
      {
        for( int i=0; ; ++i )
        {
          const GLubyte* str = glGetStringi(GL_EXTENSIONS,i);
          if (!str)
            break;
          Log::print( Say("%s\n") << str );
        }
      } 
      else
      {
        std::stringstream sstream;
        const char* ext_str = (const char*)glGetString(GL_EXTENSIONS);
        VL_CHECK(ext_str);
        sstream << ext_str;
        for( std::string ext; true ; )
        {
          sstream >> ext;
          if (sstream.eof())
            break;
          Log::print( Say("%s\n") << ext );
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

    GL_TEXTURE_CUBE_MAP_SEAMLESS,
    GL_CLIP_DISTANCE0,
    GL_CLIP_DISTANCE1,
    GL_CLIP_DISTANCE2,
    GL_CLIP_DISTANCE3,
    GL_CLIP_DISTANCE4,
    GL_CLIP_DISTANCE5,

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

    "EN_TEXTURE_CUBE_MAP_SEAMLESS",
    "EN_GL_CLIP_DISTANCE0",
    "EN_GL_CLIP_DISTANCE1",
    "EN_GL_CLIP_DISTANCE2",
    "EN_GL_CLIP_DISTANCE3",
    "EN_GL_CLIP_DISTANCE4",
    "EN_GL_CLIP_DISTANCE5",

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
    memset( mEnableTable, 0, sizeof(mEnableTable) );

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
        glDisable( TranslateEnable[prev->enables()[i]] ); VL_CHECK_OGL()
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
    memset(mCurrentEnable, 0, sizeof(mCurrentEnable));
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
    memset( mRenderStateTable, 0, sizeof(mRenderStateTable) );

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
        mDefaultRenderStates[prev->renderStates()[i]->type()]->apply(NULL, this); VL_CHECK_OGL()
      }
    }
  }
  else
  {
    memset(mCurrentRenderState, 0, sizeof(mCurrentRenderState));
  }

  // iterate current: apply only if needed

  for( unsigned i=0; i<cur->renderStates().size(); ++i )
  {
    mRenderStateTable[cur->renderStates()[i]->type()] = 1;
    if ( mCurrentRenderState[cur->renderStates()[i]->type()] != cur->renderStates()[i] )
    {
      mCurrentRenderState[cur->renderStates()[i]->type()] = cur->renderStates()[i].get();
      VL_CHECK(cur->renderStates()[i]);      
      cur->renderStates()[i]->apply(camera, this); VL_CHECK_OGL()
    }
  }
}
//------------------------------------------------------------------------------
void OpenGLContext::setupDefaultRenderStates()
{
  // initialize to NULL
  memset(mDefaultRenderStates, 0, sizeof(mDefaultRenderStates));

  if ( isCompatible() )
  {
    mDefaultRenderStates[RS_AlphaFunc] = new AlphaFunc;
    if (GLEW_VERSION_1_4||GLEW_EXT_blend_color)
      mDefaultRenderStates[RS_BlendColor] = new BlendColor;
    if (GLEW_VERSION_1_4)
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
    if (GLEW_VERSION_1_4||GLEW_ARB_point_parameters)
      mDefaultRenderStates[RS_PointParameter] = new PointParameter;
    mDefaultRenderStates[RS_PointSize] = new PointSize;
    mDefaultRenderStates[RS_PolygonOffset] = new PolygonOffset;
    mDefaultRenderStates[RS_PolygonStipple] = new PolygonStipple;
    if (GLEW_VERSION_1_3||GLEW_ARB_multisample)
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

    for(int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
    {
      if (i < textureUnitCount())
      {
        mDefaultRenderStates[RS_TextureUnit0   + i] = new TextureUnit(i);
        mDefaultRenderStates[RS_TexGen0        + i] = new TexGen(i);
        mDefaultRenderStates[RS_TexEnv0        + i] = new TexEnv(i);
        mDefaultRenderStates[RS_TextureMatrix0 + i] = new TextureMatrix(i);
      }
    }
  }
  else
  {
    // mic fixme: fare ordine
    // mDefaultRenderStates[RS_AlphaFunc] = new AlphaFunc;
    mDefaultRenderStates[RS_BlendColor] = new BlendColor;
    mDefaultRenderStates[RS_BlendEquation] = new BlendEquation;
    mDefaultRenderStates[RS_BlendFunc] = new BlendFunc;
    mDefaultRenderStates[RS_ColorMask] = new ColorMask;
    mDefaultRenderStates[RS_CullFace] = new CullFace;
    mDefaultRenderStates[RS_DepthFunc] = new DepthFunc;
    mDefaultRenderStates[RS_DepthMask] = new DepthMask;
    mDefaultRenderStates[RS_DepthRange] = new DepthRange;
    // mDefaultRenderStates[RS_Fog] = new Fog;
    mDefaultRenderStates[RS_FrontFace] = new FrontFace;
    // mDefaultRenderStates[RS_PolygonMode] = new PolygonMode;
    // mDefaultRenderStates[RS_Hint] = new Hint;
    // mDefaultRenderStates[RS_LightModel] = new LightModel;
    // mDefaultRenderStates[RS_LineStipple] = new LineStipple;
    mDefaultRenderStates[RS_LineWidth] = new LineWidth;
    mDefaultRenderStates[RS_LogicOp] = new LogicOp;
    // mDefaultRenderStates[RS_Material] = new Material;
    // mic fixme ??? mDefaultRenderStates[RS_PixelTransfer] = new PixelTransfer;
    mDefaultRenderStates[RS_PointParameter] = new PointParameter;
    mDefaultRenderStates[RS_PointSize] = new PointSize;
    mDefaultRenderStates[RS_PolygonOffset] = new PolygonOffset;
    // mDefaultRenderStates[RS_PolygonStipple] = new PolygonStipple;
    mDefaultRenderStates[RS_SampleCoverage] = new SampleCoverage;
    mDefaultRenderStates[RS_ShadeModel] = new ShadeModel;
    mDefaultRenderStates[RS_StencilFunc] = new StencilFunc;
    mDefaultRenderStates[RS_StencilMask] = new StencilMask;
    mDefaultRenderStates[RS_StencilOp] = new StencilOp;
    mDefaultRenderStates[RS_GLSLProgram] = new GLSLProgram;

    // mDefaultRenderStates[RS_Light0] = new Light(0);
    // mDefaultRenderStates[RS_Light1] = new Light(1);
    // mDefaultRenderStates[RS_Light2] = new Light(2);
    // mDefaultRenderStates[RS_Light3] = new Light(3);
    // mDefaultRenderStates[RS_Light4] = new Light(4);
    // mDefaultRenderStates[RS_Light5] = new Light(5);
    // mDefaultRenderStates[RS_Light6] = new Light(6);
    // mDefaultRenderStates[RS_Light7] = new Light(7);

    // mDefaultRenderStates[RS_ClipPlane0] = new ClipPlane(0);
    // mDefaultRenderStates[RS_ClipPlane1] = new ClipPlane(1);
    // mDefaultRenderStates[RS_ClipPlane2] = new ClipPlane(2);
    // mDefaultRenderStates[RS_ClipPlane3] = new ClipPlane(3);
    // mDefaultRenderStates[RS_ClipPlane4] = new ClipPlane(4);
    // mDefaultRenderStates[RS_ClipPlane5] = new ClipPlane(5);

    for(int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
    {
      if (i < textureUnitCount())
      {
        mDefaultRenderStates[RS_TextureUnit0   + i] = new TextureUnit(i);
        // mDefaultRenderStates[RS_TexGen0        + i] = new TexGen(i);
        // mDefaultRenderStates[RS_TexEnv0        + i] = new TexEnv(i);
        // mDefaultRenderStates[RS_TextureMatrix0 + i] = new TextureMatrix(i);
      }
    }
  }

  // applyes our default render states
  for( unsigned i=0; i<RS_COUNT; ++i )
  {
    // the empty ones are the ones that are not supported by the current OpenGL implementation (too old or Core profile)
    if (mDefaultRenderStates[i])
    {
      VL_CHECK_OGL();
      mDefaultRenderStates[i]->apply(NULL, this);
      VL_CHECK_OGL();
    }
  }
}
//-----------------------------------------------------------------------------
void OpenGLContext::resetRenderStates()
{
  memset( mCurrentRenderState, 0, sizeof(mCurrentRenderState) );
  memset( mRenderStateTable,   0, sizeof(mRenderStateTable)   );
  memset( mTexUnitBinding,     0, sizeof( mTexUnitBinding )   ); // set to unknown texture target
}
//-----------------------------------------------------------------------------
void OpenGLContext::resetEnables()
{
  memset( mCurrentEnable, 0, sizeof(mCurrentEnable) );
  memset( mEnableTable,   0, sizeof(mEnableTable)   );
}
//------------------------------------------------------------------------------
bool OpenGLContext::checkIsCleanState() const
{
  struct contract {
    contract()  { VL_CHECK_OGL(); }
    ~contract() { VL_CHECK_OGL(); }
  } contract_instance;

  // everything must be disabled
  for( unsigned i=0; i<EN_EnableCount; ++i )
  {
    if (i == EN_MULTISAMPLE)
      continue;
    GLboolean enabled = glIsEnabled( TranslateEnable[i] );
    // const char* name = TranslateEnableString[i];
    if (glGetError() == GL_NO_ERROR && enabled)
    {
      VL_TRAP();
      return false;
    }
  }

  for( int i=0; i<8; ++i)
    if (glIsEnabled(GL_LIGHT0+i))
    {
      VL_TRAP();
      return false;
    }

  for( int i=0; i<6; ++i)
    if (glIsEnabled(GL_CLIP_PLANE0+i))
    {
      VL_TRAP();
      return false;
    }

  if (GLEW_VERSION_3_1)
    if (glIsEnabled(GL_PRIMITIVE_RESTART))
    {
      VL_TRAP();
      return false;
    }

  int tex_count = 1;
  if (GLEW_VERSION_1_3 || GLEW_ARB_multitexture)
  {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &tex_count); VL_CHECK_OGL()
  }
  while(tex_count--)
  {
    VL_glActiveTexture(GL_TEXTURE0+tex_count); VL_CHECK_OGL()
    VL_glClientActiveTexture(GL_TEXTURE0+tex_count); VL_CHECK_OGL()

    float matrix[16];
    float imatrix[16];
    glGetFloatv(GL_TEXTURE_MATRIX, matrix); VL_CHECK_OGL()
    glMatrixMode(GL_TEXTURE); VL_CHECK_OGL()
    glLoadIdentity(); VL_CHECK_OGL()
    glGetFloatv(GL_TEXTURE_MATRIX, imatrix); VL_CHECK_OGL()
    glLoadMatrixf(matrix);
    if (memcmp(matrix,imatrix,sizeof(matrix)) != 0)
    {
      VL_TRAP();
      return false;
    }

    if (glIsEnabled(GL_TEXTURE_COORD_ARRAY))
    {
      VL_TRAP();
      return false;
    }

    // check that all texture targets are disabled and bound to texture #0

    if (glIsEnabled(GL_TEXTURE_1D))
    {
      VL_TRAP();
      return false;
    }

    GLint bound_tex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound_tex);
    if (bound_tex != 0)
    {
      VL_TRAP();
      return false;
    }

    if (glIsEnabled(GL_TEXTURE_2D))
    {
      VL_TRAP();
      return false;
    }

    bound_tex = 0; 
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_tex);     
    if (bound_tex != 0)
    {
      VL_TRAP();
      return false;
    }

    if (GLEW_ARB_texture_rectangle||GLEW_EXT_texture_rectangle||GLEW_NV_texture_rectangle||GLEW_VERSION_3_1)
    {
      if (glIsEnabled(GL_TEXTURE_RECTANGLE))
      {
        VL_TRAP();
        return false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound_tex);     
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }
    }
    if (GLEW_VERSION_1_2||GLEW_VERSION_3_0)
    {
      if (glIsEnabled(GL_TEXTURE_3D))
      {
        VL_TRAP();
        return false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound_tex);     
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }
    }
    if (GLEW_VERSION_1_3||GLEW_ARB_texture_cube_map||GLEW_VERSION_3_0)
    {
      if (glIsEnabled(GL_TEXTURE_CUBE_MAP))
      {
        VL_TRAP();
        return false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound_tex);     
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }
    }

    if (GLEW_VERSION_3_0||GLEW_EXT_texture_array)
    {
      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound_tex);     
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound_tex);
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }
    }

    if (GLEW_VERSION_3_0||GLEW_ARB_texture_multisample)
    {
      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &bound_tex);     
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &bound_tex);
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }
    }

    if (GLEW_VERSION_3_1||GLEW_EXT_texture_buffer_object)
    {
      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &bound_tex);     
      if (bound_tex != 0)
      {
        VL_TRAP();
        return false;
      }
    }
  }

  if (glIsEnabled(GL_TEXTURE_GEN_S))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_TEXTURE_GEN_T))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_TEXTURE_GEN_R))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_TEXTURE_GEN_Q))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_COLOR_MATERIAL))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_SCISSOR_TEST))
  {
    VL_TRAP();
    return false;
  }

  if (GLEW_VERSION_1_4||GLEW_EXT_fog_coord)
    if (glIsEnabled(GL_FOG_COORD_ARRAY))
    {
      VL_TRAP();
      return false;
    }

  if (GLEW_VERSION_1_4||GLEW_EXT_secondary_color)
    if (glIsEnabled(GL_SECONDARY_COLOR_ARRAY))
    {
      VL_TRAP();
      return false;
    }

  if (glIsEnabled(GL_COLOR_ARRAY))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_EDGE_FLAG_ARRAY))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_INDEX_ARRAY))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_NORMAL_ARRAY))
  {
    VL_TRAP();
    return false;
  }

  if (glIsEnabled(GL_VERTEX_ARRAY))
    return false;

  if (GLEW_ARB_imaging)
  {
    if (glIsEnabled(GL_HISTOGRAM))
    {
      VL_TRAP();
      return false;
    }

    if (glIsEnabled(GL_MINMAX))
    {
      VL_TRAP();
      return false;
    }
  }

  // we expect these settings for the default blending equation
  GLint blend_src;
  GLint blend_dst;
  glGetIntegerv(GL_BLEND_SRC,&blend_src);
  glGetIntegerv(GL_BLEND_DST,&blend_dst);
  if (blend_src != GL_SRC_ALPHA)
  {
    VL_TRAP();
    return false;
  }
  if (blend_dst != GL_ONE_MINUS_SRC_ALPHA)
  {
    VL_TRAP();
    return false;
  }

  VL_CHECK_OGL();
  return true;
}
//-----------------------------------------------------------------------------
void OpenGLContext::resetContextStates()
{
  // Check that the OpenGL state is clear.
  // If this fails use VL_CHECK_OGL to make sure your application does not generate OpenGL errors.
  // See also glGetError() -> http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml
  VL_CHECK_OGL();

  // mic fixme: far dipendere da global settings
  VL_CHECK( checkIsCleanState() );

  VL_glBindFramebuffer(GL_FRAMEBUFFER, 0); VL_CHECK_OGL();

  if ( renderTarget()->openglContext()->hasDoubleBuffer() )
  {
    glDrawBuffer(GL_BACK); VL_CHECK_OGL();
    glReadBuffer(GL_BACK); VL_CHECK_OGL();
  }
  else
  {
    glDrawBuffer(GL_FRONT); VL_CHECK_OGL();
    glReadBuffer(GL_FRONT); VL_CHECK_OGL();
  }

  glDisable(GL_SCISSOR_TEST); VL_CHECK_OGL();

  // reset enables & render states
  renderTarget()->openglContext()->resetEnables();      VL_CHECK_OGL()
  renderTarget()->openglContext()->resetRenderStates(); VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------