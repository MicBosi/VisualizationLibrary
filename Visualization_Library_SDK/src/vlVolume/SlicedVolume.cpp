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

#include <vlVolume/SlicedVolume.hpp>
#include <vl/GLSL.hpp>
#include <vl/Camera.hpp>
#include <vl/Time.hpp>

using namespace vl;
using namespace vlVolume;

/** \class vlVolume::SlicedVolume
 * A vl::ActorEventCallback used to render a volume using viewport aligned slices.
 *
 * Pictures from: \ref pagGuideSlicedVolume tutorial.
 * <center>
 * <table border=0 cellspacing=0 cellpadding=5>
 * <tr>
 * 	<td> \image html pagGuideSlicedVolume.jpg </td>
 * 	<td> \image html pagGuideSlicedVolume_3.jpg </td>
 * </tr>
 * </table>
 * <table border=0 cellspacing=0 cellpadding=5>
 * <tr>
 * 	<td> \image html pagGuideSlicedVolume_1.jpg </td>
 * 	<td> \image html pagGuideSlicedVolume_2.jpg </td>
 * 	<td> \image html pagGuideSlicedVolume_4.jpg </td>
 * </tr>
 * <tr>
 * 	<td> \image html pagGuideSlicedVolume_5.jpg </td>
 * 	<td> \image html pagGuideSlicedVolume_6.jpg </td>
 * 	<td> \image html pagGuideSlicedVolume_7.jpg </td>
 * </tr>
 * </table>
 * </center>
 *
 * Using the SlicedVolume class is very simple, see \a App_VolumeRendering.cpp for a practical example.
 *
 * Basically all SlicedVolume does is to compute the correct texture coordinates and generates on the fly N viewport
 * aligned slices, where N can be specified by the user with setSliceCount(). Such slices then can be rendered using
 * a GLSLProgram specified by the user, thus you can achieve virtually any kind of visual effect with it.
 *
 * The user can reimplement the updateUniforms() method to update his own uniform variables before the volume is rendered.
 * By default updateUniforms() updates the position of up to 4 lights in object space. Such positions are stored in the
 * \p "uniform vec3 light_position[4]" variable.
 * The updateUniforms() method also fills the \p "uniform bool light_enable[4]" variable with a flag marking if the Nth 
 * light is active or not. These light values are computed based on the lights bound to the current Shader.
 * The updateUniforms() method also fills the \p "uniform vec3 eye_position" variable which contains the camera position in
 * object space, useful to compute specular highlights etc.
 *
 * For a complete example see: \ref pagGuideSlicedVolume.
 */
//-----------------------------------------------------------------------------
//! Constructor.
SlicedVolume::SlicedVolume()
{
  mSliceCount = 1024;
  mGeometry = new Geometry;
  
  fvec3 texc[] = 
  {
    fvec3(0,0,0), fvec3(1,0,0), fvec3(1,1,0), fvec3(0,1,0),
    fvec3(0,0,1), fvec3(1,0,1), fvec3(1,1,1), fvec3(0,1,1)
  };
  memcpy(mTexCoord, texc, sizeof(texc));
}
//-----------------------------------------------------------------------------
/** Reimplement this method to update his own uniform variables of your GLSL program before the volume is rendered.
 * By default updateUniforms() updates the position of up to 4 lights in object space. Such positions are stored in the
 * \p "uniform vec3 light_position[4]" variable.
 * The updateUniforms() method also fills the \p "uniform bool light_enable[4]" variable with a flag marking if the Nth 
 * light is active or not. These light values are computed based on the lights bound to the current Shader.
 * The updateUniforms() method also fills the \p "uniform vec3 eye_position" variable which contains the camera position in
 * object space, useful to compute specular highlights etc.
 */
void SlicedVolume::updateUniforms(vl::Actor*actor, vl::Real, const vl::Camera* camera, vl::Renderable*, const vl::Shader* shader)
{
  // computes up to 4 light positions (in object space) and enables

  int light_enable[4] = { 0,0,0,0 };
  fvec3 light_position[4];

  for(int i=0; i<4; ++i)
  {
    const vl::Light* light = shader->getLight(i);
    light_enable[i] = light != NULL;
    if (light)
    {
      // light position following transform
      if (light->followedTransform())
        light_position[i] = (fmat4)light->followedTransform()->worldMatrix() * light->position().xyz();
      // light position following camera
      else
        light_position[i] = ((fmat4)camera->inverseViewMatrix() * light->position()).xyz();

      // light position in object space
      if (actor->transform())
        light_position[i] = (fmat4)actor->transform()->worldMatrix().getInverse() * light_position[i];
    }
  }

  actor->gocUniform("light_position")->setUniform(4, light_position);
  actor->gocUniform("light_enable")->setUniform1i(4, light_enable);

  // compute eye position in object space

  // eye postion
  fvec3 eye = (fvec3)camera->inverseViewMatrix().getT();
  // world to object space
  if (actor->transform())
    eye = (fmat4)actor->transform()->worldMatrix().getInverse() * eye;
  actor->gocUniform("eye_position")->setUniform(eye);
}
//-----------------------------------------------------------------------------
namespace
{
  class Edge
  {
  public:
    int v0, v1, intersection, flags;
    bool operator<(const Edge& other) const
    {
      return intersection > other.intersection;
    }
  };
}
//-----------------------------------------------------------------------------
void SlicedVolume::bindActor(Actor* actor)
{
  actor->actorEventCallbacks()->push_back( this );
  actor->lod(0) = mGeometry;
}
//-----------------------------------------------------------------------------
void SlicedVolume::onActorRenderStarted(Actor* actor, Real clock, const Camera* camera, Renderable* rend, const Shader* shader, int pass)
{
  if (pass>0)
    return;

  // setup uniform variables

  if (shader->getGLSLProgram())
    updateUniforms(actor, clock, camera, rend, shader);

  // setup geometry: generate viewport aligned slices

  // skip generation is actor and camera did not move
  fmat4 mat;
  if (actor->transform())
    mat = (fmat4)(camera->viewMatrix() * actor->transform()->worldMatrix());
  else
    mat = (fmat4)camera->viewMatrix();

  if (mCache == mat)
    return;
  else
    mCache = mat;

  fmat4 imat = mat.getInverse();

  fvec3 cube_verts[] =
  {
    fvec3((float)box().minCorner().x(), (float)box().minCorner().y(), (float)box().minCorner().z()),
    fvec3((float)box().maxCorner().x(), (float)box().minCorner().y(), (float)box().minCorner().z()),
    fvec3((float)box().maxCorner().x(), (float)box().maxCorner().y(), (float)box().minCorner().z()),
    fvec3((float)box().minCorner().x(), (float)box().maxCorner().y(), (float)box().minCorner().z()),
    fvec3((float)box().minCorner().x(), (float)box().minCorner().y(), (float)box().maxCorner().z()),
    fvec3((float)box().maxCorner().x(), (float)box().minCorner().y(), (float)box().maxCorner().z()),
    fvec3((float)box().maxCorner().x(), (float)box().maxCorner().y(), (float)box().maxCorner().z()),
    fvec3((float)box().minCorner().x(), (float)box().maxCorner().y(), (float)box().maxCorner().z())
  };

  int min_idx = 0;
  int max_idx = 0;
  for(int i=0; i<8; ++i)
  {
    cube_verts[i] = mat * cube_verts[i];
    if (fabs(cube_verts[i].z()) < fabs(cube_verts[min_idx].z())) min_idx = i;
    if (fabs(cube_verts[i].z()) > fabs(cube_verts[max_idx].z())) max_idx = i;
  }

  if (cube_verts[min_idx].z() > 0)
  {
    // fixme?
    // the actor is not visible: remove the geometry or disable the actor?
    // return;
  }

  const int TOP    = 1;
  const int BOTTOM = 2;
  const int LEFT   = 4;
  const int RIGHT  = 8;
  const int FRONT  = 16;
  const int BACK   = 32;

  Edge edges[] =
  {
    {0,1,-1,FRONT |BOTTOM}, {1,2,-1,FRONT|RIGHT}, {2,3,-1,FRONT|TOP},  {3,0,-1,FRONT |LEFT},
    {4,5,-1,BACK  |BOTTOM}, {5,6,-1,BACK |RIGHT}, {6,7,-1,BACK |TOP},  {7,4,-1,BACK  |LEFT},
    {1,5,-1,BOTTOM|RIGHT},  {2,6,-1,TOP  |RIGHT}, {3,7,-1,TOP  |LEFT}, {0,4,-1,BOTTOM|LEFT}
  };

  std::vector<fvec3> points;
  std::vector<fvec3> points_t;
  std::vector<fvec3> polygons;
  std::vector<fvec3> polygons_t;

  polygons.reserve(sliceCount()*5);
  polygons_t.reserve(sliceCount()*5);
  float zrange = cube_verts[max_idx].z() - cube_verts[min_idx].z();
  float zstep  = zrange/(sliceCount()+1);
  int vert_idx[12];
  for(int islice=0; islice<sliceCount(); ++islice)
  {
    float z = cube_verts[max_idx].z() - zstep*(islice+1);
    fvec3 plane_o(0,0,z);
    fvec3 plane_n(0,0,1.0f);
    points.clear();
    points_t.clear();
    for(int iedge=0; iedge<12; ++iedge)
    {
      edges[iedge].intersection = -1;
      fvec3 vi  = cube_verts[ edges[iedge].v0 ];
      fvec3 eij = cube_verts[ edges[iedge].v1 ] - cube_verts[ edges[iedge].v0 ];
      float denom = dot(plane_n,eij);
      if (denom == 0)
        continue;
      float lambda = (z - dot(plane_n,vi))/denom;
      if (lambda<0 || lambda>1)
        continue;
      fvec3 v = vi + eij*lambda;
      edges[iedge].intersection = (int)points.size();
      points.push_back(v);
      fvec3 a = texCoords()[ edges[iedge].v0 ];
      fvec3 b = texCoords()[ edges[iedge].v1 ] - texCoords()[ edges[iedge].v0 ];
      fvec3 vt = a + b*lambda;
      points_t.push_back(vt);
    }
    std::sort(edges, edges+12);
    int vert_idx_c = 0;
    for(int ie0=0; ie0<12-1; ++ie0)
    {
      if (edges[ie0].intersection == -1)
        break;
      vert_idx[vert_idx_c++] = edges[ie0].intersection;
      for(int ie1=ie0+1; ie1<12; ++ie1)
      {
        if (edges[ie1].intersection == -1)
          continue;
        if( (edges[ie0].flags & edges[ie1].flags) )
        {
          Edge t       = edges[ie0+1];
          edges[ie0+1] = edges[ie1];
          edges[ie1]   = t;
          break;
        }
      }
    }
    for(int vc=0; vc<vert_idx_c-2; ++vc)
    {
      polygons.push_back(imat*points  [vert_idx[0]]);
      polygons.push_back(imat*points  [vert_idx[vc+1]]);
      polygons.push_back(imat*points  [vert_idx[vc+2]]);
      polygons_t.push_back(points_t[vert_idx[0]]);
      polygons_t.push_back(points_t[vert_idx[vc+1]]);
      polygons_t.push_back(points_t[vert_idx[vc+2]]);
    }
    #ifndef NDEBUG
      for(int ie0=0; ie0<12-1; ++ie0)
      {
        if (edges[ie0].intersection == -1)
          break;
        if (edges[ie0+1].intersection == -1)
          break;
        VL_CHECK(edges[ie0].flags & edges[ie0+1].flags)
      }
    #endif
  }

  mGeometry->drawCalls()->clear();
  ref<DrawArrays> da = new DrawArrays(vl::PT_TRIANGLES, 0, (int)polygons.size());
  mGeometry->drawCalls()->push_back( da.get() );
  ref<ArrayFloat3> vertex_array = new ArrayFloat3;
  ref<ArrayFloat3> texcoo_array = new ArrayFloat3;
  vertex_array->resize(polygons.size());
  texcoo_array->resize(polygons_t.size());
  VL_CHECK((size_t)vertex_array->gpuBuffer()->bytesUsed() == sizeof(polygons  [0])*polygons.  size());
  VL_CHECK((size_t)texcoo_array->gpuBuffer()->bytesUsed() == sizeof(polygons_t[0])*polygons_t.size());
  memcpy(vertex_array->ptr(), &polygons  [0], vertex_array->gpuBuffer()->bytesUsed());
  memcpy(texcoo_array->ptr(), &polygons_t[0], texcoo_array->gpuBuffer()->bytesUsed());
  mGeometry->setVertexArray(vertex_array.get());
  mGeometry->setTexCoordArray(0,texcoo_array.get());

  mGeometry->setDisplayListDirty(true);
  mGeometry->setVBODirty(true);

  // fixme: 
  // it seems we have some problems with camera clipping/culling when the camera is close to the volume: the slices disappear or degenerate.
  // it does not seem to depend from camera clipping plane optimization.
}
//-----------------------------------------------------------------------------
/** The texture coordinate generated by this function should be used with TexParameter wrap mode vl::TPW_REPEAT, ie:
\code
vl::ref<vl::Texture> texture = new vl::Texture;
texture->getTexParameter()->setWrapS(vl::TPW_REPEAT);
texture->getTexParameter()->setWrapT(vl::TPW_REPEAT);
texture->getTexParameter()->setWrapR(vl::TPW_REPEAT);
\endcode
*/
void SlicedVolume::generateTextureCoordinates(const ivec3& size)
{
  if (!size.x() || !size.y() || !size.z())
  {
    Log::error("SlicedVolume::generateTextureCoordinates(): failed! The size passed does not represent a 3D image.\n");
    return;
  }
  float dx = 0.5f/size.x();
  float dy = 0.5f/size.y();
  float dz = 0.5f/size.z();
  float x0 = 0.0f + dx;
  float x1 = 1.0f - dx;
  float y0 = 0.0f + dy;
  float y1 = 1.0f - dy;
  float z0 = 0.0f + dz;
  float z1 = 1.0f - dz;
  fvec3 texc[] = 
  {
    fvec3(x0,y0,z0), fvec3(x1,y0,z0), fvec3(x1,y1,z0), fvec3(x0,y1,z0),
    fvec3(x0,y0,z1), fvec3(x1,y0,z1), fvec3(x1,y1,z1), fvec3(x0,y1,z1)
  };
  memcpy(mTexCoord, texc, sizeof(texc));
}
//-----------------------------------------------------------------------------
void SlicedVolume::setBox(const AABB& box) 
{
  mBox = box; 
  mCache = 0; 
  mGeometry->setBoundingBox( box );
  mGeometry->setBoundingSphere( box );
  mGeometry->setBoundsDirty(true);
}
//-----------------------------------------------------------------------------
ref<Image> SlicedVolume::genRGBAVolume(const Image* data, const Image* trfunc, const fvec3& light_dir, bool alpha_from_data)
{
  ref<Image> img;

  if(data->type() == IT_UNSIGNED_BYTE)
    img = genRGBAVolumeT<unsigned char,IT_UNSIGNED_BYTE>(data,trfunc,light_dir,alpha_from_data);
  else
  if(data->type() == IT_UNSIGNED_SHORT)
    img = genRGBAVolumeT<unsigned short,IT_UNSIGNED_SHORT>(data,trfunc,light_dir,alpha_from_data);
  else
  if(data->type() == IT_FLOAT)
    img = genRGBAVolumeT<float,IT_FLOAT>(data,trfunc,light_dir,alpha_from_data);
  else
    vl::Log::error("SlicedVolume::genRGBAVolume() called with non supported data type().\n");

  return img;
}
//-----------------------------------------------------------------------------
ref<Image> SlicedVolume::genRGBAVolume(const Image* data, const Image* trfunc, bool alpha_from_data)
{
  ref<Image> img;

  if(data->type() == IT_UNSIGNED_BYTE)
    img = genRGBAVolumeT<unsigned char,IT_UNSIGNED_BYTE>(data,trfunc,alpha_from_data);
  else
  if(data->type() == IT_UNSIGNED_SHORT)
    img = genRGBAVolumeT<unsigned short,IT_UNSIGNED_SHORT>(data,trfunc,alpha_from_data);
  else
  if(data->type() == IT_FLOAT)
    img = genRGBAVolumeT<float,IT_FLOAT>(data,trfunc,alpha_from_data);
  else
    vl::Log::error("SlicedVolume::genRGBAVolume() called with non supported data type().\n");

  return img;
}
//-----------------------------------------------------------------------------
template<typename data_type, EImageType img_type>
ref<Image> SlicedVolume::genRGBAVolumeT(const Image* data, const Image* trfunc, const fvec3& light_dir, bool alpha_from_data)
{
  if (!trfunc || !data)
    return NULL;
  if (data->format() != IF_LUMINANCE)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() called with non IF_LUMINANCE data format().\n");
    return NULL;
  }
  if (data->type() != img_type)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() called with invalid data type().\n");
    return NULL;
  }
  if (data->dimension() != ID_3D)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() called with non 3D data.\n");
    return NULL;
  }
  if (trfunc->dimension() != ID_1D)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() transfer function image must be an 1D image.\n");
    return NULL;
  }
  if (trfunc->format() != IF_RGBA)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() transfer function format() must be IF_RGBA.\n");
    return NULL;
  }
  if (trfunc->type() != IT_UNSIGNED_BYTE)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() transfer function format() must be IT_UNSIGNED_BYTE.\n");
    return NULL;
  }

  float normalizer_num = 0;
  switch(data->type())
  {
    case IT_UNSIGNED_BYTE:  normalizer_num = 1.0f/255.0f;   break;
    case IT_UNSIGNED_SHORT: normalizer_num = 1.0f/65535.0f; break;
    case IT_FLOAT:          normalizer_num = 1.0f;          break;
    default:
      break;
  }

  // light normalization
  fvec3 L = light_dir;
  L.normalize();
  int w = data->width();
  int h = data->height();
  int d = data->depth();
  int pitch = data->pitch();
  const unsigned char* lum_px = data->pixels();
  // generated volume
  ref<Image> volume = new Image( w, h, d, 1, IF_RGBA, IT_UNSIGNED_BYTE );
  ubvec4* rgba_px = (ubvec4*)volume->pixels();
  for(int z=0; z<d; ++z)
  {
    int z1 = z-1;
    int z2 = z+1;
    z1 = vl::clamp(z1, 0, d-1);
    z2 = vl::clamp(z2, 0, d-1);
    for(int y=0; y<h; ++y)
    {
      int y1 = y-1;
      int y2 = y+1;
      y1 = vl::clamp(y1, 0, h-1);
      y2 = vl::clamp(y2, 0, h-1);
      for(int x=0; x<w; ++x, ++rgba_px)
      {
        // value
        float lum = (*(data_type*)(lum_px + x*sizeof(data_type) + y*pitch + z*pitch*h)) * normalizer_num;
        // value -> transfer function
        float xval = lum*trfunc->width();
        VL_CHECK(xval>=0)
        if (xval > trfunc->width()-1.001f)
          xval = trfunc->width()-1.001f;
        int ix1 = (int)xval;
        int ix2 = ix1+1;
        VL_CHECK(ix2<trfunc->width())
        float w21  = (float)vl::fract(xval);
        float w11  = 1.0f - w21;
        fvec4 c11  = (fvec4)((ubvec4*)trfunc->pixels())[ix1];
        fvec4 c21  = (fvec4)((ubvec4*)trfunc->pixels())[ix2];
        fvec4 rgba = (c11*w11 + c21*w21)*(1.0f/255.0f);

        // bake the lighting
        int x1 = x-1;
        int x2 = x+1;
        x1 = vl::clamp(x1, 0, w-1);
        x2 = vl::clamp(x2, 0, w-1);
        data_type vx1 = (*(data_type*)(lum_px + x1*sizeof(data_type) + y *pitch + z *pitch*h));
        data_type vx2 = (*(data_type*)(lum_px + x2*sizeof(data_type) + y *pitch + z *pitch*h));
        data_type vy1 = (*(data_type*)(lum_px + x *sizeof(data_type) + y1*pitch + z *pitch*h));
        data_type vy2 = (*(data_type*)(lum_px + x *sizeof(data_type) + y2*pitch + z *pitch*h));
        data_type vz1 = (*(data_type*)(lum_px + x *sizeof(data_type) + y *pitch + z1*pitch*h));
        data_type vz2 = (*(data_type*)(lum_px + x *sizeof(data_type) + y *pitch + z2*pitch*h));
        fvec3 N1(float(vx1-vx2), float(vy1-vy2), float(vz1-vz2));
        N1.normalize();
        fvec3 N2 = -N1 * 0.15f;
        float l1 = vl::max(dot(N1,L),0.0f);
        float l2 = vl::max(dot(N2,L),0.0f); // opposite dim light to enhance 3D perception
        rgba.r() = rgba.r()*l1 + rgba.r()*l2+0.2f; // +0.2f = ambient light
        rgba.g() = rgba.g()*l1 + rgba.g()*l2+0.2f;
        rgba.b() = rgba.b()*l1 + rgba.b()*l2+0.2f;
        rgba.r() = vl::clamp(rgba.r(), 0.0f, 1.0f);
        rgba.g() = vl::clamp(rgba.g(), 0.0f, 1.0f);
        rgba.b() = vl::clamp(rgba.b(), 0.0f, 1.0f);

        // map pixel
        rgba_px->r() = (unsigned char)(rgba.r()*255.0f);
        rgba_px->g() = (unsigned char)(rgba.g()*255.0f);
        rgba_px->b() = (unsigned char)(rgba.b()*255.0f);
        if (alpha_from_data)
          rgba_px->a() = (unsigned char)(lum*255.0f);
        else
          rgba_px->a() = (unsigned char)(rgba.a()*255.0f);
      }
    }
  }

  return volume;
}
//-----------------------------------------------------------------------------
template<typename data_type, EImageType img_type>
ref<Image> SlicedVolume::genRGBAVolumeT(const Image* data, const Image* trfunc, bool alpha_from_data)
{
  if (!trfunc || !data)
    return NULL;
  if (data->format() != IF_LUMINANCE)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() called with non IF_LUMINANCE data format().\n");
    return NULL;
  }
  if (data->type() != img_type)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() called with invalid data type().\n");
    return NULL;
  }  
  if (data->dimension() != ID_3D)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() called with non 3D data.\n");
    return NULL;
  }
  if (trfunc->dimension() != ID_1D)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() transfer function image must be an 1D image.\n");
    return NULL;
  }
  if (trfunc->format() != IF_RGBA)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() transfer function format() must be IF_RGBA.\n");
    return NULL;
  }
  if (trfunc->type() != IT_UNSIGNED_BYTE)
  {
    vl::Log::error("SlicedVolume::genRGBAVolume() transfer function format() must be IT_UNSIGNED_BYTE.\n");
    return NULL;
  }

  float normalizer_num = 0;
  switch(data->type())
  {
    case IT_UNSIGNED_BYTE:  normalizer_num = 1.0f/255.0f;   break;
    case IT_UNSIGNED_SHORT: normalizer_num = 1.0f/65535.0f; break;
    case IT_FLOAT:          normalizer_num = 1.0f;          break;
    default:
      break;
  }

  // light normalization
  int w = data->width();
  int h = data->height();
  int d = data->depth();
  int pitch = data->pitch();
  const unsigned char* lum_px = data->pixels();
  // generated volume
  ref<Image> volume = new Image( w, h, d, 1, IF_RGBA, IT_UNSIGNED_BYTE );
  ubvec4* rgba_px = (ubvec4*)volume->pixels();
  for(int z=0; z<d; ++z)
  {
    int z1 = z-1;
    int z2 = z+1;
    z1 = vl::clamp(z1, 0, d-1);
    z2 = vl::clamp(z2, 0, d-1);
    for(int y=0; y<h; ++y)
    {
      int y1 = y-1;
      int y2 = y+1;
      y1 = vl::clamp(y1, 0, h-1);
      y2 = vl::clamp(y2, 0, h-1);
      for(int x=0; x<w; ++x, ++rgba_px)
      {
        // value
        float lum = (*(data_type*)(lum_px + x*sizeof(data_type) + y*pitch + z*pitch*h)) * normalizer_num;
        // value -> transfer function
        float xval = lum*trfunc->width();
        VL_CHECK(xval>=0)
        if (xval > trfunc->width()-1.001f)
          xval = trfunc->width()-1.001f;
        int ix1 = (int)xval;
        int ix2 = ix1+1;
        VL_CHECK(ix2<trfunc->width())
        float w21  = (float)vl::fract(xval);
        float w11  = 1.0f - w21;
        fvec4 c11  = (fvec4)((ubvec4*)trfunc->pixels())[ix1];
        fvec4 c21  = (fvec4)((ubvec4*)trfunc->pixels())[ix2];
        fvec4 rgba = (c11*w11 + c21*w21)*(1.0f/255.0f);

        // map pixel
        rgba_px->r() = (unsigned char)(rgba.r()*255.0f);
        rgba_px->g() = (unsigned char)(rgba.g()*255.0f);
        rgba_px->b() = (unsigned char)(rgba.b()*255.0f);
        if (alpha_from_data)
          rgba_px->a() = (unsigned char)(lum*255.0f);
        else
          rgba_px->a() = (unsigned char)(rgba.a()*255.0f);
      }
    }
  }

  return volume;
}
//-----------------------------------------------------------------------------
vl::ref<vl::Image> SlicedVolume::genGradientNormals(const vl::Image* img)
{
  vl::ref<vl::Image> gradient = new Image;
  gradient->allocate3D(img->width(), img->height(), img->depth(), 1, vl::IF_RGB, vl::IT_FLOAT);
  fvec3* px = (fvec3*)gradient->pixels();
  fvec3 A, B;
  for(int z=0; z<gradient->depth(); ++z)
  {
    for(int y=0; y<gradient->height(); ++y)
    {
      for(int x=0; x<gradient->width(); ++x)
      {
        // clamped coordinates
        int xp = x+1, xn = x-1;
        int yp = y+1, yn = y-1;
        int zp = z+1, zn = z-1;
        if (xn<0) xn = 0;
        if (yn<0) yn = 0;
        if (zn<0) zn = 0;
        if (xp>img->width() -1) xp = img->width() -1;
        if (yp>img->height()-1) yp = img->height()-1;
        if (zp>img->depth() -1) zp = img->depth() -1;

        A.x() = img->sample(xn,y,z).r();
        B.x() = img->sample(xp,y,z).r();
        A.y() = img->sample(x,yn,z).r();
        B.y() = img->sample(x,yp,z).r();
        A.z() = img->sample(x,y,zn).r();
        B.z() = img->sample(x,y,zp).r();

        // write normal packed into 0..1 format
        px[x + img->width()*y + img->width()*img->height()*z] = normalize(A - B) * 0.5f + 0.5f;
      }
    }
  }
  return gradient;
}
//-----------------------------------------------------------------------------
