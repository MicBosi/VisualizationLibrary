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

#ifndef GeometricalPrimitives_INCLUDE_ONCE
#define GeometricalPrimitives_INCLUDE_ONCE

#include <vlCore/Vector4.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlCore/Colors.hpp>

namespace vl
{
  typedef enum { CC_FlatCap, CC_RoundedCap, CC_NoCap } ECapsuleCap;

  //! Creates a sphere by iteratively subdividing an icosahedron.
  ref<Geometry> makeIcosphere( const vec3& pos, Real diameter=1, int detail=2, bool remove_doubles = true );
  //! Creates a cylinder
  ref<Geometry> makeCylinder( const vec3& origin, Real diameter=1, Real height=1, int phi=20, int theta=2, bool top=true, bool bottom=true );
  //! Creates a uv sphere
  ref<Geometry> makeUVSphere( const vec3& origin, Real diameter=1, int phi=20, int theta=20 );
  //! Creates torus. This function generates also appropriate normals.
  ref<Geometry> makeTorus( const vec3& origin, Real diameter=1, Real thickness=0.2, int phi=10, int theta=10, float tex_coords = 0.0f );
  //! Creates a cone
  ref<Geometry> makeCone( const vec3& origin, Real diameter=1, Real height=1, int phi=10, bool bottom=true );
  //! Creates a pyramid
  ref<Geometry> makePyramid( const vec3& origin, Real side=1, Real height=1 );
  //! Creates a box
  ref<Geometry> makeBox( const vec3& origin, Real xside=1, Real yside=1, Real zside=1, bool tex_coords=true );
  //! Creates a box
  ref<Geometry> makeBox( const vec3& min, const vec3& max, bool tex_coords=true );
  //! Creates a box
  ref<Geometry> makeBox( const AABB& aabb, bool tex_coords=true );
  //! Creates a 2D grid
  ref<Geometry> makeGrid( const vec3& origin, Real xside, Real zside, int x, int z, bool gen_texcoords = false, fvec2 uv0=fvec2(0,0), fvec2 uv1=fvec2(1,1));
  //! Creates an icosahedron
  ref<Geometry> makeIcosahedron( const vec3& origin, Real diameter );
  //! Creates a Geometry representing a set of points
  ref<Geometry> makePoints( const std::vector< vec3 >& pos, const fvec4& color = white);
  //! Creates a 2D circle
  ref<Geometry> makeCircle( vec3 origin, Real radius, int slices = 60 );
  //! Creates a 3d capsule with rounded, flat or no caps
  ref<Geometry> makeCapsule(float radius, float height, int segments, ECapsuleCap top_cap, ECapsuleCap bottom_cap, const fvec4& top_col, const fvec4& bottom_col);
}

#endif
