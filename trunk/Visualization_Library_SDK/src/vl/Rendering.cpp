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

#include <vl/Rendering.hpp>
#include <vl/Renderer.hpp>
#include <vl/SceneManager.hpp>
#include <vl/RenderQueue.hpp>
#include <vl/GLSL.hpp>

using namespace vl;

//------------------------------------------------------------------------------
Rendering::Rendering():
  mClearFlags(CF_CLEAR_COLOR_DEPTH),
  mAutomaticResourceInit(true),
  mCullingEnabled(true),
  mEvaluateLOD(true),
  mActorAnimationEnabled(true),
  mShaderAnimationEnabled(true),
  mNearFarClippingPlanesOptimized(false)
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mRenderQueueSorter  = new RenderQueueSorterStandard;
  mActorQueue         = new ActorCollection;
  mRenderQueue        = new RenderQueue;
  mSceneManagers      = new Collection<SceneManager>;
  mRenderer           = new Renderer;
  mCamera             = new Camera;
  mTransform          = new Transform;
}
//------------------------------------------------------------------------------
Rendering& Rendering::operator=(const Rendering& other)
{
  RenderingAbstract::operator=(other);

  mClearFlags               = other.mClearFlags;
  mEnableMask               = other.mEnableMask;
  mAutomaticResourceInit    = other.mAutomaticResourceInit;
  mCullingEnabled    = other.mCullingEnabled;
  mEvaluateLOD              = other.mEvaluateLOD;
  mActorAnimationEnabled    = other.mActorAnimationEnabled;
  mShaderAnimationEnabled   = other.mShaderAnimationEnabled;
  mNearFarClippingPlanesOptimized = other.mNearFarClippingPlanesOptimized;

  mRenderTarget        = other.mRenderTarget;
  mRenderQueueSorter   = other.mRenderQueueSorter;
  /*mActorQueue        = other.mActorQueue;*/
  /*mRenderQueue       = other.mRenderQueue;*/
  *mSceneManagers      = *other.mSceneManagers;
  mRenderer            = other.mRenderer;
  mCamera              = other.mCamera;
  mTransform           = other.mTransform;

  return *this;
}
//------------------------------------------------------------------------------
void Rendering::render()
{
  VL_CHECK_OGL();
  VL_CHECK(camera());
  VL_CHECK(camera()->viewport());
  VL_CHECK(renderer());
  VL_CHECK(renderTarget());
  VL_CHECK(renderTarget()->openglContext());

  if ( enableMask() == 0 )
    return;

  if (sceneManagers()->empty())
    return;

  if (!camera())
    return;

  if (!camera()->viewport())
    return;

  if (!renderer())
    return;

  if (!renderTarget())
    return;

  if (!renderTarget()->openglContext())
    return;

  // bind the OpenGL context

  renderer()->setOpenGLContext(renderTarget()->openglContext());

  // activate OpenGL context

  renderTarget()->openglContext()->makeCurrent();

  // render states ]shield[

  renderTarget()->openglContext()->resetContextStates(); 

  // pre rendering callback

  dispatchRenderingCallbacks(RC_PreRendering);

  // render target (note: an OpenGL context can have multiple rendering targets!)

  renderTarget()->activate();

  // transform

  if (transform() != NULL)
    transform()->computeWorldMatrixRecursive( camera() );

  // camera transform update (can be redundant)

  if (camera()->followedTransform())
    camera()->setInverseViewMatrix( camera()->followedTransform()->worldMatrix() );

  VL_CHECK_OGL()

  // culling & actor queue filling

  camera()->computeFrustumPlanes();

  // if near/far clipping planes optimization is enabled don't perform far-culling
  if (nearFarClippingPlanesOptimized())
  {
    // perform only near culling with plane at distance 0
    camera()->frustum().planes().resize(5);
    camera()->frustum().planes()[4] = Plane( camera()->inverseViewMatrix().getT(), 
                                             camera()->inverseViewMatrix().getZ());
  }

  actorQueue()->clear();
  for(int i=0; i<sceneManagers()->size(); ++i)
  {
    if ( isEnabled(sceneManagers()->at(i)->enableMask()) )
    {
      if (cullingEnabled())
      {
        if (sceneManagers()->at(i)->boundsDirty())
          sceneManagers()->at(i)->computeBounds();
        // try to cull the scene with both bsphere and bbox
        bool visible = !camera()->frustum().cull(sceneManagers()->at(i)->boundingSphere()) && 
                       !camera()->frustum().cull(sceneManagers()->at(i)->boundingBox());
        if ( visible )
            sceneManagers()->at(i)->appendVisibleActors( *actorQueue(), camera() );
      }
      else
        sceneManagers()->at(i)->appendActors( *actorQueue() );
    }
  }

  // collect near/far clipping planes optimization information
  if (nearFarClippingPlanesOptimized())
  {
    Sphere world_bounding_sphere;
    for(int i=0; i<actorQueue()->size(); ++i)
      world_bounding_sphere += actorQueue()->at(i)->boundingSphere();

    // compute the optimized
    camera()->computeNearFarOptimizedProjMatrix(world_bounding_sphere);

    // recompute frustum planes to account for new near/far values
    camera()->computeFrustumPlanes();
  }

  // camera and viewport activation: needs to be done after the near/far clipping planes optimization

  camera()->viewport()->setClearFlags(clearFlags());
  camera()->viewport()->activate();

  // render queue filling

  renderQueue()->clear();

  fillRenderQueue( actorQueue() );

  // sort the rendering queue according to this renderer sorting algorithm

  if (renderQueueSorter())
    renderQueue()->sort( renderQueueSorter(), camera() );

  // render the queue

  renderer()->render( renderQueue(), camera() );

  // post rendering callback

  dispatchRenderingCallbacks(RC_PostRendering);

  // render states ]shield[

  renderTarget()->openglContext()->resetContextStates(); 

  VL_CHECK_OGL()
}
//------------------------------------------------------------------------------
void Rendering::fillRenderQueue( ActorCollection* actor_list )
{
  if (actor_list == NULL)
    return;

  if (actor_list->empty())
    return;

  if (camera() == NULL)
    return;

  if (enableMask() == 0)
    return;

  RenderQueue* list = renderQueue();
  std::set<Shader*> shader_set;

  // iterate actor list

  for(int iactor=0; iactor<actor_list->size(); iactor++)
  {
    Actor* actor = actor_list->at(iactor);

    VL_CHECK(actor->lod(0))

    if ( !isEnabled(actor->enableMask()) || !isEnabled(actor->effect()->enableMask()) )
      continue;

    // update the Actor's bounds
    actor->computeBounds();

    Effect* effect = actor->effect();
    VL_CHECK(effect)

    // --------------- LOD evaluation ---------------

    int effect_lod = effect->evaluateLOD( actor, camera() );

    int geometry_lod = 0;
    if ( evaluateLOD() )
      geometry_lod = actor->evaluateLOD( camera() );

    // --------------- Actor update ---------------

    if ( actorAnimationEnabled() )
    {
      VL_CHECK(updateTime() >= 0)
      if( updateTime() >= 0 )
      {
        // note that the condition is != and not <
        if (actor->lastUpdateTime() != updateTime() )
        {
          actor->update( geometry_lod, camera(), updateTime() );
          // note that we set it after having called update
          actor->setLastUpdateTime( updateTime() );
        }
      }
    }

    // --------------- Display List ---------------

    if ( actor->lod(geometry_lod)->displayListEnabled() && (!actor->lod(geometry_lod)->displayList()||actor->lod(geometry_lod)->displayListDirty()) )
      actor->lod(geometry_lod)->compileDisplayList(actor, renderTarget()->openglContext(), camera());

    // --------------- Update VBOs ---------------

    if ( actor->lod(geometry_lod)->vboEnabled() && actor->lod(geometry_lod)->vboDirty() && !actor->lod(geometry_lod)->displayListEnabled() )
      actor->lod(geometry_lod)->updateVBOs(false);

    // --------------- M U L T I   P A S S I N G ---------------

    RenderToken* prev_pass = NULL;
    const int pass_count = effect->lod(effect_lod)->size();
    for(int ipass=0; ipass<pass_count; ++ipass)
    {
      // setup the shader to be used for this pass

      Shader* shader = effect->lod(effect_lod)->at(ipass);

      // --------------- fill render token ---------------

      // create a render token
      RenderToken* tok = list->newToken(prev_pass != NULL);

      // multipass chain: implemented as a linked list
      if ( prev_pass != NULL )
        prev_pass->mNextPass = tok;
      prev_pass = tok;
      tok->mNextPass = NULL;
      // track the current state
      tok->mActor = actor;
      tok->mRenderable = actor->lod(geometry_lod).get();
      // set the shader used (multipassing shader or effect->shader())
      tok->mShader = shader;

      if ( shaderAnimationEnabled() )
      {
        VL_CHECK(updateTime() >= 0)
        if( updateTime() >= 0 )
        {
          // note that the condition is != and not <
          if ( shader->lastUpdateTime() != updateTime() )
          {
            // update
            shader->update( camera(), updateTime() );
            // note that we update it afterwards
            shader->setLastUpdateTime( updateTime() );
          }
        }
      }

      // mic fixme ? make sure the EnableSet and RenderStateSet exists
      /*shader->gocEnableSet();
      shader->gocRenderStateSet();*/

      if ( automaticResourceInit() && shader_set.find(shader) == shader_set.end() )
      {
        shader_set.insert(shader);

        // link GLSLProgram
        if (shader->glslProgram())
        {
          shader->glslProgram()->linkProgram();
          VL_CHECK( shader->glslProgram()->linked() );
        }

        // lazy texture creation
        if ( shader->gocRenderStateSet() )
        {
          const std::vector< ref<RenderState> >& states = shader->gocRenderStateSet()->renderStates();
          for( size_t i=0; i<states.size(); ++i )
          {
            if (states[i]->type() >= RS_TextureUnit0 && states[i]->type() < RS_TextureUnit0+VL_MAX_TEXTURE_UNITS)
            {
              TextureUnit* tex_unit = dynamic_cast<TextureUnit*>( states[i].get() );
              VL_CHECK(tex_unit);
              if (tex_unit)
              {
                if (tex_unit->texture() && tex_unit->texture()->setupParams())
                  tex_unit->texture()->createTexture();
              }
            }
          }
          
        }
      }

      tok->mEffectRenderRank = effect->renderRank();
    }
  }
}
//------------------------------------------------------------------------------
