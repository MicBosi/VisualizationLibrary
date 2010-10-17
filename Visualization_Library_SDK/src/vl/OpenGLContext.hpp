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

#include <vl/Object.hpp>
#include <vl/UIEventListener.hpp>
#include <vl/FramebufferObject.hpp> // RenderTarget and FBORenderTarget
#include <vl/RenderState.hpp>
#include <vector>
#include <set>

namespace vl
{
  class EnableSet;
  class RenderStateSet;
  //-----------------------------------------------------------------------------
  // OpenGLContextFormat
  //-----------------------------------------------------------------------------
  /**
   * The OpenGLContextFormat class encapsulates the settings of an OpenGL rendering context.
  */
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
  /**
   * Represents an abstract GUI object containing an OpenGL context to which also keyboard, mouse or system events can be sent.
   * OpenGLContext is an abstract class that wraps a minimal common subset of GUI APIs like Win32, Qt, wxWidgets, SDL, GLUT, etc. \n
   * In order to respond to the events generated by the OpenGLContext you must subclass an UIEventListener and bind it to the OpenGLContext
   * using the functions addEventListener(ref<UIEventListener>) and removeEventListener(ref<UIEventListener>).
   * 
   * \par OpenGLContext custom implementation
   * - Key_Alt/Ctrl/Shift events must always be notified before Key_Left/Right-Alt/Ctrl/Shift events.
   * - Always update the mKeyboard structure appropriately especially with respect to Key_[Left/Right]-Alt/Ctrl/Shift events.
   * - When cycling through EventListeners to dispatch the events you must do it on a temporary copy of mEventListeners so that
   *   the EventListeners can safely add/remove themselves or other EventListeners to the OpenGLContext itself.
   */
  class OpenGLContext: public Object
  {
  public:
    virtual const char* className() { return "OpenGLContext"; }

    //! Constructor.
    OpenGLContext(int w=0, int h=0);

    /** Destructor.
     *  Dispatches also destroyEvent() to its event listeners.
     */
    ~OpenGLContext() 
    { 
      dispatchDestroyEvent();
      eraseAllEventListeners();
      destroyAllFBORenderTargets();
      mRenderTarget->mOpenGLContext = NULL;
    }

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

    //! Creates a new FBORenderTarget (framebuffer object RenderTarget).
    //! \note A framebuffer object always belongs to an OpenGL context and in order to render on it the appropriate OpenGL context must be active.
    ref<FBORenderTarget> createFBORenderTarget(int width, int height)
    { 
      mFBORenderTarget.push_back(new FBORenderTarget(this, width, height));
      return mFBORenderTarget.back();
    }
    //! Equivalent to \p "createFBORenderTarget(0,0);".
    ref<FBORenderTarget> createFBORenderTarget() { return createFBORenderTarget(0,0); }

    void destroyFBORenderTarget(FBORenderTarget* fbort)
    {
      for(unsigned i=0; i<mFBORenderTarget.size(); ++i)
        if (mFBORenderTarget[i] == fbort)
        {
          mFBORenderTarget[i]->mOpenGLContext = NULL;
          mFBORenderTarget[i]->destroy();
          mFBORenderTarget.erase(mFBORenderTarget.begin()+i);
          break;
        }
    }

    void destroyAllFBORenderTargets()
    {
      for(unsigned i=0; i<mFBORenderTarget.size(); ++i)
      {
        mFBORenderTarget[i]->mOpenGLContext = NULL;
        mFBORenderTarget[i]->destroy();
      }
      mFBORenderTarget.clear();
    }

    //! Asks to the windowing system that is managing the OpenGLContext to quit the application.
    virtual void quitApplication() {}

    // This function should be appropriately reimplemented in all the subclasses of OpenGLContext.
    virtual void destroy() { destroyAllFBORenderTargets(); }

    virtual void update() {}

    virtual void setWindowTitle(const String&) {}

    virtual bool setFullscreen(bool) { mFullscreen = false; return false; }

    virtual void show() {}

    virtual void hide() {}

    virtual void setPosition(int /*x*/, int /*y*/) {}

    virtual void setSize(int /*w*/, int /*h*/) {}

    virtual ivec2 position() const { return ivec2(); }

    virtual ivec2 size() const { return ivec2(renderTarget()->width(),renderTarget()->height()); }

    virtual bool mouseVisible() const { return mMouseVisible; }

    virtual bool fullscreen() const { return mFullscreen; }

    virtual void setMouseVisible(bool) { mMouseVisible=false; }

    virtual void getFocus() {}

    virtual void swapBuffers() {}

    virtual void makeCurrent() {}

    virtual void setMousePosition(int /*x*/, int /*y*/) {}

    void setVSyncEnabled(bool enable);

    bool vsyncEnabled() const;

    virtual void setContinuousUpdate(bool continuous) { mContinuousUpdate = continuous; }

    bool continuousUpdate() const { return mContinuousUpdate; }

    // event listeners

    void sortEventListeners();
    void addEventListener(UIEventListener* el);
    void removeEventListener(UIEventListener* el);
    void eraseAllEventListeners();
    const std::vector< ref<UIEventListener> >& eventListeners() const { return mEventListeners; }
    UIEventListener* eventListener(int i) const { return mEventListeners[i].get(); }
    int eventListenerCount() const { return (int)mEventListeners.size(); }

    const OpenGLContextFormat& openglContextInfo() const { return mGLContextInfo; }
    void setOpenGLContextInfo(const OpenGLContextFormat& info) { mGLContextInfo = info; }

    void ignoreNextMouseMoveEvent() { mIgnoreNextMouseMoveEvent = true; }

    int width() const { return mRenderTarget->width(); }
    int height() const { return mRenderTarget->height(); }

    // call this function at the beginning if you reimplement it
    void dispatchResizeEvent(int w, int h) 
    {
      mRenderTarget->setWidth(w);
      mRenderTarget->setHeight(h);

      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->resizeEvent( w, h );
    }

    void dispatchMouseMoveEvent(int x, int y)
    {
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
    
    void dispatchMouseUpEvent(EMouseButton button, int x, int y) 
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->mouseUpEvent(button, x, y);
    }
    
    void dispatchMouseDownEvent(EMouseButton button, int x, int y) 
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->mouseDownEvent(button, x, y);
    }
    
    void dispatchMouseWheelEvent(int n) 
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->mouseWheelEvent(n);
    }
    
    void dispatchKeyPressEvent(unsigned short unicode_ch, EKey key) 
    {
      keyPress(key);
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->keyPressEvent(unicode_ch, key);
    }
    
    void dispatchKeyReleaseEvent(unsigned short unicode_ch, EKey key) 
    {
      keyRelease(key);
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->keyReleaseEvent(unicode_ch, key);
    }

    void dispatchDestroyEvent()
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->destroyEvent();
    }

    void dispatchRunEvent()
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->runEvent();
    }

    void dispatchVisibilityEvent(bool visible) 
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->visibilityEvent(visible);
    }

    //! Dispatches the init event
    // - called as soon as the OpenGL context is available but before the first resize event
    // - when initEvent() is called all the supported OpenGL extensions are already available
    // - when initEvent() is called the window has already acquired its width and height
    // - only the enabled event listeners receive this message
    void dispatchInitEvent()
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->initEvent();
    }

    void dispatchFileDroppedEvent(const std::vector<String>& files)
    {
      std::vector< ref<UIEventListener> > temp_clients = eventListeners();
      for( unsigned i=0; i<temp_clients.size(); ++i )
        if ( temp_clients[i]->isEnabled() )
          temp_clients[i]->fileDroppedEvent(files);
    }
    
    // this value is clamped to VL_MAX_TEXTURE_UNITS
    int textureUnitCount() const { return mTextureUnitCount; }

    //! Returns the std::set containing the currently pressed keys
    const std::set<EKey>& keyboard() const { return mKeyboard; }

    //! Returns true if the given key is pressed
    bool isKeyPressed(EKey key) const { return mKeyboard.find(key) != mKeyboard.end(); }

    //! For internal use only
    void keyPress(EKey key) { mKeyboard.insert(key); }

    //! For internal use only
    void keyRelease(EKey key) { mKeyboard.erase(key); }

    bool hasDoubleBuffer() const { return mHasDoubleBuffer; }

    bool isInitialized() const { return mIsInitialized; }
    bool isCompatible() const { return mIsCompatible; }

    // --- render states management ---

    // mic fixme: da rivedere se alcune sono da togliere + documenta
    void applyEnables( const EnableSet* prev, const EnableSet* cur );
    void applyRenderStates( const RenderStateSet* prev, const RenderStateSet* cur, const Camera* camera );
    void resetEnables();
    void resetRenderStates();
    /** Resets the OpenGL rendering states */
    void resetContextStates();
    bool checkIsCleanState() const;

    void setTexUnitBinding(int unit, ETextureDimension target) 
    { 
      VL_CHECK(unit <= VL_MAX_TEXTURE_UNITS);
      mTexUnitBinding[unit] = target; 
    }

    ETextureDimension texUnitBinding(int unit) const
    {
      VL_CHECK(unit <= VL_MAX_TEXTURE_UNITS);
      return mTexUnitBinding[unit]; 
    }

  protected:
    ref<RenderTarget> mRenderTarget;
    std::vector< ref<FBORenderTarget> > mFBORenderTarget;
    std::vector< ref<UIEventListener> > mEventListeners;
    std::set<EKey> mKeyboard;
    OpenGLContextFormat mGLContextInfo;
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

    // RENDER STATES
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
    void setupDefaultRenderStates();
  };
  // ----------------------------------------------------------------------------
}

#endif
