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

#ifndef TrackballManipulator_INCLUDE_ONCE
#define TrackballManipulator_INCLUDE_ONCE

#include <vl/OpenGLContext.hpp>
#include <vl/Camera.hpp>
#include <vl/Vector3.hpp>
#include <vl/Actor.hpp>

namespace vl
{
  class Camera;
  class Transform;
  class Rendering;
  class SceneManager;

  //------------------------------------------------------------------------------
  //! This class lets you rotate a Camera or a Transform node using a vitual trackball.
  //! If you set a Transform node to manipulate, using the function setTransform(), this object
  //! will manipulate the given Transform. If no Transform is specified or a NULL one is
  //! passed to the function setTransform() then this object will manipulate the current camera.
  //! \note In any case, before using a TrackballManipulator you have to specify a Camera object using the function setCamera().
  //! \note The Transform is expected to contain only rotation and translation information. 
  //! Other transformations like shearing, scaling, projection, and so on can produce unspecified results.
  class TrackballManipulator: public UIEventListener
  {
  public:
    typedef enum { NoMode, RotationMode, TranslationMode, ZoomMode } ETrackballMode;
  public:
    virtual const char* className() { return "TrackballManipulator"; }
    TrackballManipulator(): 
      mMode(NoMode),
      mRotationButton(LeftButton), mTranslationButton(MiddleButton), mZoomButton(RightButton), 
      mRotationSpeed(1.0f),        mTranslationSpeed(1.0f),          mZoomSpeed(1.0f)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    virtual void mouseDownEvent(EMouseButton, int x, int y);

    virtual void mouseUpEvent(EMouseButton, int x, int y);

    virtual void mouseMoveEvent(int x, int y);

    mat4 trackballRotation(int x, int y);

    vec3 computeVector(int x, int y);

    void setCamera(Camera* camera) { mCamera = camera; }

    Camera* camera() { return mCamera.get(); }

    void prepareToReconnect()
    { 
      mMode = NoMode;
      if ( openglContext() )
      {
        openglContext()->setMouseVisible(true);
        openglContext()->setContinuousUpdate(false);
      }
    }

    //! Sets the center point the camera will rotate around
    void setPivot(vec3 pivot) { mPivot = pivot; }

    vec3 pivot() const { return mPivot; }

    void setTransform(Transform* tr) { mTransform = tr; }

    Transform* transform() { return mTransform.get(); }

    int rotationButton() const { return mRotationButton; }
    void setRotationButton(int mouse_button) { mRotationButton = mouse_button; }

    int zoomButton() const { return mZoomButton; }
    void setZoomButton(int mouse_button) { mZoomButton = mouse_button; }

    int translationButton() const { return mTranslationButton; }
    void setTranslationButton(int mouse_button) { mTranslationButton = mouse_button; }

    float rotationSpeed() const { return mRotationSpeed; }
    void setRotationSpeed(float speed) { mRotationSpeed = speed; }

    float zoomSpeed() const { return mZoomSpeed; }
    void setZoomSpeed(float speed) { mZoomSpeed = speed; }

    float translationSpeed() const { return mTranslationSpeed; }
    void setTranslationSpeed(float speed) { mTranslationSpeed = speed; }

    //! Returns the current trackball manipulator state.
    ETrackballMode mode() const { return mMode; }

    //! Adjusts the camera position in order to nicely see the scene. It also position the rotation pivot to the center of the AABB. See also Camera::adjustView().
    void adjustView(const AABB& aabb, const vec3& dir, const vec3& up, Real bias=1.0f);
    //! Adjusts the camera position in order to nicely see the scene. It also position the rotation pivot to the center of the AABB containing the Actor[s]. See also Camera::adjustView().
    void adjustView(ActorCollection& actors, const vec3& dir, const vec3& up, Real bias=1.0f);
    //! Adjusts the camera position in order to nicely see the scene. It also position the rotation pivot to the center of the AABB containing the given scene manager. See also Camera::adjustView().
    void adjustView(SceneManager* scene, const vec3& dir, const vec3& up, Real bias=1.0f);
    //! Adjusts the camera position in order to nicely see the scene. It also position the rotation pivot to the center of the AABB containing all the scene managers part of the given rendering. See also Camera::adjustView().
    void adjustView(Rendering* rendering, const vec3& dir, const vec3& up, Real bias=1.0f);

  protected:
    ref<Camera> mCamera;
    ivec2 mMouseStart;
    mat4 mStartMatrix;
    vec3 mPivot;
    vec3 mStartCameraPos;
    vec3 mStartPivot;
    ref<Transform> mTransform;
    ETrackballMode mMode;
    int mRotationButton;
    int mTranslationButton;
    int mZoomButton;
    float mRotationSpeed;
    float mTranslationSpeed;
    float mZoomSpeed;
  };
}

#endif
