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

#ifndef MultiDrawElements_INCLUDE_ONCE
#define MultiDrawElements_INCLUDE_ONCE

#include <vlGraphics/DrawCall.hpp>
#include <vlCore/Array.hpp>
#include <vlGraphics/TriangleIterator.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <algorithm>

namespace vl
{
  //------------------------------------------------------------------------------
  // MultiDrawElementsBase
  //------------------------------------------------------------------------------
  /**
   * Base interface for all MultiDrawElements* sub classes.
   * Implements the index-type-independent interface of the class. That is you can cast to MultiDrawElementsBase*
   * and access its members without needing to know whether the actual class is a 
   * vl::MultiDrawElementsUInt, vl::MultiDrawElementsUShort or vl::MultiDrawElementsUByte. */
  class MultiDrawElementsBase: public DrawCall
  {
    INSTRUMENT_CLASS(vl::MultiDrawElementsBase, DrawCall)

  public:
    /** Returns the special index which idendifies a primitive restart. By default it is set to ~0 that is 
      * 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint index types. */
    GLuint primitiveRestartIndex() const { return mPrimitiveRestartIndex; }

    /** Sets the special index which idendifies a primitive restart. By default it is set to ~0 that is 
      * 0xFF, 0xFFFF, 0xFFFFFFFF respectively for ubyte, ushort, uint index types. */
    void setPrimitiveRestartIndex(GLuint index) { mPrimitiveRestartIndex = index; }

    /** Returns whether the primitive-restart functionality is enabled or not. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt */
    bool primitiveRestartEnabled() const { return mPrimitiveRestartEnabled; }
    
    /** Enables the primitive-restart functionality. See http://www.opengl.org/registry/specs/NV/primitive_restart.txt */
    void setPrimitiveRestartEnabled(bool enabled) { mPrimitiveRestartEnabled = enabled; }

    /** Sets the vector defining the length of each primitive and automatically computes the pointer 
      * vectors used to exectue glMultiDrawElements(). */
    void setCountVector(const std::vector<GLsizei>& vcount)
    {
      mCountVector = vcount;
      compute_pointer_vector();
    }

    /** The count vector used as 'count' parameter of glMultiDrawElements. */
    const std::vector<GLsizei>& countVector() const { return mCountVector; }

    /** Returns the list of base vertices, one for each primitive. This will enable the use 
      * of glMultiDrawElementsBaseVertex() to render a set of primitives. 
      * See also http://www.opengl.org/sdk/docs/man3/xhtml/glMultiDrawElementsBaseVertex.xml */
    void setBaseVertices(const std::vector<GLint>& base_verts) { mBaseVertices = base_verts; }

    /** Returns the list of base vertices, one for each primitive. */
    const std::vector<GLint>& baseVertices() const { return mBaseVertices; }

    /** Returns the list of base vertices, one for each primitive. */
    std::vector<GLint>& baseVertices() { return mBaseVertices; }

  protected:
    virtual void compute_pointer_vector() = 0;

  protected:
    GLuint mPrimitiveRestartIndex;
    bool mPrimitiveRestartEnabled;
    std::vector<GLsizei> mCountVector;
    std::vector<GLint>   mBaseVertices;
  };
  //------------------------------------------------------------------------------
  // MultiDrawElements
  //------------------------------------------------------------------------------
  /** 
   * Wrapper for the OpenGL function glMultiDrawElements(). See also http://www.opengl.org/sdk/docs/man/xhtml/glMultiDrawElements.xml for more information. 
   *
   * Features supported: 
   * - <b>multi instancing</b>: NO 
   * - <b>base vertex</b>: YES
   * - <b>primitive restart</b>: YES
   *
   * Use the functions setPrimitiveRestartIndex() and setPrimitiveRestartEnabled() to use the <b>primitive 
   * restart</b> functionality (requires OpenGL 3.1). For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glPrimitiveRestartIndex.xml
   *
   * Use the function setBaseVertices() to use the <b>base vertex</b> functionality. 
   * Requires OpenGL 3.2 or GL_ARB_draw_elements_base_vertex. For more information see http://www.opengl.org/sdk/docs/man3/xhtml/glMultiDrawElementsBaseVertex.xml
   *
   * DrawElements, MultiDrawElements, DrawRangeElements, DrawArrays are used by Geometry to define a set of primitives to be rendered, see Geometry::drawCalls().
   * The indices are stored in a GLBufferObject and thus they can be stored locally or on the GPU. 
   * To gain direct access to the GLBufferObject use the indices() function.
   *
   * \sa DrawCall, DrawElements, DrawRangeElements, DrawArrays, Geometry, Actor */
  template <GLenum Tgltype, class arr_type>
  class MultiDrawElements: public MultiDrawElementsBase
  {
    INSTRUMENT_CLASS(vl::MultiDrawElements, MultiDrawElementsBase)

  public:
    MultiDrawElements(EPrimitiveType primitive = PT_TRIANGLES)
    {
      VL_DEBUG_SET_OBJECT_NAME()
      mType                    = primitive;
      mIndexBuffer             = new arr_type;
      mPrimitiveRestartIndex   = typename arr_type::scalar_type(~0);
      mPrimitiveRestartEnabled = false;
    }

    MultiDrawElements& operator=(const MultiDrawElements& other)
    {
      DrawCall::operator=(other);
      *indices() = *other.indices();
      mPrimitiveRestartEnabled = other.mPrimitiveRestartEnabled;
      mPrimitiveRestartIndex   = other.mPrimitiveRestartIndex;
      setCountVector(other.mCountVector);
      return *this;
    }

    virtual ref<DrawCall> clone() const 
    { 
      ref<MultiDrawElements> de = new MultiDrawElements;
      *de = *this;
      return de;
    }

    virtual unsigned int handle() const { return indices()->gpuBuffer()->handle(); }

    arr_type* indices() { return mIndexBuffer.get(); }

    const arr_type* indices() const { return mIndexBuffer.get(); }

    virtual void updateVBOs(bool discard_local_data=false, bool force_update=false)
    {
      if (indices()->isVBODirty() || force_update)
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

    virtual void render(bool use_vbo) const
    {
      VL_CHECK_OGL()
      VL_CHECK(Has_GL_EXT_multi_draw_arrays||Has_GL_Version_1_4||Has_GL_Version_3_0||Has_GL_Version_4_0);
      VL_CHECK(!use_vbo || (use_vbo && Has_VBO))
      use_vbo &= Has_VBO; // && indices()->gpuBuffer()->handle() && indices()->sizeGPU();
      if ( !use_vbo && !indices()->size() )
        return;

      // apply patch parameters if any and if using PT_PATCHES
      applyPatchParameters();

      // primitive restart enable
      if(primitiveRestartEnabled())
      {
        if(Has_Primitive_Restart)
        {
          glEnable(GL_PRIMITIVE_RESTART); VL_CHECK_OGL();
          glPrimitiveRestartIndex(primitiveRestartIndex()); VL_CHECK_OGL();
        }
        else
        {
          Log::error("Primitive restart not supported by the current OpenGL implementation!\n");
        }
        VL_CHECK(Has_Primitive_Restart);
      }

      const GLvoid **indices_ptr = (const GLvoid**)&mPointerVector[0];
      if (use_vbo && indices()->gpuBuffer()->handle())
      {
        VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices()->gpuBuffer()->handle()); VL_CHECK_OGL()
        indices_ptr = (const GLvoid**)&mNULLPointerVector[0];
      }
      else
        VL_glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      if (baseVertices().size())
      {
        VL_CHECK( baseVertices().size() == pointerVector().size() )
        VL_CHECK( baseVertices().size() == countVector().size() )
        if (Has_GL_ARB_draw_elements_base_vertex||Has_GL_Version_3_1||Has_GL_Version_4_0)
        {
          glMultiDrawElementsBaseVertex( primitiveType(), (GLsizei*)&mCountVector[0], indices()->glType(), indices_ptr, (GLsizei)mCountVector.size(), (GLint*)&mBaseVertices[0] ); VL_CHECK_OGL()
        }
        else
        {
          vl::Log::error("MultiDrawElements::render(): glMultiDrawElementsBaseVertex() not supported!\n"
            "OpenGL 3.1 or GL_ARB_draw_elements_base_vertex extension required.\n"
          );
        }
      }
      else
      {
        glMultiDrawElements( primitiveType(), (GLsizei*)&mCountVector[0], indices()->glType(), (const GLvoid**)indices_ptr, (GLsizei)mCountVector.size() ); VL_CHECK_OGL()
      }

      // primitive restart disable
      if(primitiveRestartEnabled())
      {
        glDisable(GL_PRIMITIVE_RESTART); VL_CHECK_OGL()
      }
    }

    TriangleIterator triangleIterator() const;

    IndexIterator indexIterator() const
    {
      ref< IndexIteratorElements<arr_type> > iie = new IndexIteratorElements<arr_type>;
      iie->initialize( mIndexBuffer.get(), &mBaseVertices, &mCountVector, 0, mPrimitiveRestartEnabled, mPrimitiveRestartIndex );
      IndexIterator iit;
      iit.initialize( iie.get() );
      return iit;
    }

    /** The pointer vector used as 'indices' parameter of glMultiDrawElements. */
    const std::vector<const typename arr_type::scalar_type*>& pointerVector() const { return mPointerVector; }

  protected:
    void compute_pointer_vector()
    {
      // set all to null: used when VBO is active.
      mNULLPointerVector.resize( mCountVector.size() );

      // used when VBOs are not active.
      mPointerVector.clear();
      const typename arr_type::scalar_type* ptr = (const typename arr_type::scalar_type*)indices()->gpuBuffer()->ptr();
      for(size_t i=0; i<mCountVector.size(); ++i)
      {
        mPointerVector.push_back(ptr);
        ptr += mCountVector[i];
      }
      VL_CHECK( ptr - (const typename arr_type::scalar_type*)indices()->gpuBuffer()->ptr() <= (int)indices()->size() );
    }


  protected:
    ref< arr_type > mIndexBuffer;
    std::vector<const typename arr_type::scalar_type*> mPointerVector;
    std::vector<const typename arr_type::scalar_type*> mNULLPointerVector;
  };
  //------------------------------------------------------------------------------
  // typedefs
  //------------------------------------------------------------------------------
  /** See MultiDrawElements. A MultiDrawElements using indices of type \p GLuint. */
  class MultiDrawElementsUInt: public MultiDrawElements<GL_UNSIGNED_INT, ArrayUInt1>
  {
    INSTRUMENT_CLASS(vl::MultiDrawElementsUInt, VL_GROUP(MultiDrawElements<GL_UNSIGNED_INT, ArrayUInt1>))

  public:
    MultiDrawElementsUInt(EPrimitiveType primitive = PT_TRIANGLES)
    :MultiDrawElements<GL_UNSIGNED_INT, ArrayUInt1>(primitive)
    {
      VL_DEBUG_SET_OBJECT_NAME();
    }
  };
  //------------------------------------------------------------------------------
  /** See MultiDrawElements. A MultiDrawElements using indices of type \p GLushort. */
  class MultiDrawElementsUShort: public MultiDrawElements<GL_UNSIGNED_SHORT, ArrayUShort1>
  {
    INSTRUMENT_CLASS(vl::MultiDrawElementsUShort, VL_GROUP(MultiDrawElements<GL_UNSIGNED_SHORT, ArrayUShort1>))

  public:
    MultiDrawElementsUShort(EPrimitiveType primitive = PT_TRIANGLES)
    :MultiDrawElements<GL_UNSIGNED_SHORT, ArrayUShort1>(primitive)
    {
      VL_DEBUG_SET_OBJECT_NAME();
    }
  };
  //------------------------------------------------------------------------------
  /** See MultiDrawElements. A MultiDrawElements using indices of type \p GLubyte. */
  class MultiDrawElementsUByte: public MultiDrawElements<GL_UNSIGNED_BYTE, ArrayUByte1>
  {
    INSTRUMENT_CLASS(vl::MultiDrawElementsUByte, VL_GROUP(MultiDrawElements<GL_UNSIGNED_BYTE, ArrayUByte1>))

  public:
    MultiDrawElementsUByte(EPrimitiveType primitive = PT_TRIANGLES)
    :MultiDrawElements<GL_UNSIGNED_BYTE, ArrayUByte1>(primitive)
    {
      VL_DEBUG_SET_OBJECT_NAME();
    }
  };
//-----------------------------------------------------------------------------
  template <GLenum Tgltype, class arr_type>
  TriangleIterator MultiDrawElements<Tgltype, arr_type>::triangleIterator() const
  {
    ref< TriangleIteratorMulti<arr_type> > it = 
      new TriangleIteratorMulti<arr_type>( &mBaseVertices, &mCountVector, mIndexBuffer.get(), primitiveType(), 
          primitiveRestartEnabled(), primitiveRestartIndex() );
    it->initialize();
    return TriangleIterator(it.get());
  }
//-----------------------------------------------------------------------------
}

#endif
