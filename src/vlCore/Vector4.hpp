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

#include <vlCore/Vector3.hpp>

namespace vl
{
  /**
   * The Vector4 class is a template class that implements a generic 4 components vector, see also vl::fvec4, vl::dvec4, vl::uvec4, vl::ivec4, vl::svec4, vl::usvec4, vl::bvec4, vl::ubvec4.
   * \sa Vector3, Vector2, Matrix4, Matrix3, Matrix2
   */
  template<typename T_scalar>
  class Vector4
  {
  public:
    typedef T_scalar scalar_type;
    static const int scalar_count = 4;
    Vector4(const Vector4& other) { *this = other; }
    Vector4() { x() = y() = z() = w() = 0; }

    template<class T>
    explicit Vector4(const T& other)
    {
      x() = (T_scalar)other.x();
      y() = (T_scalar)other.y();
      z() = (T_scalar)other.z();
      w() = (T_scalar)other.w();
    }

    explicit Vector4(T_scalar x, T_scalar y, T_scalar z, T_scalar w)
    {
      mScalar[0] = x;
      mScalar[1] = y;
      mScalar[2] = z;
      mScalar[3] = w;
    }

    explicit Vector4(const Vector3<T_scalar>& v, T_scalar w)
    {
      mScalar[0] = v.x();
      mScalar[1] = v.y();
      mScalar[2] = v.z();
      mScalar[3] = w;
    }

    explicit Vector4(const Vector2<T_scalar>& u, const Vector2<T_scalar>& v)
    {
      mScalar[0] = u.x();
      mScalar[1] = u.y();
      mScalar[2] = v.x();
      mScalar[3] = v.y();
    }

    T_scalar* ptr() { return mScalar; }
    const T_scalar* ptr() const { return mScalar; }

    const T_scalar& x() const { return mScalar[0]; }
    const T_scalar& y() const { return mScalar[1]; }
    const T_scalar& z() const { return mScalar[2]; }
    const T_scalar& w() const { return mScalar[3]; }

    T_scalar& x() { return mScalar[0]; }
    T_scalar& y() { return mScalar[1]; }
    T_scalar& z() { return mScalar[2]; }
    T_scalar& w() { return mScalar[3]; }

    const T_scalar& r() const { return mScalar[0]; }
    const T_scalar& g() const { return mScalar[1]; }
    const T_scalar& b() const { return mScalar[2]; }
    const T_scalar& a() const { return mScalar[3]; }

    T_scalar& r() { return mScalar[0]; }
    T_scalar& g() { return mScalar[1]; }
    T_scalar& b() { return mScalar[2]; }
    T_scalar& a() { return mScalar[3]; }

    const T_scalar& s() const { return mScalar[0]; }
    const T_scalar& t() const { return mScalar[1]; }
    const T_scalar& p() const { return mScalar[2]; }
    const T_scalar& q() const { return mScalar[3]; }

    T_scalar& s() { return mScalar[0]; }
    T_scalar& t() { return mScalar[1]; }
    T_scalar& p() { return mScalar[2]; }
    T_scalar& q() { return mScalar[3]; }

    Vector3<T_scalar> xyz() const { return Vector3<T_scalar>(x(),y(),z()); }
    Vector3<T_scalar> rgb() const { return Vector3<T_scalar>(x(),y(),z()); }
    Vector3<T_scalar> stp() const { return Vector3<T_scalar>(x(),y(),z()); }

    Vector2<T_scalar> xy() const { return Vector2<T_scalar>(x(),y()); }
    Vector2<T_scalar> rg() const { return Vector2<T_scalar>(x(),y()); }
    Vector2<T_scalar> st() const { return Vector2<T_scalar>(x(),y()); }

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
    Vector4 operator+(T_scalar val) const
    {
      return Vector4(x()+val, y()+val, z()+val, w()+val);
    }
    Vector4 operator-(T_scalar val) const
    {
      return Vector4(x()-val, y()-val, z()-val, w()-val);
    }
    Vector4 operator*(T_scalar val) const
    {
      return Vector4(x()*val, y()*val, z()*val, w()*val);
    }
    Vector4 operator/(T_scalar val) const
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
    Vector4& operator+=(T_scalar val)
    {
      *this = *this + val;
      return *this;
    }
    Vector4& operator-=(T_scalar val)
    {
      *this = *this - val;
      return *this;
    }
    Vector4& operator*=(T_scalar val)
    {
      *this = *this * val;
      return *this;
    }
    Vector4& operator/=(T_scalar val)
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
    Vector4& operator=(T_scalar val)
    {
      x() = y() = z() = w() = val;
      return *this;
    }
    bool operator==(const Vector4& other) const
    {
      return x() == other.x() && y() == other.y() && z() == other.z() && w() == other.w();
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
    T_scalar& operator[](unsigned i) { return mScalar[i]; }
    const T_scalar& operator[](unsigned i) const { return mScalar[i]; }
    T_scalar length() const { return (T_scalar)::sqrt(x()*x()+y()*y()+z()*z()+w()*w()); }
    T_scalar lengthSquared() const { return x()*x()+y()*y()+z()*z()+w()*w(); }
    bool isNull() const { return !x() && !y() && !z() && !w(); }
    const Vector4& normalize(T_scalar *len=NULL) 
    {
      T_scalar l = length();
      if (len)
        *len = l;
      if (l)
        *this *= (T_scalar)(1.0/l); 
      return *this; 
    }

  protected:
    T_scalar mScalar[scalar_count];
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

  inline float dot(const fvec4& v1, const fvec4& v2) { return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w(); }
  inline double dot(const dvec4& v1, const dvec4& v2) { return v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w(); }
  inline float dot(const ivec4& v1, const ivec4& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w()); }
  inline float dot(const uvec4& v1, const uvec4& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y() + v1.z()*v2.z() + v1.w()*v2.w()); }

  inline fvec4 min(const fvec4& a, const fvec4& b)
  {
    return fvec4( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y(),
      a.z() < b.z() ? a.z() : b.z(),
      a.w() < b.w() ? a.w() : b.w() );
  }
  inline fvec4 min(const fvec4& a, float b)
  {
    return fvec4( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b,
      a.z() < b ? a.z() : b,
      a.w() < b ? a.w() : b );
  }
  inline dvec4 min(const dvec4& a, const dvec4& b)
  {
    return dvec4( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y(),
      a.z() < b.z() ? a.z() : b.z(),
      a.w() < b.w() ? a.w() : b.w() );
  }
  inline dvec4 min(const dvec4& a, double b)
  {
    return dvec4( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b,
      a.z() < b ? a.z() : b,
      a.w() < b ? a.w() : b );
  }
  inline ivec4 min(const ivec4& a, const ivec4& b)
  {
    return ivec4( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y(),
      a.z() < b.z() ? a.z() : b.z(),
      a.w() < b.w() ? a.w() : b.w() );
  }
  inline ivec4 min(const ivec4& a, int b)
  {
    return ivec4( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b,
      a.z() < b ? a.z() : b,
      a.w() < b ? a.w() : b );
  }
  inline uvec4 min(const uvec4& a, const uvec4& b)
  {
    return uvec4( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y(),
      a.z() < b.z() ? a.z() : b.z(),
      a.w() < b.w() ? a.w() : b.w() );
  }
  inline uvec4 min(const uvec4& a, unsigned int b)
  {
    return uvec4( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b,
      a.z() < b ? a.z() : b,
      a.w() < b ? a.w() : b );
  }
  inline fvec4 max(const fvec4& a, const fvec4& b)
  {
    return fvec4( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y(),
      a.z() > b.z() ? a.z() : b.z(),
      a.w() > b.w() ? a.w() : b.w() );
  }
  inline fvec4 max(const fvec4& a, float b)
  {
    return fvec4( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b,
      a.z() > b ? a.z() : b,
      a.w() > b ? a.w() : b );
  }
  inline dvec4 max(const dvec4& a, const dvec4& b)
  {
    return dvec4( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y(),
      a.z() > b.z() ? a.z() : b.z(),
      a.w() > b.w() ? a.w() : b.w() );
  }
  inline dvec4 max(const dvec4& a, double b)
  {
    return dvec4( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b,
      a.z() > b ? a.z() : b,
      a.w() > b ? a.w() : b );
  }
  inline ivec4 max(const ivec4& a, const ivec4& b)
  {
    return ivec4( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y(),
      a.z() > b.z() ? a.z() : b.z(),
      a.w() > b.w() ? a.w() : b.w() );
  }
  inline ivec4 max(const ivec4& a, int b)
  {
    return ivec4( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b,
      a.z() > b ? a.z() : b,
      a.w() > b ? a.w() : b );
  }
  inline uvec4 max(const uvec4& a, const uvec4& b)
  {
    return uvec4( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y(),
      a.z() > b.z() ? a.z() : b.z(),
      a.w() > b.w() ? a.w() : b.w() );
  }
  inline uvec4 max(const uvec4& a, unsigned int b)
  {
    return uvec4( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b,
      a.z() > b ? a.z() : b,
      a.w() > b ? a.w() : b );
  }
  inline fvec4 clamp(const fvec4& x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline fvec4 clamp(const fvec4& x, const fvec4& minval, const fvec4& maxval) { return min(max(x,minval),maxval); }
  inline dvec4 clamp(const dvec4& x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline dvec4 clamp(const dvec4& x, const dvec4& minval, const dvec4& maxval) { return min(max(x,minval),maxval); }
  inline ivec4 clamp(const ivec4& x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline ivec4 clamp(const ivec4& x, const ivec4& minval, const ivec4& maxval) { return min(max(x,minval),maxval); }
  inline uvec4 clamp(const uvec4& x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }
  inline uvec4 clamp(const uvec4& x, const uvec4& minval, const uvec4& maxval) { return min(max(x,minval),maxval); }
}

#endif
