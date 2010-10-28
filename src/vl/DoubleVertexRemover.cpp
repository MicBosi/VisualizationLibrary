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

#include <vl/DoubleVertexRemover.hpp>
#include <vl/Time.hpp>

using namespace vl;

//-----------------------------------------------------------------------------
template<class T>
ref<ArrayAbstract> VertexMapper::regenerateT(ArrayAbstract* data, const std::vector<size_t>& map_new_to_old) const
{
  ref<T> in_data = dynamic_cast<T*>(data);
  if (in_data)
  {
    ref<T> out_data = new T;
    out_data->resize(map_new_to_old.size());
    for(unsigned i=0; i<map_new_to_old.size(); ++i)
        out_data->at(i) = in_data->at(map_new_to_old[i]);
    return out_data;
  }
  return NULL;
}
//-----------------------------------------------------------------------------
ref<ArrayAbstract> VertexMapper::regenerate(ArrayAbstract* data, const std::vector<size_t>& map_new_to_old) const
{
  ref<ArrayAbstract> out_data;

  if ( (out_data = regenerateT<ArrayIVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayIVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayIVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayFVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayFVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayFVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayDVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayDVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayDVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayFloat>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayDouble>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUInt>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayInt>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayByte>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayShort>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUByte>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUShort>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUBVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUBVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUBVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayBVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayBVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayBVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArraySVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArraySVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArraySVec4>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUSVec2>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUSVec3>(data, map_new_to_old)) )
    return out_data;
  else
  if ( (out_data = regenerateT<ArrayUSVec4>(data, map_new_to_old)) )
    return out_data;

  return NULL;
}
//-----------------------------------------------------------------------------
void DoubleVertexRemover::removeDoubles(Geometry* geom)
{
  mMapNewToOld.clear();
  mMapOldToNew.clear();

  std::vector<unsigned int> verti;
  verti.resize(geom->vertexArray()->size());
  mMapOldToNew.resize(verti.size());
  for(unsigned int i=0; i<verti.size(); ++i)
  {
    verti[i] = i;
    mMapOldToNew[i] = 0xFFFFFFFF;
  }
  mMapNewToOld.reserve(verti.size());

  std::sort(verti.begin(), verti.end(), CompareVertex(geom));

  if (verti.empty())
    return;

  unsigned int unique_vert_idx = 0;
  for(unsigned i=1; i<verti.size(); ++i)
  {
    if ( !CompareVertex(geom).equals(verti[unique_vert_idx],verti[i]) )
    {
      for(unsigned j=unique_vert_idx; j<i; ++j)
        mMapOldToNew[verti[j]] = mMapNewToOld.size();
      mMapNewToOld.push_back(verti[unique_vert_idx]);
      unique_vert_idx = i;
    }
  }
  for(unsigned j=unique_vert_idx; j<verti.size(); ++j)
  {
    mMapOldToNew[verti[j]] = mMapNewToOld.size();
    mMapNewToOld.push_back(verti[unique_vert_idx]);
  }

  // regenerate vertices

  geom->regenerateVertices(mMapNewToOld);

  // regenerate DrawCall

  std::vector< ref<DrawCall> > draw_cmd;
  for(int idraw=0; idraw<geom->drawCalls()->size(); ++idraw)
    draw_cmd.push_back( geom->drawCalls()->at(idraw) );
  geom->drawCalls()->clear();

  for(unsigned idraw=0; idraw<draw_cmd.size(); ++idraw)
  {
    ref<DrawElementsUInt> tris = new DrawElementsUInt( draw_cmd[idraw]->primitiveType() );
    geom->drawCalls()->push_back(tris.get());
    const int idx_count = draw_cmd[idraw]->indexCount();
    tris->indices()->resize(idx_count);
    for(int i=0; i<idx_count; ++i)
      tris->indices()->at(i) = mMapOldToNew[draw_cmd[idraw]->index(i)];
    // tris->sortTriangles();
  }

  #if 0
    printf("DoubleVertexRemover = %d/%d, saved = %d, shrink=%.2f\n", (int)mMapNewToOld.size(), (int)verti.size(), (int)verti.size()-(int)mMapNewToOld.size(), (float)mMapNewToOld.size()/verti.size() );
  #endif
}
//-----------------------------------------------------------------------------
