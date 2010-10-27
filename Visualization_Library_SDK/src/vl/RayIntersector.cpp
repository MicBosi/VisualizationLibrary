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

#include <vl/RayIntersector.hpp>
#include <vl/SceneManager.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
void RayIntersector::intersect(const Ray& ray, SceneManager* scene_manager)
{
  actors()->clear();
  scene_manager->extractActors( *actors() );
  setRay(ray);
  intersect();
}
//-----------------------------------------------------------------------------
void RayIntersector::intersect()
{
  mIntersections.clear();
  for(int i=0; i<actors()->size(); ++i)
  {
    if (!frustum().cull(actors()->at(i)->boundingBox()))
    {
      intersect(actors()->at(i));
    }
  }

  std::sort( mIntersections.begin(), mIntersections.end(), sorter );
}
//-----------------------------------------------------------------------------
void RayIntersector::intersect(Actor* act)
{
  Geometry* geom = dynamic_cast<Geometry*>(act->lod(0).get());
  if (geom)
    intersectGeometry(act, geom);
}
//-----------------------------------------------------------------------------
void RayIntersector::intersectGeometry(Actor* act, Geometry* geom)
{
  ArrayFVec3* vert3f = dynamic_cast<ArrayFVec3*>(geom->vertexArray());
  ArrayDVec3* vert3d = dynamic_cast<ArrayDVec3*>(geom->vertexArray());
  if (vert3f)
  {
    fmat4 matrix = act->transform() ? (fmat4)act->transform()->worldMatrix() : fmat4();

    for(int i=0; i<geom->primitives()->size(); ++i)
    {
      Primitives* prim = geom->primitives()->at(i);
      if (prim->primitiveType() == vl::PT_TRIANGLES)
      {
        for(unsigned itri=0; itri<prim->indexCount(); itri+=3)
        {
          fvec3 a = vert3f->at(prim->index(itri+0));
          fvec3 b = vert3f->at(prim->index(itri+1));
          fvec3 c = vert3f->at(prim->index(itri+2));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
          }
          intersectTriangle(a,b,c, act,geom,prim,itri);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_TRIANGLE_FAN)
      {
        for(unsigned itri=1; itri<prim->indexCount()-1; ++itri)
        {
          fvec3 a = vert3f->at(prim->index(0));
          fvec3 b = vert3f->at(prim->index(itri));
          fvec3 c = vert3f->at(prim->index(itri+1));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
          }
          intersectTriangle(a,b,c, act,geom,prim,itri);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_TRIANGLE_STRIP)
      {
        for(unsigned itri=0; itri<prim->indexCount()-2; ++itri)
        {
          fvec3 a = vert3f->at(prim->index(itri));
          fvec3 b = vert3f->at(prim->index(itri+1));
          fvec3 c = vert3f->at(prim->index(itri+2));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
          }
          intersectTriangle(a,b,c, act,geom,prim,itri);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_POLYGON)
      {
        std::vector<fvec3> polygon;
        for(unsigned itri=0; itri<prim->indexCount(); ++itri)
        {
          fvec3 a = vert3f->at(prim->index(itri));
          if (act->transform())
            a = matrix * a;
          polygon.push_back(a);
        }
        VL_CHECK(polygon.size() >= 3)
        intersectPolygon(polygon, act,geom,prim);
      }
      else
      if (prim->primitiveType() == vl::PT_QUADS)
      {
        for(unsigned iquad=0; iquad<prim->indexCount(); iquad+=4)
        {
          fvec3 a = vert3f->at(prim->index(iquad+0));
          fvec3 b = vert3f->at(prim->index(iquad+1));
          fvec3 c = vert3f->at(prim->index(iquad+2));
          fvec3 d = vert3f->at(prim->index(iquad+3));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
            d = matrix * d;
          }
          intersectQuad(a,b,c,d, act,geom,prim,iquad);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_QUAD_STRIP)
      {
        for(unsigned iquad=0; iquad<prim->indexCount()-3; iquad+=2)
        {
          fvec3 a = vert3f->at(prim->index(iquad+0));
          fvec3 b = vert3f->at(prim->index(iquad+1));
          fvec3 c = vert3f->at(prim->index(iquad+2));
          fvec3 d = vert3f->at(prim->index(iquad+3));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
            d = matrix * d;
          }
          intersectQuad(a,b,c,d, act,geom,prim,iquad);
        }
      }
    }
  }
  else
  if (vert3d)
  {
    dmat4 matrix = act->transform() ? (dmat4)act->transform()->worldMatrix() : dmat4();

    for(int i=0; i<geom->primitives()->size(); ++i)
    {
      Primitives* prim = geom->primitives()->at(i);
      if (prim->primitiveType() == vl::PT_TRIANGLES)
      {
        for(unsigned itri=0; itri<prim->indexCount(); itri+=3)
        {
          dvec3 a = vert3d->at(prim->index(itri+0));
          dvec3 b = vert3d->at(prim->index(itri+1));
          dvec3 c = vert3d->at(prim->index(itri+2));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
          }
          intersectTriangle(a,b,c, act,geom,prim,itri);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_TRIANGLE_FAN)
      {
        for(unsigned itri=1; itri<prim->indexCount()-1; ++itri)
        {
          dvec3 a = vert3d->at(prim->index(0));
          dvec3 b = vert3d->at(prim->index(itri));
          dvec3 c = vert3d->at(prim->index(itri+1));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
          }
          intersectTriangle(a,b,c, act,geom,prim,itri);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_TRIANGLE_STRIP)
      {
        for(unsigned itri=0; itri<prim->indexCount()-2; ++itri)
        {
          dvec3 a = vert3d->at(prim->index(itri));
          dvec3 b = vert3d->at(prim->index(itri+1));
          dvec3 c = vert3d->at(prim->index(itri+2));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
          }
          intersectTriangle(a,b,c, act,geom,prim,itri);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_POLYGON)
      {
        std::vector<dvec3> polygon;
        for(unsigned itri=0; itri<prim->indexCount(); ++itri)
        {
          dvec3 a = vert3d->at(prim->index(itri));
          if (act->transform())
            a = matrix * a;
          polygon.push_back(a);
        }
        VL_CHECK(polygon.size() >= 3)
        intersectPolygon(polygon, act,geom,prim);
      }
      else
      if (prim->primitiveType() == vl::PT_QUADS)
      {
        for(unsigned iquad=0; iquad<prim->indexCount(); iquad+=4)
        {
          dvec3 a = vert3d->at(prim->index(iquad+0));
          dvec3 b = vert3d->at(prim->index(iquad+1));
          dvec3 c = vert3d->at(prim->index(iquad+2));
          dvec3 d = vert3d->at(prim->index(iquad+3));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
            d = matrix * d;
          }
          intersectQuad(a,b,c,d, act,geom,prim,iquad);
        }
      }
      else
      if (prim->primitiveType() == vl::PT_QUAD_STRIP)
      {
        for(unsigned iquad=0; iquad<prim->indexCount()-3; iquad+=2)
        {
          dvec3 a = vert3d->at(prim->index(iquad+0));
          dvec3 b = vert3d->at(prim->index(iquad+1));
          dvec3 c = vert3d->at(prim->index(iquad+2));
          dvec3 d = vert3d->at(prim->index(iquad+3));
          if (act->transform())
          {
            a = matrix * a;
            b = matrix * b;
            c = matrix * c;
            d = matrix * d;
          }
          intersectQuad(a,b,c,d, act,geom,prim,iquad);
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectTriangle(const T& a, const T& b, const T& c, Actor* act, Geometry* geom, Primitives* prim, int prim_idx)
{
  T v1 = b-a;
  T v2 = c-a;
  T n = cross(v1,v2).normalize();
  Real det = (Real)dot(n,(T)ray().direction());
  if(det == 0)
    return;
  Real t = (Real)dot(n, a-(T)ray().origin()) / det;
  if (t<0)
    return;
  vec3  rp = ray().origin() + ray().direction() * t;
  T fp = (T)rp;
  T pts[] = { a, b, c, a };
  for(int i=0; i<3; ++i)
  {
    T bi_norm = -cross(pts[i+1]-pts[i],n).normalize();
    if (dot(fp-pts[i],bi_norm) < 0)
      return;
  }
  ref<RayIntersectionGeometry> record = new vl::RayIntersectionGeometry;
  record->setIntersectionPoint( rp );
  record->setPrimitiveIndex(prim_idx);
  record->setActor(act);
  record->setGeometry(geom);
  record->setPrimitives(prim);
  record->setDistance( t );
  mIntersections.push_back(record);
}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectPolygon(const std::vector<T>& polygon, Actor* act, Geometry* geom, Primitives* prim)
{
  T v1 = polygon[1]-polygon[0];
  T v2 = polygon[2]-polygon[0];
  T n = cross(v1,v2).normalize();
  float det = (Real)dot(n,(T)ray().direction());
  if(det == 0.0f)
    return;
  float t = (Real)dot(n, polygon[0]-(T)ray().origin()) / det;
  if (t<0)
    return;
  vec3  rp = ray().origin() + ray().direction()*t;
  T fp = (T)rp;
  for(unsigned i=0; i<polygon.size(); ++i)
  {
    int i2 = (i+1) % polygon.size();
    T bi_norm = -cross(polygon[i2]-polygon[i],n).normalize();
    if (dot(fp-polygon[i],bi_norm) < 0)
      return;
  }
  ref<RayIntersectionGeometry> record = new vl::RayIntersectionGeometry;
  record->setIntersectionPoint( rp );
  record->setPrimitiveIndex(0);
  record->setActor(act);
  record->setGeometry(geom);
  record->setPrimitives(prim);
  record->setDistance( t );
  mIntersections.push_back(record);
}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectQuad(const T& a, const T& b, const T& c, const T& d, Actor* act, Geometry* geom, Primitives* prim, int prim_idx)
{
  T v1 = b-a;
  T v2 = c-a;
  T n = cross(v1,v2).normalize();
  float det = (Real)dot(n,(T)ray().direction());
  if(det == 0.0f)
    return;
  float t = (Real)dot(n, a-(T)ray().origin()) / det;
  if (t<0)
    return;
  vec3  rp = ray().origin() + ray().direction()*t;
  T fp = (T)rp;
  T pts[] = {a,b,c,d,a};
  for(int i=0; i<4; ++i)
  {
    T bi_norm = -cross(pts[i+1]-pts[i],n).normalize();
    if (dot(fp-pts[i],bi_norm) < 0)
      return;
  }
  ref<RayIntersectionGeometry> record = new vl::RayIntersectionGeometry;
  record->setIntersectionPoint( rp );
  record->setPrimitiveIndex(prim_idx);
  record->setActor(act);
  record->setGeometry(geom);
  record->setPrimitives(prim);
  record->setDistance( t );
  mIntersections.push_back(record);
}
//-----------------------------------------------------------------------------
