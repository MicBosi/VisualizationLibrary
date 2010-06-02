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
   * DrawArrays and DrawElements supports all the OpenGL primitive types. Geometry multi instancing is also supported, see the functions setInstances() and instances().
   * The indices of a DrawElement are stored in a GLBufferObject and thus can be stored locally or on the GPU. 
   * To gain direct access to the GLBufferObject use the indices() function.
   *
   * \sa DrawElementsUInt, DrawElementsUShort, DrawElementsUByte, DrawArrays, Geometry, Actor
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
        mName = "DrawElements";
      #endif
      mType        = primitive;
      mInstances   = instances;
      mIndexBuffer = new arr_type;
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

    size_t triangleCount() const
    {
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

    size_t lineCount() const 
    {
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

    size_t pointCount() const
    {
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

    virtual void sortTriangles()
    {
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

  protected:
    ref< arr_type > mIndexBuffer;
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
}

#endif
