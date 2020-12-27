/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
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

#include <vlWX/WXGLCanvas.hpp>
#include <vlCore/VisualizationLibrary.hpp>
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
  /* open a console so we can see the applet's output on stdout */
  showWin32Console();

  /* init Visualization Library */
  VisualizationLibrary::init();

  MyFrame *frame = new MyFrame(NULL, L"WXGLCanvas", wxDefaultPosition, wxSize(400, 300));

  // Initialize the OpenGL context and window properties
  // See: https://docs.wxwidgets.org/3.1/glcanvas_8h.html#wxGL_FLAGS
  int gl_context_format[] =
  {
    WX_GL_RGBA,
    WX_GL_DOUBLEBUFFER,
    WX_GL_STENCIL_SIZE, 8,
    WX_GL_LEVEL, 0,
    //wx_GL_COMPAT_PROFILE,
    //WX_GL_SAMPLE_BUFFERS, 1,
    //WX_GL_SAMPLES, 4,
    //WX_GL_MIN_RED, 8,
    //WX_GL_MIN_GREEN, 8,
    //WX_GL_MIN_BLUE, 8,
    //WX_GL_MIN_ALPHA, 8,
    //WX_GL_DEPTH_SIZE, 24,
    0
  };
  ref<WXGLCanvas> vl_gl_canvas = new WXGLCanvas( 
    frame,                    // parent
    wxID_ANY,                 // id
    gl_context_format,        // attribList
    wxDefaultPosition,        // pos
    wxDefaultSize,            // size
    wxFULL_REPAINT_ON_RESIZE, // style
    "GLCanvas",               // name
    wxNullPalette             // palette
  );

  /* create the applet to be run */
  ref<Applet> applet = new App_RotatingCube;

  applet->initialize();

  /* target the window so we can render on it */
  applet->rendering()->as<Rendering>()->renderer()->setFramebuffer( vl_gl_canvas->framebuffer() );

  /* black background */
  applet->rendering()->as<Rendering>()->camera()->viewport()->setClearColor( black );

  /* define the camera position and orientation */
  vec3 eye    = vec3(0,10,35); // camera position
  vec3 center = vec3(0,0,0);   // point the camera is looking at
  vec3 up     = vec3(0,1,0);   // up direction
  mat4 view_mat = mat4::getLookAt(eye, center, up);
  applet->rendering()->as<Rendering>()->camera()->setViewMatrix( view_mat );

  /* show the window */
  frame->Show();

  /* THE ORDER IS IMPORTANT IMPORTANT */
  vl_gl_canvas->initGLContext();

  /* bind the applet so it receives all the GUI events related to the OpenGLContext */
  vl_gl_canvas->addEventListener(applet.get());

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
  VisualizationLibrary::shutdown();
  return 0;
}
//-----------------------------------------------------------------------------
// Have fun!
