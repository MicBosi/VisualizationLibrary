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

#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/OpenGLContext.hpp>
#include <vlGraphics/DoubleVertexRemover.hpp>
#include <cmath>

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
  mColor = vl::white;
  mSecondaryColor = vl::white;
  mNormal = fvec3(0,0,1);
}
//-----------------------------------------------------------------------------
Geometry::~Geometry()
{
}
//-----------------------------------------------------------------------------
void Geometry::computeBounds_Implementation()
{
  const ArrayAbstract* coords = vertexArray();
  if (!coords && vertexAttribInfo(0))
    coords = vertexAttribInfo(0)->data();

  AABB aabb;

  for(int i=0; i<drawCalls()->size(); ++i)
  {
    for(IndexIterator iit = drawCalls()->at(i)->indexIterator(); !iit.isEnd(); iit.next())
    {
      aabb += coords->vectorAsVec3( iit.index() );
    }
  }

  Real radius = 0, r = 0;
  vec3 center = aabb.center();
  for(int i=0; i<drawCalls()->size(); ++i)
  {
    for(IndexIterator iit = drawCalls()->at(i)->indexIterator(); !iit.isEnd(); iit.next())
    {
      r = (coords->vectorAsVec3(iit.index()) - center).lengthSquared();
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
  deepCopy(geom.get());
  return geom;
}
//-----------------------------------------------------------------------------
void Geometry::deepCopy(Geometry* geom) const
{
  // copy the base class Renderable
  geom->Renderable::operator=(*this);
  // copy Geometry
  geom->mVertexArray         = mVertexArray         ? mVertexArray->clone().get()         : NULL;
  geom->mNormalArray         = mNormalArray         ? mNormalArray->clone().get()         : NULL;
  geom->mColorArray          = mColorArray          ? mColorArray->clone().get()          : NULL;
  geom->mSecondaryColorArray = mSecondaryColorArray ? mSecondaryColorArray->clone().get() : NULL;
  geom->mFogCoordArray       = mFogCoordArray       ? mFogCoordArray->clone().get()       : NULL;
  geom->mTexCoordArrays.resize(mTexCoordArrays.size());
  for(int i=0; i<mTexCoordArrays.size(); ++i)
    geom->mTexCoordArrays[i] = new TextureArray(mTexCoordArrays[i]->mTextureUnit, mTexCoordArrays[i]->mTexCoordArray ? mTexCoordArrays[i]->mTexCoordArray->clone().get() : NULL);
  // custom arrays
  geom->mVertexAttribArrays.resize(mVertexAttribArrays.size());
  for(int i=0; i<mVertexAttribArrays.size(); ++i)
  {
    geom->mVertexAttribArrays[i] = new VertexAttribInfo;
    geom->mVertexAttribArrays[i]->setNormalize( mVertexAttribArrays[i]->normalize() );
    geom->mVertexAttribArrays[i]->setDataBehavior( mVertexAttribArrays[i]->dataBehavior() );
    geom->mVertexAttribArrays[i]->setAttribIndex( mVertexAttribArrays[i]->attribIndex() );
    geom->mVertexAttribArrays[i]->setData( geom->mVertexAttribArrays[i]->data() ? geom->mVertexAttribArrays[i]->data()->clone().get() : NULL );
  }
  // primitives
  for(int i=0; i<mDrawCalls.size(); ++i)
    geom->mDrawCalls.push_back( mDrawCalls[i]->clone().get() );
  geom->mColor = mColor;
  geom->mSecondaryColor = mSecondaryColor;
  geom->mNormal = mNormal;
}
//-----------------------------------------------------------------------------
Geometry& Geometry::operator=(const Geometry& other)
{
  // copy the base class Renderable
  Renderable::operator=(*this);
  // copy Geometry attributes
  mVertexArray = other.mVertexArray;
  mNormalArray = other.mNormalArray;
  mColorArray = other.mColorArray;
  mSecondaryColorArray = other.mSecondaryColorArray;
  mFogCoordArray = other.mFogCoordArray;
  mTexCoordArrays = other.mTexCoordArrays;
  mVertexAttribArrays = other.mVertexAttribArrays;
  mColor = other.mColor;
  mSecondaryColor = other.mSecondaryColor;
  mNormal = other.mNormal;
  mDrawCalls = other.mDrawCalls;
  return *this;
}
//-----------------------------------------------------------------------------
ref<Geometry> Geometry::shallowCopy()
{
  ref<Geometry> geom = new Geometry;
  geom->operator=(*this);
  return geom;
}
//-----------------------------------------------------------------------------
void Geometry::shallowCopy(Geometry* geom)
{
  geom->operator=(*this);
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
    if (mTexCoordArrays.at(i)->mTextureUnit == tex_unit)
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
    ArrayFloat3* norm3f = dynamic_cast<ArrayFloat3*>(normalArray());
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
void Geometry::toGenericVertexAttribs()
{
  vertexAttribArrays()->clear();
  int index = 0;
  
  if (vertexArray())
  {
    setVertexAttribArray(index++, vertexArray());
    setVertexArray(NULL);
  }
  
  if (normalArray())
  {
    setVertexAttribArray(index++, normalArray());
    setNormalArray(NULL);
  }
  
  if (colorArray())
  {
    setVertexAttribArray(index++, colorArray());
    setColorArray(NULL);
  }

  for(int i=0; i<mTexCoordArrays.size(); i++)
    setVertexAttribArray( index++, mTexCoordArrays[i]->mTexCoordArray.get() );
  mTexCoordArrays.clear();
  
  if (secondaryColorArray())
  {
    setVertexAttribArray(index++, secondaryColorArray());
    setSecondaryColorArray(NULL);
  }

  if (fogCoordArray())
  {
    setVertexAttribArray(index++, fogCoordArray());
    setFogCoordArray(NULL);
  }
}
//-----------------------------------------------------------------------------
void Geometry::computeNormals(bool verbose)
{
  ArrayAbstract* posarr = vertexArray() ? vertexArray() : vertexAttrib(0);
  if (!posarr || posarr->size() == 0)
  {
    Log::warning("Geometry::computeNormals() not performed: no vertex coordinate array present!\n");
    return;
  }

  ref<ArrayFloat3> norm3f = new ArrayFloat3;
  norm3f->resize( posarr->size() );
  setNormalArray( norm3f.get() );

  // zero the normals
  for(int i=0; i<(int)posarr->size(); ++i)
    (*norm3f)[i] = 0;

  // iterate all draw calls
  for(int prim=0; prim<(int)drawCalls()->size(); prim++)
  {
    // iterate all triangles, if present
    for(TriangleIterator trit = mDrawCalls[prim]->triangleIterator(); !trit.isEnd(); trit.next())
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

      v0 = posarr->vectorAsVec4(a).xyz();
      v1 = posarr->vectorAsVec4(b).xyz();
      v2 = posarr->vectorAsVec4(c).xyz();

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
void Geometry::deleteVBOs()
{
  if (!Has_VBO)
    return;

  for(int i=0; i<(int)drawCalls()->size(); ++i)
    drawCalls()->at(i)->deleteVBOs();

  if (mVertexArray)
    mVertexArray->gpuBuffer()->deleteGLBufferObject();
  
  if (mNormalArray)
    mNormalArray->gpuBuffer()->deleteGLBufferObject();
  
  if (mColorArray)
    mColorArray->gpuBuffer()->deleteGLBufferObject();
  
  if (mSecondaryColorArray)
    mSecondaryColorArray->gpuBuffer()->deleteGLBufferObject();
  
  if (mFogCoordArray)
    mFogCoordArray->gpuBuffer()->deleteGLBufferObject();
  
  for (int i=0; i<mTexCoordArrays.size(); ++i)
    mTexCoordArrays[i]->mTexCoordArray->gpuBuffer()->deleteGLBufferObject();

  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if ( vertexAttribArrays()->at(i)->data() )
      vertexAttribArrays()->at(i)->data()->gpuBuffer()->deleteGLBufferObject();
}
//-----------------------------------------------------------------------------
void Geometry::updateVBOs(bool discard_local_data, bool force_update)
{
  setVBODirty(false);

  if (!Has_VBO)
    return;

  if ( mVertexArray && (mVertexArray->isVBODirty() || force_update) )
    mVertexArray->updateVBO(discard_local_data);
  
  if ( mNormalArray && (mNormalArray->isVBODirty() || force_update) )
    mNormalArray->updateVBO(discard_local_data);
  
  if ( mColorArray && (mColorArray->isVBODirty() || force_update) )
    mColorArray->updateVBO(discard_local_data);
  
  if ( mSecondaryColorArray && (mSecondaryColorArray->isVBODirty() || force_update) )
    mSecondaryColorArray->updateVBO(discard_local_data);
  
  if ( mFogCoordArray && (mFogCoordArray->isVBODirty() || force_update) )
    mFogCoordArray->updateVBO(discard_local_data);
  
  for(int i=0; i<mTexCoordArrays.size(); ++i)
  {
    if ( mTexCoordArrays[i]->mTexCoordArray->isVBODirty() || force_update )
      mTexCoordArrays[i]->mTexCoordArray->updateVBO(discard_local_data);
  }
  
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if ( vertexAttribArrays()->at(i)->data() )
      vertexAttribArrays()->at(i)->data()->updateVBO(discard_local_data);

  for(int i=0; i<drawCalls()->size(); ++i)
    drawCalls()->at(i)->updateVBOs(discard_local_data, force_update);
}
//-----------------------------------------------------------------------------
void Geometry::render_Implementation(const Actor*, const Shader*, const Camera*, OpenGLContext* gl_context) const
{
  VL_CHECK_OGL()

  // set default normal, color and secondary colors
  // note: Has_GL_Version_1_1 is false for GLES 1.x and GLES 2.x

  if (Has_GL_Version_1_1) 
  {
    if (!normalArray())
      glNormal3fv(mNormal.ptr());

    if (!colorArray())
      glColor4fv(mColor.ptr());

    if (!secondaryColorArray() && Has_GL_Version_1_4)
      glSecondaryColor3fv(mSecondaryColor.ptr());
  }

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
  ArrayAbstract* posarr = vertexArray() ? vertexArray() : vertexAttrib(0);
  if (posarr)
    posarr->transform(m);

  if (normalArray())
  {
    mat4 nmat = m.as3x3().getInverse().transpose();
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
    if (vertexAttribArrays()->at(i)->attribIndex() == info.attribIndex())
    {
      *vertexAttribArrays()->at(i) = info;
      return;
    }
  }
  mVertexAttribArrays.push_back( new VertexAttribInfo(info) );
}
//-----------------------------------------------------------------------------
const ArrayAbstract* Geometry::vertexAttrib(unsigned int name) const
{
  const VertexAttribInfo* attrib_info = vertexAttribInfo(name);
  if (attrib_info)
    return attrib_info->data();
  else
    return NULL;
}
//-----------------------------------------------------------------------------
ArrayAbstract* Geometry::vertexAttrib(unsigned int name)
{
  VertexAttribInfo* attrib_info = vertexAttribInfo(name);
  if (attrib_info)
    return attrib_info->data();
  else
    return NULL;
}
//-----------------------------------------------------------------------------
const VertexAttribInfo* Geometry::vertexAttribInfo(unsigned int name) const
{
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if (vertexAttribArrays()->at(i)->attribIndex() == name)
      return vertexAttribArrays()->at(i);
  return NULL;
}
//-----------------------------------------------------------------------------
VertexAttribInfo* Geometry::vertexAttribInfo(unsigned int name)
{
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
    if (vertexAttribArrays()->at(i)->attribIndex() == name)
      return vertexAttribArrays()->at(i);
  return NULL;
}
//-----------------------------------------------------------------------------
ref<VertexAttribInfo> Geometry::eraseVertexAttrib(unsigned int name)
{
  ref<VertexAttribInfo> vai;
  for(int i=0; i<vertexAttribArrays()->size(); ++i)
  {
    if (vertexAttribArrays()->at(i)->attribIndex() == name)
    {
      vai = vertexAttribArrays()->at(i);
      vertexAttribArrays()->eraseAt(i);
      return vai;
    }
  }
  return NULL;
}
//-----------------------------------------------------------------------------
void Geometry::mergeTriangleStrips()
{
  std::vector< ref<DrawElementsUInt> > de;
  std::vector<size_t> indices;

  // collect DrawElementsUInt
  for(int i=drawCalls()->size(); i--; )
  {
    ref<DrawElementsUInt> de_uint = dynamic_cast<DrawElementsUInt*>( drawCalls()->at(i) );
    if (de_uint && de_uint->primitiveType() == PT_TRIANGLE_STRIP)
    {
      de.push_back(de_uint);
      drawCalls()->erase(i,1);
    }
  }

  // generate new strip
  indices.reserve( vertexArray()->size()*2 );
  for(size_t i=0; i<de.size(); ++i)
  {
    if (!de[i]->indices()->size())
      continue;
    for(size_t j=0; j<de[i]->indices()->size(); ++j)
      indices.push_back(de[i]->indices()->at(j));
    // odd -> even
    if ( de[i]->indices()->size() % 2 )
      indices.push_back(indices.back());
    // concatenate next strip inserting degenerate triangles
    if ( i != de.size()-1 )
    {
      indices.push_back(indices.back());
      indices.push_back(de[i+1]->indices()->at(0));
      indices.push_back(de[i+1]->indices()->at(0));
      indices.push_back(de[i+1]->indices()->at(1));
    }
  }

  if (indices.size())
  {
    ref<DrawElementsUInt> draw_elems = new DrawElementsUInt(PT_TRIANGLE_STRIP);
    draw_elems->indices()->resize(indices.size());
    memcpy(draw_elems->indices()->ptr(), &indices[0], sizeof(unsigned int)*indices.size());
    drawCalls()->push_back(draw_elems.get());
  }
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
    for(IndexIterator it=drawCalls()->at(i)->indexIterator(); !it.isEnd(); it.next())
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
bool Geometry::sortVertices()
{
  // works only if the primitive types are all DrawElements
  std::vector< ref<DrawElementsUInt> >   de_uint;

  // collect DrawElements
  for(int i=0; i<drawCalls()->size(); ++i)
  {
    DrawElementsUInt*   dei = dynamic_cast<DrawElementsUInt*>(drawCalls()->at(i));
    DrawElementsUShort* des = dynamic_cast<DrawElementsUShort*>(drawCalls()->at(i));
    DrawElementsUByte*  deb = dynamic_cast<DrawElementsUByte*>(drawCalls()->at(i));
    if (dei)
      de_uint.push_back(dei);
    else
    if(des)
    {
      dei = new DrawElementsUInt(des->primitiveType(), des->instances());
      de_uint.push_back(dei);
      dei->indices()->resize( des->indices()->size() );
      for(unsigned int j=0; j<des->indices()->size(); ++j)
        dei->indices()->at(j) = des->indices()->at(j);
    }
    else
    if(deb)
    {
      dei = new DrawElementsUInt(deb->primitiveType(), deb->instances());
      de_uint.push_back(dei);
      dei->indices()->resize( deb->indices()->size() );
      for(unsigned int j=0; j<deb->indices()->size(); ++j)
        dei->indices()->at(j) = deb->indices()->at(j);
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
    for(size_t idx=0; idx<de_uint[i]->indices()->size(); ++idx)
      if (!used[de_uint[i]->indices()->at(idx)])
      {
        map_new_to_old[index] = de_uint[i]->indices()->at(idx);
        map_old_to_new[de_uint[i]->indices()->at(idx)] = index;
        index++;
        used[de_uint[i]->indices()->at(idx)] = 1;
      }
  }

  regenerateVertices(map_new_to_old);

  // remap DrawElements
  for(size_t i=0; i<de_uint.size(); ++i)
  {
    drawCalls()->push_back(de_uint[i].get());
    for(size_t j=0; j<de_uint[i]->indices()->size(); ++j)
    {
      de_uint[i]->indices()->at(j) = (GLuint)map_old_to_new[de_uint[i]->indices()->at(j)];
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

    for(IndexIterator it=drawCalls()->at(i)->indexIterator(); !it.isEnd(); it.next())
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
  
  for ( TriangleIterator trit = prim->triangleIterator(); !trit.isEnd(); trit.next() )
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
