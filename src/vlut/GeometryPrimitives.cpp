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

#include <vlut/GeometryPrimitives.hpp>
#include <vl/Geometry.hpp>
#include <vl/DoubleVertexRemover.hpp>

using namespace vl;
using namespace vlut;

//-----------------------------------------------------------------------------
//! \p detail can be between 0 ( = icosahedron) and 8 (extremely detailed sphere)
//! a value of 2 yelds already very good results.
ref<Geometry> vlut::makeIcosphere(const vec3& pos, Real diameter, int detail, bool remove_doubles)
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Icosphere");

  ref<ArrayFVec3> coords = new ArrayFVec3;
  ref<ArrayFVec3> norms = new ArrayFVec3;
  ref<DrawElementsUInt> polys = new DrawElementsUInt(PT_TRIANGLES);

  const Real X = (Real)0.525731112119133606;
  const Real Z = (Real)0.850650808352039932;
  std::vector< vec3 > verts;
  verts.push_back( vec3(-X, 0, Z) );
  verts.push_back( vec3(X, 0, Z) );
  verts.push_back( vec3(-X, 0, -Z) );
  verts.push_back( vec3(X, 0, -Z) );
  verts.push_back( vec3(0, Z, X) );
  verts.push_back( vec3(0, Z, -X) );
  verts.push_back( vec3(0, -Z, X) );
  verts.push_back( vec3(0, -Z, -X) );
  verts.push_back( vec3(Z, X, 0) );
  verts.push_back( vec3(-Z, X, 0) );
  verts.push_back( vec3(Z, -X, 0) );
  verts.push_back( vec3(-Z, -X, 0) );

  int idxs[] = { 
    1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4, 
    1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2, 
    3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
    10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
  };

  std::vector<int> indices;
  for(int i=0; i<4*5*3; ++i)
    indices.push_back(idxs[i]);

  // triangulate the icosahedron
  if (detail>8)
    detail = 8;
  if (detail<0)
    detail = 0;
  for(int i=0; i<detail; ++i)
  {
    std::vector<int> indices2;
    std::vector< vec3 > verts2;
    for( int j=0, idx=0; j<(int)indices.size(); j+=3)
    {
      indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
      indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
      indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);
      indices2.push_back(idx++); indices2.push_back(idx++); indices2.push_back(idx++);

      vec3 v1 = verts[ indices[j+0] ]; v1.normalize();
      vec3 v2 = verts[ indices[j+1] ]; v2.normalize();
      vec3 v3 = verts[ indices[j+2] ]; v3.normalize();
      vec3 a = (v1 + v2) * 0.5f; a.normalize();
      vec3 b = (v2 + v3) * 0.5f; b.normalize();
      vec3 c = (v3 + v1) * 0.5f; c.normalize();
      verts2.push_back(v1); verts2.push_back( a); verts2.push_back(c);
      verts2.push_back( a); verts2.push_back(v2); verts2.push_back(b);
      verts2.push_back( a); verts2.push_back( b); verts2.push_back(c);
      verts2.push_back( c); verts2.push_back( b); verts2.push_back(v3);
    }
    verts = verts2;
    indices = indices2;
  }

  // generate sphere vertices and connection information

  Real radius = diameter / 2;

  coords->resize( (int)verts.size() );
  norms->resize( (int)verts.size() );
  for( int i=0; i<(int)verts.size(); ++i )
  {
    coords->at(i) = (fvec3)(verts[i]*radius + pos);
    vec3 n = verts[i];
    n.normalize();
    norms->at(i) = (fvec3)n;
  }

  polys->indices()->resize( (int)indices.size() );
  for(int i=0; i<(int)indices.size(); ++i)
  {
    VL_CHECK( indices[i] < (int)coords->size() )
    polys->indices()->at(i) = indices[i];
  }

  geom->setVertexArray(coords.get());
  geom->setNormalArray(norms.get());
  geom->drawCalls()->push_back(polys.get());

  if (remove_doubles)
  {
    DoubleVertexRemover dvr;
    dvr.removeDoubles(geom.get());
  }

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeUVSphere( const vec3& origin, Real diameter, int phi, int theta)
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("UVSphere");

  diameter = diameter / 2.0f;
  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  geom->setVertexArray(vert3.get());

  // create vertices
  vert3->resize( theta * phi + 2 );
  int vert_idx=0;
  vert3->at(vert_idx++) = (fvec3)(vec3(0,1*diameter,0) + origin);
  for(int i=0; i<theta; ++i)
  {
    for(int j=0; j<phi; ++j)
    {
      // vec3 v(1*radius,radius - radius*2*((Real)i/(theta-1)),0);
      vec3 v(0,1*diameter,0);
      v = mat4::rotation(180.0f/(theta+1)*(i+1),0,0,1) * v;
      v = mat4::rotation(360.0f/phi*j,0,1,0)  * v;
      vert3->at(vert_idx++) = (fvec3)(v+origin);
    }
  }
  vert3->at(vert_idx++) = (fvec3)(vec3(0,-1*diameter,0) + origin);

  // side quads

  ref<DrawElementsUInt> quads = new DrawElementsUInt( PT_QUADS );
  quads->indices()->resize( (theta-1)*phi*4 );
  geom->drawCalls()->push_back(quads.get());
  int idx = 0;
  for(int i=0; i<theta-1; ++i)
  {
    for(int j=0; j<phi; ++j)
    {
      quads->indices()->at(idx++) = 1+phi*(i+1)+(j+0)%phi;
      quads->indices()->at(idx++) = 1+phi*(i+1)+(j+1)%phi;
      quads->indices()->at(idx++) = 1+phi*(i+0)+(j+1)%phi;
      quads->indices()->at(idx++) = 1+phi*(i+0)+(j+0)%phi;
    }
  }

  // top/bottom triangles

  ref<DrawElementsUInt> tris = new DrawElementsUInt( PT_TRIANGLES );
  tris->indices()->resize( phi*3 + phi*3 );
  geom->drawCalls()->push_back(tris.get());
  idx = 0;
  // top fan
  for(int j=0; j<phi; ++j)
  {
    tris->indices()->at(idx++) = 0;
    tris->indices()->at(idx++) = 1+(j+0)%phi;
    tris->indices()->at(idx++) = 1+(j+1)%phi;
  }
  // bottom fan
  for(int j=0; j<phi; ++j)
  {
    tris->indices()->at(idx++) = (int)geom->vertexArray()->size()-1;
    tris->indices()->at(idx++) = 1+phi*(theta-1)+(j+1)%phi;
    tris->indices()->at(idx++) = 1+phi*(theta-1)+(j+0)%phi;
  }

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeCylinder( const vec3& origin, Real diameter, Real height, int phi, int theta, bool top, bool bottom)
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Cylinder");

  diameter = diameter / 2;
  height = height / 2;
  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  geom->setVertexArray(vert3.get());

  // create vertices
  vert3->resize( theta * phi + (top?phi+1:0) + (bottom?phi+1:0) );
  int vert_idx=0;
  for(int i=0; i<theta; ++i)
  {
    for(int j=0; j<phi; ++j)
    {
      vec3 v(1*diameter, 1*height - 2*height*((Real)i/(theta-1)), 0);
      v = mat4::rotation(360.0f/phi*j,0,1,0) * v;
      vert3->at(vert_idx++) = (fvec3)(v + origin);
    }
  }

  // side quads

  ref<DrawElementsUInt> quads = new DrawElementsUInt( PT_QUADS );
  quads->indices()->resize( (theta-1)*phi*4 );
  geom->drawCalls()->push_back(quads.get());
  int idx = 0;
  for(int i=0; i<theta-1; ++i)
  {
    for(int j=0; j<phi; ++j)
    {
      quads->indices()->at(idx++) = phi*(i+1)+(j+0)%phi;
      quads->indices()->at(idx++) = phi*(i+1)+(j+1)%phi;
      quads->indices()->at(idx++) = phi*(i+0)+(j+1)%phi;
      quads->indices()->at(idx++) = phi*(i+0)+(j+0)%phi;
    }
  }

  // top/bottom triangles

  if (top)
  {
    ref<DrawElementsUInt> tris = new DrawElementsUInt( PT_TRIANGLE_FAN );
    tris->indices()->resize( phi+2 );
    geom->drawCalls()->push_back(tris.get());
    idx = 0;

    int fan_center = vert_idx;
    vert3->at(vert_idx++) = (fvec3)(vec3(0, height, 0)  + origin);
    for(int j=0; j<phi; ++j)
    {
      vec3 v(1*diameter, height, 0);
      v = mat4::rotation(360.0f/phi*j,0,1,0) * v;
      vert3->at(vert_idx++) = (fvec3)(v + origin);
    }

    // top fan
    tris->indices()->at(idx++) = fan_center;
    for(int j=0; j<phi+1; ++j)
      tris->indices()->at(idx++) = 1+fan_center+j%phi;
  }

  if (bottom)
  {
    ref<DrawElementsUInt> tris = new DrawElementsUInt( PT_TRIANGLE_FAN );
    tris->indices()->resize( phi+2 );
    geom->drawCalls()->push_back(tris.get());
    idx = 0;

    int fan_center = vert_idx;
    vert3->at(vert_idx++) = (fvec3)(vec3(0, -height, 0) + origin);
    for(int j=0; j<phi; ++j)
    {
      vec3 v(1*diameter, - height, 0);
      v = mat4::rotation(360.0f/phi*j,0,1,0) * v;
      vert3->at(vert_idx++) = (fvec3)(v + origin);
    }

    // bottom fan
    tris->indices()->at(idx++) = fan_center;
    for(int j=0; j<phi+1; ++j)
      tris->indices()->at(idx++) = 1+fan_center+(phi -1 - j%phi);
  }

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeTorus( const vec3& origin, Real diameter, Real thickness, int phi, int theta, float tex_coords )
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Torus");

  // create vertices
  thickness /= 2.0f;
  const Real radius = diameter / 2.0f - thickness;

  // vertices
  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  geom->setVertexArray(vert3.get());
  vert3->resize( (phi+1) * (theta+1) );

  // normals
  ref<ArrayFVec3> norm3 = new ArrayFVec3;
  geom->setNormalArray(norm3.get());
  norm3->resize( (phi+1) * (theta+1) );

  // texture coordinates
  ref<ArrayFVec2> texc2 = new ArrayFVec2;
  if (tex_coords)
  {
    geom->setTexCoordArray(0,texc2.get());
    texc2->resize( (phi+1) * (theta+1) );
  }

  int vect_idx = 0;
  for(int i=0; i<theta+1; ++i)
  {
    for(int j=0; j<phi+1; ++j)
    {
      vec3 v(thickness, 0, 0);
      vec3 o(radius, 0, 0);
      v = mat4::rotation(360.0f/phi*j,0,1,0) * v;
      v = mat4::rotation(360.0f/theta*i,0,0,1) * v;
      o = mat4::rotation(360.0f/theta*i,0,0,1) * o;

      if (tex_coords)
        texc2->at(vect_idx) = fvec2((float)i/theta,(float)j/phi) * tex_coords;

      vert3->at(vect_idx) = (fvec3)(v + o + origin);

      norm3->at(vect_idx) = v.normalize();

      ++vect_idx;
    }
  }

  ref<DrawElementsUInt> polys = new DrawElementsUInt( PT_QUADS );
  geom->drawCalls()->push_back(polys.get());
  int idx = 0;
  polys->indices()->resize( theta * phi * 4 );
  // create indices
  for(int i=0; i<theta; ++i)
  {
    for(int j=0; j<phi; ++j)
    {
      int i1 = i+1;
      polys->indices()->at(idx++) = (phi+1)*i +(j+0);
      polys->indices()->at(idx++) = (phi+1)*i +(j+1);
      polys->indices()->at(idx++) = (phi+1)*i1+(j+1);
      polys->indices()->at(idx++) = (phi+1)*i1+(j+0);
    }
  }

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeBox( const AABB& aabb, bool tex_coords )
{
  return makeBox( aabb.minCorner(), aabb.maxCorner(), tex_coords );
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeBox( const vec3& min, const vec3& max, bool tex_coords )
{
  return makeBox( (min+max)*0.5, max.x()-min.x(), max.y()-min.y(), max.z()-min.z(), tex_coords );
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeBox( const vec3& origin, Real xside, Real yside, Real zside, bool tex_coords)
{
  /*
  1--------0 
  |\       |\
  | 5------|-4
  2--------3 |
   \|       \| 
    6------- 7 
  */

  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Box");

  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  ref<ArrayFVec2> texc2 = new ArrayFVec2;
  geom->setVertexArray(vert3.get());
  geom->setTexCoordArray(0, texc2.get());

  Real x=xside/2.0f;
  Real y=yside/2.0f;
  Real z=zside/2.0f;

  fvec3 a0( (fvec3)(vec3(+x,+y,+z) + origin) );
  fvec3 a1( (fvec3)(vec3(-x,+y,+z) + origin) );
  fvec3 a2( (fvec3)(vec3(-x,-y,+z) + origin) );
  fvec3 a3( (fvec3)(vec3(+x,-y,+z) + origin) );
  fvec3 a4( (fvec3)(vec3(+x,+y,-z) + origin) );
  fvec3 a5( (fvec3)(vec3(-x,+y,-z) + origin) );
  fvec3 a6( (fvec3)(vec3(-x,-y,-z) + origin) );
  fvec3 a7( (fvec3)(vec3(+x,-y,-z) + origin) );

  ref<DrawArrays> polys = new DrawArrays(PT_QUADS, 0, 24);
  geom->drawCalls()->push_back( polys.get() );

  vert3->resize( 24  );

  vert3->at(0)  = a1; vert3->at(1)  = a2; vert3->at(2)  = a3; vert3->at(3)  = a0;
  vert3->at(4)  = a2; vert3->at(5)  = a6; vert3->at(6)  = a7; vert3->at(7)  = a3;
  vert3->at(8)  = a6; vert3->at(9)  = a5; vert3->at(10) = a4; vert3->at(11) = a7;
  vert3->at(12) = a5; vert3->at(13) = a1; vert3->at(14) = a0; vert3->at(15) = a4;
  vert3->at(16) = a0; vert3->at(17) = a3; vert3->at(18) = a7; vert3->at(19) = a4;
  vert3->at(20) = a5; vert3->at(21) = a6; vert3->at(22) = a2; vert3->at(23) = a1;

  texc2->resize( 24 );
  int idx = 0;
  if (tex_coords)
  {
    texc2->at(idx++) = fvec2(0,1); texc2->at(idx++) = fvec2(0,0); texc2->at(idx++) = fvec2(1,0); texc2->at(idx++) = fvec2(1,1);
    texc2->at(idx++) = fvec2(0,1); texc2->at(idx++) = fvec2(0,0); texc2->at(idx++) = fvec2(1,0); texc2->at(idx++) = fvec2(1,1);
    texc2->at(idx++) = fvec2(1,0); texc2->at(idx++) = fvec2(1,1); texc2->at(idx++) = fvec2(0,1); texc2->at(idx++) = fvec2(0,0);
    texc2->at(idx++) = fvec2(0,1); texc2->at(idx++) = fvec2(0,0); texc2->at(idx++) = fvec2(1,0); texc2->at(idx++) = fvec2(1,1);
    texc2->at(idx++) = fvec2(0,0); texc2->at(idx++) = fvec2(1,0); texc2->at(idx++) = fvec2(1,1); texc2->at(idx++) = fvec2(0,1);
    texc2->at(idx++) = fvec2(1,1); texc2->at(idx++) = fvec2(0,1); texc2->at(idx++) = fvec2(0,0); texc2->at(idx++) = fvec2(1,0);
  }

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makePyramid( const vec3& origin, Real side, Real height)
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Pyramid");

  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  geom->setVertexArray(vert3.get());

  Real x = side   / 2.0f;
  Real y = height;
  Real z = side   / 2.0f;

  fvec3 a0( (fvec3)(vec3(+0,+y,+0) + origin) );
  fvec3 a1( (fvec3)(vec3(-x,+0,-z) + origin) );
  fvec3 a2( (fvec3)(vec3(-x,-0,+z) + origin) );
  fvec3 a3( (fvec3)(vec3(+x,-0,+z) + origin) );
  fvec3 a4( (fvec3)(vec3(+x,+0,-z) + origin) );

  ref<DrawArrays> polys = new DrawArrays(PT_TRIANGLES, 0, 6*3);
  geom->drawCalls()->push_back( polys.get() );

  vert3->resize(6*3);

  vert3->at(0)  = a4; vert3->at(1)  = a2; vert3->at(2)  = a1; 
  vert3->at(3)  = a2; vert3->at(4)  = a4; vert3->at(5)  = a3; 
  vert3->at(6)  = a4; vert3->at(7)  = a1; vert3->at(8)  = a0; 
  vert3->at(9)  = a1; vert3->at(10) = a2; vert3->at(11) = a0;
  vert3->at(12) = a2; vert3->at(13) = a3; vert3->at(14) = a0;
  vert3->at(15) = a3; vert3->at(16) = a4; vert3->at(17) = a0;

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeCone( const vec3& origin, Real diameter, Real height, int phi, bool bottom)
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Cone");

  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  geom->setVertexArray( vert3.get() );

  diameter = diameter / 2;

  vert3->resize( phi+1 + (bottom?phi+1:0) );
  // create vertices
  int vert_idx = 0;
  vert3->at(vert_idx++) = (fvec3)(vec3(0, height/2.0f, 0) + origin);
  for(int j=0; j<phi; ++j)
  {
    vec3 v(1*diameter, -height/2.0f, 0);
    v = mat4::rotation(360.0f/phi*j,0,1,0) * v;
    vert3->at(vert_idx++) = (fvec3)(v + origin);
  }

  // top fan
  ref<DrawElementsUInt> top_fan = new DrawElementsUInt(PT_TRIANGLE_FAN);
  top_fan->indices()->resize(phi+2);
  geom->drawCalls()->push_back(top_fan.get());
  int idx = 0;
  top_fan->indices()->at(idx++) = 0;
  for(int j=0; j<phi+1; ++j)
    top_fan->indices()->at(idx++) = 1+j%phi;

  // bottom fan
  if (bottom)
  {
    int fan_center = vert_idx;
    vert3->at(vert_idx++) = (fvec3)(vec3(0, -height/2.0f, 0) + origin);
    for(int j=0; j<phi; ++j)
    {
      vec3 v(1*diameter, -height/2.0f, 0);
      v = mat4::rotation(360.0f/phi*j,0,1,0) * v;
      vert3->at(vert_idx++) = (fvec3)(v + origin);
    }

    ref<DrawElementsUInt> bottom_fan = new DrawElementsUInt(PT_TRIANGLE_FAN);
    bottom_fan->indices()->resize(phi+2);
    geom->drawCalls()->push_back(bottom_fan.get());
    idx = 0;
    bottom_fan->indices()->at(idx++) = fan_center;
    for(int j=0; j<phi+1; ++j)
      bottom_fan->indices()->at(idx++) = fan_center+1+(phi-1-j%phi);
  }

  return geom;
}
//-----------------------------------------------------------------------------
//! \note if tex_coord_scale_u and tex_coord_scale_v are both == 0 no texture coordinate is generated
ref<Geometry> vlut::makeGrid( const vec3& origin, Real xside, Real zside, int x, int z, bool gen_texcoords, fvec2 uv0, fvec2 uv1)
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Grid");

  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  ref<ArrayFVec2> text2 = new ArrayFVec2;
  geom->setVertexArray( vert3.get() );

  VL_CHECK(x>=2)
  VL_CHECK(z>=2)
  Real dx = xside / (x-1);
  Real dz = zside / (z-1);
  xside /= 2.0f;
  zside /= 2.0f;

  vert3->resize( x * z );
  if (gen_texcoords)
  {
    geom->setTexCoordArray( 0, text2.get() );
    text2->resize( x * z );
  }

  // create vertices
  int vert_idx = 0;
  for(int i=0; i<z; ++i)
    for(int j=0; j<x; ++j, ++vert_idx)
    {
      vert3->at(vert_idx) = (fvec3)(vec3(-xside+j*dx, 0, -zside+i*dz) + origin);
      if (gen_texcoords)
      {
        float tu = (float)j/(x-1); // 0 .. 1
        float tv = (float)i/(z-1); // 0 .. 1
        text2->at(vert_idx).s() = (1.0f-tu) * uv0.s() + tu * uv1.s();
        text2->at(vert_idx).t() = (1.0f-tv) * uv0.t() + tv * uv1.t();
      }
    }

  // create indices
  ref<DrawElementsUInt> polys = new DrawElementsUInt(PT_TRIANGLES);
  geom->drawCalls()->push_back(polys.get());
  int idx = 0;
  polys->indices()->resize( (z-1)*(x-1)*6 );
  for(int i=0; i<z-1; ++i)
    for(int j=0; j<x-1; ++j)
    {
      polys->indices()->at(idx++) = j+0 + x*(i+0);
      polys->indices()->at(idx++) = j+1 + x*(i+1);
      polys->indices()->at(idx++) = j+1 + x*(i+0);

      polys->indices()->at(idx++) = j+0 + x*(i+0);
      polys->indices()->at(idx++) = j+0 + x*(i+1);
      polys->indices()->at(idx++) = j+1 + x*(i+1);
    }

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makePoints( const std::vector< vec3>& pos, const fvec4& color )
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Points");

  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  ref<ArrayFVec4> col4 = new ArrayFVec4;
  geom->setVertexArray( vert3.get() );
  geom->setColorArray( col4.get() );
  vert3->resize( (int)pos.size() );
  col4->resize( (int)pos.size() );

  for(unsigned i=0; i<pos.size(); ++i)
  {
    vert3->at(i) = (fvec3)pos[i];
    col4->at(i)  = color;
  }

  geom->drawCalls()->push_back( new DrawArrays(PT_POINTS, 0, vert3->size() ));

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeIcosahedron( const vec3& origin, Real diameter )
{
  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Icosahedron");

  ref<ArrayFVec3> vert3 = new ArrayFVec3;
  geom->setVertexArray(vert3.get());

  // red book 1.4 p89

  const Real x = 0.525731112119133606f / 1.0f;
  const Real z = 0.850650808352039932f / 1.0f;
  const Real radius = diameter / 2.0f;

  vert3->resize( 12 );

  vert3->at(0) = (fvec3)(origin + vec3(-x, 0.0, +z)*radius);
  vert3->at(1) = (fvec3)(origin + vec3(+x, 0.0, +z)*radius);
  vert3->at(2) = (fvec3)(origin + vec3(-x, 0.0, -z)*radius);
  vert3->at(3) = (fvec3)(origin + vec3(+x, 0.0, -z)*radius);

  vert3->at(4) = (fvec3)(origin + vec3(0.0, +z, +x)*radius);
  vert3->at(5) = (fvec3)(origin + vec3(0.0, +z, -x)*radius);
  vert3->at(6) = (fvec3)(origin + vec3(0.0, -z, +x)*radius);
  vert3->at(7) = (fvec3)(origin + vec3(0.0, -z, -x)*radius);

  vert3->at(8)  = (fvec3)(origin + vec3(+z, +x, 0.0)*radius);
  vert3->at(9)  = (fvec3)(origin + vec3(-z, +x, 0.0)*radius);
  vert3->at(10) = (fvec3)(origin + vec3(+z, -x, 0.0)*radius);
  vert3->at(11) = (fvec3)(origin + vec3(-z, -x, 0.0)*radius);

  int faces[20][3] = 
  {
    {1,4,0},  {4,9,0},  {4,5,9},  {8,5,4},  {1,8,4}, 
    {1,10,8}, {10,3,8}, {8,3,5},  {3,2,5},  {3,7,2}, 
    {3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0}, 
    {10,1,6}, {11,0,9}, {2,11,9}, {5,2,9},  {11,2,7}
  };

  ref<DrawElementsUInt> polys = new DrawElementsUInt;
  geom->drawCalls()->push_back(polys.get());
  polys->indices()->resize(20*3);
  memcpy(polys->indices()->ptr(), faces, sizeof(int)*20*3);

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeCircle( vec3 origin, Real radius, int slices )
{
  ref< Geometry > geom = new Geometry;
  geom->setObjectName("Circle");

  ref< ArrayFVec3 > points = new ArrayFVec3;
  geom->setVertexArray(points.get());
  points->resize( slices );
  for(int i=0; i<slices; ++i)
  {
    Real t = 360.0f * i / slices;
    vec3 v = mat4::rotation(t,0,1,0) * vec3(radius,0,0) + origin;
    points->at(i) = (fvec3)v;
  }
  geom->drawCalls()->push_back( new DrawArrays(PT_LINE_LOOP, 0, points->size()) );

  return geom;
}
//-----------------------------------------------------------------------------
ref<Geometry> vlut::makeCapsule(float radius, float height, int segments, ECapsuleCap top_cap, ECapsuleCap bottom_cap, const fvec4& top_col, const fvec4& bottom_col)
{
  float height2 = height / 2.0f;

  ref<Geometry> geom = new Geometry;
  geom->setObjectName("Capsule");

  ref<ArrayFVec3> vert_array = new ArrayFVec3;
  ref<ArrayFVec4> colr_array = new ArrayFVec4;
  geom->setVertexArray(vert_array.get());
  geom->setColorArray (colr_array.get());
  std::vector<fvec3> verts;
  std::vector<fvec4> cols;

  // upper
  for(int i=0; i<segments; ++i)
  {
    float a = (float)i/segments*fPi*2.0f;
    fvec3 v(::cos(a)*radius,+height2,::sin(a)*radius);
    verts.push_back(v);
    cols.push_back(top_col);
  }
  if (top_col != bottom_col)
  {
    // mid-upper
    for(int i=0; i<segments; ++i)
    {
      float a = (float)i/segments*fPi*2.0f;
      fvec3 v(::cos(a)*radius,0,::sin(a)*radius);
      verts.push_back(v);
      cols.push_back(top_col);
    }
    // mid-lower
    for(int i=0; i<segments; ++i)
    {
      float a = (float)i/segments*fPi*2.0f;
      fvec3 v(::cos(a)*radius,0,::sin(a)*radius);
      verts.push_back(v);
      cols.push_back(bottom_col);
    }
    ref<DrawElementsUInt> de_up = new DrawElementsUInt(PT_QUADS);
    ref<DrawElementsUInt> de_lo = new DrawElementsUInt(PT_QUADS);
    geom->drawCalls()->push_back(de_up.get());
    geom->drawCalls()->push_back(de_lo.get());
    de_up->indices()->resize(segments*4);
    de_lo->indices()->resize(segments*4);
    int upup = segments*0;
    int uplo = segments*1;
    int loup = segments*2;
    int lolo = segments*3;
    for(int i=0; i<segments; ++i)
    {
      int i1 = (i+1) % segments;
      de_up->indices()->at(i*4+3) = uplo + i;
      de_up->indices()->at(i*4+2) = uplo + i1;
      de_up->indices()->at(i*4+1) = upup + i1;
      de_up->indices()->at(i*4+0) = upup + i;

      de_lo->indices()->at(i*4+3) = lolo + i;
      de_lo->indices()->at(i*4+2) = lolo + i1;
      de_lo->indices()->at(i*4+1) = loup + i1;
      de_lo->indices()->at(i*4+0) = loup + i;
    }
  }
  else
  {
    ref<DrawElementsUInt> de_up = new DrawElementsUInt(PT_QUADS);
    geom->drawCalls()->push_back(de_up.get());
    de_up->indices()->resize(segments*4);
    int upup = segments*0;
    int uplo = segments*1;
    for(int i=0; i<segments; ++i)
    {
      int i1 = (i+1) % segments;
      de_up->indices()->at(i*4+3) = uplo + i;
      de_up->indices()->at(i*4+2) = uplo + i1;
      de_up->indices()->at(i*4+1) = upup + i1;
      de_up->indices()->at(i*4+0) = upup + i;
    }
  }
  // lower
  for(int i=0; i<segments; ++i)
  {
    float a = (float)i/segments*fPi*2.0f;
    fvec3 v(::cos(a)*radius,-height2,::sin(a)*radius);
    verts.push_back(v);
    cols.push_back(bottom_col);
  }
  // caps
  if (top_cap == CC_FlatCap)
  {
    int start = verts.size();
    for(int i=0; i<segments; ++i)
    {
      float a = (float)i/segments*fPi*2.0f;
      fvec3 v(::cos(a)*radius,+height2,::sin(a)*radius);
      verts.push_back(v);
      cols.push_back(top_col);
    }
    ref<DrawElementsUInt> de = new DrawElementsUInt(PT_TRIANGLE_FAN);
    geom->drawCalls()->push_back(de.get());
    de->indices()->resize(segments);
    for(int i=0,j=segments; j--; ++i)
      de->indices()->at(j) = start + i;
  }
  if (bottom_cap == CC_FlatCap)
  {
    int start = verts.size();
    for(int i=0; i<segments; ++i)
    {
      float a = (float)i/segments*fPi*2.0f;
      fvec3 v(::cos(a)*radius,-height2,::sin(a)*radius);
      verts.push_back(v);
      cols.push_back(bottom_col);
    }
    ref<DrawElementsUInt> de = new DrawElementsUInt(PT_TRIANGLE_FAN);
    geom->drawCalls()->push_back(de.get());
    de->indices()->resize(segments);
    for(int i=0; i<segments; ++i)
      de->indices()->at(i) = start + i;
  }
  int segments2 = segments/3; if (segments2<2) segments2=2;  
  if (top_cap == CC_RoundedCap)
  {
    int start = verts.size();
    for(int j=0; j<segments2; ++j)
    {
      float aj = (float)j/segments2*fPi/2.0f;
      for(int i=0; i<segments; ++i)
      {
        float a = (float)i/segments*360;
        fvec3 v(::cos(aj)*radius,::sin(aj)*radius,0);
        verts.push_back(fmat4::rotation(a,0,1,0) * v + fvec3(0,height2,0));
        cols.push_back(top_col);
      }
    }
    // top point
    verts.push_back(fvec3(0,+height2+radius,0));
    cols.push_back(top_col);

    ref<DrawElementsUInt> de_quads = new DrawElementsUInt(PT_QUADS);
    geom->drawCalls()->push_back(de_quads.get());
    de_quads->indices()->resize(segments*(segments2-1)*4);
    for(int j=0,idx=0; j<segments2-1; ++j)
    {
      int uplo = start+segments*j;
      int upup = start+segments*(j+1);
      for(int i=0; i<segments; ++i)
      {
        int i1 = (i+1) % segments;
        de_quads->indices()->at(idx++) = uplo + i;
        de_quads->indices()->at(idx++) = uplo + i1;
        de_quads->indices()->at(idx++) = upup + i1;
        de_quads->indices()->at(idx++) = upup + i;
      }
    }

    ref<DrawElementsUInt> de = new DrawElementsUInt(PT_TRIANGLE_FAN);
    geom->drawCalls()->push_back(de.get());
    de->indices()->resize(segments+2);
    de->indices()->at(0) = (GLuint)verts.size()-1;
    for(int i=0; i<segments+1; ++i)
      de->indices()->at(i+1) = (GLuint)verts.size()-1-segments+i%segments;
  }
  if (bottom_cap == CC_RoundedCap)
  {
    int start = verts.size();
    for(int j=0; j<segments2; ++j)
    {
      float aj = (float)j/segments2*fPi/2.0f;
      for(int i=0; i<segments; ++i)
      {
        float a = -(float)i/segments*360;
        fvec3 v(::cos(aj)*radius,-::sin(aj)*radius,0);
        verts.push_back(fmat4::rotation(a,0,1,0) * v + fvec3(0,-height2,0));
        cols.push_back(bottom_col);
      }
    }
    // bottom point
    verts.push_back(fvec3(0,-height2-radius,0));
    cols.push_back(bottom_col);

    ref<DrawElementsUInt> de_quads = new DrawElementsUInt(PT_QUADS);
    geom->drawCalls()->push_back(de_quads.get());
    de_quads->indices()->resize(segments*(segments2-1)*4);
    for(int j=0,idx=0; j<segments2-1; ++j)
    {
      int uplo = start+segments*j;
      int upup = start+segments*(j+1);
      for(int i=0; i<segments; ++i)
      {
        int i1 = (i+1) % segments;
        de_quads->indices()->at(idx++) = uplo + i;
        de_quads->indices()->at(idx++) = uplo + i1;
        de_quads->indices()->at(idx++) = upup + i1;
        de_quads->indices()->at(idx++) = upup + i;
      }
    }

    ref<DrawElementsUInt> de = new DrawElementsUInt(PT_TRIANGLE_FAN);
    geom->drawCalls()->push_back(de.get());
    de->indices()->resize(segments+2);
    de->indices()->at(0) = (GLuint)verts.size()-1;
    for(int i=0; i<segments+1; ++i)
      de->indices()->at(i+1) = (GLuint)verts.size()-1-segments+i%segments;
  }

  *vert_array = verts;
  *colr_array = cols;

  return geom;
}
//-----------------------------------------------------------------------------
