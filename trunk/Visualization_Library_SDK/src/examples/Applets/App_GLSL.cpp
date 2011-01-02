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
#include "vlGraphics/Geometry.hpp"
#include "vlGraphics/Light.hpp"
#include "vlGraphics/GLSL.hpp"

class App_GLSL: public BaseDemo
{
public:
  App_GLSL()
  {
    mTransform1 = new vl::Transform;
    mTransform2 = new vl::Transform;
    mTransform3 = new vl::Transform;
    mTransform4 = new vl::Transform;
    mTransform5 = new vl::Transform;
  }

  void initEvent()
  {
    if (!(GLEW_ARB_shading_language_100||GLEW_VERSION_3_0))
    {
      vl::Log::error("OpenGL Shading Language not supported.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    BaseDemo::initEvent();

    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mTransform1.get());
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mTransform2.get());
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mTransform3.get());
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mTransform4.get());
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mTransform5.get());

    vl::ref<vl::ResourceDatabase> res_db = vl::loadResource("/models/3ds/monkey.3ds");
    vl::ref<vl::Geometry> model = res_db->get<vl::Geometry>(0);

    vl::ref<vl::Light> light = new vl::Light(0);

    vl::ref<vl::Effect> effect1 = new vl::Effect;
    effect1->shader()->setRenderState( light.get() );
    effect1->shader()->enable(vl::EN_LIGHTING);
    effect1->shader()->enable(vl::EN_DEPTH_TEST);
    effect1->shader()->enable(vl::EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect1.get(), mTransform1.get() );

    vl::ref<vl::Effect> effect2 = new vl::Effect;
    effect2->shader()->setRenderState( light.get() );
    effect2->shader()->enable(vl::EN_LIGHTING);
    effect2->shader()->enable(vl::EN_DEPTH_TEST);
    effect2->shader()->enable(vl::EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect2.get(), mTransform2.get() );

    vl::ref<vl::Effect> effect3 = new vl::Effect;
    effect3->shader()->setRenderState( light.get() );
    effect3->shader()->enable(vl::EN_LIGHTING);
    effect3->shader()->enable(vl::EN_DEPTH_TEST);
    effect3->shader()->enable(vl::EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect3.get(), mTransform3.get() );

    vl::ref<vl::Effect> effect4 = new vl::Effect;
    effect4->shader()->setRenderState( light.get() );
    effect4->shader()->enable(vl::EN_LIGHTING);
    effect4->shader()->enable(vl::EN_DEPTH_TEST);
    effect4->shader()->enable(vl::EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect4.get(), mTransform4.get() );

    vl::ref<vl::Effect> effect5 = new vl::Effect;
    effect5->shader()->setRenderState( light.get() );
    effect5->shader()->enable(vl::EN_LIGHTING);
    effect5->shader()->enable(vl::EN_DEPTH_TEST);
    effect5->shader()->enable(vl::EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect5.get(), mTransform5.get() );

    if (GLEW_ARB_shading_language_100||GLEW_VERSION_3_0)
    {
      vl::ref<vl::GLSLProgram> glsl;

      glsl = effect1->shader()->gocGLSLProgram();
      glsl->attachShader( new vl::GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight_cartoon.fs") );

      glsl = effect2->shader()->gocGLSLProgram();
      glsl->attachShader( new vl::GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight.fs") );

      glsl = effect3->shader()->gocGLSLProgram();
      glsl->attachShader( new vl::GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/heat.fs") );

      glsl = effect4->shader()->gocGLSLProgram();
      glsl->attachShader( new vl::GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new vl::GLSLFragmentShader("/glsl/perpixellight_interlaced.fs") );

      if (GLEW_NV_geometry_shader4 || GLEW_ARB_geometry_shader4 || GLEW_VERSION_3_2)
      {
        glsl = effect5->shader()->gocGLSLProgram();
        // a vertex shader is always needed when using geometry shaders
        glsl->attachShader( new vl::GLSLVertexShader("/glsl/diffuse.vs") );
        glsl->attachShader( new vl::GLSLGeometryShader("/glsl/triangle_fur.gs") );
        glsl->setGeometryInputType(vl::GIT_TRIANGLES);
        glsl->setGeometryOutputType(vl::GOT_TRIANGLE_STRIP);
        glsl->setGeometryVerticesOut( 3*6 );
      }
      else
      {
        effect5->shader()->gocMaterial()->setDiffuse(vl::red);
        vl::Log::print("GL_NV_geometry_shader4 not supported.\n");
      }
    }
  }

  void run()
  {
    // animate the transforms
    mTransform1->setLocalMatrix( vl::mat4::getTranslation(-2,2,0)  * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
    mTransform2->setLocalMatrix( vl::mat4::getTranslation(+2,2,0)  * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
    mTransform3->setLocalMatrix( vl::mat4::getTranslation(-2,-2,0) * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
    mTransform4->setLocalMatrix( vl::mat4::getTranslation(+2,-2,0) * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
    mTransform5->setLocalMatrix( vl::mat4::getTranslation(0,0,0)   * vl::mat4::getRotation( vl::Time::currentTime()*45, 0, 1, 0) );
  }

  void shutdown() {}

protected:
  vl::ref<vl::Transform> mTransform1;
  vl::ref<vl::Transform> mTransform2;
  vl::ref<vl::Transform> mTransform3;
  vl::ref<vl::Transform> mTransform4;
  vl::ref<vl::Transform> mTransform5;
};

// Have fun!

BaseDemo* Create_App_GLSL() { return new App_GLSL; }
