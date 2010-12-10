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

#include <vl/GLBufferObject.hpp>
#include <vl/half.hpp>
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
   * - vl::ArrayFloat, vl::ArrayFVec2, vl::ArrayFVec3, vl::ArrayFVec4
   * - vl::ArrayDouble, vl::ArrayDVec2, vl::ArrayDVec3, vl::ArrayDVec4
   * - vl::ArrayInt, vl::ArrayIVec2, vl::ArrayIVec3,  vl::ArrayIVec4
   * - vl::ArrayUInt, vl::ArrayUVec2, vl::ArrayUVec3, vl::ArrayUVec4
   * - vl::ArrayByte, vl::ArrayBVec2, vl::ArrayBVec3, vl::ArrayBVec4
   * - vl::ArrayUByte, vl::ArrayUBVec2, vl::ArrayUBVec3, vl::ArrayUBVec4
   * - vl::ArrayShort, vl::ArraySVec2, vl::ArraySVec3, vl::ArraySVec4
   * - vl::ArrayUShort, vl::ArrayUSVec2, vl::ArrayUSVec3, vl::ArrayUSVec4
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

    //! Returns the number of scalar components for the array, ie 3 for ArrayFVec3, 1 for ArrayUInt etc.
    virtual size_t glSize() const = 0;

    //! Returns the OpenGL type for the array, ie GL_FLOAT for ArrayFVec3, GL_UNSIGNED_INT for ArrayUInt etc.
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
   * - vl::ArrayFloat, vl::ArrayFVec2, vl::ArrayFVec3, vl::ArrayFVec4
   * - vl::ArrayDouble, vl::ArrayDVec2, vl::ArrayDVec3, vl::ArrayDVec4
   * - vl::ArrayInt, vl::ArrayIVec2, vl::ArrayIVec3,  vl::ArrayIVec4
   * - vl::ArrayUInt, vl::ArrayUVec2, vl::ArrayUVec3, vl::ArrayUVec4
   * - vl::ArrayByte, vl::ArrayBVec2, vl::ArrayBVec3, vl::ArrayBVec4
   * - vl::ArrayUByte, vl::ArrayUBVec2, vl::ArrayUBVec3, vl::ArrayUBVec4
   * - vl::ArrayShort, vl::ArraySVec2, vl::ArraySVec3, vl::ArraySVec4
   * - vl::ArrayUShort, vl::ArrayUSVec2, vl::ArrayUSVec3, vl::ArrayUSVec4
  */
  template <typename T_vector_type, typename T_scalar_type, int T_gl_size, GLenum T_gl_type>
  class Array: public ArrayAbstract
  {
  public:
    virtual const char* className() { return "Array"; }
    typedef T_scalar_type scalar_type;

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
        const T_scalar_type* pv = reinterpret_cast<const T_scalar_type*>(&at(i));
        for( int j=0; j<count; ++j )
          v.ptr()[j] = (Real)pv[j];
        aabb += v;
      }
      Real radius = 0;
      vec3 center = aabb.center();
      for(size_t i=0; i<size(); ++i)
      {
        vec3 v;
        const T_scalar_type* pv = reinterpret_cast<const T_scalar_type*>(&at(i));
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
        const T_scalar_type* pv = reinterpret_cast<const T_scalar_type*>(&at(i));
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
        T_scalar_type* pv = reinterpret_cast<T_scalar_type*>(&at(i));
        // read
        for( size_t j=0; j<T_gl_size; ++j )
          v.ptr()[j] = (Real)pv[j];
        // transform
        v = m * v;
        // write
        for( size_t j=0; j<T_gl_size; ++j )
          pv[j] = (T_scalar_type)v.ptr()[j];
      }
    }

    void normalize()
    {
      for(size_t i=0; i<size(); ++i)
      {
        vec4 v(0,0,0,0);
        T_scalar_type* pv = reinterpret_cast<T_scalar_type*>(&at(i));
        // read
        for( size_t j=0; j<T_gl_size; ++j )
          v.ptr()[j] = (Real)pv[j];
        // normalize
        v.normalize();
        // write
        for( unsigned j=0; j<T_gl_size; ++j )
          pv[j] = (T_scalar_type)v.ptr()[j];
      }
    }

    vec4 vectorAsVec4(size_t vector_index) const
    {
      vec4 v(0,0,0,1);
      const T_scalar_type* pv = reinterpret_cast<const T_scalar_type*>(&at(vector_index));
      for( size_t j=0; j<T_gl_size; ++j )
        v.ptr()[j] = (Real)pv[j];
      return v;
    }

    vec3 vectorAsVec3(size_t vector_index) const
    {
      vec3 v;
      const T_scalar_type* pv = reinterpret_cast<const T_scalar_type*>(&at(vector_index));
      const int count = T_gl_size == 4 ? 3 : T_gl_size;
      for( int j=0; j<count; ++j )
        v.ptr()[j] = (Real)pv[j];
      return v;
    }

    int compare(int a, int b) const
    {
      const T_scalar_type* pa = reinterpret_cast<const T_scalar_type*>(&at(a));
      const T_scalar_type* pb = reinterpret_cast<const T_scalar_type*>(&at(b));
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
  typedef Array<GLfloat, GLfloat, 1, GL_FLOAT> ArrayFloat;
  //! An array of vl::fvec2
  typedef Array<fvec2,   GLfloat, 2, GL_FLOAT> ArrayFVec2;
  //! An array of vl::fvec3
  typedef Array<fvec3,   GLfloat, 3, GL_FLOAT> ArrayFVec3;
  //! An array of vl::fvec4
  typedef Array<fvec4,   GLfloat, 4, GL_FLOAT> ArrayFVec4;

  //! An array of \p GLdouble
  typedef Array<GLdouble, GLdouble, 1, GL_DOUBLE> ArrayDouble;
  //! An array of vl::dvec2
  typedef Array<dvec2,    GLdouble, 2, GL_DOUBLE> ArrayDVec2;
  //! An array of vl::dvec3
  typedef Array<dvec3,    GLdouble, 3, GL_DOUBLE> ArrayDVec3;
  //! An array of vl::dvec4
  typedef Array<dvec4,    GLdouble, 4, GL_DOUBLE> ArrayDVec4;

  //! An array of \p GLint
  typedef Array<GLint, GLint, 1, GL_INT> ArrayInt;
  //! An array of vl::ivec2
  typedef Array<ivec2, GLint, 2, GL_INT> ArrayIVec2;
  //! An array of vl::ivec3
  typedef Array<ivec3, GLint, 3, GL_INT> ArrayIVec3;
  //! An array of vl::ivec4
  typedef Array<ivec4, GLint, 4, GL_INT> ArrayIVec4;

  //! An array of \p GLuint
  typedef Array<GLuint,GLuint, 1, GL_UNSIGNED_INT> ArrayUInt;
  //! An array of vl::uvec2
  typedef Array<uvec2, GLuint, 2, GL_UNSIGNED_INT> ArrayUVec2;
  //! An array of vl::uvec3
  typedef Array<uvec3, GLuint, 3, GL_UNSIGNED_INT> ArrayUVec3;
  //! An array of vl::uvec4
  typedef Array<uvec4, GLuint, 4, GL_UNSIGNED_INT> ArrayUVec4;

  //! An array of \p GLbyte
  typedef Array<GLbyte, GLbyte, 1, GL_BYTE> ArrayByte;
  //! An array of vl::bvec2
  typedef Array<bvec2,  GLbyte, 2, GL_BYTE> ArrayBVec2;
  //! An array of vl::bvec3
  typedef Array<bvec3,  GLbyte, 3, GL_BYTE> ArrayBVec3;
  //! An array of vl::bvec4
  typedef Array<bvec4,  GLbyte, 4, GL_BYTE> ArrayBVec4;

  //! An array of \p GLubyte
  typedef Array<GLubyte, GLubyte, 1, GL_UNSIGNED_BYTE> ArrayUByte;
  //! An array of vl::ubvec2
  typedef Array<ubvec2,  GLubyte, 2, GL_UNSIGNED_BYTE> ArrayUBVec2;
  //! An array of vl::ubvec3
  typedef Array<ubvec3,  GLubyte, 3, GL_UNSIGNED_BYTE> ArrayUBVec3;
  //! An array of vl::ubvec4
  typedef Array<ubvec4,  GLubyte, 4, GL_UNSIGNED_BYTE> ArrayUBVec4;

  //! An array of \p GLshort
  typedef Array<GLshort, GLshort, 1, GL_SHORT> ArrayShort;
  //! An array of vl::svec2
  typedef Array<svec2,   GLshort, 2, GL_SHORT> ArraySVec2;
  //! An array of vl::svec3
  typedef Array<svec3,   GLshort, 3, GL_SHORT> ArraySVec3;
  //! An array of vl::svec4
  typedef Array<svec4,   GLshort, 4, GL_SHORT> ArraySVec4;

  //! An array of \p GLushort
  typedef Array<GLushort, GLushort, 1, GL_UNSIGNED_SHORT> ArrayUShort;
  //! An array of vl::usvec2
  typedef Array<usvec2,   GLushort, 2, GL_UNSIGNED_SHORT> ArrayUSVec2;
  //! An array of vl::usvec3
  typedef Array<usvec3,   GLushort, 3, GL_UNSIGNED_SHORT> ArrayUSVec3;
  //! An array of vl::usvec4
  typedef Array<usvec4,   GLushort, 4, GL_UNSIGNED_SHORT> ArrayUSVec4;

  //! An array of \p GL_HALF_FLOAT
  typedef Array<half,  half, 1, GL_HALF_FLOAT> ArrayHFloat;
  //! An array of 2d GL_HALF_FLOAT vectors
  typedef Array<hvec2, half, 2, GL_HALF_FLOAT> ArrayHFVec2;
  //! An array of 3d GL_HALF_FLOAT vectors
  typedef Array<hvec3, half, 3, GL_HALF_FLOAT> ArrayHFVec3;
  //! An array of 4d GL_HALF_FLOAT vectors
  typedef Array<hvec4, half, 4, GL_HALF_FLOAT> ArrayHFVec4;

  //! An array of \p GL_FIXED
  typedef Array<GLuint,GLuint, 1, GL_FIXED> ArrayFixed;
  //! An array 2d GL_FIXED vectors
  typedef Array<uvec2, GLuint, 2, GL_FIXED> ArrayFixedVec2;
  //! An array 3d GL_FIXED vectors
  typedef Array<uvec3, GLuint, 3, GL_FIXED> ArrayFixedVec3;
  //! An array 4d GL_FIXED vectors
  typedef Array<uvec4, GLuint, 4, GL_FIXED> ArrayFixedVec4;

  //! An array of \p GL_INT_2_10_10_10_REV
  typedef Array<GLint, GLint, 1, GL_INT_2_10_10_10_REV> ArrayInt_2_10_10_10_REV;
  //! An array of 2d GL_INT_2_10_10_10_REV vectors
  typedef Array<ivec2, GLint, 2, GL_INT_2_10_10_10_REV> ArrayIVec2_2_10_10_10_REV;
  //! An array of 3d GL_INT_2_10_10_10_REV vectors
  typedef Array<ivec3, GLint, 3, GL_INT_2_10_10_10_REV> ArrayIVec3_2_10_10_10_REV;
  //! An array of 4d GL_INT_2_10_10_10_REV vectors
  typedef Array<ivec4, GLint, 4, GL_INT_2_10_10_10_REV> ArrayIVec4_2_10_10_10_REV;

  //! An array of \p GL_UNSIGNED_INT_2_10_10_10_REV
  typedef Array<GLuint,GLuint, 1, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUInt_2_10_10_10_REV;
  //! An array of 2d GL_UNSIGNED_INT_2_10_10_10_REV vectors
  typedef Array<uvec2, GLuint, 2, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUVec2_2_10_10_10_REV;
  //! An array of 3d GL_UNSIGNED_INT_2_10_10_10_REV vectors
  typedef Array<uvec3, GLuint, 3, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUVec3_2_10_10_10_REV;
  //! An array of 4d GL_UNSIGNED_INT_2_10_10_10_REV vectors
  typedef Array<uvec4, GLuint, 4, GL_UNSIGNED_INT_2_10_10_10_REV> ArrayUVec4_2_10_10_10_REV;
}

#endif
