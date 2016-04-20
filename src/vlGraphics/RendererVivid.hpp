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
    RendererVivid();
    
    virtual ~RendererVivid() {}
    
    /** Takes as input the render queue to render and returns a possibly filtered render queue for further processing. 
      * RendererVivid's implementation of this function always returns \p in_render_queue. */
    virtual const RenderQueue* render(const RenderQueue* in_render_queue, Camera* camera, real frame_clock);
    
    void renderQueue(const RenderQueue* in_render_queue, Camera* camera, real frame_clock, bool depth_peeling_on=true);

    void DeleteDualPeelingRenderTargets();
    void InitDualPeelingRenderTargets();
    void DeleteFrontPeelingRenderTargets();
    void InitFrontPeelingRenderTargets();
    void BuildShaders();
    void MakeFullScreenQuad();
    void bindTexture(vl::GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit);
    void RenderFrontToBackPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock);
    void RenderDualPeeling(const RenderQueue* render_queue, Camera* camera, real frame_clock);
    void lazyInitialize();
    ivec2 mInitSize;
  };
  //------------------------------------------------------------------------------
}

#endif
