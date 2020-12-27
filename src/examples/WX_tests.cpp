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
#include "tests.hpp"

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
/* TestBattery implementation to work with wxWindows */
class TestBatteryWX: public TestBattery
{
public:
  TestBatteryWX() {}

  void runGUI(const vl::String& title, BaseDemo* program, vl::OpenGLContextFormat /*format*/, int x, int y, int width, int height, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center)
  {
    program->setAppletName(title);

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
    MyFrame *frame = new MyFrame(NULL, L"WXGLCanvas", wxPoint(x,y), wxSize(width, height));
    mWXWin = new WXGLCanvas( 
      frame,                    // parent
      wxID_ANY,                 // id
      gl_context_format,        // attribList
      wxPoint(x,y),             // pos
      wxSize(width, height),    // size
      wxFULL_REPAINT_ON_RESIZE, // style
      "GLCanvas",               // name
      wxNullPalette             // palette
    );

    /* needed before accessing OpenGL */
    frame->Show();

    /* common test setup */
    setupApplet(program, mWXWin, bk_color, eye, center);

    /* initialize OpenGL context */
    mWXWin->initGLContext();

    /* call initEvent() to initialize the test */
    mWXWin->dispatchInitEvent();

    /* these must be done after the window is visible and the applet is initialized */
    frame->SetPosition( wxPoint(x,y) );
    frame->SetClientSize( wxSize(width,height) );
    frame->SetLabel(wxT("Visualization Library on wxWindows"));
  }

protected:
  WXGLCanvas* mWXWin;
};
//-----------------------------------------------------------------------------
// implement the application
//-----------------------------------------------------------------------------
class MyApp: public wxApp
{
public:
  bool OnInit();
  int OnExit();
protected:
};
//-----------------------------------------------------------------------------
IMPLEMENT_APP(MyApp)
//-----------------------------------------------------------------------------
bool MyApp::OnInit()
{
  int      argc = GetInstance()->argc;
  wxChar** argv = GetInstance()->argv;

  /* parse command line arguments */
  long test = -1;
  wxString test_str;
  if ( argc >= 2 ) 
  {
      test_str = argv[ 1 ];
      if ( ! test_str.ToLong( &test ) ) {
          test = -1;
      }
  }

  TestBatteryWX test_battery;
  test_battery.run(test, test_str.ToStdString(), vl::OpenGLContextFormat()/*not used*/, false);

  return true;
}
//-----------------------------------------------------------------------------
int MyApp::OnExit()
{
  vl::VisualizationLibrary::shutdown();
  return 0;
}
//-----------------------------------------------------------------------------
