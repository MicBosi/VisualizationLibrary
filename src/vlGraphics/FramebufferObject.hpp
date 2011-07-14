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

#include <vlGraphics/Camera.hpp>
#include <vlGraphics/Texture.hpp>
#include <set>
#include <map>

namespace vl
{
  class FBORenderTarget;
//-----------------------------------------------------------------------------
// FBOAbstractAttachment
//-----------------------------------------------------------------------------
  /**
   * Abstract class that represents a framebuffer object attachment to be used with FBORenderTarget.
   */
  class VLGRAPHICS_EXPORT FBOAbstractAttachment: public Object
  {
    VL_INSTRUMENT_CLASS(vl::FBOAbstractAttachment, Object)

    friend class FBORenderTarget;

  private:
    // no copy constructor and no assignment operator
    FBOAbstractAttachment( const FBOAbstractAttachment& other ): Object( other ) {}
    void operator=( const FBOAbstractAttachment& ) {}

  public:
    /** Constructor */
    FBOAbstractAttachment() {}

    /** Destructor, removes automatically the FBO attachment from all bound FBO render targets and releases any OpenGL resource. */
    virtual ~FBOAbstractAttachment() { destroy(); }

    /** Removes the FBO attachment from all bound FBO render targets and releases any associated OpenGL resource. */
    virtual void destroy();

    /** Returns an std::set containing the FBORenderTarget that use this FBO attachment. */
    const std::set< ref<FBORenderTarget> >& fboRenderTargets() const { return mFBORenderTargets; }

  protected:
    virtual void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point ) = 0;

  protected:
    std::set< ref<FBORenderTarget> > mFBORenderTargets;
  };
  //-----------------------------------------------------------------------------
  // FBORenderbufferAttachment
  //-----------------------------------------------------------------------------
  /** 
   * Abstract class that represents a framebuffer renderbuffer attachment, that is, a non-texture fbo attachment (wraps \p glFramebufferRenderbuffer()).
   * \sa FBORenderTarget.
   * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glFramebufferRenderbuffer.xml
   */
  class VLGRAPHICS_EXPORT FBORenderbufferAttachment: public FBOAbstractAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBORenderbufferAttachment, FBOAbstractAttachment)

    friend class FBORenderTarget;

  public:    
    /** Constructor. */
    FBORenderbufferAttachment(): mHandle( 0 ), mWidth( 0 ), mHeight( 0 ), mSamples( 0 ), mReallocateRenderbuffer( true ) {}
    
    /**
     * Creates a renderbuffer object calling glGenRenderbuffers(). 
     * The identifier returned by glGenRenderbuffers() can be queried calling the handle() method.
     */
    void create();
    
    /** Deletes the renderbuffer object created with the create() method. */
    void destroy();
    
    /** 
     * Sets the handle for this attachment, the handle must have been created by glGenRenderbuffers().
     * Normally you don't need to call this. See also: create(), handle().
     */
    void setHandle( GLuint  handle ) { if ( mHandle != handle ) { mHandle = handle; mReallocateRenderbuffer = false; } }
    
    /** Returns the handle obtained by create() using glGenRenderbuffers() */
    GLuint handle() const { return mHandle; }
    
    /**
     * Initializes the storage of the renderbuffer with the given sample count and dimensions.
     * This method does nothing if the the width, height and sample count has not changed since last time it was called.
     * Note that the renderbuffer storage type is defined by the setType() method of 
     * FBOColorBufferAttachment, FBODepthBufferAttachment, FBOStencilBufferAttachment,
     * FBODepthStencilBufferAttachment. See also setSamples().
     */
    void initStorage( int w, int h, int samples );
    
    /** The same as calling initStorage( width(), height() ) */
    void initStorage() { initStorage( width(), height(), samples() ); }
    
    /** 
     * Returns the with of the renderbuffer storage. 
     * Note that if renderbufferStorageReady() returns \p false it means that 
     * the renderbuffer has not been allocated yet.
     */
    int width() const { return mWidth; }
    
    /** 
     * Returns the height of the renderbuffer storage. 
     * Note that if renderbufferStorageReady() returns \p false it means that 
     * the renderbuffer has not been allocated yet.
     */
    int height() const { return mHeight; }
    
    /** 
     * Returns the number of samples to be used when allocating the renderbuffer's storage.
     * Note that if renderbufferStorageReady() returns \p false it means that 
     * the renderbuffer has not been allocated yet.
     */
    int samples() const { return mSamples; }
    
    /**
     * The width of the renderbuffer storage to be allocated.
     * If 'w' is set to 0 the renderbuffer storage allocation will use the dimensions of the next FBORenderTarget bound.
     */
    void setWidth( int w ) { if ( w != mWidth ) /* schedules recreation */ mReallocateRenderbuffer = true; mWidth = w; }
    
    /**
     * The height of the renderbuffer storage to be allocated.
     * If 'h' is set to 0 the renderbuffer storage allocation will use the dimensions of the next FBORenderTarget bound.
     */
    void setHeight( int h ) { if ( h != mHeight ) /* schedules recreation */ mReallocateRenderbuffer = true; mHeight = h; }
    
    /** 
     * Sets the number of samples to be specified when allocating the renderbuffer's storage. 
     */
    void setSamples( int samples ) { if ( samples != mSamples ) /* schedules recreation */ mReallocateRenderbuffer = true; mSamples = samples; }
    
    /** Returns \p false if the renderbuffer storage needs to be created or reallocated due to a change of the sample count, renderbuffer type or dimension. */
    bool renderbufferStorageReady() const { return !mReallocateRenderbuffer; }

  protected:
    void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point );
    virtual int internalType() = 0;

  protected:
    GLuint mHandle;
    int mWidth;
    int mHeight;
    int mSamples;
    bool mReallocateRenderbuffer;
  };
  //-----------------------------------------------------------------------------
  // FBOColorBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A color renderbuffer to be attached to a FBORenderTarget.
   */
  class VLGRAPHICS_EXPORT FBOColorBufferAttachment: public FBORenderbufferAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOColorBufferAttachment, FBOColorBufferAttachment)

  public:
    /** Constructor */
    FBOColorBufferAttachment( EColorBufferFormat type )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mType = type;
    }
    
    /** The type to specify when allocating the renderbuffer storage. */
    void setType( EColorBufferFormat type ) { if ( type != mType ) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }
    
    /** The type to specify when allocating the renderbuffer storage. */
    EColorBufferFormat type() const { return mType; }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EColorBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBODepthBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A depth renderbuffer to be attached to a FBORenderTarget.
   */
  class VLGRAPHICS_EXPORT FBODepthBufferAttachment: public FBORenderbufferAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBODepthBufferAttachment, FBORenderbufferAttachment)

  public:
    /** Constructor */
    FBODepthBufferAttachment( EDepthBufferFormat type )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mType = type;
    }

    /** The type to specify when allocating the renderbuffer storage. */
    void setType( EDepthBufferFormat type ) { if ( type != mType ) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }

    /** The type to specify when allocating the renderbuffer storage. */
    EDepthBufferFormat type() const { return mType; }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EDepthBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBOStencilBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A stencil renderbuffer to be attached to a FBORenderTarget.
   */
  class VLGRAPHICS_EXPORT FBOStencilBufferAttachment: public FBORenderbufferAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOStencilBufferAttachment, FBORenderbufferAttachment)

  public:
    /** Constructor */
    FBOStencilBufferAttachment( EStencilBufferFormat type )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mType = type;
    }

    /** The type to specify when allocating the renderbuffer storage. */
    void setType( EStencilBufferFormat type ) { if ( type != mType ) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }

    /** The type to specify when allocating the renderbuffer storage. */
    EStencilBufferFormat type() const { return mType; }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EStencilBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBODepthStencilBufferAttachment
  //-----------------------------------------------------------------------------
  /**
   * A depth+stencil renderbuffer to be attached to a FBORenderTarget.
   */
  class VLGRAPHICS_EXPORT FBODepthStencilBufferAttachment: public FBORenderbufferAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBODepthStencilBufferAttachment, FBORenderbufferAttachment)

  public:
    /** Constructor */
    FBODepthStencilBufferAttachment( EDepthStencilBufferFormat type )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mType = type;
    }

    /** The type to specify when allocating the renderbuffer storage. */
    void setType( EDepthStencilBufferFormat type ) { if ( type != mType ) /* schedules recreation */ mReallocateRenderbuffer = true; mType = type; }

    /** The type to specify when allocating the renderbuffer storage. */
    EDepthStencilBufferFormat type() const { return mType; }

  protected:
    virtual int internalType() { return type(); }

  protected:
    EDepthStencilBufferFormat mType;
  };
  //-----------------------------------------------------------------------------
  // FBOAbstractTextureAttachment
  //-----------------------------------------------------------------------------
  /**
   * Base class for all the framebuffer texture attachments (see also FBORenderTarget).
   */
  class VLGRAPHICS_EXPORT FBOAbstractTextureAttachment: public FBOAbstractAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOAbstractTextureAttachment, FBOAbstractAttachment)

  public:
    /** Constructor. */
    FBOAbstractTextureAttachment( Texture* texture, int mipmap_level ): mTexture(texture), mMipmapLevel(mipmap_level)
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }

    /** The texture bound to this attachment. */
    void setTexture( Texture* texture ) { mTexture = texture; }

    /** The texture bound to this attachment. */
    Texture* texture() { return mTexture.get(); }

    /** The texture bound to this attachment. */
    const Texture* texture() const { return mTexture.get(); }

    /** The mipmap level of the texture to attach. */
    void setMipmapLevel( int mipmap_level ) { mMipmapLevel = mipmap_level; }

    /** The mipmap level of the texture to attach. */
    int mipmapLevel() const { return mMipmapLevel; }

  protected:
    ref<Texture> mTexture;
    int mMipmapLevel;
  };
  //-----------------------------------------------------------------------------
  // FBOTexture1DAttachment
  //-----------------------------------------------------------------------------
  /**
   * A 1D texture renderbuffer to be attached to a FBORenderTarget (wraps \p glFramebufferTexture1D()).
   * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glFramebufferTexture.xml
   */
  class VLGRAPHICS_EXPORT FBOTexture1DAttachment: public FBOAbstractTextureAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOTexture1DAttachment, FBOAbstractTextureAttachment)

  public:
    /** Constructor. */
    FBOTexture1DAttachment( Texture* texture, int mipmap_level ): FBOAbstractTextureAttachment( texture, mipmap_level )
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }

  protected:
    virtual void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point );
  };
  //-----------------------------------------------------------------------------
  // FBOTexture2DAttachment
  //-----------------------------------------------------------------------------
  /**
   * A 2D texture renderbuffer to be attached to a FBORenderTarget (wraps \p glFramebufferTexture2D()).
   * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glFramebufferTexture.xml
   */
  class VLGRAPHICS_EXPORT FBOTexture2DAttachment: public FBOAbstractTextureAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOTexture2DAttachment, FBOAbstractTextureAttachment)

  public:
    /** Constructor. */
    FBOTexture2DAttachment( Texture* texture, int mipmap_level, ETex2DTarget target ): FBOAbstractTextureAttachment( texture, mipmap_level )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mTextureTarget = target;
    }

    /** What type of texture is expected, or for cube map textures, which face is to be attached. */
    void setTextureTarget( ETex2DTarget target ) { mTextureTarget = target; }

    /** What type of texture is expected, or for cube map textures, which face is to be attached. */
    ETex2DTarget textureTarget() const { return mTextureTarget; }

  protected:
    virtual void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point );

  protected:
    ETex2DTarget mTextureTarget;
  };
  //-----------------------------------------------------------------------------
  // FBOTextureAttachment
  //-----------------------------------------------------------------------------
  /**
   * A texture renderbuffer to be attached to a FBORenderTarget (wraps \p glFramebufferTexture()).
   * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glFramebufferTexture.xml
   */
  class VLGRAPHICS_EXPORT FBOTextureAttachment: public FBOAbstractTextureAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOTextureAttachment, FBOAbstractTextureAttachment)

  public:
    /** Constructor. */
    FBOTextureAttachment( Texture* texture, int mipmap_level ): FBOAbstractTextureAttachment( texture, mipmap_level )
    {
      VL_DEBUG_SET_OBJECT_NAME()
    }

  protected:
    virtual void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point );

  };
  //-----------------------------------------------------------------------------
  // FBOTexture3DAttachment
  //-----------------------------------------------------------------------------
  /**
   * A 3D texture renderbuffer to be attached to a FBORenderTarget (wraps \p glFramebufferTexture3D()).
   * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glFramebufferTexture.xml
   */
  class VLGRAPHICS_EXPORT FBOTexture3DAttachment: public FBOAbstractTextureAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOTexture3DAttachment, FBOAbstractTextureAttachment)

  public:
    FBOTexture3DAttachment( Texture* texture, int mipmap_level, int layer ): FBOAbstractTextureAttachment( texture, mipmap_level )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mLayer   = layer;
    }

    /** The layer of a 2-dimensional image within a 3-dimensional texture. */
    void setLayer( int layer ) { mLayer = layer; }

    /** The layer of a 2-dimensional image within a 3-dimensional texture. */
    int layer() const { return mLayer; }

  protected:
    virtual void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point );

  protected:
    int mLayer;
  };
  //-----------------------------------------------------------------------------
  // FBOTextureLayerAttachment
  //-----------------------------------------------------------------------------
  /**
   * A texture layer renderbuffer to be attached to a FBORenderTarget (wraps \p glFramebufferTextureLayer()).
   * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glFramebufferTextureLayer.xml
   */
  class VLGRAPHICS_EXPORT FBOTextureLayerAttachment: public FBOAbstractTextureAttachment
  {
    VL_INSTRUMENT_CLASS(vl::FBOTextureLayerAttachment, FBOAbstractTextureAttachment)

  public:
    /** Constructor. */
    FBOTextureLayerAttachment( Texture* texture, int mipmap_level, int layer ): FBOAbstractTextureAttachment( texture, mipmap_level )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mLayer   = layer;
    }

    /** The layer of a 2-dimensional image within a 3-dimensional texture or texture array. */
    int layer() const { return mLayer; }

    /** The layer of a 2-dimensional image within a 3-dimensional texture or texture array. */
    void setLayer( int layer ) { mLayer = layer; }

  protected:
    virtual void bindAttachment( FBORenderTarget* fbo, EAttachmentPoint attach_point );

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
   * the \p ARB_framebuffer_object extension.
   * An FBORenderTarget belongs to one and only one OpenGLContext and can be created
   * using the OpenGLContext::createFBORenderTarget() method.
   * To render to a FBORenderTarget use the Rendering::setRenderTarget() function.
   *
   * \remarks
   * Before using any method from FBORenderTarget make sure that the appropriate
   * OpenGL rendering context is active using FBORenderTarget::openglContext()->makeCurrent()
   *
   * \remarks
   * All the renderbuffer attachments must specify the same number of samples.
   *
   * \sa
   * - \ref pagGuideFramebufferObject
   * - \p ARB_framebuffer_object specifications http://www.opengl.org/registry/specs/ARB/framebuffer_object.txt
   * - FBORenderbufferAttachment 
   * - FBOColorBufferAttachment 
   * - FBODepthBufferAttachment 
   * - FBODepthStencilBufferAttachment 
   * - FBOStencilBufferAttachment 
   * - FBOTexture1DAttachment 
   * - FBOTexture2DAttachment 
   * - FBOTexture3DAttachment 
   * - FBOTextureAttachment 
   * - FBOTextureLayerAttachment 
   */
  class VLGRAPHICS_EXPORT FBORenderTarget: public RenderTarget
  {
    VL_INSTRUMENT_CLASS(vl::FBORenderTarget, RenderTarget)

    friend class OpenGLContext;

  private:
    FBORenderTarget( const FBORenderTarget& other ): RenderTarget( other ), mHandle( 0 ) {}
    
    void operator=( const FBORenderTarget& ) {}
    
    FBORenderTarget( OpenGLContext* ctx, int w, int h ): RenderTarget( ctx, w, h ), mHandle( 0 )
    {
      VL_DEBUG_SET_OBJECT_NAME()
      setDrawBuffer( RDB_COLOR_ATTACHMENT0 );
      setReadBuffer( RDB_COLOR_ATTACHMENT0 );
    }

  public:
    /** Destructor. */
    ~FBORenderTarget() { if (openglContext()) destroy(); }

    /** 
     * Creates a framebuffer object by calling glGenFramebuffers(). 
     * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glGenFramebuffers.xml
     */
    void create();

    /** 
     * Deletes a framebuffer object by calling glDeleteFramebuffers(). 
     * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glDeleteFramebuffers.xml
     */
    void destroy();

    /** The handle of the framebuffer object as returned by glGenFramebuffers. */
    void setHandle( GLuint handle ) { mHandle = handle; }

    /** The handle of the framebuffer object as returned by glGenFramebuffers. */
    virtual GLuint handle() const { return mHandle; }

    /**
     * Makes the framebuffer the current rendering target calling glBindFramebuffer( GL_FRAMEBUFFER, FBORenderTarget::handle() )
     * and initializes all the previously defined attachment points.
     * \sa http://www.opengl.org/sdk/docs/man3/xhtml/glBindFramebuffer.xml
     */
    virtual void bindFramebuffer( EFrameBufferBind target = FBB_FRAMEBUFFER );

    /** Checks the framebuffer status and returns the value of glCheckFramebufferStatus() */
    GLenum checkFramebufferStatus();

    /** Prints a human readable description of the error code as returned by glCheckFramebufferStatus() */
    void printFramebufferError( GLenum status ) const;

    /** Binds a color attachment to a framebuffer object. */
    void addColorAttachment( EAttachmentPoint attach_point, FBOColorBufferAttachment* attachment );
    
    /** Binds a texture attachment to a framebuffer object. */
    void addTextureAttachment( EAttachmentPoint attach_point, FBOAbstractTextureAttachment* attachment );
    
    /** 
     * Binds a depth attachment to a framebuffer object. 
     * The \p attachment parameter must point to either a FBODepthBufferAttachment or FBODepthStencilBufferAttachment.
     */
    void addDepthAttachment( FBOAbstractAttachment* attachment );
    
    /** 
     * Binds a stencil attachment to a framebuffer object. 
     * The \p attachment parameter must point to either a FBOStencilBufferAttachment or FBODepthStencilBufferAttachment.
     */
    void addStencilAttachment( FBOAbstractAttachment* attachment );
    
    /** Binds a depth-stencil attachment to a framebuffer object. */
    void addDepthStencilAttachment( FBOAbstractAttachment* attachment );
    
    /** Unbinds the given attachments from a framebuffer object. */
    void removeAttachment( FBOAbstractAttachment* attachment );
    
    /** Unbinds the attachment associated to the given attachment point from a framebuffer object. */
    void removeAttachment( EAttachmentPoint attach_point );
    
    /** Unbinds all attachments bound to a framebuffer object. */
    void removeAllAttachments();

    /** A map associating which fbo-attachment belongs to which attachment point in a framebuffer object. */
    const std::map< EAttachmentPoint, ref<FBOAbstractAttachment> >& fboAttachments() const { return mFBOAttachments; }
    
  public:
    std::map< EAttachmentPoint, ref<FBOAbstractAttachment> > mFBOAttachments;
    GLuint mHandle;
  };
}

#endif
