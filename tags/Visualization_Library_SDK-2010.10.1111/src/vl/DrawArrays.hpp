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

#ifndef DrawArrays_INCLUDE_DEFINE
#define DrawArrays_INCLUDE_DEFINE

#include <vl/DrawCall.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  // DrawArrays
  //------------------------------------------------------------------------------
  /**
   * Wraps the OpenGL function glDrawArrays(). See also http://www.opengl.org/sdk/docs/man/xhtml/glDrawArrays.xml for more information.
   *
   * DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays are used by Geometry to define a set of primitives to be rendered, see Geometry::drawCalls().
   *
   * \sa DrawCall, DrawElements, MultiDrawElements, DrawRangeElements, Geometry, Actor */
  class DrawArrays: public DrawCall
  {
  public:
    DrawArrays(): mStart(0), mCount(0) 
    { 
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType      = PT_TRIANGLES;
      mInstances = 1;
    }
    DrawArrays(EPrimitiveType primitive, int start, size_t count, size_t instances=1)
      : mStart(start), mCount(count)
    { 
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mInstances = instances;
      mType = primitive;
    }
    virtual const char* className() { return "DrawArrays"; }

    DrawArrays& operator=(const DrawArrays& other)
    {
      DrawCall::operator=(other);
      mStart     = other.mStart;
      mCount     = other.mCount;
      mInstances = other.mInstances;
      return *this;
    }

    virtual ref<DrawCall> clone() const 
    { 
      return new DrawArrays( primitiveType(), (int)start(), (int)count(), (int)instances() ); 
    }

    virtual void deleteVBOs() {}
    virtual void updateVBOs(bool) {}
    virtual unsigned int handle() const { return 0; }
    virtual size_t indexCount() const { return count(); }
    virtual size_t index(int i) const { return start() + i; }

    int triangleCount() const
    {
      switch( mType )
      {
      case PT_POINTS: return 0;
      case PT_LINES: return 0;
      case PT_LINE_LOOP: return 0;
      case PT_LINE_STRIP: return 0;
      case PT_TRIANGLES: return mCount / 3;
      case PT_TRIANGLE_STRIP: return mCount - 2;
      case PT_TRIANGLE_FAN: return mCount - 2;
      case PT_QUADS: return mCount / 4 * 2;
      case PT_QUAD_STRIP: return ( (mCount - 2) / 2 ) * 2;
      case PT_POLYGON: return mCount - 2;
      default:
        return 0;
      }
    }

    int lineCount() const
    {
      switch( mType )
      {
      case PT_LINES: return mCount / 2;
      case PT_LINE_LOOP: return mCount;
      case PT_LINE_STRIP: return mCount - 1;
      default:
        return 0;
      }
    }

    int pointCount() const
    {
      switch( mType )
      {
      case PT_POINTS: return mCount;
      default:
        return 0;
      }
    }

    virtual bool getTriangle( size_t tri_index, unsigned int* out_triangle ) const
    {
      return getTriangle_internal(tri_index, out_triangle);
    }

    virtual void render(bool) const
    {
      if ( instances() > 1 && (GLEW_ARB_draw_instanced||GLEW_EXT_draw_instanced) )
        VL_glDrawArraysInstanced( primitiveType(), (int)start(), (int)count(), (int)instances() );
      else
        glDrawArrays( primitiveType(), (int)start(), (int)count() );

      #ifndef NDEBUG
        unsigned int glerr = glGetError();
        if (glerr != GL_NO_ERROR)
        {
          String msg( (char*)gluErrorString(glerr) );
          Log::error( Say("glGetError() [%s:%n]: %s\n") << __FILE__ << __LINE__ << msg );
          Log::print(
            "OpenGL Geometry Instancing (GL_ARB_draw_instanced) does not support display lists."
            "If you are using geometry instancing in conjunction with display lists you will have to disable one of them.\n"
          );
          VL_TRAP()
        }
      #endif
    }

    //! sets the starting vertex for the rendering.
    void setStart(size_t start) { mStart = start; }

    //! returns the starting vertex for the rendering.
    size_t start() const { return mStart; }

    //! sets the number of vertices to be rendered.
    void setCount(size_t count) { mCount = count; }

    //! returns the number of vertices to be rendered.
    size_t count() const { return mCount; }

    //! Sets the number of instances for this set of primitives.
    void setInstances(size_t instances) { mInstances = instances; }

    //! Returns the number of instances for this set of primitives.
    size_t instances() const { return mInstances; }

    protected:
      size_t mStart;
      size_t mCount;
      size_t mInstances;
  };

}

#endif

