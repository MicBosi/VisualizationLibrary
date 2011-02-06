#include "stdafx.h"
#include "MDI_App.h"

#include "MDI_AppDoc.h"
#include "MDI_AppView.h"
#include "App_RotatingCube.hpp"
#include <vlGraphics/RenderingTree.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CMDI_AppView, CView)
//-----------------------------------------------------------------------------
// VL: it is important to enable these messages
BEGIN_MESSAGE_MAP(CMDI_AppView, CView)
  ON_WM_CHAR()
  ON_WM_CLOSE()
  ON_WM_CREATE()
  ON_WM_KEYDOWN()
  ON_WM_KEYUP()
  ON_WM_LBUTTONDBLCLK()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MBUTTONDBLCLK()
  ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEWHEEL()
  ON_WM_PAINT()
  ON_WM_RBUTTONDBLCLK()
  ON_WM_RBUTTONDOWN()
  ON_WM_RBUTTONUP()
  ON_WM_SIZE()
  ON_WM_TIMER()
  ON_WM_DROPFILES()
END_MESSAGE_MAP()
//-----------------------------------------------------------------------------
CMDI_AppView::CMDI_AppView()
{
  // VL: add this view to the view list so that it can be updated
  theApp.AddView(this);
}
//-----------------------------------------------------------------------------
CMDI_AppView::~CMDI_AppView()
{
  // VL: add this view to the view list so that it can be updated
  theApp.RemoveView(this);
}
//-----------------------------------------------------------------------------
int CMDI_AppView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  __super::OnCreate(lpCreateStruct);

  /* setup the OpenGL context format */
  vl::OpenGLContextFormat format;
  format.setDoubleBuffer(true);
  format.setRGBABits( 8,8,8,0 );
  format.setDepthBufferBits(24);
  format.setStencilBufferBits(8);
  format.setFullscreen(false);
  format.setMultisampleSamples(16);
  format.setMultisample(true);

  /* create a new vl::Rendering for this window */
  vl::ref<vl::Rendering> rend = new vl::Rendering;
  rend->renderer()->setRenderTarget( this->OpenGLContext::renderTarget() );
  vl::defRendering()->as<vl::RenderingTree>()->subRenderings()->push_back(rend.get());
  
  /* black background */
  rend->camera()->viewport()->setClearColor( vl::black );
  
  /* define the camera position and orientation */
  vl::vec3 eye    = vl::vec3(0,10,35); // camera position
  vl::vec3 center = vl::vec3(0,0,0);   // point the camera is looking at
  vl::vec3 up     = vl::vec3(0,1,0);   // up direction
  vl::mat4 view_mat = vl::mat4::getLookAt(eye, center, up).getInverse();
  rend->camera()->setViewMatrix( view_mat );

  /* create the applet to be run */
  vl::ref<App_RotatingCube> applet = new App_RotatingCube(rend.get());
  applet->initialize();
  
  /* bind the applet so it receives all the GUI events related to the OpenGLContext */
  this->OpenGLContext::addEventListener(applet.get());

  /* Initialize the OpenGL context and window properties */
  initOpenGLContext(NULL, "Visualization Library MFC MDI- Rotating Cube", format);

  return 0;
}
//-----------------------------------------------------------------------------

// the rest is MFC stuff...

//-----------------------------------------------------------------------------
void CMDI_AppView::OnDraw(CDC* /*pDC*/)
{
	CMDI_AppDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
}
//-----------------------------------------------------------------------------
CMDI_AppDoc* CMDI_AppView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMDI_AppDoc)));
	return (CMDI_AppDoc*)m_pDocument;
}
//-----------------------------------------------------------------------------
