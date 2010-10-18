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

#ifndef BlitFramebuffer_INCLUDE_ONCE
#define BlitFramebuffer_INCLUDE_ONCE

#include <vl/RenderingCallback.hpp>
#include <vl/FramebufferObject.hpp>

namespace vl
{
  /**
   * A RenderingCallback that can be used to copy pixels from a framebuffer to another as
   * described in GL_EXT_framebuffer_blit.
  */
  class BlitFramebuffer: public RenderingCallback
  {
  public:
    BlitFramebuffer()
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      setSrcRect(0,0,640,480);
      setDstRect(0,0,640,480);
      mBufferMask = 0;
      mLinearFilteringEnabled = false;
    }
    virtual const char* className() { return "BlitFramebuffer"; }

    //! Performs the actual pixel copy from the read framebuffer to the draw framebuffer
    void copyPixels()
    {
      if (GLEW_EXT_framebuffer_blit||GLEW_ARB_framebuffer_object)
      {
        // initializes the source and destination FBOs
        VL_CHECK_OGL()
        readFramebuffer()->bindFramebuffer();
        VL_CHECK_OGL()
        drawFramebuffer()->bindFramebuffer();
        VL_CHECK_OGL()

        VL_CHECK( drawFramebuffer()->handle() )
        VL_CHECK( readFramebuffer()->handle() )

        // performs the blit
        VL_glBindFramebuffer( GL_READ_FRAMEBUFFER_EXT, readFramebuffer()->handle() );
        VL_CHECK_OGL()
        VL_glBindFramebuffer( GL_DRAW_FRAMEBUFFER_EXT, drawFramebuffer()->handle() );
        VL_CHECK_OGL()
        VL_glBlitFramebuffer( mSrcRect[0], mSrcRect[1], mSrcRect[2], mSrcRect[3], 
                              mDstRect[0], mDstRect[1], mDstRect[2], mDstRect[3], 
                              mBufferMask,
                              mLinearFilteringEnabled ? GL_LINEAR : GL_NEAREST);
        VL_CHECK_OGL()
      }
    }

    virtual bool renderingCallback(const RenderingAbstract*, ERenderingCallback reason)
    {
      if (reason == RC_PostRendering)
      {
        copyPixels();
        return true;
      }
      else
        return false;
    }

    void setReadFramebuffer(FBORenderTarget* fbo) { mReadFramebuffer = fbo; }
    FBORenderTarget* readFramebuffer() const { return mReadFramebuffer.get(); }

    void setDrawFramebuffer(FBORenderTarget* fbo) { mDrawFramebuffer = fbo; }
    FBORenderTarget* drawFramebuffer() const { return mDrawFramebuffer.get(); }

    void setSrcRect(int x0, int y0, int x1, int y1)
    {
      mSrcRect[0] = x0;
      mSrcRect[1] = y0;
      mSrcRect[2] = x1;
      mSrcRect[3] = y1;
    }

    void setDstRect(int x0, int y0, int x1, int y1)
    {
      mDstRect[0] = x0;
      mDstRect[1] = y0;
      mDstRect[2] = x1;
      mDstRect[3] = y1;
    }

    const int* srcRect() const { return mSrcRect; }
    const int* dstRect() const { return mDstRect; }

    //! takes a bitmask combination of EBufferBits
    void setBufferMask(int buffer_mask) { mBufferMask = buffer_mask; }
    int bufferMask() const { return mBufferMask; }

    void setLinearFilteringEnabled( bool enable_linear_filtering ) { mLinearFilteringEnabled = enable_linear_filtering; }
    bool linearFilteringEnabled() const { return mLinearFilteringEnabled; }

  protected:
    ref<FBORenderTarget> mReadFramebuffer;
    ref<FBORenderTarget> mDrawFramebuffer;
    int mSrcRect[4];
    int mDstRect[4];
    int mBufferMask;
    bool mLinearFilteringEnabled;
  };
};

#endif
