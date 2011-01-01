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

#include "BaseDemo.hpp"
#include "vlut/GeometryPrimitives.hpp"
#include "vlCore/SceneManagerActorTree.hpp"
#include "vlCore/Actor.hpp"
#include "vlCore/Effect.hpp"
#include "vlCore/Time.hpp"
#include "vlCore/Light.hpp"
#include "vlCore/RenderingTree.hpp"

class App_EffectOverride: public BaseDemo
{
  void initEvent()
  {
    BaseDemo::initEvent();

    // initialize solid & wire rendering with the default camera, transform root and scene manager.
    mSolidRendering = new vl::Rendering;
    mWireRendering  = new vl::Rendering;
    *mSolidRendering = *(vl::VisualizationLibrary::rendering()->as<vl::Rendering>());
    *mWireRendering  = *(vl::VisualizationLibrary::rendering()->as<vl::Rendering>());

    // install the render tree that calls our two solid and wire renderings
    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    vl::VisualizationLibrary::setRendering(render_tree.get());
    render_tree->subRenderings()->push_back(mSolidRendering.get());
    render_tree->subRenderings()->push_back(mWireRendering.get());

    // this is shared by both solid and wire renderings
    vl::ref<vl::SceneManagerActorTree> scene_manager = mSolidRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>();

    // initialize wire rendering's Renderer

    // create its own copy of the renderer
    vl::ref<vl::Renderer> wire_renderer = new vl::Renderer;
    // don't clear color buffer otherwise we loose the results of the solid renderer
    wire_renderer->setClearFlags(vl::CF_CLEAR_DEPTH);
    // target the same OpenGL window
    wire_renderer->setRenderTarget( mSolidRendering->renderer()->renderTarget() );
    // install the renderer
    mWireRendering->setRenderer( wire_renderer.get() );

    mCubeTransform1 = new vl::Transform;
    mCubeTransform2 = new vl::Transform;
    mCubeTransform3 = new vl::Transform;
    mSolidRendering->transform()->addChild( mCubeTransform1.get() );
    mSolidRendering->transform()->addChild( mCubeTransform2.get() );
    mSolidRendering->transform()->addChild( mCubeTransform3.get() );

    const vl::Real fsize = 8;
    vl::ref<vl::Geometry> ball = vlut::makeUVSphere( vl::vec3(0,0,0), fsize, 8, 8 );
    ball->computeNormals();

    // setup solid effect

    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->setRenderState( new vl::Light(0) );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_CULL_FACE);
    effect->shader()->gocMaterial()->setDiffuse( vlut::gold );
    effect->shader()->gocMaterial()->setTransparency( 0.5f );

    // populate the scene

    vl::Actor* ball1 = scene_manager->tree()->addActor( ball.get(), effect.get(), mCubeTransform1.get() );
    vl::Actor* ball2 = scene_manager->tree()->addActor( ball.get(), effect.get(), mCubeTransform2.get() );
    vl::Actor* ball3 = scene_manager->tree()->addActor( ball.get(), effect.get(), mCubeTransform3.get() );

    // setup wire effects

    vl::ref<vl::Effect> fx1 = new vl::Effect;
    fx1->shader()->enable(vl::EN_LIGHTING);
    fx1->shader()->gocMaterial()->setFlatColor(vlut::red);
    fx1->shader()->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
    fx1->shader()->gocLineWidth()->set(2.0f);
    fx1->shader()->enable(vl::EN_CULL_FACE);

    vl::ref<vl::Effect> fx2 = new vl::Effect;
    fx2->shader()->enable(vl::EN_LIGHTING);
    fx2->shader()->gocMaterial()->setFlatColor(vlut::green);
    fx2->shader()->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
    fx2->shader()->gocLineWidth()->set(2.0f);
    fx2->shader()->enable(vl::EN_CULL_FACE);

    vl::ref<vl::Effect> fx3 = new vl::Effect;
    fx3->shader()->enable(vl::EN_LIGHTING);
    fx3->shader()->gocMaterial()->setFlatColor(vlut::blue);
    fx3->shader()->gocPolygonMode()->set(vl::PM_LINE,vl::PM_LINE);
    fx3->shader()->gocLineWidth()->set(2.0f);
    fx3->shader()->enable(vl::EN_CULL_FACE);

    // setup the effect override masks
    // note that we can override the Effect at the vl::Rendering level but
    // we can also override the single Shader at the vl::Renderer level, see App_ShaderOverride.hpp

    mWireRendering->effectOverrideMask()[0x01] = fx1;
    mWireRendering->effectOverrideMask()[0x02] = fx2;
    mWireRendering->effectOverrideMask()[0x04] = fx3;

    ball1->setEnableMask(0x01);
    ball2->setEnableMask(0x02);
    ball3->setEnableMask(0x04);
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

  void resizeEvent(int w, int h)
  {
    // note that the camera is shared by both the solid and wire rendering.
    vl::Camera* camera = mSolidRendering->camera();
    camera->viewport()->setWidth ( w );
    camera->viewport()->setHeight( h );
    camera->setProjectionAsPerspective();
  }

protected:
  vl::ref<vl::Rendering> mSolidRendering;
  vl::ref<vl::Rendering> mWireRendering;
  vl::ref<vl::Transform> mCubeTransform1;
  vl::ref<vl::Transform> mCubeTransform2;
  vl::ref<vl::Transform> mCubeTransform3;
};

// Have fun!

BaseDemo* Create_App_EffectOverride() { return new App_EffectOverride; }
