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

#ifndef Actor_INCLUDE_ONCE
#define Actor_INCLUDE_ONCE

#include <vl/Collection.hpp>
#include <vl/Sphere.hpp>
#include <vl/AABB.hpp>
#include <vl/Effect.hpp>
#include <vl/Renderable.hpp>
#include <vl/Transform.hpp>
#include <vl/LODEvaluator.hpp>
#include <vl/UniformSet.hpp>
#include <vl/Scissor.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  // ActorRenderEventCallback
  //------------------------------------------------------------------------------
  /** The ActorRenderEventCallback class defines a callback object to react to Actor-rendering related events.

  Usually an ActorRenderEventCallback is used to perform a per-Actor operation 
  like changing some attributes of the Actor itself or of the associated Renderable/Geometry.
  For example the MorphingCallback class is used to aid the rendering of a MorphingActor, while the
  DepthSortCallback class is used to perform per-Actor polygon sorting.

  \note
  You can manipulate Uniforms within this class, for more information see vl::GLSLProgram documentation.
  If you want to update the state of a Uniform variable from here you can simply call glUniform* since the 
  GLSLProgram (if any) has been already activated by the time this function is called. You can test whether
  the shader has a GLSLProgram bound to it or not by simply testing shader->glslProgram() != NULL. If you
  update a uniform you must ensure that all the Actor[s] using the same GLSLProgram appropriately setup such 
  uniform.

  \note
  An ActorRenderEventCallback is called once for every rendering pass, ie. if an Actor's Effect specifies three
  rendering passes the Actor callbacks will be called three times, once for each rendering pass / shader.

  \sa
  - Actor::renderEventCallbacks()
  - Actor::update() */
  class ActorRenderEventCallback: public Object
  {
  public:
    ActorRenderEventCallback(): mEnabled(true) {}

    /** Event generated just before an Actor is rendered but after the render states are ready and setup.
    Reimplement to react to this event.
    \param cam The camera used for the current rendering.
    \param actor The Actor bound to this rendering callback.
    \param renderable The currently selected Actor LOD.
    \param shader The currently active Shader.
    \param pass The current Actor[s] rendering pass. */
    virtual void onActorRenderStarted(Actor* actor, const Camera* cam, Renderable* renderable, const Shader* shader, int pass) = 0;

    void setEnabled(bool enabled) { mEnabled = enabled; }

    bool isEnabled() const { return mEnabled; }

  protected:
    bool mEnabled;
  };

  //------------------------------------------------------------------------------
  // ActorAnimator
  //------------------------------------------------------------------------------
  /** Callback object used to update/animate an Actor.
  The updateActor() method will be called whenever an actor becomes visible, before
  rendering it.
  \note
  The updateActor() method will be called...
  - ... after the actor is determined to be visible.
  - ... only once per frame.
  - ... before setting up the rendering states.
  - ... before rendering any Actor, that is, VL first updates 
  each and every Actor and then renders them. This means that you should
  not use an ActorAnimator to update data that is shared among multiple actors at the 
  same time, otherwise the same data will be updated several times, and all the actors
  using it will use the values resulting from the last update.

  Use vl::ActorRenderEventCallback in such cases as the ActorRenderEventCallback is triggered
  for each Actor right before it is rendered.

  \sa
  - Actor::setActorAnimator()
  - ActorRenderEventCallback
  - renderEventCallbacks()*/
  class ActorAnimator: public Object
  {
  public:
    /** Reimplement this function to update/animate an Actor.
    \param actor the Actor to be updated.
    \param lod the currently selected \p LOD for the Actor.
    \param camera the camera used for the current rendering.
    \param cur_time the current animation time.
    \sa Actor::setActorAnimator(); */
    virtual void updateActor(Actor* actor, int lod, Camera* camera, Real cur_time) = 0;
  };

  //------------------------------------------------------------------------------
  // Actor
  //------------------------------------------------------------------------------
  /** Associates a Renderable object to an Effect and Transform.
  An Actor can associate one Renderable for each LOD (level of detail) using
  the lods() function.
  The rendering order of an Actor is defined by its rendering rank, and block,
  see setRenderRank() and setRenderBlock() for the details.

  \note

  - The same Renderable can be bound to more than one Actor at the same time
  - The same Effect     can be bound to more than one Actor at the same time
  - The same Transform  can be bound to more than one Actor at the same time

  \remarks

  An Actor must always have a Renderable and Effect bound. If no Transform is 
  specified the Renderable will be rendered as if it had an identity matrix
  transformation.
  \par Uniforms

  The Uniforms defined in the Actor and the ones defined in the Shader must not
  overlap, that is, an Actor must not define Uniforms that are also present 
  in the Shader's Uniform and vice versa.

  \sa Transform, Effect, Renderable, Geometry
  */
  class Actor: public Object
  {
  public:
    virtual const char* className() { return "Actor"; }

    /** Constructor.
    \param renderable A Renderable defining the Actor's LOD level #0
    \param effect The Effect to be used by the Actor
    \param transform The Transform to be used by the Actor
    \param block The rendering block to which the Actor belongs
    \param rank The rendering rank to which the Actor belongs
    */
    Actor(Renderable* renderable = NULL, Effect* effect = NULL, Transform* transform = NULL, int block = 0, int rank = 0):
      mLastUpdateTime(0.0), mEffect(effect), mTransform(transform), mRenderBlock(block), mRenderRank(rank),
      mTransformUpdateTick(-1), mBoundsUpdateTick(-1), mEnableMask(0xFFFFFFFF), mOcclusionQuery(0), mIsOccludee(true), mOcclusionQueryTick(0xFFFFFFFF)
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      mRenderEventCallbacks.setAutomaticDelete(false);
      lod(0) = renderable;
      // actor user data
      #if VL_ACTOR_USER_DATA
        mActorUserData = NULL;
      #endif
    }

    //! Destructor.
    virtual ~Actor();

    /** Returns the Renderable object representing the LOD level specifed by \p index. */
    ref<Renderable>& lod(int lod_index) { return mRenderables[lod_index]; }
    /** Returns the Renderable object representing the LOD level specifed by \p index. */
    const ref<Renderable>& lod(int lod_index) const { return mRenderables[lod_index]; }

    /** Utility function to assign one or more Renderable[s] to one or more LOD levels. */
    void setLODs(Renderable* lod0, Renderable* lod1=NULL, Renderable* lod2=NULL, Renderable* lod3=NULL, Renderable* lod4=NULL, Renderable* lod5=NULL);

    /** Binds a Transform to an Actor */
    void setTransform(Transform* transform)
    {
      mTransform = transform;
      mTransformUpdateTick = -1;
      mBoundsUpdateTick    = -1;
    }
    /** Returns the Transform bound tho an Actor */
    Transform* transform()  { return mTransform.get(); }
    /** Returns the Transform bound tho an Actor */
    const Transform* transform() const { return mTransform.get(); }

    /** Binds an Effect to an Actor */
    void setEffect(Effect* effect) { mEffect = effect; }
    /** Returns the Effect bound to an Actor */
    Effect* effect() { return mEffect.get(); }
    /** Returns the Effect bound to an Actor */
    const Effect* effect() const { return mEffect.get(); }

    /** Returns the bounding sphere that contains this Actor taking into consideration also its Transform. */
    const Sphere& boundingSphere() const { return mSphere; }

    /** Returns the bounding box that contains this Actor taking into consideration also its Transform. */
    const AABB& boundingBox() const { return mAABB; }

    /** Computes the bounding box and bounding sphere of an Actor taking into consideration also its Transform. */
    void computeBounds();

    /** Modifies the rendering rank of an Actor.

    The rendering rank affects the order in which an Actor is rendered, the greater the rank the later the Actor is rendered. 
    The default render rank is zero. 

    To know more about rendering order please see \ref pagGuideRenderOrder "Rendering Order".

    \sa setRenderBlock(), Effect::setRenderRank()
    */
    void setRenderRank(int rank) { mRenderRank = rank; }

    /**
    Modifies the rendering block of an Actor.

    The rendering block affects the order in which an Actor is rendered, the greater the block the later the Actor is rendered. 
    The default render block is zero.

    To know more about rendering order please see \ref pagGuideRenderOrder "Rendering Order".

    \sa setRenderRank(), Effect::setRenderRank()
    */
    void setRenderBlock(int block) { mRenderBlock = block; }

    /** Returns the rendering rank of an Actor. */
    int renderRank() const { return mRenderRank; }

    /** Returns the rendering block of an Actor. */
    int renderBlock() const { return mRenderBlock; }

    /** Installs the LODEvaluator used to compute the current LOD at rendering time. */
    void setLODEvaluator(LODEvaluator* lod_evaluator) { mLODEvaluator = lod_evaluator; }

    /** Returns the installed LODEvaluator (if any) or NULL. */
    LODEvaluator* lodEvaluator() { return mLODEvaluator.get(); }

    /** Returns the installed LODEvaluator (if any) or NULL. */
    const LODEvaluator* lodEvaluator() const { return mLODEvaluator.get(); }

    int evaluateLOD(Camera* camera);

    /** Installs the ActorAnimator used to update/animate an Actor (see vl::ActorAnimator documentation). */
    void setActorAnimator(ActorAnimator* animator) { mActorAnimator = animator; }

    /** Returns the ActorAnimator used to update/animate an Actor (see vl::ActorAnimator documentation). */
    ActorAnimator* actorAnimator() { return mActorAnimator.get(); }

    /** Returns the ActorAnimator used to update/animate an Actor (see vl::ActorAnimator documentation). */
    const ActorAnimator* actorAnimator() const { return mActorAnimator.get(); }

    /** Last time an Actor was animated/updated using an actorAnimator(). */
    Real lastUpdateTime() const { return mLastUpdateTime; }

    /** For internal use only. */
    void setLastUpdateTime(Real time) { mLastUpdateTime = time; }

    /** The enable mask of an Actor is usually used to defines whether the actor should be rendered or not 
      * depending on the Rendering::enableMask() but it can also be used for user-specific tasks. */
    void setEnableMask(unsigned int mask) { mEnableMask = mask; }

    /** The enable mask of an Actor is usually used to defines whether the actor should be rendered or not 
      * depending on the Rendering::enableMask() but it can also be used for user-specific tasks. */
    unsigned int enableMask() const { return mEnableMask; }

    // uniforms methods

    /** Equivalent to uniformSet()->setUniform(uniform)
     \remarks
     This function performs a 'setUniformSet(new UniformSet)' if uniformSet() is NULL. */
    void setUniform(Uniform* uniform);

    /** Equivalent to uniformSet()->uniforms()
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    const std::vector< ref<Uniform> >& uniforms() const;

    /** Equivalent to uniformSet()->eraseUniform(name)
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    void eraseUniform(const std::string& name);

    /** Equivalent to uniformSet()->eraseUniform(uniform)
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    void eraseUniform(const Uniform* uniform);

    /** Equivalent to uniformSet()->eraseAllUniforms()
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    void eraseAllUniforms();

    /** Equivalent to uniformSet()->getUniform(name, get_mode)
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    Uniform* gocUniform(const std::string& name);

    /** Equivalent to uniformSet()->getUniform(name, get_mode)
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    Uniform* getUniform(const std::string& name);
    
    /** Equivalent to uniformSet()->getUniform(name, get_mode)
     \remarks
     You must install a UniformSet with setUniformSet() before calling this function. */
    const Uniform* getUniform(const std::string& name) const;

    /** Installs a new UniformSet
     \sa
     - setUniform()
     - uniforms()
     - eraseUniform(const std::string& name)
     - eraseUniform(const Uniform* uniform)
     - eraseAllUniforms()
     - getUniform() */
    void setUniformSet(UniformSet* uniforms) { mUniformSet = uniforms; }

    /** Returns the UniformSet installed
     \sa
     - setUniform()
     - uniforms()
     - eraseUniform(const std::string& name)
     - eraseUniform(const Uniform* uniform)
     - eraseAllUniforms()
     - getUniform()
    */
    UniformSet* uniformSet() const { return mUniformSet.get(); }

    /** Returns the list of ActorRenderEventCallback bound to an Actor. */
    const Collection<ActorRenderEventCallback>* renderEventCallbacks() const { return &mRenderEventCallbacks; }

    /** Returns the list of ActorRenderEventCallback bound to an Actor. */
    Collection<ActorRenderEventCallback>* renderEventCallbacks() { return &mRenderEventCallbacks; }

    /** Calls all the ActorRenderEventCallback installed on this Actor. */
    void dispatchOnActorRenderStarted(const Camera* camera, Renderable* renderable, const Shader* shader, int pass)
    {
      for(int i=0; i<renderEventCallbacks()->size(); ++i)
      {
        ActorRenderEventCallback& cb = *renderEventCallbacks()->at(i);
        if (cb.isEnabled())
          cb.onActorRenderStarted(this, camera, renderable, shader, pass);
      }
    }

    /** Sets the Scissor to be used when rendering an Actor.
     \note
     You can also define a Scissor on a per-Shader basis using the function Shader::setScissor(). 
     In case both the Shader's and the Actor's Scissor are defined the Actor's Scissor is used.
     \sa
     - Scissor
     - Shader::setScissor()
    */
    void setScissor(Scissor* scissor) { mScissor = scissor; }
    /** Returns the Scissor used when rendering an Actor.
     \sa
     - Scissor
     - Actor::setScissor()
     - Shader::setScissor()
    */
    const Scissor* scissor() const { return mScissor.get(); }
    /** Returns the Scissor used when rendering an Actor.
     \sa
     - Scissor
     - Actor::setScissor()
     - Shader::setScissor()
    */
    Scissor* scissor() { return mScissor.get(); }

    /** If \p is_occludee equals true an occlusion test will be performed before the rendering of the Actor (if occlusion culling is enabled)
    otherwise the Actor will always be rendered with no occlusion test even when occlusion culling is enabled. */
    void setOccludee(bool is_occludee) { mIsOccludee = is_occludee; }

    /** If \p is_occludee equals true an occlusion test will be performed before the rendering of the Actor (if occlusion culling is enabled)
    otherwise the Actor will always be rendered with no occlusion test even when occlusion culling is enabled. */
    bool isOccludee() const { return mIsOccludee; }

    /** For internal use only.
    Creates the occlusion query object name bound this Actor using the OpenGL function glGenQueries(). */
    void createOcclusionQuery();

    /** For internal use only.
    Deletes the occlusion query object name using the OpenGL function glDeleteQueries(). */
    void deleteOcclusionQuery();

    /** For internal use only.
    Returns the occlusion query object name bound this Actor as by the OpenGL function glGenQueries(). Returns 0 if no query object name has been created yet. */
    GLuint occlusionQuery() const { return mOcclusionQuery; }

    /** For internal use only. */
    void setOcclusionQueryTick(unsigned tick) { mOcclusionQueryTick = tick; }

    /** For internal use only. */
    unsigned occlusionQueryTick() const { return mOcclusionQueryTick; }

#if VL_ACTOR_USER_DATA
  public:
    void* actorUserData() { return mActorUserData; }
    const void* actorUserData() const { return mActorUserData; }
    void setActorUserData(void* user_data) { mActorUserData = user_data; }

  private:
    void* mActorUserData;
#endif

  protected:
    AABB mAABB;
    Sphere mSphere;
    ref<Effect> mEffect;
    ref<Renderable> mRenderables[VL_MAX_ACTOR_LOD];
    ref<Transform> mTransform;
    ref<LODEvaluator> mLODEvaluator;
    ref<UniformSet> mUniformSet;
    ref<Scissor> mScissor;
    ref<ActorAnimator> mActorAnimator; /*mic fixme: remove?*/
    Real mLastUpdateTime;
    Collection<ActorRenderEventCallback> mRenderEventCallbacks;
    int mRenderBlock;
    int mRenderRank;
    long long mTransformUpdateTick;
    long long mBoundsUpdateTick;
    unsigned int mEnableMask;
    GLuint mOcclusionQuery;
    bool mIsOccludee;
    unsigned mOcclusionQueryTick;
  };
  //---------------------------------------------------------------------------
  class ActorCollection: public Collection<Actor> {};
  //---------------------------------------------------------------------------
}

#endif
