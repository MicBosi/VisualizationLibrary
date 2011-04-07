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
#include "vlut/GeometryPrimitives.hpp"
#include "vl/Light.hpp"

class App_Transforms: public BaseDemo
{
public:
  App_Transforms() {}

  virtual void shutdown() {}

  virtual void run()
  {
    // rotate arm0
    mTransfArm0->setLocalMatrix( vl::mat4::getRotation(vl::Time::currentTime()*30.0f, 0.0f, 1.0f, 0.0f) );

    // rotate arm1
    mTransfArm1->setLocalMatrix( vl::mat4::getTranslation(0,10,0)*vl::mat4::getRotation( sin(vl::Time::currentTime())*120.0f, 1.0f, 0, 0.0f) );

    // rotate arm2
    mTransfArm2->setLocalMatrix( vl::mat4::getTranslation(0,10,0)*vl::mat4::getRotation( sin(vl::Time::currentTime())*120.0f, 0.0f, 0, 1.0f) );

    // rotate arm3
    mTransfArm3->setLocalMatrix( vl::mat4::getTranslation(0,10,0)*vl::mat4::getRotation( sin(vl::Time::currentTime())*120.0f, -1.0f, 0, 0.0f) );

    // rotate finger1
    mTransfHand1->setLocalMatrix( vl::mat4::getTranslation(0,10,0)*vl::mat4::getRotation( (sin(vl::Time::currentTime()*8.0f)*0.5f+0.5f)*60.0f, 1.0f, 0, 0.0f) );

    // rotate finger2
    mTransfHand2->setLocalMatrix( vl::mat4::getTranslation(0,10,0)*vl::mat4::getRotation( (sin(vl::Time::currentTime()*8.0f)*0.5f+0.5f)*-60.0f, 1.0f, 0, 0.0f) );
  }

  virtual void initEvent()
  {
    BaseDemo::initEvent();

    /* basic render states */

    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->setRenderState( new vl::Light(0) );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->gocMaterial()->setColorMaterialEnabled(true);

    /* working desk */

    vl::ref<vl::Geometry> plane = vlut::makeGrid(vl::vec3(0,0,0), 50,50, 2,2);
    plane->computeNormals();
    plane->setColor(vlut::gray);
    sceneManager()->tree()->addActor(plane.get(), effect.get());

    /* buttons */

    /* shows how to use Transforms if they don't need to be dynamically
       animated: first of all you don't put them in the vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()'s
       hierarchy like the other transforms; secondly you have to manually call
       computeWorldMatrix()/computeWorldMatrixRecursive() to compute
       the final matrix used for the rendering. This way the rendering pipeline
       won't call computeWorldMatrix()/computeWorldMatrixRecursive()
       continuously for the Transforms we know are not going to change over time,
       thus saving precious time. */

    vl::ref<vl::Transform> tr;

    vl::ref<vl::Geometry> button = vlut::makeCylinder(vl::vec3(0,0.5,0), 1.5, 1);
    button->setColor(vlut::orange);
    button->computeNormals();

    tr = new vl::Transform( vl::mat4::getTranslation(-6,0,10) );
    tr->computeWorldMatrix(NULL);
    sceneManager()->tree()->addActor(button.get(), effect.get(), tr.get());

    tr = new vl::Transform( vl::mat4::getTranslation(-4,0,10) );
    tr->computeWorldMatrix(NULL);
    sceneManager()->tree()->addActor(button.get(), effect.get(), tr.get());

    tr = new vl::Transform( vl::mat4::getTranslation(-2,0,10) );
    tr->computeWorldMatrix(NULL);
    sceneManager()->tree()->addActor(button.get(), effect.get(), tr.get());

    tr = new vl::Transform( vl::mat4::getTranslation(+2,0,10) );
    tr->computeWorldMatrix(NULL);
    sceneManager()->tree()->addActor(button.get(), effect.get(), tr.get());

    tr = new vl::Transform( vl::mat4::getTranslation(+4,0,10) );
    tr->computeWorldMatrix(NULL);
    sceneManager()->tree()->addActor(button.get(), effect.get(), tr.get());

    tr = new vl::Transform( vl::mat4::getTranslation(+6,0,10) );
    tr->computeWorldMatrix(NULL);
    sceneManager()->tree()->addActor(button.get(), effect.get(), tr.get());

    /* robot base */

    vl::ref<vl::Geometry>  arm_base = vlut::makeBox(vl::vec3(0,0.5,0), 12, 1, 12);
    arm_base ->computeNormals();
    arm_base ->setColor(vlut::blue);
    sceneManager()->tree()->addActor(arm_base.get(), effect.get());

    /* robot arms */

    vl::ref<vl::Geometry>  arm0    = vlut::makeBox(vl::vec3(0,5,0), 2, 10, 2);
    arm0->computeNormals();
    arm0->setColor(vlut::red);
    mTransfArm0 = new vl::Transform;
    sceneManager()->tree()->addActor(arm0.get(), effect.get(), mTransfArm0.get());

    vl::ref<vl::Geometry>  arm1    = vlut::makeCylinder(vl::vec3(0,5,0), 2, 10);
    arm1->computeNormals();
    arm1->setColor(vlut::green);
    mTransfArm1 = new vl::Transform;
    sceneManager()->tree()->addActor(arm1.get(), effect.get(), mTransfArm1.get());

    vl::ref<vl::Geometry>  arm2    = vlut::makeCylinder(vl::vec3(0,5,0), 2, 10);
    arm2->computeNormals();
    arm2->setColor(vlut::green);
    mTransfArm2 = new vl::Transform;
    sceneManager()->tree()->addActor(arm2.get(), effect.get(), mTransfArm2.get());

    vl::ref<vl::Geometry>  arm3    = vlut::makeCylinder(vl::vec3(0,5,0), 2, 10);
    arm3->computeNormals();
    arm3->setColor(vlut::green);
    mTransfArm3 = new vl::Transform;
    sceneManager()->tree()->addActor(arm3.get(), effect.get(), mTransfArm3.get());

    /* robot fingers */

    vl::ref<vl::Geometry>  finger1   = vlut::makeBox(vl::vec3(0,2,0), 2, 4, 0.5f);
    finger1->computeNormals();
    finger1->setColor(vlut::crimson);
    mTransfHand1 = new vl::Transform;
    sceneManager()->tree()->addActor(finger1.get(), effect.get(), mTransfHand1.get());

    vl::ref<vl::Geometry>  finger2   = vlut::makeBox(vl::vec3(0,2,0), 2, 4, 0.5f);
    finger2->computeNormals();
    finger2->setColor(vlut::crimson);
    mTransfHand2 = new vl::Transform;
    sceneManager()->tree()->addActor(finger2.get(), effect.get(), mTransfHand2.get());

    /* concatenate the transforms */

    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mTransfArm0.get());
    mTransfArm0->addChild(mTransfArm1.get());
    mTransfArm1->addChild(mTransfArm2.get());
    mTransfArm2->addChild(mTransfArm3.get());
    mTransfArm3->addChild(mTransfHand1.get());
    mTransfArm3->addChild(mTransfHand2.get());
  }

protected:
    vl::ref<vl::Transform> mTransfArm0;
    vl::ref<vl::Transform> mTransfArm1;
    vl::ref<vl::Transform> mTransfArm2;
    vl::ref<vl::Transform> mTransfArm3;
    vl::ref<vl::Transform> mTransfHand1;
    vl::ref<vl::Transform> mTransfHand2;
};

// Have fun!

BaseDemo* Create_App_Transforms() { return new App_Transforms; }
