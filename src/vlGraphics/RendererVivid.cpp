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

using namespace vl;

//------------------------------------------------------------------------------
// RendererVivid
//------------------------------------------------------------------------------
RendererVivid::RendererVivid()
{
  VL_DEBUG_SET_OBJECT_NAME()

  mProjViewTransfCallback = new ProjViewTransfCallback;

  mDummyEnables  = new EnableSet;
  mDummyStateSet = new RenderStateSet;
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
const RenderQueue* RendererVivid::render(const RenderQueue* render_queue, Camera* camera, real frame_clock)
{
  VL_CHECK_OGL()

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

  // --------------- default scissor ---------------

  // non GLSLProgram state sets
  const RenderStateSet* cur_render_state_set = NULL;
  const EnableSet* cur_enable_set = NULL;

  // --------------- rendering ---------------

  // Update camera transform
  projViewTransfCallback()->updateMatrices( /*update_cm*/true, /*update_tr*/false, 0, camera, /*cur_transform*/NULL );

  // Disable scissor test
  glDisable(GL_SCISSOR_TEST);

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok = render_queue->at(itok); VL_CHECK(tok);
    Actor* actor = tok->mActor; VL_CHECK(actor);

    if ( !isEnabled(actor->enableMask()) )
      continue;

    VL_CHECK_OGL()

    // --------------- shader setup ---------------

    const Shader* shader = tok->mShader;

    // shader's render states

    if ( cur_render_state_set != shader->getRenderStateSet() )
    {
      opengl_context->applyRenderStates( shader->getRenderStateSet(), camera );
      cur_render_state_set = shader->getRenderStateSet();
    }

    VL_CHECK_OGL()

    // shader's enables

    if ( cur_enable_set != shader->getEnableSet() )
    {
      opengl_context->applyEnables( shader->getEnableSet() );
      cur_enable_set = shader->getEnableSet();
    }

    #ifndef NDEBUG
      if (glGetError() != GL_NO_ERROR)
      {
        Log::error("An unsupported OpenGL glEnable/glDisable capability has been enabled!\n");
        VL_TRAP()
      }
    #endif

    // --------------- Actor pre-render callback ---------------

    // here the user has still the possibility to modify the Actor's uniforms

    actor->dispatchOnActorRenderStarted( frame_clock, camera, tok->mRenderable, shader, /*ipass*/0 );

    VL_CHECK_OGL()

    // --------------- Transform setup ---------------

    // Update actor's transform
    projViewTransfCallback()->updateMatrices( /*update_cm*/false, /*update_tr*/true, 0, camera, actor->transform() );

    VL_CHECK_OGL()

    // --------------- Actor rendering ---------------

    // also compiles display lists and updates BufferObjects if necessary
    tok->mRenderable->render( actor, shader, camera, opengl_context );

    VL_CHECK_OGL()
  }

  // clear enables
  opengl_context->applyEnables( mDummyEnables.get() ); VL_CHECK_OGL();

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
