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

#include <vl/DrawCall.hpp>
#include <vl/Array.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>
#include <algorithm>

namespace vl
{

  //------------------------------------------------------------------------------
  // DrawElementsBase
  //------------------------------------------------------------------------------
  /**
   * Base interface for all DrawElements* sub classes.
   * Implements the index-type-independent interface of the class. That is you can cast to DrawElementsBase*
   * and access its members without needing to know whether the actual class is a 
   * vl::DrawElementsUInt, vl::DrawElementsUShort or vl::DrawElementsUByte. */
  class DrawElementsBase: public DrawCall
  {
  public:
    /** Returns the special index which idendifies a primitive restart. By default it is set to ~0 that is 
      * 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint index types. */
    GLuint primitiveRestartIndex() const { return mPrimitiveRestartIndex; }

    /** Sets the special index which idendifies a primitive restart. By default it is set to ~0 that is 
      * 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint index types. */
    void setPrimitiveRestartIndex(GLuint index) { mPrimitiveRestartIndex = index; }

    /** Returns whether the primitive-restart functionality is enabled or not. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt */
    bool primitiveRestartEnabled() const { return mPrimitiveRestartEnabled; }

    /** Enables the primitive-restart functionality. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt
      * \note Functions like triangleCount(), lineCount(), pointCount() and sortTriangles() should not be used when primitive restart is enabled. */
    void setPrimitiveRestartEnabled(bool enabled) { mPrimitiveRestartEnabled = enabled; }

    /** Sets the number of instances for this set of primitives. */
    void setInstances(int instances) { mInstances = instances; }

    /** Returns the number of instances for this set of primitives. */
    int instances() const { return mInstances; }

    /** If base_vertx is != 0 glDrawElementsBaseVertex/glDrawElementsInstancedBaseVertex will be used instead of their non *BaseVertx counterparts. 
      * Note that using base_vertx != requires OpenGL 3.2 or higher or ARB_draw_elements_base_vertex. 
      * For more information see also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawElementsBaseVertex.xml
      */
    void setBaseVertex(int base_vertex) { mBaseVertex = base_vertex; }

    /** Returns the currently used base vertex.
      * For more information see also http://www.opengl.org/sdk/docs/man3/xhtml/glDrawElementsBaseVertex.xml */
    int  baseVertex() const { return mBaseVertex; }

  protected:
    int mInstances;
    GLuint mPrimitiveRestartIndex;
    bool mPrimitiveRestartEnabled;
    int  mBaseVertex;
  };
  //------------------------------------------------------------------------------
  // DrawElements
  //------------------------------------------------------------------------------
  /** 
   * Wrapper for the OpenGL function glDrawElements(). See also http://www.opengl.org/sdk/docs/man/xhtml/glDrawElements.xml for more information.
   *
   * Features supported: 
   * - <b>multi instancing</b>: YES 
   * - <b>base vertex</b>: YES
   * - <b>primitive restart</b>: YES
   *
   * Use the functions setInstances() and instances() to use the <b>multi instancing</b> functionality.
   * (requires OpenGL 3.1). For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glDrawElementsInstanced.xml
   *
   * Use the functions setPrimitiveRestartIndex() and setPrimitiveRestartEnabled() to use the <b>primitive 
   * restart</b> functionality (requires OpenGL 3.1). For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glPrimitiveRestartIndex.xml
   *
   * Use the function setBaseVertex() with a non zero argument to use the <b>base vertex</b> functionality. 
   * Requires OpenGL 3.2 or GL_ARB_draw_elements_base_vertex. For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glDrawElementsBaseVertex.xml
   *
   * DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays are used by Geometry to define a set of primitives to be rendered, see Geometry::drawCalls().
   * The indices are stored in a GLBufferObject and thus they can be stored locally or on the GPU. 
   * To gain direct access to the GLBufferObject use the indices() function.
   *
   * \sa DrawCall, MultiDrawElements, DrawRangeElements, DrawArrays, Geometry, Actor */
  template <typename index_type, GLenum Tgltype, class arr_type>
  class DrawElements: public DrawElementsBase
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
    virtual const char* className() { return "DrawElements"; }

    DrawElements(EPrimitiveType primitive = PT_TRIANGLES, int instances = 1)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mType                    = primitive;
      mInstances               = instances;
      mIndexBuffer             = new arr_type;
      mPrimitiveRestartIndex   = index_type(~0);
      mPrimitiveRestartEnabled = false;
      mBaseVertex              = 0;
    }

    DrawElements& operator=(const DrawElements& other)
    {
      DrawElementsBase::operator=(other);
      *indices() = *other.indices();
      mInstances = other.mInstances;
      mPrimitiveRestartEnabled = other.mPrimitiveRestartEnabled;
      mPrimitiveRestartIndex   = other.mPrimitiveRestartIndex;
      mBaseVertex              = other.mBaseVertex;
      return *this;
    }

    virtual ref<DrawCall> clone() const 
    { 
      ref<DrawElements> de = new DrawElements;
      *de = *this;
      return de;
    }

    virtual unsigned int handle() const { return indices()->gpuBuffer()->handle(); }

    virtual size_t indexCount() const { return indices()->size(); }

    virtual size_t indexCountGPU() const { return indices()->sizeGPU(); }

    /** The index returned include the baseVertex() (with the exception of the primitive restart index). 
      * \note The function DrawElements::index() returns an index that does not include the base vertex. 
      * \sa DrawRangeElements::index()
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

      int count = (int)indexCount();
      if (count == 0)
        count = (int)indices()->sizeGPU();
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

      int count = (int)indexCount();
      if (count == 0)
        count = (int)indices()->sizeGPU();
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

      int count = (int)indexCount();
      if (count == 0)
        count = (int)indices()->sizeGPU();
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
          vl::Log::error("DrawElements error: primitive restart not supported by this OpenGL implementation!\n");
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
      {
        if ( instances() > 1 && (GLEW_ARB_draw_instanced||GLEW_EXT_draw_instanced) )
          VL_glDrawElementsInstanced( primitiveType(), use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr, (GLsizei)instances() );
        else
          glDrawElements( primitiveType(), use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr );
      }
      else
      {
        if ( instances() > 1 && (GLEW_ARB_draw_instanced||GLEW_EXT_draw_instanced) )
          VL_glDrawElementsInstancedBaseVertex( primitiveType(), use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr, (GLsizei)instances(), mBaseVertex );
        else
          VL_glDrawElementsBaseVertex( primitiveType(), use_vbo ? (GLsizei)indexCountGPU() : (GLsizei)indexCount(), indices()->glType(), ptr, mBaseVertex );
      }

      // primitive restart disable

      if(primitiveRestartEnabled())
      {
        if(GLEW_VERSION_3_1)
          glDisable(GL_PRIMITIVE_RESTART);
        else
        if(GLEW_NV_primitive_restart)
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

      if (use_vbo && this->indices()->gpuBuffer()->handle())
        VL_glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    }

  protected:
    ref< arr_type > mIndexBuffer;
  };
  //------------------------------------------------------------------------------
  // typedefs
  //------------------------------------------------------------------------------
  /** See DrawElements. A DrawElements using indices of type \p GLuint. */
  class DrawElementsUInt:  public DrawElements<GLuint, GL_UNSIGNED_INT, ArrayUInt>
  {
  public:
    DrawElementsUInt(EPrimitiveType primitive = PT_TRIANGLES, int instances = 1)
    :DrawElements<GLuint, GL_UNSIGNED_INT, ArrayUInt>(primitive, instances) {}
  };
  //------------------------------------------------------------------------------
  /** See DrawElements. A DrawElements using indices of type \p GLushort. */
  class DrawElementsUShort: public DrawElements<GLushort, GL_UNSIGNED_SHORT, ArrayUShort>
  {
  public:
    DrawElementsUShort(EPrimitiveType primitive = PT_TRIANGLES, int instances = 1)
    :DrawElements<GLushort, GL_UNSIGNED_SHORT, ArrayUShort>(primitive, instances) {}
  };
  //------------------------------------------------------------------------------
  /** See DrawElements. A DrawElements using indices of type \p GLubyte. */
  class DrawElementsUByte:  public DrawElements<GLubyte, GL_UNSIGNED_BYTE, ArrayUByte>
  {
  public:
    DrawElementsUByte(EPrimitiveType primitive = PT_TRIANGLES, int instances = 1)
    :DrawElements<GLubyte, GL_UNSIGNED_BYTE, ArrayUByte>(primitive, instances) {}
  };
   //------------------------------------------------------------------------------
}

#endif
