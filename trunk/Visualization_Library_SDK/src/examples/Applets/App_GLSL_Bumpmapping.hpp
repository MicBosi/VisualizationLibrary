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

#include "vl/VisualizationLibrary.hpp"
#include "vl/Light.hpp"
#include "vl/GLSL.hpp"

class App_GLSL_Bumpmapping: public BaseDemo
{
public:
  App_GLSL_Bumpmapping()
  {
  }

  void initEvent()
  {
    if (!GLEW_ARB_shading_language_100)
    {
      vl::Log::error("GL_ARB_shading_language_100 not supported.\n");
      vl::Time::sleep(3000);
      openglContext()->quitApplication();
      return;
    }

    BaseDemo::initEvent();

    // generate torus, with normals and uv coords
    vl::ref<vl::Geometry> model = vlut::makeTorus( vl::vec3(0,0,0), 10, 2, 25, 25, 2.0f );

    vl::ref<vl::Light> light = new vl::Light(0);

    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->setRenderState( light.get() );
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->enable(vl::EN_CULL_FACE);
    // effect->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
    torus = sceneManager()->tree()->addActor( model.get(), effect.get(), NULL );

    vl::ref<vl::Texture> texture0 = new vl::Texture;
    texture0->setupTexture2D("images/normalmap.jpg");
    effect->shader()->gocTextureUnit(0)->setTexture(texture0.get());
    texture0->getTexParameter()->setAnisotropy(16.0);
    texture0->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    texture0->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);

    glsl = effect->shader()->gocGLSLProgram();
    glsl->attachShader( new vl::GLSLVertexShader("/glsl/bumpmap.vs") );
    glsl->attachShader( new vl::GLSLFragmentShader("/glsl/bumpmap.fs") );
    vl::ref<vl::Uniform> sampler0 = new vl::Uniform;
    sampler0->setUniform(0);
    sampler0->setName("sampler0");
    glsl->setUniform( sampler0.get() );
    light_obj_space_pos = new vl::Uniform;
    light_obj_space_pos->setName("light_obj_space_pos");
    glsl->setUniform( light_obj_space_pos.get() );

    vl::ref<vl::ArrayFVec3> tangent = new vl::ArrayFVec3;
    // vl::ref<vl::ArrayFVec3> bitangent = new vl::ArrayFVec3;
    tangent->resize( model->vertexArray()->size() );
    // bitangent->resize( model->vertexArray()->size() );

    VL_CHECK( model->primitives()->at(0)->primitiveType() == vl::PT_QUADS );
    // tessellate torus quads
    std::vector<unsigned int> index_buffer;
    index_buffer.resize( model->primitives()->at(0)->triangleCount() * 3 );
    unsigned int* triangle = &index_buffer.front();
    for( size_t i=0; i<model->primitives()->at(0)->indexCount(); i+=4 )
    {
      size_t a = model->primitives()->at(0)->index(i+0);
      size_t b = model->primitives()->at(0)->index(i+1);
      size_t c = model->primitives()->at(0)->index(i+2);
      size_t d = model->primitives()->at(0)->index(i+3);

      VL_CHECK(a < model->vertexArray()->size());
      VL_CHECK(b < model->vertexArray()->size());
      VL_CHECK(c < model->vertexArray()->size());
      VL_CHECK(d < model->vertexArray()->size());

      triangle[0] = a;
      triangle[1] = b;
      triangle[2] = c;
      triangle += 3;

      triangle[0] = c;
      triangle[1] = d;
      triangle[2] = a;
      triangle += 3;
    }

    computeTangentSpace(
      model->vertexArray()->size(), 
      (vl::fvec3*)model->vertexArray()->ptr(), 
      (vl::fvec3*)model->normalArray()->ptr(), 
      (vl::fvec2*)model->texCoordArray(0)->ptr(),
      model->primitives()->at(0)->triangleCount(),
      &index_buffer.front(),
      tangent->begin(), 
      NULL/*bitangent->begin()*/ );

    glsl->linkProgram();
    int tangent_idx = glsl->getAttribLocation("tangent");
    // int bitangent_idx = glsl->getAttribLocation("bitangent");
    model->setVertexAttributeArray(tangent_idx, false, false, tangent.get() );
    // model->setVertexAttributeArray(bitangent_idx, false, false, bitangent.get() );

    /*
    // visualize tangent space
    vl::ref<vl::ArrayFVec3> ntb_verts = new vl::ArrayFVec3;
    ntb_verts->resize( model->vertexArray()->size() * 6 );

    vl::ref<vl::ArrayFVec4> ntb_cols = new vl::ArrayFVec4;
    ntb_cols->resize( model->vertexArray()->size() * 6 );

    vl::fvec3* verts = (vl::fvec3*)model->vertexArray()->ptr();
    vl::fvec3* norms = (vl::fvec3*)model->normalArray()->ptr();

    float tick_size = 0.5f;

    for( size_t i=0; i<model->vertexArray()->size(); ++i )
    {
      (*ntb_verts)[i*6 + 0] = verts[i];
      (*ntb_verts)[i*6 + 1] = verts[i] + norms[i] * tick_size;
      (*ntb_verts)[i*6 + 2] = verts[i];
      (*ntb_verts)[i*6 + 3] = verts[i] + (*tangent)[i] * tick_size;
      (*ntb_verts)[i*6 + 4] = verts[i];
      (*ntb_verts)[i*6 + 5] = verts[i] + (*bitangent)[i] * tick_size;

      (*ntb_cols)[i*6 + 0] = vlut::red;
      (*ntb_cols)[i*6 + 1] = vlut::red;
      (*ntb_cols)[i*6 + 2] = vlut::green;
      (*ntb_cols)[i*6 + 3] = vlut::green;
      (*ntb_cols)[i*6 + 4] = vlut::blue;
      (*ntb_cols)[i*6 + 5] = vlut::blue;
    }

    vl::ref<vl::Geometry> NTBGeom = new vl::Geometry;
    NTBGeom->setVertexArray( ntb_verts.get() );
    NTBGeom->setColorArray( ntb_cols.get() );
    NTBGeom->primitives()->push_back( new vl::DrawArrays(vl::PT_LINES, 0, ntb_verts->size() ) );
    sceneManager()->tree()->addActor( NTBGeom.get(), effect.get(), NULL );
    */

    // ... testa con oggetto che ruota per i fatti suoi ...
    // ... ripulisci implementazione: passa direttamente la matrice buona.

    // FIXME
    //MAKE UTILITY CLASSES:
    //+ Compute Tangent Space
    //+ Tangent Space Geometry Visualizer (vert, norm, tang, btang, norm_col, tang_col, btang_col, width).
    //  +--> Exposes the effect used and the vertex and color buffer and primitive lump generated.
    //+ Index Buffer Tessellator -> returns a triangle list UINT out of any primitive type.
    //  +--> Convert to triangles utility function.
    //+ Generate & Distribute your bump map.
    //- why do we need to negate the bitangent? =|> ask OpenGL.org
    //- implementa normalization cubemap
    //- GLSL shaders sono in uno stato da fare schifo, non li dovresti ridistribuire cosi
    //- 3DS support fix
    //- Primitives: texture coordinates generation OFF by default.
  }

  // Based on:
  // Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. 
  // http://www.terathon.com/code/tangent.html
  void computeTangentSpace( 
    size_t vert_count, 
    const vl::fvec3 *vertex, 
    const vl::fvec3* normal,
    const vl::fvec2 *texcoord, 
    size_t tri_count, 
    const unsigned int *triangles, 
    vl::fvec3 *tangent, 
    vl::fvec3 *bitangent )
  {
      std::vector<vl::fvec3> tan1;
      std::vector<vl::fvec3> tan2;
      tan1.resize(vert_count);
      tan2.resize(vert_count);
      
      for ( size_t a = 0; a < tri_count; ++a, triangles+=3 )
      {
          unsigned int i1 = triangles[0];
          unsigned int i2 = triangles[1];
          unsigned int i3 = triangles[2];

          VL_CHECK(i1 < vert_count );
          VL_CHECK(i2 < vert_count );
          VL_CHECK(i3 < vert_count );
          
          const vl::fvec3& v1 = vertex[i1];
          const vl::fvec3& v2 = vertex[i2];
          const vl::fvec3& v3 = vertex[i3];
          
          const vl::fvec2& w1 = texcoord[i1];
          const vl::fvec2& w2 = texcoord[i2];
          const vl::fvec2& w3 = texcoord[i3];
          
          float x1 = v2.x() - v1.x();
          float x2 = v3.x() - v1.x();
          float y1 = v2.y() - v1.y();
          float y2 = v3.y() - v1.y();
          float z1 = v2.z() - v1.z();
          float z2 = v3.z() - v1.z();
          
          float s1 = w2.x() - w1.x();
          float s2 = w3.x() - w1.x();
          float t1 = w2.y() - w1.y();
          float t2 = w3.y() - w1.y();
          
          float r = 1.0F / (s1 * t2 - s2 * t1);
          vl::fvec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
          vl::fvec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

          tan1[i1] += sdir;
          tan1[i2] += sdir;
          tan1[i3] += sdir;

          tan2[i1] += tdir;
          tan2[i2] += tdir;
          tan2[i3] += tdir;
      }

      for ( size_t a = 0; a < vert_count; a++)
      {
          const vl::fvec3& n = normal[a];
          const vl::fvec3& t = tan1[a];

          // Gram-Schmidt orthogonalize
          tangent[a] = (t - n * vl::dot(n, t)).normalize();

          if ( bitangent )
          {
            // Calculate handedness
            float w = (vl::dot(vl::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
            bitangent[a] = vl::cross( n, tangent[a] ) * w;
          }
      }
  }

  void run() 
  {
    vl::fvec3 camera_pos = vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->inverseViewMatrix().getT();
    vl::mat4 obj_mat;
    if (torus->transform()) 
      obj_mat = torus->transform()->worldMatrix();
    obj_mat.invert();
    vl::fvec3 camera_pos_obj_space = obj_mat * camera_pos;
    // light_obj_space_pos->setUniform( /*camera_pos_obj_space*/ vl::vec3(100,0,0) );
    light_obj_space_pos->setUniform( camera_pos_obj_space );
    // printf("camera_pos = %f %f %f\n", camera_pos.x(), camera_pos.y(), camera_pos.z() );
    // printf("camera_pos_obj_space = %f %f %f\n", camera_pos_obj_space.x(), camera_pos_obj_space.y(), camera_pos_obj_space.z() );
  }

  void shutdown() {}

protected:
    vl::ref<vl::GLSLProgram> glsl;
    vl::ref<vl::Uniform> light_obj_space_pos;
    vl::ref<vl::Actor> torus;

};

// Have fun!
