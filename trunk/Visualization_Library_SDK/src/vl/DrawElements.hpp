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

#ifndef DrawElements_INCLUDE_ONCE
#define DrawElements_INCLUDE_ONCE

#include <vl/Primitives.hpp>
#include <vl/Array.hpp>
#include <algorithm>

namespace vl
{
  //------------------------------------------------------------------------------
  // DrawElements
  //------------------------------------------------------------------------------
  /** 
   * Wrapper for the OpenGL function glDrawElements(), see also http://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml for more information.
   *
   * DrawElements is the template base of vl::DrawElementsUInt, vl::DrawElementsUShort, vl::DrawElementsUByte.
   * DrawArrays and DrawElements are used by Geometry to define a set of primitives to be rendered (Geometry::primitives()).
   * DrawArrays and DrawElements supports all the OpenGL primitive types. Geometry multi instancing is also supported, see the functions setInstances() and instances() (requires OpenGL 3.1).
   * The indices of a DrawElement are stored in a GLBufferObject and thus can be stored locally or on the GPU. 
   * To gain direct access to the GLBufferObject use the indices() function.
   * Use the functions setPrimitiveRestartIndex() and setPrimitiveRestartEnabled() to access the primitive restart functionality (requires OpenGL 3.1)
   *
   * \sa DrawElementsUInt, DrawElementsUShort, DrawElementsUByte, DrawRangeElementsUInt, DrawRangeElementsUShort, DrawRangeElementsUByte, DrawArrays, Geometry, Actor
  */
  template <typename index_type, GLenum Tgltype, class arr_type>
  class DrawElements: public Primitives
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
    DrawElements(EPrimitiveType primitive = PT_TRIANGLES, int instances = 1)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType        = primitive;
      mInstances   = instances;
      mIndexBuffer = new arr_type;
      mPrimitiveRestartIndex   = ~(index_type)0;
      mPrimitiveRestartEnabled = false;
    }
    virtual const char* className() { return "DrawElements"; }

    virtual ref<Primitives> clone() const 
    { 
      ref<DrawElements> de = new DrawElements( primitiveType(), (int)instances() );
      if (indices()->size())
      {
        de->indices()->resize( indices()->size() );
        memcpy( de->indices()->ptr(), indices()->ptr(), indices()->bytesUsed() );
      }
      de->setPrimitiveRestartEnabled( primitiveRestartEnabled());
      de->setPrimitiveRestartIndex( primitiveRestartIndex());
      return de;
    }

    virtual unsigned int handle() const { return indices()->gpuBuffer()->handle(); }

    void setPrimitiveType(EPrimitiveType type) { mType = type; }

    EPrimitiveType primitiveType() const { return mType; }

    virtual size_t indexCount() const { return indices()->size(); }

    virtual size_t indexCountGPU() const { return indices()->sizeGPU(); }

    virtual size_t index(int i) const { return (int)indices()->at(i); }

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

    //! Returns the number of triangles contained in this primitive set.
    //! \note This function returns 0 if primitive restart is enabled.
    size_t triangleCount() const
    {
      if (primitiveRestartEnabled())
        return 0;

      size_t count = indexCount();
      if (count == 0)
        count = indices()->sizeGPU();
      switch( mType )
      {
      case PT_POINTS: return 0;
      case PT_LINES: return 0;
      case PT_LINE_LOOP: return 0;
      case PT_LINE_STRIP: return 0;
      case PT_TRIANGLES: return (count / 3) * instances();
      case PT_TRIANGLE_STRIP: return (count - 2) * instances();
      case PT_TRIANGLE_FAN: return (count - 2) * instances();;
      case PT_QUADS: return (count / 4 * 2) * instances();;
      case PT_QUAD_STRIP: return ( (count - 2) / 2 ) * 2 * instances();;
      case PT_POLYGON: return (count - 2) * instances();;
      default:
        return 0;
      }
    }

    //! Returns the number of lines contained in this primitive set.
    //! \note This function returns 0 if primitive restart is enabled.
    size_t lineCount() const 
    {
      if (primitiveRestartEnabled())
        return 0;

      size_t count = indexCount();
      if (count == 0)
        count = indices()->sizeGPU();
      switch( mType )
      {
      case PT_LINES: return (count / 2) * instances();
      case PT_LINE_LOOP: return (count) * instances();
      case PT_LINE_STRIP: return (count - 1) * instances();
      default:
        return 0;
      }
    }

    //! Returns the number of points contained in this primitive set.
    //! \note This function returns 0 if primitive restart is enabled.
    size_t pointCount() const
    {
      if (primitiveRestartEnabled())
        return 0;

      size_t count = indexCount();
      if (count == 0)
        count = indices()->sizeGPU();
      switch( mType )
      {
      case PT_POINTS: return count * instances();
      default:
        return 0;
      }
    }

    //! Tries to sort the triangles in a GPU-cache friendly manner.
    //! \note This function does nothing if primitive restart is enabled.
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

    virtual void render(bool use_vbo) const
    {
      VL_WARN(!use_vbo || (use_vbo && (GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5)))
      use_vbo &= GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5; // && indices()->gpuBuffer()->handle() && indexCountGPU();
      if ( !use_vbo && !indexCount() )
        return;

      // primitive restart enable

      if(primitiveRestartEnabled())
      {
        if (glPrimitiveRestartIndex)
        {
          glEnable(GL_PRIMITIVE_RESTART);
          glPrimitiveRestartIndex(primitiveRestartIndex());
        }
        else
        if (glPrimitiveRestartIndexNV)
        {
          glEnable(GL_PRIMITIVE_RESTART_NV);
          glPrimitiveRestartIndexNV(primitiveRestartIndex());
        }
        else
        {
          vl::Log::error("DrawElements error: primitive restart not supported by this OpenGL implementation!\n");
          VL_TRAP();
          return;
        }
      }

      const GLvoid* ptr = indices()->gpuBuffer()->ptr();

      VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      if (use_vbo && indices()->gpuBuffer()->handle())
      {
        VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices()->gpuBuffer()->handle());
        ptr = 0;
      }

      if ( instances() > 1 && (GLEW_ARB_draw_instanced||GLEW_EXT_draw_instanced) )
        VL_glDrawElementsInstanced( primitiveType(), use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr, (GLsizei)instances() );
      else
        glDrawElements( primitiveType(), use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr );

      // primitive restart disable

      if(primitiveRestartEnabled())
      {
        if (glPrimitiveRestartIndex)
          glDisable(GL_PRIMITIVE_RESTART);
        else
        if (glPrimitiveRestartIndexNV)
          glDisable(GL_PRIMITIVE_RESTART_NV);
      }

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

      if (use_vbo && indices() && indices()->gpuBuffer()->handle())
        VL_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    //! Returns the special index which idendifies a primitive restart. By default it is set to ~0 that is 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint types.
    GLuint primitiveRestartIndex() const { return mPrimitiveRestartIndex; }
    //! Sets the special index which idendifies a primitive restart. By default it is set to ~0 that is 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint types.
    void setPrimitiveRestartIndex(GLuint index) { mPrimitiveRestartIndex = index; }

    //! Returns whether the primitive-restart functionality is enabled or not. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt
    bool primitiveRestartEnabled() const { return mPrimitiveRestartEnabled; }
    //! Enables the primitive-restart functionality. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt
    //! \note Functions like triangleCount(), lineCount(), pointCount() and sortTriangles() should not be used when primitive restart is enabled.
    void setPrimitiveRestartEnabled(bool enabled) { mPrimitiveRestartEnabled = enabled; }


  protected:
    ref< arr_type > mIndexBuffer;
    GLuint mPrimitiveRestartIndex;
    bool mPrimitiveRestartEnabled;
  };
  //------------------------------------------------------------------------------
  // typedefs
  //------------------------------------------------------------------------------
  //! A DrawElements using indices of type \p GLuint.
  typedef DrawElements<GLuint, GL_UNSIGNED_INT, ArrayUInt> DrawElementsUInt;
  //------------------------------------------------------------------------------
  //! A DrawElements using indices of type \p GLushort.
  typedef DrawElements<GLushort, GL_UNSIGNED_SHORT, ArrayUShort> DrawElementsUShort;
  //------------------------------------------------------------------------------
  //! A DrawElements using indices of type \p GLubyte.
  typedef DrawElements<GLubyte, GL_UNSIGNED_BYTE, ArrayUByte> DrawElementsUByte;
  //------------------------------------------------------------------------------



  //------------------------------------------------------------------------------
  // DrawRangeElements
  //------------------------------------------------------------------------------
  /** 
   * Wrapper for the OpenGL function glDrawRangeElements(), see also http://www.opengl.org/sdk/docs/man/xhtml/glDrawRangeElements.xml for more information.
   *
   * DrawRangeElements is the template base of vl::DrawRangeElementsUInt, vl::DrawRangeElementsUShort, vl::DrawRangeElementsUByte.
   * Use the functions setPrimitiveRestartIndex() and setPrimitiveRestartEnabled() to access the primitive restart functionality (requires OpenGL 3.1)
   *
   * \note DrawRangeElements does not support multi instancing.
   *
   * \sa DrawElementsUInt, DrawElementsUShort, DrawElementsUByte, DrawRangeElementsUInt, DrawRangeElementsUShort, DrawRangeElementsUByte, DrawArrays, Geometry, Actor
  */
  template <typename index_type, GLenum Tgltype, class arr_type>
  class DrawRangeElements: public DrawElements<index_type, Tgltype, arr_type>
  {

  public:
    DrawRangeElements(EPrimitiveType primitive = PT_TRIANGLES, GLuint start = ~(GLuint)0, GLuint end = 0)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType = primitive;
      // start/end range
      mStart = start;
      mEnd   = end;
    }
    virtual const char* className() { return "DrawRangeElements"; }

    virtual ref<Primitives> clone() const 
    { 
      ref<DrawRangeElements> de = new DrawRangeElements( primitiveType(), (int)instances() );
      if (indices()->size())
      {
        de->indices()->resize( indices()->size() );
        memcpy( de->indices()->ptr(), indices()->ptr(), indices()->bytesUsed() );
      }
      de->setPrimitiveRestartEnabled( primitiveRestartEnabled());
      de->setPrimitiveRestartIndex( primitiveRestartIndex());
      return de;
    }

    virtual void render(bool use_vbo) const
    {
      VL_CHECK(mEnd >= mStart);
      VL_WARN(!use_vbo || (use_vbo && (GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5)))
      use_vbo &= GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5; // && indices()->gpuBuffer()->handle() && indexCountGPU();
      if ( !use_vbo && !indexCount() )
        return;

      #ifndef NDEBUG
      if (instances() != 1)
        vl::Log::error("DrawRangeElements does not support multi instancing!\n");
        VL_CHECK(instances() == 1);
      #endif

      // primitive restart enable

      if(primitiveRestartEnabled())
      {
        if (glPrimitiveRestartIndex)
        {
          glEnable(GL_PRIMITIVE_RESTART);
          glPrimitiveRestartIndex(primitiveRestartIndex());
        }
        else
        if (glPrimitiveRestartIndexNV)
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

      VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      if (use_vbo && indices()->gpuBuffer()->handle())
      {
        VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices()->gpuBuffer()->handle());
        ptr = 0;
      }

      // No multi instancing supported by glDrawRangeElements
      glDrawRangeElements( primitiveType(), mStart, mEnd, use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr );

      // primitive restart disable

      if(primitiveRestartEnabled())
      {
        if (glPrimitiveRestartIndex)
          glDisable(GL_PRIMITIVE_RESTART);
        else
        if (glPrimitiveRestartIndexNV)
          glDisable(GL_PRIMITIVE_RESTART_NV);
      }

      if (use_vbo && indices() && indices()->gpuBuffer()->handle())
        VL_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

    //! Automatically computes the start/end values from the current indices.
    void computeRange()
    {
      if (indexCount() == 0)
      {
        mStart = (GLuint)-1;
        mEnd   =  0;
        return;
      }
      mStart = (GLuint)index(0);
      mEnd   = (GLuint)index(0);
      for(int i=0; i<indexCount(); ++i)
      {
        GLuint idx = (GLuint)index(i);
        // ignore primitive restart special index.
        if ( primitiveRestartEnabled() && primitiveRestartIndex() == idx )
          continue;
        if (idx<mStart)
          mStart = idx;
        if (idx>mEnd)
          mEnd = idx;
      }

    }

    //! Sets the start index of the range to be drawn. See also http://www.opengl.org/sdk/docs/man/xhtml/glDrawRangeElements.xml
    void setStart(GLuint start) { mStart = start; }
    //! Returns the start index of the range to be drawn. See also http://www.opengl.org/sdk/docs/man/xhtml/glDrawRangeElements.xml
    GLuint start() const { return mStart; }
    //! Sets the start index of the range to be drawn. See also http://www.opengl.org/sdk/docs/man/xhtml/glDrawRangeElements.xml
    void setEnd(GLuint end) { mEnd = end; }
    //! Sets the end index of the range to be drawn. See also http://www.opengl.org/sdk/docs/man/xhtml/glDrawRangeElements.xml
    GLuint end() const { return mEnd; }

  protected:
    GLuint mStart;
    GLuint mEnd;
  };
  //------------------------------------------------------------------------------
  // typedefs
  //------------------------------------------------------------------------------
  //! A DrawRangeElements using indices of type \p GLuint.
  typedef DrawRangeElements<GLuint, GL_UNSIGNED_INT, ArrayUInt> DrawRangeElementsUInt;
  //------------------------------------------------------------------------------
  //! A DrawRangeElements using indices of type \p GLushort.
  typedef DrawRangeElements<GLushort, GL_UNSIGNED_SHORT, ArrayUShort> DrawRangeElementsUShort;
  //------------------------------------------------------------------------------
  //! A DrawRangeElements using indices of type \p GLubyte.
  typedef DrawRangeElements<GLubyte, GL_UNSIGNED_BYTE, ArrayUByte> DrawRangeElementsUByte;
  //------------------------------------------------------------------------------
}

#endif
