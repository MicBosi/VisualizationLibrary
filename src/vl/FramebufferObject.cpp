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

#include <vl/VisualizationLibrary.hpp>
#include <vl/FramebufferObject.hpp>
#include <vl/Say.hpp>
#include <vl/Log.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
// FBORenderTarget
//-----------------------------------------------------------------------------
void FBORenderTarget::bindFramebuffer()
{
  VL_CHECK_OGL()

  if (!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
  {
    Log::error("FBORenderTarget::bindFramebuffer(): framebuffer object not supported.\n");
    return;
  }

  #ifndef NDEBUG
    if ( width() <= 0 || height() <= 0 )
    {
      Log::error(Say("FBORenderTarget::bindFramebuffer() called with illegal dimensions: width = %n, height = %n\n") << width() << height() );
      VL_TRAP()
    }
  #endif

  if ( width() <= 0 || height() <= 0 )
    return;

  if (!handle())
    create();

  VL_glBindFramebuffer(GL_FRAMEBUFFER, handle()); VL_CHECK_OGL()

  // init FBO attachments
  std::map< EAttachmentPoint, ref<FBOAttachmentAbstract> >::const_iterator it = mFBOAttachments.begin();
  for(; it != mFBOAttachments.end(); ++it)
    if (it->second)
      it->second->bindAttachment( width(), height(), it->first );

  #ifndef NDEBUG
    GLenum status = VL_glCheckFramebufferStatus(GL_FRAMEBUFFER); VL_CHECK_OGL()
    if ( status != GL_FRAMEBUFFER_COMPLETE )
    {
      VL_glBindFramebuffer(GL_FRAMEBUFFER, 0); VL_CHECK_OGL()
    }
    printFramebufferError(status);
  #endif
}
//-----------------------------------------------------------------------------
//! Returns 0 if no FBO support is found otherwise returns the value obtained
//! from VL_glCheckFramebufferStatus()
GLenum FBORenderTarget::checkFramebufferStatus()
{
  VL_CHECK_OGL()

  if (!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
  {
    Log::error("FBORenderTarget::checkFramebufferStatus(): framebuffer object not supported.\n");
    return GL_FRAMEBUFFER_UNSUPPORTED;
  }

  #ifndef NDEBUG
    if ( width() <= 0 || height() <= 0 )
    {
      Log::error(Say("FBORenderTarget::activate() called with illegal dimensions: width = %n, height = %n\n") << width() << height() );
      VL_TRAP()
    }
  #endif

  if ( width() <= 0 || height() <= 0 )
    return 0;

  if (!handle())
    create();

  // saves current FBO
  int fbo = 0;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);
  VL_glBindFramebuffer(GL_FRAMEBUFFER, handle()); VL_CHECK_OGL()

  // init FBO attachments
  std::map< EAttachmentPoint, ref<FBOAttachmentAbstract> >::iterator it = mFBOAttachments.begin();
  for(; it != mFBOAttachments.end(); ++it)
    if (it->second)
      it->second->bindAttachment( width(), height(), it->first );

  GLenum status = VL_glCheckFramebufferStatus(GL_FRAMEBUFFER); VL_CHECK_OGL()
  // restore the FBO
  VL_glBindFramebuffer(GL_FRAMEBUFFER, fbo); VL_CHECK_OGL()

  #ifndef NDEBUG
    printFramebufferError(status);
  #endif

  return status;
}
//-----------------------------------------------------------------------------
void FBORenderTarget::printFramebufferError(GLenum status) const
{
  switch(status)
  {
  case GL_FRAMEBUFFER_COMPLETE:
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_UNSUPPORTED\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB\n"); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
    Log::error("FBORenderTarget::activate(): GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n"); VL_TRAP()
    break;
  }
  VL_CHECK( status == GL_FRAMEBUFFER_COMPLETE )
}
//-----------------------------------------------------------------------------
void FBORenderTarget::addColorAttachment(EAttachmentPoint color_attachment, FBOAttachmentAbstract* attachment)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  removeAttachment(color_attachment);
  mFBOAttachments[color_attachment] = attachment;
  attachment->mFBORenderTargets.insert(this);
}
//-----------------------------------------------------------------------------
void FBORenderTarget::addTextureAttachment(EAttachmentPoint color_attachment, FBOAttachmentAbstract* attachment)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  removeAttachment(color_attachment);
  mFBOAttachments[color_attachment] = attachment;
  attachment->mFBORenderTargets.insert(this);
}
//-----------------------------------------------------------------------------
void FBORenderTarget::addDepthAttachment(FBOAttachmentAbstract* attachment)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  removeAttachment(AP_DEPTH_ATTACHMENT);
  mFBOAttachments[AP_DEPTH_ATTACHMENT] = attachment;
  attachment->mFBORenderTargets.insert(this);
}
//-----------------------------------------------------------------------------
void FBORenderTarget::addStencilAttachment(FBOAttachmentAbstract* attachment)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  removeAttachment(AP_STENCIL_ATTACHMENT);
  mFBOAttachments[AP_STENCIL_ATTACHMENT] = attachment;
  attachment->mFBORenderTargets.insert(this);
}
//-----------------------------------------------------------------------------
void FBORenderTarget::removeAttachment(FBOAttachmentAbstract* attachment)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  // collect for all the attachment points
  std::vector<EAttachmentPoint> attachment_points;
  std::map< EAttachmentPoint, ref<FBOAttachmentAbstract> >::iterator it = mFBOAttachments.begin();
  for(; it != mFBOAttachments.end(); ++it)
    if (it->second == attachment)
      attachment_points.push_back(it->first);

  // remove it from all the attachment points
  for(unsigned i=0; i<attachment_points.size(); ++i)
    removeAttachment( attachment_points[i] );
}
//-----------------------------------------------------------------------------
void FBORenderTarget::removeAttachment(EAttachmentPoint attach_point)
{
  VL_CHECK(vl::VisualizationLibrary::initialized())

  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  if (handle())
  {
    // save current fbo
    int fbo = -1;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo); VL_CHECK_OGL()
    // bind this fbo
    VL_glBindFramebuffer(GL_FRAMEBUFFER, handle()); VL_CHECK_OGL()
    // detach should work for any kind of buffer and texture
    VL_glFramebufferRenderbuffer( GL_FRAMEBUFFER, attach_point, GL_RENDERBUFFER, 0 ); VL_CHECK_OGL()
    // restore fbo
    VL_glBindFramebuffer(GL_FRAMEBUFFER, fbo); VL_CHECK_OGL()
  }
  // remove FBORenderTarget from FBOAttachmentAbstract
  FBOAttachmentAbstract* fbo_attachment = /*mFBOAttachments.find(attachment) != mFBOAttachments.end() ? */mFBOAttachments[attach_point].get()/* : NULL*/;
  if (fbo_attachment)
    fbo_attachment->mFBORenderTargets.erase( this );
  mFBOAttachments.erase(attach_point);
}
//-----------------------------------------------------------------------------
void FBORenderTarget::removeAllAttachments()
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  // look for all the attachment points
  std::vector<EAttachmentPoint> attachment_points;
  std::map< EAttachmentPoint, ref<FBOAttachmentAbstract> >::iterator it = mFBOAttachments.begin();
  for(; it != mFBOAttachments.end(); ++it)
    attachment_points.push_back(it->first);

  // remove attachment points
  for(unsigned i=0; i<attachment_points.size(); ++i)
    removeAttachment( attachment_points[i] );
}
//-----------------------------------------------------------------------------
void FBOTexture1DAttachment::bindAttachment(int w, int h, EAttachmentPoint attach_point)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  VL_CHECK(texture())
  VL_CHECK(texture()->handle())
  VL_CHECK(texture()->dimension() == GL_TEXTURE_1D)
  VL_CHECK( w == texture()->width()  );
  VL_CHECK( h == 1 );

  VL_glFramebufferTexture1D(GL_FRAMEBUFFER, attach_point, GL_TEXTURE_1D, texture()->handle(), level() ); VL_CHECK_OGL()

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  glBindTexture( texture()->dimension(), texture()->handle() );
  glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glBindTexture( texture()->dimension(), 0 );
}
//-----------------------------------------------------------------------------
void FBOTexture2DAttachment::bindAttachment(int w, int h, EAttachmentPoint attach_point)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  VL_CHECK_OGL()
  VL_CHECK(texture())
  VL_CHECK(texture()->handle())
  VL_CHECK(texture()->dimension() == GL_TEXTURE_2D)
  VL_CHECK( w == texture()->width()  );
  VL_CHECK( h == texture()->height() );

  int target = texture()->dimension() == TD_TEXTURE_CUBE_MAP ? (int)textureTarget() : texture()->dimension();
  #ifndef NDEBUG
    if( !(texture()->dimension() == TD_TEXTURE_CUBE_MAP || (int)textureTarget() == (int)texture()->dimension()) )
    {
      Log::bug("FBOTexture2DAttachment::init(): textureTarget() doens't match texture()->dimension().\n");
    }
  #endif

  VL_glFramebufferTexture2D( GL_FRAMEBUFFER, attach_point, target, texture()->handle(), level() );

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  glBindTexture( texture()->dimension(), texture()->handle() );
  glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glBindTexture( texture()->dimension(), 0 );

  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void FBOTextureAttachment::bindAttachment(int /*w*/, int /*h*/, EAttachmentPoint attach_point)
{
  VL_WARN(GLEW_NV_geometry_shader4||GLEW_ARB_geometry_shader4)
  VL_CHECK_OGL()
  VL_CHECK(texture())
  VL_CHECK(texture()->handle())

  VL_glFramebufferTexture( GL_FRAMEBUFFER, attach_point, texture()->handle(), level() );

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  glBindTexture( texture()->dimension(), texture()->handle() );
  glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glBindTexture( texture()->dimension(), 0 );

  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void FBOTexture3DAttachment::bindAttachment(int w, int h, EAttachmentPoint attach_point)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  VL_CHECK_OGL()
  VL_CHECK(texture())
  VL_CHECK(texture()->handle())
  VL_CHECK( w == texture()->width()  );
  VL_CHECK( h == texture()->height() );
  VL_CHECK( layer() <= texture()->depth() );
  VL_CHECK( texture()->dimension() == GL_TEXTURE_3D )

  VL_glFramebufferTexture3D( GL_FRAMEBUFFER, attach_point, texture()->dimension(), texture()->handle(), mipmapLevel(), layer() );

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  glBindTexture(texture()->dimension(), texture()->handle());
  glTexParameteri(texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(texture()->dimension(), 0);

  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void FBOTextureLayerAttachment::bindAttachment(int /*w*/, int /*h*/, EAttachmentPoint attach_point)
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  VL_WARN(GLEW_EXT_texture_array||GLEW_NV_geometry_shader4||GLEW_ARB_geometry_shader4||GLEW_EXT_geometry_shader4)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  if(!(GLEW_EXT_texture_array||GLEW_NV_geometry_shader4||GLEW_ARB_geometry_shader4||GLEW_EXT_geometry_shader4))
    return;
  VL_CHECK_OGL()
  VL_CHECK( GLEW_EXT_texture_array )
  VL_CHECK(texture())
  VL_CHECK(texture()->handle())
  VL_CHECK( texture()->dimension() == GL_TEXTURE_2D_ARRAY || texture()->dimension() == GL_TEXTURE_1D_ARRAY )
  // VL_CHECK( w == texture()->width()  );
  // VL_CHECK( h == texture()->height() );
  // VL_CHECK( layer() <= texture()->depth() );

  VL_glFramebufferTextureLayer(GL_FRAMEBUFFER, attach_point, texture()->handle(), mipmapLevel(), layer() );

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  glBindTexture(texture()->dimension(), texture()->handle());
  glTexParameteri(texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glBindTexture(texture()->dimension(), 0);

  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void FBOAttachmentAbstract::destroy()
{
  std::set< ref<FBORenderTarget> > fbos = fboRenderTargets();
  for(std::set< ref<FBORenderTarget> >::iterator it = fbos.begin(); it != fbos.end(); ++it)
    it->get()->removeAttachment(this);
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::create()
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  if ( !mHandle )
  {
    VL_glGenRenderbuffers(1, &mHandle); VL_CHECK_OGL()
  }
  VL_CHECK(mHandle)
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::destroy()
{
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  FBOAttachmentAbstract::destroy();
  mWidth  = 0;
  mHeight = 0;
  if (mHandle)
  {
    VL_glDeleteRenderbuffers(1, &mHandle); VL_CHECK_OGL()
    mHandle = 0;
  }
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::initStorage(int w, int h)
{
  VL_CHECK(handle());
  VL_CHECK_OGL()
  if (w == 0 || h == 0)
    destroy();

  if ( w != width() || h != height() || mReallocateRenderbuffer )
  {
    mWidth  = w;
    mHeight = h;
    VL_glBindRenderbuffer(GL_RENDERBUFFER, handle());
    if (GLEW_VERSION_3_0||GLEW_ARB_framebuffer_object||GLEW_EXT_framebuffer_multisample)
      VL_glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples(), internalType(), width(), height());
    else
      VL_glRenderbufferStorage(GL_RENDERBUFFER, internalType(), width(), height());
    VL_glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }
  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::internalBindAttachment(int w, int h, int attach_point)
{
  VL_CHECK_OGL()
  VL_WARN(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0)
  if(!(GLEW_EXT_framebuffer_object||GLEW_ARB_framebuffer_object||GLEW_VERSION_3_0))
    return;
  VL_CHECK(w)
  VL_CHECK(h)
  if (!handle())
    create();
  if ( mReallocateRenderbuffer )
  {
    int actual_w = width()  ? width()  : w;
    int actual_h = height() ? height() : h;
    initStorage(actual_w, actual_h);
    mReallocateRenderbuffer = false;
  }
  VL_glFramebufferRenderbuffer( GL_FRAMEBUFFER, attach_point, GL_RENDERBUFFER, handle() );
  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
