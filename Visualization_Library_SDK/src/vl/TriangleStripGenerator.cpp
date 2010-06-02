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

#include "../3rdparty/tristripper/tri_stripper.cpp"
#include "../3rdparty/tristripper/policy.cpp"
#include "../3rdparty/tristripper/connectivity_graph.cpp"

using namespace triangle_stripper;

#include <vl/TriangleStripGenerator.hpp>
#include <vl/Geometry.hpp>
#include <vl/DoubleVertexRemover.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>

using namespace vl;

namespace
{
  void fillIndices(std::vector<unsigned int>& indices, const Primitives* dc, bool substitute_quads)
  {
    indices.clear();
    switch(dc->primitiveType())
    {
      case PT_TRIANGLES:
      {
        indices.reserve(dc->indexCount());
        for(unsigned j=0; j<dc->indexCount(); j+=3)
        {
          int a = dc->index(j+0);
          int b = dc->index(j+1);
          int c = dc->index(j+2);
          // skip degenerate triangles
          if (a != b && b != c)
          {
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);
          }
        }
      }
      break;

      case PT_QUADS:
      {
        if (substitute_quads)
        {
          indices.reserve(dc->indexCount()/4*6);
          for(unsigned q=0; q<dc->indexCount(); q+=4)
          {
            int a = dc->index(q+0);
            int b = dc->index(q+1);
            int c = dc->index(q+2);
            int d = dc->index(q+3);
            if (a != b && a != c)
            {
              indices.push_back(a);
              indices.push_back(b);
              indices.push_back(c);
            }
            if (c!=d && d!=a)
            {
              indices.push_back(c);
              indices.push_back(d);
              indices.push_back(a);
            }
          }
          break;
        }
        else
          return;
      }

      default:
        return;
    }
  }
} // namespace vl

void TriangleStripGenerator::stripfy(Geometry* geom, int cache_size, bool merge_strips, bool remove_doubles, bool substitute_quads)
{
  if (remove_doubles)
  {
    DoubleVertexRemover dvr;
    dvr.removeDoubles(geom);
  }

  for( int idraw=geom->primitives()->size(); idraw--; )
  {
    Primitives* dc = geom->primitives()->at(idraw);

    triangle_stripper::indices indices;

    fillIndices(indices, dc, substitute_quads);

    std::vector<unsigned int> algo2_strip;
    std::vector<unsigned int> algo2_tris;
    // stripfyAlgo2(algo2_strip, algo2_tris, indices, cache_size);

    tri_stripper striper(indices);
    striper.SetCacheSize(cache_size);
    striper.SetMinStripSize(4);
    primitive_vector out;
    striper.Strip(&out);

    // install new strip
    if (out.size())
    {
      geom->primitives()->erase(idraw,1);
      algo2_strip.reserve(indices.size());
      for(unsigned s=0; s<out.size(); ++s)
      {
        if (out[s].Type == TRIANGLE_STRIP)
        {
          algo2_strip.clear();
          for(unsigned p=0; p<out[s].Indices.size(); ++p)
            algo2_strip.push_back(out[s].Indices[p]);

          ref<DrawElementsUInt> draw_elems = new DrawElementsUInt(PT_TRIANGLE_STRIP);
          draw_elems->indices()->resize(algo2_strip.size());
          memcpy(draw_elems->indices()->ptr(), &algo2_strip[0], sizeof(unsigned int)*algo2_strip.size());
          geom->primitives()->push_back(draw_elems.get());
        }
        else // TRIANGLES
        {
          algo2_tris.clear();
          for(unsigned p=0; p<out[s].Indices.size(); ++p)
            algo2_tris.push_back(out[s].Indices[p]);

          ref<DrawElementsUInt> draw_elems = new DrawElementsUInt(PT_TRIANGLES);
          draw_elems->indices()->resize(algo2_tris.size());
          memcpy(draw_elems->indices()->ptr(), &algo2_tris[0], sizeof(unsigned int)*algo2_tris.size());
          geom->primitives()->push_back(draw_elems.get());
        }
      }
    }
  }

  if (merge_strips)
    geom->mergeTriangleStrips();

  geom->shrinkDrawElements();
}

