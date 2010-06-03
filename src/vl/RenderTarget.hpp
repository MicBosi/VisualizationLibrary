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

#ifndef RenderTarget_INCLUDE_ONCE
#define RenderTarget_INCLUDE_ONCE

#include <vl/vlnamespace.hpp>
#include <vl/Object.hpp>
#include <vector>

namespace vl
{
  class OpenGLContext;
  //-----------------------------------------------------------------------------
  // RenderTarget
  //-----------------------------------------------------------------------------
  /** The RenderTarget class defines an abstract 'surface' where OpenGL can render into.
   * \sa
   * OpenGLContext::renderTarget() and FBORenderTarget
  */
  class RenderTarget: public Object
  {
    friend class OpenGLContext;
  protected:
    virtual const char* className() { return "RenderTarget"; }
    RenderTarget(OpenGLContext* ctx, int w, int h): mWidth(w), mHeight(h), mOpenGLContext(ctx)
    {
      #ifndef NDEBUG
        mName = className();
      #endif
      setDrawBuffers(RDB_BACK_LEFT);
    }

  public:
    OpenGLContext* openglContext() { return mOpenGLContext; }
    const OpenGLContext* openglContext() const { return mOpenGLContext; }

    int width() const { return mWidth; }
    int height() const { return mHeight; }
    void setWidth(int width) { mWidth = width; }
    void setHeight(int height) { mHeight = height; }

    //! Activates the RenderTarget by calling bindFramebuffer() and setDrawBuffers()
    void activate()
    {
      bindFramebuffer();
      bindDrawBuffers();
    }

    virtual void bindFramebuffer()
    {
      VL_CHECK_OGL()
      // the base render target is the framebuffer 0, that is, the normal OpenGL buffers
      VL_glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
      VL_CHECK_OGL()
    }

    void bindDrawBuffers() const;

    void checkDrawBuffers() const;

    void setDrawBuffers(EReadDrawBuffer render_buffer)
    {
      std::vector< EReadDrawBuffer > render_buffers;
      render_buffers.push_back(render_buffer);
      setDrawBuffers(render_buffers);
    }
    void setDrawBuffers(EReadDrawBuffer render_buffer1, EReadDrawBuffer render_buffer2)
    {
      std::vector< EReadDrawBuffer > render_buffers;
      render_buffers.push_back(render_buffer1);
      render_buffers.push_back(render_buffer2);
      setDrawBuffers(render_buffers);
    }
    void setDrawBuffers(EReadDrawBuffer render_buffer1, EReadDrawBuffer render_buffer2, EReadDrawBuffer render_buffer3)
    {
      std::vector< EReadDrawBuffer > render_buffers;
      render_buffers.push_back(render_buffer1);
      render_buffers.push_back(render_buffer2);
      render_buffers.push_back(render_buffer3);
      setDrawBuffers(render_buffers);
    }
    void setDrawBuffers(EReadDrawBuffer render_buffer1, EReadDrawBuffer render_buffer2, EReadDrawBuffer render_buffer3, EReadDrawBuffer render_buffer4)
    {
      std::vector< EReadDrawBuffer > render_buffers;
      render_buffers.push_back(render_buffer1);
      render_buffers.push_back(render_buffer2);
      render_buffers.push_back(render_buffer3);
      render_buffers.push_back(render_buffer4);
      setDrawBuffers(render_buffers);
    }
    void setDrawBuffers(const std::vector< EReadDrawBuffer >& render_buffers) { mDrawBuffers = render_buffers; }
    const std::vector< EReadDrawBuffer >& renderBuffers() { return mDrawBuffers; }

  protected:
    std::vector< EReadDrawBuffer > mDrawBuffers;
    int mWidth;
    int mHeight;
    OpenGLContext* mOpenGLContext;
  };
  //------------------------------------------------------------------------------
}

#endif
