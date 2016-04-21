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

#ifndef RendererVivid_INCLUDE_ONCE
#define RendererVivid_INCLUDE_ONCE

#include <vlGraphics/Renderer.hpp>
#include <vlGraphics/ProjViewTransfCallback.hpp>
#include <vlGraphics/Shader.hpp>
#include <map>

namespace vl
{
  //-----------------------------------------------------------------------------
  // RendererVivid
  //-----------------------------------------------------------------------------
  /** The RendererVivid class is experimental.
    * \sa Rendering */
  class VLGRAPHICS_EXPORT RendererVivid: public Renderer
  {
    VL_INSTRUMENT_CLASS(vl::RendererVivid, Renderer)

  public:
    typedef enum {
      NoDepthPeeling,
      DualDepthPeeling,
      FrontToBackDepthPeeling
    } ERenderingMode;

  public:
    RendererVivid();
    
    virtual ~RendererVivid() {}
    
    /** Takes as input the render queue to render and returns a possibly filtered render queue for further processing. 
      * RendererVivid's implementation of this function always returns \p in_render_queue. */
    virtual const RenderQueue* render(const RenderQueue* in_render_queue, Camera* camera, real frame_clock);
    
    void setRenderingMode(ERenderingMode mode) { mRenderingMode = mode; }
    ERenderingMode renderingMode() const { return mRenderingMode; }

  protected:

    void renderQueue(const RenderQueue* in_render_queue, Camera* camera, real frame_clock, bool depth_peeling_on=true);

    void lazyInitialize();
    void deleteDualPeelingRenderTargets();
    void initDualPeelingRenderTargets();
    void deleteFrontPeelingRenderTargets();
    void initFrontPeelingRenderTargets();
    void buildShaders();
    void makeFullScreenQuad();
    void bindTexture(vl::GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit);
    void renderFrontToBackPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock);
    void renderDualPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock);

  protected:
    ivec2 mImageSize;
    ERenderingMode mRenderingMode;

    vl::ref<vl::GLSLProgram> mShaderDualInit;
    vl::ref<vl::GLSLProgram> mShaderDualPeel;
    vl::ref<vl::GLSLProgram> mShaderDualBlend;
    vl::ref<vl::GLSLProgram> mShaderDualFinal;
    vl::ref<vl::GLSLProgram> mShaderFrontInit;
    vl::ref<vl::GLSLProgram> mShaderFrontPeel;
    vl::ref<vl::GLSLProgram> mShaderFrontBlend;
    vl::ref<vl::GLSLProgram> mShaderFrontFinal;
    vl::ref<vl::GLSLProgram> mShaderAverageInit;
    vl::ref<vl::GLSLProgram> mShaderAverageFinal;
    vl::ref<vl::GLSLProgram> mShaderWeightedSumInit;
    vl::ref<vl::GLSLProgram> mShaderWeightedSumFinal;

    GLuint mDualBackBlenderFboId;
    GLuint mDualPeelingSingleFboId;
    GLuint mDualDepthTexId[2];
    GLuint mDualFrontBlenderTexId[2];
    GLuint mDualBackTempTexId[2];
    GLuint mDualBackBlenderTexId;

    GLuint mFrontFboId[2];
    GLuint mFrontDepthTexId[2];
    GLuint mFrontColorTexId[2];
    GLuint mFrontColorBlenderTexId;
    GLuint mFrontColorBlenderFboId;

    int mNumPasses;
    bool mUseOQ;
    GLuint mQueryID;

    // MIC FIXME: remove
    GLuint g_quadDisplayList;
  };
  //------------------------------------------------------------------------------
}

#endif
