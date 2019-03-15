/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

#ifndef VividRendering_INCLUDE_ONCE
#define VividRendering_INCLUDE_ONCE

#include <vlVivid/link_config.hpp>
#include <vlVivid/VividRenderer.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/CalibratedCamera.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>

namespace vl
{
  namespace Vivid {
    typedef enum {
      //! Standard Vivid depth peeling rendering.
      DepthPeeling,
      //! Used for debugging.
      FastRender,
      //! Renders the stencil texture - used for debugging.
      StencilRender
    } ERenderingMode;

    typedef enum {
      //! Use this Actor enable mask to use the Vivid depth peeling rendering path.
      //! Actors rendered with vl::Vivid::VividEnableMask require a vl::Effect created using vl::VividRendering::makeVividEffect().
      VividEnableMask = 1,
      //! Use this Actor enable mask to use the standard VL Renderer rendering path. Used for volumes, text, etc.
      StandardEnableMask = 2
    } EEnableMask;

    typedef enum {
      //! Texture unit reserved for Vivid usage.
      TextureUnit0 = 0,
      //! Texture unit reserved for Vivid usage.
      TextureUnit1 = 1,
      //! Texture unit reserved for Vivid usage.
      TextureUnit2 = 2,
      //! Texture unit reserved for Vivid usage.
      TextureUnit3 = 3,
      //! Texture unit usable by the user. We only support one texture at the moment.
      UserTexture  = 4
    } ETextureUnits;

    typedef enum {
      //! Direct volume rendering.
      Direct,
      //! Isosurface volume rendering.
      Isosurface,
      //! MIP volume rendering.
      MIP
    } EVolumeMode;

    typedef enum {
      //! 3D point representation used by niftk::VLMapperPoints
      Point3D,
      //! 2D point representation used by niftk::VLMapperPoints
      Point2D
    } EPointMode;

    typedef enum {
      //! Color "smart" target for Vivid fog and clipping settings.
      Color,
      //! Alpha "smart" target for Vivid fog and clipping settings.
      Alpha,
      //! Saturation "smart" target for Vivid fog and clipping settings.
      Saturation
    } ESmartTarget;

    typedef enum {
      //! Disables fog
      FogOff,
      //! Use linear fog equation
      FogLinear,
      //! Use exp fog equation
      FogExp,
      //! Use exp2 fog equation
      FogExp2
    } EFogMode;

    typedef enum {
      //! Disables a clipping unit
      ClipOff,
      //! Enables sphere clipping
      ClipSphere,
      //! Enables box clipping
      ClipBox,
      //! Enables plane clipping
      ClipPlane
    } EClipMode;

    typedef enum {
      //! Renders a 3D surface (default)
      //! \sa niftk::VLMapper::setRenderingMode()
      Polys,
      //! Renders the 3D outline of a 3D surface (default)
      //! \sa niftk::VLMapper::setRenderingMode()
      Outline3D,
      //! Renders the polygons and 3D outline of a 3D surface (default)
      //! \sa niftk::VLMapper::setRenderingMode()
      PolysAndOutline3D,
      //! Renders the 3D clipping outline of a 3D surface (default)
      //! \sa niftk::VLMapper::setRenderingMode()
      Slice,
      //! Renders the 2D outline of a 3D surface (default)
      Outline2D,
      //! Renders the polygons and 2D outline of a 3D surface (default)
      //! \sa niftk::VLMapper::setRenderingMode()
      PolysAndOutline2D,
    } ESurfaceMode;
  }

  /**
   * vl::VividRendering implements a "rendering engine" with a pipeline and features specifically designed around NifTK needs.
   *
   * vl::VividRendering differs from vl::Rendering in that vl::Rendering is designed as a general purpose rendering engine
   * supporting all standard OpenGL features & render states like Material, ColorMaterial, Light, 3D textures etc.,
   * while vl::VividRendering supports a much smaller subset of standard OpenGL render states but supports out of the box
   * some key features specific to NifTK like animated background, depth peeling, 2D & 3D outlines, smart clipping,
   * smart fogging, 3D stencil, camera calibration etc.
   *
   * Most of these new features are driven either by options here in the vl::VividRendering class or as Uniform variables
   * in the Actor's Effect. Such Uniform variables are initialized by the vl::VividRendering::makeVividEffect().
   * The ultimate reference to which Uniform variable does what is the /vivid/glsl/uniforms.glsl file included in most GLSL
   * shaders. Secondly vl::VividRendering::makeVividEffect()'s source code will show their default values and some extra description.
   *
   * The bulk of the rendering engine is implemented by the vl::ViviRenderer around a depth peeling rendering core.
   *
   * Actors added to the vl::VividRendering::sceneManager() can be rendered by either vl::ViviRenderer when using
   * vl::Actors::setEnableMask( vl::Vivid::VividEnableMask ) or by the standard vl::Renderer using
   * vl::Actors::setEnableMask( vl::Vivid::StandadrdEnableMask ). This is useful when rendering objects using render states
   * not supported by the VividRenderer. The rendering pipeline always draws vl::Vivid::VividEnableMask objects first and
   * then vl::Vivid::StandadrdEnableMask ones after.
   *
   * Actors rendered with vl::Vivid::VividEnableMask requires a vl::Effect created using vl::VividRendering::makeVividEffect().
   */
  class VLVIVID_EXPORT VividRendering: public Rendering
  {
    VL_INSTRUMENT_CLASS(vl::VividRendering, Rendering)

  public:
    VividRendering();

    //! Renders the scene on the given framebuffer
    void render( Framebuffer* framebuffer ) { vividRenderer()->setFramebuffer(framebuffer); Rendering::render(); }

    //! Returns the default vl::CalibratedCamera
    CalibratedCamera* calibratedCamera() { return mCalibratedCamera.get(); }
    const CalibratedCamera* calibratedCamera() const { return mCalibratedCamera.get(); }

    //! Returns the default vl::SceneManagerActorTree
    SceneManagerActorTree* sceneManager() { return mSceneManagerActorTree.get(); }
    const SceneManagerActorTree* sceneManager() const { return mSceneManagerActorTree.get(); }

    // --- Settings ---

    //! Sets the global rendering mode, used for debuggig purposes.
    void setRenderingMode(Vivid::ERenderingMode mode) { mRenderingMode = mode; }
    Vivid::ERenderingMode renderingMode() const { return mRenderingMode; }

    //! Sets the number of depth peeling passes.
    //! The more passes the more transparency layers will be visible.
    //! Each transparency layer requires an additional rendering of the scene so the higher the number the slower the rendering.
    //! \sa setDepthPeelingAutoThrottleEnabled()
    void setDepthPeelingPasses( int n ) { mVividRenderer->setDetphPeelingPasses( n ); }
    int depthPeelingPasses() const { return mVividRenderer->depthPeelingPasses(); }

    //! Allows depth peeling to be disabled for a faster rendering using a simple test.
    //! This works in the simplest but usual cases where:
    //! - Lighting is enabled and material diffuse alpha is == 1
    //! - Outline color alpha is == 1
    //! - All vertex colors alphas are == 1
    //! - Texture has either completely opaque or completely transparent texels (point sprites, alpha masking etc.)
    //! - Point sprites may look a bit rough
    //! Does not detect:
    //! - Some but not all vertex color alphas are < 1
    //! - Texture has translucent texels, ie 0 < alpha < 1 (as opposed to either 0 or 1)
    void setDepthPeelingAutoThrottleEnabled( bool enabled ) { m_DepthPeelingAutoThrottleEnabled = enabled; }
    bool isDepthPeelingAutoThrottleEnabled() const { return m_DepthPeelingAutoThrottleEnabled; }

    //!! The opacity of the overall 3D rendering on top of the background. The background is always fully opaque.
    void setOpacity( float a ) { mOpacity = a; }
    float opacity() const { return mOpacity; }

    // --- Outline Settings ---

    //! Advanced. Offset outline by given amount in camera space to make it more visible. Only for `Outline3D` modes.
    void setOutline3DEyeOffset( float offset ) { mOutline3DEyeOffset = offset; }
    float outline3DEyeOffset() const { return mOutline3DEyeOffset; }

    // Advanced. Offset outline by given amount in NDC space to make it more visible. Only for `Outline3D` modes.
    void setOutline3DClipOffset( float offset ) { mOutline3DClipOffset = offset; }
    float outline3DClipOffset() const { return mOutline3DClipOffset; }

    // --- Stencil Settings ---

    //! Enable/disable Stencil Rendering mode.
    void setStencilEnabled( bool enabled ) { mStencilEnabled = enabled; }
    bool isStencilEnabled() const { return mStencilEnabled; }

    //! Background color used when stencil rendering mode is enabled.
    //! Stencil background color cannot be transparent at the moment.
    void setStencilBackgroundColor( const vec4& color) { mStencilBackground = color; }
    const vec4& stencilBackgroundColor() const { return mStencilBackground; }

    //! The Actors used to render into the Stencil Texture.
    std::vector< ref<Actor> >& stencilActors() { return mStencilActors; }
    const std::vector< ref<Actor> >& stencilActors() const { return mStencilActors; }

    //! Stencil Texture smoothness, ie. the size of the blur kernel, default is 10. Larger values take longer to compute.
    //! See also this issue: https://cmiclab.cs.ucl.ac.uk/CMIC/NifTK/issues/4717
    void setStencilSmoothness( int smoothness ) { mStencilSmoothness = smoothness; }
    int stencilSmoothness() const { return mStencilSmoothness; }

    // --- Background Settings ---

    //! The background color
    void setBackgroundColor( const vec4& color ) { camera()->viewport()->setClearColor( color ); }
    const vec4& backgroundColor() const { return camera()->viewport()->clearColor(); }

    //! Enable/disable the background image
    void setBackgroundImageEnabled( bool enabled ){ mBackgroundImageEnabled = enabled; }
    bool backgroundImageEnabled() const { return mBackgroundImageEnabled; }

    //! Internal. The background TextureSampler that contains the vl::Texture used to render the background
    TextureSampler* backgroundTexSampler() { return mVividRenderer->backgroundTexSampler(); }
    const TextureSampler* backgroundTexSampler() const { return mVividRenderer->backgroundTexSampler(); }

    // ---  Vivid Effect ---

    //! Returns an Effect that is compatible with vl::VividRenderer.
    //! All Actors with vl::Vivid::VividEnableMask are required to have an Effect created by this function.
    //! \param effect If != NULL it will initialize the input effect else a new one will be returned.
    static ref<Effect> makeVividEffect( Effect* effect = NULL );

    // --- Internal ---

    //! Internal. Used by vl::VividVolume.
    Texture* depthFBOTex() { return mVividRenderer->depthFBOTex(); }
    //! Internal. Used by vl::VividVolume.
    const Texture* depthFBOTex() const { return mVividRenderer->depthFBOTex(); }

    VividRenderer* vividRenderer() { return mVividRenderer.get(); }
    const VividRenderer* vividRenderer() const { return mVividRenderer.get(); }

  private:
    ref< VividRenderer > mVividRenderer;
    ref< CalibratedCamera > mCalibratedCamera;
    ref< SceneManagerActorTree > mSceneManagerActorTree;

    // Settings

    Vivid::ERenderingMode mRenderingMode;

    float mOpacity;

    bool mStencilEnabled;
    std::vector< ref<Actor> > mStencilActors;
    int mStencilSmoothness;
    vec4 mStencilBackground;

    float mOutline3DClipOffset;
    float mOutline3DEyeOffset;

    bool mBackgroundImageEnabled;

    bool m_DepthPeelingAutoThrottleEnabled;
  };
}

#endif
