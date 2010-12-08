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

#ifndef FramebufferObject_INCLUDE_ONCE
#define FramebufferObject_INCLUDE_ONCE

#include <vl/Camera.hpp>
#include <vl/Texture.hpp>
#include <set>
#include <map>

namespace vl
{
  class FBORenderTarget;
  //-----------------------------------------------------------------------------
  // FBOAttachmentAbstract
  //-----------------------------------------------------------------------------
  /**
   * Abstract class that represents a framebuffer object attachment, i.e. a texture or renderbuffer attachment.
  */
  class FBOAttachmentAbstract: public Object
  {
    friend class FBORenderTarget;

  private:
    // no copy constructor and no assignment operator
    FBOAttachmentAbstract(const FBOAttachmentAbstract& other): Object(other) {}
    void operator=(const FBOAttachmentAbstract&) {}

  public:
    virtual const char* className() { return "FBOAttachmentAbstract"; }
    //! Constructor.
    FBOAttachmentAbstract() {}

    //! Destructor.
    virtual ~FBOAttachmentAbstract() { destroy(); }

    virtual void destroy();

    /** Binds this renderbuffer to the currently active framebuffer object (GL_FRAMEBUFFER).
     * This function also initializes its storage if 'w' and 'h' differ from the renderbuffer's 
     * current width() and height() and the renderbuffer is not a texture. */
    virtual void bindAttachment(int w, int h, EAttachmentPoint attach_point)  = 0;

    //! Returns an std::set containing the FBORenderTarget that use this FBO attachment
    const std::set< ref<FBORenderTarget> >& fboRenderTargets() const { return mFBORenderTargets; }

  protected:
    std::set< ref<FBORenderTarget> > mFBORenderTargets;
  };
  //-----------------------------------------------------------------------------
  // FBORenderbufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * Abstract class that represents a framebuffer renderbuffer attachment, i.e. a non texture fbo attachment.
  */
  class FBORenderbufferAttachment: public FBOAttachmentAbstract
  {
    friend class FBORenderTarget;

  public:
    FBORenderbufferAttachment(): mHandle(0), mWidth(0), mHeight(0), mSamples(0), mReallocateRenderbuffer(true) {}
    virtual const char* className() { return "FBORenderbufferAttachment"; }
    //! Creates a renderbuffer object calling glGenRenderbuffers(). 
    //! The identifier returned by glGenRenderbuffers() can be queried calling the handle() method.
    void create();
    //! Deletes the renderbuffer object created with the create() function
    void destroy();
    //! Returns the handle obtained by create() using glGenRenderbuffers()
    unsigned int handle() const { return mHandle; }
    /**
     * Initializes the storage of the renderbuffer with the given dimensions.
     * Note that the renderbuffer storage type is defined by the setType() method of 
     * FBOColorBufferAttachment, FBODepthBufferAttachment, FBOStencilBufferAttachment,
     * FBODepthStencilBufferAttachment. See also setSamples().
     */
    void initStorage(int w, int h);
    //! The same as calling initStorage(width(), height());
    void initStorage() { initStorage(width(), height()); }
    //! Returns the with of the allocated renderbuffer storage
    int width() const { return mWidth; }
    //! Returns the with of the allocated renderbuffer storage
    int height() const { return mHeight; }
    //! Returns the number of samples to be used when allocating the renderbuffer's storage
    int samples() const { return mSamples; }
    /**
     * Defines the width to be specified when allocating the renderbuffer storage. Note that this function schedules a reallocation of the renderbuffer storage.
     * If 'w' is set to 0 the renderbuffer storage allocation will use the default dimensions specified by the FBORenderTarget.
     */
    void setWidth(int w) { if (w != mWidth) /* schedules recreation */ mReallocateRenderbuffer = true; mWidth = w; }
    /**
     * Defines the height to be specified when allocating the renderbuffer storage. Note that this function schedules a reallocation of the renderbuffer storage.
     * If 'h' is set to 0 the renderbuffer storage allocation will use the default dimensions specified by the FBORenderTarget.
     */
    void setHeight(int h) { if (h != mHeight) /* schedules recreation */ mReallocateRenderbuffer = true; mHeight = h; }
    //! Sets the number of samples to be specified when allocating the renderbuffer's storage. Note that this function schedules a reallocation of the renderbuffer storage.
    void setSamples(int samples) { if (samples != mSamples) /* schedules recreation */ mReallocateRenderbuffer = true; mSamples = samples; }
    //! Returns false if the renderbuffer storage needs to be created or reallocated due to a change of the sample count or renderbuffer type.
    bool renderbufferStorageReady() const { return !mReallocateRenderbuffer; }

  protected:
    virtual int internalType() = 0;
    void internalBindAttachment(int w, int h, int attach_point);

  protected:
    unsigned int mHandle;
    int mWidth;
    int mHeight;
    int mSamples;
    bool mReallocateRenderbuffer;
  };
  //-----------------------------------------------------------------------------
  // FBOColorBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A color renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Supports GL_EXT_framebuffer_multisample.
  */
  class FBOColorBufferAttachment: public FBORenderbufferAttachment
  {
  public:
    FBOColorBufferAttachment(EColorBufferFormat type)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType = type;
    }
    virtual const char* className() { return "FBOColorBufferAttachment"; }
    //! Defines the type to specify when allocating the renderbuffer storage. Note that this function schedules a reallocation of the renderbuffer storage.
    void setType(EColorBufferFormat type) { if (type != mType) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }
    EColorBufferFormat type() const { return mType; }
    void bindAttachment(int w, int h, EAttachmentPoint attach_point) { internalBindAttachment( w, h, attach_point ); }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EColorBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBODepthBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A depth renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Supports GL_EXT_framebuffer_multisample.
  */
  class FBODepthBufferAttachment: public FBORenderbufferAttachment
  {
  public:
    FBODepthBufferAttachment(EDepthBufferFormat type)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType = type;
    }
    virtual const char* className() { return "FBODepthBufferAttachment"; }
    //! Defines the type to specify when allocating the renderbuffer storage. Note that this function schedules a reallocation of the renderbuffer storage.
    void setType(EDepthBufferFormat type) { if (type != mType) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }
    EDepthBufferFormat type() const { return mType; }
    void bindAttachment(int w, int h, EAttachmentPoint attach_point) { internalBindAttachment( w, h, attach_point ); }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EDepthBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBOStencilBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A stencil renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Supports GL_EXT_framebuffer_multisample.
  */
  class FBOStencilBufferAttachment: public FBORenderbufferAttachment
  {
  public:
    FBOStencilBufferAttachment(EStencilBufferFormat type)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType = type;
    }
    virtual const char* className() { return "FBOStencilBufferAttachment"; }
    //! Defines the type to specify when allocating the renderbuffer storage. Note that this function schedules a reallocation of the renderbuffer storage.
    void setType(EStencilBufferFormat type) { if (type != mType) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }
    EStencilBufferFormat type() const { return mType; }
    void bindAttachment(int w, int h, EAttachmentPoint attach_point) { internalBindAttachment( w, h, attach_point ); }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EStencilBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBODepthStencilBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A depth+stencil renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Supports GL_EXT_framebuffer_multisample.
  */
  class FBODepthStencilBufferAttachment: public FBORenderbufferAttachment
  {
  public:
    FBODepthStencilBufferAttachment(EDepthStencilBufferFormat type)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType = type;
    }
    virtual const char* className() { return "FBODepthStencilBufferAttachment"; }
    //! Defines the type to specify when allocating the renderbuffer storage. Note that this function schedules a reallocation of the renderbuffer storage.
    void setType(EDepthStencilBufferFormat type) { if (type != mType) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }
    EDepthStencilBufferFormat type() const { return mType; }
    void bindAttachment(int w, int h, EAttachmentPoint attach_point) { internalBindAttachment( w, h, attach_point ); }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EDepthStencilBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------
  /**
   * A 1D texture renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Wraps the function glFramebufferTexture1DEXT.
  */
  class FBOTexture1DAttachment: public FBOAttachmentAbstract
  {
  public:
    FBOTexture1DAttachment(Texture* texture, int level)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mLevel      = level;
      mTexture    = texture;
    }
    virtual const char* className() { return "FBOTexture1DAttachment"; }
    Texture* texture() { return mTexture.get(); }
    const Texture* texture() const { return mTexture.get(); }
    int level() const { return mLevel; }

    virtual void bindAttachment(int w, int h, EAttachmentPoint attach_point);

  protected:
    ref<Texture> mTexture;
    int mLevel;
  };
  //-----------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------
  /**
   * A 2D texture renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Wraps the function glFramebufferTexture2DEXT.
  */
  class FBOTexture2DAttachment: public FBOAttachmentAbstract
  {
  public:
    virtual const char* className() { return "FBOTexture2DAttachment"; }
    FBOTexture2DAttachment(Texture* texture, int level, ETex2DTarget target)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mLevel      = level;
      mTexture    = texture;
      mTextureTarget = target;
    }
    void setTexture(Texture* texture) { mTexture = texture; }
    Texture* texture() { return mTexture.get(); }
    const Texture* texture() const { return mTexture.get(); }
    int level() const { return mLevel; }

    ETex2DTarget textureTarget() const { return mTextureTarget; }
    void setTextureTarget( ETex2DTarget target ) { mTextureTarget = target; }

    virtual void bindAttachment(int w, int h, EAttachmentPoint attach_point);

  protected:
    ref<Texture> mTexture;
    ETex2DTarget mTextureTarget;
    int mLevel;
  };
  //-----------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------
  /**
   * A texture renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Wraps the glFramebufferTextureEXT function from GL_ARB_geometry_shader4.
  */
  class FBOTextureAttachment: public FBOAttachmentAbstract
  {
  public:
    FBOTextureAttachment(Texture* texture, int level)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mLevel      = level;
      mTexture    = texture;
    }
    virtual const char* className() { return "FBOTextureAttachment"; }
    Texture* texture() { return mTexture.get(); }
    const Texture* texture() const { return mTexture.get(); }
    int level() const { return mLevel; }

    virtual void bindAttachment(int w, int h, EAttachmentPoint attach_point);

  protected:
    ref<Texture> mTexture;
    int mLevel;
  };
  //-----------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------
  /**
   * A 3D texture renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Wraps the function glFramebufferTexture3DEXT.
  */
  class FBOTexture3DAttachment: public FBOAttachmentAbstract
  {
  public:
    FBOTexture3DAttachment(Texture* texture, int mipmap_level, int layer)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mTexture = texture;
      mLayer   = layer;
      mMipmapLevel = mipmap_level;
    }
    virtual const char* className() { return "FBOTexture3DAttachment"; }
    Texture* texture() { return mTexture.get(); }
    const Texture* texture() const { return mTexture.get(); }
    int mipmapLevel() const { return mMipmapLevel; }
    void setMipmapLevel(int level) { mMipmapLevel = level; }
    int layer() const { return mLayer; }
    void setLayer(int layer) { mLayer = layer; }

    virtual void bindAttachment(int w, int h, EAttachmentPoint attach_point);

  protected:
    ref<Texture> mTexture;
    int mMipmapLevel;
    int mLayer;
  };
  //-----------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------
  /**
   * A texture layer renderbuffer to be attached to a framebuffer object rendering target (FBORenderTarget).
   * Wraps the function glFramebufferTextureLayerEXT from GL_EXT_texture_array and GL_ARB_geometry_shader4.
  */
  class FBOTextureLayerAttachment: public FBOAttachmentAbstract
  {
  public:
    FBOTextureLayerAttachment(Texture* texture, int mipmap_level, int layer)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mTexture = texture;
      mLayer   = layer;
      mMipmapLevel = mipmap_level;
    }
    virtual const char* className() { return "FBOTextureLayerAttachment"; }
    Texture* texture() { return mTexture.get(); }
    const Texture* texture() const { return mTexture.get(); }
    int mipmapLevel() const { return mMipmapLevel; }
    void setMipmapLevel(int level) { mMipmapLevel = level; }
    int layer() const { return mLayer; }
    void setLayer(int layer) { mLayer = layer; }

    virtual void bindAttachment(int w, int h, EAttachmentPoint attach_point);

  protected:
    ref<Texture> mTexture;
    int mMipmapLevel;
    int mLayer;
  };
  //-----------------------------------------------------------------------------
  // FBORenderTarget
  //-----------------------------------------------------------------------------
  /**
   * Implements a framebuffer object to be used as a rendering target as specified by
   * the GL_EXT_framebuffer_object extension.
   * An FBORenderTarget belongs to one and only one OpenGLContext and can be created
   * using the OpenGLContext::createFBORenderTarget() method.
   * To render to a FBORenderTarget use the Rendering::setRenderTarget() function.
   *
   * \remarks
   * Before using any method from FBORenderTarget make sure that the appropriate
   * OpenGL rendering context is active using FBORenderTarget::openglContext()->makeCurrent()
   * \remarks
   * All the renderbuffer attachments must specify the same number of samples.
   * 
  */
  class FBORenderTarget: public RenderTarget
  {
    friend class OpenGLContext;

  private:
    FBORenderTarget(const FBORenderTarget& other): RenderTarget(other), mHandle(0) {}
    void operator=(const FBORenderTarget&) {}
    FBORenderTarget(OpenGLContext* ctx, int w, int h): RenderTarget(ctx, w, h), mHandle(0)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

  public:
    ~FBORenderTarget() { destroy(); }
    virtual const char* className() { return "FBORenderTarget"; }
    /**
     * Makes the framebuffer the current rendering target calling glBindFramebuffer(GL_FRAMEBUFFER, FBORenderTarget::handle())
     * and initializes all the previously defined attachment points.
    */
    virtual void bindFramebuffer();

    GLenum checkFramebufferStatus();

    void printFramebufferError(GLenum status) const;

    void create()
    {
      if (!mHandle)
      {
        VL_glGenFramebuffers(1, (unsigned int*)&mHandle); VL_CHECK_OGL();
      }
      VL_CHECK(mHandle)
    }

    void destroy()
    {
      removeAllAttachments();
      if (handle())
      {
        VL_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        VL_glDeleteFramebuffers(1,&mHandle);
        VL_CHECK_OGL()
        mHandle = 0;
      }
      setWidth(0);
      setHeight(0);
    }

    void addColorAttachment(EAttachmentPoint color_attachment, FBOAttachmentAbstract* attachment);
    void addTextureAttachment(EAttachmentPoint color_attachment, FBOAttachmentAbstract* attachment);
    void addDepthAttachment(FBOAttachmentAbstract* attachment);
    void addStencilAttachment(FBOAttachmentAbstract* attachment);
    const std::map< EAttachmentPoint, ref<FBOAttachmentAbstract> >& fboAttachments() const { return mFBOAttachments; }
    void removeAttachment(FBOAttachmentAbstract* attachment);
    void removeAttachment(EAttachmentPoint attach_point);
    void removeAllAttachments();

    unsigned int handle() const { return mHandle; }

  public:
    std::map< EAttachmentPoint, ref<FBOAttachmentAbstract> > mFBOAttachments;
    unsigned int mHandle;
  };
}

#endif
