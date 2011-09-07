/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

using namespace vl;

class App_ModelProfiler: public BaseDemo
{
public:
  void initEvent()
  {
    vl::Log::notify(appletInfo());
    openglContext()->setContinuousUpdate(false);
    rendering()->as<Rendering>()->setNearFarClippingPlanesOptimized(true);
  }

  void loadModel(const std::vector<String>& files)
  {
    sceneManager()->tree()->actors()->clear();

    // default effects

    ref<Light> camera_light = new Light;
    ref<Effect> fx_lit = new Effect;
    fx_lit->shader()->enable(EN_DEPTH_TEST);
    fx_lit->shader()->enable(EN_LIGHTING);
    fx_lit->shader()->setRenderState(camera_light.get(), 0);

    ref<Effect> fx_solid = new Effect;
    fx_solid->shader()->enable(EN_DEPTH_TEST);

    for(unsigned int i=0; i<files.size(); ++i)
    {
      ref<ResourceDatabase> resource_db = loadResource(files[i], false);

      if (!resource_db || resource_db->count<Actor>() == 0)
      {
        VL_LOG_ERROR << "No data found.\n";
        continue;
      }

      showStatistics(resource_db);

      for(size_t i=0; i<resource_db->resources().size(); ++i)
      {
        Actor* act     = resource_db->resources()[i]->as<Actor>();
        Geometry* geom = resource_db->resources()[i]->as<Geometry>();

        if (act)
        {
          sceneManager()->tree()->addActor(act);
        }
        else
        if (geom && geom->normalArray())
        {
          sceneManager()->tree()->addActor(geom, fx_lit.get(), NULL);
        }
        else
        if (geom && !geom->normalArray())
        {
          sceneManager()->tree()->addActor(geom, fx_solid.get(), NULL);
        }
      }
    }

    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( rendering()->as<Rendering>(), vec3(0,0,1), vec3(0,1,0), 1.0f );

    // throttle ghost camera manipulator speed based on the scene size, using a simple euristic formula
    sceneManager()->computeBounds();
    const AABB& scene_aabb = sceneManager()->boundingBox();
    real speed = (scene_aabb.width() + scene_aabb.height() + scene_aabb.depth()) / 20.0f;
    ghostCameraManipulator()->setMovementSpeed(speed);

    // update the rendering
    openglContext()->update();
  }

  void showStatistics(ref<ResourceDatabase> res_db)
  {
    std::set<Geometry*> geometries;
    std::vector< ref<Geometry> > geom_db;
    std::vector< ref<Actor> > actor_db;

    res_db->get<Actor>(actor_db);
    res_db->get<Geometry>(geom_db);

    // find number of unique geometries

    for(size_t i=0; i<geom_db.size(); ++i)
      geometries.insert( geom_db[i].get() );

    for(size_t i=0; i<actor_db.size(); ++i)
    {
      Geometry* geom = actor_db[i]->lod(0)->as<Geometry>();
      if (geom)
        geometries.insert( geom );
    }

    int total_triangles = 0;
    int total_draw_calls = 0;
    for( std::set<Geometry*>::iterator it = geometries.begin(); it != geometries.end(); ++it )
    {
      total_draw_calls += (*it)->drawCalls()->size();
      for(int i=0; i < (*it)->drawCalls()->size(); ++i )
        total_triangles += (*it)->drawCalls()->at(i)->countTriangles();
    }

    VL_LOG_PRINT << "Statistics:\n";
    VL_LOG_PRINT << "+ Total triangles  = " << total_triangles << "\n";
    VL_LOG_PRINT << "+ Total draw calls = " << total_draw_calls << "\n";
    VL_LOG_PRINT << "+ Actors           = " << actor_db.size() << "\n";
    VL_LOG_PRINT << "+ Geometries       = " << geometries.size() << "\n";
  }

  void fileDroppedEvent(const std::vector<String>& files)
  {
    loadModel(files);
  }

protected:
  // nothing
};

// Have fun!

BaseDemo* Create_App_ModelProfiler() { return new App_ModelProfiler; }
