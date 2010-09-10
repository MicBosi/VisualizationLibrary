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

using namespace vl;

//------------------------------------------------------------------------------
Rendering::Rendering():
  mClearFlags(CF_CLEAR_COLOR_DEPTH),
  mAutomaticResourceInit(true),
  mCullingEnabled(true),
  mEvaluateLOD(true),
  mActorAnimationEnabled(true),
  mShaderAnimationEnabled(true)
{
  #ifndef NDEBUG
    mName = "Rendering";
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

  if ( enableMask() == 0 )
    return;

  if (sceneManagers()->empty())
    return;

  // bind the OpenGL context

  if (renderer() && renderTarget())
    renderer()->setOpenGLContext(renderTarget()->openglContext());

  // activate OpenGL context

  if (renderTarget() && renderTarget()->openglContext())
  {
    renderTarget()->openglContext()->makeCurrent();

    // render states shield
    resetStates(); // must be called glPushAttrib() to be safe with FBO
    glPushAttrib(GL_ALL_ATTRIB_BITS); VL_CHECK_OGL();
    glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS); VL_CHECK_OGL();
  }

  // pre rendering callback

  dispatchRenderingCallbacks(RC_PreRendering);

  // render target (note: and OpenGL context can have multiple rendering targets!)

  if (renderTarget())
    renderTarget()->activate();

  // transform

  if ( transform() != NULL )
    transform()->computeWorldMatrixRecursive( camera() );

  // camera transform update (can be redundant)

  if (camera() && camera()->followedTransform())
    camera()->setInverseViewMatrix( camera()->followedTransform()->worldMatrix() );

  VL_CHECK_OGL()

  // culling & actor queue filling

  if (camera())
    camera()->computeFrustumPlanes();

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
                       !camera()->frustum().cull(sceneManagers()->at(i)->boundingBox())    ;
        if ( visible )
            sceneManagers()->at(i)->appendVisibleActors( *actorQueue(), camera() );
      }
      else
        sceneManagers()->at(i)->appendActors( *actorQueue() );
    }
  }

  // collect near/far clipping planes optimization information
  if (camera() && camera()->nearFarClippingPlanesOptimized())
  {
    Sphere world_bounding_sphere;
    for(int i=0; i<actorQueue()->size(); ++i)
      world_bounding_sphere += actorQueue()->at(i)->boundingSphere();
    camera()->setSceneBoundingSphere(world_bounding_sphere);
  }

  // camera and viewport activation: needs to be done after the near/far clipping planes optimization

  if (camera() && camera()->viewport())
  {
    camera()->viewport()->setClearFlags(clearFlags());
    camera()->viewport()->activate();
    camera()->activate();
  }

  // render queue filling

  renderQueue()->clear();

  fillRenderQueue( actorQueue() );

  // sort the rendering queue according to this renderer sorting algorithm

  if (renderQueueSorter())
    renderQueue()->sort( renderQueueSorter(), camera() );

  // render the queue

  if (renderer() && camera())
    renderer()->render( renderQueue(), camera() );

  // post rendering callback

  dispatchRenderingCallbacks(RC_PostRendering);

  // render states shield

  if (renderTarget() && renderTarget()->openglContext())
  {
    resetStates(); // must be called glPopAttrib() to be safe with FBO
    glPopAttrib(); VL_CHECK_OGL();
    glPopClientAttrib(); VL_CHECK_OGL();
  }

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

  // double internal_time = Time::currentTime();
  RenderQueue* list = renderQueue();

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

      // make sure the EnableSet and RenderStateSet exists

      // fixme
      shader->gocEnableSet();
      shader->gocRenderStateSet();

      // links GLSLPrograms and create requested textures
      if (automaticResourceInit())
        shader->initResources();

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
            // note that we set it after having called update
            shader->setLastUpdateTime( updateTime() );
          }
        }
      }

      tok->mEffectRenderRank = effect->renderRank();
    }
  }
}
//------------------------------------------------------------------------------
void Rendering::resetStates()
{
  // Check that the OpenGL state is clear.
  // If this fails use VL_CHECK_OGL to make sure your application does not generate OpenGL errors.
  // See also glGetError() -> http://www.opengl.org/sdk/docs/man/xhtml/glGetError.xml
  VL_CHECK_OGL();

  glRenderMode(GL_RENDER); VL_CHECK_OGL();

  VL_glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0); VL_CHECK_OGL();

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

  renderer()->resetEnables(); VL_CHECK_OGL()
  renderer()->resetRenderStates(); VL_CHECK_OGL()

  glNormal3f(1.0f,1.0f,1.0f); VL_CHECK_OGL()
  glColor4f(1.0f,1.0f,1.0f,1.0f); VL_CHECK_OGL()
  VL_glSecondaryColor3f(1.0f,1.0f,1.0f); VL_CHECK_OGL()

  glMatrixMode(GL_MODELVIEW); VL_CHECK_OGL()
  glLoadIdentity(); VL_CHECK_OGL()
  glMatrixMode(GL_PROJECTION); VL_CHECK_OGL()
  glLoadIdentity(); VL_CHECK_OGL()

  int max_texture = 1;
  if (GLEW_VERSION_1_3 || GLEW_ARB_multitexture)
  {
    glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &max_texture); VL_CHECK_OGL()
  }
  for(int i=0; i<max_texture; ++i)
  {
    VL_glActiveTexture(GL_TEXTURE0+i); VL_CHECK_OGL()
    VL_glClientActiveTexture(GL_TEXTURE0+i); VL_CHECK_OGL()

    glTexCoord3f(0,0,0); VL_CHECK_OGL()
    
    glMatrixMode(GL_TEXTURE); VL_CHECK_OGL()
    glLoadIdentity(); VL_CHECK_OGL()

    glDisableClientState(GL_TEXTURE_COORD_ARRAY); VL_CHECK_OGL()
    glDisable( GL_TEXTURE_1D ); VL_CHECK_OGL()
    glBindTexture( GL_TEXTURE_1D, 0 ); VL_CHECK_OGL()
    glDisable( GL_TEXTURE_2D ); VL_CHECK_OGL()
    glBindTexture( GL_TEXTURE_2D, 0 ); VL_CHECK_OGL()
    if (GLEW_ARB_texture_rectangle||GLEW_EXT_texture_rectangle||GLEW_NV_texture_rectangle/*TODO:||GLEW_VERSION_3_1*/)
    {
      glDisable( GL_TEXTURE_RECTANGLE_ARB ); VL_CHECK_OGL()
      glBindTexture( GL_TEXTURE_RECTANGLE_ARB, 0 ); VL_CHECK_OGL()
    }
    if (GLEW_EXT_texture_array||GLEW_VERSION_3_0)
    {
      // no need to enable/disable since is not available in the fixed function pipeline
      /*glDisable( GL_TEXTURE_1D_ARRAY_EXT );
      glDisable( GL_TEXTURE_2D_ARRAY_EXT );*/
      glBindTexture( GL_TEXTURE_1D_ARRAY_EXT, 0 ); VL_CHECK_OGL()
      glBindTexture( GL_TEXTURE_2D_ARRAY_EXT, 0 ); VL_CHECK_OGL()
    }
    if (GLEW_VERSION_1_2)
    {
      glDisable( GL_TEXTURE_3D ); VL_CHECK_OGL()
      glBindTexture( GL_TEXTURE_3D, 0 ); VL_CHECK_OGL()
    }
    if (GLEW_VERSION_1_3||GLEW_ARB_texture_cube_map)
    {
      glDisable( GL_TEXTURE_CUBE_MAP ); VL_CHECK_OGL()
      glBindTexture( GL_TEXTURE_CUBE_MAP, 0 ); VL_CHECK_OGL()
    }

    // glTexGen
    // glTexEnv
  }

  VL_glActiveTexture(GL_TEXTURE0); VL_CHECK_OGL()
  VL_glClientActiveTexture(GL_TEXTURE0); VL_CHECK_OGL()

  glMatrixMode(GL_MODELVIEW); VL_CHECK_OGL()

  glDisable(GL_TEXTURE_GEN_S); VL_CHECK_OGL()
  glDisable(GL_TEXTURE_GEN_T); VL_CHECK_OGL()
  glDisable(GL_TEXTURE_GEN_R); VL_CHECK_OGL()
  glDisable(GL_TEXTURE_GEN_Q); VL_CHECK_OGL()

  glDisable(GL_COLOR_MATERIAL); VL_CHECK_OGL()

  glDisable(GL_SCISSOR_TEST); VL_CHECK_OGL()

  if (GLEW_VERSION_1_4||GLEW_EXT_fog_coord)
    glDisableClientState(GL_FOG_COORD_ARRAY); 
  VL_CHECK_OGL()

  if (GLEW_VERSION_1_4||GLEW_EXT_secondary_color)
    glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
  VL_CHECK_OGL()

  glDisableClientState(GL_COLOR_ARRAY); VL_CHECK_OGL()
  glDisableClientState(GL_EDGE_FLAG_ARRAY); VL_CHECK_OGL()
  glDisableClientState(GL_INDEX_ARRAY); VL_CHECK_OGL()
  glDisableClientState(GL_NORMAL_ARRAY); VL_CHECK_OGL()
  glDisableClientState(GL_TEXTURE_COORD_ARRAY); VL_CHECK_OGL()
  glDisableClientState(GL_VERTEX_ARRAY ); VL_CHECK_OGL()

  VL_glBindBuffer(GL_ARRAY_BUFFER,0); VL_CHECK_OGL()
  VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0); VL_CHECK_OGL()

  if (GLEW_VERSION_2_1||GLEW_ARB_pixel_buffer_object)
  {
    VL_glBindBuffer(GL_PIXEL_PACK_BUFFER,0); VL_CHECK_OGL()
    VL_glBindBuffer(GL_PIXEL_UNPACK_BUFFER,0); VL_CHECK_OGL()
  }

  // glPixelStore

  glPixelStorei(GL_PACK_SWAP_BYTES, 0); VL_CHECK_OGL()
  glPixelStorei(GL_PACK_LSB_FIRST, 0); VL_CHECK_OGL()
  glPixelStorei(GL_PACK_ROW_LENGTH, 0); VL_CHECK_OGL()
  glPixelStorei(GL_PACK_SKIP_ROWS, 0); VL_CHECK_OGL()
  glPixelStorei(GL_PACK_SKIP_PIXELS, 0); VL_CHECK_OGL()
  glPixelStorei(GL_PACK_ALIGNMENT, 4); VL_CHECK_OGL()
  glPixelStorei(GL_UNPACK_SWAP_BYTES, 0); VL_CHECK_OGL()
  glPixelStorei(GL_UNPACK_LSB_FIRST, 0); VL_CHECK_OGL()
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0); VL_CHECK_OGL()
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0); VL_CHECK_OGL()
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0); VL_CHECK_OGL()
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4); VL_CHECK_OGL()
  if (GLEW_VERSION_1_2)
  {
    glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0); VL_CHECK_OGL()
    glPixelStorei(GL_PACK_SKIP_IMAGES, 0); VL_CHECK_OGL()
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0); VL_CHECK_OGL()
    glPixelStorei(GL_UNPACK_SKIP_IMAGES, 0); VL_CHECK_OGL()
  }

  glPixelZoom(1.0f, 1.0f); VL_CHECK_OGL()

  if (GLEW_ARB_imaging)
  {
    glDisable(GL_HISTOGRAM); VL_CHECK_OGL()
    glDisable(GL_MINMAX); VL_CHECK_OGL()
  }

  gluOrtho2D(1,2,3,4);

  VL_CHECK_OGL();

  // fixme
}
//------------------------------------------------------------------------------
