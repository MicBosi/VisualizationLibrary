/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2016, Michele Bosi                                             */
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
#include <vlVivid/VividRendering.hpp>
#include <vlVivid/VividVolume.hpp>
#include <vlGraphics/CalibratedCamera.hpp>
#include <vlGraphics/AdjacencyExtractor.hpp>

// #define MODEL_FILENAME "/tmp/dragon.ply"
// #define MODEL_FILENAME "/tmp/bunny.ply"
// #define MODEL_FILENAME "/tmp/niftk.vlb"
#define MODEL_FILENAME "/tmp/rsg-liver.vlb"
// #define MODEL_FILENAME "/tmp/MRV.stl"


using namespace vl;

class App_Vivid: public BaseDemo
{
public:
  void initEvent()
  {
    Log::notify( appletInfo() );

    Log::print( Say("GL_VERSION: %s\n") << glGetString(GL_VERSION));
    Log::print( Say("GL_RENDERER: %s\n\n") << glGetString(GL_RENDERER));

    openglContext()->setContinuousUpdate( false );

    // Vivid Rendering

    mVividRendering = new VividRendering;
    mVivid = mVividRendering->vividRenderer();
    mVividVolume = new VividVolume( mVividRendering.get() );
    trackball()->setCamera( mVividRendering->calibratedCamera() );

    // Other

    mLiverOutlineMode = new Uniform( "vl_Vivid.renderMode" );
    mLiverOutlineMode->setUniformI( 4 );
    mOutlineMode = new Uniform( "vl_Vivid.renderMode" );
    mOutlineMode->setUniformI( 0 );
    mOutlineSlicePlane = new Uniform( "vl_Vivid.outline.slicePlane" );
    mOutlineSlicePlane->setUniform( vec4( 1, 0, 0, 0 ) );

#if 1
    initScene2();
#else
    std::vector<String> files;
    files.push_back(MODEL_FILENAME);
    loadModel(files);
#endif

    // mBackgroundImages[0] = loadImage( "/tmp/rsg-liver.png" );
    mBackgroundImages[0] = loadImage( "/tmp/liver.jpg" );
    mBackgroundImages[1] = loadImage( "/images/sun1.png" );
    mBackgroundImages[2] = loadImage( "/images/sun2.png" );
    mBackgroundImages[3] = loadImage( "/images/sun3.png" );
    // deep copy
    mBackgroundImage = new Image;
    *mBackgroundImage = *mBackgroundImages[0];
  }


  void initScene1() {
    ref<Effect> fx1 = VividRendering::makeVividEffect();
    fx1->shader()->disable(EN_BLEND);
    fx1->shader()->enable(EN_DEPTH_TEST);
    fx1->shader()->enable(EN_LIGHTING);
    fx1->shader()->setRenderState( new Light, 0 );
    fx1->shader()->gocLightModel()->setTwoSide(true);
    fx1->shader()->gocLightModel()->setLocalViewer(true);
    fx1->shader()->gocMaterial()->setAmbient( fvec4(1.0f, 1.0f, 1.0f, 1.0f) );
    fx1->shader()->gocMaterial()->setDiffuse( fvec4(1.0f, 0.0f, 0.0f, 1.0f) );
    fx1->shader()->gocMaterial()->setSpecular( fvec4(1.0f, 1.0f, 1.0f, 1.0f) );
    fx1->shader()->gocMaterial()->setShininess(128.0f);
    fx1->shader()->gocMaterial()->setColorMaterialEnabled(false);
    fx1->shader()->gocMaterial()->setColorMaterial(PF_FRONT_AND_BACK, CM_DIFFUSE);
    fx1->shader()->gocUniform("vl_Vivid.colorMaterialEnabled")->setUniform( 0 );

    ref<Effect> fx2 = VividRendering::makeVividEffect();
    fx2->shader()->enable(EN_BLEND);
    fx2->shader()->enable(EN_DEPTH_TEST);
    fx2->shader()->enable(EN_LIGHTING);
    fx2->shader()->setRenderState( new Light, 0 );
    fx2->shader()->gocLightModel()->setTwoSide(true);
    fx2->shader()->gocLightModel()->setLocalViewer(true);
    fx2->shader()->gocMaterial()->setDiffuse( fvec4(1.0f, 1.0f, 0.0f, 0.25f) );
    fx2->shader()->gocMaterial()->setSpecular( fvec4(1.0f, 1.0f, 1.0f, 1.0f) );
    fx2->shader()->gocMaterial()->setShininess(128.0f);
    fx2->shader()->gocMaterial()->setColorMaterialEnabled(true);
    fx2->shader()->gocMaterial()->setColorMaterial(PF_FRONT_AND_BACK, CM_DIFFUSE);
    fx2->shader()->gocUniform("vl_Vivid.colorMaterialEnabled")->setUniform( 1 );

    ref< Transform > tr1 = new Transform();
    tr1->setLocalAndWorldMatrix(mat4::getTranslation(+0.025f, 0, 0));

    ref< Transform > tr2 = new Transform();
    tr2->setLocalAndWorldMatrix(mat4::getTranslation(-0.025f, 0, 0) * mat4::getRotationXYZ(90, 0, 0));

    ref< Geometry > torus = makeTorus( vec3( 0, 0, 0 ), 0.1f, 0.02f, 20, 40 );
    torus->setColorArray(fvec4(1.0f, 0.5f, 0.0f, 0.25f));

    mVividRendering->sceneManager()->tree()->addActor( torus.get(), fx1.get(), tr1.get() );
    mVividRendering->sceneManager()->tree()->addActor( torus.get(), fx2.get(), tr2.get() );

    adjustScene();
  }

  void initScene2() {
    ref<Effect> fx1 = VividRendering::makeVividEffect();
    ref<Effect> fx2 = VividRendering::makeVividEffect();
    fx1->shader()->gocUniform( "vl_Vivid.material.diffuse" )->setUniform( vl::vec4(1, 0, 0, .5f) );
    fx2->shader()->gocUniform( "vl_Vivid.material.diffuse" )->setUniform( vl::vec4(1, 0, 0, .5f) );

    const float size = 5000;
#if 1
    ref< Geometry > box1 = makeBox( vl::vec3(0.0f), size, size, size, false );
#else
    ref< Geometry > box1 = makeUVSphere( vl::vec3(0.0f), 100, 20, 20 );
#endif
    ref< Geometry > box2 = makeBox( vl::vec3(0.0f), size * .8f, size * .8f, size * .8f, false );

    box1->computeNormals();
    box2->computeNormals();

    Actor* a1 = mVividRendering->sceneManager()->tree()->addActor( box1.get(), fx1.get(), NULL );
    Actor* a2 = mVividRendering->sceneManager()->tree()->addActor( box2.get(), fx2.get(), NULL );
    a1->setEnableMask( vl::Vivid::VividEnableMask );
    a2->setEnableMask( vl::Vivid::VividEnableMask );

    adjustScene();
  }

  void adjustScene() {
    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( mVividRendering.get(), vec3(0,0,1), vec3(0,1,0), 1.0f );

    // throttle ghost camera manipulator speed based on the scene size, using a simple euristic formula
    mVividRendering->sceneManager()->computeBounds();
    const AABB& scene_aabb = mVividRendering->sceneManager()->boundingBox();
    real speed = (scene_aabb.width() + scene_aabb.height() + scene_aabb.depth()) / 20.0f;
    ghostCameraManipulator()->setMovementSpeed(speed);
  }

  String filename( const String& path ) {
      std::vector< String > splits;
      path.split('/', splits, true);
      return splits.back();
  }

  void loadModel(const std::vector<String>& files)
  {
    // default effects

    mVividRendering->sceneManager()->tree()->actors()->clear();

    for(unsigned int i=0; i<files.size(); ++i)
    {
      ref<ResourceDatabase> resource_db = loadResource(files[i], false);

      if (!resource_db || resource_db->count<Actor>() == 0)
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

      for(size_t j=0; j<resource_db->resources().size(); ++j)
      {
        Actor* act = resource_db->resources()[j]->as<Actor>();

        if ( ! act )
          continue;

        printf( "Actor: %s\n", act->objectName().c_str() );

        mLastModelName = filename( files[i] );

        // saveActor( act );

        // act->actorEventCallbacks()->push_back( new DepthSortCallback );

        Geometry* geom = act->lod(0)->as<Geometry>();
        geom->computeNormals();
        if ( geom->drawCalls()[0]->primitiveType() != PT_TRIANGLES_ADJACENCY ) {
          ref< Geometry > geom_adj = AdjacencyExtractor::extract( geom );
          geom->shallowCopyFrom( *geom_adj );
        }

        // const AABB& aabb = act->boundingBox();
        geom->setBoundsDirty( true );
        act->computeBounds();

        #if 0
          // Center
          geom->vertexArray()->transform( mat4::getTranslation( -1.0f * aabb.center() ) );
          geom->setBoundsDirty( true );
          act->computeBounds();
          if ( act->transform() ) {
            act->transform()->computeWorldMatrix();
          }

          // Scale to 1 meter
          float s = 1000.0f / aabb.width();
          geom->vertexArray()->transform( mat4::getScaling( s, s, s ) );
          geom->setBoundsDirty( true );
          act->computeBounds();

          printf("aabb: %f %f %f\n", aabb.width(), aabb.height(), aabb.depth() );
          printf("aabb-min: %f %f %f\n", aabb.minCorner().x(), aabb.minCorner().y(), aabb.minCorner().z() );
          printf("aabb-max: %f %f %f\n", aabb.maxCorner().x(), aabb.maxCorner().y(), aabb.maxCorner().z() );
        #endif

        // Get, init or create the effect using incoming material settings if any
        ref<Effect> fx1 = VividRendering::makeVividEffect( act->effect() );
        act->setEffect( fx1.get() );
        if ( fx1->shader()->getMaterial() ) {
          Shader* sh = fx1->shader();
          sh->gocUniform( "vl_Vivid.material.diffuse" )->setUniform( sh->getMaterial()->frontDiffuse() );
          sh->gocUniform( "vl_Vivid.material.specular" )->setUniform( sh->getMaterial()->frontSpecular() );
          sh->gocUniform( "vl_Vivid.material.ambient" )->setUniform( sh->getMaterial()->frontAmbient() );
          sh->gocUniform( "vl_Vivid.material.emission" )->setUniform( sh->getMaterial()->frontEmission() );
          sh->gocUniform( "vl_Vivid.material.shininess" )->setUniformF( sh->getMaterial()->frontShininess() );
        }

        //if ( ! fx1.get() ) {
        //  fx1 = new Effect;
        //  act->setEffect(fx1.get());
        //}

        // Skip skin
        if ( act->objectName() == "surface:skin" ) {
          continue;
        }

        // bones
        if ( act->objectName() == "surface:bones" ) {
          //  fx1->shader()->gocMaterial()->setDiffuse( fvec4(0.8f, 0.0f, 0.0f, 1) );
          continue;
        }

        // Stencil Test: liver geometry
        if ( act->objectName() == "surface:liver" ) {
          // mVividRendering->stencilActors().push_back( act );
          // mVividRendering->setStencilEnabled( true );
          // continue;
          mLiverActor = act;
          // mLiverActor->setEnableMask( 0 );
        }

        if ( ! fx1->shader()->getMaterial() )
        {
          fx1->shader()->gocMaterial()->setDiffuse( fvec4(1.0f, 0.6f, 0.5f, 1.0) );
          fx1->shader()->gocMaterial()->setSpecular( fvec4(1.0f, 1.0f, 1.0f, 1.0f) );
          fx1->shader()->gocMaterial()->setShininess(128.0f);
        }

#if 0
        if ( j == 11 ) {
          fx1->shader()->gocMaterial()->setDiffuse( fvec4( 1, 1, 1, 1 ) );
          // continue;
        } else if ( j == 3 ) {
          fx1->shader()->gocMaterial()->setDiffuse( fvec4( 0.8f, 0.8f, 0.0f, 0.20 ) );
        } else {
          continue;
        }
#endif

        fx1->shader()->enable( EN_BLEND );
        fx1->shader()->enable( EN_DEPTH_TEST );
        fx1->shader()->enable( EN_LIGHTING );
        fx1->shader()->setRenderState( new Light, 0 );
        fx1->shader()->gocLightModel()->setTwoSide( true );
        fx1->shader()->gocLightModel()->setLocalViewer( true );
        // fx1->shader()->gocDepthMask()->set( false );

#if 0
        // Fog
        fx1->shader()->disable( EN_FOG );
        fx1->shader()->gocFog()->setMode( FM_LINEAR );
        fx1->shader()->gocFog()->setColor( black );
        fx1->shader()->gocFog()->setStart( act->boundingBox().width() * 1 ); // Only used with Linear mode
        fx1->shader()->gocFog()->setEnd( act->boundingBox().width() * 2 );   // Only used with Linear mode
        fx1->shader()->gocFog()->setDensity( 1.0f );                         // Only used with Exp & Exp2 mode
        fx1->shader()->gocUniform("vl_Vivid.fog.mode")->setUniformI( 1 );   // 0=OFF, 1=Linear, 2=Exp, 3=Exp2
        fx1->shader()->gocUniform("vl_Vivid.fog.target")->setUniformI( 0 ); // 0=Color, 1=Alpha, 2=Saturation
#endif

        fx1->shader()->gocUniform("vl_Vivid.outline.color")->setUniform( white );

        // Outline
        if ( act->objectName() == "surface:liver" ) {
          fx1->shader()->setUniform( mLiverOutlineMode.get() );
          fx1->shader()->setUniform( mOutlineSlicePlane.get() );
          fx1->shader()->gocUniform("vl_Vivid.outline.color")->setUniform( royalblue );
          fx1->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( 4 );
          fx1->setRenderRank( 1 );
        } else
        if ( act->objectName() == "surface:liver tumor" ) {
          fx1->shader()->gocUniform("vl_Vivid.outline.color")->setUniform( green );
          fx1->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( 4 );
          fx1->setRenderRank( 2 );
        } else
        if ( act->objectName() == "surface:liver hypervascular lump" ) {
          fx1->shader()->gocUniform("vl_Vivid.outline.color")->setUniform( green );
          fx1->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( 4 );
          fx1->setRenderRank( 2 );
        } else {
          // fx1->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( j == 0 || j == 11 ); // 0=Polys, 1=Outline3D, 2=Polys + Outline
          // fx1->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( 0 ); // 0=Polys, 1=Outline3D, 2=Polys + Outline
          fx1->shader()->setUniform( mOutlineMode.get() );
          fx1->shader()->setUniform( mOutlineSlicePlane.get() );
          // Outline color must be provided
          // fx1->shader()->gocUniform("vl_Vivid.outline.color")->setUniform( j == 11 ? violet : pink );
        }

        // draw all outlines
        // fx1->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( 1 );

#if 0
        // Clipping

        // Clip volume #0
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].mode")->setUniformI( 2 ); // 0=OFF, 1=Sphere, 2=Box, 3=Plane
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].fadeRange")->setUniformF( 0.0 ); // 0=Sharp, 0...=Fuzzy
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].target")->setUniformI( 0 ); // 0=Color, 1=Alpha, 2=Saturation
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].color")->setUniform( royalblue );
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].sphere")->setUniform( vec4( 0, 0, 0, 350.0 ) );  // Sphere X,Y,Z,Radius (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].boxMin")->setUniform( vec3( -250, -250, -250 ) ); // AABB min edge       (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].boxMax")->setUniform( vec3( +250, +250, +250 ) ); // AABB max edge       (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].reverse")->setUniformI( 0 ); // 0=FALSE, 1=TRUE

        // Clip volume #1
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].mode")->setUniformI( 1 ); // 0=OFF, 1=Sphere, 2=Box, 3=Plane
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].fadeRange")->setUniformF( 50.0 ); // 0=Sharp, 0...=Fuzzy
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].target")->setUniformI( 1 ); // 0=Color, 1=Alpha, 2=Saturation
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].color")->setUniform( royalblue );
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].sphere")->setUniform( vec4( 0, -350, 0, 350.0 ) );  // Sphere X,Y,Z,Radius (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].boxMin")->setUniform( vec3( -250, -250, -250 ) ); // AABB min edge       (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].boxMax")->setUniform( vec3( +250, +250, +250 ) ); // AABB max edge       (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].reverse")->setUniformI( 1 ); // 0=FALSE, 1=TRUE

        // Clip volume #2
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].mode")->setUniformI( 3 ); // 0=OFF, 1=Sphere, 2=Box, 3=Plane
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].fadeRange")->setUniformF( 00.0 ); // 0=Sharp, 0...=Fuzzy
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].target")->setUniformI( 2 ); // 0=Color, 1=Alpha, 2=Saturation
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].color")->setUniform( royalblue );
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].plane")->setUniform( vec4( 0, 1, 0, -250 ) );    // Plane Nx,Ny,Nz,Pd (World Coords)
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].reverse")->setUniformI( 1 ); // 0=FALSE, 1=TRUE

        fx1->shader()->gocUniform("vl_Vivid.smartClip[3].mode")->setUniformI( 0 );
#else
        fx1->shader()->gocUniform("vl_Vivid.smartClip[0].mode")->setUniformI( 0 );
        fx1->shader()->gocUniform("vl_Vivid.smartClip[1].mode")->setUniformI( 0 );
        fx1->shader()->gocUniform("vl_Vivid.smartClip[2].mode")->setUniformI( 0 );
        fx1->shader()->gocUniform("vl_Vivid.smartClip[3].mode")->setUniformI( 0 );
#endif

        mVividRendering->sceneManager()->tree()->addActor( act );
        act->setEnableMask( vl::Vivid::VividEnableMask );
      }
    }

    adjustScene();
    // mic fixme:
    // - adjustScene() should take care of this?
    // - why don't we call it before?
    mVividRendering->sceneManager()->computeBounds();
    mVividRendering->sceneManager()->tree()->computeAABB();

#if 1
    // make spherical stencil
    ref< Geometry > icosphere = makeIcosphere( mVividRendering->sceneManager()->tree()->aabb().center() + vec3( -50, 0, 25 ), 100, 3 );
    // ref< Geometry > stencil = makeIcosphere( vec3( 0, 0, 0 ), 100, 3 );
    icosphere->computeNormals();
    // stencil = AdjacencyExtractor::extract( stencil.get() );
    ref< Effect > fx = VividRendering::makeVividEffect();
    ref< Transform > tr1 = new Transform;
    ref< Transform > tr2 = new Transform;
    Actor* stencil_act1 = mVividRendering->sceneManager()->tree()->addActor( icosphere.get(), fx.get(), tr1.get() );
    stencil_act1->setEnableMask( 0 );
    Actor* stencil_act2 = mVividRendering->sceneManager()->tree()->addActor( icosphere.get(), fx.get(), tr2.get() );
    stencil_act2->setEnableMask( 0 );
    mVividRendering->stencilActors().push_back( stencil_act1 );
    mVividRendering->stencilActors().push_back( stencil_act2 );
    tr1->setLocalAndWorldMatrix( vl::mat4::getTranslation(-50,0,0) );
    tr2->setLocalAndWorldMatrix( vl::mat4::getTranslation(+50,0,0) );
#endif

#if 1
    // make points
    {
      ref< Geometry > points = makeIcosphere( mVividRendering->sceneManager()->tree()->aabb().center() + vec3( -50, 0, 25 ), 100, 3 );
      points->drawCalls().clear();
      points->drawCalls().push_back( new vl::DrawArrays( vl::PT_POINTS, 0, points->vertexArray()->size() ) );
      points->setColorArray( vl::yellow );
      fx = VividRendering::makeVividEffect();
      fx->shader()->getPointSize()->set( 20 );
      Actor* points_act = mVividRendering->sceneManager()->tree()->addActor( points.get(), fx.get(), NULL );
  #if 1
      points_act->setEnableMask( vl::Vivid::VividEnableMask );
      fx->shader()->getUniform( "vl_Vivid.enableLighting" )->setUniformI( 0 );
      fx->shader()->getUniform( "vl_Vivid.enablePointSprite" )->setUniformI( 1 );
      fx->shader()->gocUniform( "vl_Vivid.enableTextureMapping" )->setUniformI( 1 );
      ref<vl::Image> img = new Image("/images/sphere.png");
      ref<vl::Texture> texture = fx->shader()->getTextureSampler( vl::Vivid::UserTexture )->texture();
      texture->createTexture2D( img.get(), vl::TF_UNKNOWN, false, false );
  #else
      points_act->setEnableMask( vl::VividRenderer::StandardEnableMask );
      fx->shader()->enable( vl::EN_POINT_SMOOTH );
      fx->shader()->enable( vl::EN_BLEND );
      fx->shader()->disable( vl::EN_LIGHTING );
      fx->shader()->gocHint()->setPointSmoothHint( vl::HM_FASTEST );
  #endif
    }
#endif

#if 0
    // fog
    ref< Fog > fog = new Fog();
    fog->setMode( FM_LINEAR );
    fog->setColor( black );
    fog->setStart( 100 /*mVividRendering->sceneManager()->boundingBox().width() * 1*/ ); // Only used with Linear mode
    fog->setEnd( 150 /*mVividRendering->sceneManager()->boundingBox().width() * 2*/ );   // Only used with Linear mode
    fog->setDensity( 1.0f );                                    // Only used with Exp & Exp2 mode

    // uniforms
    mFogMode = new Uniform( "vl_Vivid.fog.mode" );
    mFogMode->setUniformI( 0 );   // 0=OFF, 1=Linear, 2=Exp, 3=Exp2
    mFogTarget = new Uniform( "vl_Vivid.fog.target" );
    mFogTarget->setUniformI( 0 ); // 0=Color, 1=Alpha, 2=Saturation

    ActorCollection* actors = mVividRendering->sceneManager()->tree()->actors();
    for( int i=0; i<actors->size(); ++i ) {
      Shader* sh = actors->at( i )->effect()->shader();
        sh->disable( EN_FOG );
        sh->setRenderState( fog.get() );
        sh->setUniform( mFogMode.get() );
        sh->setUniform( mFogTarget.get() );
    }
#endif

    // Volume
    vec3 volume_box_center = mVividRendering->sceneManager()->tree()->aabb().center();
    AABB volume_box;
    volume_box.setMinCorner( volume_box_center - vec3(100,50,50) );
    volume_box.setMaxCorner( volume_box_center + vec3(100,50,50) );
    printf("%f %f %f\n", volume_box.width(), volume_box.height(), volume_box.depth() );
#if 1
    ref< Image > volume = vl::loadImage("/volume/BostonTeapot.dat");
    mVividVolume->setupVolume( volume.get(), volume_box, NULL );
    Actor* volume_actor = mVividVolume->volumeActor();
    // mVividRendering->sceneManager()->tree()->eraseActor( volume_actor );
    mVividRendering->sceneManager()->tree()->addActor( volume_actor );

    // volume bounding box outline
    ref<Effect> fx_box = VividRendering::makeVividEffect();
    fx_box->shader()->gocLineWidth()->set( 3 );
    fx_box->shader()->gocPolygonMode()->set( PM_LINE, PM_LINE );
    fx_box->shader()->gocUniform( "vl_Vivid.enableLighting" )->setUniformI( 0 );
    ref<Geometry> box_outline = vl::makeBox( volume_box );
    box_outline->computeNormals();
    box_outline->setColorArray( vl::red );
    Actor* vol_box = mVividRendering->sceneManager()->tree()->addActor( box_outline.get(), fx_box.get(), mVividVolume->volumeTransform() );
    vol_box->setEnableMask( vl::Vivid::VividEnableMask );
    vol_box->setObjectName( "Volume Box" );
#endif

#if 1
    // texture mapped quad
    ref<Image> vl_img = loadImage( "/tmp/liver/VL-Stencil.png" );
    ref<Geometry> quad = vl::makeGrid( volume_box.center(), volume_box.width(), volume_box.depth(), 10, 10, true );
    fx = vl::VividRendering::makeVividEffect();
    Actor* quad_act = mVividRendering->sceneManager()->tree()->addActor( quad.get(), fx.get(), NULL );
    quad->setObjectName( "QUAD" );
    quad_act->setEnableMask( vl::Vivid::VividEnableMask );
    fx->shader()->getUniform("vl_Vivid.enableTextureMapping")->setUniformI( 1 );
    fx->shader()->getUniform("vl_Vivid.enableLighting")->setUniformI( 0 );
    // When texture mapping is enabled texture is modulated by vertex color
    quad->setColorArray( vl::white );
    // These must be present as part of the default Vivid material
    VL_CHECK( fx->shader()->getTextureSampler( vl::Vivid::UserTexture ) )
    VL_CHECK( fx->shader()->getTextureSampler( vl::Vivid::UserTexture )->texture() )
    VL_CHECK( fx->shader()->getUniform("vl_UserTexture2D") );
    VL_CHECK( fx->shader()->getUniform("vl_UserTexture2D")->getUniformI() == vl::Vivid::UserTexture );
    ref<vl::Texture> texture = fx->shader()->getTextureSampler( vl::Vivid::UserTexture )->texture();
    // Recreate new texture (TexParameter is not reset so we can keep the current defaults)
    texture->createTexture2D( vl_img.get(), vl::TF_UNKNOWN, false, false );
#endif
    adjustScene();
  }

  void saveActor( Actor* act ) {
      ref< ResourceDatabase > db = new ResourceDatabase;
      // db->resources().push_back( act );
      // String fname = filename( files[i] );
      db->resources().push_back( act );
      Geometry* geom = act->lod(0)->as< Geometry >();
      ref< ArrayAbstract > na = geom->normalArray();
      geom->setNormalArray( NULL );
      String fname = mLastModelName;
      saveVLT( "C:/git-ucl/VisualizationLibrary/data/tmp/_" + fname + ".vlt", db.get() );
      saveVLB( "C:/git-ucl/VisualizationLibrary/data/tmp/_" + fname + ".vlb", db.get() );
      geom->setNormalArray( na.get() );
  }
  //--------------------------------------------------------------------------
  void keyPressEvent(unsigned short unicode_ch, EKey /*key*/)
  {
    // BaseDemo::keyReleaseEvent(unicode_ch, key);

    switch((unsigned char)tolower(unicode_ch))
    {
    case 'b':
      mVividRendering->setBackgroundImageEnabled( ! mVividRendering->backgroundImageEnabled() );
      if ( mVividRendering->backgroundImageEnabled() ) {
        // mVividRendering->setBackgroundImage( mBackgroundImage.get() );
        mVividRendering->backgroundTexSampler()->texture()->createTexture2D( mBackgroundImage.get(), TF_RGBA, false, false );
      }
      break;
    case 'v':
      mVividRendering->setStencilEnabled( ! mVividRendering->isStencilEnabled() );
      break;
    case 'n':
      mOutlineMode->setUniformI( (mOutlineMode->getUniformI() + 1) % 6 );
      break;
    case 'r':
      mVivid->initShaders();
      mVividVolume->volumeGLSLProgram()->reload();
      break;
    case 'q':
      mVivid->setUseQueryObject(!mVivid->useQueryObject());
      break;
    case '+':
      mVivid->setDetphPeelingPasses(mVivid->depthPeelingPasses() + 1);
      break;
    case '-':
      if (mVivid->depthPeelingPasses() > 1) {
        mVivid->setDetphPeelingPasses(mVivid->depthPeelingPasses() - 1);
      }
      break;
    case '1':
      mVividRendering->setRenderingMode(Vivid::DepthPeeling);
      break;
    case '2':
      mVividRendering->setRenderingMode(Vivid::FastRender);
      break;
    case '3':
      mVividRendering->setRenderingMode(Vivid::StencilRender);
      break;
    case 's':
      saveActor( this->mVividRendering->sceneManager()->tree()->actors()->at(0) );
      break;
    case 'a': {
        float opacity = mVividRendering->opacity() - 0.025f;
        opacity = max(opacity, 0.0f);
        mVividRendering->setOpacity( opacity );
        printf("opacity: %f\n", opacity);
      }
      break;
    case 'd': {
        float opacity = mVividRendering->opacity() + 0.025f;
        opacity = min(opacity, 1.0f);
        mVividRendering->setOpacity( opacity );
        printf("opacity: %f\n", opacity);
      }
      break;
    case 'o':
      mFogMode->setUniformI( ( mFogMode->getUniformI() + 1 ) % 4 );
      break;
    case 'p':
      mFogTarget->setUniformI( ( mFogTarget->getUniformI() + 1 ) % 3 );
      break;
    case 'i':
      mLiverOutlineMode->setUniformI( ( mLiverOutlineMode->getUniformI() + 1 ) % 6 );
      // mLiverActor->setEnableMask( mLiverOutlineMode->getUniformI() == 1 );
      break;
    }

    const char* method[] = { "DepthPeeling", "FastRender", "StencilRender" };
    printf("method:           %s\n", method[mVividRendering->renderingMode()]);
    printf("pass counter:     %d\n", mVivid->passCounter());
    printf("num passes:       %d\n", mVivid->depthPeelingPasses());
    printf("use query object: %d\n", mVivid->useQueryObject());
    printf("---\n");
    openglContext()->update();
  }

  void resizeEvent(int w, int h)
  {
    mVividRendering->camera()->viewport()->set( 0, 0, w, h );
    mVividRendering->camera()->setProjectionPerspective();
  }

  void updateEvent() {
    openglContext()->makeCurrent();

    // FPS counter
    if (Time::currentTime() - mStartTime > 0.500f)
    {
      double secs = (Time::currentTime() - mStartTime);
      mFPS = mFrameCount / secs;
      mFrameCount = 0;
      mStartTime = Time::currentTime();
    }
    mFrameCount++;

    // update the scene content
    updateScene();

    // set frame time for all the rendering
    real now_time = Time::currentTime();
    mVividRendering->setFrameClock( now_time );

    // execute rendering
    mVividRendering->render( openglContext()->framebuffer() );

    // show rendering
    if ( openglContext()->hasDoubleBuffer() ) {
      openglContext()->swapBuffers();
    }

    VL_CHECK_OGL();

    // useful for debugging
    // wglMakeCurrent(NULL,NULL);

    if ( mFPSTimer.elapsed() > 1 )
    {
      mFPSTimer.start();
      openglContext()->setWindowTitle( vl::Say("[%.1n] %s") << fps() << appletName()  + " - " + vl::String("VL ") + vl::VisualizationLibrary::versionString() );
      vl::Log::print( vl::Say("FPS=%.1n\n") << fps() );
    }

    // update textured quad
    //if ( mVivid->backgroundImage() == mBackgroundImage.get() && mVivid->backgroundImageEnabled() ) {
    //  if ( fract( Time::currentTime() ) < 0.25f ) {
    //    memcpy( mBackgroundImage->pixels(), mBackgroundImages[0]->pixels(), mBackgroundImage->requiredMemory() );
    //    mVivid->updateBackgroundImage();
    //  } else
    //  if ( fract( Time::currentTime() ) < 0.50f ) {
    //    memcpy( mBackgroundImage->pixels(), mBackgroundImages[1]->pixels(), mBackgroundImage->requiredMemory() );
    //    mVivid->updateBackgroundImage();
    //  } else
    //  if ( fract( Time::currentTime() ) < 0.75f ) {
    //    memcpy( mBackgroundImage->pixels(), mBackgroundImages[2]->pixels(), mBackgroundImage->requiredMemory() );
    //    mVivid->updateBackgroundImage();
    //  } else {
    //    memcpy( mBackgroundImage->pixels(), mBackgroundImages[3]->pixels(), mBackgroundImage->requiredMemory() );
    //    mVivid->updateBackgroundImage();
    //  }
    //}
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

  void fileDroppedEvent(const std::vector<String>& files)
  {
    loadModel(files);
    // update the rendering
    openglContext()->update();
  }

protected:
  ref<VividRendering> mVividRendering;
  ref<VividRenderer> mVivid;
  ref<VividVolume> mVividVolume;

  std::set<ref<Effect>> mEffects;
  std::vector<String> mLastShaders;
  String mLastModelName;
  ref<Image> mBackgroundImages[4];
  ref<Image> mBackgroundImage;
  ref<Uniform> mOutlineMode;
  ref<Uniform> mOutlineSlicePlane;
  ref<Uniform> mFogMode;
  ref<Uniform> mFogTarget;
  ref<Uniform> mLiverOutlineMode;
  ref<Actor> mLiverActor;
};

// Have fun!

BaseDemo* Create_App_Vivid() { return new App_Vivid; }
