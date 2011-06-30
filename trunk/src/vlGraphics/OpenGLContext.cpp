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

using namespace vl;

//-----------------------------------------------------------------------------
// UIEventListener
//-----------------------------------------------------------------------------
OpenGLContext* UIEventListener::openglContext() { return mOpenGLContext; }
//-----------------------------------------------------------------------------
// OpenGLContext
//-----------------------------------------------------------------------------
OpenGLContext::OpenGLContext(int w, int h):
mMaxVertexAttrib(0), mTextureUnitCount(0), 
mMouseVisible(true), mContinuousUpdate(true), mIgnoreNextMouseMoveEvent(false), mFullscreen(false),
mHasDoubleBuffer(false), mIsInitialized(false), mCurVAS(NULL)
{
  VL_DEBUG_SET_OBJECT_NAME()
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
#if defined(VL_OPENGL) && defined(VL_PLATFORM_WINDOWS)
  makeCurrent();
  if (Has_GL_EXT_swap_control)
    wglSwapIntervalEXT(enable?1:0);
#else
  // Mac and Linux?
#endif
}
//-----------------------------------------------------------------------------
bool OpenGLContext::vsyncEnabled() const
{
#if defined(VL_OPENGL) && defined(VL_PLATFORM_WINDOWS)
  if (Has_GL_EXT_swap_control)
    return wglGetSwapIntervalEXT() != 0;
  else
    return false;
#else
  return false;
#endif
}
//-----------------------------------------------------------------------------
bool OpenGLContext::initGLContext(bool log)
{
  mIsInitialized = false;

  makeCurrent();

  // init OpenGL extensions
  if (!initializeOpenGL())
  {
    Log::error("Error initializing OpenGL!\n");
    VL_TRAP()
    return false;
  }
  else
  {
    mIsInitialized = true;
    mExtensions = getOpenGLExtensions();
  }

  VL_CHECK_OGL();

  if (log)
  {
    logOpenGLInfo();
  }

  // Find max number of texture units, see http://www.opengl.org/sdk/docs/man/xhtml/glActiveTexture.xml
  mTextureUnitCount = 1;
  if (Has_GL_ARB_multitexture||Has_GL_Version_1_3||Has_GLES_Version_1_x) // for GL < 2.x
  {
    int max_tmp = 0;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_tmp); VL_CHECK_OGL(); // deprecated enum
    mTextureUnitCount = max_tmp > mTextureUnitCount ? max_tmp : mTextureUnitCount;
  }
  if (Has_GL_Version_2_0) // for GL == 2.x
  {
    int max_tmp = 0;
    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_tmp); VL_CHECK_OGL(); // deprecated enum
    mTextureUnitCount = max_tmp > mTextureUnitCount ? max_tmp : mTextureUnitCount;
  }
  if (Has_GLSL) // for GL >= 2.0
  {
    int max_tmp = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tmp); VL_CHECK_OGL();
    mTextureUnitCount = max_tmp > mTextureUnitCount ? max_tmp : mTextureUnitCount;
  }
  mTextureUnitCount = mTextureUnitCount < VL_MAX_TEXTURE_UNITS ? mTextureUnitCount : VL_MAX_TEXTURE_UNITS;

  // find max number of vertex attributes
  mMaxVertexAttrib = 0;
  if(Has_GLSL)
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &mMaxVertexAttrib);
  mMaxVertexAttrib = mMaxVertexAttrib < VL_MAX_GENERIC_VERTEX_ATTRIB ? mMaxVertexAttrib : VL_MAX_GENERIC_VERTEX_ATTRIB;

  VL_CHECK_OGL();

#if defined(VL_OPENGL)
  // test for double buffer availability
  glDrawBuffer(GL_BACK);
  if ( glGetError() )
    mHasDoubleBuffer = false;
  else
    mHasDoubleBuffer = true;
#else
  mHasDoubleBuffer = true;
#endif

  setupDefaultRenderStates();

  return true;
}
//-----------------------------------------------------------------------------
bool OpenGLContext::isExtensionSupported(const char* ext_name)
{
  makeCurrent();
  size_t len = strlen(ext_name);
  const char* ext = mExtensions.c_str();
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
  return getGLProcAddress((const GLubyte*)function_name);
}
//-----------------------------------------------------------------------------
void OpenGLContext::logOpenGLInfo()
{
  makeCurrent();

  if (globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_NORMAL)
  {
    Log::print(" --- OpenGL Info ---\n");
    Log::print( Say("OpenGL version: %s\n") << glGetString(GL_VERSION) );
    Log::print( Say("OpenGL vendor: %s\n") << glGetString(GL_VENDOR) );
    Log::print( Say("OpenGL renderer: %s\n") << glGetString(GL_RENDERER) );
    Log::print( Say("OpenGL profile: %s\n") << (Has_GL_Compatibility ? "Compatible" : "Core") );

    if (Has_GLSL)
      Log::print( Say("GLSL version: %s\n") << glGetString(GL_SHADING_LANGUAGE_VERSION) );
    
    int max_val = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_val);
    Log::print( Say("Max texture size: %n\n")<<max_val);

    max_val = 1;
    if (Has_GL_ARB_multitexture||Has_GL_Version_1_3||Has_GLES_Version_1_x)
      glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_val); // deprecated enum
    Log::print( Say("Texture units (legacy): %n\n") << max_val);

    max_val = 0;
    if (Has_GL_Version_2_0)
    {
      glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_val); // deprecated enum
      Log::print( Say("Texture units (client): %n\n") << max_val);
    }
    if (Has_GLSL)
    {
      int tmp = 0;
      glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmp);
      // max between GL_MAX_TEXTURE_COORDS and GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS
      max_val = tmp > max_val ? tmp : max_val;
      Log::print( Say("Texture units (combined): %n\n") << max_val);
    }

    max_val = 0;
    if (Has_GLSL)
    {
      glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_val);
      Log::print( Say("Texture units (fragment shader): %n\n") << max_val);
    }

    max_val = 0;
    if (Has_GL_EXT_texture_filter_anisotropic)
      glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_val);
    Log::print( Say("Anisotropic texture filter: %s, ") << (Has_GL_EXT_texture_filter_anisotropic? "YES" : "NO") );
    Has_GL_EXT_texture_filter_anisotropic ? Log::print( Say("%nX\n") << max_val) : Log::print("\n");
    Log::print( Say("S3 Texture Compression: %s\n") << (Has_GL_EXT_texture_compression_s3tc? "YES" : "NO") );
    Log::print( Say("Vertex Buffer Object: %s\n") << (Has_VBO? "YES" : "NO"));
    Log::print( Say("Pixel Buffer Object: %s\n") << (Has_PBO ? "YES" : "NO"));
    Log::print( Say("Framebuffer Object: %s\n") << (Has_FBO? "YES" : "NO"));

    max_val = 0;
    if(Has_GLSL)
    {
      glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_val); VL_CHECK_OGL();
      Log::print( Say("Max vertex attributes: %n\n")<<max_val);      
    }

    max_val = 0;
    if(Has_GLSL)
    {
      glGetIntegerv(GL_MAX_VARYING_FLOATS, &max_val); VL_CHECK_OGL();
      Log::print( Say("Max varying floats: %n\n")<<max_val);
    }

    max_val = 0;
    if(Has_GLSL)
    {
      glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &max_val); VL_CHECK_OGL();
      Log::print( Say("Max fragment uniform components: %n\n")<<max_val);
    }
    
    max_val = 0;
    if(Has_GLSL)
    {
      glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max_val); VL_CHECK_OGL();
      Log::print( Say("Max vertex uniform components: %n\n")<<max_val);
    }
    
    max_val = 0;
    if(Has_GL_Version_1_2||Has_GL_Version_3_0||Has_GL_Version_4_0)
    {
      glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_val); VL_CHECK_OGL();
      Log::print( Say("Max elements vertices: %n\n") << max_val );
    }

    max_val = 0;
    if(Has_GL_Version_1_2||Has_GL_Version_3_0||Has_GL_Version_4_0)
    {
      glGetIntegerv(GL_MAX_ELEMENTS_INDICES,  &max_val ); VL_CHECK_OGL();
      Log::print( Say("Max elements indices: %n\n") << max_val );
    }

    max_val = 0;
    glGetIntegerv(GL_MAX_CLIP_PLANES,  &max_val ); VL_CHECK_OGL();
    Log::print( Say("Max clipping planes: %n\n") << max_val );

    // --- log supported extensions on two columns ---

    Log::print("\n --- OpenGL Extensions --- \n");

    std::stringstream sstream;
    sstream << extensions();
    std::string ext, line;
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
    GL_PROGRAM_POINT_SIZE,
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
    "EN_PROGRAM_POINT_SIZE",
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
      const EEnable& cur_en = cur->enables()[i];
      mEnableTable[cur_en] += 1; // 0 -> 1; 1 -> 2;
      if ( !mCurrentEnable[cur_en] )
      {
        glEnable( TranslateEnable[cur_en] );
        mCurrentEnable[ cur_en ] = true;
  #ifndef NDEBUG
        if (glGetError() != GL_NO_ERROR)
        {
          Log::error( Say("An unsupported function has been enabled: %s.\n") << TranslateEnableString[cur_en]);
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
      const EEnable& prev_en = prev->enables()[i];
      VL_CHECK(mEnableTable[prev_en] == 1 || mEnableTable[prev_en] == 2);
      if ( mEnableTable[prev_en] == 1 )
      {
        mCurrentEnable[prev_en] = false;
        glDisable( TranslateEnable[prev_en] ); VL_CHECK_OGL()
        #ifndef NDEBUG
          if (glGetError() != GL_NO_ERROR)
          {
            Log::error( Say("An unsupported enum has been disabled: %s.\n") << TranslateEnableString[prev_en]);
            VL_TRAP()
          }
        #endif
      }
      mEnableTable[prev_en] >>= 1; // 1 -> 0; 2 -> 1
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
  VL_CHECK_OGL()

  if (prev == NULL)
    memset( mRenderStateTable, 0, sizeof(mRenderStateTable) );

  /* iterate on current */

  if (cur)
  {
    for( unsigned i=0; i<cur->renderStates().size(); ++i )
    {
      const RenderState* cur_rs = cur->renderStates()[i].get();
      mRenderStateTable[cur_rs->type()] += 1; // 0 -> 1; 1 -> 2;
      if ( mCurrentRenderState[cur_rs->type()] != cur_rs )
      {
        mCurrentRenderState[cur_rs->type()] = cur_rs;
        VL_CHECK(cur_rs);
        cur_rs->apply(camera, this); VL_CHECK_OGL()
      }
    }
  }

  /* iterate on prev: reset to default only the unused ones */

  if (prev)
  {
    for( unsigned i=0; i<prev->renderStates().size(); ++i )
    {
      const RenderState* prev_rs = prev->renderStates()[i].get();
      VL_CHECK(mRenderStateTable[prev_rs->type()] == 1 || mRenderStateTable[prev_rs->type()] == 2);
      if ( mRenderStateTable[prev_rs->type()] == 1 )
      {
        mCurrentRenderState[prev_rs->type()] = mDefaultRenderStates[prev_rs->type()].get();
        #ifndef NDEBUG
        if (!mDefaultRenderStates[prev_rs->type()])
        {
          vl::Log::error( Say("Render state type '%s' not supported by the current OpenGL implementation! (version=%s, vendor=%s)\n") << prev_rs->className() << glGetString(GL_VERSION) << glGetString(GL_VENDOR) );
          VL_TRAP()
        }
        #endif
        // if this fails you are using a render state that is not supported by the current OpenGL implementation (too old or Core profile)
        mDefaultRenderStates[prev_rs->type()]->apply(NULL, this); VL_CHECK_OGL()
      }
      mRenderStateTable[prev_rs->type()] >>= 1; // 1 -> 0; 2 -> 1;
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
  if ( Has_GL_Compatibility )
  {
    mDefaultRenderStates[RS_AlphaFunc] = new AlphaFunc;
    mDefaultRenderStates[RS_Fog] = new Fog;
    mDefaultRenderStates[RS_LightModel] = new LightModel;
    mDefaultRenderStates[RS_Material] = new Material;
    if(!Has_GLES_Version_1_x)
    {
      mDefaultRenderStates[RS_PixelTransfer] = new PixelTransfer;
      mDefaultRenderStates[RS_LineStipple] = new LineStipple;
      mDefaultRenderStates[RS_PolygonStipple] = new PolygonStipple;
    }

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
  }

  if (Has_GL_EXT_blend_color||Has_GL_Version_1_4||Has_GL_Version_3_0||Has_GL_Version_4_0)
    mDefaultRenderStates[RS_BlendColor] = new BlendColor;

  if (Has_GL_Version_1_4||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GL_OES_blend_subtract)
    mDefaultRenderStates[RS_BlendEquation] = new BlendEquation;

  mDefaultRenderStates[RS_BlendFunc] = new BlendFunc;
  mDefaultRenderStates[RS_ColorMask] = new ColorMask;
  mDefaultRenderStates[RS_CullFace] = new CullFace;
  mDefaultRenderStates[RS_DepthFunc] = new DepthFunc;
  mDefaultRenderStates[RS_DepthMask] = new DepthMask;
  mDefaultRenderStates[RS_DepthRange] = new DepthRange;
  mDefaultRenderStates[RS_FrontFace] = new FrontFace;
  if(!Has_GLES_Version_1_x)
    mDefaultRenderStates[RS_PolygonMode] = new PolygonMode;
  mDefaultRenderStates[RS_Hint] = new Hint;
  mDefaultRenderStates[RS_LineWidth] = new LineWidth;
  mDefaultRenderStates[RS_LogicOp] = new LogicOp;
  
  if (Has_GL_ARB_point_parameters||Has_GL_Version_1_4||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GLES_Version_1_x) // note GLES 2.x is excluded
    mDefaultRenderStates[RS_PointParameter] = new PointParameter;

  mDefaultRenderStates[RS_PointSize] = new PointSize;
  mDefaultRenderStates[RS_PolygonOffset] = new PolygonOffset;
  
  if (Has_GL_ARB_multisample||Has_GL_Version_1_3||Has_GL_Version_3_0||Has_GL_Version_4_0||Has_GLES_Version_1_x||Has_GLES_Version_2_x)
    mDefaultRenderStates[RS_SampleCoverage] = new SampleCoverage;
  
  mDefaultRenderStates[RS_ShadeModel] = new ShadeModel;
  mDefaultRenderStates[RS_StencilFunc] = new StencilFunc;
  mDefaultRenderStates[RS_StencilMask] = new StencilMask;
  mDefaultRenderStates[RS_StencilOp] = new StencilOp;
  mDefaultRenderStates[RS_GLSLProgram] = new GLSLProgram;

  for(int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
  {
    if (i < textureUnitCount())
    {
      mDefaultRenderStates[RS_TextureUnit0   + i] = new TextureUnit(i);
      if( Has_GL_Compatibility )
      {
        // TexGen under GLES is supported only if GL_OES_texture_cube_map is present
        if(!Has_GLES_Version_1_x || Has_GL_OES_texture_cube_map)
          mDefaultRenderStates[RS_TexGen0 + i] = new TexGen(i);
        mDefaultRenderStates[RS_TexEnv0 + i] = new TexEnv(i);
        mDefaultRenderStates[RS_TextureMatrix0 + i] = new TextureMatrix(i);
      }
    }
  }

  VL_CHECK_OGL();

  // applies default render states backwards so we don't need to call VL_glActiveTexture(GL_TEXTURE0) at the end.
  for( int i=RS_COUNT; i--; )
  {
    // the empty ones are the ones that are not supported by the current OpenGL implementation (too old or Core profile)
    if (mDefaultRenderStates[i])
    {
      mDefaultRenderStates[i]->apply(NULL, this); VL_CHECK_OGL();
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
bool OpenGLContext::isCleanState(bool verbose)
{
  struct contract {
    contract()  { VL_CHECK_OGL(); }
    ~contract() { VL_CHECK_OGL(); }
  } contract_instance;

  bool ok = true;

  // everything must be disabled
  for( unsigned i=0; i<EN_EnableCount; ++i )
  {
    // These are enabled in Core profiles
    if (!Has_GL_Compatibility)
    {
      // "Non-sprite points - Enable/Disable targets POINT_SMOOTH and POINT_-
      // SPRITE, and all associated state. Point rasterization is always performed as
      // though POINT_SPRITE were enabled." OpenGL 3.3 (Core Profile) p344
      if (TranslateEnable[i] == GL_POINT_SPRITE) // this is actually deprecated
        continue;

      // "If enabled and a vertex or geometry shader is active, then the derived point 
      // size is taken from the (potentially clipped) shader builtin gl_PointSize and 
      // clamped to the implementation-dependent point size range."
      // See http://www.opengl.org/sdk/docs/man3/xhtml/glEnable.xml
      if (TranslateEnable[i] == GL_PROGRAM_POINT_SIZE)
        continue;
    }

    // mic fixme: check GLES1 extensions, check GLES2 separately, check GLES2 extensions
#if defined(VL_OPENGL_ES1)
      // skip unsupported enables
      switch(TranslateEnable[i])
      {
      case GL_COLOR_SUM:
      case GL_LINE_STIPPLE:
      case GL_POLYGON_STIPPLE:
      case GL_POLYGON_SMOOTH:
      case GL_POLYGON_OFFSET_LINE:
      case GL_POLYGON_OFFSET_POINT:
      case GL_PROGRAM_POINT_SIZE:
      case GL_VERTEX_PROGRAM_TWO_SIDE:
      case GL_TEXTURE_CUBE_MAP_SEAMLESS:
        continue;
      }
#endif

    // Note: for simplicity we allow GL to generate errors for unsupported enables.
    GLboolean enabled = glIsEnabled( TranslateEnable[i] ); /* VL_CHECK_OGL(); */

    // mic fixme
    if(glGetError() != GL_NO_ERROR)
    {
      vl::Log::error( Say("Capability %s not supported!\n") << TranslateEnableString[i] );
      VL_TRAP();
    }

    // The order is important!
    if (glGetError() == GL_NO_ERROR && enabled)
    {
	    if (verbose)
		    vl::Log::error( Say("Capability %s was enabled!\n") << TranslateEnableString[i] );
      VL_TRAP();
      ok = false;
    }
  }

  if (Has_GL_Compatibility)
  {
    int max_lights = 0;
    glGetIntegerv(GL_MAX_LIGHTS, &max_lights);

    for( int i=0; i<max_lights; ++i)
    {
      if (glIsEnabled(GL_LIGHT0+i))
      {
        if (verbose)
          vl::Log::error( Say("GL_LIGHT%n was enabled!\n") << i );
        VL_TRAP();
        ok = false;
      }
    }

    // OpenGL requires 6 planes but GLES 1.x requires only 1.
    int max_planes = 0;
    glGetIntegerv(GL_MAX_CLIP_PLANES, &max_planes);

    for( int i=0; i<max_planes; ++i)
    {
      if (glIsEnabled(GL_CLIP_PLANE0+i))
      {
        if (verbose)
          vl::Log::error( Say("GL_CLIP_PLANE%n was enabled!\n") << i );
        VL_TRAP();
        ok = false;
      }
    }
  }

  if (Has_Primitive_Restart && glIsEnabled(GL_PRIMITIVE_RESTART))
  {
    if (verbose)
      vl::Log::error( "GL_PRIMITIVE_RESTART was enabled!\n" );
    VL_TRAP();
    ok = false;
  }

  if(Has_Multitexture)
  {
    int active_tex = -1;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_tex); VL_CHECK_OGL();
    active_tex -= GL_TEXTURE0;
    if (active_tex != 0)
    {
      if (verbose)
        vl::Log::error( Say("Active texture unit is #%n instead of #0!\n") << active_tex );
      VL_TRAP();
      ok = false;
    }

    if (Has_GL_Compatibility)
    {
      active_tex = -1;
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
  }

  VL_CHECK_OGL()

  /* We only check the subset of tex-units supported also by glClientActiveTexture() */
  // Find the minimum of the max texture units supported, starting at 16
  int coord_count = 16;
  if (Has_GL_ARB_multitexture||Has_GL_Version_1_3||Has_GLES_Version_1_x)
  {
    int max_tmp = 0;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_tmp); VL_CHECK_OGL(); // deprecated enum
    coord_count = max_tmp < coord_count ? max_tmp : coord_count;
  }

  VL_CHECK_OGL()

  if (Has_GLSL) // for GL >= 2.0
  {
    int max_tmp = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tmp); VL_CHECK_OGL();
    coord_count = max_tmp < coord_count ? max_tmp : coord_count;
  }

  if (Has_GL_Version_2_0) // for GL == 2.x && compatible higher
  {
    int max_tmp = 0;
    glGetIntegerv(GL_MAX_TEXTURE_COORDS, &max_tmp); VL_CHECK_OGL(); // deprecated enum
    coord_count = max_tmp < coord_count ? max_tmp : coord_count;
  }

  VL_CHECK_OGL()

  while(coord_count--)
  {
    VL_CHECK_OGL()
    VL_glActiveTexture(GL_TEXTURE0+coord_count); VL_CHECK_OGL()

    if (Has_GL_Compatibility)
    {
	    VL_glClientActiveTexture(GL_TEXTURE0+coord_count); VL_CHECK_OGL()

	    float matrix[16];
	    float imatrix[16];
	    glGetFloatv(GL_TEXTURE_MATRIX, matrix); VL_CHECK_OGL()
	    glMatrixMode(GL_TEXTURE); VL_CHECK_OGL()
	    glLoadIdentity(); VL_CHECK_OGL()
	    glGetFloatv(GL_TEXTURE_MATRIX, imatrix); VL_CHECK_OGL()
	    glLoadMatrixf(matrix); VL_CHECK_OGL()
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
	
      if (!Has_GLES)
      {
	      if (glIsEnabled(GL_TEXTURE_1D))
	      {
	        if (verbose)
	          vl::Log::error( Say("GL_TEXTURE_1D was enabled on texture unit #%n!\n") << coord_count );
	        VL_TRAP();
	        ok = false;
	      }
	
        GLint bound_tex = 0;
	      glGetIntegerv(GL_TEXTURE_BINDING_1D, &bound_tex); VL_CHECK_OGL()
	      if (bound_tex != 0)
	      {
	        if (verbose)
	          vl::Log::error( Say("GL_TEXTURE_BINDING_1D != 0 on texture unit #%n!\n") << coord_count );
	        VL_TRAP();
	        ok = false;
	      }
      }
	
	    if (glIsEnabled(GL_TEXTURE_2D))
	    {
	      if (verbose)
	        vl::Log::error( Say("GL_TEXTURE_2D was enabled on texture unit #%n!\n") << coord_count );
	      VL_TRAP();
	      ok = false;
	    }
    }

    GLint bound_tex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_tex); VL_CHECK_OGL()
    if (bound_tex != 0)
    {
      if (verbose)
        vl::Log::error( Say("GL_TEXTURE_BINDING_2D != 0 on texture unit #%n!\n") << coord_count );
      VL_TRAP();
      ok = false;
    }

    if (Has_Texture_Rectangle)
    {
      if (Has_GL_Compatibility && glIsEnabled(GL_TEXTURE_RECTANGLE))
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_RECTANGLE was enabled on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0;
      glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE, &bound_tex); VL_CHECK_OGL()
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_RECTANGLE != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (Has_Texture_3D)
    {
      if (Has_GL_Compatibility && glIsEnabled(GL_TEXTURE_3D))
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_3D was enabled on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0;
      glGetIntegerv(GL_TEXTURE_BINDING_3D, &bound_tex); VL_CHECK_OGL()
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_3D != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (Has_Cubemap_Textures)
    {
      if (Has_GL_Compatibility && glIsEnabled(GL_TEXTURE_CUBE_MAP))
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_CUBE_MAP was enabled on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }

      bound_tex = 0;
      glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &bound_tex); VL_CHECK_OGL()
      if (bound_tex != 0)
      {
        if (verbose)
          vl::Log::error( Say("GL_TEXTURE_BINDING_CUBE_MAP != 0 on texture unit #%n!\n") << coord_count );
        VL_TRAP();
        ok = false;
      }
    }

    if (Has_Texture_Array)
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

    if (Has_Texture_Multisample)
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

    if (Has_Texture_Buffer)
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

    if (Has_GL_Compatibility)
    {
#if defined(VL_OPENGL)
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
#elif defined(VL_OPENGL_ES1)
	    if (Has_GL_OES_texture_cube_map && glIsEnabled(GL_TEXTURE_GEN_STR_OES))
	    {
	      if (verbose)
	        vl::Log::error( "GL_TEXTURE_GEN_STR_OES was enabled!\n" );
	      VL_TRAP();
	      ok = false;
	    }
#endif
    }
  }

  if (Has_GL_Version_1_1 && glIsEnabled(GL_COLOR_MATERIAL)) // excludes also GLES
  {
    if (verbose)
      vl::Log::error( "GL_COLOR_MATERIAL was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (Has_GL_Version_1_4 || Has_GL_EXT_fog_coord) // excludes also GLES 1.x
  {
    if (glIsEnabled(GL_FOG_COORD_ARRAY))
    {
      if (verbose)
        vl::Log::error( "GL_FOG_COORD_ARRAY was enabled!\n");
      VL_TRAP();
      ok = false;
    }
  }

  if (Has_GL_Version_1_4 || Has_GL_EXT_secondary_color) // excludes also GLES 1.x
  {
    if (glIsEnabled(GL_SECONDARY_COLOR_ARRAY))
    {
      if (verbose)
        vl::Log::error( "GL_SECONDARY_COLOR_ARRAY was enabled!\n");
      VL_TRAP();
      ok = false;
    }
  }

  if (Has_GL_Compatibility && glIsEnabled(GL_COLOR_ARRAY)) // includes GLES 1.x
  {
    if (verbose)
      vl::Log::error( "GL_COLOR_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (Has_GL_Version_1_1 && glIsEnabled(GL_EDGE_FLAG_ARRAY)) // excludes GLES 
  {
    if (verbose)
      vl::Log::error( "GL_EDGE_FLAG_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (Has_GL_Version_1_1 && glIsEnabled(GL_INDEX_ARRAY)) // excludes GLES
  {
    if (verbose)
      vl::Log::error( "GL_INDEX_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (Has_GL_Compatibility && glIsEnabled(GL_NORMAL_ARRAY)) // includes GLES 1.x
  {
    if (verbose)
      vl::Log::error( "GL_NORMAL_ARRAY was enabled!\n");
    VL_TRAP();
    ok = false;
  }

  if (Has_GL_Compatibility && glIsEnabled(GL_VERTEX_ARRAY)) // includes GLES 1.x
  {
    if (verbose)
      vl::Log::error( "GL_VERTEX_ARRAY was enabled!\n");
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

  GLint max_vert_attribs = 0;
  if (Has_GLSL)
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

  if (Has_GL_ARB_imaging)
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
  GLint blend_src = 0;
  GLint blend_dst = 0;
  glGetIntegerv( GL_BLEND_SRC, &blend_src ); VL_CHECK_OGL();
  glGetIntegerv( GL_BLEND_DST, &blend_dst ); VL_CHECK_OGL();
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
  if (Has_VBO)
  {
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &buf_bind); VL_CHECK_OGL();
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_ARRAY_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
    buf_bind = 0;
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &buf_bind); VL_CHECK_OGL();
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_ELEMENT_ARRAY_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  if (Has_GL_Version_2_1)
  {
    buf_bind = 0;
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &buf_bind); VL_CHECK_OGL();
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_PIXEL_PACK_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
    buf_bind = 0;
    glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &buf_bind); VL_CHECK_OGL();
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_PIXEL_UNPACK_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  if (Has_GL_ARB_uniform_buffer_object)
  {
    buf_bind = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &buf_bind); VL_CHECK_OGL();
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_UNIFORM_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }
  if(Has_Transform_Feedback)
  {
    buf_bind = 0;
    glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &buf_bind); VL_CHECK_OGL();
    if (buf_bind != 0)
    {
      if (verbose)
        vl::Log::error( "GL_TRANSFORM_FEEDBACK_BUFFER_BINDING should be 0!\n");
      VL_TRAP();
      ok = false;
    }
  }

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
  glGetBooleanv(GL_COLOR_WRITEMASK, write_mask); VL_CHECK_OGL();
  if( !write_mask[0] || !write_mask[1] || !write_mask[2] || !write_mask[3] )
  {
    vl::Log::error( "Color write-mask should be glColorMask(GL_TRUE ,GL_TRUE, GL_TRUE, GL_TRUE)!\n" );
    ok = false;
  }

  glGetBooleanv(GL_DEPTH_WRITEMASK, write_mask); VL_CHECK_OGL();
  if ( !write_mask[0] )
  {
    vl::Log::error( "Depth write-mask should be glDepthMask(GL_TRUE)!\n" );
    ok = false;
  }

#if defined(VL_OPENGL)
  GLint poly_mode[2];
  glGetIntegerv(GL_POLYGON_MODE, poly_mode); VL_CHECK_OGL();
  if ( poly_mode[0] != GL_FILL || poly_mode[1] != GL_FILL )
  {
    vl::Log::error( "Polygon mode should be glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)!\n" );
    ok = false;
  }
#endif

  VL_CHECK_OGL();
  return ok;
}
//-----------------------------------------------------------------------------
bool OpenGLContext::areUniformsColliding(const UniformSet* u1, const UniformSet* u2)
{
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

  // not existing under OpenGL ES 1 and 2
#if defined(VL_OPENGL)
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
#endif

  // reset internal VL enables & render states tables
  resetEnables();      VL_CHECK_OGL()
  resetRenderStates(); VL_CHECK_OGL()

  // reset Vertex Attrib Set tables
  bindVAS(NULL, false, true); VL_CHECK_OGL();
}
//-----------------------------------------------------------------------------
void OpenGLContext::bindVAS(const IVertexAttribSet* vas, bool use_vbo, bool force)
{
  VL_CHECK_OGL();

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
        VL_glDisableVertexAttribArray(i); VL_CHECK_OGL();

      // note this one
      VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); VL_CHECK_OGL();

      VL_glBindBuffer(GL_ARRAY_BUFFER, 0); VL_CHECK_OGL();

      if(Has_GL_Compatibility)
      {
        // iterate backwards so the last active is #0
        for ( int i=mTextureUnitCount; i--; )
        {
          VL_glClientActiveTexture(GL_TEXTURE0 + i); VL_CHECK_OGL();
          glDisableClientState(GL_TEXTURE_COORD_ARRAY); VL_CHECK_OGL();
        }

        glDisableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
        glDisableClientState(GL_NORMAL_ARRAY); VL_CHECK_OGL();
        glDisableClientState(GL_COLOR_ARRAY); VL_CHECK_OGL();

        // not supported under GLES
#if defined(VL_OPENG)
        glDisableClientState(GL_SECONDARY_COLOR_ARRAY); VL_CHECK_OGL();
        glDisableClientState(GL_FOG_COORD_ARRAY); VL_CHECK_OGL();
#endif
      }
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
#if !defined(NDEBUG)
          if ( Has_GLES_Version_1_x && texarr->glSize() == 1)
          {
            Log::error("OpenGL ES does not allow 1D texture coordinates.\n"); VL_TRAP();
          }
#endif
          glTexCoordPointer((int)texarr->glSize(), texarr->glType(), 0/*texarr->stride()*/, ptr/*+ texarr->offset()*/); VL_CHECK_OGL();

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

      VL_glBindBuffer(GL_ARRAY_BUFFER, 0); VL_CHECK_OGL();
    }
  }

  mCurVAS = vas;

  VL_CHECK_OGL();
}
//-----------------------------------------------------------------------------
