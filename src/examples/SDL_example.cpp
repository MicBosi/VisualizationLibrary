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

#include <vl/VisualizationLibrary.hpp>
#include <vlSDL/SDLWindow.hpp>
#include "Applets/App_RotatingCube.hpp"

int main(int argc, char* args[])
{
  /* init Visualization Library */
  vl::VisualizationLibrary::init();

  /* setup the OpenGL context format */
  vl::OpenGLContextFormat format;
  format.setDoubleBuffer(true);
  format.setRGBABits( 8,8,8,8 );
  format.setDepthBufferBits(24);
  format.setStencilBufferBits(8);
  format.setFullscreen(false);
  //format.setMultisampleSamples(16);
  //format.setMultisample(true);

  /* create the applet to be run */
  vl::ref<vlut::Applet> applet = new App_RotatingCube;
  applet->initialize();
  /* create a native SDL window */
  vl::ref<vlSDL::SDLWindow> sdl_window = new vlSDL::SDLWindow;
  /* bind the applet so it receives all the GUI events related to the OpenGLContext */
  sdl_window->addEventListener(applet.get());
  /* target the window so we can render on it */
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->setRenderTarget( sdl_window->renderTarget() );
  /* black background */
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( vlut::black );
  /* define the camera position and orientation */
  vl::vec3 eye    = vl::vec3(0,10,35); // camera position
  vl::vec3 center = vl::vec3(0,0,0);   // point the camera is looking at
  vl::vec3 up     = vl::vec3(0,1,0);   // up direction
  vl::mat4 view_mat = vl::mat4::lookAt(eye, center, up).getInverse();
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );
  /* Initialize the OpenGL context and window properties */
  int x = 0;
  int y = 0;
  int width = 512;
  int height= 512;
  sdl_window->initSDLWindow( "Visualization Library on SDL - Rotating Cube", format, x, y, width, height );

  /* run SDL message loop */
  vlSDL::messageLoop();

  /* deallocate the window with all the OpenGL resources before shutting down Visualization Library */
  sdl_window = NULL;

  /* shutdown Visualization Library */
  vl::VisualizationLibrary::shutdown();

  return 0;
}
// Have fun!
