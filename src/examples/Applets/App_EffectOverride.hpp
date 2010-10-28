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

#ifndef App_EffectOverride_INCLUDE_ONCE
#define App_EffectOverride_INCLUDE_ONCE

#include "BaseDemo.hpp"
#include "vlut/GeometryPrimitives.hpp"
#include "vl/SceneManagerActorTree.hpp"
#include "vl/Actor.hpp"
#include "vl/Effect.hpp"
#include "vl/Time.hpp"
#include "vl/Light.hpp"
#include "vl/RenderingTree.hpp"

class App_EffectOverride: public BaseDemo
{
  void initEvent()
  {
    BaseDemo::initEvent();

    mRendering1 = new vl::Rendering;
    *mRendering1 = *(vl::VisualizationLibrary::rendering()->as<vl::Rendering>());
    mRendering2  = new vl::Rendering;
    *mRendering2  = *(vl::VisualizationLibrary::rendering()->as<vl::Rendering>());
    mRendering2->setClearFlags(vl::CF_CLEAR_DEPTH);

    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    vl::VisualizationLibrary::setRendering(render_tree.get());
    render_tree->subRenderings()->push_back(mRendering1.get());
    render_tree->subRenderings()->push_back(mRendering2.get());

    mCubeTransform1 = new vl::Transform;
    mCubeTransform2 = new vl::Transform;
    mCubeTransform3 = new vl::Transform;
    mRendering1->transform()->addChild( mCubeTransform1.get() );
    mRendering1->transform()->addChild( mCubeTransform2.get() );
    mRendering1->transform()->addChild( mCubeTransform3.get() );

    const vl::Real size = 6;
    vl::ref<vl::Geometry> cube = vlut::makeBox( vl::vec3(0,0,0), size, size, size);
    cube->computeNormals();

    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->setRenderState( new vl::Light(0) );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_CULL_FACE);
    effect->shader()->gocMaterial()->setDiffuse( vlut::gold );
    effect->shader()->gocMaterial()->setTransparency( 0.5f );

    vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
    mRendering1->sceneManagers()->push_back(scene_manager.get());
    mRendering2->sceneManagers()->push_back(scene_manager.get());

    vl::Actor* cube1 = scene_manager->tree()->addActor( cube.get(), effect.get(), mCubeTransform1.get() );
    vl::Actor* cube2 = scene_manager->tree()->addActor( cube.get(), effect.get(), mCubeTransform2.get() );
    vl::Actor* cube3 = scene_manager->tree()->addActor( cube.get(), effect.get(), mCubeTransform3.get() );

    cube1->setEnableMask(0x01);
    cube2->setEnableMask(0x02);
    cube3->setEnableMask(0x04);

    vl::ref<vl::Effect> fx1 = new vl::Effect;
    fx1->shader()->setRenderState( new vl::Light(0) );
    fx1->shader()->enable(vl::EN_LIGHTING);
    fx1->shader()->gocMaterial()->setFlatColor(vlut::red);
    fx1->shader()->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
    fx1->shader()->gocLineWidth()->set(2.5f);

    vl::ref<vl::Effect> fx2 = new vl::Effect;
    fx2->shader()->setRenderState( new vl::Light(0) );
    fx2->shader()->enable(vl::EN_LIGHTING);
    fx2->shader()->gocMaterial()->setFlatColor(vlut::green);
    fx2->shader()->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
    fx2->shader()->gocLineWidth()->set(2.5f);

    vl::ref<vl::Effect> fx3 = new vl::Effect;
    fx3->shader()->setRenderState( new vl::Light(0) );
    fx3->shader()->enable(vl::EN_LIGHTING);
    fx3->shader()->gocMaterial()->setFlatColor(vlut::blue);
    fx3->shader()->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
    fx3->shader()->gocLineWidth()->set(2.5f);

    std::pair< unsigned int,vl::ref<vl::Effect> > override1(0x01, fx1.get());
    std::pair< unsigned int,vl::ref<vl::Effect> > override2(0x02, fx2.get());
    std::pair< unsigned int,vl::ref<vl::Effect> > override3(0x04, fx3.get());
    mRendering2->effectOverrideMask().push_back( override1 );
    mRendering2->effectOverrideMask().push_back( override2 );
    mRendering2->effectOverrideMask().push_back( override3 );
  }

  virtual void run()
  {
    vl::Real degrees = vl::Time::currentTime() * 45.0f;
    vl::mat4 matrix;
    
    matrix.rotate( degrees, 0,1,0 );
    matrix.translate(-10,0,0);
    mCubeTransform1->setLocalMatrix( matrix );

    matrix.setIdentity();
    matrix.rotate( degrees, 0,1,0 );
    matrix.translate(0,0,0);
    mCubeTransform2->setLocalMatrix( matrix );

    matrix.setIdentity();
    matrix.rotate( degrees, 0,1,0 );
    matrix.translate(+10,0,0);
    mCubeTransform3->setLocalMatrix( matrix );
  }

  void resizeEvent(int /*w*/, int /*h*/)
  {
    mRendering1->camera()->viewport()->setWidth(mRendering1->renderTarget()->width());
    mRendering1->camera()->viewport()->setHeight(mRendering1->renderTarget()->height());
    mRendering1->camera()->setProjectionAsPerspective();

    mRendering2->camera()->viewport()->setWidth(mRendering2->renderTarget()->width());
    mRendering2->camera()->viewport()->setHeight(mRendering2->renderTarget()->height());
    mRendering2->camera()->setProjectionAsPerspective();
  }

protected:
  vl::ref<vl::Rendering> mRendering1;
  vl::ref<vl::Rendering> mRendering2;
  vl::ref<vl::Transform> mCubeTransform1;
  vl::ref<vl::Transform> mCubeTransform2;
  vl::ref<vl::Transform> mCubeTransform3;
};

// Have fun!

#endif
