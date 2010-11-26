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

#include <vl/Renderer.hpp>
#include <vl/RenderQueue.hpp>
#include <vl/Effect.hpp>
#include <vl/Transform.hpp>
#include <vl/checks.hpp>
#include <vl/GLSL.hpp>
#include <vl/Light.hpp>
#include <vl/ClipPlane.hpp>
#include <vl/Time.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>

using namespace vl;

//------------------------------------------------------------------------------
// Renderer
//------------------------------------------------------------------------------
Renderer::Renderer()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  mProjViewTranfCallback = new ProjViewTranfCallbackStandard;

  mDummyEnables  = new EnableSet;
  mDummyStateSet = new RenderStateSet;
}
//------------------------------------------------------------------------------
namespace
{
  struct ShaderInfo
  {
  public:
    ShaderInfo(): mTransform(NULL), mShaderUniformSet(NULL), mActorUniformSet(NULL) {}
    bool operator<(const ShaderInfo& other) const
    {
      if (mTransform != other.mTransform)
        return mTransform < other.mTransform;
      else
      if ( mShaderUniformSet != other.mShaderUniformSet ) 
        return mShaderUniformSet < other.mShaderUniformSet;
      else
        return mActorUniformSet < other.mActorUniformSet;
    }

    const Transform* mTransform;
    const UniformSet* mShaderUniformSet;
    const UniformSet* mActorUniformSet;
  };
}
//------------------------------------------------------------------------------
const RenderQueue* Renderer::render(const RenderQueue* render_queue, Camera* camera, Real frame_clock)
{
  VL_CHECK_OGL()

  // skip if renderer is disabled

  if (enableMask() == 0)
    return render_queue;

  // enter/exit behavior contract

  class InOutContract 
  {
    Renderer* mRenderer;
  public:
    InOutContract(Renderer* renderer, Camera* camera): mRenderer(renderer)
    {
      // increment the render tick.
      mRenderer->mRenderTick++;

      // render-target activation.
      // note: an OpenGL context can have multiple rendering targets!
      mRenderer->renderTarget()->activate();

      // viewport setup.
      camera->viewport()->setClearFlags( mRenderer->clearFlags() );
      camera->viewport()->activate();

      // dispatch the renderer-started event.
      mRenderer->dispatchOnRendererStarted();

      // check user-generated errors.
      VL_CHECK_OGL()
    }

    ~InOutContract()
    {
      // dispatch the renderer-finished event
      mRenderer->dispatchOnRendererFinished();

      // check user-generated errors.
      VL_CHECK_OGL()

      // note: we don't reset the render target here
    }
  };

  InOutContract contract(this, camera);

  // --------------- rendering --------------- 

  std::map<const GLSLProgram*, ShaderInfo> glslprogram_map;

  OpenGLContext* opengl_context = renderTarget()->openglContext();

  // --------------- default scissor ---------------

  // non GLSLProgram state sets
  const RenderStateSet* cur_render_state_set = NULL;
  const EnableSet* cur_enable_set = NULL;
  const Scissor* cur_scissor = NULL;

  // scissor the viewport by default: needed for points and lines since they are not clipped against the viewport
  #if 1
    glEnable(GL_SCISSOR_TEST);
    glScissor(camera->viewport()->x(), camera->viewport()->y(), camera->viewport()->width(), camera->viewport()->height());
  #else
    glDisable(GL_SCISSOR_TEST);
  #endif

  // --------------- rendering ---------------

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok   = render_queue->at(itok); VL_CHECK(tok);
    Actor* actor = tok->mActor; VL_CHECK(actor);

    if ( !isEnabled(actor->enableMask()) )
      continue;

    // --------------- Actor's scissor ---------------

    const Scissor* scissor = actor->scissor() ? actor->scissor() : tok->mShader->scissor();
    if (cur_scissor != scissor)
    {
      cur_scissor = scissor;
      if (cur_scissor)
      {
        cur_scissor->enable(camera->viewport());
      }
      else
      {
        #if 1
          // scissor the viewport by default: needed for points and lines with size > 1.0 as they are not clipped against the viewport.
          VL_CHECK(glIsEnabled(GL_SCISSOR_TEST))
          glScissor(camera->viewport()->x(), camera->viewport()->y(), camera->viewport()->width(), camera->viewport()->height());
        #else
          glDisable(GL_SCISSOR_TEST);
        #endif
      }
    }

    // multipassing
    for( int ipass=0; tok != NULL; tok = tok->mNextPass, ++ipass )
    {
      VL_CHECK_OGL()

      // --------------- shader setup ---------------

      const Shader* shader = tok->mShader;

      // shader override

      for( std::map< unsigned int, ref<Shader> >::const_iterator eom_it = mShaderOverrideMask.begin(); 
           eom_it != mShaderOverrideMask.end(); ++eom_it )
      {
        if ( eom_it->first & actor->enableMask() )
          shader = eom_it->second.get();
      }

      // shader's render states

      if ( cur_render_state_set != shader->getRenderStateSet() )
      {
        opengl_context->applyRenderStates(cur_render_state_set, shader->getRenderStateSet(), camera );
        cur_render_state_set = shader->getRenderStateSet();
      }

      VL_CHECK_OGL()

      // shader's enables

      if ( cur_enable_set != shader->getEnableSet() )
      {
        opengl_context->applyEnables(cur_enable_set, shader->getEnableSet() );
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
      /* mic fixme: document this */

      actor->dispatchOnActorRenderStarted( frame_clock, camera, tok->mRenderable, shader, ipass );

      VL_CHECK_OGL()

      // --------------- GLSLProgram setup ---------------

      VL_CHECK( !shader->glslProgram() || shader->glslProgram()->linked() );

      VL_CHECK_OGL()

      // current transform
      const Transform*   cur_transform          = actor->transform(); 
      const GLSLProgram* cur_glsl_program       = NULL; // NULL == fixed function pipeline
      const UniformSet*  cur_shader_uniform_set = NULL;
      const UniformSet*  cur_actor_uniform_set  = NULL;
      // make sure we update these things only if there is a valid GLSLProgram
      if (shader->glslProgram() && shader->glslProgram()->handle())
      {
        cur_glsl_program       = shader->glslProgram();
        cur_shader_uniform_set = shader->getUniformSet();
        cur_actor_uniform_set  = actor->getUniformSet();
        // consider them NULL if they are empty
        if (cur_shader_uniform_set && cur_shader_uniform_set->uniforms().empty())
          cur_shader_uniform_set = NULL;
        if (cur_actor_uniform_set  && cur_actor_uniform_set ->uniforms().empty())
          cur_actor_uniform_set = NULL;
      } 

      // is it the first update we do overall? (ie this is the first object rendered)
      bool is_first_overall = glslprogram_map.empty();
      bool update_su = false; // update shader uniforms
      bool update_au = false; // update actor uniforms
      bool update_tr = false; // update transform
      ShaderInfo* shader_info = NULL;
      // retrieve the state of this GLSLProgram (including the NULL one)
      std::map<const GLSLProgram*, ShaderInfo>::iterator shader_info_it = glslprogram_map.find(cur_glsl_program);
      bool is_first_use = false;
      if ( shader_info_it == glslprogram_map.end() )
      {
        // create a new shader-info entry
        shader_info = &glslprogram_map[cur_glsl_program];
        // update_tr = true; not needed since is_first_use overrides it.
        update_su = cur_shader_uniform_set != NULL ? true : false;
        update_au = cur_actor_uniform_set  != NULL ? true : false;
        // is it the first update to this GLSLProgram? Yes.
        is_first_use = true;
      }
      else
      {
        shader_info = &shader_info_it->second;      
        // check for differences
        update_tr = shader_info->mTransform        != cur_transform;
        update_su = shader_info->mShaderUniformSet != cur_shader_uniform_set && cur_shader_uniform_set;
        update_au = shader_info->mActorUniformSet  != cur_actor_uniform_set  && cur_actor_uniform_set;
      }

      // update shader-info structure
      shader_info->mTransform        = cur_transform;
      shader_info->mShaderUniformSet = cur_shader_uniform_set;
      shader_info->mActorUniformSet  = cur_actor_uniform_set;

      // fixme?
      // theoretically we can optimize further caching the last GLSLProgram used and it's shader-info and if 
      // the new one is the same as the old one we can avoid looking up in the map. The gain should be minimal
      // and the code would get further clutterd.

      // --- update proj, view and transform matrices ---

      VL_CHECK_OGL()

      if (is_first_use) // note: 'is_first_overall' implies 'is_first_use'
        projViewTransfCallback()->programFirstUse(this, cur_glsl_program, cur_transform, camera, is_first_overall);
      else
      if (update_tr)
        projViewTransfCallback()->programTransfChange(this, cur_glsl_program, cur_transform, camera);

      VL_CHECK_OGL()

      // --- uniforms ---

      // note: the user must not make the shader's and actor's uniforms collide!
      VL_CHECK( !opengl_context->areUniformsColliding(cur_shader_uniform_set, cur_actor_uniform_set) );

      // 'static' uniform set: update only once per rendering, if present.
      if (is_first_use && cur_glsl_program && cur_glsl_program->uniformSet())
      {
        cur_glsl_program->applyUniformSet(cur_glsl_program->uniformSet());
      }

      // shader uniform set
      if ( update_su )
      {
        VL_CHECK( cur_shader_uniform_set && cur_shader_uniform_set->uniforms().size() );
        VL_CHECK( shader->getRenderStateSet()->glslProgram() && shader->getRenderStateSet()->glslProgram()->handle() )
        cur_glsl_program->applyUniformSet( cur_shader_uniform_set );
      }

      VL_CHECK_OGL()

      // actor uniform set
      if ( update_au )
      {
        VL_CHECK( cur_actor_uniform_set && cur_actor_uniform_set->uniforms().size() );
        VL_CHECK( shader->getRenderStateSet()->glslProgram() && shader->getRenderStateSet()->glslProgram()->handle() )
        cur_glsl_program->applyUniformSet( cur_actor_uniform_set );
      }

      VL_CHECK_OGL()

      // --------------- Actor rendering ---------------

      VL_CHECK( !tok->mRenderable->displayListEnabled() || (tok->mRenderable->displayListEnabled() && tok->mRenderable->displayList()) )

      if (tok->mRenderable->displayListEnabled())
        glCallList( tok->mRenderable->displayList() );
      else
        tok->mRenderable->render( actor, shader, camera, opengl_context );

      VL_CHECK_OGL()

      // if shader is overridden it does not make sense to perform multipassing so we break the loop here.
      if (shader != tok->mShader)
        break;
    }
  }

  // clear enables
  opengl_context->applyEnables(cur_enable_set, mDummyEnables.get() );

  // clear render states
  opengl_context->applyRenderStates(cur_render_state_set, mDummyStateSet.get(), camera );

  // enabled texture unit #0
  VL_glActiveTexture( GL_TEXTURE0 );
  VL_glClientActiveTexture( GL_TEXTURE0 );

  // disable scissor test
  glDisable(GL_SCISSOR_TEST);

  // disable all vertex arrays
  opengl_context->bindVAS(NULL, false, false);
  VL_CHECK( opengl_context->isCleanState(true) );

  return render_queue;
}
//-----------------------------------------------------------------------------
void ProjViewTranfCallbackStandard::programFirstUse(const Renderer*, const GLSLProgram*, const Transform* transform, const Camera* camera, bool first_overall)
{
  if (mLastTransform != transform || first_overall)
  {
    if ( transform )
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( (camera->viewMatrix() * transform->worldMatrix() ).ptr() );
    }
    else
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( camera->viewMatrix().ptr() );
    }
  }

  // set the projection matrix only once per rendering
  if (first_overall)
  {
    glMatrixMode(GL_PROJECTION);
    VL_glLoadMatrix( (camera->projectionMatrix() ).ptr() );
  }

  // update last transform
  mLastTransform = transform;
}
//-----------------------------------------------------------------------------
void ProjViewTranfCallbackStandard::programTransfChange(const Renderer*, const GLSLProgram*, const Transform* transform, const Camera* camera)
{
  if (mLastTransform != transform)
  {
    if ( transform )
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( (camera->viewMatrix() * transform->worldMatrix() ).ptr() );
    }
    else
    {
      glMatrixMode(GL_MODELVIEW);
      VL_glLoadMatrix( camera->viewMatrix().ptr() );
    }
  }

  // update last transform
  mLastTransform = transform;
}
//-----------------------------------------------------------------------------
