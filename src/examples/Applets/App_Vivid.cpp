/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
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
#include <vlCore/glsl_math.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Text.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/DiskDirectory.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlGraphics/TriangleStripGenerator.hpp>
#include <vlGraphics/DoubleVertexRemover.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/plugins/ioVLX.hpp>
#include <vlGraphics/DepthSortCallback.hpp>
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/RendererVivid.hpp>

#define MODEL_FILENAME "/vivid/bunny.ply"

vl::ref<vl::RendererVivid> vivid = new vl::RendererVivid();

class App_Vivid: public BaseDemo
{
public:
  void initEvent()
  {
    vl::Log::print( vl::Say("GL_VERSION: %s\n") << glGetString(GL_VERSION));
    vl::Log::print( vl::Say("GL_RENDERER: %s\n\n") << glGetString(GL_RENDERER));

    vl::Log::notify(appletInfo());
    openglContext()->setContinuousUpdate(false);
    rendering()->as<vl::Rendering>()->setNearFarClippingPlanesOptimized(true);
    
    // Setup the vivid renderer
    vivid->setFramebuffer(openglContext()->framebuffer());
    rendering()->as<vl::Rendering>()->setRenderer( vivid.get() );

#if 1
    initScene();
#else
    std::vector<vl::String> files;
    files.push_back(MODEL_FILENAME);
    loadModel(files);
#endif
  }

  void initScene() {
    vl::ref<vl::Effect> fx1 = new vl::Effect;
    fx1->shader()->enable(vl::EN_BLEND);
    fx1->shader()->enable(vl::EN_DEPTH_TEST);
    fx1->shader()->enable(vl::EN_LIGHTING);
    fx1->shader()->setRenderState( new vl::Light, 0 );
    fx1->shader()->gocLightModel()->setTwoSide(true);
    fx1->shader()->gocMaterial()->setDiffuse( vl::fvec4(1.0f, 1.0f, 1.0f, 0.5f) );

    vl::ref<vl::Effect> fx2 = new vl::Effect;
    fx2->shader()->disable(vl::EN_BLEND);
    fx2->shader()->enable(vl::EN_DEPTH_TEST);
    fx2->shader()->enable(vl::EN_LIGHTING);
    fx2->shader()->setRenderState( new vl::Light, 0 );
    fx2->shader()->gocLightModel()->setTwoSide(true);
    fx2->shader()->gocMaterial()->setDiffuse( vl::fvec4(1.0f, 0.0f, 0.0f, 1.0f) );

    vl::ref< vl::Transform > tr1 = new vl::Transform();
    tr1->setLocalAndWorldMatrix(vl::mat4::getTranslation(+0.025f, 0, 0));

    vl::ref< vl::Transform > tr2 = new vl::Transform();
    tr2->setLocalAndWorldMatrix(vl::mat4::getTranslation(-0.025f, 0, 0) * vl::mat4::getRotationXYZ(90, 0, 0));

    vl::ref< vl::Geometry > torus = vl::makeTorus( vl::vec3( 0, 0, 0 ), 0.1f, 0.02f, 20, 40 );

    sceneManager()->tree()->addActor( torus.get(), fx1.get(), tr1.get() );
    sceneManager()->tree()->addActor( torus.get(), fx2.get(), tr2.get() );

    adjustScene();
  }

  void adjustScene() {
    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( rendering()->as<vl::Rendering>(), vl::vec3(0,0,1), vl::vec3(0,1,0), 1.0f );

    // throttle ghost camera manipulator speed based on the scene size, using a simple euristic formula
    sceneManager()->computeBounds();
    const vl::AABB& scene_aabb = sceneManager()->boundingBox();
    vl::real speed = (scene_aabb.width() + scene_aabb.height() + scene_aabb.depth()) / 20.0f;
    ghostCameraManipulator()->setMovementSpeed(speed);
  }

  void loadModel(const std::vector<vl::String>& files)
  {
    // default effects

    sceneManager()->tree()->actors()->clear();

    for(unsigned int i=0; i<files.size(); ++i)
    {
      vl::ref<vl::ResourceDatabase> resource_db = vl::loadResource(files[i], false);

      if (!resource_db || resource_db->count<vl::Actor>() == 0)
      {
        VL_LOG_ERROR << "No data found.\n";
        continue;
      }

#if 0
      // VLX save
      String save_path = files[i].extractPath() + files[i].extractFileName() + ".vlb";
      saveVLB(save_path, resource_db.get());
#endif

      showStatistics(resource_db);

      for(size_t i=0; i<resource_db->resources().size(); ++i)
      {
        vl::Actor* act = resource_db->resources()[i]->as<vl::Actor>();

        if (!act)
          continue;

        // act->actorEventCallbacks()->push_back( new vl::DepthSortCallback );

        vl::ref<vl::Effect> fx1 = new vl::Effect;
        fx1->shader()->enable(vl::EN_BLEND);
        fx1->shader()->enable(vl::EN_DEPTH_TEST);
        fx1->shader()->enable(vl::EN_LIGHTING);
        fx1->shader()->disable(vl::EN_CULL_FACE);
        fx1->shader()->setRenderState( new vl::Light, 0 );
        fx1->shader()->gocLightModel()->setTwoSide(true);
        fx1->shader()->gocMaterial()->setDiffuse( vl::fvec4(1.0f, 0.0f, 0.0f, 1.0f) );

        act->setEffect(fx1.get());

        vl::Geometry* geom = act->lod(0)->as<vl::Geometry>();
        geom->computeNormals();

        sceneManager()->tree()->addActor(act);
      }
    }

    adjustScene();
  }

  //--------------------------------------------------------------------------
  void keyPressEvent(unsigned short unicode_ch, vl::EKey key)
  {
    // BaseDemo::keyReleaseEvent(unicode_ch, key);

    switch((unsigned char)tolower(unicode_ch))
    {
	    case 'r':
        vivid->initShaders();
		    break;
	    case 'q':
        vivid->setUseQueryObject(!vivid->useQueryObject());
		    break;
	    case '+':
        vivid->setNumPasses(vivid->numPasses() + 1);
		    break;
	    case '-':
        if (vivid->numPasses() > 1) {
          vivid->setNumPasses(vivid->numPasses() - 1);
        }
		    break;
	    case '1':
        vivid->setRenderingMode(vl::RendererVivid::DualDepthPeeling);
		    break;
	    case '2':
		    vivid->setRenderingMode(vl::RendererVivid::FrontToBackDepthPeeling);
		    break;
	    case '3':
		    vivid->setRenderingMode(vl::RendererVivid::FastRender);
		    break;
	    /*case 'a':
		    g_opacity -= 0.05;
        g_opacity = vl::max(g_opacity, 0.0f);
		    break;
	    case 'd':
		    g_opacity += 0.05;
		    g_opacity = vl::min(g_opacity, 1.0f);
		    break;
	    case 'b':
		    g_backgroundColor = (g_backgroundColor == g_white) ? g_black : g_white;
		    break;*/
    }

    const char* method[] = { "FastRender", "DualDepthPeeling", "FrontToBackDepthPeeling" };
    printf("method:           %s\n", method[vivid->renderingMode()]);
    printf("pass counter:     %d\n", vivid->passCounter());
    printf("num passes:       %d\n", vivid->numPasses());
    printf("use query object: %d\n", vivid->useQueryObject());
    printf("---\n");
    openglContext()->update();
  }

  void showStatistics(vl::ref<vl::ResourceDatabase> res_db)
  {
    std::set<vl::Geometry*> geometries;
    std::vector< vl::ref<vl::Geometry> > geom_db;
    std::vector< vl::ref<vl::Actor> > actor_db;

    res_db->get<vl::Actor>(actor_db);
    res_db->get<vl::Geometry>(geom_db);

    // find number of unique geometries

    for(size_t i=0; i<geom_db.size(); ++i)
      geometries.insert( geom_db[i].get() );

    for(size_t i=0; i<actor_db.size(); ++i)
    {
      vl::Geometry* geom = actor_db[i]->lod(0)->as<vl::Geometry>();
      if (geom)
        geometries.insert( geom );
    }

    int total_triangles = 0;
    int total_draw_calls = 0;
    for( std::set<vl::Geometry*>::iterator it = geometries.begin(); it != geometries.end(); ++it )
    {
      total_draw_calls += (*it)->drawCalls().size();
      for(int i=0; i < (*it)->drawCalls().size(); ++i )
        total_triangles += (*it)->drawCalls().at(i)->countTriangles();
    }

    VL_LOG_PRINT << "Statistics:\n";
    VL_LOG_PRINT << "+ Total triangles  = " << total_triangles << "\n";
    VL_LOG_PRINT << "+ Total draw calls = " << total_draw_calls << "\n";
    VL_LOG_PRINT << "+ Actors           = " << actor_db.size() << "\n";
    VL_LOG_PRINT << "+ Geometries       = " << geometries.size() << "\n";
  }

  void fileDroppedEvent(const std::vector<vl::String>& files)
  {
    loadModel(files);
    // update the rendering
    openglContext()->update();
  }


protected:
  std::set< vl::ref<vl::Effect> > mEffects;
  std::vector<vl::String> mLastShaders;
};

// Have fun!

BaseDemo* Create_App_Vivid() { return new App_Vivid; }
