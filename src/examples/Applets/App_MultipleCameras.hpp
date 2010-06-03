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

#include "vl/SceneManager.hpp"

class App_MultipleCameras: public BaseDemo
{
public:
  virtual void shutdown() {}

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    // save to be used later
    vl::ref<vl::Renderer> renderer = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer();
    vl::ref<vl::RenderTarget> render_target = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderTarget();
    // install new rendering tree
    mRenderingTree = new vl::RenderingTree;
    vl::VisualizationLibrary::setRendering(mRenderingTree.get());
    mRendering0 = new vl::Rendering;
    mRendering1 = new vl::Rendering;
    mRendering2 = new vl::Rendering;
    mRendering3 = new vl::Rendering;
    mRenderingTree->subRenderings()->push_back(mRendering0.get());
    mRenderingTree->subRenderings()->push_back(mRendering1.get());
    mRenderingTree->subRenderings()->push_back(mRendering2.get());
    mRenderingTree->subRenderings()->push_back(mRendering3.get());

    _tr1 = new vl::Transform;
    _tr2 = new vl::Transform;
    _tr1->addChild( _tr2.get() );

    vl::ref<vl::ResourceDatabase> res_db = vl::loadResource("/models/3ds/monkey.3ds");
    if (!res_db)
      return;

    vl::ref<vl::Light> light = new vl::Light(0);
    light->setAmbient( vl::fvec4( .1f, .1f, .1f, 1.0f) );
    light->setSpecular( vl::fvec4( .1f, .1f, .1f, 1.0f) );

    for(unsigned i=0; i<res_db->count<vl::Actor>(); ++i)
    {
      res_db->get<vl::Actor>(i)->effect()->shader()->setRenderState( light.get() );
      res_db->get<vl::Actor>(i)->effect()->shader()->enable(vl::EN_LIGHTING);
      res_db->get<vl::Actor>(i)->effect()->shader()->enable(vl::EN_DEPTH_TEST);
    }

    vl::ref<vl::Effect> wirefx = new vl::Effect;
    wirefx->shader()->disable( vl::EN_CULL_FACE );
    wirefx->shader()->enable( vl::EN_LIGHTING );
    wirefx->shader()->enable( vl::EN_DEPTH_TEST );
    wirefx->shader()->enable( vl::EN_LINE_SMOOTH );
    wirefx->shader()->enable( vl::EN_BLEND );
    wirefx->shader()->gocLightModel()->setTwoSide(true);
    wirefx->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
    wirefx->shader()->gocMaterial()->setDiffuse( vlut::white );
    wirefx->shader()->setRenderState( light.get() );

    std::vector< vl::ref<vl::Actor> > moneky_w;

    moneky_w.resize( res_db->count<vl::Actor>() );
    for(unsigned i=0; i<res_db->count<vl::Actor>(); ++i)
    {
      moneky_w[i] = new vl::Actor( *res_db->get<vl::Actor>(i) );
      moneky_w[i]->setEffect(wirefx.get());
    }

    for( int i=0; i<mRenderingTree->subRenderings()->size(); ++i )
    {
      mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->setRenderTarget( render_target.get() );
      mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->setRenderer( renderer.get() );
      mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->setCamera( new vl::Camera );
      mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->setTransform( _tr1.get() );
      vl::ref<vl::SceneManagerActorTree> scene_manager = new vl::SceneManagerActorTree;
      mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->sceneManagers()->push_back( scene_manager.get() );

      switch(i)
      {
        case 0:
          for(unsigned j=0; j<res_db->count<vl::Actor>(); ++j)
          {
            scene_manager->tree()->addActor( res_db->get<vl::Actor>(j) );
            res_db->get<vl::Actor>(j)->setTransform( _tr2.get() );
          }
          break;
        case 1:
        case 2:
        case 3:
          for(unsigned j=0; j<moneky_w.size(); ++j)
          {
            scene_manager->tree()->addActor( moneky_w[j].get() );
            moneky_w[j]->setTransform( _tr2.get() );
          }
      }

      switch(i)
      {
      case 0: mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->camera()->viewport()->setClearColor(vlut::black); break;
      case 1: mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->camera()->viewport()->setClearColor(vlut::yellow); break;
      case 2: mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->camera()->viewport()->setClearColor(vlut::red); break;
      case 3: mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->camera()->viewport()->setClearColor(vlut::green); break;
      }

      vl::mat4 m;
      switch(i)
      {
      case 0: m = vl::mat4::lookAt( vl::vec3(0,1,3.5f), vl::vec3(0,0,0), vl::vec3(0,1,0) ); break;
      case 1: m = vl::mat4::lookAt( vl::vec3(0,1,3.5f), vl::vec3(0,0,0), vl::vec3(0,1,0) ); break;
      case 2: m = vl::mat4::lookAt( vl::vec3(3.5,1,0), vl::vec3(0,0,0), vl::vec3(0,1,0) ); break;
      case 3: m = vl::mat4::lookAt( vl::vec3(0,3.5,0), vl::vec3(0,0,0), vl::vec3(0,0,-1) ); break;
      }
      mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->camera()->setInverseViewMatrix(m);
    }
  }

  virtual void run()
  {
    _tr1->setLocalMatrix( vl::mat4::rotation(vl::Time::currentTime()*45,0,1,0) );
  }

  virtual void resizeEvent(int w, int h)
  {
    if ( mRenderingTree->subRenderings()->size() < 4 )
      return;

    int hw = w/2;
    int hh = h/2;

    mRenderingTree->subRenderings()->at(2)->as<vl::Rendering>()->renderTarget()->setWidth(w);
    mRenderingTree->subRenderings()->at(2)->as<vl::Rendering>()->renderTarget()->setHeight(h);
    mRenderingTree->subRenderings()->at(2)->as<vl::Rendering>()->camera()->viewport()->set(0,0,hw,hh);
    mRenderingTree->subRenderings()->at(2)->as<vl::Rendering>()->camera()->setProjectionAsPerspective();

    mRenderingTree->subRenderings()->at(1)->as<vl::Rendering>()->renderTarget()->setWidth(w);
    mRenderingTree->subRenderings()->at(1)->as<vl::Rendering>()->renderTarget()->setHeight(h);
    mRenderingTree->subRenderings()->at(1)->as<vl::Rendering>()->camera()->viewport()->set(hw,hh,w-hw,h-hh);
    mRenderingTree->subRenderings()->at(1)->as<vl::Rendering>()->camera()->setProjectionAsPerspective();

    mRenderingTree->subRenderings()->at(0)->as<vl::Rendering>()->renderTarget()->setWidth(w);
    mRenderingTree->subRenderings()->at(0)->as<vl::Rendering>()->renderTarget()->setHeight(h);
    mRenderingTree->subRenderings()->at(0)->as<vl::Rendering>()->camera()->viewport()->set(0,hh,hw,h-hh);
    mRenderingTree->subRenderings()->at(0)->as<vl::Rendering>()->camera()->setProjectionAsPerspective();

    mRenderingTree->subRenderings()->at(3)->as<vl::Rendering>()->renderTarget()->setWidth(w);
    mRenderingTree->subRenderings()->at(3)->as<vl::Rendering>()->renderTarget()->setHeight(h);
    mRenderingTree->subRenderings()->at(3)->as<vl::Rendering>()->camera()->viewport()->set(hw,0,w-hw,hh);
    mRenderingTree->subRenderings()->at(3)->as<vl::Rendering>()->camera()->setProjectionAsPerspective();

    bindManipulators( mRenderingTree->subRenderings()->at(0)->as<vl::Rendering>() );
  }

  void mouseDownEvent(vl::EMouseButton, int x, int y)
  {
    for( int i=0; i<mRenderingTree->subRenderings()->size(); ++i )
    {
      int height = mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->renderTarget()->height();
      if ( mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>()->camera()->viewport()->isPointInside(x,y,height) )
      {
        bindManipulators( mRenderingTree->subRenderings()->at(i)->as<vl::Rendering>() );
         trackball()->setTransform( _tr2.get() );
        break;
      }
    }
  }

protected:
  vl::ref<vl::Transform> _tr1;
  vl::ref<vl::Transform> _tr2;
  vl::ref<vl::RenderingTree> mRenderingTree;
  vl::ref<vl::Rendering> mRendering0;
  vl::ref<vl::Rendering> mRendering1;
  vl::ref<vl::Rendering> mRendering2;
  vl::ref<vl::Rendering> mRendering3;
};

// Have fun!
