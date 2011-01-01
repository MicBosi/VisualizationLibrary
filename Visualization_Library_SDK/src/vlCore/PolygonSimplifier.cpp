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

#include <vlCore/PolygonSimplifier.hpp>
#include <vlCore/DoubleVertexRemover.hpp>
#include <vlCore/Time.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <set>

using namespace vl;

//-----------------------------------------------------------------------------
namespace
{
  class VertexPtrWrapper
  {
  public:
    VertexPtrWrapper(PolygonSimplifier::Vertex* ptr=NULL): mVertex(ptr) {}

    bool operator==(const VertexPtrWrapper& other) const
    {
      return mVertex == other.mVertex;
    }

    bool operator!=(const VertexPtrWrapper& other) const
    {
      return mVertex == other.mVertex;
    }

    bool operator<(const VertexPtrWrapper& other) const
    {
      if ( mVertex->collapseCost() != other.mVertex->collapseCost() )
        return mVertex->collapseCost() < other.mVertex->collapseCost();
      else
        return mVertex < other.mVertex;
    }
    PolygonSimplifier::Vertex* mVertex;
  };
}
//-----------------------------------------------------------------------------
void PolygonSimplifier::simplify(float simplification_ratio, Geometry* geom)
{
  if ( simplification_ratio > 1.0f || simplification_ratio <= 0.0f )
    return;

  ArrayFloat3* pvec3 = dynamic_cast<ArrayFloat3*>( geom->vertexArray() );
  if (pvec3)
  {
    simplify( (int)(simplification_ratio*pvec3->size()), geom );
  }
  else
  {
    Log::error("PolygonSimplifier::simplify() supports only position arrays of type ArrayFloat3.\n");
    return;
  }
}
//-----------------------------------------------------------------------------
void PolygonSimplifier::simplify(int target_vertex_count, Geometry* geom)
{
  // we don't support vertex attributes of any kind yet
  #ifndef NDEBUG
    bool problem = false;
    problem |= geom->normalArray() != NULL;
    problem |= geom->colorArray() != NULL;
    problem |= geom->secondaryColorArray() != NULL;
    problem |= geom->fogCoordArray() != NULL;
    for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      problem |= geom->texCoordArray(i) != NULL;
    problem |= geom->vertexAttribArrays()->size() != 0;
    if (problem)
      Log::warning("PolygonSimplifier::simplify() simplifies only the position array of a Geometry, the other attibutes will be discarded.\n");
  #endif

  if (!dynamic_cast<ArrayFloat3*>(geom->vertexArray()))
  {
    Log::error("PolygonSimplifier::simplify() supports only position arrays of type ArrayFloat3!\n");
    return;
  }

  Time timer;
  timer.start();

  if ( removeDoubles() )
  {
    DoubleVertexRemover remover;
    remover.removeDoubles( geom );
  }

  std::vector<fvec3> verts;
  std::vector<int> indices;
  indices.reserve(1000);

  // merge all triangles in a single DrawElementsUInt
  ref<DrawElementsUInt> pint = new DrawElementsUInt(PT_TRIANGLES, 1);
  for(int i=0; i<geom->drawCalls()->size(); ++i)
  {
    DrawCall* prim = geom->drawCalls()->at(i);
    for(TriangleIterator trit = prim->triangleIterator(); !trit.isEnd(); trit.next())
    {
      indices.push_back( trit.a() );
      indices.push_back( trit.b() );
      indices.push_back( trit.c() );
    }
  }

  // fill vertices
  ref<ArrayFloat3> pvec3 = dynamic_cast<ArrayFloat3*>( geom->vertexArray() );
  verts.resize( pvec3->size() );
  memcpy( &verts[0], pvec3->ptr(), sizeof(verts[0])   * verts.size() );

  simplify(target_vertex_count, verts, indices);

  pvec3->resize( (int)verts.size() );
  pint->indices()->resize( (int)indices.size() );
  memcpy( pvec3->ptr(),               &verts[0],   sizeof(verts[0])   * verts.size() );
  memcpy( pint->indices()->ptr(), &indices[0], sizeof(indices[0]) * indices.size() );

  // install draw call

  geom->drawCalls()->clear();
  geom->drawCalls()->push_back(pint.get());

  // clear up other vertex arrays
  geom->setNormalArray(NULL);
  geom->setColorArray(NULL);
  geom->setSecondaryColorArray(NULL);
  geom->setFogCoordArray(NULL);
  for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
    geom->setTexCoordArray(i, NULL);
  geom->vertexAttribArrays()->clear();

  if (verbose())
    Log::print( Say("PolygonSimplifier::simplify() done in %.3ns\n") << timer.elapsed() );
}
//-----------------------------------------------------------------------------
void PolygonSimplifier::simplify(int target_vertex_count, std::vector<fvec3>& in_out_verts, std::vector<int>& in_out_tris)
{

  if (verbose())
    Log::print("PolygonSimplifier::simplify() starting ... \n");

  Time timer;
  timer.start();

  // preallocate vertices and triangles in one chunk
  mTriangleLump.resize(in_out_tris.size()/3);
  mVertexLump.resize(in_out_verts.size());

  int polys_before = (int)in_out_tris.size() / 3;
  int verts_before = (int)in_out_verts.size();

  mSimplifiedTriangles.resize( in_out_tris.size() / 3 );
  mSimplifiedVertices.resize( in_out_verts.size() );

#define SHUFFLE_VERTICES 0

#if SHUFFLE_VERTICES
  std::vector<Vertex*> vertex_pool;
  vertex_pool.resize( in_out_verts.size() );
  for(int i=0; i<(int)mVertexLump.size(); ++i)
    vertex_pool[i] = &mVertexLump[i];

  // shuffle the vertices
  for(int i=0; i<(int)vertex_pool.size(); ++i)
  {
    int a = int( (float)rand() / (RAND_MAX+1) * vertex_pool.size() );
    int b = int( (float)rand() / (RAND_MAX+1) * vertex_pool.size() );
    Vertex* tmp = vertex_pool[a];
    vertex_pool[a] = vertex_pool[b];
    vertex_pool[b] = tmp;
  }
#endif

  // initialize vertices
  for(int ivert=0; ivert<(int)in_out_verts.size(); ++ivert)
  {
#if SHUFFLE_VERTICES
    mSimplifiedVertices[ivert] = vertex_pool[ivert];
#else
    mSimplifiedVertices[ivert] = &mVertexLump[ivert];
#endif
    mSimplifiedVertices[ivert]->mPosition = in_out_verts[ivert];
    // so that the user knows which vertex is which and can regenerate per-vertex
    // information like textures coordinates, colors etc.
    mSimplifiedVertices[ivert]->mOriginalIndex = ivert;

    // unprotect the vertex
    mSimplifiedVertices[ivert]->mProtected = false;

    // reserve the memory for quicker allocation
    mSimplifiedVertices[ivert]->mIncidentTriangles.reserve(12);
    mSimplifiedVertices[ivert]->mAdjacentVerts.reserve(12);
  }

  // initialize triangles
  for(int idx=0, itri=0; idx<(int)in_out_tris.size(); idx+=3, ++itri)
  {
    mSimplifiedTriangles[itri] = &mTriangleLump[itri];
    mSimplifiedTriangles[itri]->mVertices[0] = mSimplifiedVertices[ in_out_tris[idx+0] ];
    mSimplifiedTriangles[itri]->mVertices[1] = mSimplifiedVertices[ in_out_tris[idx+1] ];
    mSimplifiedTriangles[itri]->mVertices[2] = mSimplifiedVertices[ in_out_tris[idx+2] ];
  }

  // compute vertex/vertex and vertex/triangle connectivity
  for(int itri=0; itri<(int)mSimplifiedTriangles.size(); ++itri)
  {
    // add this triangle to all its vertices
    mSimplifiedTriangles[itri]->mVertices[0]->mIncidentTriangles.push_back( mSimplifiedTriangles[itri] );
    mSimplifiedTriangles[itri]->mVertices[1]->mIncidentTriangles.push_back( mSimplifiedTriangles[itri] );
    mSimplifiedTriangles[itri]->mVertices[2]->mIncidentTriangles.push_back( mSimplifiedTriangles[itri] );
    // add adjacent vertices
    mSimplifiedTriangles[itri]->mVertices[0]->addAdjacentVertex( mSimplifiedTriangles[itri]->mVertices[1] ); // vertex 0
    mSimplifiedTriangles[itri]->mVertices[0]->addAdjacentVertex( mSimplifiedTriangles[itri]->mVertices[2] );
    mSimplifiedTriangles[itri]->mVertices[1]->addAdjacentVertex( mSimplifiedTriangles[itri]->mVertices[0] ); // vertex 1
    mSimplifiedTriangles[itri]->mVertices[1]->addAdjacentVertex( mSimplifiedTriangles[itri]->mVertices[2] );
    mSimplifiedTriangles[itri]->mVertices[2]->addAdjacentVertex( mSimplifiedTriangles[itri]->mVertices[0] ); // vertex 2
    mSimplifiedTriangles[itri]->mVertices[2]->addAdjacentVertex( mSimplifiedTriangles[itri]->mVertices[1] );
    // compute normal
    mSimplifiedTriangles[itri]->computeNormal();
    // error
    QErr qerr = mSimplifiedTriangles[itri]->computeQErr();
    mSimplifiedTriangles[itri]->mVertices[0]->addQErr(qerr);
    mSimplifiedTriangles[itri]->mVertices[1]->addQErr(qerr);
    mSimplifiedTriangles[itri]->mVertices[2]->addQErr(qerr);
  }

  // - remove vertices without triangles
  // - compute edge penalties
  // - initialize the collapse info of each vertex
  for( int ivert=(int)mSimplifiedVertices.size(); ivert--; )
  {
    if ( mSimplifiedVertices[ivert]->incidentTrianglesCount() == 0 )
      mSimplifiedVertices.erase( mSimplifiedVertices.begin() + ivert );
    else
    {
      mSimplifiedVertices[ivert]->computeEdgePenalty();
      computeCollapseInfo( mSimplifiedVertices[ivert] );
    }
  }

  // sets the protected vertices
  for(int i=0; i<(int)mProtectedVerts.size(); ++i)
  {
    VL_CHECK(mProtectedVerts[i] < (int)mSimplifiedVertices.size() )
    mSimplifiedVertices[ mProtectedVerts[i] ]->mProtected = true;
  }

  if (verbose())
    Log::print(Say("database setup = %.3n\n") << timer.elapsed() );

  std::set<VertexPtrWrapper> vertex_set;
  for(int ivert=0; ivert<(int)mSimplifiedVertices.size(); ++ivert)
    if ( !mSimplifiedVertices[ivert]->mProtected )
      vertex_set.insert( mSimplifiedVertices[ivert] );

  if (verbose())
    Log::print(Say("heap setup = %.3n\n") << timer.elapsed() );

  if (target_vertex_count < 3)
  {
    Log::print(Say("Invalid target_vertex_count = %n\n") << target_vertex_count);
    return;
  }

  std::vector< PolygonSimplifier::Vertex* > adj_verts;
  for( int remove_order=0; (int)vertex_set.size()>target_vertex_count; ++remove_order )
  {
    std::set<VertexPtrWrapper>::iterator it = vertex_set.begin();
    PolygonSimplifier::Vertex* v = it->mVertex;
    v->mRemoveOrder = remove_order;
    vertex_set.erase(it);

    // remove the adjacent vertices to v and v->collapseVert()
    adj_verts.clear();
    for(int i=0; i<v->adjacentVerticesCount(); ++i)
    {
      VL_CHECK( v != v->adjacentVertex(i) )
      VL_CHECK( !v->adjacentVertex(i)->mAlreadyProcessed )

      adj_verts.push_back( v->adjacentVertex(i) );
      adj_verts.back()->mAlreadyProcessed = true;
      vertex_set.erase( v->adjacentVertex(i) );
    }
    for(int i=0; i<v->collapseVertex()->adjacentVerticesCount(); ++i)
    {
      if ( !v->collapseVertex()->adjacentVertex(i)->mAlreadyProcessed )
      {
        adj_verts.push_back( v->collapseVertex()->adjacentVertex(i) );
        vertex_set.erase( v->collapseVertex()->adjacentVertex(i) );
      }
    }

    VL_CHECK(!v->removed())
    VL_CHECK(v->collapseVertex())
    VL_CHECK(!v->collapseVertex()->removed())

    collapse( v );

    // reinsert the adj_verts if not removed
    // NOTE: v->collapseVertex() might have been also removed
    for( int i=(int)adj_verts.size(); i--; )
    {
      adj_verts[i]->mAlreadyProcessed = false;

      if ( adj_verts[i]->removed() )
        continue;

      computeCollapseInfo( adj_verts[i] );

      VL_CHECK( adj_verts[i]->checkTriangles() )
      VL_CHECK( adj_verts[i]->collapseVertex() != v )
      VL_CHECK( !adj_verts[i]->collapseVertex()->removed() )

      vertex_set.insert( adj_verts[i] );
    }
  }

  if (verbose())
    Log::print(Say("simplification = %.3n\n") << timer.elapsed() );

  // generate indices for index buffer and regenerate vertex buffer
  in_out_verts.clear();
  int vert_index = 0;
  for(int i=0; i<(int)mSimplifiedVertices.size(); ++i)
  {
    if (!mSimplifiedVertices[i]->mRemoved)
    {
      in_out_verts.push_back( mSimplifiedVertices[i]->mPosition );
      mSimplifiedVertices[i]->mSimplifiedIndex = vert_index++;
    }
  }

  // regenerate index buffer
  in_out_tris.clear();
  for(int i=0; i<(int)mSimplifiedTriangles.size(); ++i)
  {
    if(!mSimplifiedTriangles[i]->mRemoved)
    {
      VL_CHECK( !mSimplifiedTriangles[i]->mVertices[0]->mRemoved )
      VL_CHECK( !mSimplifiedTriangles[i]->mVertices[1]->mRemoved )
      VL_CHECK( !mSimplifiedTriangles[i]->mVertices[2]->mRemoved )

      in_out_tris.push_back( mSimplifiedTriangles[i]->mVertices[0]->mSimplifiedIndex );
      in_out_tris.push_back( mSimplifiedTriangles[i]->mVertices[1]->mSimplifiedIndex );
      in_out_tris.push_back( mSimplifiedTriangles[i]->mVertices[2]->mSimplifiedIndex );
    }
  }

  if (verbose())
  {
    float elapsed = (float)timer.elapsed();
    int polys_after = (int)in_out_tris.size() / 3;
    int verts_after = (int)in_out_verts.size();
    Log::print(Say("POLYS: %n -> %n, %.2n%%, %.1nT/s\n") << polys_before << polys_after << 100.0f*verts_after/verts_before << (polys_before - polys_after)/elapsed );
    Log::print(Say("VERTS: %n -> %n, %.2n%%, %.1nV/s\n") << verts_before << verts_after << 100.0f*verts_after/verts_before << (verts_before - verts_after)/elapsed );
  }
}
//-----------------------------------------------------------------------------
void PolygonSimplifier::clearTrianglesAndVertices()
{
  mSimplifiedVertices.clear();
  mSimplifiedTriangles.clear();
  mProtectedVerts.clear();
  mTriangleLump.clear();
  mVertexLump.clear();
}
//-----------------------------------------------------------------------------
