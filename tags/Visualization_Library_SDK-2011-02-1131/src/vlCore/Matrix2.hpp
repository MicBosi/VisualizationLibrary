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

#ifndef Matrix2_INCLUDE_ONCE
#define Matrix2_INCLUDE_ONCE

#include <vlCore/checks.hpp>
#include <vlCore/Vector2.hpp>
#include <cstring> // memcpy

namespace vl
{
  //-----------------------------------------------------------------------------
  // Matrix2
  //-----------------------------------------------------------------------------
  /**
   * The Matrix2 class is a template class that implements a generic 2x2 matrix, see also vl::dmat2, vl::fmat2, vl::umat2, vl::imat2.
   * \sa Vector4, Vector3, Vector2, Matrix4, Matrix3
   */
  template<typename T_scalar>
  class Matrix2
  {
  public:
    typedef T_scalar scalar_type;
    //-----------------------------------------------------------------------------
    template<typename T>
    explicit Matrix2(const Matrix2<T>& m)
    {
      e(0,0) = (T_scalar)m.e(0,0); e(1,0) = (T_scalar)m.e(1,0);
      e(0,1) = (T_scalar)m.e(0,1); e(1,1) = (T_scalar)m.e(1,1);
    }
    //-----------------------------------------------------------------------------
    Matrix2()
    {
      setIdentity();
    }
    //-----------------------------------------------------------------------------
    explicit Matrix2(T_scalar n)
    {
      setIdentity();
      e(0,0) = e(1,1) = n;
    }
    //-----------------------------------------------------------------------------
    explicit Matrix2(T_scalar e00, T_scalar e01,
                      T_scalar e10, T_scalar e11 )
    {
      e(0,0) = e00; e(0,1) = e01;
      e(1,0) = e10; e(1,1) = e11;
    }
    //-----------------------------------------------------------------------------
    Matrix2& fill(T_scalar val)
    {
      e(0,0) = e(1,0) = 
      e(0,1) = e(1,1) = val;
      return *this;
    }
    //-----------------------------------------------------------------------------
    T_scalar diff(const Matrix2& other) const
    {
      T_scalar err = 0;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          if (e(j,i) > other.e(j,i)) // avoid fabs/abs
            err += e(j,i) - other.e(j,i);
          else
            err += other.e(j,i) - e(j,i);
      return err;
    }
    //-----------------------------------------------------------------------------
    bool operator==(const Matrix2& m) const 
    {
      return memcmp(m.mVec, mVec, sizeof(T_scalar)*4) == 0;
    }
    //-----------------------------------------------------------------------------
    bool operator!=(const Matrix2& m) const 
    {
      return !operator==(m);
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator=(const Matrix2& m) 
    {
      memcpy(mVec, m.mVec, sizeof(T_scalar)*4);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator+(const Matrix2& m) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) + m.e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator+=(const Matrix2& m)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) += m.e(j,i);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-(const Matrix2& m) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) - m.e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator-=(const Matrix2& m)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) -= m.e(j,i);
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator*=(const Matrix2& m)
    {
      return postMultiply(m);
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-() const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = -e(j,i);
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator+(T_scalar d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) + d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator+=(T_scalar d)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) += d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator-(T_scalar d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) - d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator-=(T_scalar d)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) -= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator*(T_scalar d) const
    {
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) * d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator*=(T_scalar d)
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) *= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 operator/(T_scalar d) const
    {
      d = (T_scalar)1 / d;
      Matrix2 t;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          t.e(j,i) = e(j,i) * d;
      return t;
    }
    //-----------------------------------------------------------------------------
    Matrix2& operator/=(T_scalar d)
    {
      d = (T_scalar)1 / d;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          e(j,i) *= d;
      return *this;
    }
    //-----------------------------------------------------------------------------
    bool isIdentity() const
    {
      Matrix2 i;
      return memcmp(ptr(), i.ptr(), sizeof(T_scalar)*4) == 0;
    }
    //-----------------------------------------------------------------------------
    T_scalar* ptr()
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    const T_scalar* ptr() const
    {
      return &e(0,0);
    }
    //-----------------------------------------------------------------------------
    Matrix2& transpose()
    {
      T_scalar tmp;
      for(int i=0; i<2; ++i)
        for(int j=i; j<2; ++j)
        {
          tmp = e(j,i);
          e(j,i) = e(i,j);
          e(i,j) = tmp;
        }
        return *this;
    }
    //-----------------------------------------------------------------------------
    Matrix2 getTransposed() const
    {
      Matrix2 m;
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          m.e(j,i) = e(i,j);
      return m;
    }
    //-----------------------------------------------------------------------------
    Matrix2& getTransposed(Matrix2& dest) const
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          dest.e(j,i) = e(i,j);
      return dest;
    }
    //-----------------------------------------------------------------------------
    bool isNull() const
    {
      for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
          if(mVec[j][i] != 0)
            return false;
      return true;
    }
    //-----------------------------------------------------------------------------
    Matrix2& setNull() 
    {
      fill(0);
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix2& getNull(Matrix2& out)
    {
      out.fill(0);
      return out;
    }
    //-----------------------------------------------------------------------------
    static Matrix2 getNull()
    {
      return Matrix2().fill(0);
    }
    //-----------------------------------------------------------------------------
    Matrix2& setIdentity()
    {
      static const T_scalar I2d[] = 
      { 
        (T_scalar)1, (T_scalar)0,
        (T_scalar)0, (T_scalar)1
      };
      memcpy(mVec, I2d, sizeof(T_scalar)*4);
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix2 getIdentity()
    {
      return Matrix2();
    }
    //-----------------------------------------------------------------------------
    static Matrix2& getIdentity(Matrix2& out)
    {
      out.setIdentity();
      return out;
    }
    //-----------------------------------------------------------------------------
    T_scalar getInverse(Matrix2& dest) const
    {
      if (&dest == this)
      {
        Matrix2 tmp;
        T_scalar det = getInverse(tmp);
        dest = tmp;
        return det;
      }
      else
      {
        const T_scalar& a11 = e(0,0); 
        const T_scalar& a12 = e(1,0); 
        const T_scalar& a21 = e(0,1); 
        const T_scalar& a22 = e(1,1); 

        dest.fill(0);

        T_scalar det = a11*a22-a12*a21;

        if (det != 0)
          dest = Matrix2(+a22, -a12, -a21, +a11) / det;

        return det;
      }
    }
    //-----------------------------------------------------------------------------
    Matrix2 getInverse(T_scalar *determinant=NULL) const
    {
      Matrix2 tmp;
      T_scalar det = getInverse(tmp);
      if (determinant)
        *determinant = det;
      return tmp;
    }
    //-----------------------------------------------------------------------------
    Matrix2& invert(T_scalar *determinant=NULL)
    {
      T_scalar det = getInverse(*this);
      if (determinant)
        *determinant = det;
      return *this;
    }
    //-----------------------------------------------------------------------------
    static Matrix2& multiply(Matrix2& out, const Matrix2& p, const Matrix2& q)
    {
      VL_CHECK(out.ptr() != p.ptr() && out.ptr() != q.ptr());

      out.e(0,0) = q.e(0,0)*p.e(0,0) + q.e(1,0)*p.e(0,1);
      out.e(0,1) = q.e(0,1)*p.e(0,0) + q.e(1,1)*p.e(0,1);

      out.e(1,0) = q.e(0,0)*p.e(1,0) + q.e(1,0)*p.e(1,1);
      out.e(1,1) = q.e(0,1)*p.e(1,0) + q.e(1,1)*p.e(1,1);

      return out;
    }
    //-----------------------------------------------------------------------------
    Matrix2& postMultiply(const Matrix2& m)
    {
      Matrix2<T_scalar> t;
      return *this = multiply(t, *this, m);
    }
    //-----------------------------------------------------------------------------
    Matrix2& preMultiply(const Matrix2& m)
    {
      Matrix2<T_scalar> t;
      return *this = multiply(t, m, *this);
    }
    //-----------------------------------------------------------------------------

    const T_scalar& e(int i, int j) const { return mVec[j][i]; }
    T_scalar& e(int i, int j) { return mVec[j][i]; }

  private:
    const Vector2<T_scalar>& operator[](unsigned int i) const { VL_CHECK(i<2); return mVec[i]; }
    Vector2<T_scalar>& operator[](unsigned int i) { VL_CHECK(i<2); return mVec[i]; }

  protected:
    Vector2<T_scalar> mVec[2];
  };

  //-----------------------------------------------------------------------------
  // OPERATORS
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  inline Matrix2<T_scalar> operator*(const Matrix2<T_scalar>& p, const Matrix2<T_scalar>& q)
  {
    Matrix2<T_scalar> t;
    Matrix2<T_scalar>::multiply(t, p, q);
    return t;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  inline Matrix2<T_scalar> operator+(T_scalar d, const Matrix2<T_scalar>& m)
  {
    return m + d;
  }
  //-----------------------------------------------------------------------------
  template<typename T_scalar>
  inline Matrix2<T_scalar> operator*(T_scalar d, const Matrix2<T_scalar>& m)
  {
    return m * d;
  }
  //-----------------------------------------------------------------------------
  // post multiplication: matrix * column vector
  template<typename T_scalar>
  inline Vector2<T_scalar> operator*(const Matrix2<T_scalar>& m, const Vector2<T_scalar>& v)
  {
    Vector2<T_scalar> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(0,1);
    t.y() = v.x()*m.e(1,0) + v.y()*m.e(1,1);
    return t;
  }
  //-----------------------------------------------------------------------------
  // pre-multiplication: row vector * matrix
  template<typename T_scalar>
  inline Vector2<T_scalar> operator*(const Vector2<T_scalar>& v, const Matrix2<T_scalar>& m)
  {
    Vector2<T_scalar> t;
    t.x() = v.x()*m.e(0,0) + v.y()*m.e(1,0);
    t.y() = v.x()*m.e(0,1) + v.y()*m.e(1,1);
    return t;
  }
  //-----------------------------------------------------------------------------

  //! A 2x2 matrix using \p GLdouble precision.
  typedef Matrix2<GLdouble> dmat2;
  //! A 2x2 matrix using \p GLfloat precision.
  typedef Matrix2<GLfloat>  fmat2;
  //! A 2x2 matrix using \p GLint precision.
  typedef Matrix2<GLint>    imat2;
  //! A 2x2 matrix using \p GLuint precision.
  typedef Matrix2<GLuint>  umat2;

  #if VL_PIPELINE_PRECISION == 2
    //! Defined as: \p 'typedef \p dmat2 \p mat2'. See also \ref VL_PIPELINE_PRECISION.
    typedef dmat2 mat2;
  #else
    //! Defined as: \p 'typedef \p fmat2 \p mat2'. See also \ref VL_PIPELINE_PRECISION.
    typedef fmat2 mat2;
  #endif
}

#endif
