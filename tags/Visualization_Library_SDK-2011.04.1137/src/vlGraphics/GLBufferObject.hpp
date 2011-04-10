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

#ifndef GLBufferObject_INCLUDE_ONCE
#define GLBufferObject_INCLUDE_ONCE

#include <vlCore/Vector2.hpp>
#include <vlCore/Vector3.hpp>
#include <vlCore/Vector4.hpp>
#include <vlCore/Buffer.hpp>
#include <vlGraphics/OpenGL.hpp>
#include <vlCore/vlnamespace.hpp>
#include <vlCore/Vector4.hpp>
#include <vlCore/Sphere.hpp>
#include <vlCore/AABB.hpp>

namespace vl
{
//-----------------------------------------------------------------------------
// GLBufferObject
//-----------------------------------------------------------------------------
  /**
   * The GLBufferObject class is a Buffer that can upload its data on the GPU memory. 
   * \remarks
   * GLBufferObject is the storage used by ArrayAbstract and subclasses like ArrayFloat3, ArrayUByte4 etc.
  */
  class GLBufferObject: public Buffer
  {
  public:
    virtual const char* className() { return "vl::GLBufferObject"; }
    GLBufferObject()
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mHandle = 0;
      mUsage = BU_STATIC_DRAW;
      mByteCountGPU = 0;
    }

    GLBufferObject(const GLBufferObject& other): Buffer(other)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mHandle = 0;
      mUsage = BU_STATIC_DRAW;
      mByteCountGPU = 0;
      // copy local data
      *this = other;
    }

    // deletes the GPU data and copyes only the local data
    GLBufferObject& operator=(const GLBufferObject& other) 
    { 
      deleteGLBufferObject();
      Buffer::operator=(other); 
      return *this;
    }

    // swaps the gpu data and update state, as well as local data
    void swap(GLBufferObject& other)
    {
      // swap local data
      Buffer::swap(other);
      // tmp
      unsigned int tmp_handle = mHandle;
      EGLBufferUsage tmp_usage = mUsage;
      GLsizeiptr tmp_bytes = mByteCountGPU;
      // this <- other
      mHandle = other.mHandle;
      mUsage = tmp_usage;
      mByteCountGPU = other.mByteCountGPU;
      // other <- this
      other.mHandle = tmp_handle;
      other.mUsage = tmp_usage;
      other.mByteCountGPU = tmp_bytes;
    }

    ~GLBufferObject()
    {
      deleteGLBufferObject();
    }

    void setHandle(unsigned int handle) { mHandle = handle; }

    unsigned int handle() const { return mHandle; }

    GLsizeiptr byteCountGPU() const { return mByteCountGPU; }

    void createGLBufferObject()
    {
      VL_CHECK(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)
      if(!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0))
        return;
      if (handle() == 0)
      {
        VL_CHECK(mByteCountGPU == 0)
        VL_glGenBuffers( 1, &mHandle );
        mByteCountGPU = 0;
      }
      VL_CHECK(handle())
    }

    void deleteGLBufferObject()
    {
      if (handle() != 0)
      {
        VL_glDeleteBuffers( 1, &mHandle );
        mHandle = 0;
        mByteCountGPU = 0;
      }
    }

    void downloadGLBufferObject()
    {
      if ( handle() )
      {
        resize( byteCountGPU() );
        void* gpu_ptr = mapGPUBuffer(GBA_READ_ONLY);
        memcpy( ptr(), gpu_ptr, byteCountGPU() );
        unmapGPUBuffer();
      }
    }

    // initializes the GPU Buffer from the local storage
    // attention: discarding the local storage might delete data used by other interfaces
    void setBufferData( EGLBufferUsage usage, bool discard_local_storage=false )
    {
      setBufferData( (int)bytesUsed(), ptr(), usage );
      mUsage = usage;
      if (discard_local_storage)
        clear();
    }

    // modifies the GPU Buffer from the local storage
    // attention: discarding the local storage might delete data used by other interfaces
    void setBufferSubData( GLintptr offset=0, GLsizeiptr byte_count=-1, bool discard_local_storage=false )
    {
      byte_count = byte_count < 0 ? byteCountGPU() : byte_count;
      setBufferSubData( offset, byte_count, ptr() );
      if (discard_local_storage)
        clear();
    }

    // if data == NULL the buffer will be allocated but no data will be writte into
    // if data != must point to a buffer of at least 'size' bytes
    void setBufferData( GLsizeiptr byte_count, const GLvoid* data, EGLBufferUsage usage )
    {
      VL_CHECK_OGL();
      VL_CHECK(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)
      if(!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0))
        return;
      //if (!data || !byte_count)
      //  return;
      createGLBufferObject();
      // we use the GL_ARRAY_BUFFER slot to send the data for no special reason
      VL_glBindBuffer( GL_ARRAY_BUFFER, handle() );
      VL_glBufferData( GL_ARRAY_BUFFER, byte_count, data, usage );
      VL_glBindBuffer( GL_ARRAY_BUFFER, 0 );
      mByteCountGPU = byte_count;
      mUsage = usage;
      VL_CHECK_OGL();
    }

    void setBufferSubData( GLintptr offset, GLsizeiptr byte_count, const GLvoid* data )
    {
      VL_CHECK(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)
      if(!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0))
        return;
      createGLBufferObject();
      // we use the GL_ARRAY_BUFFER slot to send the data for no special reason
      VL_glBindBuffer( GL_ARRAY_BUFFER, handle() );
      VL_glBufferSubData( GL_ARRAY_BUFFER, offset, byte_count, data );
      VL_glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }

    // there can be only one GLBufferObject mapped at a time?
    // you must unmapGPUBuffer before using the GPU Buffer again
    void* mapGPUBuffer(EGLBufferAccess access)
    {
      VL_CHECK(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)
      if(!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0))
        return NULL;
      createGLBufferObject(); VL_CHECK_OGL();
      VL_glBindBuffer( GL_ARRAY_BUFFER, handle() ); VL_CHECK_OGL();
      void* ptr = VL_glMapBuffer( GL_ARRAY_BUFFER, access ); VL_CHECK_OGL();
      VL_glBindBuffer( GL_ARRAY_BUFFER, 0 );
      return ptr;
    }

    // From OpenGL 3.0 specs:
    // "UnmapBuffer returns TRUE unless data values in the buffer’s data store have
    // become corrupted during the period that the buffer was mapped. Such corruption
    // can be the result of a screen resolution change or other window system-dependent
    // event that causes system heaps such as those for high-performance graphics memory
    // to be discarded. GL implementations must guarantee that such corruption can
    // occur only during the periods that a buffer’s data store is mapped. If such corruption
    // has occurred, UnmapBuffer returns FALSE, and the contents of the buffer’s
    // data store become undefined."
    bool unmapGPUBuffer()
    {
      VL_CHECK(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)
      if(!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0))
        return false;
      VL_CHECK_OGL();
      createGLBufferObject();
      VL_glBindBuffer( GL_ARRAY_BUFFER, handle() );
      bool ok = VL_glUnmapBuffer( GL_ARRAY_BUFFER ) == GL_TRUE;
      VL_CHECK(ok);
      VL_glBindBuffer( GL_ARRAY_BUFFER, 0 );
      VL_CHECK_OGL();
      return ok;
    }

    EGLBufferUsage usage() const { return mUsage; }

  protected:
    unsigned int mHandle;
    GLsizeiptr mByteCountGPU;
    EGLBufferUsage mUsage;
  };
}

#endif
