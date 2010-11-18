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

#include <vlWX/vlWXGLCanvas.hpp>
#include <vl/VisualizationLibrary.hpp>
#include "Applets/App_RotatingCube.hpp"

using namespace vlWX;
using namespace vl;

//-----------------------------------------------------------------------------
// main window
//-----------------------------------------------------------------------------
class MyFrame: public wxFrame
{
public:
  MyFrame(wxWindow *parent, const wxString& title, const wxPoint& pos, const wxSize& size, long style = wxDEFAULT_FRAME_STYLE):
    wxFrame(parent, wxID_ANY, title, pos, size, style) {}
  ~MyFrame(){}
};
//-----------------------------------------------------------------------------
// implement the application
//-----------------------------------------------------------------------------
class MyApp: public wxApp
{
public:
  bool OnInit();
  int OnExit();
};
//-----------------------------------------------------------------------------
IMPLEMENT_APP(MyApp)
//-----------------------------------------------------------------------------
bool MyApp::OnInit()
{
  vl::showWin32Console();
  vl::VisualizationLibrary::init();

  MyFrame *frame = new MyFrame(NULL, L"vlWXGLCanvas", wxDefaultPosition, wxSize(400, 300));

  /* create the applet to be run */
  vl::ref<vlut::Applet> applet = new App_RotatingCube;
  applet->initialize();
  /* Initialize the OpenGL context and window properties */
  // WX_GL_RGBA:            Use true colour
  // WX_GL_BUFFER_SIZE:     Bits for buffer if not WX_GL_RGBA
  // WX_GL_LEVEL:           0 for main buffer, >0 for overlay, <0 for underlay
  // WX_GL_DOUBLEBUFFER:    Use doublebuffer
  // WX_GL_STEREO:          Use stereoscopic display
  // WX_GL_AUX_BUFFERS:     Number of auxiliary buffers (not all implementation support this option)
  // WX_GL_MIN_RED:         Use red buffer with most bits (> MIN_RED bits)
  // WX_GL_MIN_GREEN:       Use green buffer with most bits (> MIN_GREEN bits)
  // WX_GL_MIN_BLUE:        Use blue buffer with most bits (> MIN_BLUE bits)
  // WX_GL_MIN_ALPHA:       Use alpha buffer with most bits (> MIN_ALPHA bits)
  // WX_GL_DEPTH_SIZE:      Bits for Z-buffer (0,16,32)
  // WX_GL_STENCIL_SIZE:    Bits for stencil buffer
  // WX_GL_MIN_ACCUM_RED:   Use red accum buffer with most bits (> MIN_ACCUM_RED bits)
  // WX_GL_MIN_ACCUM_GREEN: Use green buffer with most bits (> MIN_ACCUM_GREEN bits)
  // WX_GL_MIN_ACCUM_BLUE:  Use blue buffer with most bits (> MIN_ACCUM_BLUE bits)
  // WX_GL_MIN_ACCUM_ALPHA: Use blue buffer with most bits (> MIN_ACCUM_ALPHA bits)
  int context_format[] =
  {
    WX_GL_RGBA,
    WX_GL_DOUBLEBUFFER,
    WX_GL_MIN_RED, 8,
    WX_GL_MIN_GREEN, 8,
    WX_GL_MIN_BLUE, 8,
    WX_GL_MIN_ALPHA, 8,
    WX_GL_DEPTH_SIZE, 24,
    WX_GL_STENCIL_SIZE, 8,
    /*WX_GL_LEVEL, 0,
    WX_GL_AUX_BUFFERS, 0*/
    0
  };
  vl::ref<vlWXGLCanvas> vl_gl_canvas = new vlWXGLCanvas( frame, NULL, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, context_format );
  /* target the window so we can render on it */
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->setRenderTarget( vl_gl_canvas->renderTarget() );
  /* black background */
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( vlut::black );
  /* define the camera position and orientation */
  vl::vec3 eye    = vl::vec3(0,10,35); // camera position
  vl::vec3 center = vl::vec3(0,0,0);   // point the camera is looking at
  vl::vec3 up     = vl::vec3(0,1,0);   // up direction
  vl::mat4 view_mat = vl::mat4::lookAt(eye, center, up).getInverse();
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );
  /* show the window */
  frame->Show();
  /* THE ORDER IS IMPORTANT IMPORTANT */
  vl_gl_canvas->initGLContext();
  /* bind the applet so it receives all the GUI events related to the OpenGLContext */
  vl_gl_canvas->addEventListener(applet.get());
  vl_gl_canvas->dispatchInitEvent();
  /* these must be done after the window is visible */
  int x = 0;
  int y = 0;
  int width = 512;
  int height= 512;
  frame->SetPosition( wxPoint(x,y) );
  frame->SetClientSize( wxSize(width,height) );
  frame->SetLabel(wxT("Visualization Library on wxWindows - Rotating Cube"));

  return true;
}
//-----------------------------------------------------------------------------
int MyApp::OnExit()
{
  vl::VisualizationLibrary::shutdown();
  return 0;
}
//-----------------------------------------------------------------------------
// Have fun!
