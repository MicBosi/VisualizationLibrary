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

#ifndef DrawRangeElements_INCLUDE_ONCE
#define DrawRangeElements_INCLUDE_ONCE

#include <vl/DrawCall.hpp>
#include <vl/Array.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>
#include <algorithm>

namespace vl
{

  //------------------------------------------------------------------------------
  // DrawRangeElementsBase
  //------------------------------------------------------------------------------
  /**
   * Base interface for all DrawRangeElements* sub classes.
   * Implements the index-type-independent interface of the class. That is you can cast to DrawRangeElementsBase*
   * and access its members without needing to know whether the actual class is a 
   * vl::DrawRangeElementsUInt, vl::DrawRangeElementsUShort or vl::DrawRangeElementsUByte. */
  class DrawRangeElementsBase: public DrawCall
  {
  public:
    /** Sets the range start. See also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElements.xml */
    void setRangeStart(int rstart) { mRangeStart = rstart; }

    /** Returns the range start. See also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElements.xml */
    int rangeStart() const { return mRangeStart; }

    /** Sets the range end. See also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElements.xml */
    void setRangeEnd(int rend) { mRangeEnd = rend; }

    /** Returns the range end. See also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElements.xml */
    int rangeEnd() const { return mRangeEnd; }

    /** Returns the special index which idendifies a primitive restart. By default it is set to ~0 that is 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint types. */
    GLuint primitiveRestartIndex() const { return mPrimitiveRestartIndex; }

    /** Sets the special index which idendifies a primitive restart. By default it is set to ~0 that is 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint types. */
    void setPrimitiveRestartIndex(GLuint index) { mPrimitiveRestartIndex = index; }

    /** Returns whether the primitive-restart functionality is enabled or not. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt */
    bool primitiveRestartEnabled() const { return mPrimitiveRestartEnabled; }

    /** Enables the primitive-restart functionality. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt
      * \note Functions like triangleCount(), lineCount(), pointCount() and sortTriangles() should not be used when primitive restart is enabled. */
    void setPrimitiveRestartEnabled(bool enabled) { mPrimitiveRestartEnabled = enabled; }

    /** If base_vertx is != 0 glDrawRangeElementsBaseVertex/glDrawRangeElementsInstancedBaseVertex will be used instead of their non *BaseVertx counterparts. 
      * Note that using base_vertx != requires OpenGL 3.2 or higher or ARB_draw_elements_base_vertex. 
      * For more information see also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElementsBaseVertex.xml
      */
    void setBaseVertex(int base_vertex) { mBaseVertex = base_vertex; }

    /** Returns the currently used base vertex.
      * For more information see also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElementsBaseVertex.xml */
    int  baseVertex() const { return mBaseVertex; }

  protected:
    int mRangeStart;
    int mRangeEnd;
    int  mBaseVertex;
    GLuint mPrimitiveRestartIndex;
    bool mPrimitiveRestartEnabled;
  };
  //------------------------------------------------------------------------------
  // DrawRangeElements
  //------------------------------------------------------------------------------
  /** 
    * Wrapper for the OpenGL function glDrawRangeElements(). See also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElements.xml for more information.
   *
   * Features supported: 
   * - <b>multi instancing</b>: NO 
   * - <b>base vertex</b>: YES
   * - <b>primitive restart</b>: YES
   *
   * Use the functions setPrimitiveRestartIndex() and setPrimitiveRestartEnabled() to use the <b>primitive 
   * restart</b> functionality (requires OpenGL 3.1). For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glPrimitiveRestartIndex.xml
   *
   * Use the function setBaseVertex() to use the <b>base vertex</b> functionality. 
   * Requires OpenGL 3.2 or GL_ARB_draw_elements_base_vertex. For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glDrawRangeElementsBaseVertex.xml
   *
   * DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays are used by Geometry to define a set of primitives to be rendered, see Geometry::drawCalls().
   * The indices are stored in a GLBufferObject and thus they can be stored locally or on the GPU. 
   * To gain direct access to the GLBufferObject use the indices() function.
   *
   * \sa DrawCall, DrawElements, MultiDrawElements, DrawArrays, Geometry, Actor */
  template <typename index_type, GLenum Tgltype, class arr_type>
  class DrawRangeElements: public DrawRangeElementsBase
  {
  private:
    template<typename T>
    class Triangle
    {
    public:
      T ABC[3];
      bool operator<(const Triangle<index_type>& b) const
      {
        if (ABC[0] != b.ABC[0])
          return ABC[0] < b.ABC[0];
        else
        if (ABC[1] != b.ABC[1])
          return ABC[1] < b.ABC[1];
        else
          return ABC[2] < b.ABC[2];
      }
      void rotate()
      {
        if (ABC[0] > ABC[1])
          { T tmp = ABC[0]; ABC[0] = ABC[1]; ABC[1] = ABC[2]; ABC[2] = tmp; }
        if (ABC[0] > ABC[1])
          { T tmp = ABC[0]; ABC[0] = ABC[1]; ABC[1] = ABC[2]; ABC[2] = tmp; }
      }
    };

  public:
    virtual const char* className() { return "DrawRangeElements"; }

    DrawRangeElements(EPrimitiveType primitive = PT_TRIANGLES, int r_start=0, int r_end=index_type(~0))
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType                    = primitive;
      mRangeStart              = r_start;
      mRangeEnd                = r_end;
      mIndexBuffer             = new arr_type;
      mPrimitiveRestartIndex   = index_type(~0);
      mPrimitiveRestartEnabled = false;
      mBaseVertex              = 0;
    }

    DrawRangeElements& operator=(const DrawRangeElements& other)
    {
      DrawRangeElementsBase::operator=(other);
      *indices()               = *other.indices();
      mRangeStart              = other.mRangeStart;
      mRangeEnd                = other.mRangeEnd;
      mPrimitiveRestartEnabled = other.mPrimitiveRestartEnabled;
      mPrimitiveRestartIndex   = other.mPrimitiveRestartIndex;
      mBaseVertex              = other.mBaseVertex;
      return *this;
    }

    virtual ref<DrawCall> clone() const 
    { 
      ref<DrawRangeElements> de = new DrawRangeElements;
      *de = *this;
      return de;
    }

    virtual unsigned int handle() const { return indices()->gpuBuffer()->handle(); }

    virtual size_t indexCount() const { return indices()->size(); }

    virtual size_t indexCountGPU() const { return indices()->sizeGPU(); }

    /** The index returned include the baseVertex() (with the exception of the primitive restart index). 
      * \note The function DrawRangeElements::index() returns an index that does not include the base vertex. 
      * \sa DrawElements::index()
      */
    virtual size_t index(int i) const 
    { 
      size_t idx = indices()->at(i);
      if (primitiveRestartEnabled() && idx == primitiveRestartIndex())
        return primitiveRestartIndex();
      else
        return  idx + mBaseVertex; 
    }

    arr_type* indices() { return mIndexBuffer.get(); }

    const arr_type* indices() const { return mIndexBuffer.get(); }

    virtual void updateVBOs(bool discard_local_data = false)
    {
      indices()->updateVBO(discard_local_data);
    }

    virtual void deleteVBOs()
    {
      indices()->gpuBuffer()->deleteGLBufferObject();
    }

    virtual void clearLocalBuffer()
    {
      indices()->gpuBuffer()->resize(0);
    }

    /** Returns the number of triangles contained in this primitive set.
      * \note This function returns -1 if primitive restart is enabled. */
    int triangleCount() const
    {
      if (primitiveRestartEnabled())
        return -1;

      size_t count = indexCount();
      if (count == 0)
        count = indices()->sizeGPU();
      switch( mType )
      {
      case PT_POINTS: return 0;
      case PT_LINES: return 0;
      case PT_LINE_LOOP: return 0;
      case PT_LINE_STRIP: return 0;
      case PT_TRIANGLES: return count / 3;
      case PT_TRIANGLE_STRIP: return count - 2;
      case PT_TRIANGLE_FAN: return count - 2;
      case PT_QUADS: return count / 4 * 2;
      case PT_QUAD_STRIP: return ( (count - 2) / 2 ) * 2;
      case PT_POLYGON: return count - 2;
      default:
        return 0;
      }
    }

    /** Returns the number of lines contained in this primitive set.
      * \note This function returns -1 if primitive restart is enabled. */
    int lineCount() const 
    {
      if (primitiveRestartEnabled())
        return -1;

      size_t count = indexCount();
      if (count == 0)
        count = indices()->sizeGPU();
      switch( mType )
      {
      case PT_LINES: return count / 2;
      case PT_LINE_LOOP: return count;
      case PT_LINE_STRIP: return count - 1;
      default:
        return 0;
      }
    }

    /** Returns the number of points contained in this primitive set.
      * \note This function returns -1 if primitive restart is enabled. */
    int pointCount() const
    {
      if (primitiveRestartEnabled())
        return -1;

      size_t count = indexCount();
      if (count == 0)
        count = indices()->sizeGPU();
      switch( mType )
      {
      case PT_POINTS: return count;
      default:
        return 0;
      }
    }

    /** Tries to sort the triangles in a GPU-cache friendly manner.
      * \note This function does nothing if primitive restart is enabled. */
    virtual void sortTriangles()
    {
      if (primitiveRestartEnabled())
        return;

      if (primitiveType() == PT_TRIANGLES)
      {
        Triangle<index_type>* tri = (Triangle<index_type>*)indices()->ptr();
        for(unsigned i=0; i<indexCount()/3; ++i)
          tri[i].rotate();
        std::sort(tri, tri + indexCount()/3);
      }
    }

    virtual bool getTriangle( size_t tri_index, unsigned int* out_triangle ) const
    {
      if (primitiveRestartEnabled())
        return false;
      else
        return getTriangle_internal(tri_index, out_triangle);
    }

    virtual void render(bool use_vbo) const
    {
      VL_CHECK(mBaseVertex>=0)
      VL_CHECK(!use_vbo || (use_vbo && (GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)))
      use_vbo &= GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0; // && indices()->gpuBuffer()->handle() && indexCountGPU();
      if ( !use_vbo && !indexCount() )
        return;

      // primitive restart enable
      if(primitiveRestartEnabled())
      {
        if(GLEW_VERSION_3_1)
        {
          glEnable(GL_PRIMITIVE_RESTART);
          glPrimitiveRestartIndex(primitiveRestartIndex());
        }
        else
        if(GLEW_NV_primitive_restart)
        {
          glEnable(GL_PRIMITIVE_RESTART_NV);
          glPrimitiveRestartIndexNV(primitiveRestartIndex());
        }
        else
        {
          vl::Log::error("DrawRangeElements error: primitive restart not supported by this OpenGL implementation!\n");
          VL_TRAP();
          return;
        }
      }

      const GLvoid* ptr = indices()->gpuBuffer()->ptr();

      if (use_vbo && indices()->gpuBuffer()->handle())
      {
        VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices()->gpuBuffer()->handle());
        ptr = 0;
      }
      else
        VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      if (mBaseVertex == 0)
        glDrawRangeElements( primitiveType(), mRangeStart, mRangeEnd, use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr );
      else
        VL_glDrawRangeElementsBaseVertex( primitiveType(), mRangeStart, mRangeEnd, use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr, mBaseVertex );

      // primitive restart disable

      if(primitiveRestartEnabled())
      {
        if(GLEW_VERSION_3_1)
          glDisable(GL_PRIMITIVE_RESTART);
        else
        if(GLEW_NV_primitive_restart)
          glDisable(GL_PRIMITIVE_RESTART_NV);
      }

      if (use_vbo && this->indices()->gpuBuffer()->handle())
        VL_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    void computeRange()
    {
      size_t count = indexCount();
      mRangeStart  = index(0);
      mRangeEnd    = index(0);
      for(size_t i=1; i<count; ++i)
      {
        if(primitiveRestartEnabled() && index(i) == mPrimitiveRestartIndex)
          continue;
        if ((int)index(i)<mRangeStart)
          mRangeStart = index(i);
        if ((int)index(i)>mRangeEnd)
          mRangeEnd = index(i);
      }
    }

  protected:
    ref< arr_type > mIndexBuffer;
  };
  //------------------------------------------------------------------------------
  // typedefs
  //------------------------------------------------------------------------------
  /** See DrawRangeElements. A DrawRangeElements using indices of type \p GLuint. */
  class DrawRangeElementsUInt: public DrawRangeElements<GLuint, GL_UNSIGNED_INT, ArrayUInt>
  {
  public:
    DrawRangeElementsUInt(EPrimitiveType primitive = PT_TRIANGLES, int r_start=0, int r_end=GLuint(~0))
    :DrawRangeElements(primitive, r_start, r_end) {}
  };
  //------------------------------------------------------------------------------
  /** See DrawRangeElements. A DrawRangeElements using indices of type \p GLushort. */
  class DrawRangeElementsUShort: public DrawRangeElements<GLushort, GL_UNSIGNED_SHORT, ArrayUShort>
  {
  public:
    DrawRangeElementsUShort(EPrimitiveType primitive = PT_TRIANGLES, int r_start=0, int r_end=GLushort(~0))
    :DrawRangeElements(primitive, r_start, r_end) {}
  };
  //------------------------------------------------------------------------------
  /** See DrawRangeElements. A DrawRangeElements using indices of type \p GLubyte. */
  class DrawRangeElementsUByte: public DrawRangeElements<GLubyte, GL_UNSIGNED_BYTE, ArrayUByte>
  {
  public:
    DrawRangeElementsUByte(EPrimitiveType primitive = PT_TRIANGLES, int r_start=0, int r_end=GLubyte(~0))
    :DrawRangeElements(primitive, r_start, r_end) {}
  };
  //------------------------------------------------------------------------------
}

#endif
