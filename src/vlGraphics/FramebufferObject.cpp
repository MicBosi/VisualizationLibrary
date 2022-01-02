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

#include <vlGraphics/FramebufferObject.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlCore/GlobalSettings.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Log.hpp>

using namespace vl;

namespace
{
  class ScopedFBOBinding
  {
    GLint mPrevFBO;
  public:
    ScopedFBOBinding( FramebufferObject* fbo )
    {
      VL_CHECK( fbo );
      VL_CHECK( fbo->handle() );
      /*if ( !fbo->handle() )
        fbo->create();*/

      // saves current FBO
      mPrevFBO = 0;
      glGetIntegerv( GL_FRAMEBUFFER_BINDING, &mPrevFBO ); VL_CHECK_OGL()

      // binds this FBO
      glBindFramebuffer( GL_FRAMEBUFFER, fbo->handle() ); VL_CHECK_OGL()
    }

    ~ScopedFBOBinding()
    {
      // restore the FBO
      glBindFramebuffer( GL_FRAMEBUFFER, mPrevFBO ); VL_CHECK_OGL()
    }
  };
}
//-----------------------------------------------------------------------------
// FramebufferObject
//-----------------------------------------------------------------------------
bool FramebufferObject::checkDrawBuffers() const
{
  std::map<int, const char*> fbo_attachments;
  
  fbo_attachments[GL_NONE]        = "GL_NONE"; 
  fbo_attachments[GL_BACK_LEFT]   = "GL_BACK_LEFT";
  fbo_attachments[GL_BACK_RIGHT]  = "GL_BACK_RIGHT";
  fbo_attachments[GL_FRONT_LEFT]  = "GL_FRONT_LEFT";
  fbo_attachments[GL_FRONT_RIGHT] = "GL_FRONT_RIGHT";
  fbo_attachments[GL_AUX0] = "GL_AUX0";
  fbo_attachments[GL_AUX1] = "GL_AUX1";
  fbo_attachments[GL_AUX2] = "GL_AUX2";
  fbo_attachments[GL_AUX3] = "GL_AUX3";
  fbo_attachments[GL_COLOR_ATTACHMENT0]  = "GL_COLOR_ATTACHMENT0"; 
  fbo_attachments[GL_COLOR_ATTACHMENT1]  = "GL_COLOR_ATTACHMENT1"; 
  fbo_attachments[GL_COLOR_ATTACHMENT2]  = "GL_COLOR_ATTACHMENT2"; 
  fbo_attachments[GL_COLOR_ATTACHMENT3]  = "GL_COLOR_ATTACHMENT3"; 
  fbo_attachments[GL_COLOR_ATTACHMENT4]  = "GL_COLOR_ATTACHMENT4"; 
  fbo_attachments[GL_COLOR_ATTACHMENT5]  = "GL_COLOR_ATTACHMENT5"; 
  fbo_attachments[GL_COLOR_ATTACHMENT6]  = "GL_COLOR_ATTACHMENT6"; 
  fbo_attachments[GL_COLOR_ATTACHMENT7]  = "GL_COLOR_ATTACHMENT7"; 
  fbo_attachments[GL_COLOR_ATTACHMENT8]  = "GL_COLOR_ATTACHMENT8"; 
  fbo_attachments[GL_COLOR_ATTACHMENT9]  = "GL_COLOR_ATTACHMENT9"; 
  fbo_attachments[GL_COLOR_ATTACHMENT10] = "GL_COLOR_ATTACHMENT10"; 
  fbo_attachments[GL_COLOR_ATTACHMENT11] = "GL_COLOR_ATTACHMENT11"; 
  fbo_attachments[GL_COLOR_ATTACHMENT12] = "GL_COLOR_ATTACHMENT12"; 
  fbo_attachments[GL_COLOR_ATTACHMENT13] = "GL_COLOR_ATTACHMENT13"; 
  fbo_attachments[GL_COLOR_ATTACHMENT14] = "GL_COLOR_ATTACHMENT14"; 
  fbo_attachments[GL_COLOR_ATTACHMENT15] = "GL_COLOR_ATTACHMENT15"; 

  int fbo = 0;
  if (Has_GL_EXT_framebuffer_object||Has_GL_ARB_framebuffer_object)
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &fbo);

  if (fbo)
  {
    std::set<GLenum> legal;
    legal.insert(GL_NONE);
    legal.insert(GL_COLOR_ATTACHMENT0);
    legal.insert(GL_COLOR_ATTACHMENT1);
    legal.insert(GL_COLOR_ATTACHMENT2);
    legal.insert(GL_COLOR_ATTACHMENT3);
    legal.insert(GL_COLOR_ATTACHMENT4);
    legal.insert(GL_COLOR_ATTACHMENT5);
    legal.insert(GL_COLOR_ATTACHMENT6);
    legal.insert(GL_COLOR_ATTACHMENT7);
    legal.insert(GL_COLOR_ATTACHMENT8);
    legal.insert(GL_COLOR_ATTACHMENT9);
    legal.insert(GL_COLOR_ATTACHMENT10);
    legal.insert(GL_COLOR_ATTACHMENT11);
    legal.insert(GL_COLOR_ATTACHMENT12);
    legal.insert(GL_COLOR_ATTACHMENT13);
    legal.insert(GL_COLOR_ATTACHMENT14);
    legal.insert(GL_COLOR_ATTACHMENT15);
    for(unsigned i=0; i<mDrawBuffers.size(); ++i)
    {
      if(legal.find(mDrawBuffers[i]) == legal.end())
      {
        Log::error(Say("FBO bound but FramebufferObject::setDrawBuffers() called with non FBO compatible draw buffer '%s'.\n") << fbo_attachments[mDrawBuffers[i]]);
        return false;
      }
    }
  }
  else
  {
    std::set<GLenum> legal;
    legal.insert(GL_NONE);
    legal.insert(GL_BACK_LEFT);
    legal.insert(GL_BACK_RIGHT);
    legal.insert(GL_FRONT_LEFT);
    legal.insert(GL_FRONT_RIGHT);
    legal.insert(GL_AUX0);
    legal.insert(GL_AUX1);
    legal.insert(GL_AUX2);
    legal.insert(GL_AUX3);
    for(unsigned i=0; i<mDrawBuffers.size(); ++i)
    {
      if(legal.find(mDrawBuffers[i]) == legal.end())
      {
        Log::error(Say("FBO not bound or not supported but FramebufferObject::setDrawBuffers() called with FBO specific draw buffer '%s'.\n") << fbo_attachments[mDrawBuffers[i]]);
        return false;
      }
    }
  }
  return true;
}
//-----------------------------------------------------------------------------
void FramebufferObject::bindDrawBuffers() const
{
  VL_CHECK_OGL()

  VL_CHECK(!mDrawBuffers.empty())

  #ifndef NDEBUG
    checkDrawBuffers();
  #endif

  if (mDrawBuffers.size() > 1 && (Has_GL_ARB_draw_buffers||Has_GL_Version_2_0))
  {
    glDrawBuffers( (GLsizei)mDrawBuffers.size(), (const GLenum*)&mDrawBuffers[0] );
    VL_CHECK_OGL() // If you are using RDB_BACK_LEFT/RIGHT make sure sure you have a double buffered gl context.
                   // Otherwise use FramebufferObject::setDrawBuffer(RDB_FRONT_LEFT).
  }
  else
  {
    glDrawBuffer( mDrawBuffers[0] );
    VL_CHECK_OGL() // If you are using RDB_BACK_LEFT/RIGHT make sure sure you have a double buffered gl context.
                   // Otherwise use FramebufferObject::setDrawBuffer(RDB_FRONT_LEFT).
    if ( mDrawBuffers.size() > 1 )
    {
      Log::error( "FramebufferObject::bindDrawBuffers() error:\nglDrawBuffers() not supported by the current OpenGL driver. GL_ARB_draw_buffers or OpenGL 2.0 required.\n" );
    }
  }
}
//-----------------------------------------------------------------------------
void FramebufferObject::bindReadBuffer()
{
  VL_CHECK_OGL();

  glReadBuffer( readBuffer() );

  VL_CHECK_OGL();
}
//-----------------------------------------------------------------------------
void FramebufferObject::createFBO()
{
  VL_CHECK_OGL();
  VL_CHECK(openglContext());
  openglContext()->makeCurrent(); VL_CHECK_OGL();

  if ( !mHandle )
  {
    glGenFramebuffers( 1, ( unsigned int* )&mHandle ); VL_CHECK_OGL();
  }
  VL_CHECK( mHandle )
}
//-----------------------------------------------------------------------------
void FramebufferObject::deleteFBO()
{
  VL_CHECK_OGL();
  VL_CHECK(openglContext());
  openglContext()->makeCurrent(); VL_CHECK_OGL();

  removeAllAttachments();
  if ( handle() )
  {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 ); VL_CHECK_OGL();
    glDeleteFramebuffers( 1, &mHandle ); VL_CHECK_OGL();
    mHandle = 0;
  }
  setWidth( 0 );
  setHeight( 0 );
}
//-----------------------------------------------------------------------------
void FramebufferObject::bindFramebuffer( EFramebufferBind target )
{
  VL_CHECK_OGL();
  VL_CHECK(openglContext());
  openglContext()->makeCurrent(); VL_CHECK_OGL();

  if ( externallyManaged() ) {
    return;
  }

  if ( !Has_FBO )
  {
    Log::error( "FramebufferObject::bindFramebuffer(): framebuffer object not supported.\n" );
    return;
  }

  if ( width() <= 0 || height() <= 0 )
  {
    Log::error( Say( "FramebufferObject::bindFramebuffer() called with illegal dimensions: width = %n, height = %n\n" ) << width() << height() );
    VL_TRAP()
  }

  // This is an error only if not default framebuffer
  if ( handle() != 0 && mFBOAttachments.empty() )
  {
    Log::error( "FramebufferObject::bindFramebuffer() called with no attachment points!\n" );
    VL_TRAP()
  }

  /* handle() == 0: default framebuffer
  if ( !handle() )
  {
    Log::error( "FramebufferObject::bindFramebuffer() called but handle() == NULL!\n" );
    VL_TRAP()
  }
  */

   glBindFramebuffer( target, handle() ); VL_CHECK_OGL()

#if defined(VL_OPENGL)
  // bind draw buffers
  if ( target == FBB_FRAMEBUFFER || target == FBB_DRAW_FRAMEBUFFER ) 
  {
    bindDrawBuffers();
  }

  // bind read buffer
  if ( target == FBB_FRAMEBUFFER || target == FBB_READ_FRAMEBUFFER ) 
  {
    bindReadBuffer();
  }
#endif

  #ifndef NDEBUG
  if ( handle() )
  {
    GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER ); VL_CHECK_OGL()
    if ( status != GL_FRAMEBUFFER_COMPLETE )
    {
      printFramebufferError( status );
      glBindFramebuffer( GL_FRAMEBUFFER, 0 ); VL_CHECK_OGL()
    }
  }
  #endif
}
//-----------------------------------------------------------------------------
//! Returns 0 if no FBO support is found otherwise returns the value obtained by glCheckFramebufferStatus()
GLenum FramebufferObject::checkFramebufferStatus()
{
  VL_CHECK_OGL();
  VL_CHECK(openglContext());
  openglContext()->makeCurrent(); VL_CHECK_OGL();

  if ( !Has_FBO )
  {
    Log::error( "FramebufferObject::checkFramebufferStatus(): framebuffer object not supported.\n" );
    return 0;
  }

  if ( width() <= 0 || height() <= 0 )
  {
    Log::error( Say( "FramebufferObject::checkFramebufferStatus() called with illegal dimensions: width = %n, height = %n\n" ) << width() << height() );
    return 0;
  }

  if ( mFBOAttachments.empty() )
  {
    Log::error( "FramebufferObject::checkFramebufferStatus() called with no attachment points!\n" );
    return 0;
  }

  if ( !handle() )
  {
    Log::error( "FramebufferObject::checkFramebufferStatus() called but handle() == NULL!\n" );
    return 0;
  }

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( this );

  // checks error
  GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER ); VL_CHECK_OGL()

  // restore the FBO
  if ( globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_NORMAL )
    printFramebufferError( status );

  VL_CHECK( status == GL_FRAMEBUFFER_COMPLETE )

  return status;
}
//-----------------------------------------------------------------------------
void FramebufferObject::printFramebufferError( GLenum status ) const
{
  switch( status )
  {
  case GL_FRAMEBUFFER_COMPLETE:
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_UNSUPPORTED:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_UNSUPPORTED\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_ARB\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_LAYER_COUNT_ARB\n" ); VL_TRAP()
    break;
  case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
    Log::bug( "FramebufferObject::activate(): GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE\n" ); VL_TRAP()
    break;
  }
}
//-----------------------------------------------------------------------------
void FramebufferObject::addColorAttachment( EAttachmentPoint attach_point, FBOColorBufferAttachment* attachment )
{
  VL_CHECK( attach_point >= AP_COLOR_ATTACHMENT0 && attach_point <= AP_COLOR_ATTACHMENT15 );
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  removeAttachment( attach_point );
  mFBOAttachments[attach_point] = attachment;
  attachment->mFramebufferObjects.insert( this );
  attachment->bindAttachment( this, attach_point );
}
//-----------------------------------------------------------------------------
void FramebufferObject::addTextureAttachment( EAttachmentPoint attach_point, FBOAbstractTextureAttachment* attachment )
{
  VL_CHECK( attach_point >= AP_COLOR_ATTACHMENT0 && attach_point <= AP_COLOR_ATTACHMENT15 );
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  removeAttachment( attach_point );
  mFBOAttachments[attach_point] = attachment;
  attachment->mFramebufferObjects.insert( this );
  attachment->bindAttachment( this, attach_point );
}
//-----------------------------------------------------------------------------
void FramebufferObject::addDepthAttachment( FBOAbstractAttachment* attachment )
{
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  removeAttachment( AP_DEPTH_ATTACHMENT );
  mFBOAttachments[AP_DEPTH_ATTACHMENT] = attachment;
  attachment->mFramebufferObjects.insert( this );
  attachment->bindAttachment( this, AP_DEPTH_ATTACHMENT );
}
//-----------------------------------------------------------------------------
void FramebufferObject::addStencilAttachment( FBOAbstractAttachment* attachment )
{
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  removeAttachment( AP_STENCIL_ATTACHMENT );
  mFBOAttachments[AP_STENCIL_ATTACHMENT] = attachment;
  attachment->mFramebufferObjects.insert( this );
  attachment->bindAttachment( this, AP_STENCIL_ATTACHMENT );
}
//-----------------------------------------------------------------------------
void FramebufferObject::addDepthStencilAttachment( FBOAbstractAttachment* attachment )
{
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  removeAttachment( AP_DEPTH_STENCIL_ATTACHMENT );
  mFBOAttachments[AP_DEPTH_STENCIL_ATTACHMENT] = attachment;
  attachment->mFramebufferObjects.insert( this );
  attachment->bindAttachment( this, AP_DEPTH_STENCIL_ATTACHMENT );
}
//-----------------------------------------------------------------------------
void FramebufferObject::removeAttachment( FBOAbstractAttachment* attachment )
{
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  // collect for all the attachment points
  std::vector<EAttachmentPoint> attachment_points;
  std::map< EAttachmentPoint, ref<FBOAbstractAttachment> >::iterator it = mFBOAttachments.begin();
  for( ; it != mFBOAttachments.end(); ++it )
    if ( it->second == attachment )
      attachment_points.push_back( it->first );

  // remove it from all the attachment points
  for( unsigned i=0; i<attachment_points.size(); ++i )
    removeAttachment( attachment_points[i] );
}
//-----------------------------------------------------------------------------
void FramebufferObject::removeAttachment( EAttachmentPoint attach_point )
{
  VL_CHECK_OGL();
  VL_CHECK(openglContext());
  openglContext()->makeCurrent(); VL_CHECK_OGL();

  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  if ( handle() )
  {
    // save current fbo
    int fbo = -1;
    glGetIntegerv( GL_FRAMEBUFFER_BINDING, &fbo ); VL_CHECK_OGL()
    // bind this fbo
    glBindFramebuffer( GL_FRAMEBUFFER, handle() ); VL_CHECK_OGL()
    // detach should work for any kind of buffer and texture
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, attach_point, GL_RENDERBUFFER, 0 ); VL_CHECK_OGL()
    // restore fbo
    glBindFramebuffer( GL_FRAMEBUFFER, fbo ); VL_CHECK_OGL()
  }
  // remove FramebufferObject from FBOAbstractAttachment
  FBOAbstractAttachment* fbo_attachment = /* mFBOAttachments.find( attachment ) != mFBOAttachments.end() ? */ mFBOAttachments[attach_point].get() /* : NULL */;
  if ( fbo_attachment )
    fbo_attachment->mFramebufferObjects.erase( this );
  mFBOAttachments.erase( attach_point );
}
//-----------------------------------------------------------------------------
void FramebufferObject::removeAllAttachments()
{
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  // look for all the attachment points
  std::vector<EAttachmentPoint> attachment_points;
  std::map< EAttachmentPoint, ref<FBOAbstractAttachment> >::iterator it = mFBOAttachments.begin();
  for( ; it != mFBOAttachments.end(); ++it )
    attachment_points.push_back( it->first );

  // remove attachment points
  for( unsigned i=0; i<attachment_points.size(); ++i )
    removeAttachment( attachment_points[i] );
}
//-----------------------------------------------------------------------------
void FBOTexture1DAttachment::bindAttachment( FramebufferObject* fbo, EAttachmentPoint attach_point )
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  VL_CHECK( texture() )
  VL_CHECK( texture()->handle() )
  VL_CHECK( texture()->dimension() == GL_TEXTURE_1D )
  VL_CHECK( fbo->width() == texture()->width()  );

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( fbo );

  glFramebufferTexture1D( GL_FRAMEBUFFER, attach_point, GL_TEXTURE_1D, texture()->handle(), mipmapLevel() ); VL_CHECK_OGL()

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  glBindTexture( texture()->dimension(), texture()->handle() ); VL_CHECK_OGL()
  glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR ); VL_CHECK_OGL()
  glBindTexture( texture()->dimension(), 0 ); VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void FBOTexture2DAttachment::bindAttachment( FramebufferObject* fbo, EAttachmentPoint attach_point )
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  VL_CHECK( texture() )
  VL_CHECK( texture()->handle() )
  // VL_CHECK( texture()->dimension() == GL_TEXTURE_2D )
  VL_CHECK( fbo->width()  <= texture()->width()  );
  VL_CHECK( fbo->height() <= texture()->height() );

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( fbo );

  int target = texture()->dimension() == TD_TEXTURE_CUBE_MAP ? ( int )textureTarget() : texture()->dimension();
  #ifndef NDEBUG
    if( !( texture()->dimension() == TD_TEXTURE_CUBE_MAP || ( int )textureTarget() == ( int )texture()->dimension() ) )
    {
      Log::bug( "FBOTexture2DAttachment::init(): textureTarget() doens't match texture()->dimension().\n" );
    }
  #endif

  glFramebufferTexture2D( GL_FRAMEBUFFER, attach_point, target, texture()->handle(), mipmapLevel() ); VL_CHECK_OGL()

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  if ( texture()->dimension() != TD_TEXTURE_2D_MULTISAMPLE )
  {
    glBindTexture( texture()->dimension(), texture()->handle() ); VL_CHECK_OGL()
    glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR ); VL_CHECK_OGL()
    glBindTexture( texture()->dimension(), 0 ); VL_CHECK_OGL()
  }
}
//-----------------------------------------------------------------------------
void FBOTextureAttachment::bindAttachment( FramebufferObject* fbo, EAttachmentPoint attach_point )
{
  VL_CHECK_OGL()
  VL_CHECK( Has_GL_NV_geometry_shader4||Has_GL_EXT_geometry_shader4||Has_GL_ARB_geometry_shader4||Has_GL_Version_3_2||Has_GL_Version_4_0 )
  VL_CHECK( texture() )
  VL_CHECK( texture()->handle() )

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( fbo );

  glFramebufferTexture( GL_FRAMEBUFFER, attach_point, texture()->handle(), mipmapLevel() ); VL_CHECK_OGL()

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  if ( texture()->dimension() != TD_TEXTURE_2D_MULTISAMPLE )
  {
    glBindTexture( texture()->dimension(), texture()->handle() ); VL_CHECK_OGL()
    glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR ); VL_CHECK_OGL()
    glBindTexture( texture()->dimension(), 0 ); VL_CHECK_OGL()
  }
}
//-----------------------------------------------------------------------------
void FBOTexture3DAttachment::bindAttachment( FramebufferObject* fbo, EAttachmentPoint attach_point )
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  VL_CHECK( texture() )
  VL_CHECK( texture()->handle() )
  VL_CHECK( fbo->width()  <= texture()->width()  );
  VL_CHECK( fbo->height() <= texture()->height() );
  VL_CHECK( layer() <= texture()->depth() );
  VL_CHECK( texture()->dimension() == GL_TEXTURE_3D )

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( fbo );

  glFramebufferTexture3D( GL_FRAMEBUFFER, attach_point, texture()->dimension(), texture()->handle(), mipmapLevel(), layer() ); VL_CHECK_OGL()

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  if ( texture()->dimension() != TD_TEXTURE_2D_MULTISAMPLE_ARRAY )
  {
    glBindTexture( texture()->dimension(), texture()->handle() ); VL_CHECK_OGL()
    glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR ); VL_CHECK_OGL()
    glBindTexture( texture()->dimension(), 0 ); VL_CHECK_OGL()
  }
}
//-----------------------------------------------------------------------------
void FBOTextureLayerAttachment::bindAttachment( FramebufferObject* fbo, EAttachmentPoint attach_point )
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  VL_CHECK( Has_GL_EXT_texture_array||Has_GL_NV_geometry_shader4||Has_GL_ARB_geometry_shader4||Has_GL_EXT_geometry_shader4||Has_GL_Version_3_2||Has_GL_Version_4_0 )
  if( !Has_FBO )
    return;
  if( !( Has_GL_EXT_texture_array||Has_GL_NV_geometry_shader4||Has_GL_ARB_geometry_shader4||Has_GL_EXT_geometry_shader4||Has_GL_Version_3_2||Has_GL_Version_4_0 ) )
    return;
  VL_CHECK( texture() )
  VL_CHECK( texture()->handle() )
  VL_CHECK( texture()->dimension() == GL_TEXTURE_2D_ARRAY || texture()->dimension() == GL_TEXTURE_1D_ARRAY )
  VL_CHECK( fbo->width()  <= texture()->width()  );
  VL_CHECK( fbo->height() <= texture()->height() );
  // VL_CHECK( layer() <= texture()->depth() );

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( fbo );

  glFramebufferTextureLayer( GL_FRAMEBUFFER, attach_point, texture()->handle(), mipmapLevel(), layer() ); VL_CHECK_OGL()

  // needed to make non-mipmapped textures work with FBO, see framebuffer_object.txt line 442
  if ( texture()->dimension() != TD_TEXTURE_2D_MULTISAMPLE_ARRAY )
  {
    glBindTexture( texture()->dimension(), texture()->handle() ); VL_CHECK_OGL()
    glTexParameteri( texture()->dimension(), GL_TEXTURE_MIN_FILTER, GL_LINEAR ); VL_CHECK_OGL()
    glBindTexture( texture()->dimension(), 0 ); VL_CHECK_OGL()
  }
}
//-----------------------------------------------------------------------------
void FBOAbstractAttachment::unbindFromAllFBO()
{
  std::set< ref<FramebufferObject> > fbos = fboFramebuffers();
  for( std::set< ref<FramebufferObject> >::iterator it = fbos.begin(); it != fbos.end(); ++it )
    it->get_writable()->removeAttachment( this );
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::createRenderBuffer()
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;
  if ( !mHandle )
  {
    glGenRenderbuffers( 1, &mHandle ); VL_CHECK_OGL()
    mReallocateRenderbuffer = true;
  }
  VL_CHECK( mHandle )
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::deleteRenderBuffer()
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;

  unbindFromAllFBO();

  mWidth  = 0;
  mHeight = 0;
  if ( mHandle )
  {
    glDeleteRenderbuffers( 1, &mHandle ); VL_CHECK_OGL()
    mHandle = 0;
    mReallocateRenderbuffer = true;
  }
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::initStorage( int w, int h, int samp )
{
  VL_CHECK_OGL()
  VL_CHECK( handle() );
  VL_CHECK( w>0 && h>0 );
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;

  if ( w != width() || h != height() || samp != samples() || mReallocateRenderbuffer )
  {
    mWidth  = w;
    mHeight = h;
    mSamples = samp;
    glBindRenderbuffer( GL_RENDERBUFFER, handle() ); VL_CHECK_OGL()
    if ( Has_FBO_Multisample )
    {
      glRenderbufferStorageMultisample( GL_RENDERBUFFER, samples(), internalType(), width(), height() ); VL_CHECK_OGL()
    }
    else
    {
      VL_CHECK(samples() == 0)
      if (samples())
        Log::error("FBORenderbufferAttachment::initStorage() requesting multisampling storage but current OpenGL implementation does not support it!\n");
      glRenderbufferStorage( GL_RENDERBUFFER, internalType(), width(), height() ); VL_CHECK_OGL()
    }
    glBindRenderbuffer( GL_RENDERBUFFER, 0 ); VL_CHECK_OGL()
    mReallocateRenderbuffer = false;
  }
}
//-----------------------------------------------------------------------------
void FBORenderbufferAttachment::bindAttachment( FramebufferObject* fbo, EAttachmentPoint attach_point )
{
  VL_CHECK_OGL()
  VL_CHECK( Has_FBO )
  if( !Has_FBO )
    return;

  if (!handle())
    createRenderBuffer();

  // binds the FBO for this function call
  ScopedFBOBinding fbo_bind( fbo );

  // choose the maximum dimension
  int actual_w = width()  == 0 ? fbo->width()  : width();
  int actual_h = height() == 0 ? fbo->height() : height();
  VL_CHECK( actual_w >= fbo->width() );
  VL_CHECK( actual_h >= fbo->height() );
  initStorage( actual_w, actual_h, samples() );

  // attach the renderbuffer to the framebuffer's attachment point
  glFramebufferRenderbuffer( GL_FRAMEBUFFER, attach_point, GL_RENDERBUFFER, handle() ); VL_CHECK_OGL()
}
