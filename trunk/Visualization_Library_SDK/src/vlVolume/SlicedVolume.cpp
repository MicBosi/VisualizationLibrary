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
 * An actor encapsulating all the geometry and effects needed to directly render a volume using viewport aligned slices.
 *
 * Pictures from the \ref pagGuideSlicedVolume "Volume Rendering" tutorial.
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
 * Using the SlicedVolume class is extremely simple, see below. See also the App_VolumeRendering.hpp example.
 *
 * By default the constructor installs the following shaders \p /glsl/volume_luminance_light.fs 
 * and \p /glsl/volume_luminance_light.vs and sets the \p val_threshold uniform variable to 0.5f.
 *
 * You can change the GLSLProgram settings in the following way:
 * \code
 * glslProgram()->detachAllShaders();
 * glslProgram()->attachShader( new vl::GLSLFragmentShader("/mypath/my_fragment_shader.fs") );
 * glslProgram()->attachShader( new vl::GLSLVertexShader("/mypath/my_vertex_shader.fs") );
 * glslProgram()->gocUniform("my_uniform")->setUniform(value);
 * \endcode
 *
 * You can also reimplement the updateUniforms() method to update your own uniform variables before every rendering frame.
 *
 * \par Using an IF_RGBA volume (no GLSL)
 *
 * \code
 * vl::ref<vlVolume::SlicedVolume> volume = new SlicedVolume;
 * // loads an IF_RGBA volume (no transfer function is needed in this case)
 * vl::ref<vl::Image> rgba_img = vl::loadImage("myvolume.dat"); 
 * // installs the IF_RGBA volume
 * volume->setVolumeImage(rgba_img.get());
 * // disable GLSL shader
 * volume->setTransferFunction(NULL);
 * \endcode
 *
 * \par Generating an IF_RGBA volume from an IF_LUMINANCE volume + transfer function (no GLSL)
 *
 * \code
 * vl::ref<vlVolume::SlicedVolume> volume = new SlicedVolume;
 * // loads an IF_LUMINANCE volume
 * vl::ref<vl::Image> img      = vl::loadImage("myvolume.dat"); 
 * // generates a transfer function
 * vl::ref<vl::Image> trfunc   = vl::Image::makeColorSpectrum(128, vlut::black, vlut::blue, vlut::green, vlut::yellow, vlut::red);
 * // computes an IF_RGBA volume using the transfer function and with lighting
 * vl::ref<vl::Image> rgba_img = vlVolume::SlicedVolume::genRGBAVolume(img.get(), trfunc.get(), vl::fvec3(1.0f,1.0f,0.0f));
 * // installs the IF_RGBA volume
 * volume->setVolumeImage(rgba_img.get());
 * // disable GLSL shader
 * volume->setTransferFunction(NULL);
 * \endcode
 *
 * \par Using IF_LUMINANCE volume + transfer function with GLSL (with dynamic Blinn-Phong lighting)
 *
 * \code
 * vl::ref<vlVolume::SlicedVolume> volume = new SlicedVolume;
 * // generates and install a transfer function to be used with an IF_LUMINANCE volume.
 * vl::ref<vl::Image> trfunc = vl::Image::makeColorSpectrum(128, vlut::black, vlut::blue, vlut::green, vlut::yellow, vlut::red);
 * // setTransferFunction() also enables the GLSL shader
 * volume->setTransferFunction(trfunc.get());
 * // loads and install an IF_LUMINANCE volume.
 * vl::ref<vl::Image> img = vl::loadImage("myvolume.dat");
 * volume->setVolumeImage(img.get());
 * \endcode
 */
//-----------------------------------------------------------------------------
//! Constructor.
SlicedVolume::SlicedVolume()
{
  mSliceCount = 1024;
  mGeometry = new Geometry;
  mLight    = new Light(0);
  
  fvec3 texc[] = 
  {
    fvec3(0,0,0), fvec3(1,0,0), fvec3(1,1,0), fvec3(0,1,0),
    fvec3(0,0,1), fvec3(1,0,1), fvec3(1,1,1), fvec3(0,1,1)
  };
  memcpy(mTexCoord, texc, sizeof(texc));
}
//-----------------------------------------------------------------------------
/**
 * You can reimplement this function in order to setup your own uniform variable for your GLSLProgram.
 * The default implementation of this function updates the following two uniforms:
 * - \p light_position Light position in object space, used to compute Blinn-Phong lighting in \p /glsl/volume_luminance_light.fs
 * - \p eye_position Camera position in object space, used to compute Blinn-Phong lighting in \p /glsl/volume_luminance_light.fs
 */
void SlicedVolume::updateUniforms(const Camera* camera, Actor* actor)
{
  // light position
  fvec3 lpos;
  if (light()->followedTransform())
    lpos = (fmat4)light()->followedTransform()->worldMatrix() * light()->position().xyz();
  else
    lpos = ((fmat4)camera->inverseViewMatrix() * light()->position()).xyz();

  // world to object space
  if (actor->transform())
    lpos = (fmat4)actor->transform()->worldMatrix().getInverse() * lpos;
  actor->gocUniform("light_position")->setUniform(lpos);

  // eye postion
  fvec3 epos = (fvec3)camera->inverseViewMatrix().getT();
  // world to object space
  if (actor->transform())
    epos = (fmat4)actor->transform()->worldMatrix().getInverse() * epos;
  actor->gocUniform("eye_position")->setUniform(epos);
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
void SlicedVolume::bindActor(Actor* actor)
{
  actor->actorEventCallbacks()->push_back( this );
  actor->lod(0) = mGeometry;
}
void SlicedVolume::onActorRenderStarted(Actor* actor, Real, const Camera* camera, Renderable*, const Shader* shader, int pass)
{
  if (pass>0)
    return;

  // setup uniform variables

  if (shader->getGLSLProgram())
    updateUniforms(camera, actor);

  // setup geometry

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
  ref<DrawArrays> da = new DrawArrays(vl::PT_TRIANGLES, 0, polygons.size());
  mGeometry->drawCalls()->push_back( da.get() );
  ref<ArrayFVec3> vertex_array = new ArrayFVec3;
  ref<ArrayFVec3> texcoo_array = new ArrayFVec3;
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
//! This function also generates a 3D texture based on the given image and generates an appropriate set of texture coordinates.
void SlicedVolume::setVolumeImage(Image* img, Shader* shader)
{
  shader->gocTextureUnit(0)->setTexture( new vl::Texture( img ) );
  generateTextureCoordinates( img->width(), img->height(), img->depth() );
  if (shader->getGLSLProgram())
    shader->getGLSLProgram()->gocUniform("gradient_delta")->setUniform(fvec3(0.5f/img->width(), 0.5f/img->height(), 0.5f/img->depth()));
}
//-----------------------------------------------------------------------------
ref<Image> SlicedVolume::genRGBAVolume(Image* data, Image* trfunc, const fvec3& light_dir, bool alpha_from_data)
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
ref<Image> SlicedVolume::genRGBAVolume(Image* data, Image* trfunc, bool alpha_from_data)
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
ref<Image> SlicedVolume::genRGBAVolumeT(Image* data, Image* trfunc, const fvec3& light_dir, bool alpha_from_data)
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
  unsigned char* lum_px = data->pixels();
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
ref<Image> SlicedVolume::genRGBAVolumeT(Image* data, Image* trfunc, bool alpha_from_data)
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
  unsigned char* lum_px = data->pixels();
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
void SlicedVolume::setBox(const AABB& box) 
{
  mBox = box; 
  mCache = 0; 
  mGeometry->setBoundingBox( box );
  mGeometry->setBoundingSphere( box );
  mGeometry->setBoundsDirty(true);
}
//-----------------------------------------------------------------------------
