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

// mic fixme: implement and cleaup this demo test

#ifndef App_DrawCalls_INCLUDE_ONCE
#define App_DrawCalls_INCLUDE_ONCE

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <vlGraphics/DrawRangeElements.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Effect.hpp>
#include <vlCore/Time.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/RenderingTree.hpp>

using namespace vl;

class App_DrawCalls: public BaseDemo
{
  void setupScene_Normal()
  {
    ref<Geometry> torus1 = vl::makeTorus( vec3(-10,0,0), 8.0f, 1.0f, 20, 20 );
    ref<Geometry> torus2 = vl::makeTorus( vec3(  0,0,0), 8.0f, 2.0f, 10, 10 );
    ref<Geometry> torus3 = vl::makeTorus( vec3(+10,0,0), 8.0f, 3.0f, 7, 7 );

    ref<Effect> fx = new Effect;
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->setRenderState( new Light(0) );

    sceneManager()->tree()->actors()->clear();
    sceneManager()->tree()->addActor( torus1.get(), fx.get(), NULL );
    sceneManager()->tree()->addActor( torus2.get(), fx.get(), NULL );
    sceneManager()->tree()->addActor( torus3.get(), fx.get(), NULL );
  }

  void setupScene_MultiDrawElements()
  {
    ref<Geometry> torus1 = vl::makeTorus( vec3(-10, -9, 0), 8.0f, 1.0f, 20, 20 );
    ref<Geometry> torus2 = vl::makeTorus( vec3(  0, -9, 0), 8.0f, 2.0f, 10, 10 );
    ref<Geometry> torus3 = vl::makeTorus( vec3(+10, -9, 0), 8.0f, 3.0f, 7, 7 );

    // merge vertices
    ref<ArrayFloat3> vert = new ArrayFloat3;
    ref<ArrayFloat3> torus1_vert = vl::cast<ArrayFloat3>(torus1->vertexArray());
    ref<ArrayFloat3> torus2_vert = vl::cast<ArrayFloat3>(torus2->vertexArray());
    ref<ArrayFloat3> torus3_vert = vl::cast<ArrayFloat3>(torus3->vertexArray());

    vert->resize( torus1_vert->size() + torus2_vert->size() + torus3_vert->size() );
    memcpy( vert->ptr(), torus1_vert->ptr(), torus1_vert->bytesUsed() );
    memcpy( vert->ptr() + torus1_vert->bytesUsed(), torus2_vert->ptr(), torus2_vert->bytesUsed() );
    memcpy( vert->ptr() + torus1_vert->bytesUsed() + torus2_vert->bytesUsed(), torus3_vert->ptr(), torus3_vert->bytesUsed() );

    // merge indices
    ref<MultiDrawElementsUInt> mde = new MultiDrawElementsUInt(PT_QUADS);
    ref<DrawElementsUInt> torus1_de = vl::cast<DrawElementsUInt>(torus1->drawCalls()->at(0));
    ref<DrawElementsUInt> torus2_de = vl::cast<DrawElementsUInt>(torus2->drawCalls()->at(0));
    ref<DrawElementsUInt> torus3_de = vl::cast<DrawElementsUInt>(torus3->drawCalls()->at(0));

    mde->indices()->resize( torus1_de->indices()->size() + torus2_de->indices()->size() + torus3_de->indices()->size() );
    memset(mde->indices()->ptr(), 0, mde->indices()->bytesUsed()); // mic fixme
    MultiDrawElementsUInt::index_type* p_idx = mde->indices()->begin();
    for( size_t i=0; i< torus1_de->indices()->size(); ++i, ++p_idx )
    {
      *p_idx = torus1_de->indices()->at(i);
      VL_CHECK(*p_idx < 0xFFFF)
    }
    
    for( size_t i=0; i< torus2_de->indices()->size(); ++i, ++p_idx )
    {
      *p_idx = torus2_de->indices()->at(i) + torus1_vert->size();
      VL_CHECK(*p_idx < 0xFFFF)
    }

    for( size_t i=0; i< torus3_de->indices()->size(); ++i, ++p_idx )
    {
      *p_idx = torus3_de->indices()->at(i) + torus1_vert->size() + torus2_vert->size();
      VL_CHECK(*p_idx < 0xFFFF)
    }

    // define how many indices for each draw call
    GLsizei count_vector[] = { torus1_de->indices()->size(), torus2_de->indices()->size(), torus3_de->indices()->size() };
    mde->setCountVector( count_vector, 3 );

    ref<Geometry> geom = new Geometry;
    geom->setVertexArray( vert.get() );
    geom->drawCalls()->push_back( mde.get() );

    // compute normals must be done after the draw calls have been added.
    geom->computeNormals();

    ref<Effect> fx = new Effect;
    fx->shader()->setRenderState( new Light(0) );
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->gocMaterial()->setDiffuse( vl::pink );

    sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL );
  }

  void setupScene_BaseVertex()
  {
    // creat three meshes with the same topology but different shapes
    ref<Geometry> torus1 = vl::makeTorus( vec3(-10, 9, 0), 6.0f, 1.0f, 15, 15 );
    ref<Geometry> torus2 = vl::makeTorus( vec3(  0, 9, 0), 7.0f, 2.0f, 15, 15 );
    ref<Geometry> torus3 = vl::makeTorus( vec3(+10, 9, 0), 8.0f, 3.0f, 15, 15 );

    // merge vertices
    ref<ArrayFloat3> vert = new ArrayFloat3;
    ref<ArrayFloat3> torus1_vert = vl::cast<ArrayFloat3>(torus1->vertexArray());
    ref<ArrayFloat3> torus2_vert = vl::cast<ArrayFloat3>(torus2->vertexArray());
    ref<ArrayFloat3> torus3_vert = vl::cast<ArrayFloat3>(torus3->vertexArray());

    vert->resize( torus1_vert->size() + torus2_vert->size() + torus3_vert->size() );
    memcpy( vert->ptr(), torus1_vert->ptr(), torus1_vert->bytesUsed() );
    memcpy( vert->ptr() + torus1_vert->bytesUsed(), torus2_vert->ptr(), torus2_vert->bytesUsed() );
    memcpy( vert->ptr() + torus1_vert->bytesUsed() + torus2_vert->bytesUsed(), torus3_vert->ptr(), torus3_vert->bytesUsed() );

    ref<Geometry> geom = new Geometry;
    geom->setVertexArray( vert.get() );

    // create three DrawElementsUInt sharing the same index buffer and using base-vertex functionality.
    ref<DrawElementsUInt> torus1_de = vl::cast<DrawElementsUInt>(torus1->drawCalls()->at(0));

    // (1)
    ref<DrawElementsUInt> de1 = new DrawElementsUInt(PT_QUADS);
    de1->setIndices( torus1_de->indices() );
    de1->setBaseVertex(0);
    geom->drawCalls()->push_back( de1.get() );
    // (2)
    ref<DrawElementsUInt> de2 = new DrawElementsUInt(PT_QUADS);
    de2->setIndices( torus1_de->indices() );
    de2->setBaseVertex( torus1_vert->size() ); // skip the vertices of the first torus
    geom->drawCalls()->push_back( de2.get() );
    // (3)
    ref<DrawElementsUInt> de3 = new DrawElementsUInt(PT_QUADS);
    de3->setIndices( torus1_de->indices() );
    de3->setBaseVertex( torus1_vert->size() + torus2_vert->size() ); // skip the vertices of the first and second torus
    geom->drawCalls()->push_back( de3.get() );

    // compute normals must be done after the draw calls have been added.
    geom->computeNormals();

    ref<Effect> fx = new Effect;
    fx->shader()->setRenderState( new Light(0) );
    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->gocMaterial()->setDiffuse( vl::yellow );

    sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL );
  }

  void initEvent()
  {
    vl::Log::info(appletInfo());

    setupScene_BaseVertex();
    setupScene_MultiDrawElements();
  }

};

// Have fun!

BaseDemo* Create_App_DrawCalls() { return new App_DrawCalls; }

#endif
