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

#include <vl/EdgeRenderer.hpp>
#include <vl/RenderQueue.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
const RenderQueue* EdgeRenderer::render(const RenderQueue* render_queue, Camera* camera)
{
  if (enableMask() == 0)
    return render_queue;

  // --------------- render target activation --------------- 
  // (note: an OpenGL context can have multiple rendering targets!)

  renderTarget()->activate();

  // --------------- viewport activation --------------- 

  camera->viewport()->setClearFlags(clearFlags());
  camera->viewport()->activate();

  // update actor cache

  updateActorCache(render_queue);

  camera->applyProjMatrix();

  // solid
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset( polygonOffsetFactor(), polygonOffsetUnits() );
  glColorMask(0,0,0,0);
    renderSolids(camera);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset( 0.0f, 0.0f );
  glPolygonOffset(0,0);
  glColorMask(1,1,1,1);

  // front wireframe
  glLineWidth(mLineWidth);
  if (smoothLines())
  {
    glEnable(GL_BLEND);
    glEnable(GL_LINE_SMOOTH);
  }
  renderLines(camera);

  // back wireframe
  glDisable(GL_DEPTH_TEST);
  glLineWidth(mLineWidth > 2.0f ? mLineWidth / 2.0f : 1.0f);
  glLineStipple(1,0xF0F0);
  glEnable(GL_LINE_STIPPLE);
    if (showHiddenLines()) renderLines(camera);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_LINE_STIPPLE);
  glDisable(GL_BLEND);
  glLineWidth(1.0f);

  return render_queue;
}
//-----------------------------------------------------------------------------
void EdgeRenderer::renderSolids(Camera* camera)
{
  // transform
  const Transform* cur_transform = NULL;
  camera->applyViewMatrix();

  /* some OpenGL drivers (ATI) require this instead of the more general (and mathematically correct) viewMatrix() */
  mat4 view_matrix = camera->viewMatrix();
  view_matrix.e(0,3) = 0.0;
  view_matrix.e(1,3) = 0.0;
  view_matrix.e(2,3) = 0.0;
  view_matrix.e(3,3) = 1.0;

  for( std::map< ref<Actor>, ref<WFInfo> >::iterator it = mVisibleActors.begin(); it != mVisibleActors.end(); ++it)
  {
    Actor* actor = it->first.get();
    const WFInfo* wfinfo = it->second.get();

    // --------------- transform ---------------

    // delta-setup for modelview matrix for the object
    if ( actor->transform() != cur_transform )
    {
      cur_transform = actor->transform();

      if ( cur_transform )
      {
        #if 0
          glMatrixMode(GL_MODELVIEW);
          VL_glLoadMatrix( view_matrix.ptr() );
          VL_glMultMatrix( cur_transform->worldMatrix().ptr() );
        #else
          // should guarantee better precision
          glMatrixMode(GL_MODELVIEW);
          VL_glLoadMatrix( (view_matrix * cur_transform->worldMatrix() ).ptr() );
        #endif
      }
      else
      {
        glMatrixMode(GL_MODELVIEW);
        VL_glLoadMatrix( view_matrix.ptr() );
      }
    }

    wfinfo->mEdgeCallback->setShowCreases(showCreases());
    wfinfo->mEdgeCallback->operator()( camera, actor, wfinfo->mGeometry.get(), NULL, 0 );
    actor->lod(0)->render( actor, camera );
  }
}
//-----------------------------------------------------------------------------
void EdgeRenderer::renderLines(Camera* camera)
{
  // transform
  const Transform* cur_transform = NULL;
  camera->applyViewMatrix();

  /* some OpenGL drivers (ATI) require this instead of the more general (and mathematically correct) viewMatrix() */
  mat4 view_matrix = camera->viewMatrix();
  view_matrix.e(0,3) = 0.0f;
  view_matrix.e(1,3) = 0.0f;
  view_matrix.e(2,3) = 0.0f;
  view_matrix.e(3,3) = 1.0f;

  for( std::map< ref<Actor>, ref<WFInfo> >::iterator it = mVisibleActors.begin(); it != mVisibleActors.end(); ++it)
  {
    Actor* actor = it->first.get();
    const WFInfo* wfinfo = it->second.get();

    // --------------- transform ---------------

    // delta-setup for modelview matrix for the object
    if ( actor->transform() != cur_transform )
    {
      cur_transform = actor->transform();

      if ( cur_transform )
      {
        #if 0
          glMatrixMode(GL_MODELVIEW);
          VL_glLoadMatrix( view_matrix.ptr() );
          VL_glMultMatrix( cur_transform->worldMatrix().ptr() );
        #else
          // should guarantee better precision
          glMatrixMode(GL_MODELVIEW);
          VL_glLoadMatrix( (view_matrix * cur_transform->worldMatrix() ).ptr() );
        #endif
      }
      else
      {
        glMatrixMode(GL_MODELVIEW);
        VL_glLoadMatrix( view_matrix.ptr() );
      }
    }

    // note: no rendering callbacks here
    wfinfo->mGeometry->render( actor, camera );
  }
}
//-----------------------------------------------------------------------------
EdgeRenderer::WFInfo* EdgeRenderer::declareActor(Actor* act, const fvec4& color)
{
  std::map< ref<Actor>, ref<WFInfo> >::iterator it = mActorCache.find( act );
  if (it!=mActorCache.end())
  {
    it->second->mGeometry->setColorArray(color);
    return it->second.get();
  }
  else
  {
    ref<WFInfo> info = new WFInfo;
    EdgeExtractor ee;
    ee.setCreaseAngle( creaseAngle() );
    if (ee.extractEdges(act))
    {
      info->mGeometry = ee.generateEdgeGeometry();
      info->mEdgeCallback = new EdgeUpdateCallback(ee.edges());
      if (info->mGeometry)
      {
        info->mGeometry->setColorArray(color);
        mActorCache[act] = info;
        return info.get();
      }
    }
  }
  return NULL;
}
//-----------------------------------------------------------------------------
EdgeRenderer::WFInfo* EdgeRenderer::declareActor(Actor* act)
{
  std::map< ref<Actor>, ref<WFInfo> >::iterator it = mActorCache.find( act );
  if (it!=mActorCache.end())
    return it->second.get();
  else
  {
    ref<WFInfo> info = new WFInfo;
    EdgeExtractor ee;
    ee.setCreaseAngle( creaseAngle() );
    if (ee.extractEdges(act))
    {
      info->mGeometry = ee.generateEdgeGeometry();
      info->mEdgeCallback = new EdgeUpdateCallback(ee.edges());
      if (info->mGeometry)
      {
        info->mGeometry->setColorArray(mDefaultLineColor);
        mActorCache[act] = info;
        return info.get();
      }
    }
  }
  return NULL;
}
//-----------------------------------------------------------------------------
void EdgeRenderer::updateActorCache(const RenderQueue* render_queue)
{
  mVisibleActors.clear();
  for(int i=0; i<render_queue->size(); ++i)
  {
    if ( !isEnabled(render_queue->at(i)->mActor->enableMask()) )
      continue;
    mVisibleActors[render_queue->at(i)->mActor] = declareActor(render_queue->at(i)->mActor);
  }
}
//-----------------------------------------------------------------------------

