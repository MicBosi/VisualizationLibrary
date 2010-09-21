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

#include <vl/Matrix4.hpp>
#include <vl/Geometry.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>
#include <vl/Shader.hpp>
#include <vl/Renderer.hpp>
#include <vl/Time.hpp>
#include <vl/DoubleVertexRemover.hpp>
#include <cmath>

using namespace vl;

void func()
{
  bvec4 a;
  svec4 b;
  fvec4 c;
  a = (bvec4)b;
  a = (bvec4)c;
  short sh = 1;
  b = sh * b;
}

//-----------------------------------------------------------------------------
// Geometry
//-----------------------------------------------------------------------------
Geometry::Geometry()
{
  #ifndef NDEBUG
    mName = "Geometry";
  #endif
  mVertexAttributeArrays.setAutomaticDelete(false);
  mTexCoordArrays.setAutomaticDelete(false);
  mPrimitives.setAutomaticDelete(false);
  mColorArrayConstant = vlut::white;
}
Geometry::~Geometry()
{
  if (GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5)
    deleteVBOs();
}
//-----------------------------------------------------------------------------
void Geometry::computeBounds_Implementation()
{
  // empty
  if(!mVertexArray)
  {
    setBoundingBox( AABB() );
    setBoundingSphere( Sphere() );
    return;
  }

  setBoundingBox( vertexArray()->computeBoundingBox() );
  setBoundingSphere( vertexArray()->computeBoundingSphere() );
}
//-----------------------------------------------------------------------------
ref<Geometry> Geometry::deepCopy() const
{
  ref<Geometry> geom = new Geometry;
  // copy the base class Renderable
  geom->Renderable::operator=(*this);
  // copy Geometry
  geom->mVertexArray       = mVertexArray       ? mVertexArray->clone().get()       : NULL;
  geom->mNormalArray         = mNormalArray         ? mNormalArray->clone().get()         : NULL;
  geom->mColorArray          = mColorArray          ? mColorArray->clone().get()          : NULL;
  geom->mSecondaryColorArray = mSecondaryColorArray ? mSecondaryColorArray->clone().get() : NULL;
  geom->mFogCoordArray       = mFogCoordArray       ? mFogCoordArray->clone().get()       : NULL;
  geom->mTexCoordArrays.resize(mTexCoordArrays.size());
  for(int i=0; i<mTexCoordArrays.size(); ++i)
    geom->mTexCoordArrays[i] = new TextureArray(mTexCoordArrays[i]->mTextureUnit, mTexCoordArrays[i]->mTexCoordArray ? mTexCoordArrays[i]->mTexCoordArray->clone().get() : NULL);
  // custom arrays
  geom->mVertexAttributeArrays.resize(mVertexAttributeArrays.size());
  for(int i=0; i<mVertexAttributeArrays.size(); ++i)
  {
    geom->mVertexAttributeArrays[i] = new VertexAttributeArray;
    geom->mVertexAttributeArrays[i]->setNormalize( mVertexAttributeArrays[i]->normalize() );
    geom->mVertexAttributeArrays[i]->setPureInteger( mVertexAttributeArrays[i]->pureInteger() );
    geom->mVertexAttributeArrays[i]->setAttribIndex( mVertexAttributeArrays[i]->attribIndex() );
    geom->mVertexAttributeArrays[i]->setData( geom->mVertexAttributeArrays[i]->data() ? geom->mVertexAttributeArrays[i]->data()->clone().get() : NULL );
  }
  // primitives
  for(int i=0; i<mPrimitives.size(); ++i)
    geom->mPrimitives.push_back( mPrimitives[i]->clone().get() );
  geom->mColorArrayConstant = mColorArrayConstant;
  return geom;
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
  mColorArrayConstant = other.mColorArrayConstant;
  mVertexAttributeArrays = other.mVertexAttributeArrays;
  mPrimitives = other.mPrimitives;
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
size_t Geometry::triangleCount() const
{
  size_t count = 0;
  for(int i=0; i<(int)primitives()->size(); ++i)
    if (primitives()->at(i)->isEnabled())
      count += primitives()->at(i)->triangleCount();
  return count;
}
//-----------------------------------------------------------------------------
size_t Geometry::lineCount() const
{
  size_t count = 0;
  for(int i=0; i<(int)primitives()->size(); ++i)
    if (primitives()->at(i)->isEnabled())
      count += primitives()->at(i)->lineCount();
  return count;
}
//-----------------------------------------------------------------------------
size_t Geometry::pointCount() const
{
  size_t count = 0;
  for(int i=0; i<(int)primitives()->size(); ++i)
    if (primitives()->at(i)->isEnabled())
      count += primitives()->at(i)->pointCount();
  return count;
}
//-----------------------------------------------------------------------------
void Geometry::setVertexArray(ArrayAbstract* data)
{
  // if one of this checks fail read the OpenGL Programmers Guide or the Reference Manual 
  // to see what "size" and "type" are allowed for glVertexPointer
  VL_CHECK( !data || (data->glSize() >=2 && data->glSize()<=4) )
  VL_CHECK( !data || (data->glType() == GL_SHORT ||
                      data->glType() == GL_INT   ||
                      data->glType() == GL_FLOAT ||
                      data->glType() == GL_DOUBLE) );

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

  VL_CHECK(tex_unit<VL_MAX_TEXTURE_UNIT_COUNT);

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
void Geometry::setColorArray(const fvec4& col)
{
  mColorArrayConstant = col;
  setColorArray(NULL);
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

  mVertexAttributeArrays.clear();

  if (clear_draw_calls)
    mPrimitives.clear();
}
//-----------------------------------------------------------------------------
void Geometry::computeNormals()
{
  VL_CHECK( mVertexArray.get() )
  if (!mVertexArray.get() || vertexArray()->size() == 0)
  {
    Log::warning("Geometry::computeNormals() not performed: no vertices.\n");
    return;
  }

  ArrayAbstract * posarr = vertexArray();

  ref<ArrayFVec3> norm3f = new ArrayFVec3;
  norm3f->resize( vertexArray()->size() );
  setNormalArray( norm3f.get() );

  for(int i=0; i<(int)mVertexArray->size(); ++i)
    (*norm3f)[i] = 0;

  for(int prim=0; prim<(int)primitives()->size(); prim++)
  {
    switch( mPrimitives[prim]->primitiveType() )
    {
      default:
        break;

      case PT_TRIANGLES:
      {
        VL_CHECK(mPrimitives[prim]->indexCount())
        VL_CHECK(mPrimitives[prim]->indexCount() >= 3)

        for(int i=0; i<(int)mPrimitives[prim]->indexCount(); i+=3)
        {
          size_t a = mPrimitives[prim]->index(i+0);
          size_t b = mPrimitives[prim]->index(i+1);
          size_t c = mPrimitives[prim]->index(i+2);

          VL_CHECK( a < posarr->size() )
          VL_CHECK( b < posarr->size() )
          VL_CHECK( c < posarr->size() )

          vec3 n, v0, v1, v2;

          v0 = posarr->vectorAsVec4(a).xyz();
          v1 = posarr->vectorAsVec4(b).xyz() - v0;
          v2 = posarr->vectorAsVec4(c).xyz() - v0;

          n = cross(v1, v2);
          // VL_CHECK(n.length());
          n.normalize();
          // VL_CHECK(n.length());

          VL_CHECK( a < norm3f->size() )
          VL_CHECK( b < norm3f->size() )
          VL_CHECK( c < norm3f->size() )

          (*norm3f)[a] += (fvec3)n;
          (*norm3f)[b] += (fvec3)n;
          (*norm3f)[c] += (fvec3)n;
        }
      }
      break;

      case PT_QUADS:
      {
        VL_CHECK(mPrimitives[prim]->indexCount() >= 4)

        for(int i=0; i<(int)mPrimitives[prim]->indexCount(); i+=4)
        {
          size_t a = mPrimitives[prim]->index(i+0);
          size_t b = mPrimitives[prim]->index(i+1);
          size_t c = mPrimitives[prim]->index(i+2);
          size_t d = mPrimitives[prim]->index(i+3);

          VL_CHECK( a < posarr->size() )
          VL_CHECK( b < posarr->size() )
          VL_CHECK( c < posarr->size() )
          VL_CHECK( d < posarr->size() )

          vec3 n, v0, v1, v2;

          // a------b
          // |      |
          // |  v0  |
          // |      |
          // d------c

          // nicely manage degenerate quads

          v0 = (posarr->vectorAsVec4(a).xyz() + posarr->vectorAsVec4(b).xyz() + posarr->vectorAsVec4(c).xyz() + posarr->vectorAsVec4(d).xyz()) * 0.25f;

          v1 = posarr->vectorAsVec4(a).xyz() - v0;
          v2 = posarr->vectorAsVec4(b).xyz() - v0;
          n = cross(v1, v2);

          v1 = posarr->vectorAsVec4(b).xyz() - v0;
          v2 = posarr->vectorAsVec4(c).xyz() - v0;
          n += cross(v1, v2);

          v1 = posarr->vectorAsVec4(c).xyz() - v0;
          v2 = posarr->vectorAsVec4(d).xyz() - v0;
          n += cross(v1, v2);

          v1 = posarr->vectorAsVec4(d).xyz() - v0;
          v2 = posarr->vectorAsVec4(a).xyz() - v0;
          n += cross(v1, v2);

          n.normalize();

          VL_CHECK( a < norm3f->size() )
          VL_CHECK( b < norm3f->size() )
          VL_CHECK( c < norm3f->size() )
          VL_CHECK( d < norm3f->size() )

          (*norm3f)[a] += (fvec3)n;
          (*norm3f)[b] += (fvec3)n;
          (*norm3f)[c] += (fvec3)n;
          (*norm3f)[d] += (fvec3)n;
        }
      }
      break;

      case PT_POLYGON:
      {
        VL_CHECK(mPrimitives[prim]->indexCount() >= 3)

        size_t a = mPrimitives[prim]->index(0);
        size_t b = mPrimitives[prim]->index(1);
        size_t c = mPrimitives[prim]->index(2);

        VL_CHECK( a < posarr->size() )
        VL_CHECK( b < posarr->size() )
        VL_CHECK( c < posarr->size() )

        vec3 n, v0, v1, v2;

        v0 = posarr->vectorAsVec4(a).xyz();
        v1 = posarr->vectorAsVec4(b).xyz() - v0;
        v2 = posarr->vectorAsVec4(c).xyz() - v0;

        n = cross(v1, v2).normalize();

        for(size_t i=0; i<mPrimitives[prim]->indexCount(); ++i)
        {
          VL_CHECK( i < norm3f->size() )
          (*norm3f)[ mPrimitives[prim]->index(i) ] += (fvec3)n;
        }
      } 
      break;

      case PT_TRIANGLE_FAN:
      {
        VL_CHECK(mPrimitives[prim]->indexCount() >= 3)

        for(int i=1; i<(int)mPrimitives[prim]->indexCount()-1; ++i)
        {
          size_t a = mPrimitives[prim]->index(0);
          size_t b = mPrimitives[prim]->index(i);
          size_t c = mPrimitives[prim]->index(i+1);

          VL_CHECK( a < posarr->size() )
          VL_CHECK( b < posarr->size() )
          VL_CHECK( c < posarr->size() )

          vec3 n, v0, v1, v2;

          v0 = posarr->vectorAsVec4(a).xyz();
          v1 = posarr->vectorAsVec4(b).xyz() - v0;
          v2 = posarr->vectorAsVec4(c).xyz() - v0;

          n = cross(v1, v2).normalize();

          VL_CHECK( a < norm3f->size() )
          VL_CHECK( b < norm3f->size() )
          VL_CHECK( c < norm3f->size() )

          (*norm3f)[a] += (fvec3)n;
          (*norm3f)[b] += (fvec3)n;
          (*norm3f)[c] += (fvec3)n;
        }
      } 
      break;

      case PT_QUAD_STRIP:
      case PT_TRIANGLE_STRIP:
      {
        VL_CHECK(mPrimitives[prim]->indexCount() >= 3)

        for(int i=0; i<(int)mPrimitives[prim]->indexCount()-2; ++i)
        {
          size_t a = mPrimitives[prim]->index(i);
          size_t b = mPrimitives[prim]->index(i+1);
          size_t c = mPrimitives[prim]->index(i+2);

          VL_CHECK( a < posarr->size() )
          VL_CHECK( b < posarr->size() )
          VL_CHECK( c < posarr->size() )

          // skip degenerate tris
          if (a == b || a == c || b == c)
            continue;

          vec3 n, v0, v1, v2;

          v0 = posarr->vectorAsVec4(a).xyz();
          v1 = posarr->vectorAsVec4(b).xyz() - v0;
          v2 = posarr->vectorAsVec4(c).xyz() - v0;

          n = cross(v1, v2).normalize();

          if (i%2)
            n = -n;

          VL_CHECK( a < norm3f->size() )
          VL_CHECK( b < norm3f->size() )
          VL_CHECK( c < norm3f->size() )

          (*norm3f)[a] += (fvec3)n;
          (*norm3f)[b] += (fvec3)n;
          (*norm3f)[c] += (fvec3)n;
        }
      } 
      break;
    }
  }

  for(int i=0; i<(int)norm3f->size(); ++i)
  {
    // VL_CHECK((*norm3f)[i].length())
    (*norm3f)[i].normalize();
  }
}
//-----------------------------------------------------------------------------
void Geometry::deleteVBOs()
{
  if (!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5))
    return;

  for(int i=0; i<(int)primitives()->size(); ++i)
    primitives()->at(i)->deleteVBOs();
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
  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    if ( vertexAttributeArrays()->at(i)->data() )
      vertexAttributeArrays()->at(i)->data()->gpuBuffer()->deleteGLBufferObject();
}
//-----------------------------------------------------------------------------
void Geometry::updateVBOs(bool discard_local_data)
{
  setVBODirty(false);

  if (!(GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5))
    return;

  if (mVertexArray)
    mVertexArray->updateVBO(discard_local_data);
  if (mNormalArray)
    mNormalArray->updateVBO(discard_local_data);
  if (mColorArray)
    mColorArray->updateVBO(discard_local_data);
  if (mSecondaryColorArray)
    mSecondaryColorArray->updateVBO(discard_local_data);
  if (mFogCoordArray)
    mFogCoordArray->updateVBO(discard_local_data);
  for(int i=0; i<mTexCoordArrays.size(); ++i)
    mTexCoordArrays[i]->mTexCoordArray->updateVBO(discard_local_data);
  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    if ( vertexAttributeArrays()->at(i)->data() )
      vertexAttributeArrays()->at(i)->data()->updateVBO(discard_local_data);
  for(int i=0; i<primitives()->size(); ++i)
    primitives()->at(i)->updateVBOs(discard_local_data);
}
//-----------------------------------------------------------------------------
void Geometry::render( const Actor*, const OpenGLContext* opengl_context, const Camera* ) const
{
  VL_CHECK_OGL()

  if (!mVertexArray)
    return;

  // check vbo is enabled and supported - when VBOs are enabled they are used where available otherwise the local buffers are used.
  bool vbo_on = (GLEW_ARB_vertex_buffer_object||GLEW_VERSION_1_5) && vboEnabled() && !displayListEnabled();

  // check if there is data to render

  if ( !mVertexArray->gpuBuffer()->handle() && !mVertexArray->size() )
    return;

  bool normal_on         = false;
  bool color_on          = false;
  GLboolean sec_color_on = false;
  GLboolean fog_on       = false;

  normal_on    |= mNormalArray && mNormalArray->size();
  color_on     |= mColorArray && mColorArray->size();
  sec_color_on |= mSecondaryColorArray && mSecondaryColorArray->size();
  fog_on       |= mFogCoordArray && mFogCoordArray->size();

  if (vbo_on)
  {
    normal_on    |= mNormalArray && mNormalArray->gpuBuffer() && mNormalArray->gpuBuffer()->handle();
    color_on     |= mColorArray && mColorArray->gpuBuffer() && mColorArray->gpuBuffer()->handle();
    sec_color_on |= mSecondaryColorArray && mSecondaryColorArray->gpuBuffer() && mSecondaryColorArray->gpuBuffer()->handle();
    fog_on       |= mFogCoordArray && mFogCoordArray->gpuBuffer() && mFogCoordArray->gpuBuffer()->handle();
  }

  fog_on       &= GLEW_EXT_fog_coord||GLEW_VERSION_1_4;
  sec_color_on &= GLEW_EXT_secondary_color||GLEW_VERSION_1_4;

  // setup pointers, default is local memory pointer

  unsigned char* vertex_pointer    = mVertexArray       ? mVertexArray->gpuBuffer()->ptr() : 0;
  unsigned char* normal_pointer    = mNormalArray         ? mNormalArray->ptr() : 0;
  unsigned char* color_pointer     = mColorArray          ? mColorArray->ptr() : 0;
  unsigned char* sec_color_pointer = mSecondaryColorArray ? mSecondaryColorArray->ptr() : 0;
  unsigned char* fog_pointer       = mFogCoordArray       ? mFogCoordArray->ptr() : 0;

  // if vbo_on and VBOs exist then we set the pointers to 0

  if (vbo_on)
  {
    if (mVertexArray->gpuBuffer()->handle())
      vertex_pointer = 0;
    if (normal_on && mNormalArray->gpuBuffer()->handle())
      normal_pointer = 0;
    if (color_on && mColorArray->gpuBuffer()->handle())
      color_pointer = 0;
    if (sec_color_on && mSecondaryColorArray->gpuBuffer()->handle())
      sec_color_pointer = 0;
    if (fog_on && mFogCoordArray->gpuBuffer()->handle())
      fog_pointer = 0;
  }

  VL_CHECK_OGL()

  VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

  // vertex position

  if (vbo_on && mVertexArray->gpuBuffer()->handle())
  {
    VL_glBindBuffer( GL_ARRAY_BUFFER, mVertexArray->gpuBuffer()->handle() );
    vertex_pointer = 0;
  }
  glVertexPointer( mVertexArray->glSize(), mVertexArray->glType(), /*mVertexArray->stride()*/0, vertex_pointer /*+ mVertexArray->offset()*/ );
  glEnableClientState(GL_VERTEX_ARRAY);

  if(vbo_on && mVertexArray && mVertexArray->gpuBuffer()->handle())
    VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

  // custom vertex attributes

  VL_CHECK_OGL()

  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
  {
    ArrayAbstract* data = vertexAttributeArrays()->at(i)->data();
    VL_CHECK(data->gpuBuffer())

    const unsigned char* attrib_pointer = data->ptr();
    if (vbo_on && data->gpuBuffer()->handle())
    {
      VL_glBindBuffer( GL_ARRAY_BUFFER, data->gpuBuffer()->handle() );
      attrib_pointer = 0;
    }

    VL_CHECK((int)vertexAttributeArrays()->at(i)->attribIndex() != -1)
    if ( vertexAttributeArrays()->at(i)->pureInteger() )
    {
      VL_WARN(GLEW_EXT_gpu_shader4||GLEW_VERSION_3_0)
      VL_glVertexAttribIPointer( vertexAttributeArrays()->at(i)->attribIndex(), data->glSize(), data->glType(), /*vertexAttributeArrays()->at(i)->data()->stride()*/0, attrib_pointer /*+ vertexAttributeArrays()->at(i)->data()->offset()*/ );
      VL_glEnableVertexAttribArray( vertexAttributeArrays()->at(i)->attribIndex() );
    }
    else
    {
      VL_glVertexAttribPointer( vertexAttributeArrays()->at(i)->attribIndex(), data->glSize(), data->glType(), vertexAttributeArrays()->at(i)->normalize(), /*vertexAttributeArrays()->at(i)->data()->stride()*/0, attrib_pointer /*+ vertexAttributeArrays()->at(i)->data()->offset()*/ );
      VL_glEnableVertexAttribArray( vertexAttributeArrays()->at(i)->attribIndex() );
    }

    if(vbo_on && data && data->gpuBuffer()->handle())
      VL_glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  // standard vertex attributes

  VL_CHECK_OGL()

  if (normal_on)
  {
    if (vbo_on && mNormalArray->gpuBuffer()->handle())
    {
      VL_glBindBuffer( GL_ARRAY_BUFFER, mNormalArray->gpuBuffer()->handle() );
      normal_pointer = 0;
    }
    VL_CHECK(mNormalArray->glSize() == 3);
    glNormalPointer( mNormalArray->glType(), /*mNormalArray->stride()*/0, normal_pointer /*+ mNormalArray->offset()*/ );
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  /*else
    glNormal3f(0,1,0);*/

  if(vbo_on && mNormalArray && mNormalArray->gpuBuffer()->handle())
    VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

  VL_CHECK_OGL()

  if (color_on)
  {
    if (vbo_on && mColorArray->gpuBuffer()->handle())
    {
      VL_glBindBuffer( GL_ARRAY_BUFFER, mColorArray->gpuBuffer()->handle() );
      color_pointer = 0;
    }
    glColorPointer( mColorArray->glSize(), mColorArray->glType(), 0/*mColorArray->stride()*/, color_pointer /*+ mColorArray->offset()*/ );
    glEnableClientState(GL_COLOR_ARRAY);
  }
  else
    glColor4fv(mColorArrayConstant.ptr());

  if(vbo_on && mColorArray && mColorArray->gpuBuffer()->handle())
    VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

  VL_CHECK_OGL()

  if (sec_color_on)
  {
    if (vbo_on && mSecondaryColorArray->gpuBuffer()->handle())
    {
      VL_glBindBuffer(GL_ARRAY_BUFFER, mSecondaryColorArray->gpuBuffer()->handle() );
      sec_color_pointer = 0;
    }
    VL_glSecondaryColorPointer( mSecondaryColorArray->glSize(), mSecondaryColorArray->glType(), 0/* mSecondaryColorArray->stride()*/, sec_color_pointer /*+ mSecondaryColorArray->offset()*/ );
    glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
  }
  /*else
    VL_glSecondaryColor3f(1,1,1);*/

  VL_CHECK_OGL()

  if(vbo_on && mSecondaryColorArray && mSecondaryColorArray->gpuBuffer()->handle())
    VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

  if (fog_on)
  {
    if (vbo_on && mFogCoordArray->gpuBuffer()->handle())
    {
      VL_glBindBuffer(GL_ARRAY_BUFFER, mFogCoordArray->gpuBuffer()->handle() );
      fog_pointer = 0;
    }
    VL_CHECK(mFogCoordArray->glSize() == 1);
    VL_glFogCoordPointer( mFogCoordArray->glType(), 0/*mFogCoordArray->stride()*/, fog_pointer /*+ mFogCoordArray->offset()*/ );
    glEnableClientState(GL_FOG_COORD_ARRAY);
  }
  /*else
  if (GLEW_VERSION_1_4)
    glFogCoordf(0);*/

  if(vbo_on && mFogCoordArray && mFogCoordArray->gpuBuffer()->handle())
    VL_glBindBuffer(GL_ARRAY_BUFFER, 0);

  VL_CHECK_OGL()

  for (int i=0; i<mTexCoordArrays.size(); ++i)
  {
    VL_CHECK(mTexCoordArrays[i]->mTexCoordArray)

    #ifndef NDEBUG
      if ( !(mTexCoordArrays[i]->mTextureUnit < opengl_context->textureUnitCount()) )
      {
        Log::error( Say("Geometry: texture array index #%n not supported. Max texture array index is %n.\n") << mTexCoordArrays[i]->mTextureUnit << opengl_context->textureUnitCount()-1 );
        continue;
      }
    #endif

    /*VL_CHECK(mTexCoordArrays[i]->mTextureUnit < opengl_context->textureUnitCount())*/
    VL_CHECK(mTexCoordArrays[i]->mTextureUnit < VL_MAX_TEXTURE_UNIT_COUNT)    

    unsigned char* tex_pointer = mTexCoordArrays[i]->mTexCoordArray->ptr();

    VL_glClientActiveTexture(GL_TEXTURE0 + mTexCoordArrays[i]->mTextureUnit);

    if (vbo_on && mTexCoordArrays[i]->mTexCoordArray->gpuBuffer()->handle())
    {
      VL_glBindBuffer(GL_ARRAY_BUFFER, mTexCoordArrays[i]->mTexCoordArray->gpuBuffer()->handle());
      tex_pointer = 0;
    }

    glTexCoordPointer(mTexCoordArrays[i]->mTexCoordArray->glSize(), mTexCoordArrays[i]->mTexCoordArray->glType(), 0/*mTexCoordArrays[i]->mTexCoordArray->stride()*/, tex_pointer /*+ mTexCoordArrays[i]->mTexCoordArray->offset()*/ );
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if(vbo_on && mTexCoordArrays[i]->mTexCoordArray && mTexCoordArrays[i]->mTexCoordArray->gpuBuffer()->handle())
      VL_glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  VL_CHECK_OGL()

  // ==== A C T U A L   D R A W

  for(int i=0; i<(int)primitives()->size(); i++)
    if (primitives()->at(i)->isEnabled())
      primitives()->at(i)->render( vbo_on );

  VL_CHECK_OGL()

  // ==== D I S A B L E   S T A T E S

  glDisableClientState(GL_VERTEX_ARRAY);

  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    VL_glDisableVertexAttribArray( vertexAttributeArrays()->at(i)->attribIndex() );

  VL_CHECK_OGL()

  if (normal_on)
    glDisableClientState(GL_NORMAL_ARRAY);

  if (color_on)
    glDisableClientState(GL_COLOR_ARRAY);

  if (sec_color_on)
    glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

  if (fog_on)
    glDisableClientState(GL_FOG_COORD_ARRAY);

  VL_CHECK_OGL()

  for (int i=0; i<mTexCoordArrays.size(); ++i)
  {
    VL_glClientActiveTexture(GL_TEXTURE0 + mTexCoordArrays[i]->mTextureUnit);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  VL_CHECK_OGL()
}
//-----------------------------------------------------------------------------
void Geometry::transform(const mat4& m, bool normalize)
{
  if (vertexArray())
    vertexArray()->transform(m);

  if (normalArray())
  {
    mat4 nmat = m;
    nmat.setT(vec3(0,0,0)); // keep only the upper 3x3
    nmat = nmat.inverse().transpose();
    normalArray()->transform(nmat);
    if (normalize)
      normalArray()->normalize();
  }
}
//-----------------------------------------------------------------------------
void Geometry::setVertexAttributeArray(const VertexAttributeArray& info)
{
  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
  {
    VL_CHECK(vertexAttributeArrays()->at(i))
    if (vertexAttributeArrays()->at(i)->attribIndex() == info.attribIndex())
    {
      *vertexAttributeArrays()->at(i) = info;
      return;
    }
  }
  mVertexAttributeArrays.push_back( new VertexAttributeArray(info) );
}
//-----------------------------------------------------------------------------
const VertexAttributeArray* Geometry::findVertexAttribute(unsigned int name) const
{
  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    if (vertexAttributeArrays()->at(i)->attribIndex() == name)
      return vertexAttributeArrays()->at(i);
  return NULL;
}
//-----------------------------------------------------------------------------
VertexAttributeArray* Geometry::findVertexAttribute(unsigned int name)
{
  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    if (vertexAttributeArrays()->at(i)->attribIndex() == name)
      return vertexAttributeArrays()->at(i);
  return NULL;
}
//-----------------------------------------------------------------------------
void Geometry::eraseVertexAttributeByName(unsigned int name)
{
  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    if (vertexAttributeArrays()->at(i)->attribIndex() == name)
    {
      vertexAttributeArrays()->eraseAt(i);
      return;
    }
}
//-----------------------------------------------------------------------------
void Geometry::shrinkDrawElements()
{
  for(int i=0; i<primitives()->size(); ++i)
  {
    ref<DrawElementsUInt>   de_uint   = dynamic_cast<DrawElementsUInt*>( primitives()->at(i) );
    ref<DrawElementsUShort> de_ushort = dynamic_cast<DrawElementsUShort*>( primitives()->at(i) );
    size_t max_index = 0;
    if (de_uint)
    {
      for(size_t idx=0; idx<de_uint->indexCount(); ++idx)
        max_index = de_uint->index(idx) > max_index ? de_uint->index(idx) : max_index;

      if (max_index < 256)
      {
        ref<DrawElementsUByte> de_ubyte = new DrawElementsUByte;
        de_ubyte->setPrimitiveType( de_uint->primitiveType() );
        de_ubyte->setInstances( de_uint->instances() );
        de_ubyte->setEnabled( de_uint->isEnabled() );
        de_ubyte->indices()->resize( de_uint->indexCount() );

        for(size_t idx=0; idx<de_uint->indexCount(); ++idx)
        {
          de_ubyte->indices()->at(idx) = (unsigned char)de_uint->index(idx);
          VL_CHECK( de_ubyte->indices()->at(idx) == de_uint->index(idx))
          VL_CHECK( de_ubyte->indices()->at(idx) == de_uint->index(idx))
        }
        primitives()->set( i, de_ubyte.get() );
      }
      else
      if (max_index < 65536)
      {
        de_ushort = new DrawElementsUShort;
        de_ushort->setPrimitiveType( de_uint->primitiveType() );
        de_ushort->setInstances( de_uint->instances() );
        de_ushort->setEnabled( de_uint->isEnabled() );
        de_ushort->indices()->resize( de_uint->indexCount() );

        for(size_t idx=0; idx<de_uint->indexCount(); ++idx)
        {
          de_ushort->indices()->at(idx) = (GLushort)de_uint->index(idx);
          VL_CHECK( de_ushort->indices()->at(idx) == de_uint->index(idx))
          VL_CHECK( de_ushort->indices()->at(idx) == de_uint->index(idx))
        }
        primitives()->set( i, de_ushort.get() );
      }
    }
    if (de_ushort)
    {
      for(size_t idx=0; idx<de_ushort->indexCount(); ++idx)
        max_index = de_ushort->index(idx) > max_index ? de_ushort->index(idx) : max_index;
      if (max_index < 256)
      {
        ref<DrawElementsUByte> de_ubyte = new DrawElementsUByte;
        de_ubyte->setPrimitiveType( de_uint->primitiveType() );
        de_ubyte->setInstances( de_uint->instances() );
        de_ubyte->setEnabled( de_uint->isEnabled() );
        de_ubyte->indices()->resize( de_ushort->indexCount() );

        for(size_t idx=0; idx<de_ushort->indexCount(); ++idx)
          de_ubyte->indices()->at(idx) = (unsigned char)de_ushort->index(idx);
        primitives()->set( i, de_ubyte.get() );
      }
    }
  }
}
//-----------------------------------------------------------------------------
void Geometry::mergeTriangleStrips()
{
  std::vector< ref<Primitives> > de;
  std::vector<size_t> indices;

  // collect DrawElements
  for(int i=primitives()->size(); i--; )
  {
    ref<DrawElementsUInt>   de_uint   = dynamic_cast<DrawElementsUInt*>( primitives()->at(i) );
    ref<DrawElementsUShort> de_ushort = dynamic_cast<DrawElementsUShort*>( primitives()->at(i) );
    ref<DrawElementsUByte>  de_ubyte  = dynamic_cast<DrawElementsUByte*>( primitives()->at(i) );
    if (de_uint && de_uint->primitiveType() == PT_TRIANGLE_STRIP)
    {
      de.push_back(de_uint);
      primitives()->erase(i,1);
    }
    if (de_ubyte && de_ubyte->primitiveType() == PT_TRIANGLE_STRIP)
    {
      de.push_back(de_ubyte);
      primitives()->erase(i,1);
    }
    if (de_ushort && de_ushort ->primitiveType() == PT_TRIANGLE_STRIP)
    {
      de.push_back(de_ushort);
      primitives()->erase(i,1);
    }
  }

  // generate new strip
  indices.reserve( vertexArray()->size()*2 );
  for(size_t i=0; i<de.size(); ++i)
  {
    if (!de[i]->indexCount())
      continue;
    for(size_t j=0; j<de[i]->indexCount(); ++j)
      indices.push_back(de[i]->index(j));
    // odd -> even
    if ( de[i]->indexCount() % 2 )
      indices.push_back(indices.back());
    // concatenate next strip inserting degenerate triangles
    if ( i != de.size()-1 )
    {
      indices.push_back(indices.back());
      indices.push_back(de[i+1]->index(0));
      indices.push_back(de[i+1]->index(0));
      indices.push_back(de[i+1]->index(1));
    }
  }

  if (indices.size())
  {
    ref<DrawElementsUInt> draw_elems = new DrawElementsUInt(PT_TRIANGLE_STRIP);
    draw_elems->indices()->resize(indices.size());
    memcpy(draw_elems->indices()->ptr(), &indices[0], sizeof(unsigned int)*indices.size());
    primitives()->push_back(draw_elems.get());
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

  for(int itex=0; itex<VL_MAX_TEXTURE_UNIT_COUNT; ++itex)
    if (texCoordArray(itex))
      setTexCoordArray( itex, mapper.regenerate( texCoordArray(itex), map_new_to_old ).get() );

  for(int i=0; i<vertexAttributeArrays()->size(); ++i)
    vertexAttributeArrays()->at(i)->setData( mapper.regenerate(vertexAttributeArrays()->at(i)->data(), map_new_to_old ).get() );
}
//-----------------------------------------------------------------------------
void Geometry::convertPrimitivesToDrawArrays()
{
  // generate mapping 
  std::vector<size_t> map_new_to_old;
  map_new_to_old.reserve( vertexArray()->size() * 3 );
  //map_new_to_old.resize(vertexArray()->size());
  //memset(&map_new_to_old[0],0xFF,sizeof(unsigned int)*map_new_to_old.size());
  for(int i=primitives()->size(); i--; )
  {
    int start = map_new_to_old.size();
    for(unsigned int idx=0; idx<primitives()->at(i)->indexCount(); ++idx)
      map_new_to_old.push_back( primitives()->at(i)->index(idx) );

    // substitute with DrawArrays
    ref<DrawArrays> da = new vl::DrawArrays( primitives()->at(i)->primitiveType(), start, primitives()->at(i)->indexCount(), primitives()->at(i)->instances() );
    primitives()->erase(i,1);
    primitives()->push_back(da.get());
  }

  regenerateVertices(map_new_to_old);
}
//-----------------------------------------------------------------------------
void Geometry::sortTriangles()
{
  for(int i=0; i<primitives()->size(); ++i)
    primitives()->at(i)->sortTriangles();
}
//-----------------------------------------------------------------------------
bool Geometry::sortVertices()
{
  // works only if the primitive types are all DrawElements
  std::vector< ref<DrawElementsUInt> >   de_uint;

  // collect DrawElements
  for(int i=0; i<primitives()->size(); ++i)
  {
    DrawElementsUInt*   dei = dynamic_cast<DrawElementsUInt*>(primitives()->at(i));
    DrawElementsUShort* des = dynamic_cast<DrawElementsUShort*>(primitives()->at(i));
    DrawElementsUByte*  deb = dynamic_cast<DrawElementsUByte*>(primitives()->at(i));
    if (dei)
      de_uint.push_back(dei);
    else
    if(des)
    {
      dei = new DrawElementsUInt(des->primitiveType(), des->instances());
      de_uint.push_back(dei);
      dei->indices()->resize( des->indexCount() );
      for(unsigned int j=0; j<des->indexCount(); ++j)
        dei->indices()->at(j) = des->index(j);
    }
    else
    if(deb)
    {
      dei = new DrawElementsUInt(deb->primitiveType(), deb->instances());
      de_uint.push_back(dei);
      dei->indices()->resize( deb->indexCount() );
      for(unsigned int j=0; j<deb->indexCount(); ++j)
        dei->indices()->at(j) = deb->index(j);
    }
    else
      return false;
  }

  primitives()->clear();

  // generate mapping 
  std::vector<size_t> map_new_to_old;
  map_new_to_old.resize( vertexArray()->size() );
  memset(&map_new_to_old[0], 0xFF, map_new_to_old.size()*sizeof(size_t));

  std::vector<size_t> map_old_to_new;
  map_old_to_new.resize( vertexArray()->size() );
  memset(&map_old_to_new[0], 0xFF, map_old_to_new.size()*sizeof(size_t));

  std::vector<size_t> used;
  used.resize( vertexArray()->size() );
  memset(&used[0], 0, used.size()*sizeof(size_t));

  size_t index = 0;
  for(int i=de_uint.size(); i--; )
  {
    for(size_t idx=0; idx<de_uint[i]->indexCount(); ++idx)
      if (!used[de_uint[i]->index(idx)])
      {
        map_new_to_old[index] = de_uint[i]->index(idx);
        map_old_to_new[de_uint[i]->index(idx)] = index;
        index++;
        used[de_uint[i]->index(idx)] = 1;
      }
  }

  regenerateVertices(map_new_to_old);

  // remap DrawElements
  for(size_t i=0; i<de_uint.size(); ++i)
  {
    primitives()->push_back(de_uint[i].get());
    for(size_t j=0; j<de_uint[i]->indexCount(); ++j)
    {
      de_uint[i]->indices()->at(j) = map_old_to_new[de_uint[i]->indices()->at(j)];
    }
  }

  return true;
}
//-----------------------------------------------------------------------------
void Geometry::colorizePrimitives()
{
  ref<ArrayFVec4> col = new vl::ArrayFVec4;
  col->resize( vertexArray()->size() );
  setColorArray( col.get() );

  for(int i=0; i<primitives()->size(); ++i)
  {
    fvec4 c;
    c.r() = rand()%100 / 100.0f;
    c.g() = rand()%100 / 100.0f;
    c.b() = rand()%100 / 100.0f;
    c.a() = 1.0f;

    for(unsigned int j=0; j<primitives()->at(i)->indexCount(); ++j)
      col->at( primitives()->at(i)->index(j) ) = c;
  }
}
//-----------------------------------------------------------------------------
void Geometry::computeTangentSpace(
  size_t vert_count, 
  const fvec3 *vertex, 
  const fvec3* normal,
  const fvec2 *texcoord, 
  const Primitives* primitives,
  fvec3 *tangent, 
  fvec3 *bitangent )
{
    std::vector<fvec3> tan1;
    std::vector<fvec3> tan2;
    tan1.resize(vert_count);
    tan2.resize(vert_count);
    
    size_t tri_count = primitives->triangleCount() / primitives->instances();
    for ( size_t a = 0; a < tri_count; ++a )
    {
        unsigned int tri[3];
        primitives->getTriangle( a, tri );

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
