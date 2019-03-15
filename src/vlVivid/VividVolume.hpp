/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

#include <vlVivid/link_config.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Actor.hpp>

#ifndef VividVolume_INCLUDE_ONCE
#define VividVolume_INCLUDE_ONCE

namespace vl
{
  class VividRendering;

  /**
   * Used to setup and manages a volume rendering within a vl::VividRendering.
   *
   * Features:
   * - High quality ray-marching based rendering
   * - Direct, Isosurface (incl. transparent) and MIP modes
   * - Proper intersections with opaque objects via special VividRendering depth buffer
   * - Supports rectangular volumes
   * - Can have an associate Transform with arbitrary rotation and translation (not scaling)
   * - Supports lighting (1 light)
   * - Fast rendering using pre-computed high quality gradient texture
   * - Fast and arbitrary RGBA transfer function via 1D texture lookup
   *
   * Whish List:
   * - Support nested & intersecting volumes
   * - Adaptive ray-step: adapt ray sampling step to size of volume on the screen and distance from viewer (a la mip-mapping) producing a faster and higher quality rendering.
   * - Advanced lighting, shadows, scattering etc.
   */
  class VLVIVID_EXPORT VividVolume: public ActorEventCallback
  {
    VL_INSTRUMENT_CLASS(vl::VividVolume, ActorEventCallback)
  public:
    typedef enum {
      Direct,
      Isosurface,
      MIP
    } EVolumeMode;

  public:
    VividVolume(VividRendering* vivid_rendering);

    //! Initializes all the volume data, uniforms and textures.
    //!
    //! \param volume The 3D image representing the volume, typcally of IF_LUMINANCE format.
    //! \param volume_bounds The AABB of the volume, can be a rectangle and defines the object space dimensions of the volume.
    //! \param transfer_function A 1D image representing the RGBA values associated to a given `luminance` value (used by all modes).
    void setupVolume( Image* volume, const AABB& volume_bounds, Image* transfer_function);

    //! Select volume rendering mode: Direct, Isosurface, MIP
    void setVolumeMode( EVolumeMode mode ) { mVolumeMode = mode; }
    EVolumeMode volumeMode() { return mVolumeMode; }

    //! Iso value for Isosurface mode.
    void setIsoValue( float iso ) { mIsoValue = iso; }
    float isoValue() const { return mIsoValue; }

    //! Maximum number of samples to take along each ray. The higher the samples the higher the quality of the rendering (at the expense of rendering time).
    void setSamplesPerRay( int samples ) { mSamplesPerRay = samples; }
    int samplesPerRay() const { return mSamplesPerRay; }

    //! The density of the volume. Only used in Direct mode.
    void setVolumeDensity( float density ) { mVolumeDensity = density; }
    float volumeDensity() const { return mVolumeDensity; }

    //! The Actor to which the volume is attached. Used to enable/disable the Actor and modify the Transform.
    Actor* volumeActor() { return mVolumeActor.get(); }
    const Actor* volumeActor() const { return mVolumeActor.get(); }

    //! The Transform to which the volume is attached.
    Transform* volumeTransform() { return mVolumeTransform.get(); }
    const Transform* volumeTransform() const { return mVolumeTransform.get(); }

    //! The Effect used to render the volume, usually you don't need to touch this.
    Effect* volumeFX() { return mVolumeFX.get(); }
    const Effect* volumeFX() const { return mVolumeFX.get(); }

    //! The GLSLProgram used to render the volume, usually you don't need to touch this.
    GLSLProgram* volumeGLSLProgram() { return mVolumeGLSLProgram.get(); }
    const GLSLProgram* volumeGLSLProgram() const { return mVolumeGLSLProgram.get(); }

  protected:

    // *** you shouldn't need to care about these ones ***

    virtual void onActorRenderStarted( Actor* actor, real frame_clock, const Camera* cam, Renderable* renderable, const Shader* shader, int pass );

    virtual void onActorDelete( Actor* ) {}

    //! Binds a VividVolume to an Actor.
    void bindActor( Actor* );

    //! Updates the uniforms used by the GLSLProgram to render the volume each time the onActorRenderStarted() method is called.
    void updateUniforms( Actor* actor, real clock, const Camera* camera, Renderable* rend, const Shader* shader );

    //! Returns the Geometry associated to a VividVolume and its bound Actor.
    Geometry* geometry() { return mGeometry.get(); }

    //! Returns the Geometry associated to a VividVolume and its bound Actor.
    const Geometry* geometry() const { return mGeometry.get(); }

    //! Defines the dimensions of the box in model coordinates enclosing the volume and generates the actual geometry of the box to be rendered.
    void setBox( const AABB& box );

    //! The dimensions of the box enclosing the volume.
    const AABB& box() const { return mBox; }

    //! Returns the coordinates assigned to each of the 8 box corners of the volume.
    const fvec3* vertCoords() const { return mVertCoord->begin(); }

    //! Returns the coordinates assigned to each of the 8 box corners of the volume.
    fvec3* vertCoords() { return mVertCoord->begin(); }

    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume.
    const fvec3* texCoords() const { return mTexCoord->begin(); }

    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume.
    fvec3* texCoords() { return mTexCoord->begin(); }

    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    //! \param img_size Size in texels of the volume texture.
    void generateTextureCoordinates( const ivec3& img_size );

  protected:
    ref<Geometry> mGeometry;
    AABB mBox;
    ref<ArrayFloat3> mTexCoord;
    ref<ArrayFloat3> mVertCoord;
    VividRendering* mVividRendering;

    // Settings

    ref<Actor> mVolumeActor;
    ref<Effect> mVolumeFX;
    ref<Transform> mVolumeTransform;
    ref<GLSLProgram> mVolumeGLSLProgram;
    EVolumeMode mVolumeMode;
    int mSamplesPerRay;
    float mVolumeDensity;
    float mIsoValue;
  };
}

#endif
