/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2017, Michele Bosi                                             */
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

#ifndef GLFW_Window_INCLUDE_ONCE
#define GLFW_Window_INCLUDE_ONCE

#include <vlGLFW/link_config.hpp>
#include <vlCore/Vector4.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <GLFW/glfw3.h>
#include <map>

namespace vlGLFW
{
  //-----------------------------------------------------------------------------
  // GLFWWindow
  //-----------------------------------------------------------------------------

  /**
   * The GLFWWindow class implements an OpenGLContext using the GLFW API.
  */
  class VLGLFW_EXPORT GLFWWindow: public vl::OpenGLContext
  {
  public:
    GLFWWindow();

    //! Initializes and shows a GLFW window.
    //! @param[in] title The title of the window
    //! @param[in] info The options for the new OpenGL context
    //! @param[in] width Width of the window. If set to 0 and full screen mode is selected it will default to the monitor's size
    //! @param[in] height Height of the window. If set to 0 and full screen mode is selected it will default to the monitor's size
    //! @param[in] share If not NULL the OpenGL context will be shared with the given GLFWwindow
    GLFWWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int width, int height, GLFWwindow* share = NULL);

    //! Initializes and shows a GLFW window.
    //! @param[in] title The title of the window
    //! @param[in] info The options for the new OpenGL context
    //! @param[in] width Width of the window. If set to 0 and full screen mode is selected it will default to the monitor's size
    //! @param[in] height Height of the window. If set to 0 and full screen mode is selected it will default to the monitor's size
    //! @param[in] share If not NULL the OpenGL context will be shared with the given GLFWwindow
    bool initGLFWWindow(const vl::String& title, const vl::OpenGLContextFormat& info, int width, int height, GLFWwindow* share = NULL);

    ~GLFWWindow();

    //! Attempts to quits the program by destroying all windows
    void quitApplication() {
      eraseAllEventListeners();
      WinMapType windows = mWinMap;
      for( WinMapType::iterator it = windows.begin(); it != windows.end(); ++it ) {
        it->second->destroyWindow();
      }
    }

    void makeCurrent();

    void update();

    void swapBuffers();

    bool setFullscreen(bool fs);

    void setWindowTitle(const vl::String& title);

    void setPosition(int x, int y) ;

    void setSize(int w, int h) ;

    vl::ivec2 position() const;

    vl::ivec2 size() const;

    void show();

    void hide();

    void getFocus();

    void setMouseVisible(bool visible);

    void setMousePosition(int x, int y);

    // ---

    const GLFWwindow* handle() const { return mHandle; }

    GLFWwindow* handle() { return mHandle; }

    void destroyWindow();

  protected:
    static void glfw_size_callback(GLFWwindow* window, int w, int h);

    static void glfw_pos_callback(GLFWwindow* window, int x, int y);

    static void glfw_refresh_callback(GLFWwindow* window);

    static void glfw_close_callback(GLFWwindow* window);

    static void glfw_focus_callback(GLFWwindow* window, int);

    static void glfw_iconify_callback(GLFWwindow* window, int);

    // ---

    static vl::EKey translateKey(int key, int scancode, int mods);

    static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void glfw_unicode_callback(GLFWwindow *, unsigned int);

    static void glfw_cursor_enter_callback(GLFWwindow *, int);

    static void glfw_cursor_pos_callback(GLFWwindow *, double, double);

    static void glfw_mouse_button_callback(GLFWwindow *, int, int, int);

    static void glfw_scroll_callback(GLFWwindow *, double, double);

    static void glfw_drop_callback(GLFWwindow *, int, const char**);

  protected:
    typedef std::map< ::GLFWwindow*, vlGLFW::GLFWWindow* > WinMapType;
    static WinMapType mWinMap;
    unsigned short mUnicodeChar;
    GLFWwindow* mHandle;
  };
}

#endif
