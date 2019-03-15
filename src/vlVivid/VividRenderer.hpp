/*=============================================================================

  vlVivid is part of NifTK: A software platform for medical image computing.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See NifTK license for details.

=============================================================================*/

#ifndef VividRenderer_INCLUDE_ONCE
#define VividRenderer_INCLUDE_ONCE

#include <vlVivid/link_config.hpp>
#include <vlGraphics/Renderer.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlVolume/SlicedVolume.hpp>

namespace vl
{
  class VividRendering;

  /**
   * Implements a depth peeling rendering to be used by vl::VividRendering.
   */
  class VLVIVID_EXPORT VividRenderer: public Renderer
  {
    VL_INSTRUMENT_CLASS(vl::VividRenderer, Renderer)

  public:
    // --- Renderer Implementation ---
    VividRenderer(VividRendering* vivid_rendering);
    virtual ~VividRenderer();

    //! Maximum number of passes to do if UseQueryObject is disabled (default is 4).
    void setDetphPeelingPasses(int n) { mDepthPeelingPasses = n; VL_CHECK(mDepthPeelingPasses >= 1 && mDepthPeelingPasses <= 100); }
    int depthPeelingPasses() const { return mDepthPeelingPasses; }

    //! When enabled will use as many passes as needed to create a 100% correct rendering (disabled by default).
    void setUseQueryObject(bool use) { mUseQueryObject = use; }
    bool useQueryObject() const { return mUseQueryObject;  }

    //! Internal. Returns the depth texture used to access the depth buffer from the volume shader.
    Texture* depthFBOTex() { return mDepthFBOTex.get(); }
    const Texture* depthFBOTex() const { return mDepthFBOTex.get(); }

    //! Internal. The background TextureSampler that contains the vl::Texture used to render the background
    TextureSampler* backgroundTexSampler() { return mBackgroundTexSampler.get(); }
    const TextureSampler* backgroundTexSampler() const { return mBackgroundTexSampler.get(); }

    // For debugging

    void initShaders();
    bool shadersReady() const;
    int passCounter() const { return mPassCounter; }

  protected:
    const RenderQueue* render(const RenderQueue* in_render_queue, Camera* camera, real frame_clock);
    int renderQueue(const RenderQueue* in_render_queue, vl::GLSLProgram* cur_glsl, Camera* camera, real frame_clock, bool depth_peeling_on=true, bool outline_on = false);
    void renderBackgroundImage( Camera* camera );
    void render2DOutlines( const RenderQueue* in_render_queue, Camera* camera, real frame_clock );
    // Used to render volumes, text and all Actors that we want to render using the standard VL rendering
    void renderStandard( const RenderQueue* in_render_queue, Camera* camera, real frame_clock );

    void lazyInitialize();
    void deleteGLBuffers();
    void initGLBuffers();
    void drawFullScreenQuad();
    void bindTexture(vl::GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit);
    void renderFrontToBackPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock);
    void renderControl(const RenderQueue* render_queue, Camera* camera, real frame_clock);
    void renderStencil(Camera* camera);

  protected:
    ivec2 mImageSize;

    vl::ref<vl::GLSLProgram> mShaderOutline2D;
    // vl::ref<vl::GLSLProgram> mShaderOutline3D;

    vl::ref<vl::GLSLProgram> mShaderFrontInit_S;
    // vl::ref<vl::GLSLProgram> mShaderFrontPeel_S;
    vl::ref<vl::GLSLProgram> mShaderFrontInit;
    vl::ref<vl::GLSLProgram> mShaderFrontPeel;
    vl::ref<vl::GLSLProgram> mShaderFrontBlend;
    vl::ref<vl::GLSLProgram> mShaderFrontFinal;

    GLuint mFrontFboId[2];
    GLuint mFrontDepthTexId[2];
    GLuint mFrontColorTexId[2];
    GLuint mFrontColorBlenderTexId;
    GLuint mFrontColorBlenderFboId;

    GLuint mStencilFBO;
    ref<Texture> mStencilTexture;

    GLuint mDepthFBO;
    ref<Texture> mDepthFBOTex;

    ref<TextureSampler> mBackgroundTexSampler;

    int mPassCounter;
    int mDepthPeelingPasses;
    bool mUseQueryObject;
    GLuint mQueryID;

    ref<Geometry> mFullScreenQuad;
    ref<Geometry> mBackgroundQuad;

    VividRendering* mVividRendering;
  };
  //------------------------------------------------------------------------------
}

#endif
