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

#include <vl/quat.hpp>
#include <vl/math3D.hpp>
#include <cmath>

using namespace vl;

//-----------------------------------------------------------------------------
quat& quat::fromEulerXYZ(Real degX, Real degY, Real degZ )
{
  *this = quat(degX, vec3(1,0,0)) * quat(degY, vec3(0,1,0)) * quat(degZ, vec3(0,0,1));
  return *this;
}
//-----------------------------------------------------------------------------
quat& quat::fromEulerZYX(Real degZ, Real degY, Real degX )
{
  *this = quat(degZ, vec3(0,0,1)) * quat(degY, vec3(0,1,0)) * quat(degX, vec3(1,0,0));
  return *this;
}
//-----------------------------------------------------------------------------
quat& quat::fromMatrix(const mat4& m)
{
  Real tr, s, q[4];

  int nxt[3] = {1, 2, 0};

  tr = m[0][0] + m[1][1] + m[2][2];

  // check the diagonal
  if (tr > 0.0) 
  {
    s = vl::sqrt(tr + (Real)1.0);
    w() = s / (Real)2.0;
    s = (Real)0.5 / s;
    x() = (m[1][2] - m[2][1]) * s;
    y() = (m[2][0] - m[0][2]) * s;
    z() = (m[0][1] - m[1][0]) * s;
  } 
  else 
  {    
    // diagonal is negative
    int i, j, k;
    i = 0;
    if (m[1][1] > m[0][0]) i = 1;
    if (m[2][2] > m[i][i]) i = 2;
    j = nxt[i];
    k = nxt[j];

    s = vl::sqrt((m[i][i] - (m[j][j] + m[k][k])) + (Real)1.0);

    q[i] = s * (Real)0.5;

    if (s != 0.0) 
      s = (Real)0.5 / s;

    q[3] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;

    x() = q[0];
    y() = q[1];
    z() = q[2];
    w() = q[3];

  }

  return *this;
}
//-----------------------------------------------------------------------------
quat& quat::fromAxisAngle(Real degrees, const vec3& axis)
{
  degrees *= (Real)dDEG_TO_RAD;
  Real sa2 = sin(degrees * (Real)0.5);
  vec3 na = axis;
  na.normalize();
  mXYZW.x() = na.x() * sa2;
  mXYZW.y() = na.y() * sa2;
  mXYZW.z() = na.z() * sa2;
  mXYZW.w() = cos(degrees * (Real)0.5);
  return *this;
}
//-----------------------------------------------------------------------------
//! Expects a unit length quaternion.
void quat::toAxisAngle( vec4& axis, Real& angle ) const
{
  Real iscale = inversesqrt( x()*x() + y()*y() + z()*z() );
  axis.x() = x() * iscale;
  axis.y() = y() * iscale;
  axis.z() = z() * iscale;
  VL_CHECK(w()>=-1.0 && w()<=+1.0)
  Real tw = clamp(w(),(Real)-1.0,(Real)+1.0);
  angle = acos( tw ) * (Real)2.0;
}
//-----------------------------------------------------------------------------
//! Expects a unit length quaternion.
mat4 quat::toMatrix() const
{
  Real x2 = x() * x();
  Real y2 = y() * y();
  Real z2 = z() * z();
  Real xy = x() * y();
  Real xz = x() * z();
  Real yz = y() * z();
  Real wx = w() * x();
  Real wy = w() * y();
  Real wz = w() * z();
 
  return mat4( 
    (1.0f - 2.0f * (y2 + z2)), (2.0f * (xy + wz)),        (2.0f * (xz - wy)),        0.0f,
    (2.0f * (xy - wz)),        (1.0f - 2.0f * (x2 + z2)), (2.0f * (yz + wx)),        0.0f,
    (2.0f * (xz + wy)),        (2.0f * (yz - wx)),        (1.0f - 2.0f * (x2 + y2)), 0.0f,
    0.0f,                      0.0f,                      0.0f,                      1.0f );

}
//-----------------------------------------------------------------------------
quat quat::slerp( Real t, const quat& a, const quat& b ) const
{
  Real scale_a, scale_b, omega, sinom;
  Real cosom = a.dot(b);
  
  quat b2(b);
  
  if ( cosom < 0.0 )
  { 
      cosom = -cosom; 
      b2 = -b;
  }

  // acos(x) is defined only for -1 ... +1
  if( cosom >= -1.0 && cosom <= 1.0)
  {
    omega = acos(cosom);
    sinom = sin(omega);
    scale_a = sin( ((Real)1.0-t) * omega) / sinom;
    scale_b = sin(  t * omega) / sinom;
  }
  else
  {
    // linear interpolation for degenerate cases
    scale_a = (Real)1.0 - t;
    scale_b = t;
  }

  return (a*scale_a) + (b2*scale_b);
}
//-----------------------------------------------------------------------------
quat& quat::fromVectors(const vec3& from, const vec3& to)
{
  vec3 a,b;
  a = from;
  b = to;
  a.normalize();
  b.normalize();
  Real cosa = vl::dot(a,b);
  cosa = clamp(cosa, (Real)-1.0, (Real)+1.0);
  if (cosa == 0.0)
    *this = quat().setNoRotation();
  else
  {
    vec3 axis,n2;
    axis = cross(a,b);
    axis.normalize();
    Real alpha = acos( cosa );
    *this = quat().fromAxisAngle(alpha*(Real)dDEG_TO_RAD, axis);
  }
  return *this;
}
//-----------------------------------------------------------------------------
