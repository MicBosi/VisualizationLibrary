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

#include <vl/Vector4.hpp>
#include <vl/Geometry.hpp>
#include <vlut/Colors.hpp>

namespace vlut
{
  typedef enum { CC_FlatCap, CC_RoundedCap, CC_NoCap } ECapsuleCap;

  //! Creates a sphere by iteratively subdividing an icosahedron.
  vl::ref<vl::Geometry> makeIcosphere( const vl::vec3& pos, vl::Real diameter=1, int detail=2, bool remove_doubles = true );
  //! Creates a cylinder
  vl::ref<vl::Geometry> makeCylinder( const vl::vec3& origin, vl::Real diameter=1, vl::Real height=1, int phi=20, int theta=2, bool top=true, bool bottom=true );
  //! Creates a uv sphere
  vl::ref<vl::Geometry> makeUVSphere( const vl::vec3& origin, vl::Real diameter=1, int phi=20, int theta=20 );
  //! Creates torus. This function generates also appropriate normals.
  vl::ref<vl::Geometry> makeTorus( const vl::vec3& origin, vl::Real diameter=1, vl::Real thickness=0.2, int phi=10, int theta=10, float tex_coords = 0.0f );
  //! Creates a cone
  vl::ref<vl::Geometry> makeCone( const vl::vec3& origin, vl::Real diameter=1, vl::Real height=1, int phi=10, bool bottom=true );
  //! Creates a pyramid
  vl::ref<vl::Geometry> makePyramid( const vl::vec3& origin, vl::Real side=1, vl::Real height=1 );
  //! Creates a box
  vl::ref<vl::Geometry> makeBox( const vl::vec3& origin, vl::Real xside=1, vl::Real yside=1, vl::Real zside=1, bool tex_coords=true );
  //! Creates a box
  vl::ref<vl::Geometry> makeBox( const vl::vec3& min, const vl::vec3& max, bool tex_coords=true );
  //! Creates a box
  vl::ref<vl::Geometry> makeBox( const vl::AABB& aabb, bool tex_coords=true );
  //! Creates a 2D grid
  vl::ref<vl::Geometry> makeGrid( const vl::vec3& origin, vl::Real xside, vl::Real zside, int x, int z, bool gen_texcoords = false, vl::fvec2 uv0=vl::fvec2(), vl::fvec2 uv1=vl::fvec2());
  //! Creates an icosahedron
  vl::ref<vl::Geometry> makeIcosahedron( const vl::vec3& origin, vl::Real diameter );
  //! Creates a Geometry representing a set of points
  vl::ref<vl::Geometry> makePoints( const std::vector< vl::vec3 >& pos, const vl::fvec4& color = vlut::white);
  //! Creates a 2D circle
  vl::ref<vl::Geometry> makeCircle( vl::vec3 origin, vl::Real radius, int slices = 60 );
  //! Creates a 3d capsule with rounded, flat or no caps
  vl::ref<vl::Geometry> makeCapsule(float radius, float height, int segments, ECapsuleCap top_cap, ECapsuleCap bottom_cap, const vl::fvec4& top_col, const vl::fvec4& bottom_col);
}

#endif
