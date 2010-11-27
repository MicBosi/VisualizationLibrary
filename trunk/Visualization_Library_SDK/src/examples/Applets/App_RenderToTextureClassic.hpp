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

#include "vl/CopyTexSubImage.hpp"
#include "vl/Rendering.hpp"
#include "vl/RenderingTree.hpp"
#include "vl/SceneManager.hpp"
#include "vl/Light.hpp"

class App_RenderToTextureClassic: public BaseDemo
{
public:
  virtual void shutdown() {}

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    VL_CHECK_OGL()

    // save render target for later
    vl::ref<vl::RenderTarget> render_target = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->renderTarget();
    // install new rendering
    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    vl::VisualizationLibrary::setRendering(render_tree.get());
    mMainRendering = new vl::Rendering;
    mRTT_Rendering = new vl::Rendering;
    render_tree->subRenderings()->push_back(mRTT_Rendering.get());
    render_tree->subRenderings()->push_back(mMainRendering.get());
    mMainRendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mRTT_Rendering->sceneManagers()->push_back(new vl::SceneManagerActorTree);
    mMainRendering->renderer()->setRenderTarget( render_target.get() );
    mRTT_Rendering->renderer()->setRenderTarget( render_target.get() );

    mRTT_Rendering->camera()->viewport()->setClearColor( vlut::crimson );
    mRTT_Rendering->camera()->viewport()->setX(0);
    mRTT_Rendering->camera()->viewport()->setY(0);
    mRTT_Rendering->camera()->viewport()->setWidth (512);
    mRTT_Rendering->camera()->viewport()->setHeight(512);
    mRTT_Rendering->camera()->setProjectionAsPerspective();
    vl::mat4 m = vl::mat4::lookAt( vl::vec3(0,0,10.5f), vl::vec3(0,0,0), vl::vec3(0,1,0) );
    mRTT_Rendering->camera()->setInverseViewMatrix( m );

    VL_CHECK_OGL()

    vl::ref<vl::Texture> texture = new vl::Texture( 512, 512, vl::TF_RGBA, false );
    vl::ref<vl::CopyTexSubImage2D> copytex = new vl::CopyTexSubImage2D( 0, 0,0, 0,0, 512,512, texture.get() );
    mRTT_Rendering->onFinishedCallbacks()->push_back( copytex.get() );

    mMainRendering->camera()->viewport()->setClearColor( vlut::midnightblue );

    mMainRendering->camera()->viewport()->set(0,0,512,512);
    mMainRendering->camera()->setProjectionAsPerspective();
    m = vl::mat4::lookAt(vl::vec3(0,15,25), vl::vec3(0,0,0), vl::vec3(0,1,0));
    mMainRendering->camera()->setInverseViewMatrix( m );

    addCube(texture.get());
    addRings();

    bindManipulators( mMainRendering.get() );
  }

  void addCube(vl::Texture* texture)
  {
    vl::ref<vl::Effect> effect = new vl::Effect;
    vl::ref<vl::Light> light = new vl::Light(0);
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    effect->shader()->setRenderState( light.get() );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->enable(vl::EN_CULL_FACE);
    effect->shader()->gocTextureUnit(0)->setTexture( texture );
    effect->shader()->gocTexEnv(0)->setMode(vl::TEM_MODULATE);

    vl::Real size = 50;
    vl::ref<vl::Geometry> ground;
    ground= vlut::makeGrid( vl::vec3(0,0,0), size*1.1f, size*1.1f, 2, 2, true, vl::fvec2(0,0), vl::fvec2(1,1) );
    ground->computeNormals();
    mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ground.get(), effect.get() );

    vl::ref<vl::Geometry> box;
    box= vlut::makeBox( vl::vec3(0,5,0), 10,10,10);
    box->computeNormals();
    mMainRendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( box.get(), effect.get() );
  }
  void addRings()
  {
    vl::ref<vl::Effect> effect = new vl::Effect;
    vl::ref<vl::Light> light = new vl::Light(0);
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    effect->shader()->setRenderState( light.get() );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->enable(vl::EN_CULL_FACE);
    effect->shader()->gocMaterial()->setDiffuse( vlut::yellow );

    tr_ring1 = new vl::Transform;
    vl::ref<vl::Geometry> ring1;
    ring1= vlut::makeTorus( vl::vec3(0,0,0), 10,0.5, 20,100);
    ring1->computeNormals();
    mRTT_Rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ring1.get(), effect.get(), tr_ring1.get() );

    tr_ring2 = new vl::Transform;
    vl::ref<vl::Geometry> ring2;
    ring2= vlut::makeTorus( vl::vec3(0,0,0), 9,0.5, 20,100);
    ring2->computeNormals();
    mRTT_Rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ring2.get(), effect.get(), tr_ring2.get() );

    tr_ring3 = new vl::Transform;
    vl::ref<vl::Geometry> ring3;
    ring3= vlut::makeTorus( vl::vec3(0,0,0), 8,0.5, 20,100);
    ring3->computeNormals();
    mRTT_Rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ring3.get(), effect.get(), tr_ring3.get() );

    tr_ring4 = new vl::Transform;
    vl::ref<vl::Geometry> ring4;
    ring4= vlut::makeTorus( vl::vec3(0,0,0), 7,0.5, 20,100);
    ring4->computeNormals();
    mRTT_Rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ring4.get(), effect.get(), tr_ring4.get() );

    tr_ring5 = new vl::Transform;
    vl::ref<vl::Geometry> ring5;
    ring5= vlut::makeTorus( vl::vec3(0,0,0), 6,0.5, 20,100);
    ring5->computeNormals();
    mRTT_Rendering->sceneManagers()->at(0)->as<vl::SceneManagerActorTree>()->tree()->addActor( ring5.get(), effect.get(), tr_ring5.get() );

    mRTT_Rendering->transform()->addChild(tr_ring1.get());
    tr_ring1->addChild(tr_ring2.get());
    tr_ring2->addChild(tr_ring3.get());
    tr_ring3->addChild(tr_ring4.get());
    tr_ring4->addChild(tr_ring5.get());
  }

  vl::Real x,y,x2,y2,x3;
  vl::ref<vl::Transform> tr_ring1;
  vl::ref<vl::Transform> tr_ring2;
  vl::ref<vl::Transform> tr_ring3;
  vl::ref<vl::Transform> tr_ring4;
  vl::ref<vl::Transform> tr_ring5;

  virtual void run()
  {
    x =  vl::Time::currentTime() * 45*2;
    y =  vl::Time::currentTime() * 45*2.1f;
    x2 = vl::Time::currentTime() * 45*2.2f;
    y2 = vl::Time::currentTime() * 45*2.3f;
    x3 = vl::Time::currentTime() * 45*2.4f;

    tr_ring1->setLocalMatrix( vl::mat4::rotation(x,  1,0,0) );
    tr_ring2->setLocalMatrix( vl::mat4::rotation(y,  0,1,0) );
    tr_ring3->setLocalMatrix( vl::mat4::rotation(x2, 1,0,0) );
    tr_ring4->setLocalMatrix( vl::mat4::rotation(y2, 0,1,0) );
    tr_ring5->setLocalMatrix( vl::mat4::rotation(x3, 1,0,0) );
  }

  void resizeEvent(int w, int h)
  {
    vl::Camera* camera = mMainRendering->camera();
    camera->viewport()->setWidth ( w );
    camera->viewport()->setHeight( h );
    camera->setProjectionAsPerspective();
  }

protected:
  vl::ref<vl::Rendering> mMainRendering;
  vl::ref<vl::Rendering> mRTT_Rendering;
};

// Have fun!
