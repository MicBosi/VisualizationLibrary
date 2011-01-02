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

#ifndef GhostCameraManipulator_INCLUDE_ONCE
#define GhostCameraManipulator_INCLUDE_ONCE

#include <vlGraphics/OpenGLContext.hpp>

namespace vl
{
  class Camera;

//------------------------------------------------------------------------------
// GhostCameraManipulator
//------------------------------------------------------------------------------
  /**
   * The GhostCameraManipulator class is an UIEventListener that controls the position and orientation of a Camera.
   * Using the GhostCameraManipulator class the user can freely fly around in the scene just like a ghost would do.
  */
  class GhostCameraManipulator: public UIEventListener
  {
  public:
    GhostCameraManipulator();
    virtual const char* className() { return "GhostCameraManipulator"; }

    virtual void mouseMoveEvent(int x, int y);

    virtual void runEvent();

    void setCamera(Camera* camera);

    Camera* camera();
    const Camera* camera() const;

    void setKeysForward(EKey key, EKey modifier = Key_None)  { mKeysForward[0] = key; mKeysForward[1] = modifier; };
    void setKeysBackward(EKey key, EKey modifier = Key_None) { mKeysBackward[0] = key; mKeysBackward[1] = modifier; };
    void setKeysLeft(EKey key, EKey modifier = Key_None)     { mKeysLeft[0] = key; mKeysLeft[1] = modifier; };
    void setKeysRight(EKey key, EKey modifier = Key_None)    { mKeysRight[0] = key; mKeysRight[1] = modifier; };
    void setKeysUp(EKey key, EKey modifier = Key_None)       { mKeysUp[0] = key; mKeysUp[1] = modifier; };
    void setKeysDown(EKey key, EKey modifier = Key_None)     { mKeysDown[0] = key; mKeysDown[1] = modifier; };

    void prepareToReconnect();

    void setPosition(vec3 position) { mPosition = position; }

    const vec3& position() const { return mPosition; }

    void setXDegrees(Real degree) { mXDegrees = degree; }

    Real xDegrees() { return mXDegrees; }

    void setYDegrees(Real degree) { mYDegrees = degree; }

    Real yDegrees() { return mYDegrees; }

    Real rotationSpeed() const { return mRotationSpeed; }
    Real movementSpeed() const { return mMovementSpeed; }
    void setRotationSpeed(Real speed) { mRotationSpeed = speed; }
    void setMovementSpeed(Real speed) { mMovementSpeed = speed; }

  protected:
    ref<Camera> mCamera;
    vec3 mPosition;
    Real mLastTime;
    Real mRotationSpeed;
    Real mMovementSpeed;
    Real mXDegrees;
    Real mYDegrees;
    // int mLastX;
    // int mLastY;
    EKey mKeysForward[2];
    EKey mKeysBackward[2];
    EKey mKeysUp[2];
    EKey mKeysDown[2];
    EKey mKeysLeft[2];
    EKey mKeysRight[2];
  };
}

#endif
