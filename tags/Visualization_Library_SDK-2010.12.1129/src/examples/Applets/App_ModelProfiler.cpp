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
#include "vl/Light.hpp"
#include "vl/Text.hpp"
#include "vl/FontManager.hpp"
#include "vl/VisualizationLibrary.hpp"
#include "vl/FileSystem.hpp"
#include "vl/DiskDirectory.hpp"
#include "vl/ResourceDatabase.hpp"
#include "vl/LoadWriterManager.hpp"
#include "vl/vl3DS.hpp"
#include "vl/vlPLY.hpp"
#include "vl/vlOBJ.hpp"
#include "vl/vlAC3D.hpp"
#include "vl/TriangleStripGenerator.hpp"
#include "vl/DoubleVertexRemover.hpp"
#include "vl/FontManager.hpp"
#include "vl/GeometryLoadCallback.hpp"

class App_ModelProfiler: public BaseDemo
{
public:
  void initEvent()
  {
    BaseDemo::initEvent();
    openglContext()->setContinuousUpdate(true);

    vl::GeometryLoadCallback* glc = dynamic_cast<vl::GeometryLoadCallback*>(vl::VisualizationLibrary::loadWriterManager()->loadCallbacks()->at(0));
    glc->setUseVBOs(false);
    glc->setSortVertices(false);
    glc->setComputeNormals(true);

    // vl::VisualizationLibrary::fileSystem()->directories()->push_back( new vl::DiskDirectory( vl::VisualizationLibrary::envVars()->value("VL_DATA_PATH")+"/images/") );

    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setFarPlane(10000000.0f);

    /* bind Transform */
    mTransform = new vl::Transform;
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mTransform.get() );
    
    // geoms
    vl::ref< vl::Effect > fx = new vl::Effect;
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->enable(vl::EN_CULL_FACE);
    fx->shader()->setRenderState( new vl::Light(0) );
    // fx->shader()->gocLight(0)->setPosition(vl::fvec4(0,0,1,1));

    // text setup

    resetOptions();

    mOptions = new vl::Text();
    mOptions->setDisplayListEnabled(true);
    mOptions->setFont( vl::VisualizationLibrary::fontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 8) );
    mOptions->setMargin(0);
    mOptions->setViewportAlignment(vl::AlignTop | vl::AlignLeft);
    mOptions->setAlignment(vl::AlignTop | vl::AlignLeft);
    mOptions->setColor(vlut::white);
    mOptions->setBackgroundColor(vl::fvec4(0,0,0,.75f));
    mOptions->setBackgroundEnabled(true);
    mOptions->setInterlineSpacing(0);

    vl::ref< vl::Effect > text_fx = new vl::Effect;
    text_fx->shader()->enable(vl::EN_BLEND);

    mOptionsActor = new vl::Actor( mOptions.get(), text_fx.get() );

    sceneManager()->tree()->addActor( mOptionsActor.get() );

    updateText();
  }

  virtual void shutdown() {}

  virtual void run() {}

  void resetOptions()
  {
    mOptDoubleFace = false;
    mOptRemoveDoubles = false;
    mOptStripfy1 = false;
    mOptStripfy2 = false;
    mOptMergeTriangleStrips = false;
    mOptSortVertices = false;
    mOptConvertToDrawArrays = false;
    mOptRecomputeNormals = false;
    mOptHighlightStrips = false;
    mOptUseVBO = false;
    mOptUseDL = false;
  }

  void mouseUpEvent(vl::EMouseButton, int x, int y)
  {
    vl::AABB bbox = mOptions->boundingRect();
    vl::Real h = bbox.height();

    // implement exclusion logic
    if( x < bbox.width() )
    {
      if( y < h / 14 * 1 ) 
      {
        mOptDoubleFace = !mOptDoubleFace;
      }
      else
      if( y < h / 14 * 2 ) 
      {
        mOptRemoveDoubles = !mOptRemoveDoubles;
        if(!mOptRemoveDoubles)
        {
          mOptStripfy1 = false;
          mOptStripfy2 = false;
        }
      } 
      else
      if( y < h / 14 * 3 ) 
      { 
        mOptStripfy1 = !mOptStripfy1; 
        if (mOptStripfy1) mOptStripfy2 = false;
        if (mOptStripfy1) mOptRemoveDoubles = true;
      }
      else
      if( y < h / 14 * 4 ) 
      {
        mOptStripfy2 = !mOptStripfy2; 
        if (mOptStripfy2) mOptStripfy1 = false;
        if (mOptStripfy2) mOptRemoveDoubles = true;
      }
      else
      if( y < h / 14 * 5 ) 
      {
        mOptMergeTriangleStrips = !mOptMergeTriangleStrips; 
      }
      else
      if( y < h / 14 * 6 ) 
      {
        mOptSortVertices = !mOptSortVertices; 
      }
      else
      if( y < h / 14 * 7 ) 
      {
        mOptConvertToDrawArrays = !mOptConvertToDrawArrays; 
      }
      else
      if( y < h / 14 * 8 ) 
      {
        mOptRecomputeNormals = !mOptRecomputeNormals;
      }
      else
      if( y < h / 14 * 9) 
      {
        mOptHighlightStrips = !mOptHighlightStrips;
        mOptConvertToDrawArrays = true;
      }
      else
      if( y < h / 14 * 10) 
      {
        mOptUseVBO = !mOptUseVBO; 
        if (mOptUseVBO) mOptUseDL = false;
      }
      else
      if( y < h / 14 * 11) 
      {
        mOptUseDL = !mOptUseDL;
        if (mOptUseDL) mOptUseVBO = false;
      }
      else
      if( y < h / 14 * 12) 
        applyChanges();
    }

    updateText();
  }

  void applyChanges()
  {
    vl::Log::print("Applying changes...\n");
    std::map< vl::ref<vl::Actor>, vl::ref<vl::Geometry> >::iterator it = mActorGeomMap.begin();
    int orig_primitives = 0;
    int resu_primitives = 0;
    int orig_indices = 0;
    int resu_indices = 0;
    int orig_vertices = 0;
    int resu_vertices = 0;
    for(; it != mActorGeomMap.end(); ++it)
    {
      vl::ref<vl::Geometry> geom = it->second->deepCopy();
      it->first->lod(0) = geom.get();

      if (mOptRecomputeNormals)
        geom->setNormalArray(NULL);

      // apply changes
      vl::Time timer;

      if (mOptDoubleFace)
      {
        for(unsigned i=0; i<mEffects.size(); ++i)
        {
          mEffects[i]->shader()->gocLightModel()->setTwoSide(true);
          mEffects[i]->shader()->disable(vl::EN_CULL_FACE);
        }
      }
      else
      {
        for(unsigned i=0; i<mEffects.size(); ++i)
        {
          mEffects[i]->shader()->gocLightModel()->setTwoSide(false);
          mEffects[i]->shader()->enable(vl::EN_CULL_FACE);
        }
      }

      if (mOptRemoveDoubles)
      {
        int orig_vertc = (int)geom->vertexArray()->size();
        timer.start();
        vl::DoubleVertexRemover dvr;
        dvr.removeDoubles(geom.get());
        vl::Log::print( vl::Say("vl::DoubleVertexRemover: %.3ns, ratio:%.3n\n") << timer.elapsed() << (float)geom->vertexArray()->size()/orig_vertc );
      }

      if (mOptStripfy1)
      {
        timer.start();
        vl::TriangleStripGenerator::stripfy(geom.get(), 0, false, false, false);
        vl::Log::print( vl::Say("vl::TriangleStripGenerator::stripfy: %.3ns\n") << timer.elapsed() );
      }

      if (mOptStripfy2)
      {
        timer.start();
        vl::TriangleStripGenerator::stripfy(geom.get(), 24, false, false, false);
        vl::Log::print( vl::Say("vl::TriangleStripGenerator::stripfy: %.3ns\n") << timer.elapsed() );
      }

      if (mOptMergeTriangleStrips)
      {
        timer.start();
        geom->mergeTriangleStrips();
        vl::Log::print( vl::Say("Merge triangle strips: %.3ns\n") << timer.elapsed() );
      }

      if (mOptSortVertices)
      {
        timer.start();
        bool ok = geom->sortVertices();
        if (ok)
          vl::Log::print( vl::Say("Sort Vertices: %.3ns\n") << timer.elapsed() );
        else
          vl::Log::print("Sort Vertices Not Performed.\n");
      }

      if (mOptConvertToDrawArrays)
      {
        geom->convertDrawCallToDrawArrays();
        vl::Log::print("Convert to DrawArrays.\n");
      }

      if (mOptUseDL)
        vl::Log::print("Using display lists.\n");
      geom->setDisplayListEnabled(mOptUseDL);

      if (mOptUseVBO)
        vl::Log::print("Using vertex buffer objects.\n");
      geom->setVBOEnabled(mOptUseVBO);

      if (!geom->normalArray())
        geom->computeNormals();

      if (mOptStripfy1 || mOptStripfy2)
      {
        for(unsigned i=0; i<mEffects.size(); ++i)
          mEffects[i]->shader()->gocMaterial()->setColorMaterialEnabled(mOptHighlightStrips);
        if (mOptHighlightStrips)
          geom->colorizePrimitives();
      }

      orig_primitives += it->second->drawCalls()->size();
      resu_primitives += geom->drawCalls()->size();
      for(int iprim=0; iprim<it->second->drawCalls()->size(); ++iprim)
        orig_indices += it->second->drawCalls()->at(iprim)->countIndices();
      for(int iprim=0; iprim<geom->drawCalls()->size(); ++iprim)
        resu_indices += geom->drawCalls()->at(iprim)->countIndices();
      orig_vertices += (int)it->second->vertexArray()->size();
      resu_vertices += (int)geom->vertexArray()->size();
    }

    vl::Log::print( vl::Say("\nPrimitives: %6n -> %6n (%3.1n%%)\n") << orig_primitives << resu_primitives << (float)resu_primitives/orig_primitives*100.0f );
    vl::Log::print( vl::Say("Indices:    %6n -> %6n (%3.1n%%)\n") << orig_indices << resu_indices << (float)resu_indices/orig_indices*100.0f );
    vl::Log::print( vl::Say("Vertices:   %6n -> %6n (%3.1n%%)\n\n") << orig_vertices << resu_vertices << (float)resu_vertices/orig_vertices*100.0f);
  }

  void updateText()
  {
    vl::String str;
    mOptions->setText("");
    str += mOptDoubleFace ? "[x]" : "[ ]"; str += " Double Face\n";
    str += mOptRemoveDoubles? "[x]" : "[ ]"; str += " Remove Doubles\n";
    str += mOptStripfy1? "[x]" : "[ ]"; str += " Stripfy 0\n";
    str += mOptStripfy2? "[x]" : "[ ]"; str += " Stripfy 24\n";
    str += mOptMergeTriangleStrips? "[x]" : "[ ]"; str += " Merge Triangle Strips\n";
    str += mOptSortVertices? "[x]" : "[ ]"; str += " Sort Vertices\n";
    str += mOptConvertToDrawArrays? "[x]" : "[ ]"; str += " Convert To DrawArrays\n";
    str += mOptRecomputeNormals ? "[x]" : "[ ]"; str += " Recompute Normals\n";
    str += mOptHighlightStrips ? "[x]" : "[ ]"; str += " Highlight Strips\n";
    str += mOptUseVBO? "[x]" : "[ ]"; str += " Use Vertex Buffer Objects\n";
    str += mOptUseDL? "[x]" : "[ ]"; str += " Use Display Lists\n";
    str += "<APPLY CHANGES>";

    mOptions->setText(str);
    mOptions->setDisplayListDirty(true);
  }

  void loadModel(const std::vector<vl::String>& files)
  {
    // load the model
    resetOptions();
    updateText();

    sceneManager()->tree()->actors()->clear();
    sceneManager()->tree()->addActor( mOptionsActor.get() );
    mEffects.clear();
    mActorGeomMap.clear();

    vl::Time timer;
    timer.start();
 
    for(unsigned int i=0; i<files.size(); ++i)
    {
      if (files.size()>1)
        vl::Log::print( vl::Say("[% 3n%%] Loading: '%s'\n") << (100*i/(files.size()-1)) << files[i] );
      else
        vl::Log::print( vl::Say("Loading: '%s'\n") << files[i] );

      vl::ref<vl::ResourceDatabase> resource_db = vl::loadResource(files[i],true);

      vl::Log::print( vl::Say("Import time = %.3ns\n") << timer.elapsed() );

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

        if(std::find(mEffects.begin(), mEffects.end(), actor->effect()) == mEffects.end())
          mEffects.push_back(actor->effect());

        actor->effect()->shader()->setRenderState( new vl::Light(0) );
        actor->effect()->shader()->enable(vl::EN_DEPTH_TEST);
        actor->effect()->shader()->enable(vl::EN_LIGHTING);
        actor->effect()->shader()->enable(vl::EN_CULL_FACE);

        vl::Geometry* geom = dynamic_cast<vl::Geometry*>(actor->lod(0).get());
        if (geom)
          mActorGeomMap[actor] = geom->deepCopy();

        geom->setVBOEnabled(false);
        geom->setDisplayListEnabled(false);

        if ( actor->transform() )
        {
          VL_CHECK(mTransform != actor->transform())
          mTransform->addChild(actor->transform());
        }
        else
          actor->setTransform(mTransform.get());

        sceneManager()->tree()->addActor( actor.get() );
      }
    }

    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( vl::VisualizationLibrary::rendering()->as<vl::Rendering>(), vl::vec3(0,0,1), vl::vec3(0,1,0), 1.0f );

    // throttle ghost camera manipulator speed based on the scene size, using a simple euristic formula
    sceneManager()->computeBounds();
    const vl::AABB& scene_aabb = sceneManager()->boundingBox();
    vl::Real speed = (scene_aabb.width() + scene_aabb.height() + scene_aabb.depth()) / 20.0f;
    ghostCamera()->setMovementSpeed(speed);

    // update the rendering
    openglContext()->update();
  }

  void fileDroppedEvent(const std::vector<vl::String>& files)
  {
    loadModel(files);
  }

  void resizeEvent(int w, int h)
  {
    BaseDemo::resizeEvent(w,h);
    mOptions->setDisplayListDirty(true);
  }

  void keyPressEvent(unsigned short ch, vl::EKey key)
  {
    BaseDemo::keyPressEvent(ch,key);

    for(unsigned i=0; i<mEffects.size(); ++i)
    {
      if (key == vl::Key_1)
      {
        if (mEffects[i]->shader()->gocShadeModel()->shadeModel() == vl::SM_FLAT)
          mEffects[i]->shader()->gocShadeModel()->set(vl::SM_SMOOTH);
        else
          mEffects[i]->shader()->gocShadeModel()->set(vl::SM_FLAT);
      }
      if (key == vl::Key_2)
      {
        if (mEffects[i]->shader()->gocPolygonMode()->frontFace() == vl::PM_FILL)
          mEffects[i]->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
        else
          mEffects[i]->shader()->gocPolygonMode()->set(vl::PM_FILL, vl::PM_FILL);
      }
      if (key == vl::Key_3)
      {
        if (mEffects[i]->shader()->isEnabled(vl::EN_CULL_FACE) )
          mEffects[i]->shader()->disable(vl::EN_CULL_FACE);
        else
          mEffects[i]->shader()->enable(vl::EN_CULL_FACE);
      }
    }
  }

protected:
  std::vector< vl::ref<vl::Effect> > mEffects;
  vl::ref<vl::Transform> mTransform;
  vl::ref< vl::Text > mOptions;
  vl::ref< vl::Actor > mOptionsActor;
  std::map< vl::ref<vl::Actor>, vl::ref<vl::Geometry> > mActorGeomMap;

  // options
  bool mOptDoubleFace;
  bool mOptRemoveDoubles;
  bool mOptStripfy1;
  bool mOptStripfy2;
  bool mOptMergeTriangleStrips;
  bool mOptSortVertices;
  bool mOptConvertToDrawArrays;
  bool mOptRecomputeNormals;
  bool mOptHighlightStrips;
  bool mOptUseVBO;
  bool mOptUseDL;
};

// Have fun!

BaseDemo* Create_App_ModelProfiler() { return new App_ModelProfiler; }
