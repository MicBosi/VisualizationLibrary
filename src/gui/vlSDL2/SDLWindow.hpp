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

#ifndef SDL2Adapter_INCLUDE_ONCE
#define SDL2Adapter_INCLUDE_ONCE

#include "link_config.hpp"
#include <vlGraphics/OpenGLContext.hpp>
#include <vlCore/String.hpp>
#include <vlCore/Vector4.hpp>
#include <SDL2/SDL.h>
#include <map>

namespace vlut
{
  class Applet;
}

namespace vlSDL2
{
  VLSDL2_EXPORT void messageLoop();

//-----------------------------------------------------------------------------
// SDLWindow
//-----------------------------------------------------------------------------
  /**
   * The SDLWindow class implements an OpenGLContext using the SDL2 API.
   */
  class VLSDL2_EXPORT SDLWindow: public vl::OpenGLContext
  {

  public:
      typedef std::map< ::SDL_Window*, vlSDL2::SDLWindow* > WinMapType;
      
  public:
    SDLWindow();
    SDLWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int x=0, int y=0, int width=640, int height=480);

    //! Initializes and shows a SDL2 window.
    //! @param[in] title The title of the window
    //! @param[in] info The options for the new OpenGL context
    //! @param[in] x The x position
    //! @param[in] y The y position
    //! @param[in] width Width of the window. If set to 0 and full screen mode is selected it will default to the monitor's size
    //! @param[in] height Height of the window. If set to 0 and full screen mode is selected it will default to the monitor's size
    bool initSDLWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int x=0, int y=0, int width=640, int height=480);

    ~SDLWindow();

    void setSize(int w, int h);
    void setPosition(int x, int y);
    vl::ivec2 position() const;
    vl::ivec2 size() const;
    
    virtual void swapBuffers();

    void translateEvent(SDL_Event * ev);

    //! Quits the event loop
    void quitApplication();
    
    void setWindowTitle(const vl::String&);

    void setMouseVisible(bool visible);

    void setMousePosition(int x, int y);
    
    bool setFullscreen(bool fs);
    
    void destroyWindow();
    
    void update();

    void makeCurrent();

    SDL_Window* sdlWindow() { return mWindow; }
    const SDL_Window* sdlWindow() const { return mWindow; }

    static const WinMapType& windows() { return mWinMap; }

  protected:
    static WinMapType mWinMap; // store the reference to all window
    SDL_Window* mWindow; // the current window
    SDL_GLContext mSdlGlContext; // the OpenGL context associated with window
    int mCurrentWidth;
    int mCurrentHeight;
    bool mIsFullScreen;
  };
}

#endif
