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

#ifndef Camera_INCLUDE_ONCE
#define Camera_INCLUDE_ONCE

#include <vl/checks.hpp>
#include <vl/math3D.hpp>
#include <vl/Vector4.hpp>
#include <vl/RenderTarget.hpp>
#include <vl/Viewport.hpp>
#include <vl/Frustum.hpp>
#include <vl/Ray.hpp>

namespace vl
{
  class AABB;
  //-----------------------------------------------------------------------------
  // Camera
  //-----------------------------------------------------------------------------
  /**
   * Represents a virtual camera defining, among other things, the point of view
   * from which scenes can be rendered.
   *
  */
  class Camera: public Object
  {
  public:
    virtual const char* className() { return "Camera"; }

    //! Constructor.
    Camera();

    /**
     * Initializes the GL_PROJECTION and GL_MODELVIEW matrices according to the 
     * Camera's projection and view matrix and computes the frustum planes.
    */
    void activate();

    /**
     * Computes the Camera's frustum planes in world space.
     * If nearFarClippingPlanesOptimized() == true the near and far culling planes distances 
     * are respectively set to nearPlane() and farPlane()
     */
    void computeFrustumPlanes();

    /** 
     * Loads the GL_MODELVIEW matrix with the Camera's view matrix multiplied by the specified model matrix.
    */
    void applyModelViewMatrix(const mat4& model_matrix) const;

    /** 
     * Loads the GL_MODELVIEW matrix with the Camera's view matrix.
    */
    void applyViewMatrix() const;

    void setActive(bool active) { mActive = active; }
    bool active() const { return mActive; }

    void setFOV(Real fov) { mFOV = fov; }
    Real fov() const { return mFOV; }

    /**
     * Returns the aspect ratio computed as viewport()->width()/viewport()->height().
     * If viewport() == NULL the function returns 0.
    */
    Real aspectRatio() const 
    {
      if (viewport())
        return (Real)viewport()->width()/viewport()->height();
      else
        return 0;
    }

    void setNearPlane(Real nearplane) { mNearPlane = nearplane; }
    Real nearPlane() const { return mNearPlane; }

    void setFarPlane(Real farplane) { mFarPlane = farplane; }
    Real farPlane() const { return mFarPlane; }

    void setFrustum(const Frustum& frustum) { mFrustum = frustum; }
    const Frustum& frustum() const { return mFrustum; }

    void setViewport(Viewport* viewport) { mViewport = viewport; }
    Viewport* viewport() { return mViewport.get(); }
    const Viewport* viewport() const { return mViewport.get(); }

    void followTransform(Transform* transform) { mFollowTransform = transform; }
    const Transform* followedTransform() const { return mFollowTransform.get(); }
    Transform* followedTransform() { return mFollowTransform.get(); }

    /**
     * Sets the Camera's view matrix.
    */
    void setViewMatrix(const mat4& mat) { mViewMatrix = mat; mInverseViewMatrix = mat; mInverseViewMatrix.invert(); }

    /**
     * The Camera's view matrix. This is what you would pass to OpenGL with "glMatrixMode(GL_MODELVIEW);glLoadMatrix(camera.viewMatrix().ptr());"
    */
    const mat4& viewMatrix() const { return mViewMatrix; }

    /** 
     * The camera's reference frame matrix, the inverse of the view matrix.
    */
    void setInverseViewMatrix(const mat4& mat) { mInverseViewMatrix = mat; mViewMatrix = mat; mViewMatrix.invert(); }

    /** 
     * The camera's reference frame matrix, the inverse of the view matrix.
    */
    const mat4& inverseViewMatrix() const { return mInverseViewMatrix; }

    /** 
     * The camera's projection matrix.
    */
    void setProjectionMatrix(const mat4& mat) { mProjectionMatrix = mat; }

    /** 
     * The camera's projection matrix.
    */
    const mat4& projectionMatrix() const { return mProjectionMatrix; }

    /**
     * Builds a perspective projection matrix for the Camera based on the Camera's and Viewport's settings. See also http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml for more information. 
    */
    void setProjectionAsPerspective();

    /**
     * Builds a perspective projection matrix for the Camera based on the Camera's and Viewport's settings. See also http://www.opengl.org/sdk/docs/man/xhtml/gluPerspective.xml for more information. 
    */
    void setProjectionAsPerspective(Real fov, Real near, Real far);

    /**
     * Builds a perspective projection matrix for the Camera based on the give frustum. See also http://www.opengl.org/sdk/docs/man/xhtml/glFrustum.xml for more information.
     *
     * \note
     * This method sets fov() to -1.
    */
    void setProjectionAsFrustum(Real left, Real right, Real bottom, Real top, Real near, Real far);

    /**
     * Builds an orthographic projection matrix for the Camera based on the Camera's near/far planes and its Viewport's settings. See also http://www.opengl.org/sdk/docs/man/xhtml/glOrtho.xml for more information.
     * Equivalent to:
     * \code
     * setProjectionMatrix( 
     *   mat4::ortho(
     *     offset, (Real)mViewport->width()  + offset,
     *     offset, (Real)mViewport->height() + offset,
     *     nearPlane(), farPlane())
     * );
     * \endcode
    */
    void setProjectionAsOrtho(Real offset=-0.5f);

    /**
     * Builds an orthographic projection matrix for the Camera based on its Viewport's settings. See also http://www.opengl.org/sdk/docs/man/xhtml/glOrtho2D.xml for more information.
     *
     * Equivalent to:
     * \code
     * setProjectionMatrix( 
     *   mat4::ortho(
     *     offset, viewport()->width()  + offset,
     *     offset, viewport()->height() + offset,
     *     -1, +1)
     * );
     * \endcode
    */
    void setProjectionAsOrtho2D(Real offset=-0.5f);

    void setViewMatrixAsLookAt( const vec3& eye, const vec3& center, const vec3& up);

    void getViewMatrixAsLookAt( vec3& eye, vec3& look, vec3& up, vec3& right) const;

    bool project(const vec4& in, vec4& out) const;

    /** Unprojects a vector from window coordinates to world coordinates.
        \param win The window coordinates point to be projected. Note that, in accordance to OpenGL conventions, the window 
                   coordinates have the Y axis pointing upwards and origin at the bottom left corner of the window.
        \param out The world coordinates of the projected point.
        \note The \p w coordinate of \p win determines the position of the projected point along the ray passing through \p win. If \p w
              equals 0 then \p out will lay on the near clipping plane, if \w equals 1 then \p out will lay on the far clipping plane.
     */
    bool unproject(const vec3& win, vec4& out) const;

    //! Unprojects a set of points. See unproject(const vec3& win, vec4& out) for more information.
    bool unproject(std::vector<vec3>& points) const;

    /** Computes the ray passing through the point <winx,winy>.
     * \param winx The x position of the point defining the ray
     * \param winy The y position of the point defining the ray
     * \note <winx,winy> is considered to be point in widows coordinate. In accordance to the OpenGL conventions the windows coordinates
     * have origin on the bottom left corner of the window, with the Y axis pointing upwards. Most GUI libraries define the 
     * the orgin at the top left corner of the window and Y axis pointing downwards. You can convert such coordinates to OpenGL ones
     * using the following simple formula: \p "opengl_y = window_height - coord_y".
     */
    Ray computeRay(int winx, int winy);

    Frustum computeRayFrustum(int winx, int winy);

    /**
     * Enables the optimization of the near and far clipping planes.
     * If the optimization is enabled the near and far clipping planes are adjusted to fit the
     * sphere returned by sceneBoundingSphere(). Optimizing the near and far clipping planes 
     * results in an optimized usage of the z-buffer with the consequence of minimizing possible 
     * z-fighting artifacts and enhancing the rendering quality.
     * \sa setSceneBoundingSphere()
     * \note Optimizing the near and far clipping planes can slightly slow down the rendering performances.
     * \note At the moment the near and far clipping planes optimization is available only when using a 
     * perspective projection matrix and not for orthographic projection matrices.
    */
    void setNearFarClippingPlanesOptimized(bool enable) { mNearFarClippingPlanesOptimized = enable; }

    /**
     * Set to true if the optimization of the near and far clipping planes is enabled.
     * If the optimization is enabled the near and far clipping planes are adjusted to fit the
     * sphere returned by sceneBoundingSphere(). Optimizing the near and far clipping planes 
     * results in an optimized usage of the z-buffer with the consequence of minimizing possible 
     * z-fighting artifacts and enhancing the rendering quality.
     * \sa setSceneBoundingSphere()
     * \note Optimizing the near and far clipping planes might slightly slow down the rendering performances if the scene contains several thousands of objects.
     * \note At the moment the near and far clipping planes optimization is available only when using a 
     * perspective projection matrix set up by setProjectionAsPerspective().
    */
    bool nearFarClippingPlanesOptimized() const { return mNearFarClippingPlanesOptimized; }

    /**
     * The bounding sphere of the scene used to optimize the near and far clipping planes.
     * \sa setNearFarClippingPlanesOptimized()
    */
    void setSceneBoundingSphere(const Sphere& sphere) { mSceneBoundingSphere = sphere; }
    /**
     * The bounding sphere of the scene used to optimize the near and far clipping planes.
     * \sa setNearFarClippingPlanesOptimized()
    */
    const Sphere& sceneBoundingSphere() const { return mSceneBoundingSphere; }

    /**
     * Adjusts the camera position so that the given aabb can be properly viewed.
     * \param aabb The AABB (in world coords) that should be visible from the newly computed camera position.
     * \param dir The direction (in world coords) along which the camera should be displaced to view the given AABB.
     * \param up The vector that defines the \p up direction (in world coords). Used to properly compute the new camera matrix.
     * \param bias A bias factor used to adjust the computed camera distance from the given AABB. Values between 0 and 1 make the camera closer to the AABB center, values greater than 1 position the camera further away.
    */
    void adjustView(const AABB& aabb, const vec3& dir, const vec3& up, Real bias=1.0f);

  protected:
    mat4 mViewMatrix;
    mat4 mInverseViewMatrix;
    mat4 mProjectionMatrix;
    ref<Viewport> mViewport;
    Frustum mFrustum;
    ref<Transform> mFollowTransform;
    Real mFOV;
    Real mNearPlane;
    Real mFarPlane;
    Sphere mSceneBoundingSphere;
    bool mActive;
    bool mNearFarClippingPlanesOptimized;
  };
}

#endif
