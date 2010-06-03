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

#include "vl/vlMD2.hpp"

class App_MorphAnimation: public BaseDemo
{
  int mCount;
public:
  App_MorphAnimation(int count): mCount(count) {}

  virtual void shutdown() {}

  virtual void run()
  {
  }

  virtual void initEvent()
  {
    if (!vl::VisualizationLibrary::loadWriterManager()->canLoad("md2"))
    {
      vl::Log::error("App_MorphAnimation requires IO_MODULE_MD2.\n");
      vl::Time::sleep(3000);
      exit(1);
    }

    BaseDemo::initEvent();

    ghostCamera()->setMovementSpeed(500.0f);
    bool glsl_vertex_blend = GLEW_ARB_shading_language_100  ? false:false; // anti warning
    const float area_unit  = 1500.0f*1500.0f/2000.0f;
    const float size = sqrt( mCount * area_unit );

    // common effect settings

    vl::ref<vl::Light> light = new vl::Light(0);
    vl::ref<vl::Effect> ground_fx = new vl::Effect;
    ground_fx->shader()->setRenderState( light.get() );
    ground_fx->shader()->enable(vl::EN_LIGHTING);
    ground_fx->shader()->enable(vl::EN_DEPTH_TEST);

    // ground

    vl::ref<vl::Geometry> ground;
    ground = vlut::makeGrid( vl::vec3(0,-30,0), size*1.1f, size*1.1f, 20, 20 );
    ground->computeNormals();
    sceneManager()->tree()->addActor(ground.get(), ground_fx.get() );

    vl::ref<vl::ResourceDatabase> res_db = vl::loadResource("/3rdparty/pknight/tris.md2");

    vl::ref<vl::MorphingActor> morph_actor = new vl::MorphingActor;
    morph_actor->init(res_db.get());

    vl::ref<vl::Effect> effect[4] = { new vl::Effect, new vl::Effect, new vl::Effect, new vl::Effect };
    const char* texname[] = { "/3rdparty/pknight/evil.tif", "/3rdparty/pknight/knight.tif", "/3rdparty/pknight/ctf_r.tif", "/3rdparty/pknight/ctf_b.tif" };
    for(int i=0; i<4; ++i)
    {
      effect[i]->shader()->setRenderState( light.get() );
      effect[i]->shader()->enable(vl::EN_LIGHTING);
      effect[i]->shader()->enable(vl::EN_DEPTH_TEST);
      vl::ref<vl::Texture> texture = new vl::Texture;
      texture->setupTexture2D(texname[i]);
      effect[i]->shader()->gocTextureUnit(0)->setTexture( texture.get() );

      if (glsl_vertex_blend)
      {
        vl::ref<vl::GLSLProgram> glsl = effect[i]->shader()->gocGLSLProgram();
        effect[i]->shader()->gocGLSLProgram()->attachShader(new vl::GLSLVertexShader("glsl/vertex_blend.vs"));
      }
    }

    /* multi instancing */
    for(int i=0; i<mCount; i++)
    {
      float x = rand() % RAND_MAX / (float)RAND_MAX - 0.5f;
      float z = rand() % RAND_MAX / (float)RAND_MAX - 0.5f;
      x *= size;
      z *= size;
      vl::ref<vl::MorphingActor> instance = new vl::MorphingActor;
      instance->setEffect( effect[i%4].get() );
      sceneManager()->tree()->addActor( instance.get() );
      instance->initFrom(morph_actor.get());
      instance->setTransform( new vl::Transform );
      instance->transform()->setLocalMatrix( vl::mat4::translation(x,-instance->lod(0)->boundingBox().minCorner().y(),z) );
      instance->transform()->computeWorldMatrix(NULL);

      switch(i % 5)
      {
        case 0: instance->setAnimation(0,   39,  1.5f); break; // stand
        case 1: instance->setAnimation(40,  45,  1.5f); break; // run
        case 2: instance->setAnimation(46,  53,  1.5f); break; // attack
        case 3: instance->setAnimation(112, 122, 1.5f); break; // wave
        case 4: instance->setAnimation(190, 197, 1.5f); break; // die
      }

      instance->startAnimation();
      instance->setGLSLVertexBlendEnabled(glsl_vertex_blend);
    }
  }
};

// Have fun!
