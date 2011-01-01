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

// mic fixme: implement and cleaup this demo test

#ifndef App_DrawCalls_INCLUDE_ONCE
#define App_DrawCalls_INCLUDE_ONCE

#include "BaseDemo.hpp"
#include "vlut/GeometryPrimitives.hpp"
#include <vlCore/MultiDrawElements.hpp>
#include "vlCore/DrawRangeElements.hpp"
#include "vlCore/SceneManagerActorTree.hpp"
#include "vlCore/Actor.hpp"
#include "vlCore/Effect.hpp"
#include "vlCore/Time.hpp"
#include "vlCore/Light.hpp"
#include "vlCore/RenderingTree.hpp"

class App_DrawCalls: public BaseDemo
{
  void initEvent()
  {
    BaseDemo::initEvent();

    vl::ref<vl::DrawRangeElementsUInt> deu32 = new vl::DrawRangeElementsUInt;
    deu32->setBaseVertex(100);
    deu32->setPrimitiveRestartEnabled(true);
    deu32->setPrimitiveRestartIndex(0xFF);

    {
      int idx[] = { 10,11,12,13,  20,21,22,23,  0xFF,  30,31,32,33,  0xFF, 40,41,42,  -1  };
      int count = 0;
      for(int i=0; idx[i] != -1; ++i)
        ++count;
      deu32->indices()->resize( count );
      for(int i=0; idx[i] != -1; ++i)
        deu32->indices()->at(i) = idx[i];
    }
    
    //std::vector<GLint> base_vertices;
    //  base_vertices.push_back(100);
    //  base_vertices.push_back(200);
    //  base_vertices.push_back(300);
    //std::vector<GLsizei> vert_counts;
    //  vert_counts.push_back(7);
    //  vert_counts.push_back(5);
    //  vert_counts.push_back(3);

    //vl::ref< vl::IndexIteratorElements<vl::ArrayUInt1> > iie = new vl::IndexIteratorElements<vl::ArrayUInt1>;
    //// iie->initialize( deu32->indices(), NULL, NULL, deu32->baseVertex(), deu32->primitiveRestartEnabled(), deu32->primitiveRestartIndex() );
    //iie->initialize( deu32->indices(), &base_vertices, &vert_counts, 0, deu32->primitiveRestartEnabled(), deu32->primitiveRestartIndex() );

    ////vl::ref< vl::IndexIteratorDrawArrays > iida = new vl::IndexIteratorDrawArrays;
    ////iida->initialize(10,20);

    vl::IndexIterator iit = deu32->indexIterator();

    for( ; !iit.isEnd(); iit.next() ) 
      printf("%d\n", iit.index() );

    printf("-----\n");

    vl::ref<vl::DrawCall> da = new vl::DrawArrays(vl::PT_LINE_LOOP, 10, 20);

    iit = da->indexIterator();

    for( ; !iit.isEnd(); iit.next() ) 
      printf("%d\n", iit.index() );

    exit(0);

    for( vl::TriangleIterator trit = da->triangleIterator(); !trit.isEnd(); trit.next() ) 
      printf("%d %d %d\n", trit.a(), trit.b(), trit.c());

    vl::ref<vl::MultiDrawElementsUInt> mdeu32 = new vl::MultiDrawElementsUInt(vl::PT_LINE_LOOP);

    {
      // int idx[] = { 10,11,12,13, 0xFF, 20,21,22, 30,31,32,33,34, 0xFF, 40,41,42,43, -1 };
      int idx[] = { 10,11,12,13,  20,21,22,23, 0xFF, 30,31,32,33, -1 };
      int count = 0;
      for(int i=0; idx[i] != -1; ++i)
        ++count;
      mdeu32->indices()->resize( count );
      for(int i=0; idx[i] != -1; ++i)
        mdeu32->indices()->at(i) = idx[i];
    }

    std::vector<GLint> counts;
    counts.push_back(4);
    counts.push_back(9);
    mdeu32->setCountVector( counts );

    std::vector<GLint> basev;
    basev.push_back(100);
    basev.push_back(200);
    mdeu32->setBaseVertices(basev);

    mdeu32->setPrimitiveRestartEnabled(true);
    mdeu32->setPrimitiveRestartIndex(0xFF);

    iit = mdeu32->indexIterator();
    for( ; !iit.isEnd(); iit.next() ) 
      printf("%d\n", iit.index() );

    //for( vl::TriangleIterator trit = mdeu32->triangleIterator(); !trit.isEnd(); trit.next() ) 
    //  printf("%d %d %d\n", trit.a(), trit.b(), trit.c());

    printf("---\n");
    exit(0);

    // fill the index buffer

    vl::ref<vl::DrawRangeElementsUInt> de_u32 = new vl::DrawRangeElementsUInt( vl::PT_POLYGON );

    // int idx[] = { 0,1,2,3,4,5, -1 };
    int idx[] = { 10,11,12,13, 0xFF, 20,21,22,23,24, 0xFF, 30,31,32,33,34, -1 };
    int count = 0;
    for(int i=0; idx[i] != -1; ++i)
      ++count;
    de_u32->indices()->resize( count );
    for(int i=0; idx[i] != -1; ++i)
      de_u32->indices()->at(i) = idx[i];

    // initialize the triangle iterator

    vl::TriangleIteratorIndexed<vl::ArrayUInt1> it(de_u32->indices(), de_u32->primitiveType(), 10, true, 0xFF );

    // query the triangle iterator

    for( it.initialize(); it.next(); ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    printf("---\n");

    for( it.initialize(0,4); it.next(); ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    for( it.initialize(5,10); it.next(); ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    for( it.initialize(11,16); it.next(); ) 
      printf("%d %d %d\n", it.a(), it.b(), it.c());

    printf("---\n");

    de_u32->setBaseVertex(100);
    de_u32->setPrimitiveRestartEnabled(true);
    de_u32->setPrimitiveRestartIndex(0xFF);
    for( vl::TriangleIterator trit = de_u32->triangleIterator(); !trit.isEnd(); trit.next() ) 
      printf("%d %d %d\n", trit.a(), trit.b(), trit.c());

    printf("---\n");

    exit(0);
  }

  virtual void run() {}

};

// Have fun!

BaseDemo* Create_App_DrawCalls() { return new App_DrawCalls; }

#endif
