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

#ifndef App_DrawCalls_INCLUDE_ONCE
#define App_DrawCalls_INCLUDE_ONCE

#include "vl/DrawRangeElements.hpp"
#include "BaseDemo.hpp"
#include "vlut/GeometryPrimitives.hpp"
#include "vl/SceneManagerActorTree.hpp"
#include "vl/Actor.hpp"
#include "vl/Effect.hpp"
#include "vl/Time.hpp"
#include "vl/Light.hpp"
#include "vl/RenderingTree.hpp"

namespace vl
{
  template<class TArray>
  class TriangleIteratorMulti: public TriangleIteratorAbstract
  {
  public:
    virtual bool next() = 0;
    virtual bool isEnd() const = 0;
    virtual int a() const = 0;
    virtual int b() const = 0;
    virtual int c() const = 0;
  };
}

class App_DrawCalls: public BaseDemo
{
  void initEvent()
  {
    BaseDemo::initEvent();

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

    // mic fixme:
    // for multi-draw-elements: l'iteratore dovrebbe loopare tra i sub-draw-element ed usare questo.
    vl::TriangleIteratorIndexed<vl::ArrayUInt> it(de_u32->indices(), de_u32->primitiveType(), 10, true, 0xFF );

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
    for( vl::TriangleIterator trit = de_u32->triangles(); !trit.isEnd(); trit.next() ) 
      printf("%d %d %d\n", trit.a(), trit.b(), trit.c());

    printf("---\n");

    exit(0);
  }

  virtual void run() {}

};

// Have fun!

#endif
