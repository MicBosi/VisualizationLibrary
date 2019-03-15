/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

#include <vlVivid/VividRendering.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
VividRendering::VividRendering() {
  VL_DEBUG_SET_OBJECT_NAME()

  setNearFarClippingPlanesOptimized( false );

  mVividRenderer = new VividRenderer( this );
  // mVividRenderer->setFramebuffer( framebuffer ); // set by the render() method.
  setRenderer( mVividRenderer.get() );

  mSceneManagerActorTree = new vl::SceneManagerActorTree;
  sceneManagers()->push_back( mSceneManagerActorTree.get() );

  mCalibratedCamera = new vl::CalibratedCamera;
  setCamera( mCalibratedCamera.get() );
  mCalibratedCamera->viewport()->setScissorEnabled( true );
  mCalibratedCamera->viewport()->setClearColor( vl::black );
  mCalibratedCamera->setNearPlane( 2 );
  mCalibratedCamera->setFarPlane( 5000 );

  // Settings

  mRenderingMode = Vivid::DepthPeeling;

  mOpacity = 1;

  mOutline3DEyeOffset  = 0; // 0.25mm works well with VTK
  mOutline3DClipOffset = 0; // 0.0005 is more general but can create artifacts

  mBackgroundImageEnabled = false;

  // Stencil Texture

  mStencilSmoothness = 10;
  mStencilBackground = vl::black;
  mStencilEnabled = false;

  // Depth Peeling Throttle

  m_DepthPeelingAutoThrottleEnabled = true;
}
ref<Effect> VividRendering::makeVividEffect(Effect* effect) {

  // Here we set only the uniforms that are not marked as <automatic> in vivid-uniforms.glsl

  ref<Effect> fx = effect == NULL ? new Effect() : effect;

  fx->shader()->gocLineWidth();
  fx->shader()->gocPointSize();
  fx->shader()->gocPolygonMode();

  // Material
  fx->shader()->gocUniform( "vl_Vivid.material.diffuse" )->setUniform( fvec4(0.8f, 0.8f, 0.8f, 1) );
  fx->shader()->gocUniform( "vl_Vivid.material.specular" )->setUniform( fvec4(0, 0, 0, 1) );
  fx->shader()->gocUniform( "vl_Vivid.material.ambient" )->setUniform( fvec4(0.2f, 0.2f, 0.2f, 1) );
  fx->shader()->gocUniform( "vl_Vivid.material.emission" )->setUniform( fvec4(0, 0, 0, 1) );
  fx->shader()->gocUniform( "vl_Vivid.material.shininess" )->setUniformF( 128 );

  // Fog
  fx->shader()->gocUniform("vl_Vivid.fog.mode")->setUniformI( 0 );   // 0=Off, 1=Linear, 2=Exp, 3=Exp2
  fx->shader()->gocUniform("vl_Vivid.fog.target")->setUniformI( 0 ); // 0=Color, 1=Alpha, 2=Saturation
  fx->shader()->gocUniform("vl_Vivid.fog.start")->setUniformF( 0 );
  fx->shader()->gocUniform("vl_Vivid.fog.end")->setUniformF( 1 );
  fx->shader()->gocUniform("vl_Vivid.fog.density")->setUniformF( 1 );
  fx->shader()->gocUniform("vl_Vivid.fog.color")->setUniform( vl::black );

  // Light
  fx->shader()->gocUniform( "vl_Vivid.light.diffuse" )->setUniform( fvec4(1, 1, 1, 1) );
  fx->shader()->gocUniform( "vl_Vivid.light.specular" )->setUniform( fvec4(1, 1, 1, 1) );
  fx->shader()->gocUniform( "vl_Vivid.light.ambient" )->setUniform( fvec4(0, 0, 0, 1) );
  fx->shader()->gocUniform( "vl_Vivid.light.position" )->setUniform( fvec4(0, 0, 0, 1) );

  fx->shader()->gocUniform( "vl_Vivid.enableLighting" )->setUniformI( 1 );
  fx->shader()->gocUniform( "vl_Vivid.enablePointSprite" )->setUniformI( 0 );

  // Outline
  fx->shader()->gocUniform("vl_Vivid.renderMode")->setUniformI( 0 ); // 0=Polys, 1=Outline3D, 2=Polys+Outline3D, 3=Slice, 4=Outline2D, 5=Polys+Outline2D
  fx->shader()->gocUniform("vl_Vivid.outline.color")->setUniform( vl::white );
  fx->shader()->gocUniform("vl_Vivid.outline.width")->setUniformF( 2.0f );
  fx->shader()->gocUniform("vl_Vivid.outline.slicePlane")->setUniform( vl::vec4( 1, 0, 0, 0 ) );

  // Smart Clipping
  for( char i = '0'; i < '4'; ++i ) {
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].mode").c_str())->setUniformI( 0 ); // 0=OFF, 1=Sphere, 2=Box, 3=Plane
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].target").c_str())->setUniformI( 0 ); // 0=Color, 1=Alpha, 2=Saturation
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].fadeRange").c_str())->setUniformF( 0.0 ); // 0=Sharp, 0...X=Fuzzy
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].color").c_str())->setUniform( vl::black );
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].plane").c_str())->setUniform( vl::vec4( 0, 0, 0, 0 ) );  // Plane Nx, Ny, Nz, Pd (distance from origin) (World Coords)
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].sphere").c_str())->setUniform( vl::vec4( 0, 0, 0, 0 ) ); // Sphere X, Y, Z, Radius (World Coords)
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].boxMin").c_str())->setUniform( vl::vec3( 0, 0, 0 ) ); // AABB min edge (World Coords)
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].boxMax").c_str())->setUniform( vl::vec3( 0, 0, 0 ) ); // AABB max edge (World Coords)
    fx->shader()->gocUniform((std::string("vl_Vivid.smartClip[") + i + "].reverse").c_str())->setUniformI( 0 ); // 0=false, 1=true (Reverses what's in and what's out)
  }

  // User Texture Mapping

  ref<vl::Texture> texture = new Texture( 32, 32, vl::TF_RGBA );
  texture->getTexParameter()->setAnisotropy( 16 );
  texture->getTexParameter()->setMinFilter( vl::TPF_LINEAR );
  texture->getTexParameter()->setMagFilter( vl::TPF_LINEAR );
  texture->getTexParameter()->setWrap( vl::TPW_CLAMP_TO_EDGE );
  fx->shader()->gocTextureSampler( vl::Vivid::UserTexture )->setTexture( texture.get() );
  fx->shader()->gocUniform( "vl_UserTexture1D" )->setUniformI( vl::Vivid::UserTexture );
  fx->shader()->gocUniform( "vl_UserTexture2D" )->setUniformI( vl::Vivid::UserTexture );
  fx->shader()->gocUniform( "vl_UserTexture3D" )->setUniformI( vl::Vivid::UserTexture );
  fx->shader()->gocUniform( "vl_Vivid.enableTextureMapping" )->setUniformI( 0 );
  fx->shader()->gocUniform( "vl_Vivid.textureDimension" )->setUniformI( 0 );

  // Used by the fast renderer - ignored by depth peeling

  fx->shader()->enable( vl::EN_BLEND );
  fx->shader()->enable( vl::EN_DEPTH_TEST );
  fx->shader()->enable( vl::EN_LIGHTING );

  fx->shader()->setRenderState( new vl::Light, 0 );

  fx->shader()->gocMaterial()->setColorMaterialEnabled( false );
  fx->shader()->gocMaterial()->setColorMaterial( vl::PF_FRONT_AND_BACK, vl::CM_DIFFUSE );

  fx->shader()->gocLightModel()->setTwoSide( true );
  fx->shader()->gocLightModel()->setLocalViewer( true );

  fx->shader()->gocDepthMask()->set( true );

  return fx;
}
//-----------------------------------------------------------------------------
