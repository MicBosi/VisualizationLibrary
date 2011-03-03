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

#include <vlCore/VLSettings.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/OpenGL.hpp>
#include <vlGraphics/IVertexAttribSet.hpp>
#include <vlGraphics/Shader.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/ClipPlane.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
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
OpenGLContext* UIEventListener::openglContext() { return mOpenGLContext; }
//-----------------------------------------------------------------------------
// OpenGLContext
//-----------------------------------------------------------------------------
OpenGLContext::OpenGLContext(int w, int h): 
mMaxVertexAttrib(0), mTextureUnitCount(0), mMajorVersion(0), mMinorVersion(0),
mMouseVisible(true), mContinuousUpdate(true), mIgnoreNextMouseMoveEvent(false), mFullscreen(false), 
mHasDoubleBuffer(false), mIsInitialized(false), mIsCompatible(false), mCurVAS(NULL)
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
OpenGLContext::~OpenGLContext() 
{ 
  if (mFBORenderTarget.size() || mEventListeners.size())
    Log::warning("~OpenGLContext(): you should have called dispatchDestroyEvent() before destroying the OpenGLContext!\nNow it's too late to cleanup things!\n");
  
  // invalidate the render target
  mRenderTarget->mOpenGLContext = NULL;

  // invalidate FBOs
  for(unsigned i=0; i<mFBORenderTarget.size(); ++i)
  {
    // note, we can't destroy the FBOs here because it's too late to call makeCurrent().
    // mFBORenderTarget[i]->destroy();
    mFBORenderTarget[i]->mOpenGLContext = NULL;
  }

  // remove all the event listeners
  eraseAllEventListeners();
}
//-----------------------------------------------------------------------------
ref<FBORenderTarget> OpenGLContext::createFBORenderTarget(int width, int height)
{ 
  makeCurrent();
  mFBORenderTarget.push_back(new FBORenderTarget(this, width, height));
  mFBORenderTarget.back()->create();
  return mFBORenderTarget.back();
}
//-----------------------------------------------------------------------------
void OpenGLContext::destroyFBORenderTarget(FBORenderTarget* fbort)
{ 
  makeCurrent();
  for(unsigned i=0; i<mFBORenderTarget.size(); ++i)
  {
    if (mFBORenderTarget[i] == fbort)
    {
      mFBORenderTarget[i]->destroy();
      mFBORenderTarget[i]->mOpenGLContext = NULL;
      mFBORenderTarget.erase(mFBORenderTarget.begin()+i);
      break;
    }
  }
}
//-----------------------------------------------------------------------------
void OpenGLContext::destroyAllFBORenderTargets()
{
  makeCurrent();
  for(unsigned i=0; i<mFBORenderTarget.size(); ++i)
  {
    mFBORenderTarget[i]->destroy();
    mFBORenderTarget[i]->mOpenGLContext = NULL;
  }
  mFBORenderTarget.clear();
}
//-----------------------------------------------------------------------------
void OpenGLContext::addEventListener(UIEventListener* el)
{
  VL_CHECK( el );
  VL_CHECK( el->mOpenGLContext == NULL );
  if (el->mOpenGLContext == NULL)
  {
    mEventListeners.push_back(el);
    el->mOpenGLContext = this;
    el->addedListenerEvent(this);
    if (isInitialized())
      el->initEvent();
  }
}
//-----------------------------------------------------------------------------
void OpenGLContext::removeEventListener(UIEventListener* el)
{
  VL_CHECK( el );
  VL_CHECK( el->mOpenGLContext == this || el->mOpenGLContext == NULL );
  if (el->mOpenGLContext == this)
  {
    std::vector< ref<UIEventListener> >::iterator pos = std::find(mEventListeners.begin(), mEventListeners.end(), el);
    if( pos != mEventListeners.end() )
    {
      mEventListeners.erase( pos );
      // any operation here is safe, even adding or removing listeners.
      el->removedListenerEvent(this);
      el->mOpenGLContext = NULL;
    }
  }
}
//-----------------------------------------------------------------------------
void OpenGLContext::eraseAllEventListeners()
{
  // iterate on a temp vector so that any operations inside removedListenerEvent() is safe,
  // even adding or removing listeners.
  std::vector< ref<UIEventListener> > temp = mEventListeners;
  mEventListeners.clear();
  for(size_t i=0; i<temp.size(); ++i)
  {
    VL_CHECK( temp[i]->mOpenGLContext == this );
    temp[i]->removedListenerEvent(this);
    temp[i]->mOpenGLContext = NULL;
  }
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

  // find max number of texture units
  int max_tmp = 0;
  mTextureUnitCount = 1;
  if (GLEW_VERSION_1_3||GLEW_ARB_multitexture)
  {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_tmp); VL_CHECK_OGL();
    mTextureUnitCount = max_tmp > mTextureUnitCount ? max_tmp : mTextureUnitCount;
  }
  if (GLEW_VERSION_2_0)
  {
    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_tmp); VL_CHECK_OGL();
    mTextureUnitCount = max_tmp > mTextureUnitCount ? max_tmp : mTextureUnitCount;
  }
  if (GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
  {
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tmp); VL_CHECK_OGL();
    mTextureUnitCount = max_tmp > mTextureUnitCount ? max_tmp : mTextureUnitCount;
  }
  mTextureUnitCount = mTextureUnitCount < VL_MAX_TEXTURE_UNITS ? mTextureUnitCount : VL_MAX_TEXTURE_UNITS;

  // find max number of vertex attributes
  mMaxVertexAttrib = 0;
  if(GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS , &mMaxVertexAttrib);
  mMaxVertexAttrib = mMaxVertexAttrib < VL_MAX_GENERIC_VERTEX_ATTRIB ? mMaxVertexAttrib : VL_MAX_GENERIC_VERTEX_ATTRIB;

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

  if (globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_NORMAL)
  {
    Log::print(" --- GLEW ---\n");
    Log::print( Say("GLEW version: %s\n\n")<<glewGetString(GLEW_VERSION) );

    Log::print(" --- OpenGL Info ---\n");
    Log::print( Say("OpenGL version: %s\n") << glGetString(GL_VERSION) );
    Log::print( Say("OpenGL vendor: %s\n") << glGetString(GL_VENDOR) );
    Log::print( Say("OpenGL renderer: %s\n") << glGetString(GL_RENDERER) );
    Log::print( Say("OpenGL profile: %s\n") << (isCompatible() ? "Compatible" : "Core") );
    if (GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      Log::print( Say("GLSL version: %s\n")<<glGetString(GL_SHADING_LANGUAGE_VERSION) );
    int max_val = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_val);
    Log::print( Say("Max texture size: %n\n")<<max_val);
    max_val = 0;
    if (GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_val);
    Log::print( Say("Texture coords: %n\n") << max_val);
    max_val = 1;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3||GLEW_VERSION_3_0)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_val);
    Log::print( Say("Texture conventional units: %n\n") << max_val);
    max_val = 0;
    if (GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_val);
    Log::print( Say("Texture image units: %n\n") << max_val);
    max_val = 0;
    if (GLEW_ARB_multitexture||GLEW_VERSION_1_3)
      glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_val);
    Log::print( Say("Anisotropic texture filter: %s, ") << (GLEW_EXT_texture_filter_anisotropic? "YES" : "NO") );
    GLEW_EXT_texture_filter_anisotropic ? Log::print( Say("%nX\n") << max_val) : Log::print("\n");
    Log::print( Say("S3 Texture Compression: %s\n") << (GLEW_EXT_texture_compression_s3tc? "YES" : "NO") );
    Log::print( Say("Vertex Buffer Object: %s\n") << (GLEW_ARB_vertex_buffer_object ? "YES" : "NO"));
    Log::print( Say("Pixel Buffer Object: %s\n") << (GLEW_ARB_pixel_buffer_object ? "YES" : "NO"));
    Log::print( Say("Framebuffer Object: %s\n") << (GLEW_EXT_framebuffer_object ? "YES" : "NO"));
    max_val = 0;
    if(GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS , &max_val);
    Log::print( Say("Max vertex attributes: %n\n")<<max_val);
    VL_CHECK_OGL();
    max_val = 0;
    if(GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      glGetIntegerv(GL_MAX_VARYING_FLOATS , &max_val);
    Log::print( Say("Max varying floats: %n\n")<<max_val);
    VL_CHECK_OGL();
    max_val = 0;
    if(GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS , &max_val);
    Log::print( Say("Max fragment uniform components: %n\n")<<max_val);
    VL_CHECK_OGL();
    max_val = 0;
    if(GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
      glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS , &max_val);
    Log::print( Say("Max vertex uniform components: %n\n")<<max_val);
    VL_CHECK_OGL();
    max_val = 0;
    if(GLEW_VERSION_1_2||GLEW_VERSION_3_0)
      glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_val);
    Log::print( Say("Max elements vertices: %n\n") << max_val );
    max_val = 0;
    if(GLEW_VERSION_1_2||GLEW_VERSION_3_0)
      glGetIntegerv(GL_MAX_ELEMENTS_INDICES,  &max_val );
    Log::print( Say("Max elements indices: %n\n") << max_val );

    // --- print supported extensions on two columns ---

    Log::print("\n --- OpenGL Extensions --- \n");
    std::stringstream sstream;
    std::string ext_str;
    if (GLEW_VERSION_3_0)
    {
      int count = 0;
      glGetIntegerv(GL_NUM_EXTENSIONS, &count);
      for( int i=0; i<count; ++i )
      {
        const char* str = (const char*)glGetStringi(GL_EXTENSIONS,i); VL_CHECK_OGL();
        if (!str)
          break;
        ext_str += std::string(str) + " ";
      }
    } 
    else
    {
      VL_CHECK(glGetString(GL_EXTENSIONS));
      ext_str = (const char*)glGetString(GL_EXTENSIONS);
    }
    sstream << ext_str;
    std::string ext,line;
    for( int i=0; !sstream.eof(); ++i )
    {
      sstream >> ext;
      if (sstream.eof())
        break;

      if (i && i % 2)
      {
        line.resize(40,' ');
        line += ext;
        Log::print( Say("%s\n") << line );
        line.clear();
      }
      else
        line = ext;
    }
    if (line.length())
      Log::print( Say("%s\n") << line );
    Log::print("\n");
  }

  VL_CHECK_OGL();
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
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_ALPHA_TO_ONE,
    GL_SAMPLE_COVERAGE
  };

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
    "EN_SAMPLE_ALPHA_TO_COVERAGE",
    "EN_SAMPLE_ALPHA_TO_ONE",
    "EN_SAMPLE_COVERAGE"
  };
}
//------------------------------------------------------------------------------
void OpenGLContext::applyEnables( const EnableSet* prev, const EnableSet* cur )
{
  VL_CHECK_OGL()
  if (prev == NULL)
    memset( mEnableTable, 0, sizeof(mEnableTable) );

  /* iterate on current */

  if (cur)
  {
    for( unsigned i=0; i<cur->enables().size(); ++i )
    {
      mEnableTable[cur->enables()[i]] += 1; // 0 -> 1; 1 -> 2;
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

  /* iterate on prev: reset to default only the unused ones */

  if (prev)
  {
    for( unsigned i=0; i<prev->enables().size(); ++i )
    {
      if ( mEnableTable[prev->enables()[i]] == 1 )
      {
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
      mEnableTable[prev->enables()[i]] >>= 1; // 1 -> 0; 2 -> 1
    }
  }
  else
  {
    memset(mCurrentEnable, 0, sizeof(mCurrentEnable));
  }
}
//------------------------------------------------------------------------------
void OpenGLContext::applyRenderStates( const RenderStateSet* prev, const RenderStateSet* cur, const Camera* camera )
{
  if (prev == NULL)
    memset( mRenderStateTable, 0, sizeof(mRenderStateTable) );

  /* iterate on current */

  if (cur)
  {
    for( unsigned i=0; i<cur->renderStates().size(); ++i )
    {
      mRenderStateTable[cur->renderStates()[i]->type()] += 1; // 0 -> 1; 1 -> 2;
      if ( mCurrentRenderState[cur->renderStates()[i]->type()] != cur->renderStates()[i] )
      {
        mCurrentRenderState[cur->renderStates()[i]->type()] = cur->renderStates()[i].get();
        VL_CHECK(cur->renderStates()[i]);      
        cur->renderStates()[i]->apply(camera, this); VL_CHECK_OGL()
      }
    }
  }

  /* iterate on prev: reset to default only the unused ones */

  if (prev)
  {
    for( unsigned i=0; i<prev->renderStates().size(); ++i )
    {
      if ( mRenderStateTable[prev->renderStates()[i]->type()] == 1 )
      {
        mCurrentRenderState[prev->renderStates()[i]->type()] = mDefaultRenderStates[prev->renderStates()[i]->type()].get();
        #ifndef NDEBUG
        if (!mDefaultRenderStates[prev->renderStates()[i]->type()])
        {
          vl::Log::error( Say("Render state type '%s' not supported by this OpenGL implementation!\n") << prev->renderStates()[i]->className() );
          VL_TRAP()
        }
        #endif
        // if this fails you are using a render state that is not supported by the current OpenGL implementation (too old or Core profile)
        mDefaultRenderStates[prev->renderStates()[i]->type()]->apply(NULL, this); VL_CHECK_OGL()
      }
      mRenderStateTable[prev->renderStates()[i]->type()] >>= 1; // 1 -> 0; 2 -> 1;
    }
  }
  else
  {
    memset(mCurrentRenderState, 0, sizeof(mCurrentRenderState));
  }
}
//------------------------------------------------------------------------------
void OpenGLContext::setupDefaultRenderStates()
{
  // initialize to NULL
  memset(mDefaultRenderStates, 0, sizeof(mDefaultRenderStates));

  if ( isCompatible() ) /* COMPATIBLE profile */
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
  else /* CORE profile */
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

  // applies our default render states
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
  VL_glActiveTexture( GL_TEXTURE0 );
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
bool OpenGLContext::isCleanState(bool verbose)
{
  struct contract {
    contract()  { VL_CHECK_OGL(); }
    ~contract() { VL_CHECK_OGL(); }
  } contract_instance;

  bool ok  = true;

  // everything must be disabled
  for( unsigned i=0; i<EN_EnableCount; ++i )
  {
    GLboolean enabled = glIsEnabled( TranslateEnable[i] );
    // const char* name = TranslateEnableString[i];
    if (glGetError() == GL_NO_ERROR && enabled)
    {
	    if (verbose)
		    vl::Log::error( Say("Capability %s was enabled!\n") << TranslateEnableString[i] );
      VL_TRAP();
      ok = false;
    }
  }

  for( int i=0; i<8; ++i)
    if (glIsEnabled(GL_LIGHT0+i))
    {
      if (verbose)
        vl::Log::error( Say("GL_LIGHT%n was enabled!\n") << i );
      VL_TRAP();
      ok = false;
    }

  for( int i=0; i<6; ++i)
    if (glIsEnabled(GL_CLIP_PLANE0+i))
    {
      if (verbose)
        vl::Log::error( Say("GL_CLIP_PLANE%n was enabled!\n") << i );
      VL_TRAP();
      ok = false;
    }

  if (GLEW_VERSION_3_1)
    if (glIsEnabled(GL_PRIMITIVE_RESTART))
    {
      if (verbose)
        vl::Log::error( "GL_PRIMITIVE_RESTART was enabled!\n" );
      VL_TRAP();
      ok = false;
    }

  if(GLEW_VERSION_1_3||GLEW_ARB_multitexture||GLEW_VERSION_2_0||GLEW_VERSION_3_0)
  {
    int active_tex = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_tex); VL_CHECK_OGL();
    active_tex -= GL_TEXTURE0;
    if (active_tex != 0)
    {
      if (verbose)
        vl::Log::error( Say("Active texture unit is #%n instead of #0!\n") << active_tex );
      VL_TRAP();
      ok = false;
    }

    glGetIntegerv(GL_CLIENT_ACTIVE_TEXTURE, &active_tex); VL_CHECK_OGL();
    active_tex -= GL_TEXTURE0;
    if (active_tex != 0)
    {
      if (verbose)
        vl::Log::error( Say("Active client texture unit is #%n instead of #0!\n") << active_tex );
      VL_TRAP();
      ok = false;
    }
  }

  // find max number of texture coords
  // int units_count = 1;
  int coord_count = 1;
  int max_tmp = 0;
  if (GLEW_VERSION_1_3||GLEW_ARB_multitexture)
  {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_tmp); VL_CHECK_OGL();
    coord_count = max_tmp > coord_count ? max_tmp : coord_count;
  }
  if (GLEW_VERSION_2_0)
  {
    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_tmp); VL_CHECK_OGL();
    coord_count = max_tmp > coord_count ? max_tmp : coord_count;
  }
  //if (GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
  //{
  //  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tmp); VL_CHECK_OGL();
  //  units_count = max_tmp > coord_count ? max_tmp : coord_count;
  //}
  while(coord_count--)
  {
    VL_glActiveTexture(GL_TEXTURE0+coord_count); VL_CHECK_OGL()
    VL_glClientActiveTexture(GL_TEXTURE0+coord_count); VL_CHECK_OGL()

    float matrix[16];
    float imatrix[16];
    glGetFloatv(GL_TEXTURE_MATRIX, matrix); VL_CHECK_OGL()
    glMatrixMode(GL_TEXTURE); VL_CHECK_OGL()
    glLoadIdentity(); VL_CHECK_OGL()
    glGetFloatv(GL_TEXTURE_MATRIX, imatrix); VL_CHECK_OGL()
    glLoadMatrixf(matrix);
    if (memcmp(matrix,imatrix,sizeof(matrix)) != 0)
    {
      if (verbose)
        vl::Log::error( Say("Texture matrix was not set to identity on texture unit %n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    if (glIsEnabled(GL_TEXTURE_COORD_ARRAY))
    {
      if (verbose)
        vl::Log::error( Say("GL_TEXTURE_COORD_ARRAY was enabled on texture unit %n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    // check that all texture targets are disabled and bound to texture #0

    if (glIsEnabled(GL_TEXTURE_1D))
    {
      if (verbose)
        vl::Log::error( Say("GL_TEXTURE_1D was enabled on texture unit #%n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    GLint bound_tex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound_tex);
    if (bound_tex != 0)
    {
      if (verbose)
        vl::Log::error( Say("GL_TEXTURE_BINDING_1D != 0 on texture unit #%n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    if (glIsEnabled(GL_TEXTURE_2D))
    {
      if (verbose)
        vl::Log::error( Say("GL_TEXTURE_2D was enabled on texture unit #%n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    bound_tex = 0; 
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_tex);     
    if (bound_tex != 0)
    {
      if (verbose)
        vl::Log::error( Say("GL_TEXTURE_BINDING_2D != 0 on texture unit #%n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    if (GLEW_ARB_texture_rectangle||GLEW_EXT_texture_rectangle||GLEW_NV_texture_rectangle||GLEW_VERSION_3_1)
    {
      if (glIsEnabled(GL_TEXTURE_RECTANGLE))
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_RECTANGLE was enabled on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound_tex);     
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_RECTANGLE != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (GLEW_VERSION_1_2||GLEW_VERSION_3_0)
    {
      if (glIsEnabled(GL_TEXTURE_3D))
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_3D was enabled on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound_tex);     
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_3D != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }
    if (GLEW_VERSION_1_3||GLEW_ARB_texture_cube_map||GLEW_VERSION_3_0)
    {
      if (glIsEnabled(GL_TEXTURE_CUBE_MAP))
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_CUBE_MAP was enabled on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound_tex);     
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_CUBE_MAP != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (GLEW_VERSION_3_0||GLEW_EXT_texture_array)
    {
      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY, &bound_tex);     
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_1D_ARRAY != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY, &bound_tex);
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_2D_ARRAY != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (GLEW_VERSION_3_2||GLEW_ARB_texture_multisample)
    {
      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &bound_tex);     
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_2D_MULTISAMPLE != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, &bound_tex);
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (GLEW_VERSION_3_1||GLEW_EXT_texture_buffer_object)
    {
      bound_tex = 0; 
      glGetIntegerv(GL_TEXTURE_BINDING_BUFFER, &bound_tex);     
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_BUFFER != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (glIsEnabled(GL_TEXTURE_GEN_S))
    {
      if (verbose)
        vl::Log::error( "GL_TEXTURE_GEN_S was enabled!\n" );
      VL_TRAP();
      ok = false;
    }

    if (glIsEnabled(GL_TEXTURE_GEN_T))
    {
      if (verbose)
        vl::Log::error( "GL_TEXTURE_GEN_T was enabled!\n" );
      VL_TRAP();
      ok = false;
    }

    if (glIsEnabled(GL_TEXTURE_GEN_R))
    {
      if (verbose)
        vl::Log::error( "GL_TEXTURE_GEN_R was enabled!\n" );
      VL_TRAP();
      ok = false;
    }

    if (glIsEnabled(GL_TEXTURE_GEN_Q))
    {
      if (verbose)
        vl::Log::error( "GL_TEXTURE_GEN_Q was enabled!\n" );
      VL_TRAP();
      ok = false;
    }
  }

  if (glIsEnabled(GL_COLOR_MATERIAL))
  {
    if (verbose)
      vl::Log::error( "GL_COLOR_MATERIAL was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (glIsEnabled(GL_SCISSOR_TEST))
  {
    if (verbose)
      vl::Log::error( "GL_SCISSOR_TEST was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (GLEW_VERSION_1_4||GLEW_EXT_fog_coord)
    if (glIsEnabled(GL_FOG_COORD_ARRAY))
    {
      if (verbose)
        vl::Log::error( "GL_FOG_COORD_ARRAY was enabled!\n");
      VL_TRAP();
      ok = false;
    }

  if (GLEW_VERSION_1_4||GLEW_EXT_secondary_color)
    if (glIsEnabled(GL_SECONDARY_COLOR_ARRAY))
    {
      if (verbose)
        vl::Log::error( "GL_SECONDARY_COLOR_ARRAY was enabled!\n");
      VL_TRAP();
      ok = false;
    }

  if (glIsEnabled(GL_COLOR_ARRAY))
  {
    if (verbose)
      vl::Log::error( "GL_COLOR_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (glIsEnabled(GL_EDGE_FLAG_ARRAY))
  {
    if (verbose)
      vl::Log::error( "GL_EDGE_FLAG_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (glIsEnabled(GL_INDEX_ARRAY))
  {
    if (verbose)
      vl::Log::error( "GL_INDEX_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (glIsEnabled(GL_NORMAL_ARRAY))
  {
    if (verbose)
      vl::Log::error( "GL_NORMAL_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (glIsEnabled(GL_VERTEX_ARRAY))
  {
    if (verbose)
      vl::Log::error( "GL_VERTEX_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  GLint max_vert_attribs = 0;
  if (GLEW_VERSION_2_0||GLEW_VERSION_3_0||GLEW_VERSION_4_0)
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_vert_attribs);
  for(int i=0; i<max_vert_attribs; ++i)
  {
    GLint is_enabled = 0;
    glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &is_enabled);
    if (is_enabled)
    {
      vl::Log::error( Say("GL_VERTEX_ATTRIB_ARRAY #%n is enabled!\n") << i );
      ok = false;
    }
  }

  if (GLEW_ARB_imaging)
  {
    if (glIsEnabled(GL_HISTOGRAM))
    {
      if (verbose)
        vl::Log::error( "GL_HISTOGRAM was enabled!\n");
      VL_TRAP();
      ok = false;
    }

    if (glIsEnabled(GL_MINMAX))
    {
      if (verbose)
        vl::Log::error( "GL_MINMAX was enabled!\n");
      VL_TRAP();
      ok = false;
    }
  }

  // we expect these settings for the default blending equation
  GLint blend_src;
  GLint blend_dst;
  glGetIntegerv( GL_BLEND_SRC, &blend_src );
  glGetIntegerv( GL_BLEND_DST, &blend_dst );
  if (blend_src != GL_SRC_ALPHA)
  {
    if (verbose)
      vl::Log::error( "GL_BLEND_SRC is not GL_SRC_ALPHA!\n");
    VL_TRAP();
    ok = false;
  }
  if (blend_dst != GL_ONE_MINUS_SRC_ALPHA)
  {
    if (verbose)
      vl::Log::error( "GL_BLEND_DST is not GL_ONE_MINUS_SRC_ALPHA!\n");
    VL_TRAP();
    ok = false;
  }

  // buffer object bindings

  GLint buf_bind = 0;
  if (GLEW_VERSION_1_5)
  {
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buf_bind);
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_ARRAY_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
    buf_bind = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buf_bind);
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_ELEMENT_ARRAY_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  if (GLEW_VERSION_2_1)
  {
    buf_bind = 0;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &buf_bind);
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_PIXEL_PACK_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
    buf_bind = 0;
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &buf_bind);
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_PIXEL_UNPACK_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  if (GLEW_ARB_uniform_buffer_object)
  {
    buf_bind = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &buf_bind);
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_UNIFORM_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  if(GLEW_VERSION_3_0)
  {
    buf_bind = 0;
    glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &buf_bind);
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  /* mic fixme: GLEW forgot these?
  glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &buf_bind);
  if (buf_bind != 0)
  {
    if (verbose)
      vl::Log::error( "GL_COPY_READ_BUFFER_BINDING should be 0!\n");
    VL_TRAP();
    ok = false;
  }
  glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &buf_bind);
  if (buf_bind != 0)
  {
    if (verbose)
      vl::Log::error( "GL_COPY_WRITE_BUFFER_BINDING should be 0!\n");
    VL_TRAP();
    ok = false;
  }
  glGetIntegerv(GL_TEXTURE_BUFFER_BINDING, &buf_bind);
  if (buf_bind != 0)
  {
    if (verbose)
      vl::Log::error( "GL_TEXTURE_BUFFER_BINDING should be 0!\n");
    VL_TRAP();
    ok = false;
  }
  */

  #if 0
  // check viewport
  GLint viewport[4] = {0,0,0,0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  if (viewport[2] * viewport[3] == 1)
  {
    if (verbose)
      vl::Log::error( "Viewport dimension is 1 pixel!\n"
      "Did you forget to call camera()->viewport()->setWidth()/setHeight() upon window resize event?\n");
    VL_TRAP();
    ok = false;
  }
  #endif

  GLboolean write_mask[4];
  glGetBooleanv(GL_COLOR_WRITEMASK, write_mask);
  if( !write_mask[0] || !write_mask[1] || !write_mask[2] || !write_mask[3] )
  {
    vl::Log::error( "Color write-mask should be glColorMask(GL_TRUE ,GL_TRUE, GL_TRUE, GL_TRUE)!\n" );
    ok = false; 
  }

  glGetBooleanv(GL_DEPTH_WRITEMASK, write_mask);
  if ( !write_mask[0] )
  {
    vl::Log::error( "Depth write-mask should be glDepthMask(GL_TRUE)!\n" );
    ok = false; 
  }

  GLint poly_mode[2];
  glGetIntegerv(GL_POLYGON_MODE, poly_mode);
  if ( poly_mode[0] != GL_FILL || poly_mode[1] != GL_FILL )
  {
    vl::Log::error( "Polygon mode should be glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)!\n" );
    ok = false;
  }

  VL_CHECK_OGL();
  return ok;
}
//-----------------------------------------------------------------------------
bool OpenGLContext::areUniformsColliding(const UniformSet* u1, const UniformSet* u2)
{
  // mic fixme: test it
  if (!u1 || !u2)
    return false;

  // compile the map
  std::set<std::string> name_set;
  for( size_t i=0; i<u1->uniforms().size(); ++i )
    name_set.insert( u1->uniforms()[i]->name() );

  bool ok = false;
  // check the map
  for( size_t j=0; j<u2->uniforms().size(); ++j )
    if ( name_set.find( u2->uniforms()[j]->name() ) != name_set.end() )
    {
      vl::Log::error( Say("Uniform name collision detected: %s\n") << u2->uniforms()[j]->name() );
      ok = true;
    }

  return ok;
}
//-----------------------------------------------------------------------------
void OpenGLContext::resetContextStates()
{
  // Check that the OpenGL state is clear.
  // If this fails use VL_CHECK_OGL to make sure your application does not generate OpenGL errors.
  // See also glGetError() -> http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml
  VL_CHECK_OGL();

  // perform extra OpenGL environment sanity check
  if (globalSettings()->checkOpenGLStates() && !isCleanState(true))
  {
    VL_TRAP();
    return;
  }

  VL_glBindFramebuffer(GL_FRAMEBUFFER, 0); VL_CHECK_OGL();

  if ( hasDoubleBuffer() )
  {
    glDrawBuffer(GL_BACK); VL_CHECK_OGL();
    glReadBuffer(GL_BACK); VL_CHECK_OGL();
  }
  else
  {
    glDrawBuffer(GL_FRONT); VL_CHECK_OGL();
    glReadBuffer(GL_FRONT); VL_CHECK_OGL();
  }

  // reset internal VL enables & render states tables
  resetEnables();      VL_CHECK_OGL()
  resetRenderStates(); VL_CHECK_OGL()

  // reset Vertex Attrib Set tables
  bindVAS(NULL, false, true);
}
//-----------------------------------------------------------------------------
void OpenGLContext::bindVAS(const IVertexAttribSet* vas, bool use_vbo, bool force)
{
  // bring opengl to a known state

  if (vas != mCurVAS || force)
  {
    if (!vas || force)
    {
      mCurVAS = NULL;

      // reset all internal states

      for(int i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      {
        mVertexAttrib[i].mEnabled = false; // not used
        mVertexAttrib[i].mPtr = 0;
        mVertexAttrib[i].mVBO = 0;
        mVertexAttrib[i].mState = 0;
      }

      for(int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      {
        mTexCoordArray[i].mEnabled = false; // not used
        mTexCoordArray[i].mPtr = 0;
        mTexCoordArray[i].mVBO = 0;
        mTexCoordArray[i].mState = 0;
      }

      mVertexArray.mEnabled = false;
      mVertexArray.mPtr = 0;
      mVertexArray.mVBO = 0;
      mVertexArray.mState = 0; // not used

      mNormalArray.mEnabled = false;
      mNormalArray.mPtr = 0;
      mNormalArray.mVBO = 0;
      mNormalArray.mState = 0; // not used

      mColorArray.mEnabled = false;
      mColorArray.mPtr = 0;
      mColorArray.mVBO = 0;
      mColorArray.mState = 0; // not used

      mSecondaryColorArray.mEnabled = false;
      mSecondaryColorArray.mPtr = 0;
      mSecondaryColorArray.mVBO = 0;
      mSecondaryColorArray.mState = 0; // not used

      mFogArray.mEnabled = false;
      mFogArray.mPtr = 0;
      mFogArray.mVBO = 0;
      mFogArray.mState = 0; // not used

      // reset all gl states

      for(int i=0; i<mMaxVertexAttrib; ++i)
        VL_glDisableVertexAttribArray(i);

      // iterate backwards so the last active is #0
      for ( int i=mTextureUnitCount; i--; )
      {
        VL_glClientActiveTexture(GL_TEXTURE0 + i);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      }

      // note this one
      VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

      glDisableClientState(GL_VERTEX_ARRAY);

      glDisableClientState(GL_NORMAL_ARRAY);

      glDisableClientState(GL_COLOR_ARRAY);

      glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

      glDisableClientState(GL_FOG_COORD_ARRAY);
    }

    if (vas)
    {
      int vbo = 0;
      const unsigned char* ptr = 0;
      bool enabled = false;

      // ----- vertex array -----

      enabled = vas->vertexArray() != NULL;
      if ( mVertexArray.mEnabled || enabled )
      {
        mVertexArray.mEnabled = enabled;
        if (enabled)
        {
          if ( use_vbo && vas->vertexArray()->gpuBuffer()->handle() )
          {
            vbo = vas->vertexArray()->gpuBuffer()->handle();
            ptr = 0;
          }
          else
          {
            vbo = 0;
            ptr = vas->vertexArray()->gpuBuffer()->ptr();
          }
          if ( mVertexArray.mPtr != ptr || mVertexArray.mVBO != vbo )
          {
            mVertexArray.mPtr = ptr;
            mVertexArray.mVBO = vbo;
            VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();
            glVertexPointer((int)vas->vertexArray()->glSize(), vas->vertexArray()->glType(), /*stride*/0, ptr); VL_CHECK_OGL();
            glEnableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
          }
        }
        else
        {
          glDisableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
          mVertexArray.mPtr = 0;
          mVertexArray.mVBO = 0;
        }
      }

      // ----- normal array -----

      enabled = vas->normalArray() != NULL;
      if ( mNormalArray.mEnabled || enabled )
      {
        mNormalArray.mEnabled = enabled;
        if (enabled)
        {
          if ( use_vbo && vas->normalArray()->gpuBuffer()->handle() )
          {
            vbo = vas->normalArray()->gpuBuffer()->handle();
            ptr = 0;
          }
          else
          {
            vbo = 0;
            ptr = vas->normalArray()->gpuBuffer()->ptr();
          }
          if ( mNormalArray.mPtr != ptr || mNormalArray.mVBO != vbo )
          {
            mNormalArray.mPtr = ptr;
            mNormalArray.mVBO = vbo;
            VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();
            glNormalPointer(vas->normalArray()->glType(), /*stride*/0, ptr); VL_CHECK_OGL();
            glEnableClientState(GL_NORMAL_ARRAY); VL_CHECK_OGL();
          }
        }
        else
        {
          glDisableClientState(GL_NORMAL_ARRAY); VL_CHECK_OGL();
          mNormalArray.mPtr = 0;
          mNormalArray.mVBO = 0;
        }
      }

      // ----- color array -----

      enabled = vas->colorArray() != NULL;
      if ( mColorArray.mEnabled || enabled )
      {
        mColorArray.mEnabled = enabled;
        if (enabled)
        {
          if ( use_vbo && vas->colorArray()->gpuBuffer()->handle() )
          {
            vbo = vas->colorArray()->gpuBuffer()->handle();
            ptr = 0;
          }
          else
          {
            vbo = 0;
            ptr = vas->colorArray()->gpuBuffer()->ptr();
          }
          if ( mColorArray.mPtr != ptr || mColorArray.mVBO != vbo )
          {
            mColorArray.mPtr = ptr;
            mColorArray.mVBO = vbo;
            VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();
            glColorPointer((int)vas->colorArray()->glSize(), vas->colorArray()->glType(), /*stride*/0, ptr); VL_CHECK_OGL();
            glEnableClientState(GL_COLOR_ARRAY); VL_CHECK_OGL();
          }
        }
        else
        {
          glDisableClientState(GL_COLOR_ARRAY); VL_CHECK_OGL();
          mColorArray.mPtr = 0;
          mColorArray.mVBO = 0;
        }
      }

      // ----- secondary color array -----

      enabled = vas->secondaryColorArray() != NULL;
      if ( mSecondaryColorArray.mEnabled || enabled )
      {
        mSecondaryColorArray.mEnabled = enabled;
        if (enabled)
        {
          if ( use_vbo && vas->secondaryColorArray()->gpuBuffer()->handle() )
          {
            vbo = vas->secondaryColorArray()->gpuBuffer()->handle();
            ptr = 0;
          }
          else
          {
            vbo = 0;
            ptr = vas->secondaryColorArray()->gpuBuffer()->ptr();
          }
          if ( mSecondaryColorArray.mPtr != ptr || mSecondaryColorArray.mVBO != vbo )
          {
            mSecondaryColorArray.mPtr = ptr;
            mSecondaryColorArray.mVBO = vbo;
            VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();
            glSecondaryColorPointer((int)vas->secondaryColorArray()->glSize(), vas->secondaryColorArray()->glType(), /*stride*/0, ptr); VL_CHECK_OGL();
            glEnableClientState(GL_SECONDARY_COLOR_ARRAY); VL_CHECK_OGL();
          }
        }
        else
        {
          glDisableClientState(GL_SECONDARY_COLOR_ARRAY); VL_CHECK_OGL();
          mSecondaryColorArray.mPtr = 0;
          mSecondaryColorArray.mVBO = 0;
        }
      }

      // ----- fog array -----

      enabled = vas->fogCoordArray() != NULL;
      if ( mFogArray.mEnabled || enabled )
      {
        mFogArray.mEnabled = enabled;
        if (enabled)
        {
          if ( use_vbo && vas->fogCoordArray()->gpuBuffer()->handle() )
          {
            vbo = vas->fogCoordArray()->gpuBuffer()->handle();
            ptr = 0;
          }
          else
          {
            vbo = 0;
            ptr = vas->fogCoordArray()->gpuBuffer()->ptr();
          }
          if ( mFogArray.mPtr != ptr || mFogArray.mVBO != vbo )
          {
            mFogArray.mPtr = ptr;
            mFogArray.mVBO = vbo;
            VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();
            glFogCoordPointer(vas->fogCoordArray()->glType(), /*stride*/0, ptr); VL_CHECK_OGL();
            glEnableClientState(GL_FOG_COORD_ARRAY); VL_CHECK_OGL();
          }
        }
        else
        {
          glDisableClientState(GL_FOG_COORD_ARRAY); VL_CHECK_OGL();
          mFogArray.mPtr = 0;
          mFogArray.mVBO = 0;
        }
      }

      // ----- vertex attrib -----

      // (1) enable pass
      for(int i=0; i<vas->vertexAttribInfoCount(); ++i)
      {
        const VertexAttribInfo* info = vas->getVertexAttribInfoAt(i);
        int idx = info->attribIndex();

        mVertexAttrib[idx].mState += 1; // 0 -> 1; 1 -> 2;
        VL_CHECK( mVertexAttrib[idx].mState == 1 || mVertexAttrib[idx].mState == 2 );

        if ( use_vbo && info->data()->gpuBuffer()->handle() )
        {
          vbo = info->data()->gpuBuffer()->handle();
          ptr = 0;
        }
        else
        {
          vbo = 0;
          ptr = info->data()->gpuBuffer()->ptr();
        }
        if ( mVertexAttrib[idx].mPtr != ptr || mVertexAttrib[idx].mVBO != vbo )
        {
          mVertexAttrib[idx].mPtr = ptr;
          mVertexAttrib[idx].mVBO = vbo;
          VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();

          if ( info->dataBehavior() == VAB_NORMAL )
          {
            VL_glVertexAttribPointer( idx, (int)info->data()->glSize(), info->data()->glType(), info->normalize(), /*stride*/0, ptr ); VL_CHECK_OGL();
          }
          else
          if ( info->dataBehavior() == VAB_PURE_INTEGER )
          {
            VL_glVertexAttribIPointer( idx, (int)info->data()->glSize(), info->data()->glType(), /*stride*/0, ptr ); VL_CHECK_OGL();
          }
          else
          if ( info->dataBehavior() == VAB_PURE_DOUBLE )
          {
            VL_glVertexAttribLPointer( idx, (int)info->data()->glSize(), info->data()->glType(), /*stride*/0, ptr ); VL_CHECK_OGL();
          }

          // enable if not previously enabled
          if (mVertexAttrib[idx].mState == 1)
          {
            VL_glEnableVertexAttribArray( idx ); VL_CHECK_OGL();
          }
        }
      }

      // (2) disable pass
      if (mCurVAS)
      {
        for(int i=0; i<mCurVAS->vertexAttribInfoCount(); ++i)
        {
          // vertex array
          const VertexAttribInfo* info = mCurVAS->getVertexAttribInfoAt(i);
          VL_CHECK(info)
            int idx = info->attribIndex();
          // disable if not used by new VAS
          if ( mVertexAttrib[idx].mState == 1 )
          {
            VL_glDisableVertexAttribArray( idx ); VL_CHECK_OGL();
          }

          mVertexAttrib[idx].mPtr = 0;
          mVertexAttrib[idx].mVBO = 0;
          mVertexAttrib[idx].mState >>= 1; // 1 -> 0; 2 -> 1;
        }
      }

      // ----- texture coords -----

      // (1) enable pass
      for(int i=0; i<vas->texCoordArrayCount(); ++i)
      {
        // texture array info
        const ArrayAbstract* texarr = NULL;
        int tex_unit = 0;
        vas->getTexCoordArrayAt(i, tex_unit, texarr);
        VL_CHECK(tex_unit<VL_MAX_TEXTURE_UNITS);

        mTexCoordArray[tex_unit].mState += 1; // 0 -> 1; 1 -> 2;
        VL_CHECK( mTexCoordArray[tex_unit].mState == 1 || mTexCoordArray[tex_unit].mState == 2 );

        if ( use_vbo && texarr->gpuBuffer()->handle() )
        {
          vbo = texarr->gpuBuffer()->handle();
          ptr = 0;
        }
        else
        {
          vbo = 0;
          ptr = texarr->gpuBuffer()->ptr();
        }
        if ( mTexCoordArray[tex_unit].mPtr != ptr || mTexCoordArray[tex_unit].mVBO != vbo )
        {
          mTexCoordArray[tex_unit].mPtr = ptr;
          mTexCoordArray[tex_unit].mVBO = vbo;

          VL_glClientActiveTexture(GL_TEXTURE0 + tex_unit); VL_CHECK_OGL();
          VL_glBindBuffer(GL_ARRAY_BUFFER, vbo); VL_CHECK_OGL();
          glTexCoordPointer((int)texarr->glSize(), texarr->glType(), 0/*texarr->stride()*/, ptr/*+ texarr->offset()*/);

          // enable if not previously enabled
          if (mTexCoordArray[tex_unit].mState == 1)
          {
            glEnableClientState(GL_TEXTURE_COORD_ARRAY); VL_CHECK_OGL();
          }
          else
          {
            VL_CHECK(glIsEnabled(GL_TEXTURE_COORD_ARRAY));
          }
        }
      }

      // (2) disable pass
      if (mCurVAS)
      {
        for(int i=0; i<mCurVAS->texCoordArrayCount(); ++i)
        {
          // texture array info
          const ArrayAbstract* texarr = NULL;
          int tex_unit = 0;
          mCurVAS->getTexCoordArrayAt(i, tex_unit, texarr);
          VL_CHECK(tex_unit<VL_MAX_TEXTURE_UNITS);

          // disable if not used by new VAS
          if ( mTexCoordArray[tex_unit].mState == 1 )
          {
            VL_glClientActiveTexture(GL_TEXTURE0 + tex_unit); VL_CHECK_OGL();
            glDisableClientState(GL_TEXTURE_COORD_ARRAY); VL_CHECK_OGL();
          }

          mTexCoordArray[tex_unit].mPtr = 0;
          mTexCoordArray[tex_unit].mVBO = 0;
          mTexCoordArray[tex_unit].mState >>= 1; // 1 -> 0; 2 -> 1;
        }
      }

      VL_glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
  }

  mCurVAS = vas;
}
//-----------------------------------------------------------------------------
