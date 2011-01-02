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

#ifndef Array_INCLUDE_ONCE
#define Array_INCLUDE_ONCE

#include <vlGraphics/GLBufferObject.hpp>
#include <vlCore/half.hpp>
#include <vector>

namespace vl
{
//-----------------------------------------------------------------------------
// ArrayAbstract
//-----------------------------------------------------------------------------
  /**
   * The ArrayAbstract class defines an abstract interface to conveniently manipulate data stored in a GLBufferObject.
   * \sa
   *
   * - vl::Array
   * - vl::ArrayFloat1, vl::ArrayFloat2, vl::ArrayFloat3, vl::ArrayFloat4
   * - vl::ArrayDouble1, vl::ArrayDouble2, vl::ArrayDouble3, vl::ArrayDouble4
   * - vl::ArrayInt1, vl::ArrayInt2, vl::ArrayInt3,  vl::ArrayInt4
   * - vl::ArrayUInt1, vl::ArrayUInt2, vl::ArrayUInt3, vl::ArrayUInt4
   * - vl::ArrayByte1, vl::ArrayByte2, vl::ArrayByte3, vl::ArrayByte4
   * - vl::ArrayUByte1, vl::ArrayUByte2, vl::ArrayUByte3, vl::ArrayUByte4
   * - vl::ArrayShort1, vl::ArrayShort2, vl::ArrayShort3, vl::ArrayShort4
   * - vl::ArrayUShort1, vl::ArrayUShort2, vl::ArrayUShort3, vl::ArrayUShort4
   */
  class ArrayAbstract: public Object
  {
  public:
    virtual const char* className() { return "ArrayAbstract"; }

    //! Default constructor.
    ArrayAbstract()
    { 
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mBufferGPU = new GLBufferObject;
      mVBODirty = true;
      mVBOUsage = vl::GBU_STATIC_DRAW;
    }

    //! Copies only the local data and not the VBO related fields
    ArrayAbstract(const ArrayAbstract& other): Object(other) 
    {
      operator=(other);
    }
    //! Copies only the local data and not the VBO related fields
    void operator=(const ArrayAbstract& other) 
    {
      gpuBuffer()->resize( other.gpuBuffer()->bytesUsed() );
      memcpy( ptr(), other.ptr(), bytesUsed() );
      setVBODirty(true);
    }

    virtual ref<ArrayAbstract> clone() const = 0;

    const GLBufferObject* gpuBuffer() const { return mBufferGPU.get(); }
    GLBufferObject* gpuBuffer() { return mBufferGPU.get(); }

    void clear() { if (gpuBuffer()) gpuBuffer()->clear(); }

    //! Returns the pointer to the first element of the local buffer. Equivalent to gpuBuffer()->ptr()
    const unsigned char* ptr() const { return gpuBuffer() ? gpuBuffer()->ptr() : NULL; }

    //! Returns the pointer to the first element of the local buffer. Equivalent to gpuBuffer()->ptr()
    unsigned char* ptr() { return gpuBuffer() ? gpuBuffer()->ptr() : NULL; }

    //! Returns the amount of memory in bytes used by an array. Equivalent to gpuBuffer()->bytesUsed().
    virtual size_t bytesUsed() const { return gpuBuffer() ? gpuBuffer()->bytesUsed() : 0; }

    //! Returns the number of scalar components for the array, ie 3 for ArrayFloat3, 1 for ArrayUInt1 etc.
    virtual size_t glSize() const = 0;

    //! Returns the OpenGL type for the array, ie GL_FLOAT for ArrayFloat3, GL_UNSIGNED_INT for ArrayUInt1 etc.
    virtual int glType() const = 0;

    //! Returns the number of elements of an array
    virtual size_t size() const = 0;

    //! Computes the bounding sphere enclosing the vectors contained in the buffer.
    virtual Sphere computeBoundingSphere() const = 0;

    //! Computes the axis aligned bounding box enclosing the vectors contained in the buffer.
    virtual AABB computeBoundingBox() const = 0;

    //! Transforms the vectors contained in the buffer
    virtual void transform(const mat4& m) = 0;

    //! Normalizes the vectors contained in the buffer
    virtual void normalize() = 0;

    //! Returns a vector from the buffer as a \p vec4 value.
    virtual vec4 vectorAsVec4(size_t vector_index) const = 0;

    //! Returns a vector from the buffer as a \p vec3 value.
    virtual vec3 vectorAsVec3(size_t vector_index) const = 0;

    //! Compares two vectors
    virtual int compare(int a, int b) const = 0;

    //! true by default
    bool isVBODirty() const { return mVBODirty; }

    //! true by default
    void setVBODirty(bool dirty) { mVBODirty = dirty; }

    //! GBU_STATIC_DRAW by default
    EGLBufferUsage usage() const { return mVBOUsage; }

    //! GBU_STATIC_DRAW by default
    void setUsage(EGLBufferUsage usage) { mVBOUsage = usage; }

    void updateVBO(bool discard_local_storage=false)
    {
      if (isVBODirty())
      {
        gpuBuffer()->setBufferData(usage(),discard_local_storage);
        setVBODirty(false);
      }
    }

  protected:
    ref<GLBufferObject> mBufferGPU;
    EGLBufferUsage mVBOUsage;
    bool mVBODirty;
  };
//-----------------------------------------------------------------------------
// Array
//-----------------------------------------------------------------------------
  /**
   * The Array class is a template array used to conveniently manipulate data stored in a GLBufferObject.
   * \sa
   *
   * - ArrayAbstract
   * - vl::ArrayFloat1, vl::ArrayFloat2, vl::ArrayFloat3, vl::ArrayFloat4
   * - vl::ArrayDouble1, vl::ArrayDouble2, vl::ArrayDouble3, vl::ArrayDouble4
   * - vl::ArrayInt1, vl::ArrayInt2, vl::ArrayInt3,  vl::ArrayInt4
   * - vl::ArrayUInt1, vl::ArrayUInt2, vl::ArrayUInt3, vl::ArrayUInt4
   * - vl::ArrayByte1, vl::ArrayByte2, vl::ArrayByte3, vl::ArrayByte4
   * - vl::ArrayUByte1, vl::ArrayUByte2, vl::ArrayUByte3, vl::ArrayUByte4
   * - vl::ArrayShort1, vl::ArrayShort2, vl::ArrayShort3, vl::ArrayShort4
   * - vl::ArrayUShort1, vl::ArrayUShort2, vl::ArrayUShort3, vl::ArrayUShort4
   * - vl::ArrayHFloat1, vl::ArrayHFloat2, vl::ArrayHFloat3, vl::ArrayHFloat4
   * - vl::ArrayFixed1, vl::ArrayFixed2, vl::ArrayFixed3, vl::ArrayFixed4
   * - vl::ArrayInt_2_10_10_10_REV1, ArrayInt_2_10_10_10_REV2, ArrayInt_2_10_10_10_REV3, ArrayInt_2_10_10_10_REV4
   * - vl::ArrayUInt_2_10_10_10_REV1, ArrayUInt_2_10_10_10_REV2, ArrayUInt_2_10_10_10_REV3, ArrayUInt_2_10_10_10_REV4
  */
  template <typename T_vector_type, typename T_scalar, int T_gl_size, GLenum T_gl_type>
  class Array: public ArrayAbstract
  {
  public:
    virtual const char* className() { return "Array"; }
    typedef T_scalar scalar_type;

    virtual size_t glSize() const { return T_gl_size; }
    virtual int glType() const { return T_gl_type; }

    virtual ref<ArrayAbstract> clone() const
    {
      ref<Array> arr = new Array;
      if (size())
      {
        arr->resize(size());
        memcpy(arr->ptr(), ptr(), bytesUsed());
      }
      return arr;
    }
    
    void clear() { resize(0); gpuBuffer()->deleteGLBufferObject(); }
    void resize(size_t dim) { gpuBuffer()->resize(dim*bytesPerVector()); setVBODirty(true); }
    size_t size() const { return bytesUsed() / bytesPerVector(); }
    size_t sizeGPU() const { return gpuBuffer() ? gpuBuffer()->byteCountGPU() / bytesPerVector() : 0; }
    size_t scalarCount() const { return size() * glSize(); }
    size_t scalarCountGPU() const { return sizeGPU() * glSize(); }
    
    const T_vector_type* begin() const { return reinterpret_cast<const T_vector_type*>(ptr()); }
    T_vector_type* begin() { return reinterpret_cast<T_vector_type*>(ptr()); }
    
    const T_vector_type* end() const { return (reinterpret_cast<const T_vector_type*>(ptr()))+size(); }
    T_vector_type* end() { return (reinterpret_cast<T_vector_type*>(ptr()))+size(); }
    
    T_vector_type& at(size_t i) { VL_CHECK(i<size()); return *(reinterpret_cast<T_vector_type*>(ptr())+i); }
    const T_vector_type& at(size_t i) const { VL_CHECK(i<size()); return *(reinterpret_cast<const T_vector_type*>(ptr())+i); }

    T_vector_type& operator[](size_t i) { return at(i); }
    const T_vector_type& operator[](size_t i) const { return at(i); }

    virtual size_t bytesPerVector() const { return sizeof(T_vector_type); }
    
    Sphere computeBoundingSphere() const
    {
      AABB aabb;
      const int count = T_gl_size == 4 ? 3 : T_gl_size;
      for(size_t i=0; i<size(); ++i)
      {
        vec3 v;
        const T_scalar* pv = reinterpret_cast<const T_scalar*>(&at(i));
        for( int j=0; j<count; ++j )
          v.ptr()[j] = (Real)pv[j];
        aabb += v;
      }
      Real radius = 0;
      vec3 center = aabb.center();
      for(size_t i=0; i<size(); ++i)
      {
        vec3 v;
        const T_scalar* pv = reinterpret_cast<const T_scalar*>(&at(i));
        for( int j=0; j<count; ++j )
          v.ptr()[j] = (Real)pv[j];
        Real r = (v-center).lengthSquared();
        if (r > radius)
          radius = r;
      }
      return Sphere( center, sqrt(radius) );
    }

    AABB computeBoundingBox() const
    { 
      AABB aabb;
      const int count = T_gl_size == 4 ? 3 : T_gl_size;
      for(size_t i=0; i<size(); ++i)
      {
        vec3 v;
        const T_scalar* pv = reinterpret_cast<const T_scalar*>(&at(i));
        for( int j=0; j<count; ++j )
          v.ptr()[j] = (Real)pv[j];
        aabb += v;
      }
      return aabb;
    }

    void transform(const mat4& m)
    {
      for(size_t i=0; i<size(); ++i)
      {
        vec4 v(0,0,0,1);
        T_scalar* pv = reinterpret_cast<T_scalar*>(&at(i));
        // read
        for( size_t j=0; j<T_gl_size; ++j )
          v.ptr()[j] = (Real)pv[j];
        // transform
        v = m * v;
        // write
        for( size_t j=0; j<T_gl_size; ++j )
          pv[j] = (T_scalar)v.ptr()[j];
      }
    }

    void normalize()
    {
      for(size_t i=0; i<size(); ++i)
      {
        vec4 v(0,0,0,0);
        T_scalar* pv = reinterpret_cast<T_scalar*>(&at(i));
        // read
        for( size_t j=0; j<T_gl_size; ++j )
          v.ptr()[j] = (Real)pv[j];
        // normalize
        v.normalize();
        // write
        for( unsigned j=0; j<T_gl_size; ++j )
          pv[j] = (T_scalar)v.ptr()[j];
      }
    }

    vec4 vectorAsVec4(size_t vector_index) const
    {
      vec4 v(0,0,0,1);
      const T_scalar* pv = reinterpret_cast<const T_scalar*>(&at(vector_index));
      for( size_t j=0; j<T_gl_size; ++j )
        v.ptr()[j] = (Real)pv[j];
      return v;
    }

    vec3 vectorAsVec3(size_t vector_index) const
    {
      vec3 v;
      const T_scalar* pv = reinterpret_cast<const T_scalar*>(&at(vector_index));
      const int count = T_gl_size == 4 ? 3 : T_gl_size;
      for( int j=0; j<count; ++j )
        v.ptr()[j] = (Real)pv[j];
      return v;
    }

    int compare(int a, int b) const
    {
      const T_scalar* pa = reinterpret_cast<const T_scalar*>(&at(a));
      const T_scalar* pb = reinterpret_cast<const T_scalar*>(&at(b));
      for( size_t i=0; i<T_gl_size; ++i )
        if ( pa[i] != pb[i] )
          return pa[i] < pb[i] ? -1 : +1;        
      return 0;
    }

    void operator=(const std::vector<T_vector_type>& vector)
    {
      resize(vector.size());
      if (vector.empty())
        return;
      else
        memcpy(ptr(),&vector[0],sizeof(vector[0])*vector.size());
    }
  };
//-----------------------------------------------------------------------------
// Array typedefs
//-----------------------------------------------------------------------------
  //! An array of \p GLfloat
  typedef Array<GLfloat, GLfloat, 1, GL_FLOAT> ArrayFloat1;
  //! An array of vl::fvec2
  typedef Array<fvec2,   GLfloat, 2, GL_FLOAT> ArrayFloat2;
  //! An array of vl::fvec3
  typedef Array<fvec3,   GLfloat, 3, GL_FLOAT> ArrayFloat3;
  //! An array of vl::fvec4
  typedef Array<fvec4,   GLfloat, 4, GL_FLOAT> ArrayFloat4;

  //! An array of \p GLdouble
  typedef Array<GLdouble, GLdouble, 1, GL_DOUBLE> ArrayDouble1;
  //! An array of vl::dvec2
  typedef Array<dvec2,    GLdouble, 2, GL_DOUBLE> ArrayDouble2;
  //! An array of vl::dvec3
  typedef Array<dvec3,    GLdouble, 3, GL_DOUBLE> ArrayDouble3;
  //! An array of vl::dvec4
  typedef Array<dvec4,    GLdouble, 4, GL_DOUBLE> ArrayDouble4;

  //! An array of \p GLint
  typedef Array<GLint, GLint, 1, GL_INT> ArrayInt1;
  //! An array of vl::ivec2
  typedef Array<ivec2, GLint, 2, GL_INT> ArrayInt2;
  //! An array of vl::ivec3
  typedef Array<ivec3, GLint, 3, GL_INT> ArrayInt3;
  //! An array of vl::ivec4
  typedef Array<ivec4, GLint, 4, GL_INT> ArrayInt4;

  //! An array of \p GLuint
  typedef Array<GLuint,GLuint, 1, GL_UNSIGNED_INT> ArrayUInt1;
  //! An array of vl::uvec2
  typedef Array<uvec2, GLuint, 2, GL_UNSIGNED_INT> ArrayUInt2;
  //! An array of vl::uvec3
  typedef Array<uvec3, GLuint, 3, GL_UNSIGNED_INT> ArrayUInt3;
  //! An array of vl::uvec4
  typedef Array<uvec4, GLuint, 4, GL_UNSIGNED_INT> ArrayUInt4;

  //! An array of \p GLbyte
  typedef Array<GLbyte, GLbyte, 1, GL_BYTE> ArrayByte1;
  //! An array of vl::bvec2
  typedef Array<bvec2,  GLbyte, 2, GL_BYTE> ArrayByte2;
  //! An array of vl::bvec3
  typedef Array<bvec3,  GLbyte, 3, GL_BYTE> ArrayByte3;
  //! An array of vl::bvec4
  typedef Array<bvec4,  GLbyte, 4, GL_BYTE> ArrayByte4;

  //! An array of \p GLubyte
  typedef Array<GLubyte, GLubyte, 1, GL_UNSIGNED_BYTE> ArrayUByte1;
  //! An array of vl::ubvec2
  typedef Array<ubvec2,  GLubyte, 2, GL_UNSIGNED_BYTE> ArrayUByte2;
  //! An array of vl::ubvec3
  typedef Array<ubvec3,  GLubyte, 3, GL_UNSIGNED_BYTE> ArrayUByte3;
  //! An array of vl::ubvec4
  typedef Array<ubvec4,  GLubyte, 4, GL_UNSIGNED_BYTE> ArrayUByte4;

  //! An array of \p GLshort
  typedef Array<GLshort, GLshort, 1, GL_SHORT> ArrayShort1;
  //! An array of vl::svec2
  typedef Array<svec2,   GLshort, 2, GL_SHORT> ArrayShort2;
  //! An array of vl::svec3
  typedef Array<svec3,   GLshort, 3, GL_SHORT> ArrayShort3;
  //! An array of vl::svec4
  typedef Array<svec4,   GLshort, 4, GL_SHORT> ArrayShort4;

  //! An array of \p GLushort
  typedef Array<GLushort, GLushort, 1, GL_UNSIGNED_SHORT> ArrayUShort1;
  //! An array of vl::usvec2
  typedef Array<usvec2,   GLushort, 2, GL_UNSIGNED_SHORT> ArrayUShort2;
  //! An array of vl::usvec3
  typedef Array<usvec3,   GLushort, 3, GL_UNSIGNED_SHORT> ArrayUShort3;
  //! An array of vl::usvec4
  typedef Array<usvec4,   GLushort, 4, GL_UNSIGNED_SHORT> ArrayUShort4;

  //! An array of \p GL_HALF_FLOAT
  typedef Array<half,  half, 1, GL_HALF_FLOAT> ArrayHFloat1;
  //! A 2d array of GL_HALF_FLOAT vectors
  typedef Array<hvec2, half, 2, GL_HALF_FLOAT> ArrayHFloat2;
  //! A 3d array of GL_HALF_FLOAT vectors
  typedef Array<hvec3, half, 3, GL_HALF_FLOAT> ArrayHFloat3;
  //! A 4d array of GL_HALF_FLOAT vectors
  typedef Array<hvec4, half, 4, GL_HALF_FLOAT> ArrayHFloat4;

  //! An array of \p GL_FIXED
  typedef Array<GLuint,GLuint, 1, GL_FIXED> ArrayFixed1;
  //! An array 2d GL_FIXED vectors
  typedef Array<uvec2, GLuint, 2, GL_FIXED> ArrayFixed2;
  //! An array 3d GL_FIXED vectors
  typedef Array<uvec3, GLuint, 3, GL_FIXED> ArrayFixed3;
  //! An array 4d GL_FIXED vectors
  typedef Array<uvec4, GLuint, 4, GL_FIXED> ArrayFixed4;

  //! An array of \p GL_INT_2_10_10_10_REV
  typedef Array<GLint, GLint, 1, GL_INT_2_10_10_10_REV> ArrayInt_2_10_10_10_REV1;
  //! A 2d array of GL_INT_2_10_10_10_REV vectors
  typedef Array<ivec2, GLint, 2, GL_INT_2_10_10_10_REV> ArrayInt_2_10_10_10_REV2;
  //! A 3d array of GL_INT_2_10_10_10_REV vectors
  typedef Array<ivec3, GLint, 3, GL_INT_2_10_10_10_REV> ArrayInt_2_10_10_10_REV3;
  //! A 4d array of GL_INT_2_10_10_10_REV vectors
  typedef Array<ivec4, GLint, 4, GL_INT_2_10_10_10_REV> ArrayInt_2_10_10_10_REV4;

  //! An array of \p GL_UNSIGNED_INT_2_10_10_10_REV
  typedef Array<GLuint,GLuint, 1, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUInt_2_10_10_10_REV1;
  //! A 2d array of GL_UNSIGNED_INT_2_10_10_10_REV vectors
  typedef Array<uvec2, GLuint, 2, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUInt_2_10_10_10_REV2;
  //! A 3d array of GL_UNSIGNED_INT_2_10_10_10_REV vectors
  typedef Array<uvec3, GLuint, 3, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUInt_2_10_10_10_REV3;
  //! A 4d array of GL_UNSIGNED_INT_2_10_10_10_REV vectors
  typedef Array<uvec4, GLuint, 4, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUInt_2_10_10_10_REV4;
}

#endif
