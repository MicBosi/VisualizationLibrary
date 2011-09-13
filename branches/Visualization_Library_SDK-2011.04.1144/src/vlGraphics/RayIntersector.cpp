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

#include <vlGraphics/RayIntersector.hpp>
#include <vlGraphics/SceneManager.hpp>

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
  ArrayFloat3* vert3f = dynamic_cast<ArrayFloat3*>(geom->vertexArray());
  ArrayDouble3* vert3d = dynamic_cast<ArrayDouble3*>(geom->vertexArray());
  ArrayHFloat3* vert3h = dynamic_cast<ArrayHFloat3*>(geom->vertexArray());
  if (vert3f)
  {
    fmat4 matrix = act->transform() ? (fmat4)act->transform()->worldMatrix() : fmat4();
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      DrawCall* prim = geom->drawCalls()->at(i);
      int itri = 0;
      for(TriangleIterator trit = prim->triangleIterator(); !trit.isEnd(); trit.next(), ++itri)
      {
        fvec3 a = vert3f->at(trit.a());
        fvec3 b = vert3f->at(trit.b());
        fvec3 c = vert3f->at(trit.c());
        if (act->transform())
        {
          a = matrix * a;
          b = matrix * b;
          c = matrix * c;
        }
        intersectTriangle(a, b, c, act, geom, prim, itri);
      }
    }
  }
  else
  if (vert3d)
  {
    dmat4 matrix = act->transform() ? (dmat4)act->transform()->worldMatrix() : dmat4();
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      DrawCall* prim = geom->drawCalls()->at(i);
      int itri = 0;
      for(TriangleIterator trit = prim->triangleIterator(); !trit.isEnd(); trit.next(), ++itri)
      {
        dvec3 a = vert3d->at(trit.a());
        dvec3 b = vert3d->at(trit.b());
        dvec3 c = vert3d->at(trit.c());
        if (act->transform())
        {
          a = matrix * a;
          b = matrix * b;
          c = matrix * c;
        }
        intersectTriangle(a, b, c, act, geom, prim, itri);
      }
    }
  }
  else
  if (vert3h)
  {
    fmat4 matrix = act->transform() ? (fmat4)act->transform()->worldMatrix() : fmat4();
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      DrawCall* prim = geom->drawCalls()->at(i);
      int itri = 0;
      for(TriangleIterator trit = prim->triangleIterator(); !trit.isEnd(); trit.next(), ++itri)
      {
        fvec3 a = (fvec3)vert3h->at(trit.a());
        fvec3 b = (fvec3)vert3h->at(trit.b());
        fvec3 c = (fvec3)vert3h->at(trit.c());
        if (act->transform())
        {
          a = matrix * a;
          b = matrix * b;
          c = matrix * c;
        }
        intersectTriangle(a, b, c, act, geom, prim, itri);
      }
    }
  }
}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectTriangle(const T& a, const T& b, const T& c, Actor* act, Geometry* geom, DrawCall* prim, int tri_idx)
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
  record->setTriangleIndex(tri_idx);
  record->setActor(act);
  record->setGeometry(geom);
  record->setPrimitives(prim);
  record->setDistance( t );
  mIntersections.push_back(record);
}
//-----------------------------------------------------------------------------
