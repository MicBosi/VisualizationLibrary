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

#ifndef DepthSortCallback_INCLUDE_ONCE
#define DepthSortCallback_INCLUDE_ONCE

#include <vl/Actor.hpp>
#include <vl/Geometry.hpp>
#include <vl/Camera.hpp>

namespace vl
{
  /**
   * The DepthSortCallback class sorts the primitives of the Geometry object bound to the Actor in which the callback is installed.
   * 
   * This callback in order to work requires the following conditions:
   * - The Actor must be bound to a Geometry
   * - The bound Geometry must have a Geometry::vertexArray() of type ArrayFVec3
   * - Sorts only DrawElementsUInt/UShort/UByte objects with primitive type: PT_POINTS, PT_LINES, PT_TRIANGLES, PT_QUADS
   *
   * Despite the fact that the condition list seems to be quite limiting it actually covers the most common usage cases.
   * Furthermore the use of DrawElements* and the primitive types PT_POINTS, PT_LINES, PT_TRIANGLES, PT_QUADS grant 
   * the maximum flexibility.
   *
   * \note
   *
   * - This callback works well with different LODs.
   * - This callback works well with multipassing, the sorting is done only once.
   *
   * \remarks
   *
   * - The sorting is based on the position of the vertices as specified by Geometry::vertexArray() and for obvious
   * reasons cannot take into consideration transformations made in the vertex shader or in the geometry shader.
   * - The sorting is performed on a per Primitives basis. For example, if a Geometry has 2 Primitives A and B bound to it, 
   *   then the polygons, lines or points of A will alway be rendered before the ones specified by B.
   *
   * \sa \ref pagGuidePolygonDepthSorting "Transparency and Polygon Depth Sorting Tutorial"
   */
  class DepthSortCallback: public ActorRenderingCallback
  {
    template<typename T>
    class Point
    {
    public:
      T A;
    };
    template<typename T>
    class Line
    {
    public:
      T A,B;
    };
    template<typename T>
    class Triangle
    {
    public:
      T A,B,C;
    };
    template<typename T>
    class Quad
    {
    public:
      T A,B,C,D;
    };
    typedef Point<unsigned int>      PointUInt;
    typedef Line<unsigned int>       LineUInt;
    typedef Triangle<unsigned int>   TriangleUInt;
    typedef Quad<unsigned int>       QuadUInt;

    typedef Point<unsigned short>    PointUShort;
    typedef Line<unsigned short>     LineUShort;
    typedef Triangle<unsigned short> TriangleUShort;
    typedef Quad<unsigned short>     QuadUShort;

    typedef Point<unsigned char>     PointUByte;
    typedef Line<unsigned char>      LineUByte;
    typedef Triangle<unsigned char>  TriangleUByte;
    typedef Quad<unsigned char>      QuadUByte;

    class PrimitiveZ
    {
    public:
      PrimitiveZ(int tri=0, float z=0.0f): mPrimitiveIndex(tri), mZ(z) {}
      bool operator<(const PrimitiveZ& other) const { return mZ < other.mZ; }
      unsigned int mPrimitiveIndex;
      float mZ;
    };
    class Sorter_Back_To_Front
    {
    public:
      bool operator()(const PrimitiveZ& t1, const PrimitiveZ& t2) const { return t1.mZ < t2.mZ; }
    };
    class Sorter_Front_To_Back
    {
    public:
      bool operator()(const PrimitiveZ& t1, const PrimitiveZ& t2) const { return t1.mZ > t2.mZ; }
    };

  public:
    DepthSortCallback()
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mEyeSpaceVerts = new ArrayFVec3;
      setSortMode(SM_SortBackToFront);
    }
    virtual const char* className() { return "DepthSortCallback"; }
    //! Performs the actual sorting
    virtual void operator()(const Camera* cam, Actor* actor, Renderable* renderable, const Shader*, int pass)
    {
      // need to sort only for the first pass
      if (pass > 0)
        return;

      vl::mat4 matrix = cam->viewMatrix();
      if (actor && actor->transform())
        matrix *= actor->transform()->worldMatrix();

      if (matrix == mCacheMatrix)
        return;
      else
        mCacheMatrix = matrix;

      // this works well with LOD
      ref<Geometry> geometry = dynamic_cast<Geometry*>(renderable);
      if (!geometry)
        return;

      geometry->setDisplayListDirty(true);

      ref<ArrayFVec3> verts = dynamic_cast<ArrayFVec3*>(geometry->vertexArray());

      if (!verts)
        return;

      // computes eye-space vertex positions
      fmat4 m;
      if (actor->transform())
        m = (fmat4)(cam->viewMatrix() * actor->transform()->worldMatrix());
      else
        m = (fmat4)cam->viewMatrix();
      mEyeSpaceVerts->resize( verts->size() );
      // would be nice to optimize this with SEE2
      for(size_t i=0; i<verts->size(); ++i)
        (*mEyeSpaceVerts)[i] = m * (*verts)[i];

      geometry->setVBODirty(true);

      for(int idraw=0; idraw<geometry->primitives()->size(); ++idraw)
      {
        ref<DrawElementsUInt>   polys_uint   = dynamic_cast<DrawElementsUInt*>  (geometry->primitives()->at(idraw));
        ref<DrawElementsUShort> polys_ushort = dynamic_cast<DrawElementsUShort*>(geometry->primitives()->at(idraw));
        ref<DrawElementsUByte>  polys_ubyte  = dynamic_cast<DrawElementsUByte*> (geometry->primitives()->at(idraw));

        if (polys_uint)
          sort<unsigned int,DrawElementsUInt>(polys_uint.get(), mSortedPointsUInt, mSortedLinesUInt, mSortedTrianglesUInt, mSortedQuadsUInt);
        else
        if (polys_ushort)
          sort<unsigned short,DrawElementsUShort>(polys_ushort.get(), mSortedPointsUShort, mSortedLinesUShort, mSortedTrianglesUShort, mSortedQuadsUShort);
        else
        if (polys_ubyte)
          sort<unsigned char,DrawElementsUByte>(polys_ubyte.get(), mSortedPointsUByte, mSortedLinesUByte, mSortedTrianglesUByte, mSortedQuadsUByte);
      }
    }

    template<typename T, typename deT>
    void sort(deT* polys, std::vector<Point<T> >& sorted_points, std::vector<Line<T> >& sorted_lines, std::vector<Triangle<T> >& sorted_triangles, std::vector<Quad<T> >& sorted_quads)
    {
      if (polys->primitiveType() == PT_QUADS)
      {
        // compute zetas
        mPrimitiveZ.resize( polys->indexCount() / 4 );
        if (mPrimitiveZ.empty())
          return;

        for(unsigned iz=0, i=0; i<polys->indexCount(); i+=4, ++iz)
        {
          int a = polys->indices()->at(i+0);
          int b = polys->indices()->at(i+1);
          int c = polys->indices()->at(i+2);
          int d = polys->indices()->at(i+2);
          mPrimitiveZ[iz].mZ = (*mEyeSpaceVerts)[a].z() + (*mEyeSpaceVerts)[b].z() + (*mEyeSpaceVerts)[c].z() + (*mEyeSpaceVerts)[d].z();
          mPrimitiveZ[iz].mPrimitiveIndex = iz;
        }

        // sort triangles based on mPrimitiveZ
        if (sortMode() == SM_SortBackToFront)
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Back_To_Front() );
        else
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Front_To_Back() );

        // regenerate the sorted indices
        sorted_quads.resize( polys->indexCount() / 4 );
        Quad<T>* tris = (Quad<T>*)polys->indices()->ptr();
        for(unsigned int i=0; i<mPrimitiveZ.size(); ++i)
          sorted_quads[i] = tris[ mPrimitiveZ[i].mPrimitiveIndex ];
        memcpy(&tris[0], &sorted_quads[0], sizeof(sorted_quads[0])*sorted_quads.size() );
      }
      else
      if (polys->primitiveType() == PT_TRIANGLES)
      {
        // compute zetas
        mPrimitiveZ.resize( polys->indexCount() / 3 );
        if (mPrimitiveZ.empty())
          return;

        for(unsigned iz=0, i=0; i<polys->indexCount(); i+=3, ++iz)
        {
          int a = polys->indices()->at(i+0);
          int b = polys->indices()->at(i+1);
          int c = polys->indices()->at(i+2);
          mPrimitiveZ[iz].mZ = (*mEyeSpaceVerts)[a].z() + (*mEyeSpaceVerts)[b].z() + (*mEyeSpaceVerts)[c].z();
          mPrimitiveZ[iz].mPrimitiveIndex = iz;
        }

        // sort triangles based on mPrimitiveZ
        if (sortMode() == SM_SortBackToFront)
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Back_To_Front() );
        else
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Front_To_Back() );

        // regenerate the sorted indices
        sorted_triangles.resize( polys->indexCount() / 3 );
        Triangle<T>* tris = (Triangle<T>*)polys->indices()->ptr();
        for(unsigned int i=0; i<mPrimitiveZ.size(); ++i)
          sorted_triangles[i] = tris[ mPrimitiveZ[i].mPrimitiveIndex ];
        memcpy(&tris[0], &sorted_triangles[0], sizeof(sorted_triangles[0])*sorted_triangles.size() );
      }
      else
      if (polys->primitiveType() == PT_LINES)
      {
        // compute zetas
        mPrimitiveZ.resize( polys->indexCount() / 2 );
        if (mPrimitiveZ.empty())
          return;

        for(unsigned iz=0, i=0; i<polys->indexCount(); i+=2, ++iz)
        {
          int a = polys->indices()->at(i+0);
          int b = polys->indices()->at(i+1);
          mPrimitiveZ[iz].mZ = (*mEyeSpaceVerts)[a].z() + (*mEyeSpaceVerts)[b].z();
          mPrimitiveZ[iz].mPrimitiveIndex = iz;
        }

        // sort triangles based on mPrimitiveZ
        if (sortMode() == SM_SortBackToFront)
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Back_To_Front() );
        else
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Front_To_Back() );

        // regenerate the sorted indices
        sorted_lines.resize( polys->indexCount() / 2 );
        Line<T>* tris = (Line<T>*)polys->indices()->ptr();
        for(unsigned int i=0; i<mPrimitiveZ.size(); ++i)
          sorted_lines[i] = tris[ mPrimitiveZ[i].mPrimitiveIndex ];
        memcpy(&tris[0], &sorted_lines[0], sizeof(sorted_lines[0])*sorted_lines.size() );
      }
      else
      if (polys->primitiveType() == PT_POINTS)
      {
        // compute zetas
        mPrimitiveZ.resize( polys->indexCount() );
        if (mPrimitiveZ.empty())
          return;

        for(unsigned iz=0, i=0; i<polys->indexCount(); ++i, ++iz)
        {
          mPrimitiveZ[iz].mZ = (*mEyeSpaceVerts)[polys->indices()->at(i)].z();
          mPrimitiveZ[iz].mPrimitiveIndex = iz;
        }

        // sort triangles based on mPrimitiveZ
        if (sortMode() == SM_SortBackToFront)
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Back_To_Front() );
        else
          std::sort( mPrimitiveZ.begin(), mPrimitiveZ.end(), Sorter_Front_To_Back() );

        // regenerate the sorted indices
        sorted_points.resize( polys->indexCount() );
        Point<T>* tris = (Point<T>*)polys->indices()->ptr();
        for(unsigned int i=0; i<mPrimitiveZ.size(); ++i)
          sorted_points[i] = tris[ mPrimitiveZ[i].mPrimitiveIndex ];
        memcpy(&tris[0], &sorted_points[0], sizeof(sorted_points[0])*sorted_points.size() );
      }

      if (GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5||GLEW_VERSION_3_0)
      if (polys->indices()->gpuBuffer()->handle())
      {
        if (polys->indices()->gpuBuffer()->usage() != vl::GBU_DYNAMIC_DRAW)
        {
          polys->indices()->gpuBuffer()->setBufferData(vl::GBU_DYNAMIC_DRAW);
          polys->indices()->setVBODirty(false);
        }
        else
          polys->indices()->setVBODirty(true);
      }
    }

    ESortMode sortMode() const { return mSortMode; }
    void setSortMode(ESortMode sort_mode) { mSortMode = sort_mode; }

    /**
     * Forces sorting at the next rendering.
     */
    void invalidateCache() { mCacheMatrix = vl::mat4(); }

  protected:
    ref<ArrayFVec3> mEyeSpaceVerts;
    std::vector<PrimitiveZ> mPrimitiveZ;

    std::vector<PointUInt> mSortedPointsUInt;
    std::vector<LineUInt> mSortedLinesUInt;
    std::vector<TriangleUInt> mSortedTrianglesUInt;
    std::vector<QuadUInt> mSortedQuadsUInt;

    std::vector<PointUShort> mSortedPointsUShort;
    std::vector<LineUShort> mSortedLinesUShort;
    std::vector<TriangleUShort> mSortedTrianglesUShort;
    std::vector<QuadUShort> mSortedQuadsUShort;

    std::vector<PointUByte> mSortedPointsUByte;
    std::vector<LineUByte> mSortedLinesUByte;
    std::vector<TriangleUByte> mSortedTrianglesUByte;
    std::vector<QuadUByte> mSortedQuadsUByte;

    vl::mat4 mCacheMatrix;

    ESortMode mSortMode;
  };
}

#endif
