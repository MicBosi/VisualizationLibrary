/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

// mic fixme
#include <vlCore/VLX_Tools.hpp>

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/DistanceLODEvaluator.hpp>
#include <vlGraphics/BezierSurface.hpp>
#include <vlGraphics/plugins/vlVLX.hpp>
#include <vlCore/VLX_Tools.hpp>
#include <vlCore/ZippedFile.hpp>

using namespace vl;

bool checkFloatness(const double* in, size_t count)
{
  for(size_t i=0; i<count; ++i)
  {
    float f = (float)in[i];
    if ((double)f != in[i])
      return false;
  }

  return true;
}

class App_VLX: public BaseDemo
{
public:
  virtual void initEvent()
  {
    Log::notify(appletInfo());

#if 0
    std::vector<unsigned char> compressed;
    std::vector<double> double_in;
    std::vector<double> double_out;
    for(size_t i=0; i<1000; ++i)
    {
      double_in.push_back( ((rand() - RAND_MAX / 2) / double(rand() - RAND_MAX / 2)) );
      // double_in.push_back( i );
      if (i > 0)
      {
        long long* prev = (long long*)&double_in[i-1];
        long long* cur  = (long long*)&double_in[i];
        long long xor = (*prev) ^ (*cur);
        // xor = 0xFFFFFFFFFFFFFFFF;
        // printf("%8.2Lf -> %8.2Lf : %016llx -> %016llx: %016llx\n", *prev, *cur, *prev, *cur, xor);
      }
    }
    
    double_out.resize(double_in.size());

    VL_CHECK(checkFloatness(&double_in[0], double_in.size()))

    vl::compress( &double_in[0], sizeof(double_in[0]) * double_in.size(), compressed, 1 );
    vl::decompress( &compressed[0], compressed.size(), &double_out[0] );

    float bytes_c = (float)compressed.size();
    float bytes_d = (float)sizeof(double) * double_out.size();
    
    printf("compression = %.2f%%\n", bytes_c / bytes_d);

    VL_CHECK( memcmp(&double_in[0], &double_out[0], sizeof(double)*double_out.size()) == 0 );
#endif

    // VL_TRAP();

    //VLX_Tokenizer tokenizer;
    //tokenizer.setInputFile( new DiskFile("D:/VL/test.vl") );
    //VLX_Token token;
    //while(tokenizer.getToken(token) && token.mType != VLX_Token::TOKEN_EOF)
    //  printf(">> %s\n", token.mString.c_str());

#if 0 // general input/output tests
    VLX_TextParser parser;
    parser.tokenizer()->setInputFile( new DiskFile("D:/VL/in.vlx") );
    if ( parser.parse() )
    {
      VLX_TextExportVisitor exporter;
      exporter.visitStructure( parser.root() );
      DiskFile file;
      file.open("D:/VL/out.vlx", OM_WriteOnly);
      file.write( exporter.text().c_str(), exporter.text().length() );
      file.close();
    }
#endif

    // ref<Geometry> geom = makeBox( vec3(0,0,0), 10, 10, 10 );
    ref<Geometry> geom = makeIcosphere( vec3(0,0,0), 10, 0 );
    // ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 4 );
    geom->computeNormals();

    // geom->setColorArray( geom->normalArray() );
    geom->setTexCoordArray( 0, geom->normalArray() );
    // geom->setSecondaryColorArray( geom->normalArray() );
    // TriangleStripGenerator::stripfy(geom.get(), 22, false, false, true);
    // geom->mergeDrawCallsWithPrimitiveRestart(PT_TRIANGLE_STRIP);
    // geom->mergeDrawCallsWithMultiDrawElements(PT_TRIANGLE_STRIP);
    // mic fixme: this does no realizes that we are using primitive restart
    // mic fixme: make this manage also MultiDrawElements
    // geom->makeGLESFriendly();
    geom->drawCalls()->push_back( geom->drawCalls()->back() );

    // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    ref<Effect> fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->gocMaterial()->setColorMaterialEnabled(true);
    fx->shader()->setRenderState( new Light, 0 );
    // fx->shader()->setRenderState( new ClipPlane(10, fvec3(1,2,3)), 3 );

    ref<Actor> act = new Actor( geom.get(), fx.get(), new Transform );
    act->transform()->translate(1, 2 ,3);
    act->transform()->rotate(90, 0, 1, 0);
    act->transform()->addChild( new Transform( mat4::getScaling(1, 2, 3) ) );
    act->transform()->computeWorldMatrixRecursive();

#if 0
    GLSLProgram* glsl = fx->shader()->gocGLSLProgram();
    glsl->attachShader( new GLSLVertexShader("/glsl/bumpmap.vs") );
    glsl->attachShader( new GLSLFragmentShader("/glsl/bumpmap.fs") );
    glsl->shader(1)->setPath("");
    glsl->shader(1)->setSource("");
    glsl->gocUniform("vibrazione")->setUniformF(164.314159265f);
    glsl->bindFragDataLocation(0, "color_buffer");
    glsl->bindFragDataLocation(1, "normal_buffer");
    glsl->addAutoAttribLocation(0, "posizione");
    glsl->addAutoAttribLocation(1, "normale");
    glsl->addAutoAttribLocation(3, "colore");
#endif

    fx->shader()->gocTextureSampler(0)->setTexture( new Texture );
    // fx->shader()->gocTextureSampler(0)->texture()->prepareTexture2D( new Image("/images/spheremap_klimt.jpg"), TF_UNKNOWN );
    fx->shader()->gocTextureSampler(0)->texture()->prepareTexture2D( "/images/spheremap_klimt.jpg", TF_UNKNOWN );
    // fx->shader()->gocTextureSampler(0)->texture()->getTexParameter()->setWrapS(TWM_REPEAT);
    // fx->shader()->gocTextureSampler(0)->setTexParameter( new TexParameter );
    // fx->shader()->gocTextureSampler(0)->getTexParameter()->setMinFilter(TPF_NEAREST);
    // fx->shader()->gocTextureSampler(0)->getTexParameter()->setMagFilter(TPF_NEAREST);

#if 0
    ref<PixelLODEvaluator> lod_eval = new PixelLODEvaluator;
    act->setLODEvaluator( lod_eval.get() );
    lod_eval->pixelRangeSet().push_back(11);
    lod_eval->pixelRangeSet().push_back(22);
    lod_eval->pixelRangeSet().push_back(33);
#endif

#if 0
    act->actorEventCallbacks()->push_back( new DepthSortCallback );
#endif

#if 0 // uniform tests
    act->gocUniform("mic_uniform1")->setUniformF(24.0f);
    act->gocUniform("mic_uniform2")->setUniformI(11);
    act->gocUniform("mic_uniform3")->setUniformD(100);

    fvec4 float4(1,2,3,4);
    ivec4 int4(5,6,7,8);
    dvec4 double4(9,0,1,2);

    act->gocUniform("mic_uniform4")->setUniform(float4);
    act->gocUniform("mic_uniform5")->setUniform(int4);
    act->gocUniform("mic_uniform6")->setUniform(double4);

    fvec4 float4_arr[] = { fvec4(1,2,3,4), fvec4(5,6,7,8), fvec4(9,0,1,2) };
    ivec4 int4_arr[] = { ivec4(1,2,3,4), ivec4(5,6,7,8), ivec4(9,0,1,2) };
    dvec4 double4_arr[] = { dvec4(1,2,3,4), dvec4(5,6,7,8), dvec4(9,0,1,2) };

    act->gocUniform("mic_uniform7")->setUniform(3, float4_arr);
    act->gocUniform("mic_uniform8")->setUniform(3, int4_arr);
    act->gocUniform("mic_uniform9")->setUniform(3, double4_arr);

    float float1_arr[]   = { 1, 2, 3, 4, 5 };
    int int1_arr[]       = { 1, 2, 3, 4, 5 };
    double double1_arr[] = { 1, 2, 3, 4, 5 };

    act->gocUniform("mic_uniform10")->setUniform(5, float1_arr);
    act->gocUniform("mic_uniform11")->setUniform(5, int1_arr);
    act->gocUniform("mic_uniform12")->setUniform(5, double1_arr);

    act->gocUniform("mat4x4")->setUniform( mat4::getTranslation(10,20,30) );
    act->gocUniform("mat3x3")->setUniform( mat3::getTranslation(40,50) );
    act->gocUniform("mat2x2")->setUniform( mat2(1,2,3,4) );

    mat4 mat4_arr[] = { mat4::getTranslation(10,20,30), mat4::getTranslation(40,50,60), mat4::getTranslation(70,80,90) };
    act->gocUniform("mat4x4_array")->setUniform( 3, mat4_arr );

    act->gocUniform("empty");
#endif

    // mic fixme: test <Rotation> <Translation> <Scaling>
    // mic fixme: matrix should be formatted
    // mic fixme: we should need a mechanism that user_made objects are exported by user_made exporters.

    ref<ResourceDatabase> res_db = new ResourceDatabase;
#if 0
  #if 1
      for (int i=0; i<100; ++i)
      {
        ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 16 );
        geom->computeNormals();
        res_db->resources().push_back( geom.get() );
      }
  #else
      res_db->resources().push_back( geom.get() );
  #endif
#elif 0
    res_db->resources().push_back( sceneManager()->tree()->actors()->at(0)->lod(0) );
    res_db->resources().push_back( sceneManager()->tree()->actors()->at(0) );
    res_db->resources().push_back( new Camera );
#endif

#if 0
    bool ok = saveVLT("D:/VL/export.vl", res_db.get());
    VL_CHECK(ok);

    sceneManager()->tree()->actors()->clear();

    res_db = loadVLT("D:/VL/export.vl");
    VL_CHECK(res_db);

    geom = res_db->get<Geometry>(0);
    VL_CHECK(geom)
#endif

    res_db->resources().push_back(act.get());

    vl::writeResource("D:/VL/export.vlb", res_db.get());
    res_db = vl::loadResource("D:/VL/export.vlb");

    vl::writeResource("D:/VL/export.vlt", res_db.get());
    res_db = vl::loadResource("D:/VL/export.vlt");

    vl::writeResource("D:/VL/export.vlb", res_db.get());
    res_db = vl::loadResource("D:/VL/export.vlb");

    vl::writeResource("D:/VL/export.vlt", res_db.get());

    // vl::saveVLB("D:/VL/export.vlb", res_db.get());
    // res_db = vl::loadVLB("D:/VL/export.vlb");
    // vl::saveVLT("D:/VL/re-export.vlt", res_db .get());

    //VLX_Serializer serializer;

    //// serialize
    //serializer.saveText( "D:/VL/export.vlx", act.get() );

    //ref<Object> obj = serializer.loadText("D:/VL/export.vlx");
    //ref<Actor> act2 = obj->as<Actor>(); VL_CHECK(act2);

    //// re-export
    //serializer.saveText( "D:/VL/re-export.vlx", act2.get() );

    // put into scene
    Actor* act2 = res_db->get<Actor>(0); VL_CHECK(act2);
    sceneManager()->tree()->addActor( act2 );

    // sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);
    // sceneManager()->tree()->addActor( act.get() );
  }
};

// Have fun!

BaseDemo* Create_App_VLX() { return new App_VLX; }
