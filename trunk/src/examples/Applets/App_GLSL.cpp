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
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/GLSL.hpp>

using namespace vl;

class App_GLSL: public BaseDemo
{
public:
  App_GLSL()
  {
    mTransform1 = new Transform;
    mTransform2 = new Transform;
    mTransform3 = new Transform;
    mTransform4 = new Transform;
    mTransform5 = new Transform;
  }

  void initEvent()
  {
    if (!(Has_GL_ARB_shading_language_100||Has_GL_Version_3_0))
    {
      Log::error("OpenGL Shading Language not supported.\n");
      Time::sleep(3000);
      exit(1);
    }

    Log::print(appletInfo());

    rendering()->as<Rendering>()->transform()->addChild(mTransform1.get());
    rendering()->as<Rendering>()->transform()->addChild(mTransform2.get());
    rendering()->as<Rendering>()->transform()->addChild(mTransform3.get());
    rendering()->as<Rendering>()->transform()->addChild(mTransform4.get());
    rendering()->as<Rendering>()->transform()->addChild(mTransform5.get());

    ref<ResourceDatabase> res_db = loadResource("/models/3ds/monkey.3ds");
    ref<Geometry> model = res_db->get<Geometry>(0);
    model->computeNormals();

    ref<Light> light = new Light(0);

    ref<Effect> effect1 = new Effect;
    effect1->shader()->setRenderState( light.get() );
    effect1->shader()->enable(EN_LIGHTING);
    effect1->shader()->enable(EN_DEPTH_TEST);
    effect1->shader()->enable(EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect1.get(), mTransform1.get() );

    ref<Effect> effect2 = new Effect;
    effect2->shader()->setRenderState( light.get() );
    effect2->shader()->enable(EN_LIGHTING);
    effect2->shader()->enable(EN_DEPTH_TEST);
    effect2->shader()->enable(EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect2.get(), mTransform2.get() );

    ref<Effect> effect3 = new Effect;
    effect3->shader()->setRenderState( light.get() );
    effect3->shader()->enable(EN_LIGHTING);
    effect3->shader()->enable(EN_DEPTH_TEST);
    effect3->shader()->enable(EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect3.get(), mTransform3.get() );

    ref<Effect> effect4 = new Effect;
    effect4->shader()->setRenderState( light.get() );
    effect4->shader()->enable(EN_LIGHTING);
    effect4->shader()->enable(EN_DEPTH_TEST);
    effect4->shader()->enable(EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect4.get(), mTransform4.get() );

    ref<Effect> effect5 = new Effect;
    effect5->shader()->setRenderState( light.get() );
    effect5->shader()->enable(EN_LIGHTING);
    effect5->shader()->enable(EN_DEPTH_TEST);
    effect5->shader()->enable(EN_CULL_FACE);
    sceneManager()->tree()->addActor( model.get(), effect5.get(), mTransform5.get() );

    if (Has_GL_ARB_shading_language_100||Has_GL_Version_3_0)
    {
      ref<GLSLProgram> glsl;

      glsl = effect1->shader()->gocGLSLProgram();
      glsl->attachShader( new GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new GLSLFragmentShader("/glsl/perpixellight_cartoon.fs") );

      glsl = effect2->shader()->gocGLSLProgram();
      glsl->attachShader( new GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new GLSLFragmentShader("/glsl/perpixellight.fs") );

      glsl = effect3->shader()->gocGLSLProgram();
      glsl->attachShader( new GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new GLSLFragmentShader("/glsl/heat.fs") );

      glsl = effect4->shader()->gocGLSLProgram();
      glsl->attachShader( new GLSLVertexShader("/glsl/perpixellight.vs") );
      glsl->attachShader( new GLSLFragmentShader("/glsl/perpixellight_interlaced.fs") );

      if (Has_Geometry_Shader)
      {
        glsl = effect5->shader()->gocGLSLProgram();
        // a vertex shader is always needed when using geometry shaders
        glsl->attachShader( new GLSLVertexShader("/glsl/diffuse.vs") );
        glsl->attachShader( new GLSLGeometryShader("/glsl/triangle_fur.gs") );
        glsl->setGeometryInputType(GIT_TRIANGLES);
        glsl->setGeometryOutputType(GOT_TRIANGLE_STRIP);
        glsl->setGeometryVerticesOut( 3*6 );
      }
      else
      {
        effect5->shader()->gocMaterial()->setDiffuse(red);
        Log::print("GL_NV_geometry_shader4 not supported.\n");
      }
    }
  }

  void updateScene()
  {
    // animate the transforms
    mTransform1->setLocalMatrix( mat4::getTranslation(-2,2,0)  * mat4::getRotation( Time::currentTime()*45, 0, 1, 0) );
    mTransform2->setLocalMatrix( mat4::getTranslation(+2,2,0)  * mat4::getRotation( Time::currentTime()*45, 0, 1, 0) );
    mTransform3->setLocalMatrix( mat4::getTranslation(-2,-2,0) * mat4::getRotation( Time::currentTime()*45, 0, 1, 0) );
    mTransform4->setLocalMatrix( mat4::getTranslation(+2,-2,0) * mat4::getRotation( Time::currentTime()*45, 0, 1, 0) );
    mTransform5->setLocalMatrix( mat4::getTranslation(0,0,0)   * mat4::getRotation( Time::currentTime()*45, 0, 1, 0) );
  }

protected:
  ref<Transform> mTransform1;
  ref<Transform> mTransform2;
  ref<Transform> mTransform3;
  ref<Transform> mTransform4;
  ref<Transform> mTransform5;
};

// Have fun!

BaseDemo* Create_App_GLSL() { return new App_GLSL; }
