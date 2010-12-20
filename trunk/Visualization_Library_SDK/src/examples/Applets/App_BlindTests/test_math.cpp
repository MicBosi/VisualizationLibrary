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
  bool test_math()
  {
    // 1) testa tutti i metodi
    // 2) implementa operator+() basato su operator+=() e non viceversa -> quaternion e anche tutte le altre classi

    // quaternion compilation and functionality tests

    std::set<fquat> qset;
    qset.insert(fquat());

    dquat qd;
    fquat q1(1,2,3,4), q2( fvec4(1,2,3,4) );
    fquat q3(q1), q4(1,fvec3(2,3,4));
    q2 = fvec4(1,2,3,4);
    fvec4 v1;
    q1 = q2;
    q1 = (fquat)qd;
    v1 = q1 * v1;

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
    q1.fromAxisAngle(fvec3(0,1,0), 90);
    q1.toAxisAngle(v3, angle);
    CONDITION( distance(v3, fvec3(0,1,0)) < 0.001f );
    CONDITION( angle == 90 );

    q1.fromVectors(fvec3(1,0,0), fvec3(-1,0,-1));
    q1.toAxisAngle(v3, angle);
    CONDITION( distance(v3, fvec3(0,1,0)) < 0.001f );
    CONDITION( angle == 135 );

    q1.fromMatrix( fmat4::rotation(0, 1, 1, 0) );
    q1.toAxisAngle(v3, angle);
    CONDITION( v3 == fvec3(0,0,0) )
    CONDITION( angle == 0 );

    for (float i=5.0; i<180.0; i+=5.0f)
    {
      q1.fromMatrix( fmat4::rotation(i, 1, 1, 0) );
      q1.toAxisAngle(v3, angle);
      CONDITION( distance(v3, fvec3(0.70710683f,0.70710683f,0)) < 0.001f );
      CONDITION( fabs(angle - i) < 0.001f );

      q1.fromAxisAngle( fvec3(1,1,0), i );
      q1.toAxisAngle(v3, angle);
      CONDITION( distance(v3, fvec3(0.70710683f,0.70710683f,0)) < 0.001f );
      CONDITION( fabs(angle - i) < 0.001f );

      q1.fromMatrix( fmat4::rotation(-i, 1, 1, 0) );
      q1.toAxisAngle(v3, angle);
      CONDITION( distance(v3, fvec3(-0.70710683f,-0.70710683f,0)) < 0.001f );
      CONDITION( fabs(angle - i) < 0.001f );

      q1.fromAxisAngle( fvec3(1,1,0), -i );
      q1.toAxisAngle(v3, angle);
      CONDITION( distance(v3, fvec3(-0.70710683f,-0.70710683f,0)) < 0.001f );
      CONDITION( fabs(angle - i) < 0.001f );

      q1.fromMatrix( fmat4::rotation(i, -1, -1, 0) );
      q1.toAxisAngle(v3, angle);
      CONDITION( distance(v3, fvec3(-0.70710683f,-0.70710683f,0)) < 0.001f );
      CONDITION( fabs(angle - i) < 0.001f );

      q1.fromAxisAngle( fvec3(-1,-1,0), i );
      q1.toAxisAngle(v3, angle);
      CONDITION( distance(v3, fvec3(-0.70710683f,-0.70710683f,0)) < 0.001f );
      CONDITION( fabs(angle - i) < 0.001f );
    }

    q1.fromEulerXYZ(10,20,30);
    q1.fromEulerZYX(10,20,30);
    q1.fromAxisAngle(fvec3(1,2,3), 90);
    fmat4 m1 = q1.toMatrix();
    v1 = q1.xyzw();
    q1 = v1;

    return true;
  }
}
