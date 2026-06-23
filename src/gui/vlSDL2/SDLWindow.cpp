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

#include "SDLWindow.hpp"
#include "vlGraphics/OpenGL.hpp"
#include "vlGraphics/Applet.hpp"
#include "vlCore/VisualizationLibrary.hpp"
#include "vlCore/Log.hpp"
#include "vlCore/Say.hpp"
#include <algorithm>
#include <SDL2/SDL.h>

#ifdef WIN32
  #include <SDL2/SDL_syswm.h>
  #include <shellapi.h>
#endif

using namespace vlSDL2;
using namespace vl;

//-----------------------------------------------------------------------------
SDLWindow::WinMapType SDLWindow::mWinMap;
//-----------------------------------------------------------------------------

namespace
{
  SDLWindow* mSDLWindow = NULL;
  bool mUpdateFlag = true;

  std::map<int, vl::EKey> key_translation_map;

  int key_translation_vec[] =
  {
    SDLK_0, vl::Key_0,
    SDLK_1, vl::Key_1,
    SDLK_2, vl::Key_2,
    SDLK_3, vl::Key_3,
    SDLK_4, vl::Key_4,
    SDLK_5, vl::Key_5,
    SDLK_6, vl::Key_6,
    SDLK_7, vl::Key_7,
    SDLK_8, vl::Key_8,
    SDLK_9, vl::Key_9,

    SDLK_a, vl::Key_A,
    SDLK_b, vl::Key_B,
    SDLK_c, vl::Key_C,
    SDLK_d, vl::Key_D,
    SDLK_e, vl::Key_E,
    SDLK_f, vl::Key_F,
    SDLK_g, vl::Key_G,
    SDLK_h, vl::Key_H,
    SDLK_i, vl::Key_I,
    SDLK_j, vl::Key_J,
    SDLK_k, vl::Key_K,
    SDLK_l, vl::Key_L,
    SDLK_m, vl::Key_M,
    SDLK_n, vl::Key_N,
    SDLK_o, vl::Key_O,
    SDLK_p, vl::Key_P,
    SDLK_q, vl::Key_Q,
    SDLK_r, vl::Key_R,
    SDLK_s, vl::Key_S,
    SDLK_t, vl::Key_T,
    SDLK_u, vl::Key_U,
    SDLK_v, vl::Key_V,
    SDLK_w, vl::Key_W,
    SDLK_x, vl::Key_X,
    SDLK_y, vl::Key_Y,
    SDLK_z, vl::Key_Z,

    SDLK_RETURN,       vl::Key_Return,
    SDLK_BACKSPACE,    vl::Key_BackSpace,
    SDLK_SPACE,        vl::Key_Space,
    SDLK_TAB,          vl::Key_Tab,
    SDLK_CLEAR,        vl::Key_Clear,
    SDLK_ESCAPE,       vl::Key_Escape,
    SDLK_EXCLAIM,      vl::Key_Exclam,
    SDLK_QUOTEDBL,     vl::Key_QuoteDbl,
    SDLK_HASH,         vl::Key_Hash,
    SDLK_DOLLAR,       vl::Key_Dollar,
    SDLK_AMPERSAND,    vl::Key_Ampersand,
    SDLK_QUOTE,        vl::Key_Quote,
    SDLK_LEFTPAREN,    vl::Key_LeftParen,
    SDLK_RIGHTPAREN,   vl::Key_RightParen,
    SDLK_ASTERISK,     vl::Key_Asterisk,
    SDLK_PLUS,         vl::Key_Plus,
    SDLK_COMMA,        vl::Key_Comma,
    SDLK_MINUS,        vl::Key_Minus,
    SDLK_PERIOD,       vl::Key_Period,
    SDLK_SLASH,        vl::Key_Slash,
    SDLK_COLON,        vl::Key_Colon,
    SDLK_SEMICOLON,    vl::Key_Semicolon,
    SDLK_LESS,         vl::Key_Less,
    SDLK_EQUALS,       vl::Key_Equal,
    SDLK_GREATER,      vl::Key_Greater,
    SDLK_QUESTION,     vl::Key_Question,
    SDLK_AT,           vl::Key_At,
    SDLK_LEFTBRACKET,  vl::Key_LeftBracket,
    SDLK_BACKSLASH,    vl::Key_BackSlash,
    SDLK_RIGHTBRACKET, vl::Key_RightBracket,
    SDLK_CARET,        vl::Key_Caret,
    SDLK_UNDERSCORE,   vl::Key_Underscore,
    SDLK_BACKQUOTE,    vl::Key_QuoteLeft,

    // non unicode

    SDLK_LEFT,     vl::Key_Left,
    SDLK_RIGHT,    vl::Key_Right,
    SDLK_UP,       vl::Key_Up,
    SDLK_DOWN,     vl::Key_Down,
    SDLK_LCTRL,    vl::Key_LeftCtrl,
    SDLK_RCTRL,    vl::Key_RightCtrl,
    SDLK_LSHIFT,   vl::Key_LeftShift,
    SDLK_RSHIFT,   vl::Key_RightShift,
    SDLK_LALT,     vl::Key_LeftAlt,
    SDLK_RALT,     vl::Key_RightAlt,
    SDLK_INSERT,   vl::Key_Insert,
    SDLK_DELETE,   vl::Key_Delete,
    SDLK_HOME,     vl::Key_Home,
    SDLK_END,      vl::Key_End,
    SDLK_PAGEUP,   vl::Key_PageUp,
    SDLK_PAGEDOWN, vl::Key_PageDown,
    SDLK_PAUSE,    vl::Key_Pause,
    SDLK_PRINTSCREEN, vl::Key_Print,
    SDLK_F1,       vl::Key_F1,
    SDLK_F2,       vl::Key_F2,
    SDLK_F3,       vl::Key_F3,
    SDLK_F4,       vl::Key_F4,
    SDLK_F5,       vl::Key_F5,
    SDLK_F6,       vl::Key_F6,
    SDLK_F7,       vl::Key_F7,
    SDLK_F8,       vl::Key_F8,
    SDLK_F9,       vl::Key_F9,
    SDLK_F10,      vl::Key_F10,
    SDLK_F11,      vl::Key_F11,
    SDLK_F12,      vl::Key_F12,
    0,0
  };
}
//-----------------------------------------------------------------------------
SDLWindow::SDLWindow()
{
}
//-----------------------------------------------------------------------------
SDLWindow::~SDLWindow()
{
    destroyWindow();
}
//-----------------------------------------------------------------------------
SDLWindow::SDLWindow( const vl::String& title, const vl::OpenGLContextFormat& info, int /*x*/, int /*y*/, int width, int height)
{
  initSDLWindow(title, info, width, height);
}
//-----------------------------------------------------------------------------
bool SDLWindow::initSDLWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int x, int y, int width, int height)
{
  destroyWindow();
  
  mSDLWindow = this;

  // init key translation map
  for (int i = 0; key_translation_vec[i]; i += 2)
    key_translation_map[ key_translation_vec[i] ] = (vl::EKey)key_translation_vec[i+1];

  // init SDL  
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    vl::Log::error( vl::Say("Unable to init SDL: %s\n") <<  SDL_GetError());
    return false;
  }

  SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   info.rgbaBits().r());
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, info.rgbaBits().g());
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  info.rgbaBits().b());
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, info.rgbaBits().a());

  SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE,   info.accumRGBABits().r());
  SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, info.accumRGBABits().g());
  SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE,  info.accumRGBABits().b());
  SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, info.accumRGBABits().a());

  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, info.depthBufferBits());
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, info.stencilBufferBits());

  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, info.doubleBuffer()?1:0);
  SDL_GL_SetAttribute(SDL_GL_STEREO, info.stereo());
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, info.multisample()?1:0);

  if (info.multisample())
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, info.multisampleSamples());

  switch( info.openGLProfile() )
  {
  case vl::GLP_Compatibility:
      if ( info.majVersion() >= 3 && info.minVersion() >= 2 ) {
	  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY );
	  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, info.majVersion() );
	  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, info.minVersion() );
	  vl::Log::debug( vl::Say("\n debug: OpenGLProfile vl::GLP_Compatibility minor version %n major version %n \n")  << info.minVersion() << info.majVersion() );
      }
      break;
  case vl::GLP_Core:
      if ( info.majVersion() >= 3 && info.minVersion() >= 2 ) {
	  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
	  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, info.majVersion() );
	  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, info.minVersion() );
	  vl::Log::debug( vl::Say("\n debug: OpenGLProfile vl::GLP_Core minor version %n  major version %n \n") << info.minVersion() << info.majVersion() );
      }
      break;
  case vl::GLP_Default:
      vl::Log::debug( vl::Say("\n debug: OpenGLProfile default vl::GLP_Default \n") ) ;
      // do nothing
      break;
  }
  
  if (SDL_GetNumDisplayModes(0) < 1)
  {
      vl::Log::error( vl::Say("\n  error: SDL_GetNumDisplayModes(0) failed: %s\n") << SDL_GetError() );
      exit(EXIT_FAILURE);
  }
  
  SDL_DisplayMode mode;
  if (SDL_GetCurrentDisplayMode(0, &mode) != 0)
  {
      vl::Log::error( vl::Say("\n  error: SDL_GetCurrentDisplayMode(0, &mode) failed, could not get display mode for video display %d : %s\n") << 0 << SDL_GetError() );
      exit(EXIT_FAILURE);
  }
  
  int bpp = SDL_BITSPERPIXEL(mode.format);
  Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
  if (info.fullscreen())
  {
      flags |= SDL_WINDOW_FULLSCREEN;
      mIsFullScreen = true;
  }
  
  width  = width  !=0 ? width  : mode.w;
  height = height !=0 ? height : mode.h;

  mWindow = SDL_CreateWindow(title.toStdString().c_str(),
			     (x < 0 ? SDL_WINDOWPOS_UNDEFINED: x),
			     (y < 0 ? SDL_WINDOWPOS_UNDEFINED: y),
			     width, height,
			     flags);
  if (mWindow == 0)
  {
    vl::Log::error( vl::Say("\n  error: SDL_CreateWindow(%n, %n, %n, %hn) failed: %s\n") << width << height << bpp << flags << SDL_GetError() );
    return false;
  }

  // Create an OpenGL context associated with the window. Every window has its own context.
  mSdlGlContext = SDL_GL_CreateContext(mWindow);
  if (!mSdlGlContext)
  {
      vl::Log::error( vl::Say("\n  error: SDL_GL_CreateContext(%n, %n, %n, %hn) failed: %s\n") << width << height << bpp << flags << SDL_GetError() );
      SDL_DestroyWindow(mWindow);
      return false;
  }

  // Window and context are ok
  mWinMap[ mWindow ] = this;
  mCurrentWidth = width;
  mCurrentHeight = width;

  // window size problem
  // TODO: this check is needed with SDL2?
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT,  viewport);
  VL_CHECK(viewport[0] == 0);
  VL_CHECK(viewport[1] == 0);
  int win_w, win_h;
  SDL_GetWindowSize(mWindow, &win_w, &win_h);
  if (viewport[2] != win_w || viewport[3] != win_h)
  {
    vl::Log::warning( vl::Say("\n  warning: OpenGL reported %nx%n as video size but SDL2 says %nx%n\n") << viewport[2] << viewport[3] << win_w << win_h );
    VL_TRAP();
  }

  // OpenGL extensions initialization
  initGLContext();
  if (SDL_GL_SetSwapInterval((info.vSync() == true ? 1: 0)) != 0)
  {
      vl::Log::warning( vl::Say("\n  warning: SDL_GL_SetSwapInterval(info.vSync()) failed, could not set vSync for the value %s : %s\n") << (info.vSync() == true ? "true": "false") << SDL_GetError() );
  }
  dispatchInitEvent();
  dispatchResizeEvent(width, height);
  
  #ifndef NDEBUG
    vl::Log::debug( vl::Say("\nSDL2 screen: %n x %n x %n %s\n") << win_w << win_h << bpp << (info.fullscreen() ? "fullscreen" : "windowed") );
  #endif


  #ifdef WIN32
    // file drag & drop support
    SDL_EventState(SDL_SYSWMEVENT, SDL_ENABLE);
    static SDL_SysWMinfo pInfo;
    SDL_VERSION(&pInfo.version);
    if (!SDL_GetWindowWMInfo(mWindow, &pInfo))
    {
	vl::Log::error( vl::Say("\n  error: SDL_GetWindowWMInfo(mWindow, &systemInfo) failed, couldn't get window information. SDL2 says %nx%n\n") << SDL_GetError() );
	exit(EXIT_FAILURE);
    }

    HWND hWnd = pInfo.window;
    DWORD ExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    ExStyle |= WS_EX_ACCEPTFILES;
    SetWindowLong(hWnd, GWL_EXSTYLE, ExStyle);
  #endif

  SDL_ShowCursor(SDL_ENABLE);

  // don't confine mouse to the window
  SDL_SetWindowGrab(mWindow, SDL_FALSE);
 
  // event cleaning
  SDL_PumpEvents();
  SDL_Event event;
  while ( SDL_PollEvent(&event) ) {}
  return true;
}
//-----------------------------------------------------------------------------
void SDLWindow::destroyWindow()
{
  if ( mWindow )
    {
	dispatchDestroyEvent();
	SDL_GL_DeleteContext(mSdlGlContext);
	SDL_DestroyWindow(mWindow);
	mWinMap.erase(mWindow);
	mWindow = NULL;
	mSDLWindow = NULL;
    }
}
//-----------------------------------------------------------------------------
void SDLWindow::makeCurrent()
{
    if ( mWindow )
    {
	SDL_GL_MakeCurrent(mWindow, mSdlGlContext);
    }
}

bool SDLWindow::setFullscreen(bool fs)
{
    if (mWindow)
    {
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
	    vl::Log::error(vl::Say("\n  error: SDL_GetDesktopDisplayMode failed: %s\n") << SDL_GetError());
	    return false;
	}
	
	// from windowed mode to fullscreen
	if (!mIsFullScreen && fs)	    
	{   vl::Log::debug("from windowed mode to fullscreen");
	    if (SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
	    {
		vl::Log::error( vl::Say("\n  error: SDL_SetWindowFullscreen(SDL_WINDOW_FULLSCREEN) failed: %s\n") << SDL_GetError() );
		return false;
	    }
	    mIsFullScreen = true;
	    dispatchResizeEvent(dm.w, dm.h);
	    return true;
	}

	// from fullscreen to windowed mode, this toggle to windowed mode even when fs == false or fs == true
	if (mIsFullScreen)
	{
	    vl::Log::debug("from fullscreen to windowed mode");
	    if (SDL_SetWindowFullscreen(mWindow, 0) != 0)
	    {
		vl::Log::error( vl::Say("\n  error: SDL_SetWindowFullscreen(0) failed: %s\n") << SDL_GetError() );
		return false;
	    }
	    mIsFullScreen = false;
	    dispatchResizeEvent(mCurrentWidth, mCurrentHeight);
	    return true;
	}
    }
    return false;
}

//-----------------------------------------------------------------------------
void SDLWindow::quitApplication()
{
  dispatchDestroyEvent();
  WinMapType windows = mWinMap;
  for( WinMapType::iterator it = windows.begin(); it != windows.end(); ++it ) {
      it->second->destroyWindow();
  }

  SDL_Quit();
}
//-----------------------------------------------------------------------------
void SDLWindow::update()
{
  mUpdateFlag = true;
}
//-----------------------------------------------------------------------------
void SDLWindow::setMouseVisible(bool visible)
{
    mMouseVisible = visible;
    SDL_ShowCursor(visible ? SDL_ENABLE : SDL_DISABLE);
}
//-----------------------------------------------------------------------------
void SDLWindow::setMousePosition(int x, int y)
{
    SDL_WarpMouseInWindow(mWindow, (Uint16)x, (Uint16)y);
}
//-----------------------------------------------------------------------------
void SDLWindow::setWindowTitle(const vl::String& title)
{
  SDL_SetWindowTitle(mWindow, title.toStdString().c_str());
}
//-----------------------------------------------------------------------------
void SDLWindow::swapBuffers()
{
  SDL_GL_SwapWindow(mWindow);
}
//-----------------------------------------------------------------------------
void SDLWindow::setSize(int w, int h)
{
    if (!mWindow || (w == 0 || h == 0))	return;
    
    SDL_SetWindowSize(mWindow, w, h);
    // TODO: check if dispatchResizeEvent is needed
    dispatchResizeEvent(w, h);
}
//-----------------------------------------------------------------------------
void SDLWindow::setPosition(int x, int y)
{
    if (!mWindow) return;
    
  #ifdef WIN32
	  static SDL_SysWMinfo pInfo;
	  SDL_VERSION(&pInfo.version);
	  if (!SDL_GetWindowWMInfo(mWindow, &pInfo))
	  {
	      vl::Log::print( vl::Say("\n  error: SDL_GetWindowWMInfo(mWindow, &systemInfo) failed, couldn't get window information. SDL2 says %nx%n\n") << SDL_GetError() );
	      exit(1);
	  }
	  HWND hWnd = pInfo.window;
	  SetWindowPos(hWnd, 0, x, y, 0, 0, SWP_NOSIZE );
  #else
	  SDL_SetWindowPosition(mWindow, x, y);
  #endif
}

//-----------------------------------------------------------------------------
vl::ivec2 SDLWindow::position() const
{
    int x = 0, y = 0;
    if (mWindow)
	SDL_GetWindowPosition(mWindow, &x, &y);
    
    return vl::ivec2( x, y );
}
//-----------------------------------------------------------------------------
vl::ivec2 SDLWindow::size() const
{
    int w = 0, h = 0;
    if (mWindow)
	SDL_GetWindowSize(mWindow, &w, &h);
    
    return vl::ivec2( w, h );
}
//-----------------------------------------------------------------------------
void SDLWindow::translateEvent( SDL_Event * ev )
{
  vl::EKey key = vl::Key_None;
  unsigned short unicode = 0;
  if ( ev && (ev->type == SDL_KEYDOWN || ev->type == SDL_KEYUP) )
  {
    if ( key_translation_map.find(ev->key.keysym.sym) != key_translation_map.end() )
      key = key_translation_map[ ev->key.keysym.sym ];
    else
      key = vl::Key_Unknown;

    unicode = ev->key.keysym.sym; // sym for now can be stored into unsigned short but in the future...
  }

  //// save it here cause the listeners could modify it via update() function
  //bool update = mUpdateFlag;
  //// schedule possible update, note that listeners can override this calling the update() function
  //mUpdateFlag = continuousUpdate();

  //if (update)
  //  dispatchUpdateEvent();

  switch (ev->type)
  {
    case SDL_KEYDOWN:
    {
      switch(key)
      {
      default: break;

      case vl::Key_LeftCtrl:
      case vl::Key_RightCtrl:
	dispatchKeyPressEvent(unicode, vl::Key_Ctrl);
      break;

      case vl::Key_LeftShift:
      case vl::Key_RightShift:
	dispatchKeyPressEvent(unicode, vl::Key_Shift);
      break;

      case vl::Key_LeftAlt:
      case vl::Key_RightAlt:
	dispatchKeyPressEvent(unicode, vl::Key_Alt);
      break;
      }
      dispatchKeyPressEvent(unicode, key);
    }
    break;
    case SDL_KEYUP:
    {
      switch(key)
      {
      default: break;

      case vl::Key_LeftCtrl:
      case vl::Key_RightCtrl:
	dispatchKeyReleaseEvent(unicode, vl::Key_Ctrl);
      break;

      case vl::Key_LeftShift:
      case vl::Key_RightShift:
	dispatchKeyReleaseEvent(unicode, vl::Key_Shift);
      break;

      case vl::Key_LeftAlt:
      case vl::Key_RightAlt:
	dispatchKeyReleaseEvent(unicode, vl::Key_Alt);
      break;
      }
      dispatchKeyReleaseEvent(unicode, key);
    }
    break;
    case SDL_MOUSEWHEEL:
    {
      if (ev->wheel.y > 0) // scroll up
	  dispatchMouseWheelEvent(1);
      else
      if (ev->wheel.y < 0) // scroll down
	  dispatchMouseWheelEvent(-1);
    }
    break;
    case SDL_MOUSEBUTTONDOWN:
    {
      if (ev->button.button == SDL_BUTTON_LEFT)
	dispatchMouseDownEvent(vl::LeftButton, ev->button.x, ev->button.y);
      else
      if (ev->button.button == SDL_BUTTON_RIGHT)
	dispatchMouseDownEvent(vl::RightButton, ev->button.x, ev->button.y);
      else
      if (ev->button.button == SDL_BUTTON_MIDDLE)
	dispatchMouseDownEvent(vl::MiddleButton, ev->button.x, ev->button.y);
    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
      if (ev->button.button == SDL_BUTTON_LEFT)
	dispatchMouseUpEvent(vl::LeftButton, ev->button.x, ev->button.y);
      else
      if (ev->button.button == SDL_BUTTON_RIGHT)
	dispatchMouseUpEvent(vl::RightButton, ev->button.x, ev->button.y);
      else
      if (ev->button.button == SDL_BUTTON_MIDDLE)
	dispatchMouseUpEvent(vl::MiddleButton, ev->button.x, ev->button.y);
    }
    break;
    case SDL_MOUSEMOTION:
    {
      dispatchMouseMoveEvent(ev->motion.x, ev->motion.y);
    }
    break;
    case SDL_WINDOWEVENT:
    {
      if (ev->window.event == SDL_WINDOWEVENT_RESIZED)
      {
	int width = ev->window.data1;
	int height = ev->window.data2;
	SDL_SetWindowSize(mWindow, width, height);
	mCurrentWidth = width;
	mCurrentHeight = height;
	dispatchResizeEvent(width, height);
      }
    }
    break;
    case SDL_SYSWMEVENT:
    {
      #ifdef WIN32
      if (ev->syswm.msg->msg == WM_DROPFILES)
      {
	HDROP hDrop = (HDROP) ev->syswm.msg->wParam;
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
	dispatchFileDroppedEvent(files);
      }
      #endif
    }
    break;
    case SDL_QUIT:
    {
      quitApplication();
    }
    break;
  }
}
//-----------------------------------------------------------------------------
void vlSDL2::messageLoop()
{
  SDL_Event ev;
  while (mSDLWindow)
  {
    if ( SDL_PollEvent(&ev) ) {
      mSDLWindow->translateEvent(&ev);
      continue;
    }
    
    if ( mUpdateFlag || mSDLWindow->continuousUpdate() )
    {
      mSDLWindow->dispatchUpdateEvent();
      mUpdateFlag = false;
      continue;
    }
    // rest for 10ms if there are not events to process, and we don't need to repaint
    SDL_Delay(10);
  }

  // if mSDLWindow is null, quit
  SDL_Quit();
}
