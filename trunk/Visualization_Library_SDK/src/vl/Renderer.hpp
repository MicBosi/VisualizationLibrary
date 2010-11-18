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

#include <vl/Camera.hpp>
#include <vl/Renderable.hpp>
#include <vl/Shader.hpp>
#include <vl/Effect.hpp>
#include <vl/OpenGLContext.hpp>
#include <vl/RenderEventCallback.hpp>
#include <vector>
#include <map>
#include <set>

namespace vl
{
  class RenderQueue;
  class Renderer;

  /** Vitual class used as a callback to update the state of the \p projection, \p view and \p transform matrices of a GLSLProgram or fixed function pipeline. */
  class ProjViewTranfCallback: public Object
  {
  public:
    /** This function is called whenever a new GLSLProgram (or the NULL one, i.e. the fixed function pipeline) is being activated for the first time in the current rendering.
     * This callback is most useful to initialize the GLSLProgram with the current projection and view matrices, besides the current Actor's transform.
     * \param caller The Renderer object calling this function.
     * \param glsl The GLSLProgram being activated. If NULL the fixed function pipeline is being activated.
     * \param transform The transform of the current Actor being rendered.
     * \param camera The Camera used for the rendering from which you can retrieve the projection and view matrices.
     * \param first_overall If \p true it means that the rendering has just started. Useful if you want to initialized your callback object.
     */
    virtual void programFirstUse(const Renderer* caller, const GLSLProgram* glsl, const Transform* transform, const Camera* camera, bool first_overall) = 0;
    /** This function is called whenever the Transform changes with respect to the current GLSLProgram (including the NULL one, i.e. the fixed function pipeline).
     * This callback is most useful to update the GLSLProgram with the current Actor's transform matrix.
     * \param caller The Renderer object calling this function.
     * \param glsl The GLSLProgram being activated. If NULL the fixed function pipeline is being activated.
     * \param transform The transform of the current Actor being rendered.
     * \param camera The Camera used for the rendering from which you can retrieve the projection and view matrices.
     */
    virtual void programTransfChange(const Renderer* caller, const GLSLProgram* glsl, const Transform* transform, const Camera* camera) = 0;
  };

  //! Updates the GL_MODELVIEW and GL_PROJECTION matrices of the fixed function pipeline in an optimized manner.
  //! You usually want to install this callback if the fixed fuction pipeline is available, even when using GLSL shaders.
  //! In fact the GL_MODELVIEW and GL_PROJECTION matrices are visible from all the GLSL shaders, thus requiring fewer matrix updates
  //! compared to being forced to send projection, view and transform matrix to every single GLSLProgram at least once during the rendering!
  class ProjViewTranfCallbackStandard: public ProjViewTranfCallback
  {
  public:
    ProjViewTranfCallbackStandard(): mLastTransform(NULL) {}
    virtual void programFirstUse(const Renderer*, const GLSLProgram* glsl, const Transform*, const Camera*, bool first_overall );
    virtual void programTransfChange(const Renderer*, const GLSLProgram* glsl, const Transform*, const Camera* );
  private:
    const Transform* mLastTransform;
  };

  class RendererAbstract: public Object
  {
  public:
    RendererAbstract()
    {
      mRenderEventCallbacks = new Collection<RenderEventCallback>;
      mClearFlags = CF_CLEAR_COLOR_DEPTH;
      mEnableMask = 0xFFFFFFFF;
      mRenderTick = 0;
    }

    RendererAbstract& operator=(const RendererAbstract& other)
    {
      *mRenderEventCallbacks = *other.mRenderEventCallbacks;
      mClearFlags = other.mClearFlags;
      mEnableMask = other.mEnableMask;
      /* mRenderTick = other.mRenderTick; */ // render-tick remains local
    }

    /** Takes as input the render queue to render and returns a possibly filtered render queue for further processing. 
      * Renderer's implementation of this function always returns \p in_render_queue. */
    virtual const RenderQueue* render(const RenderQueue* in_render_queue, Camera* camera) = 0;
    virtual const RenderTarget* renderTarget() const = 0;
    virtual RenderTarget* renderTarget() = 0;

    void dispatchOnRendererStarted()
    {
      const Collection<RenderEventCallback>& cb = *mRenderEventCallbacks;
      for(int i=0; i<cb.size(); ++i)
      {
        if ( cb[i]->isEnabled() && cb[i]->onRendererStarted(this) && cb[i]->removeAfterCall() )
        {
          renderEventCallbacks()->eraseAt( i );
          --i;
        }
      }
    }

    void dispatchOnRendererFinished()
    {
      const Collection<RenderEventCallback>& cb = *mRenderEventCallbacks;
      for(int i=0; i<cb.size(); ++i)
      {
        if ( cb[i]->isEnabled() && cb[i]->onRendererFinished(this) && cb[i]->removeAfterCall() )
        {
          renderEventCallbacks()->eraseAt( i );
          --i;
        }
      }
    }

    //! Returns the list of RenderEventCallback objects bound to a Rendering
    Collection<RenderEventCallback>* renderEventCallbacks() { return mRenderEventCallbacks.get(); }

    //! Returns the list of RenderEventCallback objects bound to a Rendering
    const Collection<RenderEventCallback>* renderEventCallbacks() const { return mRenderEventCallbacks.get(); }

    /** The current render tick number, equivalent to the number or calls made to the render() method. */
    unsigned long renderTick() const { return mRenderTick; }

    /** Increments the rendering tick count. */
    void incrementRenderTick() { ++mRenderTick; }

    void setClearFlags(EClearFlags clear_flags) { mClearFlags = clear_flags; }

    EClearFlags clearFlags() const { return mClearFlags; }

    void setEnableMask(unsigned int mask) { mEnableMask = mask; }

    unsigned int enableMask() const { return mEnableMask; }

  protected:
    ref< Collection<RenderEventCallback> > mRenderEventCallbacks;
    unsigned long mRenderTick;
    unsigned int mEnableMask;
    EClearFlags mClearFlags;
  };

  /** The Renderer class executes the actual rendering on the given RenderQueue.
    * \sa Rendering */
  class Renderer: public RendererAbstract
  {
  public:
    virtual const char* className() { return "Renderer"; }
    
    Renderer();
    
    virtual ~Renderer() {}
    
    /** Takes as input the render queue to render and returns a possibly filtered render queue for further processing. 
      * Renderer's implementation of this function always returns \p in_render_queue. */
    virtual const RenderQueue* render(const RenderQueue* in_render_queue, Camera* camera);

    void setProjViewTransfCallback(ProjViewTranfCallbackStandard* callback) { mProjViewTranfCallback = callback; }
    
    const ProjViewTranfCallbackStandard* projViewTransfCallback() const { return mProjViewTranfCallback.get(); }
    
    ProjViewTranfCallbackStandard* projViewTransfCallback() { return mProjViewTranfCallback.get(); }

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

    ref<ProjViewTranfCallbackStandard> mProjViewTranfCallback;
  };
  //------------------------------------------------------------------------------
}

#endif
