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

#ifndef Renderer_INCLUDE_ONCE
#define Renderer_INCLUDE_ONCE

#include <vlGraphics/RendererAbstract.hpp>
#include <vlGraphics/ProjViewTransfCallback.hpp>
#include <vlGraphics/Shader.hpp>
#include <map>

namespace vl
{
  //-----------------------------------------------------------------------------
  // Renderer
  //-----------------------------------------------------------------------------
  /** The Renderer class executes the actual rendering on the given RenderQueue.
    * \sa Rendering */
  class VLGRAPHICS_EXPORT Renderer: public RendererAbstract
  {
    INSTRUMENT_CLASS(vl::Renderer, RendererAbstract)

  public:
    Renderer();
    
    virtual ~Renderer() {}
    
    /** Takes as input the render queue to render and returns a possibly filtered render queue for further processing. 
      * Renderer's implementation of this function always returns \p in_render_queue. */
    virtual const RenderQueue* render(const RenderQueue* in_render_queue, Camera* camera, Real frame_clock);

    void setProjViewTransfCallback(ProjViewTransfCallback* callback) { mProjViewTransfCallback = callback; }
    
    const ProjViewTransfCallback* projViewTransfCallback() const { return mProjViewTransfCallback.get(); }
    
    ProjViewTransfCallback* projViewTransfCallback() { return mProjViewTransfCallback.get(); }

    /** A bitmask/Shader map used to everride the Shader of those Actors whose enable mask satisfy the following condition: (Actors::enableMask() & bitmask) != 0. */
    const std::map<unsigned int, ref<Shader> >& shaderOverrideMask() const { return mShaderOverrideMask; }

    /** A bitmask/Shader map used to everride the Shader of those Actors whose enable mask satisfy the following condition: (Actors::enableMask() & bitmask) != 0. */
    std::map<unsigned int, ref<Shader> >& shaderOverrideMask() { return mShaderOverrideMask; }

    bool isEnabled(unsigned int mask) { return (mask & mEnableMask) != 0; }

    /** The RenderTarget on which the rendering is performed. */
    void setRenderTarget(RenderTarget* render_target) { mRenderTarget = render_target; }

    /** The RenderTarget on which the rendering is performed. */
    const RenderTarget* renderTarget() const { return mRenderTarget.get(); }
    
    /** The RenderTarget on which the rendering is performed. */
    RenderTarget* renderTarget() { return mRenderTarget.get(); }

  protected:
    ref<RenderTarget> mRenderTarget;

    // used to reset the OpenGL states & enables at the end of the rendering.
    vl::ref<EnableSet> mDummyEnables;
    vl::ref<RenderStateSet> mDummyStateSet;

    std::map<unsigned int, ref<Shader> > mShaderOverrideMask;

    ref<ProjViewTransfCallback> mProjViewTransfCallback;
  };
  //------------------------------------------------------------------------------
}

#endif
