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
#include "vlut/Colors.hpp"
#include "vlGraphics/PolygonSimplifier.hpp"
#include "vlGraphics/Geometry.hpp"
#include "vlGraphics/Light.hpp"

class App_PolygonReduction: public BaseDemo
{
public:
  App_PolygonReduction(const vl::String& path): mFileName(path) {}

  virtual void shutdown() {}

  void keyPressEvent(unsigned short, vl::EKey key)
  {
    if (key == vl::Key_1)
    {
      if (mEffect->shader()->gocShadeModel()->shadeModel() == vl::SM_FLAT)
        mEffect->shader()->gocShadeModel()->set(vl::SM_SMOOTH);
      else
        mEffect->shader()->gocShadeModel()->set(vl::SM_FLAT);
    }
    if (key == vl::Key_2)
    {
      if (mEffect->shader()->gocPolygonMode()->frontFace() == vl::PM_FILL)
        mEffect->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
      else
        mEffect->shader()->gocPolygonMode()->set(vl::PM_FILL, vl::PM_FILL);
    }
    if (key == vl::Key_3)
    {
      if (mEffect->shader()->isEnabled(vl::EN_CULL_FACE) )
        mEffect->shader()->disable(vl::EN_CULL_FACE);
      else
        mEffect->shader()->enable(vl::EN_CULL_FACE);
    }
  }

  virtual void run()
  {
    vl::Real degrees = 0;
    vl::mat4 matrix = vl::mat4::getRotation( degrees, 0,1,0 );
    mTransform_Left->setLocalMatrix( vl::mat4::getTranslation(-10,0,0) * matrix );
    mTransform_Right->setLocalMatrix(vl::mat4::getTranslation(+10,0,0) *  matrix );
  }

  void initEvent()
  {
    BaseDemo::initEvent();

    /* bind Transform */
    mTransform_Left  = new vl::Transform;
    mTransform_Right = new vl::Transform;
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mTransform_Left.get() );
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild( mTransform_Right.get() );

    /* bind Effect */
    mEffect = new vl::Effect;

    /* enable depth test and lighting */
    mEffect->shader()->enable(vl::EN_DEPTH_TEST);
    /* since no Transform is associated to the Light it will follow the camera */
    mEffect->shader()->setRenderState( new vl::Light(0) );
    mEffect->shader()->enable(vl::EN_LIGHTING);
    mEffect->shader()->gocLightModel()->setTwoSide(true);
    mEffect->shader()->gocMaterial()->setBackDiffuse( vl::darkgreen );

    vl::ref<vl::ResourceDatabase> res_db;
    vl::ref<vl::Geometry> geom1, geom2;
    res_db = vl::loadResource(mFileName); if ( res_db && res_db->count<vl::Geometry>() ) geom1 = res_db->get<vl::Geometry>(0);
    res_db = vl::loadResource(mFileName); if ( res_db && res_db->count<vl::Geometry>() ) geom2 = res_db->get<vl::Geometry>(0);

    // -------------- simplification start -------------------

    vl::PolygonSimplifier simplifier;
    simplifier.setQuick(false);
    simplifier.setVerbose(true);
    #if 1
      simplifier.simplify( 0.10f, geom1.get() );
    #else
      simplifier.simplify( 5000, geom1.get() );
    #endif

    // -------------- simplification end -------------------

    geom1->setDisplayListEnabled(false);
    geom2->setDisplayListEnabled(false);
    geom1->setVBOEnabled(true);
    geom2->setVBOEnabled(true);
    sceneManager()->tree()->addActor( geom1.get(), mEffect.get(), mTransform_Left.get() );
    sceneManager()->tree()->addActor( geom2.get(), mEffect.get(), mTransform_Right.get() );

    geom2->computeBounds();
    geom1->computeBounds();

    /* center and scale the geometry */

    vl::AABB aabb = geom1->boundingBox();
    geom1->transform( vl::mat4::getTranslation( - aabb.center() ) );
    vl::Real max = aabb.width();
    max = aabb.height() > max ? aabb.height() : max;
    max = aabb.depth() > max ? aabb.depth() : max;
    geom1->transform( vl::mat4::getScaling( vl::vec3(1.0f/max, 1.0f/max, 1.0f/max) * vl::vec3(15,15,15) ) );
    geom1->computeBounds();
    geom1->computeNormals();

    aabb = geom2->boundingBox();
    geom2->transform( vl::mat4::getTranslation( - aabb.center() ) );
    max = aabb.width();
    max = aabb.height() > max ? aabb.height() : max;
    max = aabb.depth() > max ? aabb.depth() : max;
    geom2->transform( vl::mat4::getScaling( vl::vec3(1.0f/max, 1.0f/max, 1.0f/max) * vl::vec3(15,15,15) ) );
    geom2->computeBounds();
    geom2->computeNormals();
  }

protected:
  vl::ref<vl::Transform> mTransform_Left;
  vl::ref<vl::Transform> mTransform_Right;
  vl::String mFileName;
  vl::ref<vl::Effect> mEffect;
};

// Have fun!

BaseDemo* Create_App_PolygonReduction(const vl::String& path) { return new App_PolygonReduction(path); }
