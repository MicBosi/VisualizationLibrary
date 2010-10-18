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

#ifndef Clear_INCLUDE_ONCE
#define Clear_INCLUDE_ONCE

#include <vl/Renderable.hpp>
#include <vl/Vector4.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  // Clear
  //------------------------------------------------------------------------------
  /**
   * The Clear class is a Renderable used to clear the whole or a portion of the color, stencil or depth buffer.
   * 
   * The Clear class is a special Renderable that wraps the functionalities of the following OpenGL functions:
   * - glClear() - http://www.opengl.org/sdk/docs/man/xhtml/glClear.xml
   * - glClearColor() - http://www.opengl.org/sdk/docs/man/xhtml/glClearColor.xml
   * - glClearDepth() - http://www.opengl.org/sdk/docs/man/xhtml/glClearDepth.xml
   * - glClearStencil() - http://www.opengl.org/sdk/docs/man/xhtml/glClearStencil.xml
   * - glScissor() - http://www.opengl.org/sdk/docs/man/xhtml/glScissor.xml
   *
   * Usually the color, depth and stencil buffers are cleared at the beginning of a rendering when the Viewport
   * is initialized. To handle less common cases the Clear class allows you to perform a clear operation 
   * also in the middle of a rendering. Just bind it to an an Actor and setup the appropriate clearing options.
   *
   * \note
   *
   * - Binding a Transform to the same Actor will not affect the clearing operation in any way.
   * - You still need to bind an Effect to the same Actor infact the Effect's renderRank() and blending 
   * state enabled/disabled might affect the rendering order of the Actor bound to your Clear object, 
   * i.e. the moment at which your buffers are cleared. In other words the usual rules to determine the 
   * rendering order of an Actor apply also when a Clear object is bound to it. For this reason in order 
   * to define the exact point at which the buffers are cleared you should pay special attention to your 
   * Actor's and Effect's renderRank() and Actor's renderBlock(). 
   * - The following render states also affect the clearing process: ColorMask, StencilMask, DepthMask.
  */
  class Clear: public Renderable
  {
  public:
    virtual const char* className() { return "Clear"; }
    Clear(): mClearColorMode(CCM_Float), mClearDepthValue(1.0f), mClearStencilValue(0),
             mClearColorBuffer(false), mClearDepthBuffer(false), mClearStencilBuffer(false)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      // no scissor box by default
      mScissorBox[0] = 0;
      mScissorBox[1] = 0;
      mScissorBox[2] = -1;
      mScissorBox[3] = -1;
    }

    virtual void render(const Actor*, const OpenGLContext*, const Camera*) const
    {
      // build buffer bit mask
      GLbitfield mask = 0;
      mask = mask | (mClearColorBuffer   ? GL_COLOR_BUFFER_BIT   : 0);
      mask = mask | (mClearDepthBuffer   ? GL_DEPTH_BUFFER_BIT   : 0);
      mask = mask | (mClearStencilBuffer ? GL_STENCIL_BUFFER_BIT : 0);

      // check for integer texture support
      if ( (!glClearColorIiEXT || !glClearColorIuiEXT) && (clearColorMode() == CCM_Int || clearColorMode() == CCM_UInt) )
      {
        Log::error("Clear::render(): glClearColorIiEXT and glClearColorIuiEXT not supported.\n");
        return;
      }

      if (mask)
      {
        int viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        // save scissor settings
        GLboolean scissor_on = glIsEnabled(GL_SCISSOR_TEST);
        int scissor_box_save[4] = {0,0,-1,-1};
        glGetIntegerv(GL_SCISSOR_BOX, scissor_box_save);

        int scissor_box[4] = {0,0,-1,-1};

        // fit to the viewport
        if (mScissorBox[2] == -1 || mScissorBox[3] == -1)
        {
          scissor_box[0] = viewport[0];
          scissor_box[1] = viewport[1];
          scissor_box[2] = viewport[2];
          scissor_box[3] = viewport[3];
        }
        else
        // scissor box in viewport coords
        {
          scissor_box[0] = mScissorBox[0] + viewport[0];
          scissor_box[1] = mScissorBox[1] + viewport[1];
          scissor_box[2] = mScissorBox[2];
          scissor_box[3] = mScissorBox[3];
        }

        // viewport from x,y,w,h -> x,y,x2,y2
        viewport[2] = viewport[0] + viewport[2] -1;
        viewport[3] = viewport[1] + viewport[3] -1;
        // scissor_box from x,y,w,h -> x,y,x2,y2
        scissor_box[2] = scissor_box[0] + scissor_box[2] -1;
        scissor_box[3] = scissor_box[1] + scissor_box[3] -1;
        // clip scissor box
        if (scissor_box[0] < viewport[0]) scissor_box[0] = viewport[0];
        if (scissor_box[1] < viewport[1]) scissor_box[1] = viewport[1];
        if (scissor_box[2] > viewport[2]) scissor_box[2] = viewport[2];
        if (scissor_box[3] > viewport[3]) scissor_box[3] = viewport[3];
        // culling
        if (scissor_box[0] > scissor_box[2]) 
          return;
        if (scissor_box[1] > scissor_box[3]) 
          return;
        // scissor_box from x,y,x2,y2 -> x,y,w,h 
        scissor_box[2] = scissor_box[2] -scissor_box[0] +1;
        scissor_box[3] = scissor_box[3] -scissor_box[1] +1;

        // enable scissor test
        glEnable(GL_SCISSOR_TEST);
        glScissor(scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]); VL_CHECK_OGL()

        // defines the clear values
        if (mClearColorBuffer)
        {
          switch(clearColorMode())
          {
            case CCM_Float: glClearColor(      mClearColorValue.r(),     mClearColorValue.g(),     mClearColorValue.b(),     mClearColorValue.a());     break;
            case CCM_Int:   glClearColorIiEXT( mClearColorValueInt.r(),  mClearColorValueInt.g(),  mClearColorValueInt.b(),  mClearColorValueInt.a());  break;
            case CCM_UInt:  glClearColorIuiEXT(mClearColorValueUInt.r(), mClearColorValueUInt.g(), mClearColorValueUInt.b(), mClearColorValueUInt.a()); break;
          }
        }
        if (mClearDepthBuffer)
          glClearDepth(mClearDepthValue);
        if (mClearStencilBuffer)
          glClearStencil(mClearStencilValue);

        // clear!
        glClear(mask);

        // restore scissor settings
        if (!scissor_on)
          glDisable(GL_SCISSOR_TEST);
        glScissor(scissor_box_save[0], scissor_box_save[1], scissor_box_save[2], scissor_box_save[3]); VL_CHECK_OGL()
      }
    }

    void setClearColorBuffer(bool clear)   { mClearColorBuffer   = clear; }
    void setClearDepthBuffer(bool clear)   { mClearDepthBuffer   = clear; }
    void setClearStencilBuffer(bool clear) { mClearStencilBuffer = clear; }

    void setClearColorValue(const fvec4& clear_val) { mClearColorValue   = clear_val; }
    void setClearColorValueInt(const ivec4& clear_val) { mClearColorValueInt   = clear_val; }
    void setClearColorValueUInt(const uvec4& clear_val) { mClearColorValueUInt   = clear_val; }
    void setClearDepthValue(float clear_val)        { mClearDepthValue   = clear_val; }
    void setClearStencilValue(int clear_val)      { mClearStencilValue = clear_val; }

    void setClearColorMode(EClearColorMode mode) { mClearColorMode = mode; }
    EClearColorMode clearColorMode() const { return mClearColorMode; }

    //! Defines which portion of the rendering buffers should be cleared.
    //! The parameters 'x', 'y', 'z', 'w' specify a rectangular area in viewport coordinates.
    //! Such area is also clipped against the viewport borders.
    //! If 'w' or 'h' are set to -1 then the whole viewport is cleared.
    void setScissorBox(int x, int y, int w, int h)     { mScissorBox[0] = x; mScissorBox[1] = y; mScissorBox[2] = w; mScissorBox[3] = h; }
    void getScissorBox(int& x, int& y, int& w, int& h) { x = mScissorBox[0]; y = mScissorBox[1]; w = mScissorBox[2]; h = mScissorBox[3]; }

  protected:
    virtual void computeBounds_Implementation() { setBoundingBox(AABB()); setBoundingSphere(Sphere()); }

    fvec4 mClearColorValue;
    ivec4 mClearColorValueInt;
    uvec4 mClearColorValueUInt;
    int mScissorBox[4];
    EClearColorMode mClearColorMode;
    float mClearDepthValue;
    int mClearStencilValue;
    bool mClearColorBuffer;
    bool mClearDepthBuffer;
    bool mClearStencilBuffer;
  };
}

#endif
