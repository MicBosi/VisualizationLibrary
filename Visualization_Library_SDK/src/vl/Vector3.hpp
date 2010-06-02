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

#ifndef Vector3_INCLUDE_ONCE
#define Vector3_INCLUDE_ONCE

#include <vl/Vector2.hpp>

namespace vl
{
  /**
   * The Vector3 class is a template class that implements a generic 3 components vector, see also vl::fvec3, vl::dvec3, vl::uvec3, vl::ivec3, vl::svec3, vl::usvec3, vl::bvec3, vl::ubvec3.
   * \sa Vector4, Vector2, Matrix4, Matrix3, Matrix2
   */
  template<typename scalar_type>
  class Vector3
  {
  public:
    static const int scalar_count = 3;
    Vector3(const Vector3& other) { *this = other; }
    Vector3() { x() = y() = z() = 0; }

    template<class T>
    explicit Vector3(const T& other)
    {
      x() = (scalar_type)other.x();
      y() = (scalar_type)other.y();
      z() = (scalar_type)other.z();
    }

    explicit Vector3(scalar_type x, scalar_type y, scalar_type z)
    {
      mScalar[0] = x;
      mScalar[1] = y;
      mScalar[2] = z;
    }

    explicit Vector3(const Vector2<scalar_type>& v, scalar_type z)
    {
      mScalar[0] = v.x();
      mScalar[1] = v.y();
      mScalar[2] = z;
    }

    scalar_type* ptr() { return mScalar; }
    const scalar_type* ptr() const { return mScalar; }

    const scalar_type& x() const { return mScalar[0]; }
    const scalar_type& y() const { return mScalar[1]; }
    const scalar_type& z() const { return mScalar[2]; }

    scalar_type& x() { return mScalar[0]; }
    scalar_type& y() { return mScalar[1]; }
    scalar_type& z() { return mScalar[2]; }

    const scalar_type& r() const { return mScalar[0]; }
    const scalar_type& g() const { return mScalar[1]; }
    const scalar_type& b() const { return mScalar[2]; }

    scalar_type& r() { return mScalar[0]; }
    scalar_type& g() { return mScalar[1]; }
    scalar_type& b() { return mScalar[2]; }

    const scalar_type& s() const { return mScalar[0]; }
    const scalar_type& t() const { return mScalar[1]; }
    const scalar_type& p() const { return mScalar[2]; }

    scalar_type& s() { return mScalar[0]; }
    scalar_type& t() { return mScalar[1]; }
    scalar_type& p() { return mScalar[2]; }

    Vector2<scalar_type> xy() const { return Vector2<scalar_type>(x(),y()); }
    Vector2<scalar_type> st() const { return Vector2<scalar_type>(x(),y()); }

    Vector3 operator+(const Vector3& other) const
    {
      return Vector3(x()+other.x(), y()+other.y(), z()+other.z());
    }
    Vector3 operator-(const Vector3& other) const
    {
      return Vector3(x()-other.x(), y()-other.y(), z()-other.z());
    }
    Vector3 operator*(const Vector3& other) const
    {
      return Vector3(x()*other.x(), y()*other.y(), z()*other.z());
    }
    Vector3 operator/(const Vector3& other) const
    {
      return Vector3(x()/other.x(), y()/other.y(), z()/other.z());
    }
    Vector3 operator+(scalar_type val) const
    {
      return Vector3(x()+val, y()+val, z()+val);
    }
    Vector3 operator-(scalar_type val) const
    {
      return Vector3(x()-val, y()-val, z()-val);
    }
    Vector3 operator*(scalar_type val) const
    {
      return Vector3(x()*val, y()*val, z()*val);
    }
    Vector3 operator/(scalar_type val) const
    {
      return Vector3(x()/val, y()/val, z()/val);
    }
    Vector3 operator-() const
    {
      return Vector3(-x(), -y(), -z());
    }
    Vector3& operator+=(const Vector3& other)
    {
      *this = *this + other;
      return *this;
    }
    Vector3& operator-=(const Vector3& other)
    {
      *this = *this - other;
      return *this;
    }
    Vector3& operator*=(const Vector3& other)
    {
      *this = *this * other;
      return *this;
    }
    Vector3& operator/=(const Vector3& other)
    {
      *this = *this / other;
      return *this;
    }
    Vector3& operator+=(scalar_type val)
    {
      *this = *this + val;
      return *this;
    }
    Vector3& operator-=(scalar_type val)
    {
      *this = *this - val;
      return *this;
    }
    Vector3& operator*=(scalar_type val)
    {
      *this = *this * val;
      return *this;
    }
    Vector3& operator/=(scalar_type val)
    {
      *this = *this / val;
      return *this;
    }
    Vector3& operator=(const Vector3& other)
    {
      x() = other.x();
      y() = other.y();
      z() = other.z();
      return *this;
    }
    Vector3& operator=(scalar_type val)
    {
      x() = y() = z() = val;
      return *this;
    }
    bool operator==(const Vector3& other) const
    {
      return x() == other.x() && y() == other.y() && z() == other.z();
    }
    bool operator!=(const Vector3& other) const
    {
      return !operator==(other);
    }
    bool operator<(const Vector3& other) const
    {
      if (x() != other.x())
        return x() < other.x();
      else
      if (y() != other.y())
        return y() < other.y();
      else
        return z() < other.z();
    }
    scalar_type& operator[](unsigned i) { return mScalar[i]; }
    const scalar_type& operator[](unsigned i) const { return mScalar[i]; }
    scalar_type length() const { return ::sqrt(x()*x()+y()*y()+z()*z()); }
    scalar_type lengthSquared() const { return x()*x()+y()*y()+z()*z(); }
    bool isNull() const { return !x() && !y() && !z(); }
    const Vector3& normalize()
    {
      scalar_type l = length();
      if (l)
        *this *= (scalar_type)(1.0/l); 
      return *this; 
    }

  protected:
    scalar_type mScalar[scalar_count];
  };

  template<typename T>
  inline const Vector3<T> operator*(T val, const Vector3<T>& v)
  {
    return v * val;
  }

  //! A 3 components vector with \p GLint precision.
  typedef Vector3<GLint> ivec3;
  //! A 3 components vector with \p GLuint precision.
  typedef Vector3<GLuint> uvec3;
  //! A 3 components vector with \p GLfloat precision.
  typedef Vector3<GLfloat> fvec3;
  //! A 3 components vector with \p GLdouble precision.
  typedef Vector3<GLdouble> dvec3;
  //! A 3 components vector with \p GLbyte precision.
  typedef Vector3<GLbyte> bvec3;
  //! A 3 components vector with \p GLubyte precision.
  typedef Vector3<GLubyte> ubvec3;
  //! A 3 components vector with \p GLshort precision.
  typedef Vector3<GLshort> svec3;
  //! A 3 components vector with \p GLushort precision.
  typedef Vector3<GLushort> usvec3;

  #if VL_PIPELINE_PRECISION == 2
    //! Defined as: \p 'typedef \p dvec3 \p vec3'. See also \ref VL_PIPELINE_PRECISION.
    typedef dvec3 vec3;
  #else
    //! Defined as: \p 'typedef \p fvec3 \p vec3'. See also \ref VL_PIPELINE_PRECISION.
    typedef fvec3 vec3;
  #endif
}

#endif
