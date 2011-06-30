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
    vl::Log::info(appletInfo());
    openglContext()->setContinuousUpdate(true);

    rendering()->as<Rendering>()->camera()->setFarPlane(10000000.0f);

    /* bind Transform */
    mTransform = new Transform;
    rendering()->as<Rendering>()->transform()->addChild( mTransform.get() );
    
    // geoms
    ref< Effect > fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->enable(EN_CULL_FACE);
    fx->shader()->setRenderState( new Light(0) );
    // fx->shader()->gocLight(0)->setPosition(fvec4(0,0,1,1));

    // text setup

    resetOptions();

    mOptions = new Text();
    mOptions->setDisplayListEnabled(vl::Has_GL_Version_1_1);
    mOptions->setFont( defFontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 8) );
    mOptions->setMargin(0);
    mOptions->setViewportAlignment(AlignTop | AlignLeft);
    mOptions->setAlignment(AlignTop | AlignLeft);
    mOptions->setColor(white);
    mOptions->setBackgroundColor(fvec4(0,0,0,.75f));
    mOptions->setBackgroundEnabled(true);

    ref< Effect > text_fx = new Effect;
    text_fx->shader()->enable(EN_BLEND);

    mOptionsActor = new Actor( mOptions.get(), text_fx.get() );

    sceneManager()->tree()->addActor( mOptionsActor.get() );

    updateText();
  }

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

  void mouseUpEvent(EMouseButton, int x, int y)
  {
    AABB bbox = mOptions->boundingRect();
    Real h = bbox.height();

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
    Log::print("Applying changes...\n");
    std::map< ref<Actor>, ref<Geometry> >::iterator it = mActorGeomMap.begin();
    int orig_primitives = 0;
    int resu_primitives = 0;
    int orig_indices = 0;
    int resu_indices = 0;
    int orig_vertices = 0;
    int resu_vertices = 0;
    for(; it != mActorGeomMap.end(); ++it)
    {
      ref<Geometry> geom = it->second->deepCopy();
      it->first->setLod(0, geom.get());

      if (mOptRecomputeNormals)
        geom->setNormalArray(NULL);

      // apply changes
      Time timer;

      if (mOptDoubleFace)
      {
        for(unsigned i=0; i<mEffects.size(); ++i)
        {
          mEffects[i]->shader()->gocLightModel()->setTwoSide(true);
          mEffects[i]->shader()->disable(EN_CULL_FACE);
        }
      }
      else
      {
        for(unsigned i=0; i<mEffects.size(); ++i)
        {
          mEffects[i]->shader()->gocLightModel()->setTwoSide(false);
          mEffects[i]->shader()->enable(EN_CULL_FACE);
        }
      }

      if (mOptRemoveDoubles)
      {
        int orig_vertc = (int)geom->vertexArray()->size();
        timer.start();
        DoubleVertexRemover dvr;
        dvr.removeDoubles(geom.get());
        Log::print( Say("DoubleVertexRemover: %.3ns, ratio:%.3n\n") << timer.elapsed() << (float)geom->vertexArray()->size()/orig_vertc );
      }

      if (mOptStripfy1)
      {
        timer.start();
        TriangleStripGenerator::stripfy(geom.get(), 0, false, false, false);
        Log::print( Say("TriangleStripGenerator::stripfy: %.3ns\n") << timer.elapsed() );
      }

      if (mOptStripfy2)
      {
        timer.start();
        TriangleStripGenerator::stripfy(geom.get(), 24, false, false, false);
        Log::print( Say("TriangleStripGenerator::stripfy: %.3ns\n") << timer.elapsed() );
      }

      if (mOptMergeTriangleStrips)
      {
        timer.start();
        geom->mergeTriangleStrips();
        Log::print( Say("Merge triangle strips: %.3ns\n") << timer.elapsed() );
      }

      if (mOptSortVertices)
      {
        timer.start();
        bool ok = geom->sortVertices();
        if (ok)
          Log::print( Say("Sort Vertices: %.3ns\n") << timer.elapsed() );
        else
          Log::print("Sort Vertices Not Performed.\n");
      }

      if (mOptConvertToDrawArrays)
      {
        geom->convertDrawCallToDrawArrays();
        Log::print("Convert to DrawArrays.\n");
      }

      if (mOptUseDL)
        Log::print("Using display lists.\n");
      geom->setDisplayListEnabled(mOptUseDL);

      if (mOptUseVBO)
        Log::print("Using vertex buffer objects.\n");
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

    Log::print( Say("\nPrimitives: %6n -> %6n (%3.1n%%)\n") << orig_primitives << resu_primitives << (float)resu_primitives/orig_primitives*100.0f );
    Log::print( Say("Indices:    %6n -> %6n (%3.1n%%)\n") << orig_indices << resu_indices << (float)resu_indices/orig_indices*100.0f );
    Log::print( Say("Vertices:   %6n -> %6n (%3.1n%%)\n\n") << orig_vertices << resu_vertices << (float)resu_vertices/orig_vertices*100.0f);
  }

  void updateText()
  {
    String str;
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

  void loadModel(const std::vector<String>& files)
  {
    // load the model
    resetOptions();
    updateText();

    sceneManager()->tree()->actors()->clear();
    sceneManager()->tree()->addActor( mOptionsActor.get() );
    mEffects.clear();
    mActorGeomMap.clear();

    Time timer;
    timer.start();
 
    for(unsigned int i=0; i<files.size(); ++i)
    {
      if (files.size()>1)
        Log::print( Say("[% 3n%%] Loading: '%s'\n") << (100*i/(files.size()-1)) << files[i] );
      else
        Log::print( Say("Loading: '%s'\n") << files[i] );

      ref<ResourceDatabase> resource_db = loadResource(files[i],true);

      Log::print( Say("Import time = %.3ns\n") << timer.elapsed() );

      if (!resource_db || resource_db->count<Actor>() == 0)
      {
        Log::error("No data found.\n");
        continue;
      }

      std::vector< ref<Actor> > actors;
      resource_db->get<Actor>(actors);
      for(unsigned i=0; i<actors.size(); ++i)
      {
        ref<Actor> actor = actors[i].get();

        if(std::find(mEffects.begin(), mEffects.end(), actor->effect()) == mEffects.end())
          mEffects.push_back(actor->effect());

        actor->effect()->shader()->setRenderState( new Light(0) );
        actor->effect()->shader()->enable(EN_DEPTH_TEST);
        actor->effect()->shader()->enable(EN_LIGHTING);
        actor->effect()->shader()->enable(EN_CULL_FACE);

        Geometry* geom = dynamic_cast<Geometry*>(actor->lod(0).get());
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
    trackball()->adjustView( rendering()->as<Rendering>(), vec3(0,0,1), vec3(0,1,0), 1.0f );

    // throttle ghost camera manipulator speed based on the scene size, using a simple euristic formula
    sceneManager()->computeBounds();
    const AABB& scene_aabb = sceneManager()->boundingBox();
    Real speed = (scene_aabb.width() + scene_aabb.height() + scene_aabb.depth()) / 20.0f;
    ghostCameraManipulator()->setMovementSpeed(speed);

    // update the rendering
    openglContext()->update();
  }

  void fileDroppedEvent(const std::vector<String>& files)
  {
    loadModel(files);
  }

  void resizeEvent(int w, int h)
  {
    BaseDemo::resizeEvent(w,h);
    mOptions->setDisplayListDirty(true);
  }

  void keyPressEvent(unsigned short ch, EKey key)
  {
    BaseDemo::keyPressEvent(ch,key);

    for(unsigned i=0; i<mEffects.size(); ++i)
    {
      if (key == Key_1)
      {
        if (mEffects[i]->shader()->gocShadeModel()->shadeModel() == SM_FLAT)
          mEffects[i]->shader()->gocShadeModel()->set(SM_SMOOTH);
        else
          mEffects[i]->shader()->gocShadeModel()->set(SM_FLAT);
      }
      if (key == Key_2)
      {
        if (mEffects[i]->shader()->gocPolygonMode()->frontFace() == PM_FILL)
          mEffects[i]->shader()->gocPolygonMode()->set(PM_LINE, PM_LINE);
        else
          mEffects[i]->shader()->gocPolygonMode()->set(PM_FILL, PM_FILL);
      }
      if (key == Key_3)
      {
        if (mEffects[i]->shader()->isEnabled(EN_CULL_FACE) )
          mEffects[i]->shader()->disable(EN_CULL_FACE);
        else
          mEffects[i]->shader()->enable(EN_CULL_FACE);
      }
    }
  }

protected:
  std::vector< ref<Effect> > mEffects;
  ref<Transform> mTransform;
  ref< Text > mOptions;
  ref< Actor > mOptionsActor;
  std::map< ref<Actor>, ref<Geometry> > mActorGeomMap;

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
