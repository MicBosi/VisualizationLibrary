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
#include "vlGraphics/Light.hpp"

class App_RenderOrder: public BaseDemo
{
public:
  App_RenderOrder(int test): mTestNumber(test) {}

  void shutdown() {}

  void run()
  {
    if (mTestNumber == 0)
    {
      mTransform1->setLocalMatrix( vl::mat4::getRotation(vl::Time::currentTime()*45,    0, 1, 0 ) );
      mTransform2->setLocalMatrix( vl::mat4::getRotation(vl::Time::currentTime()*45-60, 0, 1, 0 ) );
      mTransform3->setLocalMatrix( vl::mat4::getRotation(vl::Time::currentTime()*45+60, 0, 1, 0 ) );
    }
    else
    {
      mTransform1->setLocalMatrix( vl::mat4::getRotation(vl::Time::currentTime()*45,    0, 1, 0 ) );
      mTransform2->setLocalMatrix( vl::mat4::getRotation(vl::Time::currentTime()*45+90, 0, 1, 0 ) );
    }
  }

  void initEvent()
  {
    BaseDemo::initEvent();

    vl::ref<vl::Light> light = new vl::Light(0);

    mTransform1 = new vl::Transform;
    vl::defRendering()->as<vl::Rendering>()->transform()->addChild(mTransform1.get());
    mTransform2 = new vl::Transform;
    vl::defRendering()->as<vl::Rendering>()->transform()->addChild(mTransform2.get());
    mTransform3 = new vl::Transform;
    vl::defRendering()->as<vl::Rendering>()->transform()->addChild(mTransform3.get());

    vl::ref<vl::Geometry> box    = vl::makeBox(vl::vec3(0,0,-2), 1,1,1);
    vl::ref<vl::Geometry> sphere = vl::makeUVSphere(vl::vec3(0,0,0),0.5f);
    vl::ref<vl::Geometry> cone   = vl::makeCone(vl::vec3(0,0,+2), 1, 1, 10, true);
    box   ->computeNormals();
    sphere->computeNormals();
    cone  ->computeNormals();

    if (mTestNumber == 0)
    {
      // rendering order: 
      // red -> yellow
      // box -> sphere -> cone

      vl::ref<vl::Effect> red_fx = new vl::Effect;
      red_fx->shader()->disable(vl::EN_DEPTH_TEST);
      red_fx->shader()->enable(vl::EN_CULL_FACE);
      red_fx->shader()->enable(vl::EN_LIGHTING);
      red_fx->shader()->setRenderState( light.get() );
      red_fx->shader()->gocMaterial()->setDiffuse(vl::red);
      red_fx->setRenderRank(1);

      vl::ref<vl::Actor> act;
      act = sceneManager()->tree()->addActor( box.get(), red_fx.get(), mTransform1.get() );
      act->setRenderRank( 10 );
      act = sceneManager()->tree()->addActor( sphere.get(), red_fx.get(), mTransform1.get() );
      act->setRenderRank( 20 );
      act = sceneManager()->tree()->addActor( cone.get(), red_fx.get(), mTransform1.get() );
      act->setRenderRank( 30 );

      vl::ref<vl::Effect> yellow_fx = new vl::Effect;
      yellow_fx->shader()->disable(vl::EN_DEPTH_TEST);
      yellow_fx->shader()->enable(vl::EN_CULL_FACE);
      yellow_fx->shader()->enable(vl::EN_LIGHTING);
      yellow_fx->shader()->setRenderState( light.get() );
      yellow_fx->shader()->gocMaterial()->setDiffuse(vl::yellow);
      yellow_fx->setRenderRank(2);

      act = sceneManager()->tree()->addActor( box.get(),  yellow_fx.get(), mTransform2.get() );
      act->setRenderRank( 10 );
      act = sceneManager()->tree()->addActor( cone.get(), yellow_fx.get(), mTransform2.get() );
      act->setRenderRank( 20 );
    }
    else
    {
      /* transp_fx */

      vl::ref<vl::Effect> transp_fx = new vl::Effect;
      transp_fx->shader()->enable(vl::EN_BLEND);
      transp_fx->shader()->enable(vl::EN_DEPTH_TEST);
      transp_fx->shader()->enable(vl::EN_CULL_FACE);
      transp_fx->shader()->enable(vl::EN_LIGHTING);
      transp_fx->shader()->setRenderState( light.get() );
      transp_fx->shader()->gocMaterial()->setDiffuse(vl::blue);
      transp_fx->shader()->gocMaterial()->setTransparency(0.5f);

      vl::ref<vl::Actor> act;
      act = sceneManager()->tree()->addActor( box.get(),    transp_fx.get(), mTransform1.get() );
      act = sceneManager()->tree()->addActor( sphere.get(), transp_fx.get(), mTransform1.get() );
      act = sceneManager()->tree()->addActor( cone.get(),   transp_fx.get(), mTransform1.get() );

      /* solid_fx */

      vl::ref<vl::Effect> solid_fx = new vl::Effect;
      solid_fx->shader()->enable(vl::EN_DEPTH_TEST);
      solid_fx->shader()->enable(vl::EN_CULL_FACE);
      solid_fx->shader()->enable(vl::EN_LIGHTING);
      solid_fx->shader()->setRenderState( light.get() );
      solid_fx->shader()->gocMaterial()->setDiffuse(vl::yellow);

      act = sceneManager()->tree()->addActor( box.get(),  solid_fx.get(), mTransform2.get() );
      act = sceneManager()->tree()->addActor( cone.get(), solid_fx.get(), mTransform2.get() );

      if (mTestNumber == 1)
      {
        // depth-sort only alpha blended objects (default settings)
        vl::ref<vl::RenderQueueSorterStandard> list_sorter = new vl::RenderQueueSorterStandard;
        list_sorter->setDepthSortMode(vl::AlphaDepthSort);
        vl::defRendering()->as<vl::Rendering>()->setRenderQueueSorter( list_sorter.get() );
      }
      else
      if (mTestNumber == 2)
      {
        // depth-sort solid and alpha blended objects
        vl::ref<vl::RenderQueueSorterStandard> list_sorter = new vl::RenderQueueSorterStandard;
        list_sorter->setDepthSortMode(vl::AlwaysDepthSort);
        vl::defRendering()->as<vl::Rendering>()->setRenderQueueSorter( list_sorter.get() );
      }
      else
      if (mTestNumber == 3)
      {
        // depth-sort alpha blended back to front
        // depth-sort solid object front to back
        vl::ref<vl::RenderQueueSorterOcclusion> list_sorter = new vl::RenderQueueSorterOcclusion;
        vl::defRendering()->as<vl::Rendering>()->setRenderQueueSorter( list_sorter.get() );
      }
    }
  }
public:
  vl::ref<vl::Transform> mTransform1;
  vl::ref<vl::Transform> mTransform2;
  vl::ref<vl::Transform> mTransform3;
  int mTestNumber;
};

// Have fun!

BaseDemo* Create_App_RenderOrder(int test) { return new App_RenderOrder(test); }
