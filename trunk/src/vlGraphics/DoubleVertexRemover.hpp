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

#ifndef DoubleVertexRemover_INCLUDE_ONCE
#define DoubleVertexRemover_INCLUDE_ONCE

#include <vlGraphics/Geometry.hpp>
#include <vector>

namespace vl
{
  //-----------------------------------------------------------------------------
  // VertexMapper
  //-----------------------------------------------------------------------------
  //! Generates a set of new vertices from the old one.
  class VLGRAPHICS_EXPORT VertexMapper: public Object
  {
    VL_INSTRUMENT_CLASS(vl::VertexMapper, Object)

  public:
    //! Regenerates a new Array based on the given mapping.
    //! \param data The array to be regenerated
    //! \param map_new_to_old Specifies the mapping from the old vetices to the new one. The \p i-th vertex of the new vertex array will use the \p map_new_to_old[i]-th vertex of the old array, 
    //! that is, \p map_new_to_old[i] specifies the \a old vertex to be used to generate the \a new \p i-th vertex.
    ref<ArrayAbstract> regenerate(ArrayAbstract* data, const std::vector<size_t>& map_new_to_old) const;
  private:
    template<class T>
    ref<ArrayAbstract> regenerateT(ArrayAbstract* data, const std::vector<size_t>& map_new_to_old) const;
  };
  //-----------------------------------------------------------------------------
  // DoubleVertexRemover
  //-----------------------------------------------------------------------------
  //! Removes from a Geometry the vertices with the same attributes. 
  //! As a result also all the DrawArrays prensent in the Geometry are substituted with DrawElements.
  class VLGRAPHICS_EXPORT DoubleVertexRemover: public VertexMapper
  {
    VL_INSTRUMENT_CLASS(vl::DoubleVertexRemover, VertexMapper)

  private:
    class LessCompare
    {
    public:
      LessCompare(const Geometry* geom)
      {
        if (geom->vertexArray())
          mAttribs.push_back(geom->vertexArray());
        if (geom->normalArray())
          mAttribs.push_back(geom->normalArray());
        if (geom->colorArray())
          mAttribs.push_back(geom->colorArray());
        if (geom->secondaryColorArray())
          mAttribs.push_back(geom->secondaryColorArray());
        if (geom->fogCoordArray())
          mAttribs.push_back(geom->fogCoordArray());
        for(int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
          if (geom->texCoordArray(i))
            mAttribs.push_back(geom->texCoordArray(i));
        for(int i=0; i<geom->vertexAttribArrays()->size(); ++i)
          mAttribs.push_back(geom->vertexAttribArrays()->at(i)->data());
      }

      bool operator()(unsigned int a, unsigned int b) const 
      { 
        for(unsigned i=0; i<mAttribs.size(); ++i)
        {
          int val = mAttribs[i]->compare(a,b);
          if (val != 0)
            return val < 0;
        }
        return false;
      }

    protected:
      std::vector< const ArrayAbstract* > mAttribs;
    };

    class EqualsCompare
    {
    public:
      EqualsCompare(const Geometry* geom)
      {
        if (geom->vertexArray())
          mAttribs.push_back(geom->vertexArray());
        if (geom->normalArray())
          mAttribs.push_back(geom->normalArray());
        if (geom->colorArray())
          mAttribs.push_back(geom->colorArray());
        if (geom->secondaryColorArray())
          mAttribs.push_back(geom->secondaryColorArray());
        if (geom->fogCoordArray())
          mAttribs.push_back(geom->fogCoordArray());
        for(int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
          if (geom->texCoordArray(i))
            mAttribs.push_back(geom->texCoordArray(i));
        for(int i=0; i<geom->vertexAttribArrays()->size(); ++i)
          mAttribs.push_back(geom->vertexAttribArrays()->at(i)->data());
      }

      bool operator()(unsigned int a, unsigned int b) const 
      { 
        for(unsigned i=0; i<mAttribs.size(); ++i)
        {
          if (mAttribs[i]->compare(a,b) != 0)
            return false;
        }
        return true;
      }

    protected:
      std::vector< const ArrayAbstract* > mAttribs;
    };

  public:
    DoubleVertexRemover() {}
    void removeDoubles(Geometry* geom);
    const std::vector<size_t>& mapNewToOld() const { return mMapNewToOld; }
    const std::vector<size_t>& mapOldToNew() const { return mMapOldToNew; }

  protected:
    std::vector<size_t> mMapNewToOld;
    std::vector<size_t> mMapOldToNew;
  };
}

#endif
