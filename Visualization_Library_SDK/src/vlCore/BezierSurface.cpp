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

#include <vlCore/BezierSurface.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
/**
\verbatim
- The simplest bicubic Bézier patch requires 4x4 = 16 control points: A, B, C, D, E, F, G, H, I, L, M, N, O, P, Q, R
- The Bézier surface is guaranteed to touch only the 4 corner control points A, D, O and R.

A---B---C---D
|   |   |   |
E---F---G---H
|   |   |   |
I---L---M---N
|   |   |   |
O---P---Q---R

In this case we would call "resize(4,4)"

- You can concatenate two bicubic Bézier patches to form a larger suface by sharing their control points like this:

   patch 1     patch 2
A---+---+---B---+---+---G
|   |   |   |   |   |   |
+---+---+---C---+---+---+
|   |   |   |   |   |   |
+---+---+---D---+---+---+
|   |   |   |   |   |   |
F---+---+---E---+---+---H

In this case we would call "resize(7,4)"

- In this case the two patches share the control points B, C, D and E.
- As we can see the total control points needed are 28 = (2 (patches along x) * 3 + 1) * (1 (patches along y) * 3 + 1)
- Also in this case the Bézier surface is guaranteed to touch only the 6 corner control points A, B, E, F, G and H.
\endverbatim
*/
void BezierPatch::resize(int x, int y)
{
  if ( ((x-1)/3)*3+1 != x  || ((y-1)/3)*3+1 != y )
  {
    vl::Log::error("BezierPatch::resize(): illegal patch dimensions.\n");
    mControlPoints.clear();
    // in debug mode stop here
    VL_CHECK( ((x-1)/3)*3+1 == x )
    VL_CHECK( ((y-1)/3)*3+1 == y )
    return;
  }

  mX = x;
  mY = y;
  mControlPoints.resize(x);
  for(int i=0; i<x; ++i)
    mControlPoints[i].resize(y);
}
//-----------------------------------------------------------------------------
void BezierSurface::updateBezierSurface(bool gen_tex_coords)
{
  int patch_count = 0;
  for(unsigned ipatch=0; ipatch<patches().size(); ++ipatch)
    patch_count += ((patches()[ipatch]->x()-1)/3)*((patches()[ipatch]->y()-1)/3);

  ref<ArrayFloat3> vert_array = dynamic_cast<ArrayFloat3*>(vertexArray());
  if (!vert_array)
  {
    vert_array = new ArrayFloat3;
    setVertexArray(vert_array.get());
  }
  vert_array->resize(detail()*detail()*patch_count);

  ref<ArrayFloat2> texc_array = dynamic_cast<ArrayFloat2*>(texCoordArray(0));
  if ( gen_tex_coords )
  {
    if (!texc_array)
    {
      texc_array = new ArrayFloat2;
      setTexCoordArray(0,texc_array.get());
    }
    texc_array->resize(detail()*detail()*patch_count);
  }

  ref<DrawElementsUInt> de = drawCalls()->size() == 1 ? dynamic_cast<DrawElementsUInt*>(drawCalls()->at(0)) : NULL;
  if (!de)
  {
    drawCalls()->clear();
    de = new DrawElementsUInt(PT_QUADS);
    drawCalls()->push_back(de.get());
  }
  de->indices()->resize((detail()-1)*(detail()-1)*4*patch_count);

  int ivert = 0;
  int iquad = 0;
  for(unsigned ipatch=0, patch_num=0; ipatch<patches().size(); ++ipatch)
  {
    const BezierPatch::Points& p = patches()[ipatch]->points();
    for(unsigned ix=0; ix<p.size()-3;     ix+=3)
    for(unsigned iy=0; iy<p[ix].size()-3; iy+=3)
    {
      for(unsigned y=0; y<detail(); ++y)
      {
        // A   B   C   D
        // ^   ^   ^   ^
        // +---+---+---+
        // |   |   |   |
        // +---+---+---+
        // |   |   |   |
        // +---+---+---+
        // |   |   |   |
        // +---+---+---+
        double v  = (double)y/(detail()-1);
        double ty = 1.0f - v;
        double ty1 = 1.0f - ty;
        double k0 = ty*ty*ty;
        double k1 = 3*ty*ty*ty1;
        double k2 = 3*ty*ty1*ty1;
        double k3 = ty1*ty1*ty1;
        dvec3 A = p[ix+0][iy+0]*k0 + p[ix+0][iy+1]*k1 + p[ix+0][iy+2]*k2 + p[ix+0][iy+3]*k3;
        dvec3 B = p[ix+1][iy+0]*k0 + p[ix+1][iy+1]*k1 + p[ix+1][iy+2]*k2 + p[ix+1][iy+3]*k3;
        dvec3 C = p[ix+2][iy+0]*k0 + p[ix+2][iy+1]*k1 + p[ix+2][iy+2]*k2 + p[ix+2][iy+3]*k3;
        dvec3 D = p[ix+3][iy+0]*k0 + p[ix+3][iy+1]*k1 + p[ix+3][iy+2]*k2 + p[ix+3][iy+3]*k3;
        for(unsigned x=0; x<detail(); ++x, ++ivert)
        {
          double u  = (double)x/(detail()-1);
          double tx = 1.0f - u;
          double tx1 = 1.0f - tx;
          vert_array->at(ivert) = (fvec3)(A*tx*tx*tx + B*3*tx*tx*tx1 + C*3*tx*tx1*tx1 + D*tx1*tx1*tx1);
          if(gen_tex_coords)
          {
            texc_array->at(ivert).x() = (float)u;
            texc_array->at(ivert).y() = (float)v;
          }
        }
      }

      int istart = detail()*detail()*patch_num;
      for(unsigned y=0; y<detail()-1; ++y)
      {
        for(unsigned x=0; x<detail()-1; ++x)
        {
          de->indices()->at(iquad++) = istart + y    *detail() + x;
          de->indices()->at(iquad++) = istart + y    *detail() + x+1;
          de->indices()->at(iquad++) = istart + (y+1)*detail() + x+1;
          de->indices()->at(iquad++) = istart + (y+1)*detail() + x;
        }
      }
      ++patch_num;
    }
  }
}
//-----------------------------------------------------------------------------
