/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

#include <vlVivid/VividVolume.hpp>
#include <vlVivid/VividRendering.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Camera.hpp>
#include <vlCore/Vector3.hpp>
#include <vlVolume/VolumeUtils.hpp>
#include <vlCore/Time.hpp>

using namespace vl;

/** \class vl::VividVolume
 * A ActorEventCallback used to render a volume using GPU raycasting.
 *
 * Pictures from: \ref pagGuideVividVolume tutorial.
 *
 * <center>
 * <table border=0 cellspacing=0 cellpadding=5>
 * <tr>
 * 	<td> <img src="pics/pagGuideVividVolume_1.jpg"> </td>
 * 	<td> <img src="pics/pagGuideVividVolume_2.jpg"> </td>
 * 	<td> <img src="pics/pagGuideVividVolume_3.jpg"> </td>
 * </tr>
 * <tr>
 * 	<td> <img src="pics/pagGuideVividVolume_4.jpg"> </td>
 * 	<td> <img src="pics/pagGuideVividVolume_5.jpg"> </td>
 * 	<td> <img src="pics/pagGuideVividVolume_6.jpg"> </td>
 * </tr>
 * </table>
 * </center>
 *
 * \sa
 * - \ref pagGuideVividVolume
 * - \ref pagGuideSlicedVolume
 * - SlicedVolume
 *
 */
// TODO:
// - Within the volume rendering shader values are all mapped to 0..1 however we could
//   pass a `vec2 vl_Vivid.volume.dataRange` uniform to inform the shader of what the
//   original data rage was so we can map back to it. We could use this range also to map
//   the Iso value. We could use this to support more easily things like Hounsfield units etc.
VividVolume::VividVolume(VividRendering* vivid_rendering)
{
  VL_DEBUG_SET_OBJECT_NAME()
  mVividRendering = vivid_rendering;

  // box geometry
  mGeometry = new Geometry;

  // install vertex coords array
  mVertCoord = new ArrayFloat3;
  mVertCoord->resize( 8 );
  mGeometry->setVertexArray( mVertCoord.get() );

  // install texture coords array
  mTexCoord = new ArrayFloat3;
  mTexCoord->resize( 8 );
  mGeometry->setTexCoordArray( 0, mTexCoord.get() );

  // install index array
  ref<DrawElementsUInt> de = new DrawElementsUInt( PT_QUADS );
  mGeometry->drawCalls().push_back( de.get() );
  unsigned int de_indices[] =
  {
    0,1,2,3, 1,5,6,2, 5,4,7,6, 4,0,3,7, 3,2,6,7, 4,5,1,0
  };
  de->indexBuffer()->resize( 4*6 );
  memcpy( de->indexBuffer()->ptr(), de_indices, sizeof( de_indices ) );

  // generate default texture coordinates
  fvec3 texc[] =
  {
    fvec3( 0,0,0 ), fvec3( 1,0,0 ), fvec3( 1,1,0 ), fvec3( 0,1,0 ),
    fvec3( 0,0,1 ), fvec3( 1,0,1 ), fvec3( 1,1,1 ), fvec3( 0,1,1 )
  };
  memcpy( mTexCoord->ptr(), texc, sizeof( texc ) );

  // default box dimensions and geometry
  setBox( AABB( vec3( 0,0,0 ), vec3( 1,1,1 ) ) );

  // --- --- ---

  mVolumeMode = Isosurface;
  mSamplesPerRay = 512;
  mVolumeDensity = 4;
  mIsoValue = 0.5f;

  mVolumeActor = new Actor;
  mVolumeFX = new Effect;
  mVolumeTransform = new Transform;

  mVolumeActor->setEnableMask( Vivid::StandardEnableMask );
  mVolumeActor->setObjectName( "Volume Actor" );

  bindActor( mVolumeActor.get() );
  mVolumeActor->setEffect( mVolumeFX.get() );
  mVolumeActor->setTransform( mVolumeTransform.get() );
  mVolumeActor->gocUniform( "vl_Vivid.opacity" )->setUniform( mVividRendering->opacity() );

  // do not read nor write the depth buffer - we do it in the shader
  mVolumeFX->shader()->disable( EN_DEPTH_TEST );
  mVolumeFX->shader()->gocDepthMask()->set( false );
  // enable standard blending
  mVolumeFX->shader()->enable( EN_BLEND );
  // add one light
  mVolumeFX->shader()->setRenderState( new Light, 0 );
  // back to front rendering for correct blending
  mVolumeFX->shader()->enable( EN_CULL_FACE );
  mVolumeFX->shader()->gocCullFace()->set( PF_FRONT );
}
//-----------------------------------------------------------------------------
void VividVolume::updateUniforms( vl::Actor*actor, vl::real, const vl::Camera* camera, vl::Renderable*, const vl::Shader* shader )
{
  const GLSLProgram* glsl = shader->getGLSLProgram();
  VL_CHECK( glsl );

  // used later
  fmat4 inv_mat;
  if ( actor->transform() ) {
    inv_mat = ( fmat4 )actor->transform()->worldMatrix().getInverse();
  }

  if ( glsl->getUniformLocation( "eyePosition" ) != -1 )
  {
    // pass the eye position in object space

    // eye postion
    fvec3 eye = ( fvec3 )camera->modelingMatrix().getT();
    // world to object space
    if ( actor->transform() )
      eye = inv_mat * eye;
    actor->gocUniform( "eyePosition" )->setUniform( eye );
  }

  const vl::TextureSampler* tex_sampler = shader->getTextureSampler( 0 );
  VL_CHECK( tex_sampler );
  // Normalized x/y/z offeset required to center on a texel
  const Texture* tex = tex_sampler->texture();
  VL_CHECK( tex );
  fvec3 centering = fvec3(
    0.5f / tex->width(),
    0.5f / tex->height(),
    0.5f / tex->depth()
  );
  actor->gocUniform( "volumeDelta" )->setUniform( centering );

  // Compute gradient delta: 0.25 seems to produce better results than 0.5.
  actor->gocUniform( "gradientDelta" )->setUniform( fvec3( 0.25f / tex->width(), 0.25f / tex->height(), 0.25f / tex->depth() ) );

  // Automatic sample step computataion
  // NOTE:
  // Ideally we would adapt this to how big the volume is on the screen.
  float sample_step = 1.0f / samplesPerRay();
  actor->gocUniform( "sampleStep" )->setUniform( sample_step );

  // Volume density
  // NOTE:
  // Automatically compensates for the change in SamplesPerRay
  float volume_density = sample_step * volumeDensity();
  actor->gocUniform( "volumeDensity" )->setUniform( volume_density );

  // volumeMode
  actor->gocUniform( "volumeMode" )->setUniform( volumeMode() );

  // Iso value
  actor->gocUniform( "isoValue" )->setUniform( isoValue() );

  // Volume Scaling Correction for non cubic volumes
  vec3 boxdim = vec3( box().width(), box().height(), box().depth() );
  float min_size = min( min( boxdim.x(), boxdim.y() ), boxdim.z() );
  vec3 volumeScalingCorrection = vec3(1,1,1) / (boxdim / min_size);
  actor->gocUniform( "volumeScalingCorrection" )->setUniform( volumeScalingCorrection );

  // vl_Vivid.opacity
  actor->gocUniform( "vl_Vivid.opacity" )->setUniform( mVividRendering->opacity() );

  // volume bounding box min & max corners
  actor->gocUniform( "volumeBoxMin" )->setUniform( box().minCorner() );
  actor->gocUniform( "volumeBoxMax" )->setUniform( box().maxCorner() );
}
//-----------------------------------------------------------------------------
void VividVolume::bindActor( Actor* actor )
{
  actor->actorEventCallbacks()->erase( this );
  actor->actorEventCallbacks()->push_back( this );
  actor->setLod( 0, mGeometry.get() );
}
//-----------------------------------------------------------------------------
void VividVolume::onActorRenderStarted( Actor* actor, real clock, const Camera* camera, Renderable* rend, const Shader* shader, int pass )
{
  if ( pass>0 )
    return;

  // setup uniform variables

  if ( shader->getGLSLProgram() ) {
    updateUniforms( actor, clock, camera, rend, shader );
  }
}
//-----------------------------------------------------------------------------
void VividVolume::generateTextureCoordinates( const ivec3& img_size )
{
  if ( ! img_size.x() || ! img_size.y() || ! img_size.z() )
  {
    Log::error( "VividVolume::generateTextureCoordinates(): failed! The size passed does not represent a 3D image.\n" );
    return;
  }

  float dx = 0.5f / img_size.x();
  float dy = 0.5f / img_size.y();
  float dz = 0.5f / img_size.z();

  float x0 = 0.0f + dx;
  float x1 = 1.0f - dx;
  float y0 = 0.0f + dy;
  float y1 = 1.0f - dy;
  float z0 = 0.0f + dz;
  float z1 = 1.0f - dz;

  fvec3 texc[] =
  {
    fvec3( x0,y0,z1 ), fvec3( x1,y0,z1 ), fvec3( x1,y1,z1 ), fvec3( x0,y1,z1 ),
    fvec3( x0,y0,z0 ), fvec3( x1,y0,z0 ), fvec3( x1,y1,z0 ), fvec3( x0,y1,z0 ),
  };

  memcpy( mTexCoord->ptr(), texc, sizeof( texc ) );
}
//-----------------------------------------------------------------------------
void VividVolume::setBox( const AABB& box )
{
  mBox = box;
  // generate the box geometry
  float x0 = box.minCorner().x();
  float y0 = box.minCorner().y();
  float z0 = box.minCorner().z();
  float x1 = box.maxCorner().x();
  float y1 = box.maxCorner().y();
  float z1 = box.maxCorner().z();
  fvec3 box_verts[] =
  {
    fvec3( x0,y0,z1 ), fvec3( x1,y0,z1 ), fvec3( x1,y1,z1 ), fvec3( x0,y1,z1 ),
    fvec3( x0,y0,z0 ), fvec3( x1,y0,z0 ), fvec3( x1,y1,z0 ), fvec3( x0,y1,z0 ),
  };
  memcpy( mVertCoord->ptr(), box_verts, sizeof( box_verts ) );
  mGeometry->setBoundsDirty( true );
}
//-----------------------------------------------------------------------------
void VividVolume::setupVolume( Image* img, const AABB& bounds, Image* transfer_func) {
  if ( ! mVolumeGLSLProgram ) {
    mVolumeGLSLProgram = mVolumeFX->shader()->gocGLSLProgram();
    mVolumeGLSLProgram->attachShader( new GLSLVertexShader("/vivid/glsl/volume/raycast.vs") );
    mVolumeGLSLProgram->attachShader( new GLSLFragmentShader("/vivid/glsl/volume/raycast.fs") );
    mVolumeGLSLProgram->linkProgram();
  }

  setBox( bounds );

  // Texture #0: volume data
  vl::ref< vl::Texture > vol_tex = new vl::Texture( img );
  vol_tex->getTexParameter()->setMagFilter( vl::TPF_LINEAR );
  vol_tex->getTexParameter()->setMinFilter( vl::TPF_LINEAR );
  vol_tex->getTexParameter()->setWrap( vl::TPW_CLAMP_TO_EDGE );
  mVolumeFX->shader()->gocTextureSampler( 0 )->setTexture( vol_tex.get() );
  mVolumeActor->gocUniform( "volumeTexture" )->setUniformI( 0 );
  generateTextureCoordinates( ivec3( img->width(), img->height(), img->depth() ) );

  // Default transfer function image
  ref<Image> trf_img = transfer_func ? transfer_func : vl::makeColorSpectrum(128, vl::blue, vl::aqua, vl::green, vl::yellow, vl::red, vl::violet);

  // Texture #1: transfer function
  vl::ref< vl::Texture > trf_tex = new Texture( trf_img.get() );
  trf_tex->getTexParameter()->setMagFilter( vl::TPF_LINEAR );
  trf_tex->getTexParameter()->setMinFilter( vl::TPF_LINEAR );
  trf_tex->getTexParameter()->setWrap( vl::TPW_CLAMP_TO_EDGE );
  mVolumeFX->shader()->gocTextureSampler( 1 )->setTexture( trf_tex.get() );
  mVolumeActor->gocUniform( "transferFunctionTexture" )->setUniformI( 1 );

  // Texture: #2: gradient texture
  float t0 = vl::Time::currentTime();
  Log::print( String::printf( "Generating gradient texture (%dx%dx%d)... ", img->width(), img->height(), img->depth() ) );
  ref<Image> gradient = vl::genGradientNormals( img );
  Log::print( String::printf( "done (%.1fs).\n", vl::Time::currentTime() - t0 ) );
  ref<Texture> gra_tex = new Texture( gradient.get(), TF_RGB8, false, false );
  gra_tex->getTexParameter()->setMagFilter( vl::TPF_LINEAR );
  gra_tex->getTexParameter()->setMinFilter( vl::TPF_LINEAR );
  gra_tex->getTexParameter()->setWrap( vl::TPW_CLAMP_TO_EDGE );
  mVolumeFX->shader()->gocTextureSampler( 2 )->setTexture( gra_tex.get() );
  mVolumeActor->gocUniform( "gradientTexture" )->setUniformI( 2 );

  // Texture: #3: depth texture
  mVolumeFX->shader()->gocTextureSampler( 3 )->setTexture( mVividRendering->depthFBOTex() );
  mVolumeActor->gocUniform( "depthBuffer" )->setUniformI( 3 );

  // Texel centering offset
  mVolumeActor->gocUniform( "transferFunctionDelta" )->setUniformF( 0.5f / trf_img->width() );

  // Compute gradient delta: 0.25 seems to produce better results than 0.5.
  mVolumeActor->gocUniform( "gradientDelta" )->setUniform( fvec3( 0.25f / vol_tex->width(), 0.25f / vol_tex->height(), 0.25f / vol_tex->depth() ) );

  // Defaults
  mVolumeActor->gocUniform( "isoValue" )->setUniformF( mIsoValue );
}
