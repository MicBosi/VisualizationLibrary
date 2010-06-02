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

#include "BaseDemo.hpp"
#include "vl/RayIntersector.hpp"
#include "vl/ReadPixels.hpp"

class App_Picking: public BaseDemo
{
public:

  void mouseDownEvent(vl::EMouseButton, int x, int y)
  {
    vl::Camera* camera = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera();

    // convert Y coordinates to the OpenGL conventions
    y = openglContext()->height() - y;
    // compute the ray passing through the selected pixel
    vl::Ray ray = camera->computeRay(x,y);
    // instance our ray-intersector
    vl::RayIntersector intersector;
    // compute a frustum along the ray to accelerate the intersection test
    intersector.setFrustum( camera->computeRayFrustum( x,y ) );
    // compute the intersections!
    // (1) short way
    intersector.intersect(ray, sceneManager());
    // (2) long way
    /*
    // specify the Actor[s] to be tested
    intersector.actors()->clear();
    sceneManager()->appendActors( *intersector.actors() );
    // set the intersecting ray
    intersector.setRay(ray);
    // run intersection test
    intersector.intersect();
    */

    // inspect our intersections, the intersections returned are sorted according to their distance, the first one is the closest.
    if (intersector.intersections().size())
    {
      // highlight the intersection point by moving the green sphere there
      mIntersectionPoint->setLocalMatrix( vl::mat4() );
      mIntersectionPoint->translate( intersector.intersections()[0]->intersectionPoint() );
      mIntersectionPoint->computeWorldMatrix();

      // print the name of the picked object
      vl::Log::print( vl::Say("Intersections detected = %n (%s).\n") << intersector.intersections().size() << intersector.intersections()[0]->actor()->name() );
    }
    else
      vl::Log::print("No intersections detected.\n");
  }

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    // populate our scene with some random objects

    vl::ref<vl::Effect> fx = new vl::Effect;
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->gocLight(0)->setLinearAttenuation(0.025f);
    fx->shader()->gocMaterial()->setColorMaterialEnabled(true);

    int   count    = 1;
    float displace = 2.0f;
    for(int z=-count; z<=count; ++z)
    for(int y=-count; y<=count; ++y)
    for(int x=-count; x<=count; ++x)
    {
      vl::ref<vl::Geometry> geom = randomObject();
      vl::Actor* act = sceneManager()->tree()->addActor( geom.get(), fx.get(), new vl::Transform );
      act->setName(geom->name());
      act->transform()->translate(x*displace, y*displace, z*displace);
      act->transform()->computeWorldMatrix();
    }

    // create a uv-sphere used to highlight the intersection point

    vl::ref<vl::Geometry> intersection_point_geom = vlut::makeUVSphere(vl::vec3(0,0,0), 0.1f);
    intersection_point_geom->computeNormals();
    intersection_point_geom->setColorArray(vlut::green);
    vl::Actor* intersection_point_act = sceneManager()->tree()->addActor( intersection_point_geom.get(), fx.get(), new vl::Transform );
    mIntersectionPoint = intersection_point_act->transform();
  }

  // generate random objects
  vl::ref<vl::Geometry> randomObject()
  {
    vl::ref<vl::Geometry> geom;
    // random shape
    switch(rand() % 6)
    {
      case 0: geom = vlut::makeIcosphere(vl::vec3(0,0,0), 1, 2, false); break;
      case 1: geom = vlut::makeBox(vl::vec3(0,0,0), 1, 1, 1); break;
      case 2: geom = vlut::makeCone(vl::vec3(0,0,0),1,1); break;
      case 3: geom = vlut::makeUVSphere(vl::vec3(0,0,0),1); break;
      case 4: geom = vlut::makeCylinder(vl::vec3(0,0,0),1,1); break;
      case 5: geom = vlut::makeTorus(vl::vec3(0,0,0),2,0.5f,20,20); break;
    }
    // random color
    geom->setColorArray( vl::fvec4((float)vl::randomMinMax(0,1), (float)vl::randomMinMax(0,1), (float)vl::randomMinMax(0,1),1.0f) );
    // normals are needed for lighting
    geom->computeNormals();
    return geom;
  }

protected:
  vl::Transform* mIntersectionPoint;
};

// Have fun!
