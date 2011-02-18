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

#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlGraphics/Light.hpp>

namespace vl
{
  class SlicedVolume: public ActorEventCallback
  {
  public:
    
    //! Constructor.
    SlicedVolume();
    
    void onActorRenderStarted(Actor* actor, Real frame_clock, const Camera* cam, Renderable* renderable, const Shader* shader, int pass);

    void onActorDelete(Actor* ) {}

    //! Binds a SlicedVolume to an Actor so that the SlicedVolume can generate the viewport aligned slices' geometry for the Actor as appropriate.
    void bindActor(Actor*);

    //! Updates the uniforms used by the GLSLProgram to render the volume each time the onActorRenderStarted() method is called.
    virtual void updateUniforms(Actor* actor, Real clock, const Camera* camera, Renderable* rend, const Shader* shader);
    
    //! Defines the number of slices used to render the volume: more slices generate a better (and slower) rendering.
    void setSliceCount(int count) { mSliceCount = count; }
    
    //! Returns the number of slices used to render the volume.
    int sliceCount() const { return mSliceCount; }
    
    //! Returns the Geometry associated to a SlicedVolume and its bound Actor
    Geometry* geometry() { return mGeometry.get(); }
    
    //! Returns the Geometry associated to a SlicedVolume and its bound Actor
    const Geometry* geometry() const { return mGeometry.get(); }
    
    //! Defines the dimensions of the box enclosing the volume
    void setBox(const AABB& box);
    
    //! The dimensions of the box enclosing the volume
    const AABB& box() const { return mBox; }
    
    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume
    const fvec3* texCoords() const { return mTexCoord; }
    
    //! Returns the texture coordinates assigned to each of the 8 box corners of the volume
    fvec3* texCoords() { return mTexCoord; }
    
    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    void generateTextureCoordinates(const ivec3& size);
    
    //! Generates a default set of texture coordinates for the 8 box corners of the volume based on the given texture dimensions.
    void generateTextureCoordinates(int width, int height, int depth) { generateTextureCoordinates(ivec3(width,height,depth)); }

  protected:
    int mSliceCount;
    ref<Geometry> mGeometry;
    AABB mBox;
    fmat4 mCache;
    fvec3 mTexCoord[8];
  };
}

#endif
