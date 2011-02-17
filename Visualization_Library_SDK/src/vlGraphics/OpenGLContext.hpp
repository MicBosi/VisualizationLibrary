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

#ifndef OpenGLContext_INCLUDE_ONCE
#define OpenGLContext_INCLUDE_ONCE

#include <vlCore/Object.hpp>
#include <vlGraphics/UIEventListener.hpp>
#include <vlGraphics/FramebufferObject.hpp> // RenderTarget and FBORenderTarget
#include <vlGraphics/RenderState.hpp>
#include <vector>
#include <set>

namespace vl
{
  class EnableSet;
  class RenderStateSet;
  class UniformSet;
  class IVertexAttribSet;
  class ArrayAbstract;

  //-----------------------------------------------------------------------------
  // OpenGLContextFormat
  //-----------------------------------------------------------------------------
  //! The OpenGLContextFormat class encapsulates the settings of an OpenGL rendering context.
  class OpenGLContextFormat
  {
  public:
    OpenGLContextFormat():
      mRGBABits(ivec4(8,8,8,8)),
      mAccumRGBABits(ivec4(0,0,0,0)),
      mHasDoubleBuffer(true),
      mZBufferBits(24),
      mStencilBufferBits(8),
      mHasMultisample(false),
      mMultisampleSamples(16),
      mStereo(false),
      mFullscreen(false),
      mVSync(false) 
    {}
    virtual const char* className() { return "OpenGLContextFormat"; }

    void setRGBABits(int r, int g, int b, int a) { mRGBABits = ivec4(r,g,b,a); }
    void setAccumRGBABits(int r, int g, int b, int a) { mAccumRGBABits = ivec4(r,g,b,a); }
    void setDoubleBuffer(bool double_buffer_on) { mHasDoubleBuffer = double_buffer_on; }
    void setDepthBufferBits(int bits) { mZBufferBits = bits; }
    void setStencilBufferBits(int bits) { mStencilBufferBits = bits; }
    void setMultisample(bool multisample_on) { mHasMultisample = multisample_on; }
    void setMultisampleSamples(int samples) { mMultisampleSamples = samples; }
    void setStereo(bool stereo_on) { mStereo = stereo_on; }
    void setFullscreen(bool fullscreent) { mFullscreen = fullscreent; }
    void setVSync(bool vsync_on) { mVSync = vsync_on; }

    const ivec4& rgbaBits() const { return mRGBABits; }
    const ivec4& accumRGBABits() const { return mAccumRGBABits; }
    bool doubleBuffer() const { return mHasDoubleBuffer; }
    int depthBufferBits() const { return mZBufferBits; }
    int stencilBufferBits() const { return mStencilBufferBits; }
    bool multisample() const { return mHasMultisample; }
    int multisampleSamples() const { return mMultisampleSamples; }
    bool stereo() const { return mStereo; }
    bool fullscreen() const { return mFullscreen; }
    bool vSync() const { return mVSync; }

    //! Returns rgbaBits().r() + rgbaBits().g() + rgbaBits().b() + rgbaBits().a()
    int bitsPerPixel() const { return rgbaBits().r() + rgbaBits().g() + rgbaBits().b() + rgbaBits().a(); }

  protected:
    ivec4 mRGBABits;
    ivec4 mAccumRGBABits;
    bool mHasDoubleBuffer;
    int mZBufferBits;
    int mStencilBufferBits;
    bool mHasMultisample;
    int mMultisampleSamples;
    bool mStereo;
    bool mFullscreen;
    bool mVSync;
  };
  //-----------------------------------------------------------------------------
  // OpenGLContext
  //-----------------------------------------------------------------------------
  //! Represents an OpenGL context, possibly a widget or a pbuffer, which can also respond to keyboard, mouse or system events.
  //!
  //! OpenGLContext is an abstract class that wraps a minimal common subset of GUI APIs like Win32, Qt, wxWidgets, SDL, GLUT, etc. \n
  //! In order to respond to the events generated by the OpenGLContext you must subclass an UIEventListener and bind it to the OpenGLContext
  //! using the functions addEventListener(ref<UIEventListener>) and removeEventListener(ref<UIEventListener>).
  //! 
  //! \par OpenGLContext Custom Implementation
  //! - Key_Alt/Ctrl/Shift events must always be notified before Key_Left/Right-Alt/Ctrl/Shift events.
  //! - Always update the mKeyboard structure appropriately especially with respect to Key_[Left/Right]-Alt/Ctrl/Shift events.
  //! - When cycling through EventListeners to dispatch the events you must do it on a temporary copy of mEventListeners so that
  //!   the EventListeners can safely add/remove themselves or other EventListeners to the OpenGLContext itself. */
  class OpenGLContext: public Object
  {
  public:
    virtual const char* className() { return "OpenGLContext"; }

    //! Constructor.
    OpenGLContext(int w=0, int h=0);

    //! Destructor.
    //! Dispatches also destroyEvent() to its event listeners.
    ~OpenGLContext() 
    { 
      eraseAllEventListeners();
      mRenderTarget->mOpenGLContext = NULL;
    }

    //! Swaps the back and front buffers to present the last rendering.
    virtual void swapBuffers() = 0;

    //! Sets the OpenGL context as current for the calling thread.
    virtual void makeCurrent() = 0;

    //! Initializes the supported OpenGL extensions.
    void initGLContext(bool log=true);

    //! Logs some information about the OpenGL context
    void logOpenGLInfo();

    //! Returns true if the given extension is supported.
    //! \note This is a relatively slow function, don't use it inside loops and similar.
    bool isExtensionSupported(const char* ext_name);

    //! Returns the address of an OpenGL extension function
    void* getProcAddress(const char* function_name);

    //! The render target representing this OpenGL context.
    //! The returned RenderTarget's dimensions will be automatically updated to the OpenGLContext's dimensions.
    RenderTarget* renderTarget() { return mRenderTarget.get(); }

    //! The render target representing this OpenGL context.
    //! The returned RenderTarget's dimensions will be automatically updated to the OpenGLContext's dimensions.
    const RenderTarget* renderTarget() const { return mRenderTarget.get(); }

    //! Equivalent to \p "createFBORenderTarget(0,0);".
    ref<FBORenderTarget> createFBORenderTarget() { return createFBORenderTarget(0,0); }

    //! Creates a new FBORenderTarget (framebuffer object RenderTarget).
    //! \note A framebuffer object always belongs to an OpenGL context and in order to render on it the appropriate OpenGL context must be active.
    ref<FBORenderTarget> createFBORenderTarget(int width, int height);

    //! Destroys the specified FBORenderTarget.
    void destroyFBORenderTarget(FBORenderTarget* fbort);

    //! Removes all FBORenderTargets belonging to an OpenGLContext.
    void destroyAllFBORenderTargets();

    //! Asks to the windowing system that is managing the OpenGLContext to quit the application.
    virtual void quitApplication() {}

    //! If the OpenGLContext is a widget this function requests a redraw and generates an updateEvent().
    virtual void update() = 0;

    //! If the OpenGL context is a top window this function sets its title.
    virtual void setWindowTitle(const String&) {}

    //! If the OpenGL context is a widget this function requests a maximization to fullscreen.
    virtual bool setFullscreen(bool) { mFullscreen = false; return false; }

    //! If the OpenGL context is a widget this function returns whether it has been maximized to fullscreen.
    virtual bool fullscreen() const { return mFullscreen; }

    //! If the OpenGL context is a widget this function makes it visible to the user.
    virtual void show() {}

    //! If the OpenGL context is a widget this function makes it invisible to the user.
    virtual void hide() {}

    //! If the OpenGL context is a widget this function sets its position.
    virtual void setPosition(int /*x*/, int /*y*/) {}

    //! If the OpenGL context is a widget this function returns its position.
    virtual ivec2 position() const { return ivec2(); }

    //! If the OpenGL context is a widget this function sets its size.
    virtual void setSize(int /*w*/, int /*h*/) {}

    //! Returns the width in pixels of an OpenGLContext.
    int width() const { return mRenderTarget->width(); }
    
    //! Returns the height in pixels of an OpenGLContext.
    int height() const { return mRenderTarget->height(); }

    //! If the OpenGL context is a widget this function sets whether the mouse is visible over it or not.
    virtual void setMouseVisible(bool) { mMouseVisible=false; }

    //! If the OpenGL context is a widget this function returns whether the mouse is visible over it or not.
    virtual bool mouseVisible() const { return mMouseVisible; }

    //! If the OpenGL context is a widget this function sets the mouse position.
    virtual void setMousePosition(int /*x*/, int /*y*/) {}

    //! If the OpenGL context is a widget this function requests the mouse focus on it.
    virtual void getFocus() {}

    //! If the OpenGL context is a widget this function enabled/disables double buffer swapping to the monitor's vertical synch.
    void setVSyncEnabled(bool enable);

    //! If the OpenGL context is a widget this function returns whether vsync is enabled or not.
    bool vsyncEnabled() const;

    //! If the OpenGL context is a widget this function sets whether its area is continuously updated at each frame.
    virtual void setContinuousUpdate(bool continuous) { mContinuousUpdate = continuous; }

    //! If the OpenGL context is a widget this function returns whether its area is continuously updated at each frame.
    bool continuousUpdate() const { return mContinuousUpdate; }

    //! Adds an UIEventListener to be notified of OpenGLContext related events.
    //! This method triggers immediately an UIEventListener::addedListenerEvent() and if the OpenGLContext is initialized also an UIEventListener::initEvent().
    //! \note An \p UIEventListener can be associated only to one OpenGLContext at a time.
    void addEventListener(UIEventListener* el);
    
    //! Removes an UIEventListener
    void removeEventListener(UIEventListener* el);
    
    //! Removes all UIEventListener previously registered
    void eraseAllEventListeners();
    
    //! The currently UIEventListener registered to be notified of OpenGLContext related events.
    const std::vector< ref<UIEventListener> >& eventListeners() const { return mEventListeners; }
    
    //! Returns the \p i-th UIEventListener registered to an OpenGLContext.
    UIEventListener* eventListener(int i) const { return mEventListeners[i].get(); }
    
    //! Returns the number of UIEventListener registered to an OpenGLContext.
    int eventListenerCount() const { return (int)mEventListeners.size(); }

    //! Returns an OpenGLContextFormat structure describing an OpenGLContext.
    const OpenGLContextFormat& openglContextInfo() const { return mGLContextInfo; }
    
    //! Sets the OpenGLContextFormat associated to an OpenGLContext.
    void setOpenGLContextInfo(const OpenGLContextFormat& info) { mGLContextInfo = info; }

    //! Requests not to dispatch the next mouse move event.
    void ignoreNextMouseMoveEvent() { mIgnoreNextMouseMoveEvent = true; }

    //! Dispatches the UIEventListener::resizeEvent() notification to the subscribed UIEventListener objects.
    //! Call this function at the beginning if you reimplement it
    void dispatchResizeEvent(int w, int h) 
    {
      makeCurrent();
      mRenderTarget->setWidth(w);
      mRenderTarget->setHeight(h);

      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->resizeEvent( w, h );
    }

    //! Dispatches the UIEventListener::mouseMoveEvent() notification to the subscribed UIEventListener objects.
    void dispatchMouseMoveEvent(int x, int y)
    {
      makeCurrent();
      if (mIgnoreNextMouseMoveEvent)
        mIgnoreNextMouseMoveEvent = false;
      else
      {
        std::vector< ref<UIEventListener> > temp_clients = eventListeners();
        for( unsigned i=0; i<temp_clients.size(); ++i )
          if ( temp_clients[i]->isEnabled() )
            temp_clients[i]->mouseMoveEvent(x, y);
      }
    }
    
    //! Dispatches the UIEventListener::mouseUpEvent() notification to the subscribed UIEventListener objects.
    void dispatchMouseUpEvent(EMouseButton button, int x, int y) 
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->mouseUpEvent(button, x, y);
    }
    
    //! Dispatches the UIEventListener::mouseDownEvent() notification to the subscribed UIEventListener objects.
    void dispatchMouseDownEvent(EMouseButton button, int x, int y) 
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->mouseDownEvent(button, x, y);
    }
    
    //! Dispatches the UIEventListener::mouseWheelEvent() notification to the subscribed UIEventListener objects.
    void dispatchMouseWheelEvent(int n) 
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->mouseWheelEvent(n);
    }
    
    //! Dispatches the UIEventListener::keyPressEvent() notification to the subscribed UIEventListener objects.
    void dispatchKeyPressEvent(unsigned short unicode_ch, EKey key) 
    {
      makeCurrent();
      keyPress(key);
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->keyPressEvent(unicode_ch, key);
    }
    
    //! Dispatches the UIEventListener::keyReleaseEvent() notification to the subscribed UIEventListener objects.
    void dispatchKeyReleaseEvent(unsigned short unicode_ch, EKey key) 
    {
      makeCurrent();
      keyRelease(key);
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->keyReleaseEvent(unicode_ch, key);
    }

    //! Dispatches the UIEventListener::destroyEvent() notification to the subscribed UIEventListener(s), 
    //! calls destroyAllFBORenderTargets() and eraseAllEventListeners()
    //! This event must be issued just before the actual GL context is destroyed.
    void dispatchDestroyEvent()
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->destroyEvent();
      destroyAllFBORenderTargets();
      eraseAllEventListeners();
    }

    //! Dispatches the UIEventListener::updateEvent() notification to the subscribed UIEventListener objects.
    void dispatchRunEvent()
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->updateEvent();
    }

    //! Dispatches the UIEventListener::visibilityEvent() notification to the subscribed UIEventListener objects.
    void dispatchVisibilityEvent(bool visible) 
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->visibilityEvent(visible);
    }

    //! Dispatches the UIEventListener::initEvent() notification to the subscribed UIEventListener objects.
    // - called as soon as the OpenGL context is available but before the first resize event
    // - when initEvent() is called all the supported OpenGL extensions are already available
    // - when initEvent() is called the window has already acquired its width and height
    // - only the enabled event listeners receive this message
    void dispatchInitEvent()
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->initEvent();
    }

    //! Dispatches the UIEventListener::fileDroppedEvent() notification to the subscribed UIEventListener objects.
    void dispatchFileDroppedEvent(const std::vector<String>& files)
    {
      makeCurrent();
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->fileDroppedEvent(files);
    }
    
    //! Returns the std::set containing the currently pressed keys.
    const std::set<EKey>& keyboard() const { return mKeyboard; }

    //! Returns true if the given key is pressed.
    bool isKeyPressed(EKey key) const { return mKeyboard.find(key) != mKeyboard.end(); }

    //! Inserts the specified key in the set of currently active keys - For internal use only.
    void keyPress(EKey key) { mKeyboard.insert(key); }

    //! Removes the specified key from the set of currently active keys - For internal use only.
    void keyRelease(EKey key) { mKeyboard.erase(key); }

    //! Returns true if the OpenGLContext is in an initialized state.
    bool isInitialized() const { return mIsInitialized; }

    //! The number (clamped to VL_MAX_TEXTURE_UNITS) of texture units supported by the current hardware.
    int textureUnitCount() const { return mTextureUnitCount; }

    //! Returns \p true if an OpenGLContext supports double buffering.
    bool hasDoubleBuffer() const { return mHasDoubleBuffer; }
    
    //! Returns \p true if an OpenGLContext provides backwards compatibility to previous OpenGL API versions.
    bool isCompatible() const { return mIsCompatible; }

    //! Returns the OpenGL's driver version's major number.
    int openglVersionMajorNumber() const { return mMajorVersion; }

    //! Returns the OpenGL's driver version's minor number.
    int openglVersionMinorNumber() const { return mMinorVersion; }

    // --- render states management ---

    //! Activates the specified vertex attribute set - For internal use only.
    //! \param vas The IVertexAttribSet to be activated. It can be NULL, in which case all vertex attributes are disabled.
    //! If \p vas is the same as the last activated IVertexAttribSet then no operation is done.
    //! \param use_vbo Whether vertex-buffer-objects should be used when activating the vertex attributes.
    //! \param force Binds \p vas even if it was the last to be activated (this is also valid for NULL).
    void bindVAS(const IVertexAttribSet* vas, bool use_vbo, bool force);

    //! Applies an EnableSet to an OpenGLContext - For internal use only.
    void applyEnables( const EnableSet* prev, const EnableSet* cur );

    //! Applies a RenderStateSet to an OpenGLContext - For internal use only.
    void applyRenderStates( const RenderStateSet* prev, const RenderStateSet* cur, const Camera* camera );

    //! Resets all the interanal enable-tables - For internal use only.
    void resetEnables();

    //! Resets all the interanal render-states-tables - For internal use only.
    void resetRenderStates();

    //! Resets the OpenGL states necessary to begin and finish a rendering. - For internal use only.
    void resetContextStates();

    //! Declares that texture unit \p unit_i is currently bound to the specified texture target. - For internal use only.
    void setTexUnitBinding(int unit_i, ETextureDimension target) 
    { 
      VL_CHECK(unit_i <= VL_MAX_TEXTURE_UNITS);
      mTexUnitBinding[unit_i] = target; 
    }

    //! Returnes the texture target currently active for the specified texture unit. - For internal use only.
    ETextureDimension texUnitBinding(int unit_i) const
    {
      VL_CHECK(unit_i <= VL_MAX_TEXTURE_UNITS);
      return mTexUnitBinding[unit_i]; 
    }

    //! Returns \p true if the two UniformSet contain at least one Uniform variable with the same name.
    static bool areUniformsColliding(const UniformSet* u1, const UniformSet* u2);

    //! Checks whether the OpenGL state is clean or not.
    //! \par Clean state conditions:
    //! - All functionalities must be disabled, no GL_LIGHTING, GL_DEPTH_TEST, GL_LIGHTn, GL_CLIP_PLANEn etc. enabled, 
    //!   with the sole exception of GL_MULTISAMPLE.
    //! - All buffer objects targets such as GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER etc. must be bound to buffer object #0.
    //! - Current texture unit and client texture unit must be #0.
    //! - All texture matrices must be set to identity.
    //! - All texture targets must be bound to texture #0.
    //! - All GL_TEXTURE_COORD_ARRAYs must be disabled.
    //! - All texture targets such as GL_TEXTURE_1D, GL_TEXTURE_2D etc. must be disabled.
    //! - All texture targets should be bound to texture #0.
    //! - All texture coordinate generation modes such as GL_TEXTURE_GEN_S/T/R/Q must be disable for all texture units.
    //! - All vertex arrays such as GL_COLOR_ARRAY, GL_NORMAL_ARRAY etc. must be disabled, including the ones enabled with glEnableVertexAttribArray()
    //! - <b>NOTE: blending function must be set to glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)</b>.
    //! - Color write-mask should be glColorMask(GL_TRUE ,GL_TRUE, GL_TRUE, GL_TRUE).
    //! - Depth write-mask should be glDepthMask(GL_TRUE).
    //! - Polygon mode should be glPolygonMode(GL_FRONT_AND_BACK, GL_FILL).
    //! - <i>In general all OpenGL render states should be set to their default values.</i>
    static bool isCleanState(bool verbose);

  protected:
    ref<RenderTarget> mRenderTarget;
    std::vector< ref<FBORenderTarget> > mFBORenderTarget;
    std::vector< ref<UIEventListener> > mEventListeners;
    std::set<EKey> mKeyboard;
    OpenGLContextFormat mGLContextInfo;
    int mMaxVertexAttrib;
    int mTextureUnitCount;
    int mMajorVersion;
    int mMinorVersion;
    bool mMouseVisible;
    bool mContinuousUpdate;
    bool mIgnoreNextMouseMoveEvent;
    bool mFullscreen;
    bool mHasDoubleBuffer;
    bool mIsInitialized;
    bool mIsCompatible;

    // --- Render States ---
    // state table
    int mEnableTable[EN_EnableCount];
    int mRenderStateTable[RS_COUNT];
    // current state
    bool mCurrentEnable[EN_EnableCount];
    const RenderState* mCurrentRenderState[RS_COUNT];
    // default render states
    ref<RenderState> mDefaultRenderStates[RS_COUNT];

    // for each texture unit tells which target has been bound last.
    ETextureDimension mTexUnitBinding[VL_MAX_TEXTURE_UNITS];

  private:
    struct VertexArrayInfo
    {
      VertexArrayInfo(): mVBO(0), mPtr(0), mState(0), mEnabled(false) {}
      int   mVBO;
      const unsigned char* mPtr;
      int mState;
      bool mEnabled;
    };
  protected:
    // --- VertexAttribSet Management ---
    const IVertexAttribSet* mCurVAS;
    VertexArrayInfo mVertexArray;
    VertexArrayInfo mNormalArray;
    VertexArrayInfo mColorArray;
    VertexArrayInfo mSecondaryColorArray;
    VertexArrayInfo mFogArray;
    VertexArrayInfo mTexCoordArray[VL_MAX_TEXTURE_UNITS];
    VertexArrayInfo mVertexAttrib[VL_MAX_GENERIC_VERTEX_ATTRIB];

  private:
    void setupDefaultRenderStates();
  };
  // ----------------------------------------------------------------------------
}

#endif
