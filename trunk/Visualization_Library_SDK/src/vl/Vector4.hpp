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

#ifndef Vector4_INCLUDE_ONCE
#define Vector4_INCLUDE_ONCE

#include <vl/Vector3.hpp>

namespace vl
{
  /**
   * The Vector4 class is a template class that implements a generic 4 components vector, see also vl::fvec4, vl::dvec4, vl::uvec4, vl::ivec4, vl::svec4, vl::usvec4, vl::bvec4, vl::ubvec4.
   * \sa Vector3, Vector2, Matrix4, Matrix3, Matrix2
   */
  template<typename scalar_type>
  class Vector4
  {
  public:
    static const int scalar_count = 4;
    Vector4(const Vector4& other) { *this = other; }
    Vector4() { x() = y() = z() = w() = 0; }

    template<class T>
    explicit Vector4(const T& other)
    {
      x() = (scalar_type)other.x();
      y() = (scalar_type)other.y();
      z() = (scalar_type)other.z();
      w() = (scalar_type)other.w();
    }

    explicit Vector4(scalar_type x, scalar_type y, scalar_type z, scalar_type w)
    {
      mScalar[0] = x;
      mScalar[1] = y;
      mScalar[2] = z;
      mScalar[3] = w;
    }

    explicit Vector4(const Vector3<scalar_type>& v, scalar_type w)
    {
      mScalar[0] = v.x();
      mScalar[1] = v.y();
      mScalar[2] = v.z();
      mScalar[3] = w;
    }

    explicit Vector4(const Vector2<scalar_type>& u, const Vector2<scalar_type>& v)
    {
      mScalar[0] = u.x();
      mScalar[1] = u.y();
      mScalar[2] = v.x();
      mScalar[3] = v.y();
    }

    scalar_type* ptr() { return mScalar; }
    const scalar_type* ptr() const { return mScalar; }

    const scalar_type& x() const { return mScalar[0]; }
    const scalar_type& y() const { return mScalar[1]; }
    const scalar_type& z() const { return mScalar[2]; }
    const scalar_type& w() const { return mScalar[3]; }

    scalar_type& x() { return mScalar[0]; }
    scalar_type& y() { return mScalar[1]; }
    scalar_type& z() { return mScalar[2]; }
    scalar_type& w() { return mScalar[3]; }

    const scalar_type& r() const { return mScalar[0]; }
    const scalar_type& g() const { return mScalar[1]; }
    const scalar_type& b() const { return mScalar[2]; }
    const scalar_type& a() const { return mScalar[3]; }

    scalar_type& r() { return mScalar[0]; }
    scalar_type& g() { return mScalar[1]; }
    scalar_type& b() { return mScalar[2]; }
    scalar_type& a() { return mScalar[3]; }

    const scalar_type& s() const { return mScalar[0]; }
    const scalar_type& t() const { return mScalar[1]; }
    const scalar_type& p() const { return mScalar[2]; }
    const scalar_type& q() const { return mScalar[3]; }

    scalar_type& s() { return mScalar[0]; }
    scalar_type& t() { return mScalar[1]; }
    scalar_type& p() { return mScalar[2]; }
    scalar_type& q() { return mScalar[3]; }

    Vector3<scalar_type> xyz() const { return Vector3<scalar_type>(x(),y(),z()); }
    Vector3<scalar_type> rgb() const { return Vector3<scalar_type>(x(),y(),z()); }
    Vector3<scalar_type> stp() const { return Vector3<scalar_type>(x(),y(),z()); }

    Vector2<scalar_type> xy() const { return Vector2<scalar_type>(x(),y()); }
    Vector2<scalar_type> rg() const { return Vector2<scalar_type>(x(),y()); }
    Vector2<scalar_type> st() const { return Vector2<scalar_type>(x(),y()); }

    Vector4 operator+(const Vector4& other) const
    {
      return Vector4(x()+other.x(), y()+other.y(), z()+other.z(), w()+other.w());
    }
    Vector4 operator-(const Vector4& other) const
    {
      return Vector4(x()-other.x(), y()-other.y(), z()-other.z(), w()-other.w());
    }
    Vector4 operator*(const Vector4& other) const
    {
      return Vector4(x()*other.x(), y()*other.y(), z()*other.z(), w()*other.w());
    }
    Vector4 operator/(const Vector4& other) const
    {
      return Vector4(x()/other.x(), y()/other.y(), z()/other.z(), w()/other.w());
    }
    Vector4 operator+(scalar_type val) const
    {
      return Vector4(x()+val, y()+val, z()+val, w()+val);
    }
    Vector4 operator-(scalar_type val) const
    {
      return Vector4(x()-val, y()-val, z()-val, w()-val);
    }
    Vector4 operator*(scalar_type val) const
    {
      return Vector4(x()*val, y()*val, z()*val, w()*val);
    }
    Vector4 operator/(scalar_type val) const
    {
      return Vector4(x()/val, y()/val, z()/val, w()/val);
    }
    Vector4 operator-() const
    {
      return Vector4(-x(), -y(), -z(), -w());
    }
    Vector4& operator+=(const Vector4& other)
    {
      *this = *this + other;
      return *this;
    }
    Vector4& operator-=(const Vector4& other)
    {
      *this = *this - other;
      return *this;
    }
    Vector4& operator*=(const Vector4& other)
    {
      *this = *this * other;
      return *this;
    }
    Vector4& operator/=(const Vector4& other)
    {
      *this = *this / other;
      return *this;
    }
    Vector4& operator+=(scalar_type val)
    {
      *this = *this + val;
      return *this;
    }
    Vector4& operator-=(scalar_type val)
    {
      *this = *this - val;
      return *this;
    }
    Vector4& operator*=(scalar_type val)
    {
      *this = *this * val;
      return *this;
    }
    Vector4& operator/=(scalar_type val)
    {
      *this = *this / val;
      return *this;
    }
    Vector4& operator=(const Vector4& other)
    {
      x() = other.x();
      y() = other.y();
      z() = other.z();
      w() = other.w();
      return *this;
    }
    Vector4& operator=(scalar_type val)
    {
      x() = y() = z() = w() = val;
      return *this;
    }
    bool operator==(const Vector4& other) const
    {
      return x() == other.x() && y() == other.y() && z() == other.z() && w() == other.w() ;
    }
    bool operator!=(const Vector4& other) const
    {
      return !operator==(other);
    }
    bool operator<(const Vector4& other) const
    {
      if (x() != other.x())
        return x() < other.x();
      else
      if (y() != other.y())
        return y() < other.y();
      else
      if (z() != other.z())
        return z() < other.z();
      else
        return w() < other.w();
    }
    scalar_type& operator[](unsigned i) { return mScalar[i]; }
    const scalar_type& operator[](unsigned i) const { return mScalar[i]; }
    scalar_type length() const { return (scalar_type)::sqrt(x()*x()+y()*y()+z()*z()+w()*w()); }
    scalar_type lengthSquared() const { return x()*x()+y()*y()+z()*z()+w()*w(); }
    bool isNull() const { return !x() && !y() && !z() && !w(); }
    const Vector4& normalize() 
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
  inline const Vector4<T> operator*(T val, const Vector4<T>& v)
  {
    return v * val;
  }

  //! A 4 components vector with \p GLint precision.
  typedef Vector4<GLint> ivec4;
  //! A 4 components vector with \p GLuint precision.
  typedef Vector4<GLuint> uvec4;
  //! A 4 components vector with \p GLfloat precision.
  typedef Vector4<GLfloat> fvec4;
  //! A 4 components vector with \p GLdouble precision.
  typedef Vector4<GLdouble> dvec4;
  //! A 4 components vector with \p GLbyte precision.
  typedef Vector4<GLbyte> bvec4;
  //! A 4 components vector with \p GLubyte precision.
  typedef Vector4<GLubyte> ubvec4;
  //! A 4 components vector with \p GLshort precision.
  typedef Vector4<GLshort> svec4;
  //! A 4 components vector with \p GLushort precision.
  typedef Vector4<GLushort> usvec4;

  #if VL_PIPELINE_PRECISION == 2
    //! Defined as: \p 'typedef \p dvec4 \p vec4'. See also \ref VL_PIPELINE_PRECISION.
    typedef dvec4 vec4;
  #else
    //! Defined as: \p 'typedef \p fvec4 \p vec4'. See also \ref VL_PIPELINE_PRECISION.
    typedef fvec4 vec4;
  #endif
}

#endif
