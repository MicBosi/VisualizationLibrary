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

#include <vlWin32/Win32Context.hpp>
#include <vlCore/Log.hpp>

using namespace vl;
using namespace vlWin32;

//-----------------------------------------------------------------------------
Win32Context::~Win32Context()
{
  dispatchDestroyEvent();
  destroyAllFBORenderTargets();
}
//-----------------------------------------------------------------------------
void Win32Context::shareOpenGLResources(HGLRC hGLRC)
{
  if (hwnd() && mHDC && mHGLRC)
    wglShareLists(hglrc(), hGLRC);
}
//-----------------------------------------------------------------------------
void Win32Context::makeCurrent()
{
  if (mHDC && mHGLRC)
    wglMakeCurrent(mHDC, mHGLRC);
}
//-----------------------------------------------------------------------------
void Win32Context::update()
{
  if (hwnd())
    PostMessage(hwnd(), WM_PAINT, 0, 0);
}
//-----------------------------------------------------------------------------
void Win32Context::quitApplication()
{
  PostQuitMessage(0);
  eraseAllEventListeners();
}
//-----------------------------------------------------------------------------
void Win32Context::setMouseVisible(bool visible)
{
  mMouseVisible = visible;
  if (visible)
    while(ShowCursor(TRUE ) <  0) {}
  else
    while(ShowCursor(FALSE) >= 0) {}
}
//-----------------------------------------------------------------------------
void Win32Context::setPosition(int x, int y)
{
  if (hwnd())
	  SetWindowPos(hwnd(), 0, x, y, 0, 0, SWP_NOSIZE );
}
//-----------------------------------------------------------------------------
void Win32Context::setSize(int w, int h)
{
  if (hwnd())
  {
    RECT windowRect = { 0, 0, w, h };
    AdjustWindowRectEx(&windowRect, (DWORD)GetWindowLongPtr(hwnd(), GWL_STYLE), 0, (DWORD)GetWindowLongPtr(hwnd(), GWL_EXSTYLE) );
    // computes the actual window based on the client dimensions
    int cx = windowRect.right  - windowRect.left;
    int cy = windowRect.bottom - windowRect.top;
    SetWindowPos(hwnd(), 0, 0, 0, cx, cy, SWP_NOMOVE );
  }
}
//-----------------------------------------------------------------------------
void Win32Context::setWindowSize(int w, int h)
{
  // this are set by WM_SIZE event handler
  // mRenderTarget->setWidth(w);
  // mRenderTarget->setHeight(h);
	SetWindowPos(hwnd(), 0, 0, 0, w, h, SWP_NOMOVE);
}
//-----------------------------------------------------------------------------
vl::ivec2 Win32Context::position() const
{
  RECT r = {0,0,0,0};
  if (hwnd())
	  GetWindowRect(hwnd(), &r);
  return vl::ivec2(r.left,r.top);
}
//-----------------------------------------------------------------------------
vl::ivec2 Win32Context::windowSize() const
{
  RECT r = {0,0,0,0};
  if (hwnd())
	  GetWindowRect(hwnd(), &r);
  return vl::ivec2(r.right - r.left, r.bottom - r.top);
}
//-----------------------------------------------------------------------------
vl::ivec2 Win32Context::size() const
{
  RECT r = {0,0,0,0};
  if (hwnd())
	  GetClientRect(hwnd(), &r);
  return vl::ivec2(r.right - r.left, r.bottom - r.top);
//  return vl::ivec2(width(), height());
}
//-----------------------------------------------------------------------------
void Win32Context::setWindowTitle(const String& title)
{
  if (hwnd())
    SetWindowText(hwnd(), (wchar_t*)title.ptr());
}
//-----------------------------------------------------------------------------
void Win32Context::show()
{
  if (hwnd())
    ShowWindow(hwnd(), SW_SHOW);
}
//-----------------------------------------------------------------------------
void Win32Context::hide()
{
  if (hwnd())
    ShowWindow(hwnd(), SW_HIDE);
}
//-----------------------------------------------------------------------------
void Win32Context::getFocus()
{
  if (hwnd())
    SetFocus(hwnd());
}
//-----------------------------------------------------------------------------
void Win32Context::setMousePosition(int x, int y)
{
  if (hwnd())
  {
    POINT pt = {x, y};
    ClientToScreen( hwnd(), &pt );
    SetCursorPos(pt.x, pt.y);
  }
}
//-----------------------------------------------------------------------------
void Win32Context::swapBuffers()
{
  if(hwnd() && hdc())
    SwapBuffers(hdc());
}
//-----------------------------------------------------------------------------
bool Win32Context::setFullscreen(bool fullscreen_on)
{
  if (!hwnd())
    return false;

  if (fullscreen_on == fullscreen())
    return true;

  if (!fullscreen_on)
  {
    SetWindowLongPtr(hwnd(), GWL_STYLE, mNormFlags/*swl_style*/);

    if (!((mNormFlags & WS_MAXIMIZE) || (mNormFlags & WS_MINIMIZE)))
    {
      setPosition(mNormPosit.x(),mNormPosit.y());
      setSize(mNormSize.x(), mNormSize.y());
    }

    SetWindowPos(hwnd(), 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOSIZE | SWP_NOMOVE);

    // restores display settings
    ChangeDisplaySettings(NULL, 0);
  }
  else
  {
    DEVMODE devmode;
    EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&devmode);

    // devmode.dmPelsWidth  = ... leave current width
    // devmode.dmPelsHeight = ... leave current height
    // change color depth
    devmode.dmBitsPerPel = openglContextInfo().bitsPerPixel();					  
	  devmode.dmFields		 |= DM_BITSPERPEL;

    mNormFlags = (unsigned int)GetWindowLongPtr(hwnd(), GWL_STYLE);
    mNormPosit = position();
    mNormSize  = size();

    switch( ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) )
    {
      case DISP_CHANGE_SUCCESSFUL:
      {
        RECT windowRect = { 0, 0, devmode.dmPelsWidth, devmode.dmPelsHeight };
        /*mStyle = */SetWindowLongPtr(hwnd(), GWL_STYLE, WS_POPUP | WS_VISIBLE );
        AdjustWindowRectEx(&windowRect, (DWORD)GetWindowLongPtr(hwnd(), GWL_STYLE), 0, (DWORD)GetWindowLongPtr(hwnd(), GWL_EXSTYLE) );
        SetWindowPos(hwnd(), HWND_TOP, windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_FRAMECHANGED );
        break;
      }
      #if(_WIN32_WINNT >= 0x0501)
        case DISP_CHANGE_BADDUALVIEW:
          MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_BADDUALVIEW", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
          return false;
      #endif
      case DISP_CHANGE_BADFLAGS:
        MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_BADFLAGS", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
      case DISP_CHANGE_BADMODE:
        MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_BADMODE", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
      case DISP_CHANGE_BADPARAM:
        MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_BADPARAM", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
      case DISP_CHANGE_FAILED:
        MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_FAILED", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
      case DISP_CHANGE_NOTUPDATED:
        MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_NOTUPDATED", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
      case DISP_CHANGE_RESTART:
        MessageBox(NULL, L"Full-screen mode switch failed: DISP_CHANGE_RESTART", L"Visualization Library Error", MB_OK | MB_ICONEXCLAMATION);
        return false;
      default:
        return false;
    }
  }

  mFullscreen = fullscreen_on;
  update();
  return true;
}
//-----------------------------------------------------------------------------
bool Win32Context::init(HGLRC share_context, const vl::String& title, const vl::OpenGLContextFormat& fmt, int x, int y, int width, int height)
{
  renderTarget()->setWidth(width);
  renderTarget()->setHeight(height);

  if (!hwnd())
  {
    MessageBox(NULL, L"Cannot create OpenGL context: null HWND.", L"Visualization Library Error", MB_OK);
    destroy();
    return false;
  }

  setWindowTitle(title);

  if (mHDC)
    DeleteDC(mHDC);
  mHDC = ::GetDC(hwnd());
  if (!mHDC)
  {
    MessageBox(NULL, L"Device context acquisition failed.", L"Visualization Library Error", MB_OK); 
    destroy();
    return false;
  }

  int pixel_format_index = vlWin32::choosePixelFormat(fmt);
  if (pixel_format_index == -1)
  {
    MessageBox(NULL, L"No suitable pixel fmt found.", L"Visualization Library Error", MB_OK); 
    destroy();
    return false;
  }

  if (SetPixelFormat(mHDC, pixel_format_index, NULL) == FALSE)
  {
    MessageBox(NULL, L"Pixel fmt setup failed.", L"Visualization Library Error", MB_OK);
    destroy();
    return false;
  }

  // OpenGL rendering context creation

  if (mHGLRC)
  {
    if ( wglDeleteContext(mHGLRC) == FALSE )
    {
      MessageBox(NULL, L"OpenGL context creation failed.\n"
       L"The handle either doesn't specify a valid context or the context is being used by another thread.", L"Visualization Library Error", MB_OK);
    }
    mHGLRC = NULL;
  }

  if (wglCreateContextAttribsARB && mContextAttribs.size() > 1)
  {
    // must be 0-terminated list
    VL_CHECK(mContextAttribs.back() == 0);
    // Creates an OpenGL 3.x / 4.x context with the specified attributes.
    mHGLRC = wglCreateContextAttribsARB(mHDC, 0, &mContextAttribs[0]);
  }
  else
  {
    // Creates default OpenGL context
    mHGLRC = wglCreateContext(mHDC);
  }

  if (!mHGLRC)
  {
    MessageBox(NULL, L"OpenGL rendering context creation failed.", L"Visualization Library Error", MB_OK);
    destroy();
    return false;
  }
  wglMakeCurrent(mHDC, mHGLRC);
  initGLContext();

  if (fmt.multisample() && !WGLEW_ARB_multisample)
    vl::Log::error("WGL_ARB_multisample not supported.\n");

  dispatchInitEvent();

  setPosition(x, y);
  setSize(width, height);

  if (WGLEW_EXT_swap_control)
    wglSwapIntervalEXT( fmt.vSync() ? 1 : 0 );

  if (share_context)
    shareOpenGLResources(share_context);

  if (fmt.fullscreen())
    setFullscreen(true);
  
  return true;
}
//-----------------------------------------------------------------------------
void Win32Context::setContextAttribs(const int* attribs)
{
  mContextAttribs.clear();
  for( ; *attribs; ++attribs )
    mContextAttribs.push_back(*attribs);
  mContextAttribs.push_back(0);
}
//-----------------------------------------------------------------------------
