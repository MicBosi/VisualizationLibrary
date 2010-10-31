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

namespace
{
  unsigned g_OcclusionQueryMasterTick = 0;
}

//------------------------------------------------------------------------------
// Renderer
//------------------------------------------------------------------------------
Renderer::Renderer()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif

  mClearFlags = CF_CLEAR_COLOR_DEPTH;
  mEnableMask = 0xFFFFFFFF;

  mOcclusionQueryTickPrev  = 0;
  mOcclusionQueryTick      = 0;
  mOcclusionThreshold      = 0;
  mOcclusionCullingEnabled = false;
  mCollectStatistics       = false;
  mRenderedRenderableCount = 0;
  mRenderedTriangleCount   = 0;
  mRenderedLineCount       = 0;
  mRenderedPointCount      = 0;

  mProjViewTranfCallback = new ProjViewTranfCallbackStandard;

  mDummyEnables  = new EnableSet;
  mDummyStateSet = new RenderStateSet;

  // occlusion culling shader
  // mic fixme
  //mOcclusionShader = new Shader;
  //mOcclusionShader->gocDepthMask()->set(false);
  //mOcclusionShader->gocColorMask()->set(false,false,false,false);
  //mOcclusionShader->enable(vl::EN_CULL_FACE);
  //mOcclusionShader->enable(vl::EN_DEPTH_TEST);
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
const RenderQueue* Renderer::render(const RenderQueue* render_queue, Camera* camera)
{
  VL_CHECK_OGL()

  std::map<const GLSLProgram*, ShaderInfo> glslprogram_map;

  mRenderedRenderableCount = 0;
  mRenderedTriangleCount  = 0;
  mRenderedLineCount = 0;
  mRenderedPointCount = 0;

  // skip if renderer is disabled

  if (enableMask() == 0)
    return render_queue;

  // --------------- viewport activation --------------- 

  camera->viewport()->setClearFlags(clearFlags());
  camera->viewport()->activate();

  // --------------- rendering ---------------

  // non GLSLProgram state sets
  const RenderStateSet* cur_render_state_set = NULL;
  const EnableSet* cur_enable_set = NULL;
  const Scissor* cur_scissor = NULL;

  // scissor the viewport by default: needed for points and lines sice they are not clipped against the viewport
  #if 1
    glEnable(GL_SCISSOR_TEST);
    glScissor(camera->viewport()->x(), camera->viewport()->y(), camera->viewport()->width(), camera->viewport()->height());
  #else
    glDisable(GL_SCISSOR_TEST);
  #endif

  /* camera/eye position (mic fixme) */
  // vec3 eye = camera->inverseViewMatrix().getT();

  /* current occlusion query tick (mic fixme) */
  // mOcclusionQueryTickPrev = mOcclusionQueryTick;
  // mOcclusionQueryTick     = g_OcclusionQueryMasterTick++;

  for(int itok=0; itok < render_queue->size(); ++itok)
  {
    const RenderToken* tok   = render_queue->at(itok);
    Actor* actor = tok->mActor;

    if ( !isEnabled(actor->enableMask()) )
      continue;

    VL_CHECK(tok);

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

    // --------------- occlusion culling ---------------

    // mic fixme - occlusion culling disabled for now until the transition to the new architecture is finalized.

    // New Occlusion Culling System: occludee devono essere messi in un rendering a parte e renderizzati
    // con un unico occlusion-shader, this way: 
    // 1) occluder rendering(normal shader)
    // 2) occludee rendering(occlusion shader)
    // 3) occludee rendering(normal shader) if previous occlusion query returns the object is visible
    // nota che i passi 1, 2, 3 non e' per-actor, ma prima si renderizzano tutti gli occluders, poi tutti i 
    // candidati occludee con occluder shader, poi di nuovo tutti gli occludee non occlusi con il loro shader normale.

    // ? forse si dovrebbero sintetizzare dei RenderToken al volo ?
    // ? lasciare l'utente definire degli OcclusionQueryGeometry  ?
    // ? quando l'occludee e' renderizzato l'occlusion query va fatta prima di settare shaders, states etc. ?

    /*
    bool occluded = false;
    if ( occlusionCullingEnabled() && !actor->boundingBox().isInside(eye) )
    {
      VL_CHECK(GLEW_ARB_occlusion_query || GLEW_VERSION_1_5 || GLEW_VERSION_3_0)

      if ( actor->occlusionQuery() && actor->occlusionQueryTick() == mOcclusionQueryTickPrev )
      {
        #if 0
          GLint ready = GL_FALSE;
          glGetQueryObjectiv(actor->occlusionQuery(),GL_QUERY_RESULT_AVAILABLE,&ready); VL_CHECK_OGL();
          if (ready == GL_FALSE)
            vl::Log::error("Occlusion culling query not yet available.\n");
        #endif
        // a few benchmarks say that even if it is not ready it is convenient to flush the OpenGL pipeline at this point
        GLint pixels = 0;
        glGetQueryObjectiv(actor->occlusionQuery(),GL_QUERY_RESULT,&pixels); VL_CHECK_OGL();
        // object is occluded
        if (pixels <= occlusionThreshold())
          occluded = true;
      }

      // if occludee -> perform occlusion test to be used for the next frame
      if (actor->isOccludee())
      {
        // register occlusion query tick

        actor->setOcclusionQueryTick(mOcclusionQueryTick);

        // activate occlusion culling shader

        applyRenderStates(cur_render_state_set, mOcclusionShader->getRenderStateSet(), camera );
        cur_render_state_set = mOcclusionShader->getRenderStateSet();

        VL_CHECK_OGL()

        applyEnables(cur_enable_set, mOcclusionShader->getEnableSet() );
        cur_enable_set = mOcclusionShader->getEnableSet();

        VL_CHECK_OGL()

        // compute Renderable AABB geometry (we are using the currently active Transform)
        const AABB& aabb = tok->mRenderable->boundingBox();
        const float verts[] = 
        {
          (float)aabb.minCorner().x(), (float)aabb.minCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.minCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.minCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.minCorner().z(),
          (float)aabb.minCorner().x(), (float)aabb.minCorner().y(), (float)aabb.maxCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.minCorner().y(), (float)aabb.maxCorner().z(),
          (float)aabb.maxCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.maxCorner().z(),
          (float)aabb.minCorner().x(), (float)aabb.maxCorner().y(), (float)aabb.maxCorner().z()
        };
        const unsigned quads[] = { 3,2,1,0, 2,6,5,1, 3,7,6,2, 7,3,0,4, 4,0,1,5, 6,7,4,5 };
        // glColor3f(1.0f,1.0f,1.0f);
        glEnableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
        glVertexPointer(3, GL_FLOAT, 0, verts); VL_CHECK_OGL();
          actor->createOcclusionQuery(); VL_CHECK_OGL();
          glBeginQuery(GL_SAMPLES_PASSED, actor->occlusionQuery()); VL_CHECK_OGL();
          glDrawElements(GL_QUADS, 6*4, GL_UNSIGNED_INT, quads); VL_CHECK_OGL();
          glEndQuery(GL_SAMPLES_PASSED); VL_CHECK_OGL();
        glDisableClientState(GL_VERTEX_ARRAY); VL_CHECK_OGL();
        glVertexPointer(3, GL_FLOAT, 0, NULL); VL_CHECK_OGL();
      }
    }

    // skip occluded object
    if (occluded)
      continue;
    */

    VL_CHECK_OGL()

    // multipassing
    for( int ipass=0; tok != NULL; tok = tok->mNextPass, ++ipass )
    {
      VL_CHECK_OGL()

      // --------------- shader setup ---------------

      const Shader* shader = tok->mShader;

      // shader override

      for( std::map< unsigned int, ref<Shader> >::const_iterator eom_it = mShaderOverrideMask.begin(); 
        eom_it != mShaderOverrideMask.end();
        ++eom_it )
      {
        if (eom_it->first & actor->enableMask())
          shader = eom_it->second.get();
      }

      // shader's render states

      if ( cur_render_state_set != shader->getRenderStateSet() )
      {
        openglContext()->applyRenderStates(cur_render_state_set, shader->getRenderStateSet(), camera );
        cur_render_state_set = shader->getRenderStateSet();
      }

      VL_CHECK_OGL()

      // shader's enables

      if ( cur_enable_set != shader->getEnableSet() )
      {
        openglContext()->applyEnables(cur_enable_set, shader->getEnableSet() );
        cur_enable_set = shader->getEnableSet();
      }

      #ifndef NDEBUG
        if (glGetError() != GL_NO_ERROR)
        {
          Log::error("An unsupported OpenGL glEnable/glDisable mode has been enabled!\n");
          VL_TRAP()
        }
      #endif

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
        cur_actor_uniform_set  = actor->uniformSet();
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
      VL_CHECK( !openglContext()->areUniformsColliding(cur_shader_uniform_set, cur_actor_uniform_set) );

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

      // --- Actor's prerender callback --- done after GLSLProgam has been bound and setup
      actor->executeRenderingCallbacks( camera, tok->mRenderable, shader, ipass );

      VL_CHECK_OGL()

      // --------------- Actor rendering ---------------

      // statistics

      if (collectStatistics())
      {
        mRenderedRenderableCount++;
        mRenderedTriangleCount += tok->mRenderable->triangleCount();
        mRenderedLineCount     += tok->mRenderable->lineCount();
        mRenderedPointCount    += tok->mRenderable->pointCount();
      }

      // contract (fixme: to be changed for vertex-array and elem-array lazy bind):
      // 1 - all vertex arrays and VBOs are disabled before calling render()
      // 2 - all vertex arrays and VBOs are disabled after  calling render()

      VL_CHECK( !tok->mRenderable->displayListEnabled() || (tok->mRenderable->displayListEnabled() && tok->mRenderable->displayList()) )

      if (tok->mRenderable->displayListEnabled())
        glCallList( tok->mRenderable->displayList() );
      else
        tok->mRenderable->render( actor, camera );

      VL_CHECK_OGL()

      // if shader is overridden it does not make sense to perform multipassing so we break the loop here.
      if (shader != tok->mShader)
        break;
    }
  }

  // clear enables
  openglContext()->applyEnables(cur_enable_set, mDummyEnables.get() );

  // clear render states
  openglContext()->applyRenderStates(cur_render_state_set, mDummyStateSet.get(), camera );

  glDisable(GL_SCISSOR_TEST);

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
