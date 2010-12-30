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

#include "vl/Quaternion.hpp"
#include "vl/Say.hpp"
#include "vl/Log.hpp"
#include <set>

using namespace vl;

#define CONDITION(cond) \
  if (!(cond)) \
  { \
    vl::Log::print( Say("%s %n: condition \""#cond"\" failed.\n") << __FILE__ << __LINE__); \
    VL_CHECK(0); \
    return false; \
  }

namespace blind_tests
{
  bool test_quat();
  bool test_matr();

  bool test_math()
  {
    return test_matr() && test_quat();
  }

  bool test_matr()
  {
    //-------------------------------------------------------------------------
    // Matrix compilation and functionality tests
    //-------------------------------------------------------------------------

    // row/column order
    fmat4 m;
    m.e(0,0) = 1.0; m.e(0,1) = 0.0; m.e(0,2) = 0.0; m.e(0,3) = 13.0;
    m.e(1,0) = 0.0; m.e(1,1) = 1.0; m.e(1,2) = 0.0; m.e(1,3) = 17.0;
    m.e(2,0) = 0.0; m.e(2,1) = 0.0; m.e(2,2) = 1.0; m.e(2,3) = 19.0;
    m.e(3,0) = 0.0; m.e(3,1) = 0.0; m.e(3,2) = 0.0; m.e(3,3) =  1.0;
    fvec4 v4 = m * fvec4(0,0,0,1);
    CONDITION(v4 == fvec4(13,17,19,1))
    fvec3 v3;
    fvec2 v2;
    m.setT( fvec3(23,26,29) );
    v4 = m * fvec4(0,0,0,1);
    CONDITION(v4 == fvec4(23,26,29,1))

    // rotazione & concatenazione
    m = fmat4::getRotation( 90, 0,0,1 );
    v4 = m * fvec4(1,0,0,1);
    v3 = m * fvec3(1,0,0);
    v2 = m * fvec2(1,0);
    CONDITION( distance( v4, fvec4(0,1,0,1) ) < 0.001f );
    CONDITION( distance( v3, fvec3(0,1,0)   ) < 0.001f );
    CONDITION( distance( v2, fvec2(0,1)     ) < 0.001f );

    m = fmat4::getRotation( 180, 1,0,0 ) * fmat4::getRotation( 90, 0,0,1 );
    v4 = m * fvec4(1,0,0,1);
    v3 = m * fvec3(1,0,0);
    v2 = m * fvec2(1,0);
    CONDITION( distance( v4, fvec4(0,-1,0,1) ) < 0.001f );
    CONDITION( distance( v3, fvec3(0,-1,0)   ) < 0.001f );
    CONDITION( distance( v2, fvec2(0,-1)     ) < 0.001f );

    // inversione
    fmat4 inv = m.getInverse();
    v4 = inv * m * fvec4(1,0,0,1);
    v3 = inv * m * fvec3(1,0,0);
    v2 = inv * m * fvec2(1,0);
    CONDITION( distance( v4, fvec4(1,0,0,1) ) < 0.001f );
    CONDITION( distance( v3, fvec3(1,0,0)   ) < 0.001f );
    CONDITION( distance( v2, fvec2(1,0)     ) < 0.001f );

    v4 = m * inv * fvec4(1,0,0,1);
    v3 = m * inv * fvec3(1,0,0);
    v2 = m * inv * fvec2(1,0);
    CONDITION( distance( v4, fvec4(1,0,0,1) ) < 0.001f );
    CONDITION( distance( v3, fvec3(1,0,0)   ) < 0.001f );
    CONDITION( distance( v2, fvec2(1,0)     ) < 0.001f );

    // pre-multiply
    m = fmat4::getRotation( -90, 0,0,1 ) * fmat4::getRotation( -180, 1,0,0 );
    v4 = fvec4(1,0,0,1) * m;
    v3 = fvec3(1,0,0) * m;
    v2 = fvec2(1,0) * m;
    CONDITION( distance( v4, fvec4(0,-1,0,1) ) < 0.001f );
    CONDITION( distance( v3, fvec3(0,-1,0)   ) < 0.001f );
    CONDITION( distance( v2, fvec2(0,-1)     ) < 0.001f );

    // getX/Y/Z/T()
    m = fmat4::getTranslation(13,17,19) * fmat4::getRotation( 90, 1,0,0 ) * fmat4::getRotation( 90, 0,0,1 );
    fvec3 x = m.getX();
    fvec3 y = m.getY();
    fvec3 z = m.getZ();
    CONDITION( m.getT() == fvec3(13,17,19) );
    CONDITION( distance(x, fvec3(0,0,+1)) < 0.001f );
    CONDITION( distance(y, fvec3(-1,0,0)) < 0.001f );
    CONDITION( distance(z, fvec3(0,-1,0)) < 0.001f );

    // transposition
    for(int i=0; i<4; ++i)
      for(int j=0; j<4; ++j)
        m.e(i,j) = rand();
    fmat4 m2 = m;
    m2.transpose();
    for(int i=0; i<4; ++i)
    {
      for(int j=0; j<4; ++j)
      {
        CONDITION( m2.e(i,j) = m.e(j,i) );
      }
    }
    CONDITION( m2 == m.getTransposed() )

    // misc
    m = fmat4::getRotation( 90, 1,0,0 ) * fmat4::getRotation( 90, 0,0,1 ) * fmat4::getIdentity();
    m = m * m.getTransposed();
    v4 = m * fvec4(1,0,0,1);
    v3 = m * fvec3(1,0,0);
    v2 = m * fvec2(1,0);
    CONDITION( distance( v4, fvec4(1,0,0,1) ) < 0.001f );
    CONDITION( distance( v3, fvec3(1,0,0)   ) < 0.001f );
    CONDITION( distance( v2, fvec2(1,0)     ) < 0.001f );

    // operators compilation checks
    //m = fmat4() * fmat4();
    //m *= fmat4();
    m = fmat4(3.0f) + fmat4(4.0f);
    m += fmat4(7.0f);
    CONDITION( m == fmat4(14.0f) );
    m = fmat4(10.0f) - fmat4(3.0f);
    m -= fmat4(5.0f);
    CONDITION( m == fmat4(2.0f) );
    // m = fmat4() / fmat4();
    // m /= fmat4();
    m = fmat4(3.0f) * 10.0f;
    m *= 10.0f;
    CONDITION( m == fmat4(300.0f) );
    m = fmat4(0.0f) + 10.0f;
    m += 5.0f;
    CONDITION( m == fmat4().fill(+15.0f) );
    m = fmat4(0.0f) - 10.0f;
    m -= 5.0f;
    CONDITION( m == fmat4().fill(-15.0f) );
    m = fmat4(40.0f) / 10.0f;
    m /= 2.0f;
    CONDITION( m == fmat4(2.0f) );

    CONDITION( fmat4(1) == fmat4(1) );
    CONDITION( fmat4(1) != fmat4(2) );

    // mic fixme:
    // coprire tutte le funzioni di tutti i tipi di matrici.
    // documentare tutte le funzioni.
    // GLSLmath implementazione da rifare tutta coi templates.

    return true;
  }

  bool test_quat()
  {
    //-------------------------------------------------------------------------
    // Quaternion compilation and functionality tests
    //-------------------------------------------------------------------------

    std::set<fquat> fqset;
    fqset.insert(fquat());

    std::set<dquat> dqset;
    dqset.insert(dquat());

    dquat qd;
    fquat q1(1,2,3,4), q2( fvec4(1,2,3,4) );
    fquat q3(q1), q4(1,fvec3(2,3,4));
    q2 = fvec4(1,2,3,4);
    fvec4 v1;
    q1 = q2;
    q1 = (fquat)qd;
    qd = (dquat)q1;
    v1 = q1 * v1;
    v1 = q1.xyzw();
    q1 = v1;
    q1 = fquat::getSquad(0, q1,q1,q1,q1);

    q1 = fquat(4,3,2,1);
    q2 = fquat(4,3,2,1);
    CONDITION(q1 == q2 && !(q1 != q2))
    q2 = fquat(3,2,1,0);
    CONDITION(q1 != q2 && !(q1 == q2))

    q1 = fquat(1,2,3,4);
    q1.setZero();
    CONDITION( q1 == fquat(0,0,0,0) );

    q1 = fquat(1,2,3,4);
    q1.setNoRotation();
    CONDITION( q1 == fquat(0,0,0,1) );

    fvec3 v3; float angle = 0;
    q1.setFromAxisAngle(fvec3(0,1,0), 90);
    q1.toAxisAngle(v3, angle);
    CONDITION( distance(v3, fvec3(0,1,0)) < 0.001f );
    CONDITION( angle == 90 );

    q1.setFromVectors(fvec3(1,0,0), fvec3(-1,0,-1));
    q1.toAxisAngle(v3, angle);
    CONDITION( distance(v3, fvec3(0,1,0)) < 0.001f );
    CONDITION( angle == 135 );

    q1.setFromMatrix( fmat4::getRotation(0, 1, 1, 0) );
    q1.toAxisAngle(v3, angle);
    CONDITION( v3 == fvec3(0,0,0) )
    CONDITION( angle == 0 );

    fvec3 A3 = fquat::getFromAxisAngle(fvec3(1,1,0), 35) * fquat::getFromAxisAngle(fvec3(0,1,1), 35) * fquat::getFromAxisAngle(fvec3(1,0,1), 35) * fvec3(1,0,0);
    fvec3 B3 = fmat4::getRotation(35, fvec3(1,1,0)) * fmat4::getRotation(35, fvec3(0,1,1)) * fmat4::getRotation(35, fvec3(1,0,1)) * fvec3(1,0,0);
    CONDITION( distance(A3, B3) < 0.001f )

    fvec4 A4 = fquat::getFromAxisAngle(fvec3(1,1,0), 35) * fquat::getFromAxisAngle(fvec3(0,1,1), 35) * fquat::getFromAxisAngle(fvec3(1,0,1), 35) * fvec4(1,0,0,1);
    fvec4 B4 = fmat4::getRotation(35, fvec3(1,1,0)) * fmat4::getRotation(35, fvec3(0,1,1)) * fmat4::getRotation(35, fvec3(1,0,1)) * fvec4(1,0,0,1);
    CONDITION( distance(A4, B4) < 0.001f )

    q1 = fquat::getZero();
    fquat::getZero(q1);
    q1.setZero();

    q1 = fquat::getNoRotation();
    fquat::getNoRotation(q1);
    q1.setNoRotation();

    q1 = fquat::getFromVectors(fvec3(1,2,3), fvec3(3,2,1));
    fquat::getFromVectors(q1, fvec3(1,2,3), fvec3(3,2,1));
    q1.setFromVectors(fvec3(1,2,3), fvec3(3,2,1));

    fmat3 m3;
    q1 = fquat::getFromMatrix(m3);
    fquat::getFromMatrix(q1, m3);
    q1.setFromMatrix(m3);

    fmat4 m4;
    q1 = fquat::getFromMatrix(m4);
    fquat::getFromMatrix(q1, m4);
    q1.setFromMatrix(m4);

    q1 = fquat::getFromEulerXYZ(10,20,30);
    fquat::getFromEulerXYZ(q1, 10,20,30);
    q1.setFromEulerXYZ(10,20,30);

    q1 = fquat::getFromEulerZYX(10,20,30);
    fquat::getFromEulerZYX(q1, 10,20,30);
    q1.setFromEulerZYX(10,20,30);

    q1 = fquat::getFromAxisAngle( fvec3(1,2,3), 10 );
    fquat::getFromAxisAngle(q1, fvec3(1,2,3), 10);
    q1.setFromAxisAngle(fvec3(1,2,3), 10);

    q1.toMatrix3(m3);
    m3 = q1.toMatrix3();

    q1.toMatrix4(m4);
    m4 = q1.toMatrix4();

    CONDITION(fquat().getFromAxisAngle(fvec3(1,1,0),10).dot(fquat().getFromAxisAngle(fvec3(1,1,0),30)) > 0 );

    q1.normalize();
    q1.getNormalized(q2);
    q2 = q1.getNormalized();

    q1 = q2.getConjugate();
    q2.getConjugate(q1);

    q1 = q2.getInverse();
    q2.getInverse(q1);

    q1 = fquat::getSlerp( 0.5, q1, q2 );
    fquat::getSlerp( q1, 0.5, q1, q2 );

    q1 = fquat::getNlerp( 0.5, q1, q2 );
    fquat::getNlerp( q1, 0.5, q1, q2 );

    q1 = fquat::getSquad( 0.5, q1, q2, q1, q2 );
    fquat::getSquad( q1, 0.5, q1, q2, q1, q2 );

    {
      fquat q1 = fquat::getFromAxisAngle( fvec3(0,1,0), 45.0f*1.0f );
      fquat q2 = fquat::getFromAxisAngle( fvec3(0,1,0), 45.0f*3.0f );

      for(int i=0; i<=100; ++i)
      {
        float t = i / 100.0f;
        fquat qs = fquat::getSlerp( t, q1, q2 );
        fquat qn = fquat::getNlerp( t, q1, q2 );
        fmat4 m  = fmat4::getRotation( 45.0f*1.0f*(1-t) + 45.0f*3.0f*t, 0,1,0 );
        fvec3 v1 = qs * fvec3(1,0,0);
        fvec3 v2 = qn * fvec3(1,0,0);
        fvec3 v3 =  m * fvec3(1,0,0);
        CONDITION( distance(v1,v2) < 0.0175f );
        CONDITION( distance(v1,v3) < 0.001f  );
        CONDITION( distance(v2,v3) < 0.0175f );
      }
    }

    fvec3 faxes[] = { 
      fvec3(+1,+1, 0).normalize(), 
      fvec3( 0,+1,+1).normalize(), 
      fvec3(+1, 0,+1).normalize(), 
      fvec3(-1,-1, 0).normalize(), 
      fvec3( 0,-1,-1).normalize(), 
      fvec3(-1, 0,-1).normalize(), 
      fvec3(1,0,0).normalize(),
      fvec3(0,1,0).normalize(),
      fvec3(0,0,1).normalize(),
      fvec3(0,0,0)
    };

    for (int k=1; k<180/*1800*/; k+=1)
    {
      float i = k / 1.0f/*10.0f*/;
      for(int j=0; !faxes[j].isNull(); ++j )
      {
        fvec3 ax;
        fquat q;
        float alpha;
        float alpha_eps = 0.005f;
        float mat_eps = 0.01f;

        q.setFromMatrix( fmat4::getRotation(i, faxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(fmat4::getRotation(i, faxes[j])) < mat_eps )

        q.setFromAxisAngle( faxes[j], i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );

        q.setFromMatrix( fmat4::getRotation(-i, faxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(fmat4::getRotation(-i, faxes[j])) < mat_eps )

        q.setFromAxisAngle( faxes[j], -i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );

        q.setFromMatrix( fmat4::getRotation(i, -faxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(fmat4::getRotation(i, -faxes[j])) < mat_eps )

        q.setFromAxisAngle( -faxes[j], i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );

        q.setFromMatrix( fmat4::getRotation(-i, -faxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(fmat4::getRotation(-i, -faxes[j])) < mat_eps )

        q.setFromAxisAngle( -faxes[j], -i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, faxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
      }
    }

    dvec3 daxes[] = { 
      dvec3(+1,+1, 0).normalize(), 
      dvec3( 0,+1,+1).normalize(), 
      dvec3(+1, 0,+1).normalize(), 
      dvec3(-1,-1, 0).normalize(), 
      dvec3( 0,-1,-1).normalize(), 
      dvec3(-1, 0,-1).normalize(), 
      dvec3(1,0,0).normalize(),
      dvec3(0,1,0).normalize(),
      dvec3(0,0,1).normalize(),
      dvec3(0,0,0)
    };

    for (int k=1; k<18000; k+=1)
    {
      double i = k / 100.0;
      for(int j=0; !daxes[j].isNull(); ++j )
      {
        dvec3 ax;
        dquat q;
        double alpha;
        double alpha_eps = 0.005f;
        double mat_eps = 0.01f;

        q.setFromMatrix( dmat4::getRotation(i, daxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(dmat4::getRotation(i, daxes[j])) < mat_eps )

        q.setFromAxisAngle( daxes[j], i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );

        q.setFromMatrix( dmat4::getRotation(-i, daxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(dmat4::getRotation(-i, daxes[j])) < mat_eps )

        q.setFromAxisAngle( daxes[j], -i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );

        q.setFromMatrix( dmat4::getRotation(i, -daxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(dmat4::getRotation(i, -daxes[j])) < mat_eps )

        q.setFromAxisAngle( -daxes[j], i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, -daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );

        q.setFromMatrix( dmat4::getRotation(-i, -daxes[j]) );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
        CONDITION( q.toMatrix4().diff(dmat4::getRotation(-i, -daxes[j])) < mat_eps )

        q.setFromAxisAngle( -daxes[j], -i );
        q.toAxisAngle(ax, alpha);
        CONDITION( distance(ax, daxes[j]) < 0.001f );
        CONDITION( fabs(alpha - i) < alpha_eps );
      }
    }

    q1 = fquat::getFromAxisAngle(fvec3(1,1,1), 45);
    q1 = q1 * q1.getInverse();
    v3 = q1 * fvec3(1,1,1);
    CONDITION( distance(v3,fvec3(1,1,1)) < 0.001f )
    v1 = q1 * fvec4(1,1,1,1);
    CONDITION( distance(v1,fvec4(1,1,1,1)) < 0.001f )

    return true;
  }
}
