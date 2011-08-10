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

#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/DoubleVertexRemover.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <cmath>
#include <algorithm>

using namespace vl;

//-----------------------------------------------------------------------------
// Geometry
//-----------------------------------------------------------------------------
Geometry::Geometry()
{
  VL_DEBUG_SET_OBJECT_NAME()
  mVertexAttribArrays.setAutomaticDelete(false);
  mTexCoordArrays.setAutomaticDelete(false);
  mDrawCalls.setAutomaticDelete(false);
}
//-----------------------------------------------------------------------------
Geometry::~Geometry()
{
}
//-----------------------------------------------------------------------------
void Geometry::computeBounds_Implementation()
{
  const ArrayAbstract* coords = vertexArray();
  if (!coords && vertexAttribArray(0))
    coords = vertexAttribArray(0)->data();

  if (coords == NULL)
  {
    Log::error("Geometry::computeBounds_Implementation() failed! No vertex buffer present!\n");
    return;
  }

  if (coords->size() == 0)
  {
    Log::error("Geometry::computeBounds_Implementation() failed! No vertices present in the local buffer! Did you forget to call setBoundingBox() and setBoundingSphere()?\n");
    return;
  }

  AABB aabb;
  for(int i=0; i<drawCalls()->size(); ++i)
  {
    for(IndexIterator iit = drawCalls()->at(i)->indexIterator(); iit.hasNext(); iit.next())
    {
      aabb += coords->getAsVec3( iit.index() );
    }
  }

  Real radius = 0, r = 0;
  vec3 center = aabb.center();
  for(int i=0; i<drawCalls()->size(); ++i)
  {
    for(IndexIterator iit = drawCalls()->at(i)->indexIterator(); iit.hasNext(); iit.next())
    {
      r = (coords->getAsVec3(iit.index()) - center).lengthSquared();
      if (r > radius)
        radius = r;
    }
  }

  setBoundingBox( aabb );
  setBoundingSphere( Sphere(center, radius) );
}
//-----------------------------------------------------------------------------
ref<Geometry> Geometry::deepCopy() const
{
  ref<Geometry> geom = new Geometry;
  geom->deepCopyFrom(*this);
  return geom;
}
//-----------------------------------------------------------------------------
Geometry& Geometry::deepCopyFrom(const Geometry& other)
{
  // copy the base class Renderable
  super::operator=(other);

  // copy Geometry
  mVertexArray         = other.mVertexArray         ? other.mVertexArray->clone().get()         : NULL;
  mNormalArray         = other.mNormalArray         ? other.mNormalArray->clone().get()         : NULL;
  mColorArray          = other.mColorArray          ? other.mColorArray->clone().get()          : NULL;
  mSecondaryColorArray = other.mSecondaryColorArray ? other.mSecondaryColorArray->clone().get() : NULL;
  mFogCoordArray       = other.mFogCoordArray       ? other.mFogCoordArray->clone().get()       : NULL;

  mTexCoordArrays.resize( other.mTexCoordArrays.size() );
  for(int i=0; i<mTexCoordArrays.size(); ++i)
    mTexCoordArrays[i] = new TextureArray(other.mTexCoordArrays[i]->mTextureSampler, other.mTexCoordArrays[i]->mTexCoordArray ? other.mTexCoordArrays[i]->mTexCoordArray->clone().get() : NULL);

  // custom arrays
  mVertexAttribArrays.resize( other.mVertexAttribArrays.size() );
  for(int i=0; i<mVertexAttribArrays.size(); ++i)
  {
    mVertexAttribArrays[i] = new VertexAttribInfo;
    mVertexAttribArrays[i]->setNormalize( other.mVertexAttribArrays[i]->normalize() );
    mVertexAttribArrays[i]->setDataBehavior( other.mVertexAttribArrays[i]->dataBehavior() );
    mVertexAttribArrays[i]->setAttribLocation( other.mVertexAttribArrays[i]->attribLocation() );
    mVertexAttribArrays[i]->setData( other.mVertexAttribArrays[i]->data() ? other.mVertexAttribArrays[i]->data()->clone().get() : NULL );
  }

  // primitives
  mDrawCalls.clear();
  for(int i=0; i<other.mDrawCalls.size(); ++i)
    mDrawCalls.push_back( other.mDrawCalls[i]->clone().get() );

  return *this;
}
//-----------------------------------------------------------------------------
ref<Geometry> Geometry::shallowCopy() const
{
  ref<Geometry> geom = new Geometry;
  geom->shallowCopyFrom(*this);
  return geom;
}
//-----------------------------------------------------------------------------
Geometry& Geometry::shallowCopyFrom(const Geometry& other)
{
  // copy the base class Renderable
  super::operator=(other);

  // copy Geometry attributes
  mVertexArray = other.mVertexArray;
  mNormalArray = other.mNormalArray;
  mColorArray = other.mColorArray;
  mSecondaryColorArray = other.mSecondaryColorArray;
  mFogCoordArray = other.mFogCoordArray;
  mTexCoordArrays = other.mTexCoordArrays;
  mVertexAttribArrays = other.mVertexAttribArrays;
  mDrawCalls = other.mDrawCalls;

  return *this;
}
//-----------------------------------------------------------------------------
void Geometry::setVertexArray(ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glVertexPointer
  VL_CHECK( !data || (data->glSize() >=2 && data->glSize()<=4) )

  mVertexArray = data;
}
//-----------------------------------------------------------------------------
void Geometry::setNormalArray(ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glNormalPointer
  VL_CHECK( !data || data->glSize() == 3 )
  VL_CHECK( !data || (data->glType() == GL_BYTE||
                      data->glType() == GL_SHORT ||
                      data->glType() == GL_INT ||
                      data->glType() == GL_FLOAT ||
                      data->glType() == GL_DOUBLE) );

  mNormalArray = data;
}
//-----------------------------------------------------------------------------
void Geometry::setColorArray(ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glColorPointer
  VL_CHECK( !data || (data->glSize() >=3 && data->glSize()<=4) )
  VL_CHECK( !data || (data->glType() == GL_BYTE ||
                      data->glType() == GL_SHORT ||
                      data->glType() == GL_INT ||
                      data->glType() == GL_UNSIGNED_BYTE ||
                      data->glType() == GL_UNSIGNED_SHORT ||
                      data->glType() == GL_UNSIGNED_INT ||
                      data->glType() == GL_FLOAT ||
                      data->glType() == GL_DOUBLE) );

  mColorArray = data;
}
//-----------------------------------------------------------------------------
void Geometry::setSecondaryColorArray(ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glSecondaryColorPointer
  VL_CHECK( !data || (data->glSize() >=3 && data->glSize()<=4) )
  VL_CHECK( !data || (data->glType() == GL_BYTE ||
                      data->glType() == GL_SHORT ||
                      data->glType() == GL_INT ||
                      data->glType() == GL_UNSIGNED_BYTE ||
                      data->glType() == GL_UNSIGNED_SHORT ||
                      data->glType() == GL_UNSIGNED_INT ||
                      data->glType() == GL_FLOAT ||
                      data->glType() == GL_DOUBLE) );

  mSecondaryColorArray = data;
}
//-----------------------------------------------------------------------------
void Geometry::setFogCoordArray(ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glFogCoordPointer
  VL_CHECK( !data || (data->glSize() == 1) )
  VL_CHECK( !data || (data->glType() == GL_FLOAT || data->glType() == GL_DOUBLE) );
  
  mFogCoordArray = data;
}
//-----------------------------------------------------------------------------
void Geometry::setTexCoordArray(int tex_unit, ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glTexCoordPointer
  VL_CHECK( !data || (data->glSize() == 1 || data->glSize() == 2 || data->glSize() == 3 || data->glSize() == 4) )
  VL_CHECK( !data || (data->glType() == GL_FLOAT  || 
                      data->glType() == GL_DOUBLE ||
                      data->glType() == GL_SHORT  ||
                      data->glType() == GL_INT) );

  VL_CHECK(tex_unit<VL_MAX_TEXTURE_UNITS);

  for(int i=0; i<mTexCoordArrays.size(); ++i)
  {
    if (mTexCoordArrays.at(i)->mTextureSampler == tex_unit)
    {
      if (data)
        mTexCoordArrays.at(i)->mTexCoordArray = data;
      else
        mTexCoordArrays.erase(i,1); // removes if NULL
      return;
    }
  }
  if (data)
    mTexCoordArrays.push_back(new TextureArray(tex_unit,data));
}
//-----------------------------------------------------------------------------
void Geometry::clearArrays(bool clear_draw_calls)
{
  mVertexArray = NULL;
  mNormalArray = NULL;
  mColorArray = NULL;
  mSecondaryColorArray = NULL;
  mFogCoordArray = NULL;
  mTexCoordArrays.clear();
  mVertexAttribArrays.clear();
  if (clear_draw_calls)
    mDrawCalls.clear();
}
//-----------------------------------------------------------------------------
bool Geometry::flipNormals()
{
  if (normalArray())
  {
    ArrayFloat3* norm3f = cast<ArrayFloat3>(normalArray());
    if (norm3f)
    {
      for(size_t i=0; i<norm3f->size(); ++i)
      {
        norm3f->at(i) = -norm3f->at(i);
      }
      return true;
    }
  }
  return false;
}
//-----------------------------------------------------------------------------
void Geometry::convertToVertexAttribs()
{
  std::map<int, ref<ArrayAbstract> > attrib_map;

  if (vertexArray())
  {
    attrib_map[VA_Position] = vertexArray();
    setVertexArray(NULL);
  }
  
  if (normalArray())
  {
    attrib_map[VA_Normal] = normalArray();
    setNormalArray(NULL);
  }
  
  if (colorArray())
  {
    attrib_map[VA_Color] = colorArray();
    setColorArray(NULL);
  }

  // Texture coordinates starting from VA_TexCoord0
  for(int i=0; i<mTexCoordArrays.size(); i++)
  {
    attrib_map[VA_TexCoord0+i] = mTexCoordArrays[i]->mTexCoordArray;
  }
  mTexCoordArrays.clear();
  
  // Secondary color and fog are packed right after the texture coordinates
  int index = VA_TexCoord0 + mTexCoordArrays.size();
  if (secondaryColorArray())
  {
    attrib_map[index++] = secondaryColorArray();
    setSecondaryColorArray(NULL);
  }

  if (fogCoordArray())
  {
    attrib_map[index++] = fogCoordArray();
    setFogCoordArray(NULL);
  }

  // copy over the collected attributes
  // note: we override eventual existing vertex attributes if they are in busy positions, the other are left where they are
  for(std::map<int, ref<ArrayAbstract> >::iterator it=attrib_map.begin(); it != attrib_map.end(); ++it)
  {
    if (vertexAttribArray(it->first) != NULL)
      Log::warning( Say("Geometry::convertToVertexAttribs(): vertex attrib index #%n is already in use, it will be overwritten.\n") << it->first );
    setVertexAttribArray(it->first, it->second.get());
  }

}
//-----------------------------------------------------------------------------
void Geometry::computeNormals(bool verbose)
{
  // Retrieve vertex position array
  ArrayAbstract* posarr = vertexArray() ? vertexArray() : vertexAttribArray(VA_Position)->data();
  if (!posarr || posarr->size() == 0)
  {
    Log::warning("Geometry::computeNormals() not performed: no vertex coordinate array present!\n");
    return;
  }

  ref<ArrayFloat3> norm3f = new ArrayFloat3;
  norm3f->resize( posarr->size() );

  // Install the normal array
  if (vertexArray())
    setNormalArray( norm3f.get() );
  else
    setVertexAttribArray(VA_Normal, norm3f.get());

  // zero the normals
  for(size_t i=0; i<norm3f->size(); ++i)
    (*norm3f)[i] = 0;

  // iterate all draw calls
  for(int prim=0; prim<(int)drawCalls()->size(); prim++)
  {
    // iterate all triangles, if present
    for(TriangleIterator trit = mDrawCalls[prim]->triangleIterator(); trit.hasNext(); trit.next())
    {
      size_t a = trit.a();
      size_t b = trit.b();
      size_t c = trit.c();

      if (verbose)
      if (a == b || b == c || c == a)
      {
        Log::warning( Say("Geometry::computeNormals(): skipping degenerate triangle %n %n %n\n") << a << b << c );
        continue;
      }

      VL_CHECK( a < posarr->size() )
      VL_CHECK( b < posarr->size() )
      VL_CHECK( c < posarr->size() )

      vec3 n, v0, v1, v2;

      v0 = posarr->getAsVec3(a);
      v1 = posarr->getAsVec3(b);
      v2 = posarr->getAsVec3(c);

      if (verbose)
      if (v0 == v1 || v1 == v2 || v2 == v0)
      {
        Log::warning("Geometry::computeNormals(): skipping degenerate triangle (same vertex coodinate).\n");
        continue;
      }

      v1 -= v0;
      v2 -= v0;

      n = cross(v1, v2);
      n.normalize();
      if (verbose)
      if ( fabs(1.0f - n.length()) > 0.1f )
      {
        Log::warning("Geometry::computeNormals(): skipping degenerate triangle (normalization failed).\n");
        continue;
      }

      (*norm3f)[a] += (fvec3)n;
      (*norm3f)[b] += (fvec3)n;
      (*norm3f)[c] += (fvec3)n;
    }
  }

  // normalize the normals
  for(int i=0; i<(int)norm3f->size(); ++i)
    (*norm3f)[i].normalize();
}
//-----------------------------------------------------------------------------
void Geometry::deleteVBO()
{
  if (!Has_VBO)
    return;

  for(int i=0; i<(int)drawCalls()->size(); ++i)
    drawCalls()->at(i)->deleteVBO();

  if (mVertexArray)
    mVertexArray->vbo()->deleteVBO();
  
  if (mNormalArray)
    mNormalArray->vbo()->deleteVBO();
  
  if (mColorArray)
    mColorArray->vbo()->deleteVBO();
  
  if (mSecondaryColorArray)
    mSecondaryColorArray->vbo()->deleteVBO();
  
  if (mFogCoordArray)
    mFogCoordArray->vbo()->deleteVBO();
  
  for (int i=0; i<mTexCoordArrays.size(); ++i)
    mTexCoordArrays[i]->mTexCoordArray->vbo()->deleteVBO();

  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if ( vertexAttribArrays()->at(i)->data() )
      vertexAttribArrays()->at(i)->data()->vbo()->deleteVBO();
}
//-----------------------------------------------------------------------------
void Geometry::updateDirtyVBO(EVBOUpdateMode mode)
{
  if (!Has_VBO)
    return;

  bool force_update = (mode & VUF_ForceUpdate) != 0;

  if ( mVertexArray && (mVertexArray->isVBODirty() || force_update) )
    mVertexArray->updateVBO(mode);
  
  if ( mNormalArray && (mNormalArray->isVBODirty() || force_update) )
    mNormalArray->updateVBO(mode);
  
  if ( mColorArray && (mColorArray->isVBODirty() || force_update) )
    mColorArray->updateVBO(mode);
  
  if ( mSecondaryColorArray && (mSecondaryColorArray->isVBODirty() || force_update) )
    mSecondaryColorArray->updateVBO(mode);
  
  if ( mFogCoordArray && (mFogCoordArray->isVBODirty() || force_update) )
    mFogCoordArray->updateVBO(mode);
  
  for(int i=0; i<mTexCoordArrays.size(); ++i)
  {
    if ( mTexCoordArrays[i]->mTexCoordArray->isVBODirty() || force_update )
      mTexCoordArrays[i]->mTexCoordArray->updateVBO(mode);
  }
  
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if ( vertexAttribArrays()->at(i)->data() && (vertexAttribArrays()->at(i)->data()->isVBODirty() || force_update) )
      vertexAttribArrays()->at(i)->data()->updateVBO(mode);

  for(int i=0; i<drawCalls()->size(); ++i)
    drawCalls()->at(i)->updateDirtyVBO(mode);
}
//-----------------------------------------------------------------------------
void Geometry::render_Implementation(const Actor*, const Shader*, const Camera*, OpenGLContext* gl_context) const
{
  VL_CHECK_OGL()

  // bind Vertex Attrib Set

  bool vbo_on = Has_VBO && vboEnabled() && !isDisplayListEnabled();
  gl_context->bindVAS(this, vbo_on, false);

  // actual draw

  for(int i=0; i<(int)drawCalls()->size(); i++)
    if (drawCalls()->at(i)->isEnabled())
      drawCalls()->at(i)->render( vbo_on );

  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void Geometry::transform(const mat4& m, bool normalize)
{
  ArrayAbstract* posarr = vertexArray() ? vertexArray() : vertexAttribArray(0) ? vertexAttribArray(0)->data() : NULL;
  if (posarr)
    posarr->transform(m);

  if (normalArray())
  {
    mat4 nmat = m.as3x3().invert().transpose();
    normalArray()->transform(nmat);
    if (normalize)
      normalArray()->normalize();
  }
}
//-----------------------------------------------------------------------------
void Geometry::setVertexAttribArray(const VertexAttribInfo& info)
{
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
  {
    VL_CHECK(vertexAttribArrays()->at(i))
    if (vertexAttribArrays()->at(i)->attribLocation() == info.attribLocation())
    {
      *vertexAttribArrays()->at(i) = info;
      return;
    }
  }
  mVertexAttribArrays.push_back( new VertexAttribInfo(info) );
}
//-----------------------------------------------------------------------------
const VertexAttribInfo* Geometry::vertexAttribArray(unsigned int attrib_location) const
{
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if (vertexAttribArrays()->at(i)->attribLocation() == attrib_location)
      return vertexAttribArrays()->at(i);
  return NULL;
}
//-----------------------------------------------------------------------------
VertexAttribInfo* Geometry::vertexAttribArray(unsigned int attrib_location)
{
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if (vertexAttribArrays()->at(i)->attribLocation() == attrib_location)
      return vertexAttribArrays()->at(i);
  return NULL;
}
//-----------------------------------------------------------------------------
DrawCall* Geometry::mergeTriangleStrips()
{
  std::vector< ref<DrawElementsBase> > de_vector;
  std::vector<size_t> indices;

  // collect DrawElementsUInt
  for(int i=drawCalls()->size(); i--; )
  {
    ref<DrawElementsBase> deb = cast<DrawElementsBase>( drawCalls()->at(i) );
    if (deb && deb->primitiveType() == PT_TRIANGLE_STRIP)
    {
      // preserve order
      de_vector.push_back( deb );
      drawCalls()->eraseAt(i);
    }
  }
  // preseve rendering order
  std::reverse(de_vector.begin(), de_vector.end());

  // generate new strip
  indices.reserve( vertexArray()->size()*2 );
  for(size_t i=0; i<de_vector.size(); ++i)
  {
    size_t index_count = 0;
    for(IndexIterator it=de_vector[i]->indexIterator(); it.hasNext(); it.next(), ++index_count)
      indices.push_back(it.index());

    if (index_count == 0)
      continue;
    
    // odd -> even
    if ( index_count % 2 )
      indices.push_back( indices.back() );

    // concatenate next strip inserting degenerate triangles
    if ( i != de_vector.size()-1 )
    {
      // grab the first two indices of the next draw call
      IndexIterator it = de_vector[i+1]->indexIterator();
      int A = it.index();
      it.next();
      int B = it.index();

      if (A == -1 || B == -1)
        continue;

      indices.push_back( indices.back() );
      indices.push_back(A);
      indices.push_back(A);
      indices.push_back(B);
    }
  }

  if (indices.size())
  {
    ref<DrawElementsUInt> draw_elems = new DrawElementsUInt(PT_TRIANGLE_STRIP);
    draw_elems->indexBuffer()->resize(indices.size());
    memcpy(draw_elems->indexBuffer()->ptr(), &indices[0], sizeof(indices[0])*indices.size());
    drawCalls()->push_back(draw_elems.get());
    return draw_elems.get();
  }
  else
    return NULL;
}
//-----------------------------------------------------------------------------
void Geometry::mergeDrawCallsWithPrimitiveRestart(EPrimitiveType primitive_type)
{
  size_t total_index_count = 0;
  std::vector< ref<DrawCall> > mergendo_calls;
  for( size_t i=drawCalls()->size(); i--; )
  {
    if (drawCalls()->at(i)->primitiveType() == primitive_type)
    {
      int index_count = drawCalls()->at(i)->countIndices();
      VL_CHECK(index_count >= 0);
      total_index_count += index_count;
      // insert at the head to preserve the primitive rendering order
      mergendo_calls.push_back( drawCalls()->at(i) );
      drawCalls()->eraseAt(i);
    }
  }
  // preseve rendering order
  std::reverse(mergendo_calls.begin(), mergendo_calls.end());

  Log::debug( Say("%n draw calls will be merged using primitive restart.\n") << mergendo_calls.size() );

  ref<DrawElementsUInt> de_prim_restart = new DrawElementsUInt(primitive_type);
  // make space for all the indices plus the primitive restart markers.
  de_prim_restart->indexBuffer()->resize(total_index_count + mergendo_calls.size()-1);
  GLuint* index = de_prim_restart->indexBuffer()->begin();
  // merge draw calls using primitive restart!
  for( size_t i=0; i<mergendo_calls.size(); ++i )
  {
    for( IndexIterator it = mergendo_calls[i]->indexIterator(); it.hasNext(); it.next(), ++index )
    {
      *index = it.index();
      VL_CHECK(*index < vertexArray()->size());
    }
    if ( i != mergendo_calls.size() -1 )
    {
      *index = DrawElementsUInt::primitive_restart_index;
      ++index;
    }
  }
  VL_CHECK( index == de_prim_restart->indexBuffer()->end() )

  // enable primitive restart!
  de_prim_restart->setPrimitiveRestartEnabled(true);

  drawCalls()->push_back( de_prim_restart.get() );
}
//-----------------------------------------------------------------------------
void Geometry::mergeDrawCallsWithMultiDrawElements(EPrimitiveType primitive_type)
{
  size_t total_index_count = 0;
  std::vector< ref<DrawCall> > mergendo_calls;
  std::vector<GLsizei> count_vector;
  for( size_t i=drawCalls()->size(); i--; )
  {
    if (drawCalls()->at(i)->primitiveType() == primitive_type)
    {
      int index_count = drawCalls()->at(i)->countIndices();
      VL_CHECK(index_count >= 0);
      total_index_count += index_count;
      count_vector.push_back( index_count );
      mergendo_calls.push_back( drawCalls()->at(i) );
      drawCalls()->eraseAt(i);
    }
  }
  // preseve rendering order
  std::reverse(mergendo_calls.begin(), mergendo_calls.end());
  std::reverse(count_vector.begin(), count_vector.end());

  Log::debug( Say("%n draw calls will be merged using MultiDrawElements.\n") << mergendo_calls.size() );

  ref<MultiDrawElementsUInt> de_multi = new MultiDrawElementsUInt(primitive_type);
  // make space for all the indices plus the primitive restart markers.
  de_multi->indexBuffer()->resize(total_index_count);
  GLuint* index = de_multi->indexBuffer()->begin();
  // merge draw calls using primitive restart!
  for( size_t i=0; i<mergendo_calls.size(); ++i )
  {
    for( IndexIterator it = mergendo_calls[i]->indexIterator(); it.hasNext(); it.next(), ++index )
    {
      *index = it.index();
      VL_CHECK(*index < vertexArray()->size());
    }
  }
  VL_CHECK( index == de_multi->indexBuffer()->end() )

  // Specify primitive boundaries. This must be done last!
  de_multi->setCountVector( count_vector );

  drawCalls()->push_back( de_multi.get() );
}
//-----------------------------------------------------------------------------
void Geometry::mergeDrawCallsWithTriangles(EPrimitiveType primitive_type)
{
  size_t triangle_count = 0;
  std::vector< ref<DrawCall> > mergendo_calls;
  for( size_t i=drawCalls()->size(); i--; )
  {
    const DrawCall& dc = *drawCalls()->at(i);

    // ignore primitives that cannot be triangulated
    switch(dc.primitiveType())
    {
    case PT_TRIANGLES:
    case PT_TRIANGLE_STRIP:
    case PT_TRIANGLE_FAN:
    case PT_QUADS:
    case PT_QUAD_STRIP:
    case PT_POLYGON:
      break;
    default:
      continue;
    }

    if (primitive_type == PT_UNKNOWN || dc.primitiveType() == primitive_type || dc.primitiveType() == PT_TRIANGLES)
    {
      triangle_count += dc.countTriangles();
      // insert at the head to preserve the primitive rendering order
      mergendo_calls.insert( mergendo_calls.begin(), drawCalls()->at(i) );
      drawCalls()->eraseAt(i);
    }
  }
  // preseve rendering order
  std::reverse(mergendo_calls.begin(), mergendo_calls.end());

  // if there was one single PT_TRIANGLES draw calls then we are done.
  if ( mergendo_calls.size() == 1 && mergendo_calls[0]->primitiveType() == PT_TRIANGLES )
  {
    drawCalls()->push_back( mergendo_calls[0].get() );
    return;
  }

  ref<DrawElementsUInt> de = new DrawElementsUInt;
  ArrayUInt1& index_buffer = *de->indexBuffer();
  index_buffer.resize( triangle_count * 3 );
  size_t idx = 0;
  int max_idx = (int)vertexArray()->size();
  for(size_t i=0; i<mergendo_calls.size(); ++i)
  {
    for(TriangleIterator it = mergendo_calls[i]->triangleIterator(); it.hasNext(); it.next(), idx+=3)
    {
      VL_CHECK( idx+2 < index_buffer.size() );
      index_buffer[idx+0] = it.a();
      index_buffer[idx+1] = it.b();
      index_buffer[idx+2] = it.c();

      // some sanity checks since we are here...
      VL_CHECK( it.a() < max_idx && it.b() < max_idx && it.c() < max_idx );
      VL_CHECK( it.a() >= 0 && it.b() >= 0 && it.c() >= 0 );
    }
  }
  VL_CHECK( idx == index_buffer.size() );
  drawCalls()->push_back(de.get());
}
//-----------------------------------------------------------------------------
void Geometry::regenerateVertices(const std::vector<size_t>& map_new_to_old)
{
  VertexMapper mapper;

  if (vertexArray())
    setVertexArray( mapper.regenerate( vertexArray(), map_new_to_old ).get() );

  if (normalArray())
    setNormalArray( mapper.regenerate( normalArray(), map_new_to_old ).get() );

  if (colorArray())
    setColorArray( mapper.regenerate( colorArray(), map_new_to_old ).get() );

  if (secondaryColorArray())
    setSecondaryColorArray( mapper.regenerate( secondaryColorArray(), map_new_to_old ).get() );

  if (fogCoordArray())
    setFogCoordArray( mapper.regenerate( fogCoordArray(), map_new_to_old ).get() );

  for(int itex=0; itex<VL_MAX_TEXTURE_UNITS; ++itex)
    if (texCoordArray(itex))
      setTexCoordArray( itex, mapper.regenerate( texCoordArray(itex), map_new_to_old ).get() );

  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    vertexAttribArrays()->at(i)->setData( mapper.regenerate(vertexAttribArrays()->at(i)->data(), map_new_to_old ).get() );
}
//-----------------------------------------------------------------------------
void Geometry::convertDrawCallToDrawArrays()
{
  // generate mapping 
  std::vector<size_t> map_new_to_old;
  map_new_to_old.reserve( vertexArray()->size() * 3 );

  for(int i=drawCalls()->size(); i--; )
  {
    int start = (int)map_new_to_old.size();
    for(IndexIterator it=drawCalls()->at(i)->indexIterator(); it.hasNext(); it.next())
      map_new_to_old.push_back(it.index());
    int count = (int)map_new_to_old.size() - start;

    // substitute with DrawArrays
    ref<DrawArrays> da = new vl::DrawArrays( drawCalls()->at(i)->primitiveType(), start, count, drawCalls()->at(i)->instances() );
    drawCalls()->erase(i,1);
    drawCalls()->push_back(da.get());
  }

  regenerateVertices(map_new_to_old);
}
//-----------------------------------------------------------------------------
void Geometry::makeGLESFriendly()
{
  // Pass#1: remove PT_QUADS, PT_QUADS_STRIP, PT_POLYGON
  for( int idraw=this->drawCalls()->size(); idraw--; )
  {
    DrawCall* dc = this->drawCalls()->at(idraw);
    if( dc->classType() == vl::DrawElementsUInt::Type() )
    {
      if (dc->primitiveType() == PT_POLYGON)
      {
        DrawElementsUInt* polygon = static_cast<DrawElementsUInt*>(dc);
        ref<DrawElementsUInt> triangles = new DrawElementsUInt(PT_TRIANGLES);
        triangles->indexBuffer()->resize( (polygon->indexBuffer()->size()-2) * 3 );
        for(size_t i=0, itri=0; i<polygon->indexBuffer()->size()-2; ++i, itri+=3)
        {
          triangles->indexBuffer()->at(itri+0) = polygon->indexBuffer()->at(0);
          triangles->indexBuffer()->at(itri+1) = polygon->indexBuffer()->at(i+1);
          triangles->indexBuffer()->at(itri+2) = polygon->indexBuffer()->at(i+2);
        }
        // substitute the draw call
        this->drawCalls()->eraseAt(idraw);
        this->drawCalls()->push_back(triangles.get());
      }
      else
      if (dc->primitiveType() == PT_QUAD_STRIP)
      {
        dc->setPrimitiveType(vl::PT_TRIANGLE_STRIP);
      }
      else
      if (dc->primitiveType() == PT_QUADS)
      {
        DrawElementsUInt* quads = static_cast<DrawElementsUInt*>(dc);
        ref<DrawElementsUInt> triangles = new DrawElementsUInt(PT_TRIANGLES);
        triangles->indexBuffer()->resize( quads->indexBuffer()->size() / 4 * 6 );
        unsigned int* triangle_idx = &triangles->indexBuffer()->at(0);
        unsigned int* quad_idx = &quads->indexBuffer()->at(0);
        unsigned int* quad_end = &quads->indexBuffer()->at(0) + quads->indexBuffer()->size();
        for( ; quad_idx < quad_end ; quad_idx += 4, triangle_idx += 6 )
        {
          triangle_idx[0] = quad_idx[0];
          triangle_idx[1] = quad_idx[1];
          triangle_idx[2] = quad_idx[2];

          triangle_idx[3] = quad_idx[2];
          triangle_idx[4] = quad_idx[3];
          triangle_idx[5] = quad_idx[0];
        }
        // substitute the draw call
        this->drawCalls()->eraseAt(idraw);
        this->drawCalls()->push_back(triangles.get());
      }
    }
    else
    if ( strstr(dc->className(), "vl::MultiDrawElements") )
    {
        dc->setEnabled(false);
        Log::warning( "Geometry::makeGLESFriendly(): cannot convert vl::MultiDrawElements, draw call disabled.\n" );
    }
    else
    if ( strstr(dc->className(), "vl::DrawRangeElements") )
    {
        dc->setEnabled(false);
        Log::warning( "Geometry::makeGLESFriendly(): cannot convert vl::DrawRangeElements, draw call disabled.\n" );
    }
  } // for()

  // Pass #2: shrink DrawElementsUInt to UByte or UShort
  for( int idraw=this->drawCalls()->size(); idraw--; )
  {
    DrawCall* dc = this->drawCalls()->at(idraw);
    if( dc->classType() == vl::DrawElementsUInt::Type() )
    {
      // find max index
      int max_idx = -1;
      int idx_count = 0;
      for( vl::IndexIterator it = dc->indexIterator(); it.hasNext(); it.next(), ++idx_count )
        max_idx = it.index() > max_idx ? it.index() : max_idx;

      if(max_idx <= 0xFF)
      {
        // shrink to DrawElementsUByte
        ref<DrawElementsUByte> de = new DrawElementsUByte( dc->primitiveType() );
        de->indexBuffer()->resize( idx_count );
        int i=0;
        for( vl::IndexIterator it = dc->indexIterator(); it.hasNext(); it.next(), ++i )
          de->indexBuffer()->at(i) = (GLubyte)it.index();

        // substitute new draw call
        this->drawCalls()->eraseAt(idraw);
        this->drawCalls()->push_back(de.get());
      }
      else
      if(max_idx <= 0xFFFF)
      {
        // shrink to DrawElementsUShort
        ref<DrawElementsUShort> de = new DrawElementsUShort( dc->primitiveType() );
        de->indexBuffer()->resize( idx_count );
        int i=0;
        for( vl::IndexIterator it = dc->indexIterator(); it.hasNext(); it.next(), ++i )
          de->indexBuffer()->at(i) = (GLushort)it.index();

        // substitute new draw call
        this->drawCalls()->eraseAt(idraw);
        this->drawCalls()->push_back(de.get());
      }
      else
      {
        dc->setEnabled(false);
        Log::error( Say("Geometry::makeGLESFriendly(): could not shrink DrawElementsUInt, max index found is %n! Draw call disabled.\n") << max_idx );
      }
    }

    // check supported primitive types
    switch(dc->primitiveType())
    {
    case GL_POINTS:
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
    case GL_LINES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    case GL_TRIANGLES:
      break;

    case PT_QUADS:
    case PT_QUAD_STRIP:
    case PT_POLYGON:
    case PT_LINES_ADJACENCY:
    case PT_LINE_STRIP_ADJACENCY:
    case PT_TRIANGLES_ADJACENCY:
    case PT_TRIANGLE_STRIP_ADJACENCY:
    case PT_PATCHES:
      dc->setEnabled(false);
      Log::error("Geometry::makeGLESFriendly(): primitive type illegal under GLES, draw call disabled.\n");
      break;
    }
  } // for()

  // converts legacy vertex arrays into generic vertex attributes
#if defined(VL_OPENGL_ES2)
  convertToVertexAttribs();
#endif
}
//-----------------------------------------------------------------------------
bool Geometry::sortVertices()
{
  // works only if the primitive types are all DrawElements
  std::vector< ref<DrawElementsUInt> >   de_uint;

  // collect DrawElements
  for(int i=0; i<drawCalls()->size(); ++i)
  {
    DrawElementsUInt*   dei = cast<DrawElementsUInt>(drawCalls()->at(i));
    DrawElementsUShort* des = cast<DrawElementsUShort>(drawCalls()->at(i));
    DrawElementsUByte*  deb = cast<DrawElementsUByte>(drawCalls()->at(i));
    if (dei)
      de_uint.push_back(dei);
    else
    if(des)
    {
      dei = new DrawElementsUInt(des->primitiveType(), des->instances());
      de_uint.push_back(dei);
      dei->indexBuffer()->resize( des->indexBuffer()->size() );
      for(unsigned int j=0; j<des->indexBuffer()->size(); ++j)
        dei->indexBuffer()->at(j) = des->indexBuffer()->at(j);
    }
    else
    if(deb)
    {
      dei = new DrawElementsUInt(deb->primitiveType(), deb->instances());
      de_uint.push_back(dei);
      dei->indexBuffer()->resize( deb->indexBuffer()->size() );
      for(unsigned int j=0; j<deb->indexBuffer()->size(); ++j)
        dei->indexBuffer()->at(j) = deb->indexBuffer()->at(j);
    }
    else
      return false;
  }

  drawCalls()->clear();

  // generate mapping 
  std::vector<size_t> map_new_to_old;
  map_new_to_old.resize( vertexArray()->size() );
  memset(&map_new_to_old[0], 0xFF, map_new_to_old.size()*sizeof(map_new_to_old[0]));

  std::vector<size_t> map_old_to_new;
  map_old_to_new.resize( vertexArray()->size() );
  memset(&map_old_to_new[0], 0xFF, map_old_to_new.size()*sizeof(map_old_to_new[0]));

  std::vector<size_t> used;
  used.resize( vertexArray()->size() );
  memset(&used[0], 0, used.size()*sizeof(used[0]));

  size_t index = 0;
  for(int i=(int)de_uint.size(); i--; )
  {
    for(size_t idx=0; idx<de_uint[i]->indexBuffer()->size(); ++idx)
      if (!used[de_uint[i]->indexBuffer()->at(idx)])
      {
        map_new_to_old[index] = de_uint[i]->indexBuffer()->at(idx);
        map_old_to_new[de_uint[i]->indexBuffer()->at(idx)] = index;
        index++;
        used[de_uint[i]->indexBuffer()->at(idx)] = 1;
      }
  }

  regenerateVertices(map_new_to_old);

  // remap DrawElements
  for(size_t i=0; i<de_uint.size(); ++i)
  {
    drawCalls()->push_back(de_uint[i].get());
    for(size_t j=0; j<de_uint[i]->indexBuffer()->size(); ++j)
    {
      de_uint[i]->indexBuffer()->at(j) = (GLuint)map_old_to_new[de_uint[i]->indexBuffer()->at(j)];
    }
  }

  return true;
}
//-----------------------------------------------------------------------------
void Geometry::colorizePrimitives()
{
  ref<ArrayFloat4> col = new vl::ArrayFloat4;
  col->resize( vertexArray()->size() );
  setColorArray( col.get() );

  for(int i=0; i<drawCalls()->size(); ++i)
  {
    fvec4 c;
    c.r() = rand()%100 / 99.0f;
    c.g() = rand()%100 / 99.0f;
    c.b() = rand()%100 / 99.0f;
    c.a() = 1.0f;

    for(IndexIterator it=drawCalls()->at(i)->indexIterator(); it.hasNext(); it.next())
      col->at( it.index() ) = c;
  }
}
//-----------------------------------------------------------------------------
void Geometry::computeTangentSpace(
  size_t vert_count, 
  const fvec3 *vertex, 
  const fvec3* normal,
  const fvec2 *texcoord, 
  const DrawCall* prim,
  fvec3 *tangent, 
  fvec3 *bitangent )
{
  std::vector<fvec3> tan1;
  std::vector<fvec3> tan2;
  tan1.resize(vert_count);
  tan2.resize(vert_count);
  
  for ( TriangleIterator trit = prim->triangleIterator(); trit.hasNext(); trit.next() )
  {
    unsigned int tri[] = { trit.a(), trit.b(), trit.c() };

    VL_CHECK(tri[0] < vert_count );
    VL_CHECK(tri[1] < vert_count );
    VL_CHECK(tri[2] < vert_count );
    
    const fvec3& v1 = vertex[tri[0]];
    const fvec3& v2 = vertex[tri[1]];
    const fvec3& v3 = vertex[tri[2]];
    
    const fvec2& w1 = texcoord[tri[0]];
    const fvec2& w2 = texcoord[tri[1]];
    const fvec2& w3 = texcoord[tri[2]];
    
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
    fvec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
    fvec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

    tan1[tri[0]] += sdir;
    tan1[tri[1]] += sdir;
    tan1[tri[2]] += sdir;

    tan2[tri[0]] += tdir;
    tan2[tri[1]] += tdir;
    tan2[tri[2]] += tdir;
  }

  for ( size_t a = 0; a < vert_count; a++)
  {
    const fvec3& n = normal[a];
    const fvec3& t = tan1[a];

    // Gram-Schmidt orthogonalize
    tangent[a] = (t - n * dot(n, t)).normalize();

    if ( bitangent )
    {
      // Calculate handedness
      float w = (dot(cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
      bitangent[a] = cross( n, tangent[a] ) * w;
    }
  }
}
//-----------------------------------------------------------------------------
