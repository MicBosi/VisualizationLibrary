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

#ifndef Applet_INCLUDE_ONCE
#define Applet_INCLUDE_ONCE

#include <vlGraphics/UIEventListener.hpp>
#include <vlGraphics/TrackballManipulator.hpp>
#include <vlGraphics/GhostCameraManipulator.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Rendering.hpp>

namespace vl
{
//-----------------------------------------------------------------------------
// Applet
//-----------------------------------------------------------------------------
  /**
   * The Applet class is an utilitly UIEventListener that features a ghost manipulator, 
   * trackball manipulator, an FPS counter and a simple rendering pipeline. */
  class Applet: public UIEventListener
  {
  public:
    virtual const char* className() { return "Applet"; }
    Applet();
    ~Applet() {}

    void initialize();

    virtual void openglContextBoundEvent(OpenGLContext* openglContext);

    void keyReleaseEvent(unsigned short, EKey key);

    virtual void destroyEvent();

    void runEvent();

    void resizeEvent(int w, int h);

    SceneManagerActorTree* sceneManager() { return mSceneManagerActorTree.get(); }
    const SceneManagerActorTree* sceneManager() const { return mSceneManagerActorTree.get(); }

    GhostCameraManipulator* ghostCamera() { return mFly.get(); }
    TrackballManipulator* trackball() { return mTrackball.get(); }

    double fps() const { return mFPS; }

    virtual void run()       = 0;
    virtual void initEvent() = 0;
    virtual void shutdown()  = 0;

  protected:
    void bindManipulators(Rendering* rendering);

  protected:
    ref<Rendering> mRendering;
    ref<GhostCameraManipulator> mFly;
    ref<TrackballManipulator> mTrackball;
    ref<SceneManagerActorTree> mSceneManagerActorTree;
    int mFrameCount;
    double mStartTime;
    double mFPS;
  };
}

#endif
