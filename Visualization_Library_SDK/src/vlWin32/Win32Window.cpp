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

#include "vlWin32/Win32Window.hpp"
#include "vl/Log.hpp"
#include "vl/Say.hpp"
#include "vl/Time.hpp"
#include <shellapi.h>

using namespace vl;
using namespace vlWin32;

//-----------------------------------------------------------------------------
namespace
{
  #if 0
    // used for debugging purposes
    void win32PrintError(LPTSTR lpszFunction) 
    { 
        TCHAR szBuf[80]; 
        LPVOID lpMsgBuf;
        DWORD dw = GetLastError(); 

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, NULL );

        wsprintf(szBuf, 
            L"%s failed with error %d: %s", 
            lpszFunction, dw, lpMsgBuf); 
     
        MessageBox(NULL, szBuf, L"Visualization Library Error", MB_OK); 

        LocalFree(lpMsgBuf);
    }
  #endif

  bool registerClass()
  {
    static bool class_already_registered = false;
    if (!class_already_registered)
    {
      WNDCLASS wc;
      memset(&wc, 0, sizeof(wc));
      /* only register the window class once. */
      wc.style         = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
      wc.lpfnWndProc   = (WNDPROC)Win32Window::WindowProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = 0;
      wc.hInstance     = GetModuleHandle(NULL);
      wc.hIcon         = LoadIcon(NULL, IDI_WINLOGO);
      wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
      wc.hbrBackground = NULL;
      wc.lpszMenuName  = NULL;
      wc.lpszClassName = Win32Window::Win32WindowClassName;

      if (!RegisterClass(&wc))
        MessageBox(NULL, L"Class registration failed.", L"Visualization Library Error", MB_OK);
      else
        class_already_registered = true;
    }
    return class_already_registered;
  }
}
//-----------------------------------------------------------------------------
const wchar_t* Win32Window::Win32WindowClassName = L"VisualizationLibraryWindowClass";
//-----------------------------------------------------------------------------
std::map< HWND, Win32Window* > Win32Window::mWinMap;
//-----------------------------------------------------------------------------
LONG WINAPI Win32Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{ 
  Win32Window* win = Win32Window::getWindow(hWnd);
  if (!win)
    return (LONG)DefWindowProc(hWnd, uMsg, wParam, lParam); 

  switch(uMsg) 
  {
    case WM_PAINT:
    {
      // handle event and then dispatch: solves MessageBox dialog problem.
      LONG val = (LONG)DefWindowProc(hWnd, uMsg, wParam, lParam); 
      if (win->hglrc() && win->hdc() && win->hwnd())
        win->dispatchRunEvent();
      return val;
    }

    case WM_SIZE:
    {
      win->renderTarget()->setWidth( LOWORD(lParam) );
      win->renderTarget()->setHeight( HIWORD(lParam) );
      win->dispatchResizeEvent( LOWORD(lParam), HIWORD(lParam) );
      break;
    }

    case WM_MOUSEMOVE:
    {
      POINTS pt = MAKEPOINTS(lParam);
      win->dispatchMouseMoveEvent( pt.x, pt.y );
      break;
    }

    case WM_LBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    {
      win->mMouseDownCount++;
      if (win->mMouseDownCount == 1)
        SetCapture(win->hwnd());
      EMouseButton button = UnknownButton;
      if (uMsg == WM_LBUTTONDBLCLK || uMsg == WM_LBUTTONDOWN)
        button = LeftButton;
      else if (uMsg == WM_MBUTTONDBLCLK || uMsg == WM_MBUTTONDOWN)
        button = MiddleButton;
      else if (uMsg == WM_RBUTTONDBLCLK || uMsg == WM_RBUTTONDOWN)
        button = RightButton;
      POINTS pt = MAKEPOINTS(lParam);
      win->dispatchMouseDownEvent( button, pt.x, pt.y );
      break;
    }

    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
      win->mMouseDownCount--;
      if (win->mMouseDownCount <= 0)
      {
        ReleaseCapture();
        win->mMouseDownCount = 0;
      }
      EMouseButton button = UnknownButton;
      if (uMsg == WM_LBUTTONUP)
        button = LeftButton;
      else if (uMsg == WM_MBUTTONUP)
        button = MiddleButton;
      else if (uMsg == WM_RBUTTONUP)
        button = RightButton;
      POINTS pt = MAKEPOINTS(lParam);
      win->dispatchMouseUpEvent( button, pt.x, pt.y );
      break;
    }

    // If you get a compilation error here:
    // 1 - you didn't define _WIN32_WINNT as 0x0400 or above.
    // 2 - you are trying to compile VL with a VERY old (unsupported) Visual Studio / Platform SDK.
    case WM_MOUSEWHEEL:
    {
      win->dispatchMouseWheelEvent( GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA );
      break;
    }

    case WM_CLOSE:
    {
      win->destroy();
      break;
    }

    case WM_KEYDOWN:
    {
      unsigned short unicode_out = 0;
      vl::EKey       key_out     = Key_None;
      translateKeyEvent(wParam, lParam, unicode_out, key_out);
      win->dispatchKeyPressEvent(unicode_out, key_out);
      break;
    }

    case WM_KEYUP:
    {
      unsigned short unicode_out = 0;
      vl::EKey       key_out     = Key_None;
      translateKeyEvent(wParam, lParam, unicode_out, key_out);
      win->dispatchKeyReleaseEvent(unicode_out, key_out);
      break;
    }

    case WM_DROPFILES:
    {
      HDROP hDrop = (HDROP)wParam;
      int count = DragQueryFile(hDrop, 0xFFFFFFFF, 0, 0);
      const int char_count = 1024;
      std::vector<String> files;
      for(int i=0; i<count; ++i)
      {
        wchar_t file_path[char_count];
        memset(file_path, 0, char_count);
        DragQueryFile(hDrop,i,file_path,char_count);
        files.push_back(file_path);
      }
      win->dispatchFileDroppedEvent(files);
      break;
    }

    // WM_SYSKEYDOWN
    // WM_SYSKEYUP
    // WM_GETICON
    // WM_SETCURSOR
    // WM_SETICON
    // WM_CAPTURECHANGED
    // WM_MOUSEFIRST 
  }

  return (LONG)DefWindowProc(hWnd, uMsg, wParam, lParam); 
}
//-----------------------------------------------------------------------------
// Win32Window
//-----------------------------------------------------------------------------
Win32Window::Win32Window()
{
  mHWND  = NULL;
  mHDC   = NULL;
  mHGLRC = NULL;
  mMouseDownCount = 0;

  mStyle   = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
  mExStyle = WS_EX_APPWINDOW | WS_EX_ACCEPTFILES;
  mWindowClassName = Win32WindowClassName;
}
//-----------------------------------------------------------------------------
Win32Window::~Win32Window()
{
  destroy(); // destroyWindow();
}
//-----------------------------------------------------------------------------
bool Win32Window::initWin32Window(HWND parent, HGLRC share_context, const vl::String& title, const vl::OpenGLContextFormat& fmt, int x, int y, int width, int height)
{
  destroyWindow();

  if (!registerClass())
    return false;

  unsigned style = mStyle & ~(WS_CHILD|WS_OVERLAPPEDWINDOW);;
  style |= parent?WS_CHILD:WS_OVERLAPPEDWINDOW;

  mHWND = CreateWindowEx(
    mExStyle,
    mWindowClassName,
    L"Visualization Library Win32",
    style,
    x, y, width, height, 
    parent, NULL, GetModuleHandle(NULL), NULL);

  if (init(share_context, title, fmt, x, y, width, height))
  {
    mWinMap[mHWND] = this;
    return true;
  }
  else
    return false;
}
//-----------------------------------------------------------------------------
Win32Window* Win32Window::getWindow(HWND hWnd)
{
  std::map< HWND, Win32Window* >::const_iterator it = mWinMap.find(hWnd);
  if (it != mWinMap.end())
    return it->second;
  else 
    return NULL;
}
//-----------------------------------------------------------------------------
void Win32Window::destroy()
{
  Win32Context::destroy();
  /*dispatchDestroyEvent();*/
  destroyWindow();
}
//-----------------------------------------------------------------------------
void Win32Window::destroyWindow()
{
  // wglMakeCurrent(NULL, NULL); // not needed 
  if (hwnd())
  {
    bool destroy_win = mWinMap.find(mHWND) != mWinMap.end();
    mWinMap.erase(mHWND);

    if (mHGLRC)
    {
      if ( wglDeleteContext(mHGLRC) == FALSE )
      {
        MessageBox(NULL, L"OpenGL context creation failed.\n"
         L"The handle either doesn't specify a valid context or the context is being used by another thread.", L"Visualization Library Error", MB_OK);
      }
      mHGLRC = NULL;
    }
    if (mHDC)
    {
      DeleteDC(mHDC);
      mHDC = NULL;
    }
    if (destroy_win)
    {
      DestroyWindow(mHWND);
      mHWND = NULL;
    }
  }
}
//-----------------------------------------------------------------------------
void vlWin32::dispatchUpdate()
{
  // iterate over all opengl contexts
  std::map< HWND, Win32Window* > wins = Win32Window::winMap();
  for( std::map< HWND, Win32Window* >::iterator it = wins.begin();
       it != wins.end(); 
       ++it )
  {
    Win32Window* win = it->second;
    if ( win->continuousUpdate() )
      win->update();
    else
      Sleep(10);
  }
}
//-----------------------------------------------------------------------------
void vlWin32::peekMessage(MSG& msg)
{
  if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
  {
    if (msg.message != WM_QUIT)
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
  else
    dispatchUpdate();
}
//-----------------------------------------------------------------------------
int vlWin32::messageLoop()
{
  while(!Win32Window::winMap().empty())
  {
    MSG msg = {0,0,0,0,0,0,0};
    peekMessage(msg);
    if (msg.message == WM_QUIT)
      return (int)msg.wParam;
  }
  return 0; /* never reached */
}
//-----------------------------------------------------------------------------
void vlWin32::translateKeyEvent(WPARAM wParam, LPARAM lParam, unsigned short& unicode_out, vl::EKey& key_out)
{
  // translate non unicode characters
  key_out     = Key_None;
  unicode_out = 0;

  switch(wParam)
  {
    case VK_CLEAR:    key_out = Key_Clear; break;
    case VK_CONTROL:  key_out = Key_Ctrl; break;
    case VK_LCONTROL: key_out = Key_LeftCtrl; break;
    case VK_RCONTROL: key_out = Key_RightCtrl; break;
    case VK_MENU:     key_out = Key_Alt; break;
    case VK_LMENU:    key_out = Key_LeftAlt; break;
    case VK_RMENU:    key_out = Key_RightAlt; break;
    case VK_SHIFT:    key_out = Key_Shift; break;
    case VK_LSHIFT:   key_out = Key_LeftShift; break;
    case VK_RSHIFT:   key_out = Key_RightShift; break;
    case VK_INSERT:   key_out = Key_Insert; break;
    case VK_DELETE:   key_out = Key_Delete; break;
    case VK_HOME:     key_out = Key_Home; break;
    case VK_END:      key_out = Key_End; break;
    case VK_PRINT:    key_out = Key_Print; break;
    case VK_PAUSE:    key_out = Key_Pause; break;
    case VK_PRIOR:    key_out = Key_PageUp; break;
    case VK_NEXT:     key_out = Key_PageDown; break;
    case VK_LEFT:     key_out = Key_Left; break;
    case VK_RIGHT:    key_out = Key_Right; break;
    case VK_UP:       key_out = Key_Up; break;
    case VK_DOWN:     key_out = Key_Down; break;
    case VK_F1:       key_out = Key_F1; break;
    case VK_F2:       key_out = Key_F2; break;
    case VK_F3:       key_out = Key_F3; break;
    case VK_F4:       key_out = Key_F4; break;
    case VK_F5:       key_out = Key_F5; break;
    case VK_F6:       key_out = Key_F6; break;
    case VK_F7:       key_out = Key_F7; break;
    case VK_F8:       key_out = Key_F8; break;
    case VK_F9:       key_out = Key_F9; break;
    case VK_F10:      key_out = Key_F10; break;
    case VK_F11:      key_out = Key_F11; break;
    case VK_F12:      key_out = Key_F12; break;

    /*
     * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
     * 0x40 : is unassigned
     * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
     */

    case L'0': key_out = Key_0; break;
    case L'1': key_out = Key_1; break;
    case L'2': key_out = Key_2; break;
    case L'3': key_out = Key_3; break;
    case L'4': key_out = Key_4; break;
    case L'5': key_out = Key_5; break;
    case L'6': key_out = Key_6; break;
    case L'7': key_out = Key_7; break;
    case L'8': key_out = Key_8; break;
    case L'9': key_out = Key_9; break;

    case L'A': key_out = Key_A; break;
    case L'B': key_out = Key_B; break;
    case L'C': key_out = Key_C; break;
    case L'D': key_out = Key_D; break;
    case L'E': key_out = Key_E; break;
    case L'F': key_out = Key_F; break;
    case L'G': key_out = Key_G; break;
    case L'H': key_out = Key_H; break;
    case L'I': key_out = Key_I; break;
    case L'J': key_out = Key_J; break;
    case L'K': key_out = Key_K; break;
    case L'L': key_out = Key_L; break;
    case L'M': key_out = Key_M; break;
    case L'N': key_out = Key_N; break;
    case L'O': key_out = Key_O; break;
    case L'P': key_out = Key_P; break;
    case L'Q': key_out = Key_Q; break;
    case L'R': key_out = Key_R; break;
    case L'S': key_out = Key_S; break;
    case L'T': key_out = Key_T; break;
    case L'U': key_out = Key_U; break;
    case L'V': key_out = Key_V; break;
    case L'W': key_out = Key_W; break;
    case L'X': key_out = Key_X; break;
    case L'Y': key_out = Key_Y; break;
    case L'Z': key_out = Key_Z; break;
  }

  // fill unicode
  BYTE mskeys[256];
  memset( mskeys, 0, sizeof(BYTE)*256 );
  WCHAR unicode[4] = { 0, 0 };
  if ( ToUnicode( (UINT)wParam, (UINT)((lParam >> 16) & 0xFF), mskeys, unicode, 4, 0 ) == 1 )
  {
    unicode_out = unicode[0];

    // fill key
    switch(unicode_out)
    {
      case L'0': key_out = Key_0; break;
      case L'1': key_out = Key_1; break;
      case L'2': key_out = Key_2; break;
      case L'3': key_out = Key_3; break;
      case L'4': key_out = Key_4; break;
      case L'5': key_out = Key_5; break;
      case L'6': key_out = Key_6; break;
      case L'7': key_out = Key_7; break;
      case L'8': key_out = Key_8; break;
      case L'9': key_out = Key_9; break;

      case L'A': key_out = Key_A; break;
      case L'B': key_out = Key_B; break;
      case L'C': key_out = Key_C; break;
      case L'D': key_out = Key_D; break;
      case L'E': key_out = Key_E; break;
      case L'F': key_out = Key_F; break;
      case L'G': key_out = Key_G; break;
      case L'H': key_out = Key_H; break;
      case L'I': key_out = Key_I; break;
      case L'J': key_out = Key_J; break;
      case L'K': key_out = Key_K; break;
      case L'L': key_out = Key_L; break;
      case L'M': key_out = Key_M; break;
      case L'N': key_out = Key_N; break;
      case L'O': key_out = Key_O; break;
      case L'P': key_out = Key_P; break;
      case L'Q': key_out = Key_Q; break;
      case L'R': key_out = Key_R; break;
      case L'S': key_out = Key_S; break;
      case L'T': key_out = Key_T; break;
      case L'U': key_out = Key_U; break;
      case L'V': key_out = Key_V; break;
      case L'W': key_out = Key_W; break;
      case L'X': key_out = Key_X; break;
      case L'Y': key_out = Key_Y; break;
      case L'Z': key_out = Key_Z; break;

      case L'a': key_out = Key_A; break;
      case L'b': key_out = Key_B; break;
      case L'c': key_out = Key_C; break;
      case L'd': key_out = Key_D; break;
      case L'e': key_out = Key_E; break;
      case L'f': key_out = Key_F; break;
      case L'g': key_out = Key_G; break;
      case L'h': key_out = Key_H; break;
      case L'i': key_out = Key_I; break;
      case L'j': key_out = Key_J; break;
      case L'k': key_out = Key_K; break;
      case L'l': key_out = Key_L; break;
      case L'm': key_out = Key_M; break;
      case L'n': key_out = Key_N; break;
      case L'o': key_out = Key_O; break;
      case L'p': key_out = Key_P; break;
      case L'q': key_out = Key_Q; break;
      case L'r': key_out = Key_R; break;
      case L's': key_out = Key_S; break;
      case L't': key_out = Key_T; break;
      case L'u': key_out = Key_U; break;
      case L'v': key_out = Key_V; break;
      case L'w': key_out = Key_W; break;
      case L'x': key_out = Key_X; break;
      case L'y': key_out = Key_Y; break;
      case L'z': key_out = Key_Z; break;

      case 13: key_out = Key_Return; break;
      case 8: key_out = Key_BackSpace; break;
      case 9: key_out = Key_Tab; break;
      case L' ': key_out = Key_Space; break;

      case 27: key_out = Key_Escape; break;
      case L'!': key_out = Key_Exclam; break;
      case L'"': key_out = Key_QuoteDbl; break;
      case L'#': key_out = Key_Hash; break;
      case L'$': key_out = Key_Dollar; break;
      case L'&': key_out = Key_Ampersand; break;
      case L'\'': key_out = Key_Quote; break;
      case L'(': key_out = Key_LeftParen; break;
      case L')': key_out = Key_RightParen; break;
      case L'*': key_out = Key_Asterisk; break;
      case L'+': key_out = Key_Plus; break;
      case L',': key_out = Key_Comma; break;
      case L'-': key_out = Key_Minus; break;
      case L'.': key_out = Key_Period; break;
      case L'\\': key_out = Key_Slash; break;
      case L':': key_out = Key_Colon; break;
      case L';': key_out = Key_Semicolon; break;
      case L'<': key_out = Key_Less; break;
      case L'=': key_out = Key_Equal; break;
      case L'>': key_out = Key_Greater; break;
      case L'?': key_out = Key_Question; break;
      case L'@': key_out = Key_At; break;
      case L'[': key_out = Key_LeftBracket; break;
      case L'/': key_out = Key_BackSlash; break;
      case L']': key_out = Key_RightBracket; break;
      case L'|': key_out = Key_Caret; break;
      case L'_': key_out = Key_Underscore; break;
      case L'`': key_out = Key_QuoteLeft; break;
    }
  }
}
//-----------------------------------------------------------------------------
int vlWin32::choosePixelFormat(const vl::OpenGLContextFormat& fmt, bool verbose)
{
  if (!registerClass())
    return false;

  // this is true only under Win32
  // VL_CHECK( sizeof(wchar_t) == sizeof(short int) )

  HWND hWnd = CreateWindowEx(
    WS_EX_APPWINDOW | WS_EX_ACCEPTFILES,
    Win32Window::Win32WindowClassName,
    L"Temp GL Window",
    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
    NULL, NULL, GetModuleHandle(NULL), NULL);

  if (!hWnd)
  {
    if (verbose) MessageBox(NULL, L"choosePixelFormat() critical failure: could not create window.", L"Visualization Library error", MB_OK);
    return -1;
  }

  HDC hDC = GetDC(hWnd);
  if (!hDC)
  {
    if (verbose) MessageBox(NULL, L"choosePixelFormat() critical failure: could not create HDC.", L"Visualization Library error", MB_OK);
    DestroyWindow(hWnd);
    return -1;
  }

  PIXELFORMATDESCRIPTOR pfd;
  memset(&pfd, 0, sizeof(pfd));
  pfd.nSize           = sizeof(pfd);
  pfd.nVersion        = 1;
  pfd.dwFlags         = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
  pfd.dwFlags         |= fmt.doubleBuffer() ? PFD_DOUBLEBUFFER : 0;
  pfd.dwFlags         |= fmt.stereo() ? PFD_STEREO : 0;
  pfd.iPixelType      = PFD_TYPE_RGBA;
  pfd.cColorBits      = 0;
  pfd.cRedBits        = (BYTE)fmt.rgbaBits().r();
  pfd.cGreenBits      = (BYTE)fmt.rgbaBits().g();
  pfd.cBlueBits       = (BYTE)fmt.rgbaBits().b();
  pfd.cAlphaBits      = (BYTE)fmt.rgbaBits().a();
  pfd.cAccumRedBits   = (BYTE)fmt.accumRGBABits().r();
  pfd.cAccumGreenBits = (BYTE)fmt.accumRGBABits().g();
  pfd.cAccumBlueBits  = (BYTE)fmt.accumRGBABits().b();
  pfd.cAccumAlphaBits = (BYTE)fmt.accumRGBABits().a();
  pfd.cDepthBits      = (BYTE)fmt.depthBufferBits();
  pfd.cStencilBits    = (BYTE)fmt.stencilBufferBits();
  pfd.iLayerType      = PFD_MAIN_PLANE;

  int pixel_format_index = ChoosePixelFormat(hDC, &pfd);

  if (pixel_format_index == 0)
  {
    if (verbose) MessageBox(NULL, L"choosePixelFormat() critical failure: could not choose temporary format.", L"Visualization Library error", MB_OK);
    DeleteDC(hDC);
    DestroyWindow(hWnd);
    return -1;
  }

  if (SetPixelFormat(hDC, pixel_format_index, &pfd) == FALSE)
  {
    if (verbose) MessageBox(NULL, L"choosePixelFormat() critical failure: could not set temporary format.", L"Visualization Library error", MB_OK);
    DeleteDC(hDC);
    DestroyWindow(hWnd);
    return -1;
  }

  // OpenGL Rendering Context
  HGLRC hGLRC = wglCreateContext(hDC);
  if (!hGLRC)
  {
    if (verbose) MessageBox(NULL, L"choosePixelFormat() critical failure: could not create temporary OpenGL context.", L"Visualization Library error", MB_OK);
    DeleteDC(hDC);
    DestroyWindow(hWnd);
    return -1;
  }

  wglMakeCurrent(hDC, hGLRC);

  // init opengl
  const char* err = initOpenGLExtensions();
  if (err != NULL)
  {
    if (verbose) MessageBox(NULL, L"choosePixelFormat() critical failure during GLEW initialization.", L"Visualization Library error", MB_OK);
    DeleteDC(hDC);
    DestroyWindow(hWnd);
    return -1;
  }

  // if this is not supported we use the current 'pixel_format_index' returned by ChoosePixelFormat above.

  int samples = 0;
  if(Has_WGL_ARB_pixel_format && fmt.multisample())
  {
    float fAttributes[] = { 0, 0 };
    int iAttributes[] =
    {
      // multi sampling
	    WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
      WGL_SAMPLES_ARB,        -1, // this is set below
      // generic
	    WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
	    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
	    WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
      // color buffer
      WGL_RED_BITS_ARB,         pfd.cRedBits,
      WGL_GREEN_BITS_ARB,       pfd.cGreenBits,
      WGL_BLUE_BITS_ARB,        pfd.cBlueBits,
      WGL_ALPHA_BITS_ARB,       pfd.cAlphaBits,
      // accumulation buffer
      WGL_ACCUM_RED_BITS_ARB,   pfd.cAccumRedBits,
      WGL_ACCUM_GREEN_BITS_ARB, pfd.cAccumGreenBits,
      WGL_ACCUM_BLUE_BITS_ARB,  pfd.cAccumBlueBits,
      WGL_ACCUM_ALPHA_BITS_ARB, pfd.cAccumAlphaBits,
      // depth buffer
      WGL_DEPTH_BITS_ARB,       pfd.cDepthBits,
      WGL_DOUBLE_BUFFER_ARB,    fmt.doubleBuffer() ? GL_TRUE : GL_FALSE,
      // stencil buffer
      WGL_STENCIL_BITS_ARB,     pfd.cStencilBits,
      // stereo
      WGL_STEREO_ARB,           fmt.stereo() ? GL_TRUE : GL_FALSE,
	    0,0
    };

    for(samples = fmt.multisampleSamples(); samples > 1; samples/=2)
    {
      // sets WGL_SAMPLES_ARB value
      iAttributes[3] = samples;
      pixel_format_index = -1;
      UINT num_formats  = 0;
      if ( wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixel_format_index,&num_formats) && num_formats >= 1 )
        break;
      else
        pixel_format_index = -1;
    }
  }

  // destroy temporary HWND, HDC, HGLRC
  if ( wglDeleteContext(hGLRC) == FALSE )
    if (verbose) MessageBox(NULL, L"Error deleting temporary OpenGL context, wglDeleteContext(hGLRC) failed.", L"Visualization Library error", MB_OK);
  DeleteDC(hDC);
  DestroyWindow(hWnd);

  if (verbose)
  {
    if(pixel_format_index == -1)
      vl::Log::error("No suitable pixel format found.\n");
    else
    {
      // check the returned pixel format
      #ifndef NEDBUG
        DescribePixelFormat(hDC, pixel_format_index, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
        vl::Log::print(" --- Pixel Format ---\n");
        // This one returns "not supported" even when its supported...
        // vl::Log::print( vl::Say("  OpenGL        = %s\n") << (pfd.dwFlags & PFD_SUPPORT_OPENGL ? "Supported" : "Not supported") );
        vl::Log::print( vl::Say("RGBA Bits     = %n %n %n %n\n") << pfd.cRedBits << pfd.cGreenBits << pfd.cBlueBits << pfd.cAlphaBits);
        vl::Log::print( vl::Say("Depth Bits    = %n\n")  << pfd.cDepthBits );
        vl::Log::print( vl::Say("Stencil Bits  = %n \n") << pfd.cStencilBits);
        vl::Log::print( vl::Say("Double Buffer = %s\n")  << (pfd.dwFlags & PFD_DOUBLEBUFFER ? "Yes" : "No") );
        vl::Log::print( vl::Say("Stereo        = %s\n")  << (pfd.dwFlags & PFD_STEREO ? "Yes" : "No") );
        vl::Log::print( vl::Say("Samples       = %n\n")  << samples );
        vl::Log::print("\n");
      #endif
    }
  }

  return pixel_format_index;
}
//-----------------------------------------------------------------------------
