/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi, Brent Fulgham                              */
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

#include "vlCocoa/CGLContext.hpp"
#include "vl/VisualizationLibrary.hpp"
#include "vl/Log.hpp"
#include "vl/Say.hpp"
#include <algorithm>
#include <map>

using namespace vlCGL;
using namespace vl;

namespace
{
   // From NSEvent.h
   enum {
      NSAlphaShiftKeyMask = 1 << 16,
      NSShiftKeyMask      = 1 << 17,
      NSControlKeyMask    = 1 << 18,
      NSAlternateKeyMask  = 1 << 19,
      NSCommandKeyMask    = 1 << 20,
      NSNumericPadKeyMask = 1 << 21,
      NSHelpKeyMask       = 1 << 22,
      NSFunctionKeyMask   = 1 << 23,
   };   

  std::map<unsigned short, vl::EKey> key_translation_map;

  int key_translation_vec[] =
  {
    '0', vl::Key_0,
    '1', vl::Key_1,
    '2', vl::Key_2,
    '3', vl::Key_3,
    '4', vl::Key_4,
    '5', vl::Key_5,
    '6', vl::Key_6,
    '7', vl::Key_7,
    '8', vl::Key_8,
    '9', vl::Key_9,

    'a', vl::Key_A,
    'b', vl::Key_B,
    'c', vl::Key_C,
    'd', vl::Key_D,
    'e', vl::Key_E,
    'f', vl::Key_F,
    'g', vl::Key_G,
    'h', vl::Key_H,
    'i', vl::Key_I,
    'j', vl::Key_J,
    'k', vl::Key_K,
    'l', vl::Key_L,
    'm', vl::Key_M,
    'n', vl::Key_N,
    'o', vl::Key_O,
    'p', vl::Key_P,
    'q', vl::Key_Q,
    'r', vl::Key_R,
    's', vl::Key_S,
    't', vl::Key_T,
    'u', vl::Key_U,
    'v', vl::Key_V,
    'w', vl::Key_W,
    'x', vl::Key_X,
    'y', vl::Key_Y,
    'z', vl::Key_Z,

    'A', vl::Key_A,
    'B', vl::Key_B,
    'C', vl::Key_C,
    'D', vl::Key_D,
    'E', vl::Key_E,
    'F', vl::Key_F,
    'G', vl::Key_G,
    'H', vl::Key_H,
    'I', vl::Key_I,
    'J', vl::Key_J,
    'K', vl::Key_K,
    'L', vl::Key_L,
    'M', vl::Key_M,
    'N', vl::Key_N,
    'O', vl::Key_O,
    'P', vl::Key_P,
    'Q', vl::Key_Q,
    'R', vl::Key_R,
    'S', vl::Key_S,
    'T', vl::Key_T,
    'U', vl::Key_U,
    'V', vl::Key_V,
    'W', vl::Key_W,
    'X', vl::Key_X,
    'Y', vl::Key_Y,
    'Z', vl::Key_Z,

    0x03, vl::Key_Return,
    0x0D, vl::Key_Return,
    0x7F, vl::Key_BackSpace,
    ' ',  vl::Key_Space,
    0x09, vl::Key_Tab,
    //SDLK_CLEAR,        vl::Key_Clear,
    0x1B, vl::Key_Escape,
    '!', vl::Key_Exclam,
    '"', vl::Key_QuoteDbl,
    '#', vl::Key_Hash,
    '$', vl::Key_Dollar,
    '&', vl::Key_Ampersand,
    '\'', vl::Key_Quote,
    '(', vl::Key_LeftParen,
    ')', vl::Key_RightParen,
    '*', vl::Key_Asterisk,
    '+', vl::Key_Plus,
    ',', vl::Key_Comma,
    '-', vl::Key_Minus,
    '.', vl::Key_Period,
    '\\', vl::Key_Slash,
    ':', vl::Key_Colon,
    ';', vl::Key_Semicolon,
    '<', vl::Key_Less,
    '=', vl::Key_Equal,
    '>', vl::Key_Greater,
    '?', vl::Key_Question,
    '@', vl::Key_At,
    '[', vl::Key_LeftBracket,
    '/', vl::Key_BackSlash,
    ']', vl::Key_RightBracket,
    '^', vl::Key_Caret,
    '_', vl::Key_Underscore,
    //SDLK_BACKQUOTE,    vl::Key_QuoteLeft,

    // non unicode
    0xF702, vl::Key_Left,
    0xF703, vl::Key_Right,
    0xF700, vl::Key_Up,
    0xF701, vl::Key_Down,
    //SDLK_LCTRL,    vl::Key_LeftCtrl,
    //SDLK_RCTRL,    vl::Key_RightCtrl,
    //SDLK_LSHIFT,   vl::Key_LeftShift,
    //SDLK_RSHIFT,   vl::Key_RightShift,
    //SDLK_LALT,     vl::Key_LeftAlt,
    //SDLK_RALT,     vl::Key_RightAlt,
    0xF727, vl::Key_Insert,
    0xF728, vl::Key_Delete,
    0xF729, vl::Key_Home,
   // 0xF72A, vl::Key_Begin,
    0xF72B, vl::Key_End,
    0xF72C, vl::Key_PageUp,
    0xF72D, vl::Key_PageDown,
    0xF730, vl::Key_Pause,
    0xF72E, vl::Key_Print,
    0xF704, vl::Key_F1,
    0xF705, vl::Key_F2,
    0xF706, vl::Key_F3,
    0xF707, vl::Key_F4,
    0xF708, vl::Key_F5,
    0xF709, vl::Key_F6,
    0xF70A, vl::Key_F7,
    0xF70B, vl::Key_F8,
    0xF70C, vl::Key_F9,
    0xF70D, vl::Key_F10,
    0xF70E, vl::Key_F11,
    0xF70F, vl::Key_F12,

    0,0
  };
}

//-----------------------------------------------------------------------------
CGLContext::~CGLContext()
{
  destroy();
}

//-----------------------------------------------------------------------------
void CGLContext::destroy()
{
  dispatchDestroyEvent();
}

//-----------------------------------------------------------------------------
CGLContext::CGLContext()
{
}

//-----------------------------------------------------------------------------
CGLContext::CGLContext(
                     const vl::String& title, 
                     const vl::OpenGLContextFormat& info,
                     CGLContextObj context,
                     int x, 
                     int y, 
                     int width, 
                     int height)
      :mGLCtx(context)
{
  initCGLContext(title, context, info, width, height);
}

//-----------------------------------------------------------------------------
bool CGLContext::initCGLContext(const vl::String& title, CGLContextObj& context, const vl::OpenGLContextFormat& info, int x, int y, int width, int height)
{
  CGLSetCurrentContext (mGLCtx);

  mGLCtx = context;
   
  setOpenGLContextInfo(info);
   
  mRenderTarget->setWidth(width);
  mRenderTarget->setHeight(height);
   
  // init key translation map
  for(int i=0; key_translation_vec[i]; i+=2)
    key_translation_map[ key_translation_vec[i] ] = (vl::EKey)key_translation_vec[i+1];
   
  // OpenGL extensions initialization
  initGLContext(true);

  dispatchInitEvent();
  dispatchResizeEvent(width, height);

  #ifndef NDEBUG
    //vl::Log::print( vl::Say("CGL screen: %n x %n x %n %s\n") << mScreen->w << mScreen->h << mScreen->format->BitsPerPixel << (info.fullscreen() ? "fullscreen" : "windowed") );
  #endif

  return true;
}

void CGLContext::swapBuffers()
{
   CGLLockContext (mGLCtx);
   CGLFlushDrawable (mGLCtx);
   CGLUnlockContext (mGLCtx);
}

void CGLContext::makeCurrent()
{
   CGLLockContext (mGLCtx);
   CGLSetCurrentContext (mGLCtx);
   CGLUnlockContext (mGLCtx);
}

//-----------------------------------------------------------------------------
void CGLContext::translateKeyPress(unsigned short keypress, unsigned int modifiers)
{
   vl::EKey key = vl::Key_None;

   if( key_translation_map.find(keypress) != key_translation_map.end() )
      key = key_translation_map[keypress];
   else
      key = vl::Key_Unknown;
   
   if (modifiers & NSControlKeyMask)
      dispatchKeyPressEvent(keypress, vl::Key_Ctrl);
   else if (modifiers & NSShiftKeyMask)
      dispatchKeyPressEvent(keypress, vl::Key_Shift);
   else if (modifiers & NSAlternateKeyMask)
      dispatchKeyPressEvent(keypress, vl::Key_Alt);
   else
      dispatchKeyPressEvent(keypress, key);
}

//-----------------------------------------------------------------------------
void CGLContext::translateKeyRelease(unsigned short keypress, unsigned int modifiers)
{
   vl::EKey key = vl::Key_None;
   
   if( key_translation_map.find(keypress) != key_translation_map.end() )
      key = key_translation_map[keypress];
   else
      key = vl::Key_Unknown;
   
   if (modifiers & NSControlKeyMask)
      dispatchKeyReleaseEvent(keypress, vl::Key_Ctrl);
   else if (modifiers & NSShiftKeyMask)
      dispatchKeyReleaseEvent(keypress, vl::Key_Shift);
   else if (modifiers & NSAlternateKeyMask)
      dispatchKeyReleaseEvent(keypress, vl::Key_Alt);
   else
      dispatchKeyReleaseEvent(keypress, key);
}

//-----------------------------------------------------------------------------
