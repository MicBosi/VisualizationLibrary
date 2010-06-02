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

#ifndef VolumePlot_INCLUDE_ONCE
#define VolumePlot_INCLUDE_ONCE

#include <vl/Actor.hpp>
#include <vl/Geometry.hpp>
#include <vl/Text.hpp>
#include <vl/Effect.hpp>
#include <vl/Transform.hpp>
#include <vl/ActorTreeAbstract.hpp>
#include <vl/SceneManagerActorTree.hpp>
#include <vlVolume/MarchingCubes.hpp>

namespace vl
{
  class SceneManager;
}

namespace vlVolume
{
  //! Generates a 3D plot with labels and isosurface. The isosurface is generated using the MarchingCubes algorithm.
  class VolumePlot: public vl::Object
  {
  public:
    //! A function to be used with VolumePlot
    class Function
    {
    public:
      virtual float operator()(float x, float y, float z) const = 0;
    };

  public:
    //! Constructor.
    VolumePlot();

    //! Computes the function and generates the plot. This method should be called after all the other methods.
    void compute(const Function& func, float threshold);

    //! The Actor representing the isosurface
    const vl::Actor* isosurfaceActor() const { return mIsosurfaceActor.get(); }
    //! The Actor representing the isosurface
    vl::Actor* isosurfaceActor() { return mIsosurfaceActor.get(); }

    //! The Geometry representing the isosurface
    const vl::Geometry* isosurfaceGeometry() const { return mIsosurfaceGeometry.get(); }
    //! The Geometry representing the isosurface
    vl::Geometry* isosurfaceGeometry() { return mIsosurfaceGeometry.get(); }

    //! Used to get/set the rendering options (like color, material, transparency) etc. of the isosurface 
    const vl::Effect* isosurfaceEffect() const { return mIsosurfaceEffect.get(); }
    //! Used to get/set the rendering options (like color, material, transparency) etc. of the isosurface 
    vl::Effect* isosurfaceEffect() { return mIsosurfaceEffect.get(); }

    //! Used to get/set the rendering options (like color, material, transparency) etc. of the box
    const vl::Effect* boxEffect() const { return mBoxEffect.get(); }
    //! Used to get/set the rendering options (like color, material, transparency) etc. of the box
    vl::Effect* boxEffect() { return mBoxEffect.get(); }

    //! Default value: vl::fvec3(-1,-1,-1)
    const vl::fvec3& minCorner() const { return mMinCorner; }
    //! Default value: vl::fvec3(-1,-1,-1)
    void setMinCorner(const vl::fvec3& min_corner) { mMinCorner = min_corner; }

    //! Default value: vl::fvec3(+1,+1,+1)
    const vl::fvec3& maxCorner() const { return mMaxCorner; }
    //! Default value: vl::fvec3(+1,+1,+1)
    void setMaxCorner(const vl::fvec3& max_corner) { mMaxCorner = max_corner; }

    //! The transform associated to the whole plot
    vl::Transform* plotTransform() const { return mPlotTransform.get(); }
    //! The transform associated to the whole plot
    void setPlotTransform(vl::Transform* tr) { mPlotTransform = tr; }

    //! Default value: vl::ivec3(64,64,64)
    const vl::ivec3& samplingResolution() const { return mSamplingResolution; }
    //! Default value: vl::ivec3(64,64,64)
    void setSamplingResolution(const vl::ivec3& size) { mSamplingResolution = size; }

    //! Sets the format of the labels
    const vl::String& labelFormat() const { return mLabelFormat; }
    //! Sets the format of the label to be generated, es. "(%.2n %.2n %.2n)" or "<%.3n, %.3n, %.3n>"
    void setLabelFormat(const vl::String& format) { mLabelFormat = format; }

    //! The Font to be used for the box labels
    vl::Font* labelFont() const { return mLabelFont.get(); }
    //! The Font to be used for the box labels
    void setLabelFont(vl::Font* font) { mLabelFont = font; }

    //! A Text used to initialize the plot labels
    const vl::Text* textTemplate() const { return mTextTemplate.get(); }
    //! A Text used to initialize the plot labels
    vl::Text* textTemplate() { return mTextTemplate.get(); }

    vl::ActorTree* actorTreeMulti() { return mActorTreeMulti.get(); }
    const vl::ActorTree* actorTreeMulti() const { return mActorTreeMulti.get(); }

  protected:
    void setupLabels(const vl::String& format, const vl::fvec3& min_corner, const vl::fvec3& max_corner, vl::Font* font, vl::Transform* root_tr);
    void evaluateFunction(float* scalar, const vl::fvec3& min_corner, const vl::fvec3& max_corner, const Function& func);

  protected:
    std::vector< vl::ref<vl::Actor> > mActors;
    vl::ref<vl::Transform> mPlotTransform;
    vl::ivec3 mSamplingResolution;
    vl::String mLabelFormat;
    vl::ref< vl::Font > mLabelFont;
    vl::fvec3 mMinCorner;
    vl::fvec3 mMaxCorner;
    vl::ref<vl::Geometry> mIsosurfaceGeometry;
    vl::ref<vl::Actor> mIsosurfaceActor;
    vl::ref<vl::Effect> mIsosurfaceEffect;
    vl::ref<vl::Effect> mBoxEffect;
    vl::ref<vl::Text> mTextTemplate;
    vl::ref<vl::ActorTree> mActorTreeMulti;
  };
}

#endif
