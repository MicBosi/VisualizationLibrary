/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi                                             */
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

#include <vlCore\VisualizationLibrary.hpp>
#include <vlCore\Log.hpp>
#include <vlCore\Time.hpp>
#include <vlEGL\EGLWindow.hpp>
#include "Applets/App_RotatingCube.hpp"

using namespace vl;

int main ( int argc, char *argv[] )
{ 
  struct SContract
  {
    /* Init VL on startup */
    SContract() { vl::VisualizationLibrary::init(); }
    
    /* Shutdown VL on exit */
    ~SContract() { vl::VisualizationLibrary::shutdown(); }
  
  } contract;
  
  ref<vlEGL::EGLWindow> egl_window = new vlEGL::EGLWindow;

  vl::OpenGLContextFormat format;
  format.setDoubleBuffer(true);
  format.setRGBABits( 5,6,5,0 );
  format.setDepthBufferBits(16);
  format.setStencilBufferBits(0);
  format.setMultisampleSamples(0);
  format.setMultisample(false);
  format.setVSync(false);
  /* NOTE THIS */
  format.setContextClientVersion(1);

  if ( !egl_window->initEGLWindow(NULL, "OpenGL ES 1.x Example", format, 0, 0, 640, 480) )
  {
    Log::error("EGLWindow::initEGLWindow() failed!\n");
    return 1;
  }

  /* create the applet to be run */
  ref<Applet> applet = new App_RotatingCube;
  applet->initialize();

  egl_window->addEventListener( applet.get() );

  /* target the window so we can render on it */
  applet->rendering()->as<Rendering>()->renderer()->setRenderTarget( egl_window->renderTarget() );
  
  /* black background */
  applet->rendering()->as<Rendering>()->camera()->viewport()->setClearColor( black );
  
  /* define the camera position and orientation */
  vec3 eye    = vec3(0,10,35); // camera position
  vec3 center = vec3(0,0,0);   // point the camera is looking at
  vec3 up     = vec3(0,1,0);   // up direction
  mat4 view_mat = mat4::getLookAt(eye, center, up).getInverse();
  applet->rendering()->as<Rendering>()->camera()->setViewMatrix( view_mat );

  /* shows the window and dispatches the first resize event */
  egl_window->show();

  return vlEGL::messageLoop();
}
