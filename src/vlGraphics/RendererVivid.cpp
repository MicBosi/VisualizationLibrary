/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
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

#include <vlCore/GlobalSettings.hpp>
#include <vlGraphics/RendererVivid.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/RenderQueue.hpp>
#include <vlCore/Log.hpp>

#define SHADER_PATH "/depth-peeling/glsl/"

#if 1
#define CHECK_GL_ERRORS  \
{ \
    GLenum err = glGetError(); \
    if (err) \
        printf( "Error %x at line %d\n", err, __LINE__); \
}
#else
#define CHECK_GL_ERRORS {}
#endif

vl::ref<vl::GLSLProgram> g_shaderDualInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderDualPeel = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderDualBlend = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderDualFinal = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontPeel = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontBlend = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontFinal = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderAverageInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderAverageFinal = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderWeightedSumInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderWeightedSumFinal = new vl::GLSLProgram();

vl::ref<vl::Uniform> g_uniformAlpha = new vl::Uniform("Alpha");
vl::ref<vl::Uniform> g_uniformBackgroundColor = new vl::Uniform("BackgroundColor");

enum {
	DUAL_PEELING_MODE,
	F2B_PEELING_MODE,
	WEIGHTED_AVERAGE_MODE,
	WEIGHTED_SUM_MODE
};

GLuint g_quadDisplayList;

#define MAX_DEPTH 1.0

int g_numPasses = 4;
int g_imageWidth = 1024;
int g_imageHeight = 768;

bool g_useOQ = true;
GLuint g_queryId;

float g_opacity = 0.6f;
char g_mode = DUAL_PEELING_MODE;
bool g_showOsd = true;
bool g_bShowUI = true;
unsigned g_numGeoPasses = 0;

int g_rotating = 0;
int g_panning = 0;
int g_scaling = 0;
int g_oldX, g_oldY;
int g_newX, g_newY;
float g_bbScale = 1.0;
vl::fvec3 g_bbTrans(0.0, 0.0, 0.0);
vl::fvec2 g_rot(0.0, 45.0);
vl::fvec3 g_pos(0.0, 0.0, 2.0);

float g_white[3] = {1.0,1.0,1.0};
float g_black[3] = {0.0};
float *g_backgroundColor = g_white;

GLuint g_dualBackBlenderFboId;
GLuint g_dualPeelingSingleFboId;
GLuint g_dualDepthTexId[2];
GLuint g_dualFrontBlenderTexId[2];
GLuint g_dualBackTempTexId[2];
GLuint g_dualBackBlenderTexId;

GLuint g_frontFboId[2];
GLuint g_frontDepthTexId[2];
GLuint g_frontColorTexId[2];
GLuint g_frontColorBlenderTexId;
GLuint g_frontColorBlenderFboId;

GLuint g_accumulationTexId[2];
GLuint g_accumulationFboId;

GLenum g_drawBuffers[] = {
  GL_COLOR_ATTACHMENT0,
  GL_COLOR_ATTACHMENT1,
  GL_COLOR_ATTACHMENT2,
  GL_COLOR_ATTACHMENT3,
  GL_COLOR_ATTACHMENT4,
  GL_COLOR_ATTACHMENT5,
  GL_COLOR_ATTACHMENT6
};

using namespace vl;

//------------------------------------------------------------------------------
// RendererVivid
//------------------------------------------------------------------------------
RendererVivid::RendererVivid()
{
  mInitSize = ivec2(0, 0);
}
//------------------------------------------------------------------------------
void RendererVivid::lazyInitialize()
{
  OpenGLContext* opengl_context = framebuffer()->openglContext();
  ivec2 fb_size = ivec2(framebuffer()->width(), framebuffer()->height());
  if (mInitSize == ivec2(0, 0)) {
    // MIC FIXME: remove and use mInitSize
    mInitSize = fb_size;
    g_imageWidth = fb_size.x();
	  g_imageHeight = fb_size.y();
    InitDualPeelingRenderTargets();
    InitFrontPeelingRenderTargets();
    vl::glBindFramebuffer(GL_FRAMEBUFFER, 0); // MIC FIXME: remove

    BuildShaders();
    MakeFullScreenQuad();
    vl::glGenQueries(1, &g_queryId);
  } else
  if (mInitSize != fb_size) {
    mInitSize = fb_size;
    g_imageWidth = fb_size.x();
	  g_imageHeight = fb_size.y();

	  DeleteDualPeelingRenderTargets();
	  InitDualPeelingRenderTargets();

	  DeleteFrontPeelingRenderTargets();
	  InitFrontPeelingRenderTargets();
  }
}
//------------------------------------------------------------------------------
namespace
{
  struct GLSLProgState
  {
  public:
    GLSLProgState(): mCamera(NULL), mTransform(NULL), mGLSLProgUniformSet(NULL), mShaderUniformSet(NULL), mActorUniformSet(NULL) {}

    bool operator<(const GLSLProgState& other) const
    {
      if ( mCamera != other.mCamera )
        return mCamera < other.mCamera;
      else
      if ( mTransform != other.mTransform )
        return mTransform < other.mTransform;
      else
      if ( mGLSLProgUniformSet != other.mGLSLProgUniformSet )
        return mGLSLProgUniformSet < other.mGLSLProgUniformSet;
      else
      if ( mShaderUniformSet != other.mShaderUniformSet ) 
        return mShaderUniformSet < other.mShaderUniformSet;
      else
        return mActorUniformSet < other.mActorUniformSet;
    }

    const Camera* mCamera;
    const Transform* mTransform;
    const UniformSet* mGLSLProgUniformSet;
    const UniformSet* mShaderUniformSet;
    const UniformSet* mActorUniformSet;
  };
}
//------------------------------------------------------------------------------
void RendererVivid::renderQueue(const RenderQueue* render_queue, Camera* camera, real frame_clock, bool depth_peeling_on) {
  
  OpenGLContext* opengl_context = framebuffer()->openglContext();

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok = render_queue->at(itok); VL_CHECK(tok);
    Actor* actor = tok->mActor; VL_CHECK(actor);

    if ( !isEnabled(actor->enableMask()) )
      continue;

    VL_CHECK_OGL()

    // --------------- shader setup ---------------

    const Shader* shader = tok->mShader;

    shader->getMaterial()->apply(0, NULL, NULL); VL_CHECK_OGL()
    shader->getLight(0)->apply(0, camera, NULL); VL_CHECK_OGL()
    shader->getLightModel()->apply(0, NULL, NULL);

    /* These two states are managed by the Depth Peeling algorithm */
    if (!depth_peeling_on) {
      shader->isEnabled(vl::EN_BLEND) ? glEnable(GL_BLEND) : glDisable(GL_BLEND); VL_CHECK_OGL()
      // glDepthMask(shader->isEnabled(vl::EN_BLEND) ? GL_FALSE : GL_TRUE);
      shader->isEnabled(vl::EN_BLEND) ? glEnable(GL_BLEND) : glDisable(GL_BLEND); VL_CHECK_OGL()
      shader->isEnabled(vl::EN_DEPTH_TEST) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST); VL_CHECK_OGL()
      shader->isEnabled(vl::EN_LIGHTING) ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING); VL_CHECK_OGL()
    }

    // --------------- Actor pre-render callback ---------------

    // here the user has still the possibility to modify the Actor's uniforms

    actor->dispatchOnActorRenderStarted( frame_clock, camera, tok->mRenderable, shader, /*ipass*/0 );

    VL_CHECK_OGL()

    // --------------- Transform setup ---------------

    // Update camera/actor model-view transform
    projViewTransfCallback()->updateMatrices( /*update_cm*/false, /*update_tr*/true, 0, camera, actor->transform() );

    VL_CHECK_OGL()

    // --------------- Actor rendering ---------------

    // also compiles display lists and updates BufferObjects if necessary
    tok->mRenderable->render( actor, shader, camera, opengl_context );

    VL_CHECK_OGL()
  }
}
//------------------------------------------------------------------------------
const RenderQueue* RendererVivid::render(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  VL_CHECK_OGL()

  lazyInitialize();

  // skip if renderer is disabled

  if (enableMask() == 0)
    return render_queue;

  // enter/exit behavior contract

  class InOutContract 
  {
    RendererVivid* mRenderer;
    std::vector<RenderStateSlot> mOriginalDefaultRS;
  public:
    InOutContract(RendererVivid* renderer, Camera* camera): mRenderer(renderer)
    {
      // increment the render tick.
      mRenderer->mRenderTick++;

      // render-target activation.
      // note: an OpenGL context can have multiple rendering targets!
      mRenderer->framebuffer()->activate();

      // viewport setup.
      camera->viewport()->setClearFlags( mRenderer->clearFlags() );
      camera->viewport()->activate();

      OpenGLContext* gl_context = renderer->framebuffer()->openglContext();

      // default render states override
      for(size_t i=0; i<renderer->overriddenDefaultRenderStates().size(); ++i)
      {
        // save overridden default render state to be restored later
        ERenderState type = renderer->overriddenDefaultRenderStates()[i].type();
        mOriginalDefaultRS.push_back(gl_context->defaultRenderState(type));
        // set new default render state
        gl_context->setDefaultRenderState(renderer->overriddenDefaultRenderStates()[i]);
      }

      // dispatch the renderer-started event.
      mRenderer->dispatchOnRendererStarted();

      // check user-generated errors.
      VL_CHECK_OGL()
    }

    ~InOutContract()
    {
      // dispatch the renderer-finished event
      mRenderer->dispatchOnRendererFinished();

      OpenGLContext* gl_context = mRenderer->framebuffer()->openglContext();

      // restore default render states
      for(size_t i=0; i<mOriginalDefaultRS.size(); ++i)
      {
        gl_context->setDefaultRenderState(mOriginalDefaultRS[i]);
      }

      VL_CHECK( !globalSettings()->checkOpenGLStates() || mRenderer->framebuffer()->openglContext()->isCleanState(true) );

      // check user-generated errors.
      VL_CHECK_OGL()

      // note: we don't reset the render target here
    }
  } contract(this, camera);

  // --------------- rendering --------------- 

  std::map<const GLSLProgram*, GLSLProgState> glslprogram_map;

  OpenGLContext* opengl_context = framebuffer()->openglContext();

  // --------------- rendering ---------------

  // Update camera projection
  projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/false, 0, camera, /*cur_transform*/NULL );

  // Disable scissor test
  glDisable(GL_SCISSOR_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  glDisable(GL_NORMALIZE);
  glDepthMask(GL_TRUE);

  // renderQueue(render_queue, camera, frame_clock, false);
  this->RenderDualPeeling(render_queue, camera, frame_clock);
  // this->RenderFrontToBackPeeling(render_queue, camera, frame_clock);

  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_LIGHTING);
  glDisable(GL_LIGHT0);

  // clear enables
  // opengl_context->applyEnables( mDummyEnables.get() ); VL_CHECK_OGL();

  // clear render states
  opengl_context->applyRenderStates( mDummyStateSet.get(), NULL ); VL_CHECK_OGL();

  // enabled texture unit #0
  VL_glActiveTexture( GL_TEXTURE0 ); VL_CHECK_OGL();
  if (Has_Fixed_Function_Pipeline)
    VL_glClientActiveTexture( GL_TEXTURE0 ); VL_CHECK_OGL();

  // disable scissor test
  glDisable(GL_SCISSOR_TEST); VL_CHECK_OGL();

  // disable all vertex arrays, note this also calls "glBindBuffer(GL_ARRAY_BUFFER, 0)"
  opengl_context->bindVAS(NULL, false, false); VL_CHECK_OGL();

  return render_queue;
}
//-----------------------------------------------------------------------------
void RendererVivid::InitDualPeelingRenderTargets()
{
  glGenTextures(2, g_dualDepthTexId);
  glGenTextures(2, g_dualFrontBlenderTexId);
  glGenTextures(2, g_dualBackTempTexId);
  vl::glGenFramebuffers(1, &g_dualPeelingSingleFboId);
  for (int i = 0; i < 2; i++)
  {
	  glBindTexture(GL_TEXTURE_RECTANGLE, g_dualDepthTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RG32F, g_imageWidth, g_imageHeight, 0, GL_RGB, GL_FLOAT, 0);

	  glBindTexture(GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, g_imageWidth, g_imageHeight, 0, GL_RGBA, GL_FLOAT, 0);

	  glBindTexture(GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, g_imageWidth, g_imageHeight, 0, GL_RGBA, GL_FLOAT, 0);
  }

  glGenTextures(1, &g_dualBackBlenderTexId);
  glBindTexture(GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, g_imageWidth, g_imageHeight, 0, GL_RGB, GL_FLOAT, 0);

  vl::glGenFramebuffers(1, &g_dualBackBlenderFboId);
  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualBackBlenderFboId);
  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId, 0);

  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingSingleFboId);

  int j = 0;
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							    GL_TEXTURE_RECTANGLE, g_dualDepthTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
							    GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
							    GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[j], 0);

  j = 1;
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
							    GL_TEXTURE_RECTANGLE, g_dualDepthTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4,
							    GL_TEXTURE_RECTANGLE, g_dualFrontBlenderTexId[j], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5,
							    GL_TEXTURE_RECTANGLE, g_dualBackTempTexId[j], 0);

  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6,
						    GL_TEXTURE_RECTANGLE, g_dualBackBlenderTexId, 0);

  // Cleanup
  vl::glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  CHECK_GL_ERRORS;
}

//--------------------------------------------------------------------------
void RendererVivid::DeleteDualPeelingRenderTargets()
{
  vl::glDeleteFramebuffers(1, &g_dualBackBlenderFboId);
  vl::glDeleteFramebuffers(1, &g_dualPeelingSingleFboId);
  glDeleteTextures(2, g_dualDepthTexId);
  glDeleteTextures(2, g_dualFrontBlenderTexId);
  glDeleteTextures(2, g_dualBackTempTexId);
  glDeleteTextures(1, &g_dualBackBlenderTexId);
}

//--------------------------------------------------------------------------
void RendererVivid::InitFrontPeelingRenderTargets()
{
  glGenTextures(2, g_frontDepthTexId);
  glGenTextures(2, g_frontColorTexId);
  vl::glGenFramebuffers(2, g_frontFboId);

  for (int i = 0; i < 2; i++)
  {
	  glBindTexture(GL_TEXTURE_RECTANGLE, g_frontDepthTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT32F,
				    g_imageWidth, g_imageHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	  glBindTexture(GL_TEXTURE_RECTANGLE, g_frontColorTexId[i]);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, g_imageWidth, g_imageHeight,
				    0, GL_RGBA, GL_FLOAT, 0);

	  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontFboId[i]);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							    GL_TEXTURE_RECTANGLE, g_frontDepthTexId[i], 0);
	  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							    GL_TEXTURE_RECTANGLE, g_frontColorTexId[i], 0);
  }

  glGenTextures(1, &g_frontColorBlenderTexId);
  glBindTexture(GL_TEXTURE_RECTANGLE, g_frontColorBlenderTexId);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA, g_imageWidth, g_imageHeight,
			    0, GL_RGBA, GL_FLOAT, 0);

  vl::glGenFramebuffers(1, &g_frontColorBlenderFboId);
  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontColorBlenderFboId);
  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						    GL_TEXTURE_RECTANGLE, g_frontDepthTexId[0], 0);
  vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						    GL_TEXTURE_RECTANGLE, g_frontColorBlenderTexId, 0);

  // Cleanup
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);

  CHECK_GL_ERRORS;
}

//--------------------------------------------------------------------------
void RendererVivid::DeleteFrontPeelingRenderTargets()
{
  vl::glDeleteFramebuffers(2, g_frontFboId);
  vl::glDeleteFramebuffers(1, &g_frontColorBlenderFboId);
  glDeleteTextures(2, g_frontDepthTexId);
  glDeleteTextures(2, g_frontColorTexId);
  glDeleteTextures(1, &g_frontColorBlenderTexId);
}

void RendererVivid::BuildShaders()
{
  printf("\nloading shaders...\n");

  g_shaderDualInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_init_vertex.glsl" ) );
  g_shaderDualInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_init_fragment.glsl" ) );
  g_shaderDualInit->linkProgram();

  g_shaderDualPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  g_shaderDualPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_peel_vertex.glsl" ) );
  g_shaderDualPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  g_shaderDualPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_peel_fragment.glsl" ) );
  g_shaderDualPeel->linkProgram();

  g_shaderDualBlend->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_blend_vertex.glsl" ) );
  g_shaderDualBlend->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_blend_fragment.glsl" ) );
  g_shaderDualBlend->linkProgram();

  g_shaderDualFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_final_vertex.glsl" ) );
  g_shaderDualFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_final_fragment.glsl" ) );
  g_shaderDualFinal->linkProgram();

  g_shaderFrontInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  g_shaderFrontInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_init_vertex.glsl" ) );
  g_shaderFrontInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  g_shaderFrontInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_init_fragment.glsl" ) );
  g_shaderFrontInit->linkProgram();

  g_shaderFrontPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  g_shaderFrontPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_peel_vertex.glsl" ) );
  g_shaderFrontPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  g_shaderFrontPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_peel_fragment.glsl" ) );
  g_shaderFrontPeel->linkProgram();

  g_shaderFrontBlend->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_blend_vertex.glsl" ) );
  g_shaderFrontBlend->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_blend_fragment.glsl" ) );
  g_shaderFrontBlend->linkProgram();

  g_shaderFrontFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_final_vertex.glsl" ) );
  g_shaderFrontFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_final_fragment.glsl" ) );
  g_shaderFrontFinal->linkProgram();

  g_shaderAverageInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  g_shaderAverageInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wavg_init_vertex.glsl" ) );
  g_shaderAverageInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  g_shaderAverageInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wavg_init_fragment.glsl" ) );
  g_shaderAverageInit->linkProgram();

  g_shaderAverageFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wavg_final_vertex.glsl" ) );
  g_shaderAverageFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wavg_final_fragment.glsl" ) );
  g_shaderAverageFinal->linkProgram();

  g_shaderWeightedSumInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
  g_shaderWeightedSumInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wsum_init_vertex.glsl" ) );
  g_shaderWeightedSumInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
  g_shaderWeightedSumInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wsum_init_fragment.glsl" ) );
  g_shaderWeightedSumInit->linkProgram();

  g_shaderWeightedSumFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wsum_final_vertex.glsl" ) );
  g_shaderWeightedSumFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wsum_final_fragment.glsl" ) );
  g_shaderWeightedSumFinal->linkProgram();
}
  
void RendererVivid::MakeFullScreenQuad()
{
  g_quadDisplayList = glGenLists(1);
  glNewList(g_quadDisplayList, GL_COMPILE);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0.0, 1.0, 0.0, 1.0);
  glBegin(GL_QUADS);
  {
	  glVertex2f(0.0, 0.0); 
	  glVertex2f(1.0, 0.0);
	  glVertex2f(1.0, 1.0);
	  glVertex2f(0.0, 1.0);
  }
  glEnd();
  glPopMatrix();

  glEndList();
}

void RendererVivid::RenderDualPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);

  // ---------------------------------------------------------------------
  // 1. Initialize Min-Max Depth Buffer
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingSingleFboId);

  // Render targets 1 and 2 store the front and back colors
  // Clear to 0.0 and use MAX blending to filter written color
  // At most one front color and one back color can be written every pass
  vl::glDrawBuffers(2, &g_drawBuffers[1]);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
  glDrawBuffer(g_drawBuffers[0]);
  glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  vl::glBlendEquation(GL_MAX);

  g_shaderDualInit->useProgram();
  g_shaderDualInit->applyUniformSet();
  
  renderQueue(render_queue, camera, frame_clock); // DrawModel();

  vl::glUseProgram(0);

  CHECK_GL_ERRORS;

  // ---------------------------------------------------------------------
  // 2. Dual Depth Peeling + Blending
  // ---------------------------------------------------------------------

  // Since we cannot blend the back colors in the geometry passes,
  // we use another render target to do the alpha blending
  //vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualBackBlenderFboId);
  glDrawBuffer(g_drawBuffers[6]);
  glClearColor(g_backgroundColor[0], g_backgroundColor[1], g_backgroundColor[2], 0);
  glClear(GL_COLOR_BUFFER_BIT);

  int currId = 0;

  for (int pass = 1; g_useOQ || pass < g_numPasses; pass++) {
	  currId = pass % 2;
	  int prevId = 1 - currId;
	  int bufId = currId * 3;
		
	  //vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingFboId[currId]);
      
	  vl::glDrawBuffers(2, &g_drawBuffers[bufId+1]);
	  glClearColor(0, 0, 0, 0);
	  glClear(GL_COLOR_BUFFER_BIT);

	  glDrawBuffer(g_drawBuffers[bufId+0]);
	  glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
	  glClear(GL_COLOR_BUFFER_BIT);

	  // Render target 0: RG32F MAX blending
	  // Render target 1: RGBA MAX blending
	  // Render target 2: RGBA MAX blending
	  vl::glDrawBuffers(3, &g_drawBuffers[bufId+0]);
	  vl::glBlendEquation(GL_MAX);

	  g_shaderDualPeel->useProgram();
	  bindTexture(g_shaderDualPeel.get(), GL_TEXTURE_RECTANGLE, "DepthBlenderTex", g_dualDepthTexId[prevId], 0);
	  bindTexture(g_shaderDualPeel.get(), GL_TEXTURE_RECTANGLE, "FrontBlenderTex", g_dualFrontBlenderTexId[prevId], 1);
	    
    // g_shaderDualPeel.setUniform("Alpha", (float*)&g_opacity, 1);
    g_shaderDualPeel->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_opacity);
    g_shaderDualPeel->applyUniformSet();
    
    renderQueue(render_queue, camera, frame_clock); // DrawModel();
	  
    vl::glUseProgram(0);

	  CHECK_GL_ERRORS;

	  // Full screen pass to alpha-blend the back color
	  glDrawBuffer(g_drawBuffers[6]);

	  vl::glBlendEquation(GL_FUNC_ADD);
	  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	  if (g_useOQ) {
      vl::glBeginQuery(GL_SAMPLES_PASSED_ARB, g_queryId);
	  }

	  g_shaderDualBlend->useProgram();
	  bindTexture(g_shaderDualBlend.get(), GL_TEXTURE_RECTANGLE, "TempTex", g_dualBackTempTexId[currId], 0);
    g_shaderDualBlend->applyUniformSet();
	  glCallList(g_quadDisplayList);
	  vl::glUseProgram(0);

	  CHECK_GL_ERRORS;

	  if (g_useOQ) {
		  vl::glEndQuery(GL_SAMPLES_PASSED_ARB);
		  GLuint sample_count;
		  vl::glGetQueryObjectuiv(g_queryId, GL_QUERY_RESULT_ARB, &sample_count);
		  if (sample_count == 0) {
			  break;
		  }
	  }
  }

  glDisable(GL_BLEND);

  // ---------------------------------------------------------------------
  // 3. Final Pass
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);

  g_shaderDualFinal->useProgram();
  // bindTexture(g_shaderDualFinal.get(), GL_TEXTURE_RECTANGLE, "DepthBlenderTex", g_dualDepthTexId[currId], 0);
  bindTexture(g_shaderDualFinal.get(), GL_TEXTURE_RECTANGLE, "FrontBlenderTex", g_dualFrontBlenderTexId[currId], 1);
  bindTexture(g_shaderDualFinal.get(), GL_TEXTURE_RECTANGLE, "BackBlenderTex", g_dualBackBlenderTexId, 2);
  g_shaderDualFinal->applyUniformSet();
  glCallList(g_quadDisplayList);
  vl::glUseProgram(0);

  // Cleanup
  vl::glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  vl::glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  vl::glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);
  vl::glActiveTexture(GL_TEXTURE0);

  CHECK_GL_ERRORS;
}

//--------------------------------------------------------------------------
void RendererVivid::RenderFrontToBackPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  // ---------------------------------------------------------------------
  // 1. Initialize Min Depth Buffer
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontColorBlenderFboId);
  glDrawBuffer(g_drawBuffers[0]);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);

  g_shaderFrontInit->useProgram();
  // g_shaderFrontInit.setUniform("Alpha", (float*)&g_opacity, 1);
  g_shaderFrontInit->setUniform(g_uniformAlpha.get());
  g_uniformAlpha->setUniform(1, (float*)&g_opacity);    
  g_shaderFrontInit->applyUniformSet();
  
  renderQueue(render_queue, camera, frame_clock); // DrawModel();

  vl::glUseProgram(0);

  CHECK_GL_ERRORS;

  // ---------------------------------------------------------------------
  // 2. Depth Peeling + Blending
  // ---------------------------------------------------------------------

  int numLayers = (g_numPasses - 1) * 2;
  for (int layer = 1; g_useOQ || layer < numLayers; layer++) {
	  int currId = layer % 2;
	  int prevId = 1 - currId;

	  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontFboId[currId]);
	  glDrawBuffer(g_drawBuffers[0]);

	  glClearColor(0, 0, 0, 0);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	  glDisable(GL_BLEND);
	  glEnable(GL_DEPTH_TEST);

	  if (g_useOQ) {
		  vl::glBeginQuery(GL_SAMPLES_PASSED_ARB, g_queryId);
	  }

	  g_shaderFrontPeel->useProgram();
	  bindTexture(g_shaderFrontPeel.get(), GL_TEXTURE_RECTANGLE, "DepthTex", g_frontDepthTexId[prevId], 0);
    // g_shaderFrontPeel.setUniform("Alpha", (float*)&g_opacity, 1);
    g_shaderFrontPeel->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_opacity);    
	  g_shaderFrontPeel->applyUniformSet();
    
    renderQueue(render_queue, camera, frame_clock); // DrawModel();

	  vl::glUseProgram(0);

	  if (g_useOQ) {
		  vl::glEndQuery(GL_SAMPLES_PASSED_ARB);
	  }

	  CHECK_GL_ERRORS;

	  vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontColorBlenderFboId);
	  glDrawBuffer(g_drawBuffers[0]);

	  glDisable(GL_DEPTH_TEST);
	  glEnable(GL_BLEND);

	  vl::glBlendEquation(GL_FUNC_ADD);
	  vl::glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		
	  g_shaderFrontBlend->useProgram();
	  bindTexture(g_shaderFrontBlend.get(), GL_TEXTURE_RECTANGLE, "TempTex", g_frontColorTexId[currId], 0);
    g_shaderFrontBlend->applyUniformSet();
	  glCallList(g_quadDisplayList);
	  vl::glUseProgram(0);

	  glDisable(GL_BLEND);

	  CHECK_GL_ERRORS;

	  if (g_useOQ) {
		  GLuint sample_count;
		  vl::glGetQueryObjectuiv(g_queryId, GL_QUERY_RESULT_ARB, &sample_count);
		  if (sample_count == 0) {
			  break;
		  }
	  }
  }

  // ---------------------------------------------------------------------
  // 3. Final Pass
  // ---------------------------------------------------------------------

  vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDrawBuffer(GL_BACK);
  glDisable(GL_DEPTH_TEST);

  g_shaderFrontFinal->useProgram();

  // g_shaderFrontFinal.setUniform("BackgroundColor", g_backgroundColor, 3);
  g_shaderFrontFinal->setUniform(g_uniformBackgroundColor.get());
  g_uniformBackgroundColor->setUniform3f(1, g_backgroundColor);

  bindTexture(g_shaderFrontFinal.get(), GL_TEXTURE_RECTANGLE, "ColorTex", g_frontColorBlenderTexId, 0);
  g_shaderFrontFinal->applyUniformSet();
  glCallList(g_quadDisplayList);
  vl::glUseProgram(0);

  // cleanup
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  vl::glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_RECTANGLE, 0);


  CHECK_GL_ERRORS;
}

void RendererVivid::bindTexture(vl::GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit)
{
  vl::glActiveTexture(GL_TEXTURE0 + texunit);
	glBindTexture(target, texid);
	vl::glActiveTexture(GL_TEXTURE0);

  int current_glsl_program = -1;
  glGetIntegerv(GL_CURRENT_PROGRAM, &current_glsl_program); VL_CHECK_OGL();
  VL_CHECK(current_glsl_program == (int)glsl->handle())

	// setTextureUnit(texname, texunit);
  int location = glsl->getUniformLocation(texname.c_str());
	if (location == -1) {
    printf("Warning: Invalid texture %s\n", texname.c_str());
		return;
	}
  vl::glUniform1i(location, texunit);
}

