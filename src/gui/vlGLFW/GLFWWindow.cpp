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

#include <vlGLFW/GLFWWindow.hpp>
#include <cstdlib>
#include <cstdio>

using namespace vlGLFW;
using namespace vl;

//-----------------------------------------------------------------------------
GLFWWindow::WinMapType GLFWWindow::mWinMap;
//-----------------------------------------------------------------------------

GLFWWindow::~GLFWWindow()
{
  destroyWindow();
}

GLFWWindow::GLFWWindow()
{
  mHandle = NULL;
  mUnicodeChar = 0;
}

GLFWWindow::GLFWWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int width, int height, GLFWwindow* share)
{
  mHandle = NULL;
  mUnicodeChar = 0;

  initGLFWWindow(title, info, width, height);
}

//! Initializes and shows a GLFW window.
bool GLFWWindow::initGLFWWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int width, int height, GLFWwindow* share)
{
  destroyWindow();

  // Allow user to set hints we don't support in here
  // glfwDefaultWindowHints();

  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode* mode = glfwGetVideoMode( monitor );

  glfwWindowHint( GLFW_RED_BITS, info.rgbaBits().r() ? info.rgbaBits().r() : mode->redBits );
  glfwWindowHint( GLFW_GREEN_BITS, info.rgbaBits().g() ? info.rgbaBits().g() : mode->greenBits );
  glfwWindowHint( GLFW_BLUE_BITS, info.rgbaBits().b() ? info.rgbaBits().b() : mode->blueBits );
  glfwWindowHint( GLFW_ALPHA_BITS, info.rgbaBits().a() ? info.rgbaBits().a() : GLFW_DONT_CARE );
  glfwWindowHint( GLFW_DEPTH_BITS, info.depthBufferBits() ? info.depthBufferBits() : GLFW_DONT_CARE );
  glfwWindowHint( GLFW_STENCIL_BITS, info.stencilBufferBits() ? info.stencilBufferBits() : GLFW_DONT_CARE );

  glfwWindowHint( GLFW_ACCUM_RED_BITS, info.accumRGBABits().r() ? info.accumRGBABits().r() : GLFW_DONT_CARE );
  glfwWindowHint( GLFW_ACCUM_GREEN_BITS, info.accumRGBABits().g() ? info.accumRGBABits().g() : GLFW_DONT_CARE );
  glfwWindowHint( GLFW_ACCUM_BLUE_BITS, info.accumRGBABits().b() ? info.accumRGBABits().b() : GLFW_DONT_CARE );
  glfwWindowHint( GLFW_ACCUM_ALPHA_BITS, info.accumRGBABits().a() ? info.accumRGBABits().a() : GLFW_DONT_CARE );

  glfwWindowHint( GLFW_AUX_BUFFERS, GLFW_DONT_CARE );

  glfwWindowHint( GLFW_STEREO, info.stereo() );

  glfwWindowHint( GLFW_SAMPLES, info.multisample() ? info.multisampleSamples() : 0 );

  glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_DONT_CARE );

  glfwWindowHint( GLFW_DOUBLEBUFFER, info.doubleBuffer() );

  glfwWindowHint( GLFW_REFRESH_RATE, mode->refreshRate );

  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );

  //glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR );
  //glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR );
  //glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT ); // Only for OpenGL 3.0 or above
  //glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT );
  //glfwWindowHint( GLFW_CONTEXT_ROBUSTNESS, GLFW_NO_RESET_NOTIFICATION|GLFW_LOSE_CONTEXT_ON_RESET|GLFW_NO_ROBUSTNESS);

  switch( info.openGLProfile() )
  {
  case vl::GLP_Compatibility:
    if ( info.majVersion() >= 3 && info.minVersion() >= 2 ) {
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE );
    }
    if ( info.majVersion() ) {
      glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, info.majVersion() );
      glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, info.minVersion() );
    }
    break;
  case vl::GLP_Core:
    if ( info.majVersion() >= 3 && info.minVersion() >= 2 ) {
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    }
    if ( info.majVersion() ) {
      glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, info.majVersion() );
      glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, info.minVersion() );
    }
    break;
  case vl::GLP_Default:
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE );
    break;
  }

  mFullscreen = info.fullscreen();
  if ( info.fullscreen() ) {
    width  = mode->width;
    height = mode->height;
  } else {
    monitor = NULL;
  }

  mHandle = glfwCreateWindow(width, height, title.toStdString().c_str(), monitor, share);

  if ( ! mHandle )
  {
    //glfwTerminate();
    //exit(EXIT_FAILURE);
    return false;
  } else {
    mWinMap[ mHandle ] = this;
  }

  glfwMakeContextCurrent( mHandle );

  initGLContext();

  // Init is dispatched before resize
  dispatchInitEvent();

  // Init is dispatched before resize
  dispatchResizeEvent( width, height );

  glfwSetWindowSizeCallback( mHandle, glfw_size_callback );
  glfwSetWindowPosCallback( mHandle, glfw_pos_callback );
  glfwSetWindowRefreshCallback( mHandle, glfw_refresh_callback );
  glfwSetWindowCloseCallback( mHandle, glfw_close_callback );
  glfwSetWindowFocusCallback( mHandle, glfw_focus_callback );
  glfwSetWindowIconifyCallback( mHandle, glfw_iconify_callback );

  glfwSetKeyCallback( mHandle, glfw_key_callback );
  glfwSetCharCallback( mHandle, glfw_unicode_callback );
  glfwSetCursorEnterCallback( mHandle, glfw_cursor_enter_callback );
  glfwSetCursorPosCallback( mHandle, glfw_cursor_pos_callback );
  glfwSetMouseButtonCallback( mHandle, glfw_mouse_button_callback );
  glfwSetScrollCallback( mHandle, glfw_scroll_callback );
  glfwSetDropCallback( mHandle, glfw_drop_callback );

  return true;
}

void GLFWWindow::destroyWindow()
{
  if ( mHandle )
  {
    dispatchDestroyEvent();
    glfwDestroyWindow( mHandle );
    mWinMap.erase( mHandle );
    mHandle = NULL;
  }
}

void GLFWWindow::makeCurrent()
{
  if ( mHandle )
  {
    glfwMakeContextCurrent( mHandle );
  }
}

void GLFWWindow::update() {
  dispatchUpdateEvent();
}

void GLFWWindow::swapBuffers() {
  if ( mHandle )
  {
    glfwSwapBuffers( mHandle );
  }
}

void GLFWWindow::glfw_size_callback(GLFWwindow* window, int w, int h)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    vlwin->dispatchResizeEvent( w, h );
  }
}

void GLFWWindow::glfw_pos_callback(GLFWwindow* window, int x, int y)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    // At the moment we don't have such event
    // vlwin->dispatchPositionEvent( focused );
  }
}

void GLFWWindow::glfw_refresh_callback(GLFWwindow* window)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    vlwin->dispatchUpdateEvent();
  }
}

void GLFWWindow::glfw_close_callback(GLFWwindow* window)
{
  if ( glfwWindowShouldClose( window ) ) {
    WinMapType::iterator it = mWinMap.find( window );
    GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
    if ( vlwin ) {
      vlwin->destroyWindow();
    }
  }
}

void GLFWWindow::glfw_focus_callback(GLFWwindow* window, int focused)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    // At the moment we don't have such event
    // vlwin->dispatchFocusEvent( focused );
  }
}

void GLFWWindow::glfw_iconify_callback(GLFWwindow* window, int iconified)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    vlwin->dispatchVisibilityEvent( ! iconified );
  }
}

void GLFWWindow::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    vl::EKey vlkey = translateKey( key, scancode, mods );

    switch( action ) {
    case GLFW_PRESS:
      vlwin->dispatchKeyPressEvent( 0, vlkey );
      break;
    case GLFW_RELEASE:
      vlwin->dispatchKeyReleaseEvent( vlwin->mUnicodeChar, vlkey );
      vlwin->mUnicodeChar = 0;
      break;
    case GLFW_REPEAT:
      break;
    }
  }
}

void GLFWWindow::glfw_unicode_callback(GLFWwindow* window, unsigned int unicode)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    // Note: this event is triggered after key press but before key release
    // this is a work around in order to get the unicode of the currently
    // pressed key - for a better input experience one should hook into this
    // callback directly without relying on key press/release events.
    vlwin->mUnicodeChar = (unsigned int)unicode;
  }
}

void GLFWWindow::glfw_cursor_enter_callback(GLFWwindow* window, int entered)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    // At the moment we don't have such event
  }
}

void GLFWWindow::glfw_cursor_pos_callback(GLFWwindow* window, double dx, double dy)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    int x = floor(dx);
    int y = floor(dy);
    vlwin->dispatchMouseMoveEvent( x, y );
  }
}

void GLFWWindow::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    vl::EMouseButton btn = vl::UnknownButton;
    switch( button )
    {
      case GLFW_MOUSE_BUTTON_LEFT:
        btn = vl::LeftButton;
        break;
      case GLFW_MOUSE_BUTTON_MIDDLE:
        btn = vl::MiddleButton;
        break;
      case GLFW_MOUSE_BUTTON_RIGHT:
        btn = vl::RightButton;
        break;
    }

    double dx = 0, dy = 0;
    glfwGetCursorPos( window, &dx, &dy );
    int x = floor(dx);
    int y = floor(dy);

    if (action == GLFW_PRESS)
      vlwin->dispatchMouseDownEvent(btn, x, y);
    else
    if (action == GLFW_RELEASE)
      vlwin->dispatchMouseUpEvent(btn, x, y);
  }
}

void GLFWWindow::glfw_scroll_callback(GLFWwindow* window, double dx, double dy)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    int x = floor(dx);
    int y = floor(dy);
    vlwin->dispatchMouseWheelEvent( y );
  }
}

void GLFWWindow::glfw_drop_callback(GLFWwindow * window, int count, const char** df)
{
  WinMapType::iterator it = mWinMap.find( window );
  GLFWWindow* vlwin = it != mWinMap.end() ? it->second : NULL;
  if ( vlwin ) {
    std::vector<String> files;
    for(int i=0; i<count; ++i) {
      files.push_back( df[ i ] );
    }
    vlwin->dispatchFileDroppedEvent( files );
  }
}

bool GLFWWindow::setFullscreen(bool fs)
{
  if ( mHandle )
  {
    mFullscreen = fs;
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode( monitor );
    int x = 100;
    int y = 100;
    int w = 512;
    int h = 512;
    int r = GLFW_DONT_CARE;
    if ( fs ) {
      x = 0;
      y = 0;
      w = mode->width;
      h = mode->height;
      r = mode->refreshRate;
    } else {
      monitor = 0;
    }
    glfwSetWindowMonitor( mHandle, monitor, x, y, w, h, r );
    if ( ! fs ) {
      setPosition( x, y );
    }
    return true;
  }

  return false;
}

void GLFWWindow::setWindowTitle(const vl::String& title)
{
  if ( mHandle )
  {
    glfwSetWindowTitle( mHandle, title.toStdString().c_str() );
  }
}

void GLFWWindow::setPosition(int x, int y)
{
  if ( mHandle )
  {
    glfwSetWindowPos( mHandle, x, y );
  }
}

void GLFWWindow::setSize(int w, int h)
{
  if ( mHandle )
  {
    glfwSetWindowSize( mHandle, w, h );
  }
}

vl::ivec2 GLFWWindow::position() const
{
  int x = 0, y = 0;
  if ( mHandle )
  {
    glfwGetWindowPos( mHandle, &x, &y );
  }
  return vl::ivec2( x, y );
}

vl::ivec2 GLFWWindow::size() const
{
  int w = 0, h = 0;
  if ( mHandle )
  {
    glfwGetWindowSize( mHandle, &w, &h );
  }
  return vl::ivec2( w, h );
}

void GLFWWindow::show()
{
  if ( mHandle )
  {
    glfwShowWindow( mHandle );
  }
}

void GLFWWindow::hide()
{
  if ( mHandle )
  {
    glfwHideWindow( mHandle );
  }
}

void GLFWWindow::getFocus()
{
  if ( mHandle )
  {
    glfwFocusWindow( mHandle );
  }
}

void GLFWWindow::setMouseVisible(bool visible)
{
  if ( mHandle )
  {
    glfwSetInputMode( mHandle, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED );
  }
}

void GLFWWindow::setMousePosition(int x, int y)
{
  if ( mHandle )
  {
    glfwSetCursorPos( mHandle, x, y );
  }
}

vl::EKey GLFWWindow::translateKey(int key, int scancode, int mods)
{
  // Other keys depending on keyboard layout
  //  vl::Key_Exclam
  //  vl::Key_QuoteDbl
  //  vl::Key_Hash
  //  vl::Key_Dollar
  //  vl::Key_Ampersand
  //  vl::Key_LeftParen
  //  vl::Key_RightParen
  //  vl::Key_Asterisk
  //  vl::Key_Plus
  //  vl::Key_Colon
  //  vl::Key_Less
  //  vl::Key_Greater
  //  vl::Key_Question
  //  vl::Key_At
  //  vl::Key_Caret
  //  vl::Key_Underscore

  switch( key ) {
    default: return vl::Key_Unknown;
    case GLFW_KEY_UNKNOWN: return vl::Key_Unknown;
    case GLFW_KEY_SPACE: return vl::Key_Space;
    case GLFW_KEY_APOSTROPHE: vl::Key_Quote;
    case GLFW_KEY_COMMA: return vl::Key_Comma;
    case GLFW_KEY_MINUS: return vl::Key_Minus;
    case GLFW_KEY_PERIOD: return vl::Key_Period;
    case GLFW_KEY_SLASH: return vl::Key_Slash;
    case GLFW_KEY_0: return vl::Key_0;
    case GLFW_KEY_1: return vl::Key_1;
    case GLFW_KEY_2: return vl::Key_2;
    case GLFW_KEY_3: return vl::Key_3;
    case GLFW_KEY_4: return vl::Key_4;
    case GLFW_KEY_5: return vl::Key_5;
    case GLFW_KEY_6: return vl::Key_6;
    case GLFW_KEY_7: return vl::Key_7;
    case GLFW_KEY_8: return vl::Key_8;
    case GLFW_KEY_9: return vl::Key_9;
    case GLFW_KEY_SEMICOLON: return vl::Key_Semicolon;
    case GLFW_KEY_EQUAL: return vl::Key_Equal;
    case GLFW_KEY_A: return vl::Key_A;
    case GLFW_KEY_B: return vl::Key_B;
    case GLFW_KEY_C: return vl::Key_C;
    case GLFW_KEY_D: return vl::Key_D;
    case GLFW_KEY_E: return vl::Key_E;
    case GLFW_KEY_F: return vl::Key_F;
    case GLFW_KEY_G: return vl::Key_G;
    case GLFW_KEY_H: return vl::Key_H;
    case GLFW_KEY_I: return vl::Key_I;
    case GLFW_KEY_J: return vl::Key_J;
    case GLFW_KEY_K: return vl::Key_K;
    case GLFW_KEY_L: return vl::Key_L;
    case GLFW_KEY_M: return vl::Key_M;
    case GLFW_KEY_N: return vl::Key_N;
    case GLFW_KEY_O: return vl::Key_O;
    case GLFW_KEY_P: return vl::Key_P;
    case GLFW_KEY_Q: return vl::Key_Q;
    case GLFW_KEY_R: return vl::Key_R;
    case GLFW_KEY_S: return vl::Key_S;
    case GLFW_KEY_T: return vl::Key_T;
    case GLFW_KEY_U: return vl::Key_U;
    case GLFW_KEY_V: return vl::Key_V;
    case GLFW_KEY_W: return vl::Key_W;
    case GLFW_KEY_X: return vl::Key_X;
    case GLFW_KEY_Y: return vl::Key_Y;
    case GLFW_KEY_Z: return vl::Key_Z;
    case GLFW_KEY_LEFT_BRACKET: return vl::Key_LeftBracket;
    case GLFW_KEY_BACKSLASH: return vl::Key_BackSlash;
    case GLFW_KEY_RIGHT_BRACKET: return vl::Key_RightBracket;
    case GLFW_KEY_GRAVE_ACCENT: vl::Key_QuoteLeft;
    //case GLFW_KEY_WORLD_1:
    //case GLFW_KEY_WORLD_2:
    case GLFW_KEY_ESCAPE: return vl::Key_Escape;
    case GLFW_KEY_ENTER: return vl::Key_Return;
    case GLFW_KEY_TAB: return vl::Key_Tab;
    case GLFW_KEY_BACKSPACE: return vl::Key_BackSpace;
    case GLFW_KEY_INSERT: return vl::Key_Insert;
    case GLFW_KEY_DELETE: return vl::Key_Delete;
    case GLFW_KEY_RIGHT: return vl::Key_Right;
    case GLFW_KEY_LEFT: return vl::Key_Left;
    case GLFW_KEY_DOWN: return vl::Key_Down;
    case GLFW_KEY_UP: return vl::Key_Up;
    case GLFW_KEY_PAGE_UP: return vl::Key_PageUp;
    case GLFW_KEY_PAGE_DOWN: return vl::Key_PageDown;
    case GLFW_KEY_HOME: return vl::Key_Home;
    case GLFW_KEY_END: return vl::Key_End;
    //case GLFW_KEY_CAPS_LOCK:
    //case GLFW_KEY_SCROLL_LOCK:
    //case GLFW_KEY_NUM_LOCK:
    case GLFW_KEY_PRINT_SCREEN: vl::Key_Print;
    case GLFW_KEY_PAUSE: return vl::Key_Pause;
    case GLFW_KEY_F1: return vl::Key_F1;
    case GLFW_KEY_F2: return vl::Key_F2;
    case GLFW_KEY_F3: return vl::Key_F3;
    case GLFW_KEY_F4: return vl::Key_F4;
    case GLFW_KEY_F5: return vl::Key_F5;
    case GLFW_KEY_F6: return vl::Key_F6;
    case GLFW_KEY_F7: return vl::Key_F7;
    case GLFW_KEY_F8: return vl::Key_F8;
    case GLFW_KEY_F9: return vl::Key_F9;
    case GLFW_KEY_F10: return vl::Key_F10;
    case GLFW_KEY_F11: return vl::Key_F11;
    case GLFW_KEY_F12: return vl::Key_F12;
    //case GLFW_KEY_F13:
    //case GLFW_KEY_F14:
    //case GLFW_KEY_F15:
    //case GLFW_KEY_F16:
    //case GLFW_KEY_F17:
    //case GLFW_KEY_F18:
    //case GLFW_KEY_F19:
    //case GLFW_KEY_F20:
    //case GLFW_KEY_F21:
    //case GLFW_KEY_F22:
    //case GLFW_KEY_F23:
    //case GLFW_KEY_F24:
    //case GLFW_KEY_F25:
    case GLFW_KEY_KP_0: return vl::Key_0;
    case GLFW_KEY_KP_1: return vl::Key_1;
    case GLFW_KEY_KP_2: return vl::Key_2;
    case GLFW_KEY_KP_3: return vl::Key_3;
    case GLFW_KEY_KP_4: return vl::Key_4;
    case GLFW_KEY_KP_5: return vl::Key_5;
    case GLFW_KEY_KP_6: return vl::Key_6;
    case GLFW_KEY_KP_7: return vl::Key_7;
    case GLFW_KEY_KP_8: return vl::Key_8;
    case GLFW_KEY_KP_9: return vl::Key_9;
    case GLFW_KEY_KP_DECIMAL: return vl::Key_Period;
    case GLFW_KEY_KP_DIVIDE: return vl::Key_Slash;
    case GLFW_KEY_KP_MULTIPLY: return vl::Key_Asterisk;
    case GLFW_KEY_KP_SUBTRACT: return vl::Key_Minus;
    case GLFW_KEY_KP_ADD: return vl::Key_Plus;
    case GLFW_KEY_KP_ENTER: return vl::Key_Return;
    case GLFW_KEY_KP_EQUAL: return vl::Key_Equal;
    case GLFW_KEY_LEFT_SHIFT: return vl::Key_Shift;
    case GLFW_KEY_LEFT_CONTROL: return vl::Key_Ctrl;
    case GLFW_KEY_LEFT_ALT: return vl::Key_Alt;
    //case GLFW_KEY_LEFT_SUPER:
    case GLFW_KEY_RIGHT_SHIFT: return vl::Key_Shift;
    case GLFW_KEY_RIGHT_CONTROL: return vl::Key_Ctrl;
    case GLFW_KEY_RIGHT_ALT: return vl::Key_Alt;
    //case GLFW_KEY_RIGHT_SUPER:
    // case GLFW_KEY_MENU:
  }
}

