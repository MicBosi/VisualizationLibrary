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
#include "vlut/Colors.hpp"
#include "vlCore/OcclusionCullRenderer.hpp"
#include "vlCore/Text.hpp"
#include "vlCore/Light.hpp"
#include "vlCore/FontManager.hpp"

class App_OcclusionCulling: public BaseDemo
{
public:
  void initEvent()
  {
    BaseDemo::initEvent();

    // #######################################################################
    // # These 4 lines are the only code needed to enable occlusion culling, #
    // # no special sorter or render rank/block setup needed!                #
    // #######################################################################

    // wraps the regular renderer inside the occlusion renderer
    vl::Renderer* regular_renderer = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer();
    // creates our occlusion renderer
    mOcclusionRenderer = new vl::OcclusionCullRenderer;
    mOcclusionRenderer->setWrappedRenderer( regular_renderer );
    // installs the occlusion renderer in place of the regular one
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->setRenderer( mOcclusionRenderer.get() );

    // note: to disable occlusion culling just restore the 'regular_renderer' as we do below in 'keyPressEvent()'

    populateScene();
  }

  /* populates the scene with tree-like actors */
  void populateScene()
  {
    /* the rest of the code simply generates a forest of thousands of trees */

    /* setup a simple effect to use for all our objects */
    vl::ref<vl::Effect> fx = new vl::Effect;
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->gocMaterial()->setColorMaterialEnabled(true);
    fx->shader()->gocLight(0)->setLinearAttenuation(0.0025f);

    /* the ground under the trees */
    float side = 400;
    vl::ref<vl::Geometry> ground = vl::makeGrid(vl::vec3(0, -1.0f, 0), side*2.1f, side*2.1f, (int)side, (int)side);
    ground->computeNormals();
    ground->setColor(vl::green);
    sceneManager()->tree()->addActor(ground.get(), fx.get(), NULL);

    /* the red wall in front of the camera */
    vl::ref<vl::Geometry> wall = vl::makeBox(vl::vec3(0,25,500), 50, 50 ,1);
    wall->computeNormals();
    wall->setColor(vl::red);
    sceneManager()->tree()->addActor(wall.get(), fx.get(), NULL);

    /* the trees */
    float trunk_h   = 20;
    float trunk_w   = 4;
    /* the tree's branches */
    vl::ref<vl::Geometry> branches = vl::makeIcosphere(vl::vec3(0,trunk_h/2.0f,0), 14, 2, false);
    branches->computeNormals();
    branches->setColor( vl::green );
    /* the tree's trunk */
    vl::ref<vl::Geometry> trunk = vl::makeCylinder(vl::vec3(0,0,0),trunk_w,trunk_h, 50, 50);
    trunk->computeNormals();
    trunk->setColor( vl::gold );

    /* fill our forest with trees! */
    int trunk_count = 20;
    for(int i=-trunk_count; i<=trunk_count; ++i)
    for(int j=-trunk_count; j<=trunk_count; ++j)
    {
      float x = (float) i * side / trunk_count;
      float z = (float) j * side / trunk_count;

      vl::ref<vl::Transform> tr = new vl::Transform( vl::mat4::getTranslation(x,trunk_h/2.0f+0.1f,z) );
      tr->computeWorldMatrix();
      sceneManager()->tree()->addActor(trunk.get(), fx.get(), tr.get());
      sceneManager()->tree()->addActor(branches.get(), fx.get(), tr.get());
    }

    /* text statistics */
    mText = new vl::Text;
    mText->setText("*** N/A ***");
    mText->setFont( vl::VisualizationLibrary::fontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 10) );
    mText->setAlignment( vl::AlignLeft | vl::AlignTop );
    mText->setViewportAlignment( vl::AlignLeft | vl::AlignTop );
    mText->setTextAlignment(vl::TextAlignLeft);
    mText->translate(+5,-5,0);
    mText->setColor(vl::white);
    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    vl::Actor* text_actor = sceneManager()->tree()->addActor(mText.get(), effect.get());
    text_actor->setOccludee(false);

    /* start stats timer */
    mTimer.start();

    /* occlusion culling enable flag */
    mOcclusionCullingOn = true;
  }

  void updateText()
  {
    if (mOcclusionRenderer)
    {
      vl::String msg = vl::Say("Occlusion ratio = %.1n%% (%n/%n)\n") 
        << 100.0f * mOcclusionRenderer->statsOccludedObjects() / mOcclusionRenderer->statsTotalObjects() 
        << mOcclusionRenderer->statsTotalObjects() - mOcclusionRenderer->statsOccludedObjects() 
        << mOcclusionRenderer->statsTotalObjects();
      mText->setText( msg );
    }
  }

  void runEvent()
  {
    BaseDemo::runEvent();

    /* update text every 0.5 secs */
    if( mTimer.elapsed() > 0.5f && mOcclusionCullingOn )
    {
      updateText();
      mTimer.start();
    }
  }

  /* spacebar = toggles occlusion culling */
  void keyPressEvent(unsigned short ch, vl::EKey key)
  {
    BaseDemo::keyPressEvent(ch, key);
    if (key == vl::Key_Space)
    {
      mOcclusionCullingOn = !mOcclusionCullingOn;
      if (mOcclusionCullingOn)
      {
        vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->setRenderer( mOcclusionRenderer.get() );
      }
      else
      {
        vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->setRenderer( mOcclusionRenderer->wrappedRenderer() );
        mText->setText("Occlusion Culling Off");
      }
    }
  }

protected:
  vl::ref<vl::OcclusionCullRenderer> mOcclusionRenderer;
  bool mOcclusionCullingOn;
  vl::ref<vl::Text> mText;
  vl::Time mTimer;
};

// Have fun!

BaseDemo* Create_App_OcclusionCulling() { return new App_OcclusionCulling; }
