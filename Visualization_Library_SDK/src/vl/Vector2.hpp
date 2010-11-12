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

#ifndef Vector2_INCLUDE_ONCE
#define Vector2_INCLUDE_ONCE

#include <vl/OpenGL.hpp>
#include <cmath>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifdef dot
#undef dot
#endif

#ifdef cross
#undef cross
#endif

namespace vl
{
  // trigonometric constants

  //! Greek Pi constant using \p double precision.
  const double dPi = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093845;
  //! Constant to convert degrees into radians using \p double precision.
  const double dDEG_TO_RAD = dPi / 180.0;
  //! Constant to convert radians into degrees using \p double precision.
  const double dRAD_TO_DEG = 180.0 / dPi;

  //! Greek Pi constant using \p float precision.
  const float fPi = (float)3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093845;
  //! Constant to convert degrees into radians using \p float precision.
  const float fDEG_TO_RAD = float(dPi / 180.0);
  //! Constant to convert radians into degrees using \p float precision.
  const float fRAD_TO_DEG = float(180.0 / dPi);

  // fast square root

  #if VL_FAST_SQUARE_ROOTS == 1
    #define VL_FLOAT_SQRT(x)    fast_sqrt(x)
    #define VL_FLOAT_INVSQRT(x) fast2_inversesqrt(x)
  #else
    #define VL_FLOAT_SQRT(x)    ((float)::sqrt(x))
    #define VL_FLOAT_INVSQRT(x) (1.0f/(float)::sqrt(x))
  #endif

  // fast square root functions, see Dave Eberly's paper and http://www.beyond3d.com/content/articles/8/

  inline float fast1_inversesqrt(float x)
  {
    float xhalf = 0.5f*x;
    unsigned int i = *((unsigned int*)&x);
    i = 0x5f3759df - (i>>1);
    x = *((float*)&i);
    x = x*(1.5f - xhalf*x*x); // single iteration, very quick, but very poor precision
    return x;
  }
  inline float fast2_inversesqrt(float x)
  {
    float xhalf = 0.5f*x;
    unsigned int i = *((unsigned int*)&x);
    i = 0x5f3759df - (i>>1);
    x = *((float*)&i);
    x = x*(1.5f - xhalf*x*x);
    x = x*(1.5f - xhalf*x*x); // the trackball shows that one iteration is not enough so we do two
    return x;
  }
  inline float fast_sqrt(float x) { if (x == 0.0f) return 0.0f; else return x * fast2_inversesqrt(x); }

  /**
   * The Vector2 class is a template class that implements a generic 2 components vector, see also vl::fvec2, vl::dvec2, vl::uvec2, vl::ivec2, vl::svec2, vl::usvec2, vl::bvec2, vl::ubvec2.
   * \sa Vector4, Vector3, Matrix4, Matrix3, Matrix2
   */
  template<typename T_scalar_type>
  class Vector2
  {
  public:
    typedef T_scalar_type scalar_type;
    static const int scalar_count = 2;
    Vector2(const Vector2& other) { *this = other; }
    Vector2() { x() = y() = 0; }

    template<class T>
    explicit Vector2(const T& other)
    {
      x() = (T_scalar_type)other.x();
      y() = (T_scalar_type)other.y();
    }

    explicit Vector2(T_scalar_type x, T_scalar_type y)
    {
      mScalar[0] = x;
      mScalar[1] = y;
    }

    T_scalar_type* ptr() { return mScalar; }
    const T_scalar_type* ptr() const { return mScalar; }

    const T_scalar_type& x() const { return mScalar[0]; }
    const T_scalar_type& y() const { return mScalar[1]; }

    T_scalar_type& x() { return mScalar[0]; }
    T_scalar_type& y() { return mScalar[1]; }

    const T_scalar_type& r() const { return mScalar[0]; }
    const T_scalar_type& g() const { return mScalar[1]; }

    T_scalar_type& r() { return mScalar[0]; }
    T_scalar_type& g() { return mScalar[1]; }

    const T_scalar_type& s() const { return mScalar[0]; }
    const T_scalar_type& t() const { return mScalar[1]; }

    T_scalar_type& s() { return mScalar[0]; }
    T_scalar_type& t() { return mScalar[1]; }

    Vector2 operator+(const Vector2& other) const
    {
      return Vector2(x()+other.x(), y()+other.y());
    }
    Vector2 operator-(const Vector2& other) const
    {
      return Vector2(x()-other.x(), y()-other.y());
    }
    Vector2 operator*(const Vector2& other) const
    {
      return Vector2(x()*other.x(), y()*other.y());
    }
    Vector2 operator/(const Vector2& other) const
    {
      return Vector2(x()/other.x(), y()/other.y());
    }
    Vector2 operator+(T_scalar_type val) const
    {
      return Vector2(x()+val, y()+val);
    }
    Vector2 operator-(T_scalar_type val) const
    {
      return Vector2(x()-val, y()-val);
    }
    Vector2 operator*(T_scalar_type val) const
    {
      return Vector2(x()*val, y()*val);
    }
    Vector2 operator/(T_scalar_type val) const
    {
      return Vector2(x()/val, y()/val);
    }
    Vector2 operator-() const
    {
      return Vector2(-x(), -y());
    }
    Vector2& operator+=(const Vector2& other)
    {
      *this = *this + other;
      return *this;
    }
    Vector2& operator-=(const Vector2& other)
    {
      *this = *this - other;
      return *this;
    }
    Vector2& operator*=(const Vector2& other)
    {
      *this = *this * other;
      return *this;
    }
    Vector2& operator/=(const Vector2& other)
    {
      *this = *this / other;
      return *this;
    }
    Vector2& operator+=(T_scalar_type val)
    {
      *this = *this + val;
      return *this;
    }
    Vector2& operator-=(T_scalar_type val)
    {
      *this = *this - val;
      return *this;
    }
    Vector2& operator*=(T_scalar_type val)
    {
      *this = *this * val;
      return *this;
    }
    Vector2& operator/=(T_scalar_type val)
    {
      *this = *this / val;
      return *this;
    }
    Vector2& operator=(const Vector2& other)
    {
      x() = other.x();
      y() = other.y();
      return *this;
    }
    Vector2& operator=(T_scalar_type val)
    {
      x() = y() = val;
      return *this;
    }
    bool operator==(const Vector2& other) const
    {
      return x() == other.x() && y() == other.y();
    }
    bool operator!=(const Vector2& other) const
    {
      return !operator==(other);
    }
    bool operator<(const Vector2& other) const
    {
      if (x() != other.x())
        return x() < other.x();
      else
        return y() < other.y();
    }
    T_scalar_type& operator[](unsigned i) { return mScalar[i]; }
    const T_scalar_type& operator[](unsigned i) const { return mScalar[i]; }
    T_scalar_type length() const { return ::sqrt(x()*x()+y()*y()); }
    T_scalar_type lengthSquared() const { return x()*x()+y()*y(); }
    bool isNull() const { return !x() && !y(); }
    const Vector2& normalize(T_scalar_type *len=NULL)
    {
      T_scalar_type l = length();
      if (len)
        *len = l;
      if (l)
        *this *= (T_scalar_type)(1.0/l); 
      return *this; 
    }

  protected:
    T_scalar_type mScalar[scalar_count];
  };

  template<typename T>
  inline const Vector2<T> operator*(T val, const Vector2<T>& v)
  {
    return v * val;
  }

  //! A 2 components vector with \p GLint precision.
  typedef Vector2<GLint> ivec2;
  //! A 2 components vector with \p GLuint precision.
  typedef Vector2<GLuint> uvec2;
  //! A 2 components vector with \p GLfloat precision.
  typedef Vector2<GLfloat> fvec2;
  //! A 2 components vector with \p GLdouble precision.
  typedef Vector2<GLdouble> dvec2;
  //! A 2 components vector with \p GLbyte precision.
  typedef Vector2<GLbyte> bvec2;
  //! A 2 components vector with \p GLubyte precision.
  typedef Vector2<GLubyte> ubvec2;
  //! A 2 components vector with \p GLshort precision.
  typedef Vector2<GLshort> svec2;
  //! A 2 components vector with \p GLushort precision.
  typedef Vector2<GLushort> usvec2;

  #if VL_PIPELINE_PRECISION == 2
    //! Defined as: \p 'typedef \p dvec2 \p vec2'. See also \ref VL_PIPELINE_PRECISION.
    typedef dvec2 vec2;
  #else
    //! Defined as: \p 'typedef \p fvec2 \p vec2'. See also \ref VL_PIPELINE_PRECISION.
    typedef fvec2 vec2;
  #endif

  inline float dot(const fvec2& v1, const fvec2& v2) { return v1.x()*v2.x() + v1.y()*v2.y(); }
  inline double dot(const dvec2& v1, const dvec2& v2) { return v1.x()*v2.x() + v1.y()*v2.y(); }
  inline float dot(const ivec2& v1, const ivec2& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y()); }
  inline float dot(const uvec2& v1, const uvec2& v2) { return (float)(v1.x()*v2.x() + v1.y()*v2.y()); }

  inline float min(float a, float b) { return a < b ? a : b; }
  inline double min(double a, double b) { return a < b ? a : b; }
  inline int min(int a, int b) { return a < b ? a : b; }
  inline unsigned int min(unsigned int a, unsigned int b) { return a < b ? a : b; }
  inline float max(float a, float b) { return a > b ? a : b; }
  inline double max(double a, double b) { return a > b ? a : b; }
  inline int max(int a, int b) { return a > b ? a : b; }
  inline unsigned int max(unsigned int a, unsigned int b) { return a > b ? a : b; }
  inline float clamp(float x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline double clamp(double x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline int clamp(int x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline unsigned int clamp(unsigned int x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }

  inline fvec2 min(const fvec2& a, const fvec2& b)
  {
    return fvec2( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y());
  }
  inline fvec2 min(const fvec2& a, float b)
  {
    return fvec2( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b);
  }
  inline dvec2 min(const dvec2& a, const dvec2& b)
  {
    return dvec2( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y());
  }
  inline dvec2 min(const dvec2& a, double b)
  {
    return dvec2( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b);
  }
  inline ivec2 min(const ivec2& a, const ivec2& b)
  {
    return ivec2( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y());
  }
  inline ivec2 min(const ivec2& a, int b)
  {
    return ivec2( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b);
  }
  inline uvec2 min(const uvec2& a, const uvec2& b)
  {
    return uvec2( a.x() < b.x() ? a.x() : b.x(),
      a.y() < b.y() ? a.y() : b.y());
  }
  inline uvec2 min(const uvec2& a, unsigned int b)
  {
    return uvec2( a.x() < b ? a.x() : b,
      a.y() < b ? a.y() : b);
  }
  inline fvec2 max(const fvec2& a, const fvec2& b)
  {
    return fvec2( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y());
  }
  inline fvec2 max(const fvec2& a, float b)
  {
    return fvec2( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b);
  }
  inline dvec2 max(const dvec2& a, const dvec2& b)
  {
    return dvec2( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y());
  }
  inline dvec2 max(const dvec2& a, double b)
  {
    return dvec2( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b);
  }
  inline ivec2 max(const ivec2& a, const ivec2& b)
  {
    return ivec2( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y());
  }
  inline ivec2 max(const ivec2& a, int b)
  {
    return ivec2( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b);
  }
  inline uvec2 max(const uvec2& a, const uvec2& b)
  {
    return uvec2( a.x() > b.x() ? a.x() : b.x(),
      a.y() > b.y() ? a.y() : b.y());
  }
  inline uvec2 max(const uvec2& a, unsigned int b)
  {
    return uvec2( a.x() > b ? a.x() : b,
      a.y() > b ? a.y() : b);
  }
  inline fvec2 clamp(const fvec2& x, float minval, float maxval) { return min(max(x,minval),maxval); }
  inline fvec2 clamp(const fvec2& x, const fvec2& minval, const fvec2& maxval) { return min(max(x,minval),maxval); }
  inline dvec2 clamp(const dvec2& x, double minval, double maxval) { return min(max(x,minval),maxval); }
  inline dvec2 clamp(const dvec2& x, const dvec2& minval, const dvec2& maxval) { return min(max(x,minval),maxval); }
  inline ivec2 clamp(const ivec2& x, int minval, int maxval) { return min(max(x,minval),maxval); }
  inline ivec2 clamp(const ivec2& x, const ivec2& minval, const ivec2& maxval) { return min(max(x,minval),maxval); }
  inline uvec2 clamp(const uvec2& x, unsigned int minval, unsigned int maxval) { return min(max(x,minval),maxval); }
  inline uvec2 clamp(const uvec2& x, const uvec2& minval, const uvec2& maxval) { return min(max(x,minval),maxval); }
}

#endif
