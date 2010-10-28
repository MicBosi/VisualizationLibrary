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

class App_OcclusionCulling: public BaseDemo
{
public:
  void initEvent()
  {
    BaseDemo::initEvent();

    // #######################################################################
    // # These 2 lines are the only code needed to enable occlusion culling! #
    // #######################################################################
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->setOcclusionCullingEnabled(true);
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->setRenderQueueSorter(new vl::RenderQueueSorterOcclusion);

    /* the rest of the code simply generates a forest of thousands of trees */

    /* setup a simple effect to use for all our objects */
    vl::ref<vl::Effect> fx = new vl::Effect;
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->gocMaterial()->setColorMaterialEnabled(true);
    fx->shader()->gocLight(0)->setLinearAttenuation(0.0025f);

    /* the ground under the trees */
    float side = 400;
    vl::ref<vl::Geometry> ground = vlut::makeGrid(vl::vec3(0,0,0), side*2.0f,side*2.0f, (int)side, (int)side);
    ground->computeNormals();
    ground->setColorArray(vlut::green);
    vl::Actor* ground_act = sceneManager()->tree()->addActor(ground.get(), fx.get(), NULL);
    /* assign a render rank of -1 (default is 0) to be sure to render the ground before all the trees */
    ground_act->setRenderRank(-1);

    /* the red wall in front of the camera */
    vl::ref<vl::Geometry> wall = vlut::makeBox(vl::vec3(0,25,500), 50,50,1);
    wall->computeNormals();
    wall->setColorArray(vlut::red);
    sceneManager()->tree()->addActor(wall.get(), fx.get(), NULL);

    /* the trees */
    float trunk_h   = 20;
    float trunk_w   = 4;
    /* the tree's branches */
    vl::ref<vl::Geometry> branches = vlut::makeIcosphere(vl::vec3(0,trunk_h/2.0f,0), 7, 1, false);
    branches->computeNormals();
    branches->setColorArray( vlut::green );
    /* the tree's trunk */
    vl::ref<vl::Geometry> trunk = vlut::makeCylinder(vl::vec3(0,0,0),trunk_w,trunk_h, 35, 35);
    trunk->computeNormals();
    trunk->setColorArray( vlut::gold );

    /* fill our forest with trees! */
    int trunk_count = 20;
    for(int i=-trunk_count; i<=trunk_count; ++i)
    for(int j=-trunk_count; j<=trunk_count; ++j)
    {
      float x = (float) i * side / trunk_count;
      float z = (float) j * side / trunk_count;

      vl::ref<vl::Transform> tr = new vl::Transform( vl::mat4::translation(x,trunk_h/2.0f+0.1f,z) );
      tr->computeWorldMatrix();
      sceneManager()->tree()->addActor(trunk.get(), fx.get(), tr.get());
      sceneManager()->tree()->addActor(branches.get(), fx.get(), tr.get());
    }
  }

  /* spacebar = toggles occlusion culling */
  void keyPressEvent(unsigned short ch, vl::EKey key)
  {
    BaseDemo::keyPressEvent(ch, key);
    if (key == vl::Key_Space)
    {
      bool on = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->occlusionCullingEnabled();
      vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->setOcclusionCullingEnabled(!on);
      vl::Log::print( vl::Say("Occlusion = %s\n") << (vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->occlusionCullingEnabled() ? "On" : "Off") );
    }
  }
};

// Have fun!
