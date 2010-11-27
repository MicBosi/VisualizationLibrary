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

#ifndef EventListener_INCLUDE_ONCE
#define EventListener_INCLUDE_ONCE

#include <vl/String.hpp>

namespace vl
{
  class OpenGLContext;
  //-----------------------------------------------------------------------------
  // UIEventListener
  //-----------------------------------------------------------------------------
  /**
   * The UIEventListener class listens to the events emitted by an OpenGLContext.
   *
   * \remarks
   * - Qt amongst others does not distinguish between left and right \a alt, \a shift and \a control keys, which means that you
   *   won't receive messages like Key_LeftCtrl, Key_RightCtrl, Key_LeftAlt, Key_RightAlt, Key_LeftShift and Key_RightShift
   *   but only the more general Key_Ctrl, Key_Alt and Key_Shift messages.
   * - SDL supports Unicode only partially at the moment, which means that you will receive Unicode codes only for key press events and not
   *   for release events.
   * - keyPressed() returns the correct state only for the keys whose messages have been received by the OpenGLContext. This means that for
   *   example if a key was pressed when the BanckendAdaper did not have the keyboard focus, the function keyPressed() will wrongly
   *   report a \a released state for that key. If a key release event is not sent to the OpenGLContext because generated when the
   *   OpenGLContext did not have the keyboard focus, the function keyPressed() will wrongly report a \a pressed state for that key.
  */
  class UIEventListener: public Object
  {
  friend class OpenGLContext;
  public:
    virtual const char* className() { return "UIEventListener"; }

    UIEventListener(): mOpenGLContext(NULL), mEnabled(true) 
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    virtual ~UIEventListener() {}

    virtual void initEvent() {}

    virtual void mouseMoveEvent(int /*x*/, int /*y*/) {}
    virtual void mouseUpEvent(EMouseButton /*button*/, int /*x*/, int /*y*/) {}
    virtual void mouseDownEvent(EMouseButton /*button*/, int /*x*/, int /*y*/) {}
    virtual void mouseWheelEvent(int /*n*/) {}
    virtual void keyPressEvent(unsigned short /*unicode_ch*/, EKey /*key*/) {}
    virtual void keyReleaseEvent(unsigned short /*unicode_ch*/, EKey /*key*/) {}
    virtual void resizeEvent(int /*x*/, int /*y*/) {}
    virtual void destroyEvent() {}
    virtual void runEvent() {}
    virtual void visibilityEvent(bool /*visible*/) {}
    virtual void fileDroppedEvent(const std::vector<String>& /*files*/) {}

    virtual void openglContextBoundEvent(OpenGLContext*) {}

    //! Returns the OpenGL context to which this UIEventListener is bound or NULL if no context is bound.
    OpenGLContext* openglContext();

    //! Enables or disables a UIEventListener
    //! \note When an UIEventListener is enabled its prepareToReconnect() method is called so that 
    //! it can be appropriately setup before it starts to receive events.
    virtual void setEnabled(bool enabled) { mEnabled = enabled; if (enabled) prepareToReconnect(); }

    //! Reimplement this in order to react to a setEnabled(true), see setEnabled() for more information.
    virtual void prepareToReconnect() {}

    bool isEnabled() const { return mEnabled; }

  protected:
    OpenGLContext* mOpenGLContext;
    bool mEnabled;
  };
}

#endif
