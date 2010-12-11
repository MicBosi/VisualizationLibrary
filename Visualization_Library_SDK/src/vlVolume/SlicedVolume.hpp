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

#ifndef SlicedVolume_INCLUDE_ONCE
#define SlicedVolume_INCLUDE_ONCE

#include <vl/Actor.hpp>
#include <vl/Geometry.hpp>
#include <vl/Light.hpp>

namespace vlVolume
{
  class SlicedVolume: public vl::ActorEventCallback
  {
  public:
    
    SlicedVolume();
    
    void onActorRenderStarted(vl::Actor* actor, vl::Real frame_clock, const vl::Camera* cam, vl::Renderable* renderable, const vl::Shader* shader, int pass);

    void onActorDelete(vl::Actor* ) {}

    //! Binds a SlicedVolume to an Actor so that the SlicedVolume can generate the viewport aligned slices' geometry for the Actor as appropriate.
    void bindActor(vl::Actor*);

    //! Updates the uniforms used by the vl::GLSLProgram to render the volume each time the onActorRenderStarted() method is called.
    virtual void updateUniforms(vl::Actor* actor, vl::Real clock, const vl::Camera* camera, vl::Renderable* rend, const vl::Shader* shader);
    
    //! Defines the number of slices used to render the volume: more slices generate a better (and slower) rendering.
    void setSliceCount(int count) { mSliceCount = count; }
    
    //! Returns the number of slices used to render the volume.
    int sliceCount() const { return mSliceCount; }
    
    //! Returns the vl::Geometry associated to a SlicedVolume and its bound Actor
    vl::Geometry* geometry() { return mGeometry.get(); }
    
    //! Returns the vl::Geometry associated to a SlicedVolume and its bound Actor
    const vl::Geometry* geometry() const { return mGeometry.get(); }
    
    //! Defines the dimensions of the box enclosing the volume
    void setBox(const vl::AABB& box);
    
    //! The dimensions of the box enclosing the volume
    const vl::AABB& box() const { return mBox; }
    
    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume
    const vl::fvec3* texCoords() const { return mTexCoord; }
    
    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume
    vl::fvec3* texCoords() { return mTexCoord; }
    
    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    void generateTextureCoordinates(const vl::ivec3& size);
    
    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    void generateTextureCoordinates(int width, int height, int depth) { generateTextureCoordinates(vl::ivec3(width,height,depth)); }

    /** Generates an RGBA image based on the given data source and transfer function.
     * \param data The vl::Image used as the volume data source. It must have format() equal to IF_LUMINANCE and type() equal to IT_UNSIGNED_BYTE, IT_UNSIGNED_SHORT or IT_FLOAT.
     * \param trfunc An 1D vl::Image used as transfer function that is used to assign to each value in \p data an RGBA value in the new image.
     * The vl::Image pointed by \p trfunc must mast have type() \p IT_UNSIGNED_BYTE and format() \p IF_RGBA.
     * \param light_dir The direction of the light in object space.
     * \param alpha_from_data If set to true the \p alpha channel of the generated image will be taken from \p data otherwise from the transfer function. */
    static vl::ref<vl::Image> genRGBAVolume(const vl::Image* data, const vl::Image* trfunc, const vl::fvec3& light_dir, bool alpha_from_data=true);

    /** Generates an RGBA image based on the given data source and transfer function.
     * \param data The vl::Image used as the volume data source. It must have format() equal to IF_LUMINANCE and type() equal to IT_UNSIGNED_BYTE, IT_UNSIGNED_SHORT or IT_FLOAT.
     * \param trfunc An 1D vl::Image used as transfer function that is used to assign to each value in \p data an RGBA value in the new image.
     * The vl::Image pointed by \p trfunc must mast have type() \p IT_UNSIGNED_BYTE and format() \p IF_RGBA.
     * \param alpha_from_data If set to true the \p alpha channel of the generated image will be taken from \p data otherwise from the transfer function.
     *
     * Unlike genRGBAVolume(vl::Image* data, vl::Image* trfunc, const vl::fvec3& light_dir, bool alpha_from_data=true) this function does not 
     * compute lighting. */
    static vl::ref<vl::Image> genRGBAVolume(const vl::Image* data, const vl::Image* trfunc, bool alpha_from_data=true);

    /** Generates an image whose RGB components represent the normals computed from the input image gradient packed into 0..1 range. 
    * The format of the image is vl::IF_RGB/vl::IT_FLOAT which is equivalent to a 3D grid of vl::fvec3.
    * The generated image is ready to be used as a texture for normal lookup. 
    * The original normal can be recomputed as N = (RGB - 0.5)*2.0. */
    static vl::ref<vl::Image> genGradientNormals(const vl::Image* data);

  protected:
    int mSliceCount;
    vl::ref<vl::Geometry> mGeometry;
    vl::AABB mBox;
    vl::fmat4 mCache;
    vl::fvec3 mTexCoord[8];

  private:
    template<typename data_type, vl::EImageType img_type>
    static vl::ref<vl::Image> genRGBAVolumeT(const vl::Image* data, const vl::Image* trfunc, const vl::fvec3& light_dir, bool alpha_from_data);
    template<typename data_type, vl::EImageType img_type>
    static vl::ref<vl::Image> genRGBAVolumeT(const vl::Image* data, const vl::Image* trfunc, bool alpha_from_data);
  };
}

#endif
