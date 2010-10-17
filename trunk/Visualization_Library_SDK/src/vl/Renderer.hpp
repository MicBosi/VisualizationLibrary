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

  /**
   * The Renderer class executes the actual rendering on the given RenderQueue.
   *
   * \sa Rendering
  */
  class Renderer: public Object
  {
  public:
    virtual const char* className() { return "Renderer"; }
    Renderer();
    virtual ~Renderer() {}
    virtual void render(const RenderQueue* render_queue, Camera* camera);

    void setCollectStatistics(bool on) { mCollectStatistics = on; }
    bool collectStatistics() const { return mCollectStatistics; }

    size_t renderedRenderablesCount() const { return mRenderedRenderableCount; }
    size_t renderedTrianglesCount() const { return mRenderedTriangleCount; }
    size_t renderedLinesCount() const { return mRenderedLineCount; }
    size_t renderedPointsCount() const { return mRenderedPointCount; }

    OpenGLContext* openglContext() { return mOpenGLContext; }
    const OpenGLContext* openglContext() const { return mOpenGLContext; }
    void setOpenGLContext(OpenGLContext* ogl_context) { mOpenGLContext = ogl_context; }

    /** Whether occlusion culling is enabled or not.
    See also \ref pagGuideOcclusionCulling "OpenGL-Accelerated Occlusion Culling Tutorial".
    */
    void setOcclusionCullingEnabled(bool enabled) { mOcclusionCullingEnabled = enabled; }
    /** Whether occlusion culling is enabled or not.
    See also \ref pagGuideOcclusionCulling "OpenGL-Accelerated Occlusion Culling Tutorial".
    */
    bool occlusionCullingEnabled() const { return mOcclusionCullingEnabled; }

    /** The number of pixels visible for an actor to be considered occluded (default = 0) */
    void setOcclusionThreshold(int threshold) { mOcclusionThreshold = threshold; }
    /** The number of pixels visible for an actor to be considered occluded (default = 0) */
    int occlusionThreshold() const { return mOcclusionThreshold; }

    void setProjViewTransfCallback(ProjViewTranfCallbackStandard* callback) { mProjViewTranfCallback = callback; }
    const ProjViewTranfCallbackStandard* projViewTransfCallback() const { return mProjViewTranfCallback.get(); }
    ProjViewTranfCallbackStandard* projViewTransfCallback() { return mProjViewTranfCallback.get(); }

  protected:
    bool mCollectStatistics;
    size_t mRenderedRenderableCount;
    size_t mRenderedTriangleCount;
    size_t mRenderedLineCount;
    size_t mRenderedPointCount;

    OpenGLContext* mOpenGLContext;

    // used to reset the OpenGL states & enables at the end of the rendering.
    vl::ref<EnableSet> mDummyEnables;
    vl::ref<RenderStateSet> mDummyStateSet;

    bool mOcclusionCullingEnabled;
    // mic fixme
    // shader used to render proxy shapes during occlusion cull tests
    // ref<Shader> mOcclusionShader;
    int mOcclusionThreshold;
    unsigned mOcclusionQueryTick;
    unsigned mOcclusionQueryTickPrev;

    ref<ProjViewTranfCallbackStandard> mProjViewTranfCallback;
  };
  //------------------------------------------------------------------------------
}

#endif
