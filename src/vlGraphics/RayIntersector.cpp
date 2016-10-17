/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
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
#include <vlGraphics/LineIterator.hpp>

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
  Geometry* geom = cast<Geometry>(act->lod(0));
  if (geom)
    intersectGeometry(act, geom);
}
//-----------------------------------------------------------------------------
void RayIntersector::intersectGeometry(Actor* act, Geometry* geom)
{
  ArrayAbstract* posarr = geom->vertexArray() ? geom->vertexArray() : geom->vertexAttribArray(vl::VA_Position) ? geom->vertexAttribArray(vl::VA_Position)->data() : NULL;
  if (posarr)
  {
    mat4 matrix = act->transform() ? act->transform()->worldMatrix() : mat4();
    for(int i=0; i<geom->drawCalls().size(); ++i)
    {
      DrawCall* prim = geom->drawCalls().at(i);
      int itri = 0;
      if(prim->primitiveType() == vl::PT_LINES ||
              prim->primitiveType() == vl::PT_LINE_STRIP ||
              prim->primitiveType() == vl::PT_LINE_LOOP)
      {
        for(LineIterator liit = prim->lineIterator(); liit.hasNext(); liit.next(), ++itri)
        {
            int ia = liit.a();
            int ib = liit.b();
            vec3 a = posarr->getAsVec3(ia);
            vec3 b = posarr->getAsVec3(ib);
            if(act->transform())
            {
                a = matrix * a;
                b = matrix * b;
            }
            // Compute distance to the line. Compute all and keep the minimum( attribute for an epsilon?)
            intersectLine(a, b, ia, ib, act, geom, prim, itri);
        }
      }
      else if(prim->primitiveType() == vl::PT_POINTS)
      {
          for(unsigned int i = 0; i < posarr->size(); i++, ++itri)
          {
             vec3 a = posarr->getAsVec3(i);
             if(act->transform())
             {
                 a = matrix * a;
             }
             intersectPoint(a, i, act, geom, prim, itri);
          }
      }
      else
      {
        for(TriangleIterator trit = prim->triangleIterator(); trit.hasNext(); trit.next(), ++itri)
        {
            int ia = trit.a();
            int ib = trit.b();
            int ic = trit.c();
            vec3 a = posarr->getAsVec3(ia);
            vec3 b = posarr->getAsVec3(ib);
            vec3 c = posarr->getAsVec3(ic);
            if (act->transform())
            {
                a = matrix * a;
                b = matrix * b;
                c = matrix * c;
            }
            intersectTriangle(a, b, c, ia, ib, ic, act, geom, prim, itri);
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectTriangle(const T& a, const T& b, const T& c, int ia, int ib, int ic, Actor* act, Geometry* geom, DrawCall* prim, int tri_idx)
{
  T v1 = b-a;
  T v2 = c-a;
  T n = cross(v1,v2).normalize();
  real det = (real)dot(n,(T)ray().direction());
  if(det == 0)
    return;
  real t = (real)dot(n, a-(T)ray().origin()) / det;
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

  // compute the nearest point
  T aRp = rp - a;
  T bRp = rp - b;
  T cRp = rp - c;

  T v3 = b - c;
  float d1 = cross(aRp, v1).length() / v1.length();
  float d2 = cross(aRp, v2).length() / v2.length();
  float d3 = cross(cRp, v3).length() / v3.length();

  int ila, ilb;
  int l_idx;
  if(d1 < d2)
  {
      if(d1 < d3)
      {
          ila = ia;
          ilb = ib;
          l_idx = 1;
      }
      else
      {
          ila = ic;
          ilb = ib;
          l_idx = 2;
      }
  }
  else
  {
      if(d2 < d3)
      {
          ila = ia;
          ilb = ic;
          l_idx = 3;
      }
      else
      {
          ila = ic;
          ilb = ib;
          l_idx = 2;
      }
  }

  ref<RayIntersectionGeometry> record = new vl::RayIntersectionGeometry;
  record->setIntersectionPoint( rp );
  record->setTriangleIndex(tri_idx);
  record->setTriangle(ia, ib, ic);
  record->setLineIndex(l_idx);
  record->setLine(ila, ilb);
  record->setNearestPoint(aRp.lengthSquared() < bRp.lengthSquared() ? (aRp.lengthSquared() < cRp.lengthSquared() ? ia : ic) : (bRp.lengthSquared() < cRp.lengthSquared() ? ib : ic));
  record->setActor(act);
  record->setGeometry(geom);
  record->setPrimitives(prim);
  record->setDistance( t );
  mIntersections.push_back(record);
}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectLine(const T& a, const T& b, int ia, int ib, Actor* act, Geometry* geom, DrawCall* prim, int tri_idx)
{
	T   u = b - a;
	T   w = a - ray().origin();
	T   v = ray().direction();
	
	// In 3D
	// // We got the parametric representation of both line
	// Line AB
	// x = a.x() + u.x() * t
	// y = a.y() + u.y() * t
	// z = a.z() + u.z() * t
	// Ray direction
	// x' = ray().origin().x() + ray().direction().x() * s
	// y' = ray().origin().y() + ray().direction().y() * s
	// z' = ray().origin().z() + ray().direction().z() * s
	// So we have to solve
	// x = x'
	// y = y'
	// z = z'
	// We change to express all above with s ant then we do
	// 2s - s - s = 0
	// So we can find t
	float denominator = 2 * ray().direction().y() * ray().direction().z() * u.x() - ray().direction().x() * ray().direction().z() * u.y() - ray().direction().x() * ray().direction().y() * u.z();
	if(denominator == 0)
		return;
    float t = (ray().direction().x() * ray().direction().z() * w.y() + ray().direction().x() * ray().direction().y() * w.z() - 2 * ray().direction().y() * ray().direction().z() * w.x()) / denominator;
 /* 
	In 2D
	float denominator = ray().direction().y() * (b.x() - a.x()) - ray().direction().x() * (b.y() - a.y());
	if(denominator == 0)
		return;
	float t = (ray().direction().x() * (a.y() - ray().origin().y()) - ray().direction().y() * (a.x() - ray().origin().x())) / denominator;*/
  if(t < 0 || t > 1)
      return;


  //--------------------------------------------------------------------------




      
      
      float    a1 = dot(u,u);        // always >= 0
      float    b1 = dot(u,v);
      float    c = dot(v,v);        // always >= 0
      float    d = dot(u,w);
      float    e = dot(v,w);
      float    D = a1*c - b1*b1;       // always >= 0
      float    sc, tc;

      // compute the line parameters of the two closest points
      if (D < 00000001) {         // the lines are almost parallel
          sc = 0.0;
          tc = (b1>c ? d/b1 : e/c);   // use the largest denominator
      }
      else {
          sc = (b1*e - c*d) / D;
          tc = (a1*e - b1*d) / D;
      }

      // get the difference of the two closest points
      T   dP = w + (sc * u) - (tc * v);  // = L1(sc) - L2(tc)

      float dist = dP.length();   // return the closest distance

      if(dist > mDistance)
          return;


        ref<RayIntersectionGeometry> record = new vl::RayIntersectionGeometry;
        //record->setIntersectionPoint( -1 );
        record->setTriangleIndex(-1);
        record->setTriangle(-1, -1, -1);
        record->setLineIndex(tri_idx);
        record->setLine(ia, ib);
        record->setNearestPoint(-1); // pour que ca ne crash pas dans ViewerActorTree au moment du decoupage de l'actor
		//record->setNearestPoint(aRp.lengthSquared() < bRp.lengthSquared() ? (aRp.lengthSquared() < cRp.lengthSquared() ? ia : ic) : (bRp.lengthSquared() < cRp.lengthSquared() ? ib : ic));
        record->setActor(act);
        record->setGeometry(geom);
        record->setPrimitives(prim);
        record->setDistance( dist );
        mIntersections.push_back(record);

}
//-----------------------------------------------------------------------------
template<class T>
void RayIntersector::intersectPoint(const T& a, int ia, Actor* act, Geometry* geom, DrawCall* prim, int prim_idx)
{
    // Find the equation of the plan thanks to the normal and one point
    float nd = ray().direction().x() * a.x() + ray().direction().y() * a.y() + ray().direction().z() * a.z();

    // Find the intersection between the ray and the plan
    float k = (-nd + ray().direction().x() * ray().origin().x() + ray().direction().y() * ray().origin().y() + ray().direction().z() * ray().origin().z()) /
            (-ray().direction().x() * ray().direction().x() - ray().direction().y() * ray().direction().y() - ray().direction().z() * ray().direction().z());

		//vl::defLogger()->error(vl::String("k = ") + vl::String::fromDouble(k));
			
    // Compute the distance between the intersection and our point (a)
    T intersectionPoint(ray().origin().x() + k * ray().direction().x(),
                        ray().origin().y() + k * ray().direction().y(),
                        ray().origin().z() + k * ray().direction().z());

    float distance = length(a - intersectionPoint);
	
	//vl::defLogger()->error(vl::String("distance = ") + vl::String::fromDouble(distance));
    if(distance > 0.1f)  // To convert into pixel
        return;

    ref<RayIntersectionGeometry> record = new vl::RayIntersectionGeometry;
    record->setIntersectionPoint(a);
    record->setTriangleIndex(-1);
    record->setTriangle(-1, -1, -1);
    record->setLineIndex(-1);
    record->setLine(-1, -1);
    record->setNearestPoint(prim_idx);
    record->setActor(act);
    record->setGeometry(geom);
    record->setPrimitives(prim);
    mIntersections.push_back(record);

}
//-----------------------------------------------------------------------------
