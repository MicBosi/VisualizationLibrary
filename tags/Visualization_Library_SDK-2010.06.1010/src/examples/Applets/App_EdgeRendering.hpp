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
#include "vl/EdgeExtractor.hpp"
#include "vl/EdgeRenderer.hpp"
#include "vl/LoadWriterManager.hpp"

class App_EdgeRendering: public BaseDemo
{
public:
  void setupScene()
  {
    // setup common states
    vl::ref<vl::Light> camera_light = new vl::Light(0);
    vl::ref<vl::EnableSet> enables = new vl::EnableSet;
    enables->enable(vl::EN_DEPTH_TEST);
    enables->enable(vl::EN_LIGHTING);

    // red material fx
    vl::ref<vl::Effect> red_fx = new vl::Effect;
    red_fx->shader()->setEnableSet(enables.get());
    red_fx->shader()->gocMaterial()->setDiffuse(vlut::red);
    red_fx->shader()->setRenderState(camera_light.get());

    // green material fx
    vl::ref<vl::Effect> green_fx = new vl::Effect;
    green_fx->shader()->setEnableSet(enables.get());
    green_fx->shader()->gocMaterial()->setDiffuse(vlut::green);
    green_fx->shader()->setRenderState(camera_light.get());

    // blue material fx
    vl::ref<vl::Effect> yellow_fx = new vl::Effect;
    yellow_fx->shader()->setEnableSet(enables.get());
    yellow_fx->shader()->gocMaterial()->setDiffuse(vlut::yellow);
    yellow_fx->shader()->setRenderState(camera_light.get());

    // add box, cylinder, cone actors to the scene
    vl::ref<vl::Geometry> geom1 = vlut::makeBox     (vl::vec3(-7,0,0),5,5,5);
    vl::ref<vl::Geometry> geom2 = vlut::makeCylinder(vl::vec3(0,0,0), 5,5, 10,2, true, true);
    vl::ref<vl::Geometry> geom3 = vlut::makeCone    (vl::vec3(+7,0,0),5,5, 20, true);

    // needed since we enabled the lighting
    geom1->computeNormals();
    geom2->computeNormals();
    geom3->computeNormals();

    // add the actors to the scene
    mSceneManager->tree()->addActor( geom1.get(), red_fx.get(),    mSolidRendering->transform() );
    mSceneManager->tree()->addActor( geom2.get(), green_fx.get(),  mSolidRendering->transform() );
    mSceneManager->tree()->addActor( geom3.get(), yellow_fx.get(), mSolidRendering->transform() );
  }

  void initEvent()
  {
    BaseDemo::initEvent();

    // initialize our renderings to use the camera, the scene-manager, rendering target etc. from the default renderingTree()
    mSolidRendering = new vl::Rendering;
    *mSolidRendering = *(vl::VisualizationLibrary::rendering()->as<vl::Rendering>());
    mEdgeRendering  = new vl::Rendering;
    *mEdgeRendering  = *(vl::VisualizationLibrary::rendering()->as<vl::Rendering>());

    /*
    Outline of our rendering tree:
    
                renderingTree() <- empty rendering
                  /   \
                 /     \          
    mSolidRendering   mEdgeRendering  <- first calls the solid rendering, then calls the edge rendering
    */

    // install the new rendering tree
    vl::ref<vl::RenderingTree> render_tree = new vl::RenderingTree;
    vl::VisualizationLibrary::setRendering(render_tree.get());
    render_tree->subRenderings()->push_back(mSolidRendering.get());
    render_tree->subRenderings()->push_back(mEdgeRendering.get());

    // bind the scene manager containing the actors to be rendered to both the solid and the edge rendering
    mSceneManager = new vl::SceneManagerActorTree;
    mSolidRendering->sceneManagers()->push_back(mSceneManager.get());
    mEdgeRendering ->sceneManagers()->push_back(mSceneManager.get());

    // bind an vl::EdgeRenderer to the mEdgeRendering in order to perform automatic edge extraction and rendering.
    // we set the clear flags to be vl::CF_CLEAR_DEPTH (by default is set to CF_CLEAR_COLOR_DEPTH) because when the 
    // wireframe rendering starts we want to preserve the color-buffer as generated by the solid rendering but we 
    // want to clear the Z-buffer as it is needed by the hidden-line-removal algorithm implemented by vl::EdgeRenderer.
    mEdgeRenderer = new vl::EdgeRenderer;
    mEdgeRendering->setRenderer( mEdgeRenderer.get() );
    mEdgeRendering->setClearFlags(vl::CF_CLEAR_DEPTH);

    // hidden line and crease options
    mEdgeRenderer->setShowHiddenLines(true);
    mEdgeRenderer->setShowCreases(true);
    mEdgeRenderer->setCreaseAngle(35.0f);

    // style options
    mEdgeRenderer->setLineWidth(2.0f);
    mEdgeRenderer->setSmoothLines(true);
    mEdgeRenderer->setDefaultLineColor(vlut::black);

    // fills mSceneManager with a few actors.
    // the beauty of this system is that you setup your actors ony once in a single scene managers and
    // they will be rendered twice, first using a normal renderer and then using the wireframe renderer.
    setupScene();
  }

  // user controls:
  // '1' = edge rendering off.
  // '2' = edge rendering on: silhouette only.
  // '3' = edge rendering on: silhouette + creases.
  // '4' = edge rendering on: silhouette + creases + hidden lines.
  // '5' = hidden line removal wireframe: silhouette + creases.
  // '6' = hidden line removal wireframe: silhouette + creases + hidden lines.
  void keyPressEvent(unsigned short ch, vl::EKey key)
  {
    BaseDemo::keyPressEvent(ch, key);

    if (ch == '1')
    {
      mSolidRendering->setEnableMask(0xFFFFFFFF);
      mEdgeRendering->setEnableMask(0);
      vl::Log::print("Edge rendering disabled.\n");
    }
    else
    if (ch == '2')
    {
      mSolidRendering->setEnableMask(0xFFFFFFFF);
      // preserve color buffer, clear depth buffer
      mEdgeRendering->setClearFlags(vl::CF_CLEAR_DEPTH);
      mEdgeRendering->setEnableMask(0xFFFFFFFF);
      mEdgeRenderer->setShowCreases(false);
      mEdgeRenderer->setShowHiddenLines(false);
      vl::Log::print("Edge rendering enabled. Creases = off, hidden lines = off.\n");
    }
    else
    if (ch == '3')
    {
      mSolidRendering->setEnableMask(0xFFFFFFFF);
      // preserve color buffer, clear depth buffer
      mEdgeRendering->setClearFlags(vl::CF_CLEAR_DEPTH);
      mEdgeRendering->setEnableMask(0xFFFFFFFF);
      mEdgeRenderer->setShowCreases(true);
      mEdgeRenderer->setShowHiddenLines(false);
      vl::Log::print("Edge rendering enabled. Creases = on, hidden lines = off.\n");
    }
    else
    if (ch == '4')
    {
      mSolidRendering->setEnableMask(0xFFFFFFFF);
      // preserve color buffer, clear depth buffer
      mEdgeRendering->setClearFlags(vl::CF_CLEAR_DEPTH);
      mEdgeRendering->setEnableMask(0xFFFFFFFF);
      mEdgeRenderer->setShowCreases(true);
      mEdgeRenderer->setShowHiddenLines(true);
      vl::Log::print("Edge rendering enabled. Creases = on, hidden lines = on.\n");
    }
    else
    if (ch == '5')
    {
      mSolidRendering->setEnableMask(0);
      // clear color and depth buffer
      mEdgeRendering->setClearFlags(vl::CF_CLEAR_COLOR_DEPTH);
      mEdgeRendering->setEnableMask(0xFFFFFFFF);
      mEdgeRenderer->setShowCreases(true);
      mEdgeRenderer->setShowHiddenLines(false);
      vl::Log::print("Hidden line removal wireframe enabled. Creases = on, hidden lines = off.\n");
    }
    if (ch == '6')
    {
      mSolidRendering->setEnableMask(0);
      // clear color and depth buffer
      mEdgeRendering->setClearFlags(vl::CF_CLEAR_COLOR_DEPTH);
      mEdgeRendering->setEnableMask(0xFFFFFFFF);
      mEdgeRenderer->setShowCreases(true);
      mEdgeRenderer->setShowHiddenLines(true);
      vl::Log::print("Hidden line removal wireframe enabled. Creases = on, hidden lines = on.\n");
    }
  }

  void resizeEvent(int /*w*/, int /*h*/)
  {
    // solid rendering: update viewport and projection matrix
    mSolidRendering->camera()->viewport()->setWidth(mSolidRendering->renderTarget()->width());
    mSolidRendering->camera()->viewport()->setHeight(mSolidRendering->renderTarget()->height());
    mSolidRendering->camera()->setProjectionAsPerspective();

    // edge rendering: update viewport and projection matrix
    mEdgeRendering->camera()->viewport()->setWidth(mEdgeRendering->renderTarget()->width());
    mEdgeRendering->camera()->viewport()->setHeight(mEdgeRendering->renderTarget()->height());
    mEdgeRendering->camera()->setProjectionAsPerspective();
  }

  void loadModel(const std::vector<vl::String>& files)
  {
    // resets the scene
    mSceneManager->tree()->actors()->clear();
    // resets the EdgeRenderer cache
    mEdgeRenderer->clearCache();

    for(unsigned int i=0; i<files.size(); ++i)
    {
      vl::ref<vl::ResourceDatabase> resource_db = vl::loadResource(files[i],true);

      if (!resource_db || resource_db->count<vl::Actor>() == 0)
      {
        vl::Log::error("No data found.\n");
        continue;
      }

      std::vector< vl::ref<vl::Actor> > actors;
      resource_db->get<vl::Actor>(actors);
      for(unsigned i=0; i<actors.size(); ++i)
      {
        vl::ref<vl::Actor> actor = actors[i].get();
        // define a reasonable Shader
        actor->effect()->shader()->setRenderState( new vl::Light(0) );
        actor->effect()->shader()->enable(vl::EN_DEPTH_TEST);
        actor->effect()->shader()->enable(vl::EN_LIGHTING);
        actor->effect()->shader()->gocLightModel()->setTwoSide(true);
        // add the actor to the scene
        mSceneManager->tree()->addActor( actor.get() );
      }
    }

    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( mSceneManager.get(), vl::vec3(0,0,1)/*direction*/, vl::vec3(0,1,0)/*up*/, 1.0f/*bias*/ );
  }

  // laod the files dropped in the window
  void fileDroppedEvent(const std::vector<vl::String>& files) { loadModel(files); }

protected:
  vl::ref< vl::EdgeRenderer > mEdgeRenderer;
  vl::ref<vl::Rendering> mSolidRendering;
  vl::ref<vl::Rendering> mEdgeRendering;
  vl::ref<vl::SceneManagerActorTree> mSceneManager;
};

// Have fun!
