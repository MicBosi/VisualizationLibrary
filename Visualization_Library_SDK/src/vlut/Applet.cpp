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

#include <vlut/Applet.hpp>
#include <vlGraphics/Rendering.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlGraphics/SceneManager.hpp>
#include <vlGraphics/RenderQueue.hpp>
#include <vlCore/Time.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Log.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
Applet::Applet()
{
  #ifndef NDEBUG
    mObjectName = className();
  #endif
  mFrameCount = 0;
  mStartTime  = Time::currentTime();
  mFPS        = 0;
}
//-----------------------------------------------------------------------------
void Applet::initialize()
{
  // by default Visualization Library installs a Rendering
  ref<Rendering> rend = VisualizationLibrary::rendering()->as<Rendering>();
  rend->setShaderAnimationEnabled(true);
  // attached later: viewport
  // attached later: opengl context
  // attached later: render target

  // installs a SceneManagerActorTree as the default scene manager
  mSceneManagerActorTree = new SceneManagerActorTree;
  rend->sceneManagers()->push_back(sceneManager());

  mFly       = new GhostCameraManipulator;
  mTrackball = new TrackballManipulator;
  mFly->setEnabled(false);
  mTrackball->setEnabled(true);

  bindManipulators( rend.get() );
}
//-----------------------------------------------------------------------------
void Applet::bindManipulators(Rendering* rendering)
{
  mFly->setCamera( rendering->camera() );
  // mFly->prepareToReconnect();

  mTrackball->setCamera( rendering->camera() );
  mTrackball->setTransform( rendering->transform() );
  mTrackball->setPivot( vec3(0,0,0) );
}
//-----------------------------------------------------------------------------
void Applet::openglContextBoundEvent(OpenGLContext* openglContext)
{
  VL_CHECK(openglContext)
  VL_CHECK(mFly->openglContext() == NULL);
  // mFly->setCamera( pipeline()->camera() );
  openglContext->addEventListener( mFly.get() );
  // mFly->prepareToReconnect();

  VL_CHECK(mTrackball->openglContext() == NULL);
  // mTrackball->setCamera( pipeline()->camera() );
  // mTrackball->setTransform( pipeline()->rend->transform() );
  mTrackball->setPivot( vec3(0,0,0) );
  openglContext->addEventListener( mTrackball.get() );
}
//-----------------------------------------------------------------------------
void Applet::keyReleaseEvent(unsigned short, EKey key)
{
  if (key == Key_Escape)
  {
    openglContext()->destroyAllFBORenderTargets();
    openglContext()->quitApplication();
  }
  else
  if (key == Key_U)
    openglContext()->update();
  else
  if(key == Key_T)
  {
    mFly->setEnabled(false);
    mTrackball->setEnabled(true);
  }
  else
  if(key == Key_F)
  {
    mTrackball->setEnabled(false);
    mFly->setEnabled(true);
  }
  else
  if(key == Key_C)
    openglContext()->setContinuousUpdate( !openglContext()->continuousUpdate() );
  else
  if(key == Key_F1)
    openglContext()->setFullscreen( !openglContext()->fullscreen() );
}
//-----------------------------------------------------------------------------
void Applet::destroyEvent()
{
  shutdown();
  mFly->setCamera(NULL);
  mTrackball->setCamera(NULL);
  mTrackball->setTransform(NULL);
}
//-----------------------------------------------------------------------------
void Applet::runEvent()
{
  // FPS counter
  if (Time::currentTime() - mStartTime > 1.000)
  {
    double secs = (Time::currentTime() - mStartTime);
    mFPS = mFrameCount / secs;
    mFrameCount = 0;
    mStartTime = Time::currentTime();
  }
  mFrameCount++;

  // execute program
  run();

  // set frame time for all the rendering
  Real now_time = Time::currentTime();
  VisualizationLibrary::rendering()->setFrameClock( now_time );

  // execute rendering
  VisualizationLibrary::rendering()->render();

  // show rendering
  if( openglContext()->hasDoubleBuffer() )
    openglContext()->swapBuffers();

  VL_CHECK_OGL();
}
//-----------------------------------------------------------------------------
void Applet::resizeEvent(int w, int h)
{
  // if a simple Rendering is attached as the rendering root than update viewport and projection matrix.
  Rendering* rendering = VisualizationLibrary::rendering()->as<Rendering>();
  if (rendering)
  {
    VL_CHECK( w == rendering->renderer()->renderTarget()->width() );
    VL_CHECK( h == rendering->renderer()->renderTarget()->height() );
    rendering->camera()->viewport()->setWidth( w );
    rendering->camera()->viewport()->setHeight( h );
    rendering->camera()->setProjectionAsPerspective();
  }
}
//-----------------------------------------------------------------------------
