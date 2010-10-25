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

#ifndef Rendering_INCLUDE_ONCE
#define Rendering_INCLUDE_ONCE

#include <vl/RenderingAbstract.hpp>
#include <vl/RenderQueueSorter.hpp>
#include <vl/Actor.hpp>
#include <vl/RenderQueue.hpp>
#include <vl/Renderer.hpp>
#include <vl/RenderTarget.hpp>
#include <vl/Camera.hpp>
#include <vl/Transform.hpp>
#include <vl/Collection.hpp>
#include <vl/SceneManager.hpp>

namespace vl
{
  /** The Rendering class collects all the information to perform the rendering of a scene.
  The Rendering class performs the following steps:
  -# activates the appropriate OpenGLContext
  -# dispatches the RC_PreRendering RenderingCallback[s]
  -# activates the RenderTarget's framebuffer and draw buffers
  -# recursively computes the world matrix of the installed Transform hierarchy
  -# setups the Camera transform and the Viewport
  -# extracts all the visible Actor[s] from the installed SceneManager[s]
  -# compiles and sorts the RenderQueue using the installed RenderQueueSorter
  -# uses the installed Renderer to perform the rendering of the RenderQueue
  -# dispatches the RC_PostRendering RenderingCallback[s]

  To be included in the rendering an Actor must have an enableMask() and Effect::enableMask() such that
  \p "(Actor::enableMask() & Rendering::enableMask()) != 0" and \p "(Actor::effect()->enableMask() & Rendering::enableMask()) != 0".

  \sa

  - Renderer
  - Actor
  - Effect
  - Transform
  */
  class Rendering: public RenderingAbstract
  {
  public:
    virtual const char* className() { return "Rendering"; }
    /** Constructor. */
    Rendering();
    /** Copy constructor. */
    Rendering(const Rendering& other): RenderingAbstract(other) { *this = other; } 
    /** Assignment operator. */
    Rendering& operator=(const Rendering& other);

    /** Executes the rendering.
    - RC_PreRendering RenderingCallback is issued right before the rendering takes place.
    - RC_PostRendering RenderingCallback is issued right after the rendering is completed.
    */
    virtual void render();

    /** The RenderQueueSorter used to perform the sorting of the objects to be rendered, if NULL no sorting is performed. */
    void setRenderQueueSorter(RenderQueueSorter* render_queue_sorter) { mRenderQueueSorter = render_queue_sorter; }
    /** The RenderQueueSorter used to perform the sorting of the objects to be rendered, if NULL no sorting is performed. */
    RenderQueueSorter* renderQueueSorter() { return mRenderQueueSorter.get(); }

    /** The Renderer object used to perform the actual rendering of the scene. */
    void setRenderer(Renderer* renderer) { mRenderer = renderer; }
    /** The Renderer object used to perform the actual rendering of the scene. */
    const Renderer* renderer() const { return mRenderer.get(); }
    /** The Renderer object used to perform the actual rendering of the scene. */
    Renderer* renderer() { return mRenderer.get(); }

    /** The Camera that defines the point of view and viewport to be used when rendering the scene. */
    void setCamera(Camera* camera) { mCamera = camera; }
    /** The Camera that defines the point of view and viewport to be used when rendering the scene. */
    const Camera* camera() const { return mCamera.get(); }
    /** The Camera that defines the point of view and viewport to be used when rendering the scene. */
    Camera* camera() { return mCamera.get(); }

    /** Returns the list of SceneManager[s] containing the Actor[s] to be rendered. */
    Collection<SceneManager>* sceneManagers() { return mSceneManagers.get(); }
    /** Returns the list of SceneManager[s] containing the Actor[s] to be rendered. */
    const Collection<SceneManager>* sceneManagers() const { return mSceneManagers.get(); }

    /** The RenderTarget on which the rendering is performed. */
    void setRenderTarget(RenderTarget* render_target) { mRenderTarget = render_target; }
    /** The RenderTarget on which the rendering is performed. */
    const RenderTarget* renderTarget() const { return mRenderTarget.get(); }
    /** The RenderTarget on which the rendering is performed. */
    RenderTarget* renderTarget() { return mRenderTarget.get(); }

    /** The root of the Transform tree updated at every rendering frame. */
    void setTransform(Transform* transform) { mTransform = transform; }
    /** The root of the Transform tree updated at every rendering frame. */
    const Transform* transform() const { return mTransform.get(); }
    /** The root of the Transform tree updated at every rendering frame. */
    Transform* transform() { return mTransform.get(); }

    /** Whether the Level-Of-Detail should be evaluated or not. When disabled lod #0 is used. */
    void setEvaluateLOD(bool evaluate_lod) { mEvaluateLOD = evaluate_lod; }
    /** Whether the Level-Of-Detail should be evaluated or not. When disabled lod #0 is used. */
    bool evaluateLOD() const { return mEvaluateLOD; }

    /** Whether Actor::update() should be called or not. 
    \note
    Only visible Actor[s] are updated.
    */
    void setActorAnimationEnabled(bool animate_actors) { mActorAnimationEnabled = animate_actors; }
    /** Whether Actor::update() should be called or not. 
    \note
    Only visible Actor[s] are updated.
    */
    bool actorAnimationEnabled() const { return mActorAnimationEnabled; }

    /** Whether Shader::update() should be called or not. 
    \note
    Only Shader[s] belonging to visible Actor[s] are updated.
    */
    void setShaderAnimationEnabled(bool animate_shaders) { mShaderAnimationEnabled = animate_shaders; }
    /** Whether Shader::update() should be called or not. 
    \note
    Only Shader[s] belonging to visible Actor[s] are updated.
    */
    bool shaderAnimationEnabled() const { return mShaderAnimationEnabled; }

    /** Whether the installed SceneManager[s] should perform Actor culling or not in order to maximize the rendering performances. */
    void setCullingEnabled(bool enabled) { mCullingEnabled = enabled; }
    /** Whether the installed SceneManager[s] should perform Actor culling or not in order to maximize the rendering performances. */
    bool cullingEnabled() const { return mCullingEnabled; }

    void setClearFlags(EClearFlags clear_flags) { mClearFlags = clear_flags; }
    EClearFlags clearFlags() const { return mClearFlags; }

    /** Whether OpenGL resources such as textures and GLSL programs should be automatically initialized when first used. 
      * Enabling this features forces VL to keep track of which resources are used for each rendering, which might slighly impact the 
      * rendering time, thus to obtain the maximum performances disable this option and manually initialize your textures and GLSL shaders. */
    void setAutomaticResourceInit(bool enable) { mAutomaticResourceInit = enable; }
    /** Whether OpenGL resources such as textures and GLSL programs should be automatically initialized before the rendering takes place. */
    bool automaticResourceInit() const { return mAutomaticResourceInit; }

    /** Returns whether near/far planes optimization is enabled. */
    bool nearFarClippingPlanesOptimized() const { return mNearFarClippingPlanesOptimized; }
    /** Enabled/disables near/far planes optimization. When enabled, the automatic near/far clipping planes optimization
      * modifies the projection matrix of the current camera to minimize z-fighting artifacts. If later you disable
      * this feature you might want to recompute the original projection matrix of the camera using the method 
      * vl::Camera::setProjectionAsPerspective(). */
    void setNearFarClippingPlanesOptimized(bool enabled) { mNearFarClippingPlanesOptimized = enabled; }

  protected:
    void fillRenderQueue( ActorCollection* actor_list );
    ActorCollection* actorQueue() { return mActorQueue.get(); }
    RenderQueue* renderQueue() { return mRenderQueue.get(); }

  protected:
    ref<RenderQueueSorter> mRenderQueueSorter;
    ref<ActorCollection> mActorQueue;
    ref<RenderQueue> mRenderQueue;
    ref<Renderer> mRenderer;
    ref<RenderTarget> mRenderTarget;
    ref<Camera> mCamera;
    ref<Transform> mTransform;
    ref<Collection<SceneManager> > mSceneManagers;
    ref<SceneManager> mPippoBaudo;

    EClearFlags mClearFlags;

    bool mAutomaticResourceInit;
    bool mCullingEnabled;
    bool mEvaluateLOD;
    bool mActorAnimationEnabled;
    bool mShaderAnimationEnabled;
    bool mNearFarClippingPlanesOptimized;
  };
}

#endif
