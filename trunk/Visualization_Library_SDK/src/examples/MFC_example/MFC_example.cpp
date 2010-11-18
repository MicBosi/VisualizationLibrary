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

#include "StdAfx.h"
#include "vl/VisualizationLibrary.hpp"
#include "vlMFC/MFCWindow.hpp"
#include "Applets/App_RotatingCube.hpp"

using namespace vl;
using namespace vlMFC;

/* MFC_Example: implements the MFC application */
class MFC_Example: public CWinApp
{
public:
  MFC_Example() {}

  virtual BOOL InitInstance();
  virtual int ExitInstance();
  /*virtual int Run();*/
  /*virtual BOOL OnIdle(LONG lCount);*/

protected:
  ref<MFCWindow> mVLCWin;

  DECLARE_MESSAGE_MAP ()
};
BEGIN_MESSAGE_MAP(MFC_Example, CWinApp)
END_MESSAGE_MAP()

/* instance of the MFC application */
MFC_Example mfc_app;

/* called when the application exits */
int MFC_Example::ExitInstance()
{
  CWinApp::ExitInstance();

  /* destroy the window and the OpenGL rendering context */
  mVLCWin = NULL;

  /* shutdown Visualization Library */
  vl::VisualizationLibrary::shutdown();

  return 0;
}

/* called when the application starts */
BOOL MFC_Example::InitInstance()
{
  CWinApp::InitInstance();

  /* open a console so we can see the program's output on stdout */
  vl::showWin32Console();

  /* init Visualization Library */
  vl::VisualizationLibrary::init();

  /* setup the OpenGL context format */
  vl::OpenGLContextFormat format;
  format.setDoubleBuffer(true);
  format.setRGBABits( 8,8,8,0 );
  format.setDepthBufferBits(24);
  format.setStencilBufferBits(8);
  format.setFullscreen(false);
  format.setMultisampleSamples(16);
  format.setMultisample(true);

  /* create the applet to be run */
  vl::ref<vlut::Applet> applet = new App_RotatingCube;
  applet->initialize();
  /* instance the MFC window/OpenGLContext */
  mVLCWin = new MFCWindow;
  /* bind the applet so it receives all the GUI events related to the OpenGLContext */
  mVLCWin->addEventListener(applet.get());
  /* target the window so we can render on it */
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->setRenderTarget( mVLCWin->renderTarget() );
  /* black background */
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( vlut::black );
  /* define the camera position and orientation */
  vl::vec3 eye    = vl::vec3(0,10,35); // camera position
  vl::vec3 center = vl::vec3(0,0,0);   // point the camera is looking at
  vl::vec3 up     = vl::vec3(0,1,0);   // up direction
  vl::mat4 view_mat = vl::mat4::lookAt(eye, center, up).getInverse();
  vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setViewMatrix( view_mat );
  /* Initialize the OpenGL context and window properties */
  int x = 100;
  int y = 100;
  int width = 512;
  int height= 512;
  mVLCWin->initMFCWindow(NULL, NULL, "Visualization Library on MFC - Rotating Cube", format, x, y, width, height);

  /* MFC specific stuff */
  m_pMainWnd = mVLCWin.get();
  m_pMainWnd->ShowWindow(m_nCmdShow);
  m_pMainWnd->UpdateWindow();

  return TRUE;
}
// Have fun!