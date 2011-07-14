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

#ifndef Transform_INCLUDE_ONCE
#define Transform_INCLUDE_ONCE

#include <vlCore/vlnamespace.hpp>
#include <vlCore/Object.hpp>
#include <vlCore/Matrix4.hpp>
#include <vector>
#include <set>
#include <algorithm>

namespace vl
{
  class Camera;

  // mic fixme: finalize the generalization by
  // - moving the local matrix management away from ITransform
  // - implementing a Quat/Scale/Translation-based transform type: QuScTrTransform?
  // - rename Transform to MatrixTransform
  // - the engine, rendering, actors etc. should use ITransform instead of MatrixTransform

  //------------------------------------------------------------------------------
  // TransformHierarchy
  //------------------------------------------------------------------------------
  //! Abstract interface for a generic transform.
  class VLCORE_EXPORT ITransform: public Object
  {
    VL_INSTRUMENT_CLASS(vl::ITransform, Object)

  public:
    ITransform(): mWorldMatrixUpdateTick(0), mAssumeIdentityWorldMatrix(false)  
    {
      #if VL_TRANSFORM_USER_DATA 
        mTransformUserData = NULL;
      #endif
    }

    virtual size_t childrenCount() const = 0;
    
    virtual ITransform* getChildren(size_t i) = 0;
    
    virtual const ITransform* getChildren(size_t i) const = 0;
    
    virtual ITransform* parent() = 0;
    
    virtual const ITransform* parent() const = 0;
    
    /** The matrix representing the transform's local space.
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void setLocalMatrix(const mat4& m)
    { 
      mLocalMatrix = m;
    }

    /** The matrix representing the transform's local space. */
    const mat4& localMatrix() const
    { 
      return mLocalMatrix;
    }

    /** Computes the world matrix by concatenating the parent's world matrix with its own local matrix. */
    virtual void computeWorldMatrix(Camera* /*camera*/ = NULL)
    {
      if( assumeIdentityWorldMatrix() )
      {
        setWorldMatrix(mat4()); 
      }
      else
      /* top Transforms are usually assumeIdentityWorldMatrix() == true for performance reasons */
      if( parent() && !parent()->assumeIdentityWorldMatrix() )
      {
        setWorldMatrix( parent()->worldMatrix() * localMatrix() );
      }
      else
        setWorldMatrix( localMatrix() );
    }

    /** Computes the world matrix by concatenating the parent's world matrix with its local matrix, recursively descending to the children. */
    void computeWorldMatrixRecursive(Camera* camera = NULL)
    {
      computeWorldMatrix(camera);
      for(size_t i=0; i<childrenCount(); ++i)
        getChildren(i)->computeWorldMatrixRecursive(camera);
    }

    /** Returns the matrix computed concatenating this Transform's local matrix with the local matrices of all its parents. */
    mat4 getComputedWorldMatrix()
    {
      mat4 world = localMatrix();
      ref<ITransform> par = parent();
      while(par)
      {
        world = par->localMatrix() * world;
        par = par->parent();
      }
      return world;
    }

    /** Returns the world matrix used for rendering. */
    const mat4& worldMatrix() const 
    { 
      return mWorldMatrix;
    }

    /** Normally you should not use directly this function, call it only if you are sure you cannot do otherwise. 
      * Usually you want to call computeWorldMatrix() or computeWorldMatrixRecursive().
      * Calling this function will also increment the worldMatrixUpdateTick(). */
    void setWorldMatrix(const mat4& matrix) 
    { 
      mWorldMatrix = matrix; 
      ++mWorldMatrixUpdateTick;
    }

    /** Returns the internal update tick used to avoid unnecessary computations. The world matrix thick 
      * gets incremented every time the setWorldMatrix() or setLocalAndWorldMatrix() functions are called. */
    long long worldMatrixUpdateTick() const { return mWorldMatrixUpdateTick; }

    /** If set to true the world matrix of this transform will always be considered and identity.
      * Is usually used to save calculations for top Transforms with many sub-Transforms. */
    void setAssumeIdentityWorldMatrix(bool assume_I) { mAssumeIdentityWorldMatrix = assume_I; }

    /** If set to true the world matrix of this transform will always be considered and identity.
      * Is usually used to save calculations for top Transforms with many sub-Transforms. */
    bool assumeIdentityWorldMatrix() { return mAssumeIdentityWorldMatrix; }

    /** Checks whether there are duplicated entries in the Transform's children list. */
    bool hasDuplicatedChildren() const
    {
      std::set<const ITransform*> tr_set;
      for(size_t i=0; i<childrenCount(); ++i)
        tr_set.insert(getChildren(i));
      return tr_set.size() != childrenCount();
    }

#if VL_TRANSFORM_USER_DATA 
  public:
    void setTransformUserData(void* data) { mTransformUserData = data; }
    const void* transformUserData() const { return mTransformUserData; }
    void* transformUserData() { return mTransformUserData; }

  private:
    void* mTransformUserData;
#endif

  protected:
    mat4 mLocalMatrix;
    mat4 mWorldMatrix;
    long long mWorldMatrixUpdateTick;
    bool mAssumeIdentityWorldMatrix;
  };

  //------------------------------------------------------------------------------
  // TransformHierarchy
  //------------------------------------------------------------------------------
  //! Adds parent/child management functions to the specified transform type.
  template<class Ttype>
  class TransformHierarchy: public ITransform
  {
    VL_INSTRUMENT_CLASS(vl::TransformHierarchy<class Ttype>, ITransform)

  public:
    TransformHierarchy(): mParent(NULL) {}

    // --- --- ITransform interface implementation --- ---

    virtual size_t childrenCount() const { return mChildren.size(); }

    virtual ITransform* getChildren(size_t i) { return mChildren[i].get(); }
    
    virtual const ITransform* getChildren(size_t i) const { return mChildren[i].get(); }

    virtual const ITransform* parent() const { return mParent; }
    
    virtual ITransform* parent() { return mParent; }

    // --- --- children management --- ---

    /** Returns the children Transforms. */
    const std::vector< ref<Ttype> >& children() const { return mChildren; }

    /** Adds a child transform. */
    void addChild(Ttype* child)
    {
      VL_CHECK(child != NULL)
      VL_CHECK(child->mParent == NULL)

      mChildren.push_back(child);
      child->mParent = this;
    }
    
    /** Adds \p count children transforms. */
    void addChildren(Ttype*const* children, size_t count)
    {
      VL_CHECK(children != NULL)

      if (count)
      {
        size_t insert_point = mChildren.size();
        mChildren.resize(mChildren.size() + count);
        vl::ref<Ttype>* ptr = &mChildren[insert_point];
        for(size_t i=0; i<count; ++i, ++ptr)
        {
          VL_CHECK(children[i]->mParent == NULL);
          children[i]->mParent = this;
          (*ptr) = children[i];
        }
      }
    }

    void addChildren(const ref<Ttype>* children, size_t count)
    {
      VL_CHECK(children != NULL)

      if (count)
      {
        size_t insert_point = mChildren.size();
        mChildren.resize(mChildren.size() + count);
        vl::ref<Ttype>* ptr = &mChildren[insert_point];
        for(size_t i=0; i<count; ++i)
        {
          VL_CHECK(children[i]->mParent == NULL);
          children[i]->mParent = this;
          ptr[i] = children[i];
        }
      }
    }

    void addChildren(const std::vector< ref<Ttype> >& children)
    {
      if (children.size())
        addChildren( &children[0], children.size() );
    }
    
    /** Adds the specified \p children transforms. */
    void addChildren(const std::vector< Ttype* >& children)
    {
      if (children.size())
        addChildren( &children[0], children.size() );
    }
    
    /** Sets the \p index-th child. */
    void setChild(int index, Ttype* child)
    {
      VL_CHECK(child)
      VL_CHECK( index < (int)mChildren.size() )
      mChildren[index]->mParent = NULL;
      mChildren[index] = child;
      mChildren[index]->mParent = this;
    }
    
    /** Returns the last child. */
    Ttype* lastChild()
    {
      return mChildren.back().get();
    }
    
    /** Removes the given \p child Ttype. */
    void eraseChild(Ttype* child)
    {
      VL_CHECK(child != NULL)
      typename std::vector< ref<Ttype> >::iterator it;
      it = std::find(mChildren.begin(), mChildren.end(), child);
      VL_CHECK(it != mChildren.end())
      if (it != mChildren.end())
      {
        (*it)->mParent = NULL;
        mChildren.erase(it);
      }
    }
    
    /** Removes \p count children Transforms starting at position \p index. */
    void eraseChildren(int index, int count)
    {
      VL_CHECK( index + count <= (int)mChildren.size() );

      for(int j=index; j<index+count; ++j)
        mChildren[j]->mParent = NULL;

      for(int i=index+count, j=index; i<(int)mChildren.size(); ++i, ++j)
        mChildren[j] = mChildren[i];

      mChildren.resize( mChildren.size() - count );
    }    

    /** Removes all the children of a Ttype. */
    void eraseAllChildren()
    {
      for(int i=0; i<(int)mChildren.size(); ++i)
        mChildren[i]->mParent = NULL;
      mChildren.clear();
    }
    
    /** Removes all the children of a Ttype recursively descending the hierarchy. */
    void eraseAllChildrenRecursive()
    {
      for(int i=0; i<(int)mChildren.size(); ++i)
      {
        mChildren[i]->eraseAllChildrenRecursive();
        mChildren[i]->mParent = NULL;
      }
      mChildren.clear();
    }

    /** Minimizes the amount of memory used to store the children Transforms. */
    void shrink()
    {
      std::vector< ref<Ttype> > tmp (mChildren);
      mChildren.swap(tmp);
    }

    /** Minimizes recursively the amount of memory used to store the children Transforms. */
    void shrinkRecursive()
    {
      shrink();
      for(size_t i=0; i<mChildren.size(); ++i)
        mChildren[i]->shrinkRecursive();
    }

    /** Reserves space for \p count children. This function is very useful when you need to add one by one a large number of children transforms. 
      * \note This function does not affect the number of children, it only reallocates the buffer used to store them 
      * so that it's large enough to \p eventually contain \p count of them. This will make subsequent calls to addChild() quicker
      * as fewer or no reallocations of the buffer will be needed. */
    void reserveChildren(size_t count) { mChildren.reserve(count); }

  protected:
    std::vector< ref<Ttype> > mChildren;
    TransformHierarchy* mParent;
  };

  //------------------------------------------------------------------------------
  // Transform
  //------------------------------------------------------------------------------
  /** Implements a 4x4 matrix transform used to define the position and orientation of an Actor.
    *
    * Transforms can be linked together to create a tree-like hierarchy.
    *
    * \par Optimizing Your Transforms
    *
    * - Don't use them unless strictly necessary: if an Actor uses an I (identity) transform you do not need one, just call \p vl::Actor::setTransform(NULL).
    *
    * - Don't create Transform hierarchies if not needed. Just call setLocalAndWorldMatrix() per-Transform if you have a set of parent-less Transforms.
    *
    * - If the root of a hierarchy is an I (identity) transform, let VL know it by calling \p setAssumeIdentityWorldMatrix(true). This will save
    *   unnecessary matrix multiplications since multiplying by an identity matrix has no effect.
    *
    * - Call computeWorldMatrix() / computeWorldMatrixRecursive() not at each frame but only if the local matrix has actually changed.
    *
    * - Do not add a Transform hierarchy to vl::Rendering::transform() if such Transforms are not animated every frame, 
    *   this will cause VL unnecessarily update the Transforms at each frame.
    *
    * - Remember: VL does not require your Actors to have a Transform or such Transforms to be part of any hierarchy, it just expect that the 
    *   worldMatrix() of an Actor's Transform (if it has any) is up to date at rendering time. How and when they are updated can be fine 
    *   tuned by the user according to the specific application needs.
    *
    * \sa setLocalAndWorldMatrix(), setAssumeIdentityWorldMatrix(), Rendering::transform()
    * \sa Actor, Rendering, Effect, Renderable, Geometry */
  class VLCORE_EXPORT Transform: public TransformHierarchy<Transform>
  {
    VL_INSTRUMENT_CLASS(vl::Transform, TransformHierarchy<Transform>)

  public:
    /** Constructor. */
    Transform()
    { 
      VL_DEBUG_SET_OBJECT_NAME()
    }

    /** Constructor. The \p matrix parameter is used to set both the local and world matrix. */
    Transform(const mat4& matrix)
    { 
      VL_DEBUG_SET_OBJECT_NAME()
      setLocalMatrix(matrix);
      setWorldMatrix(matrix);
    }

    /** Sets both the local and the world matrices.
      * This function is useful to quickly set those Transforms that do not have a parent, for which
      * is equivalent to: \p setLocalMatrix(matrix); \p computeWorldMatrix(NULL); */
    void setLocalAndWorldMatrix(const mat4& matrix)
    { 
      mLocalMatrix = matrix;
      setWorldMatrix(matrix);
    }

    /** Utility function equivalent to \p setLocalMatrix( mat4::getTranslation(x,y,z)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void translate(Real x, Real y, Real z);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::getTranslation(t)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void translate(const vec3& t);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::getScaling(x,y,z)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void scale(Real x, Real y, Real z);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::getRotation(degrees,x,y,z)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void rotate(Real degrees, Real x, Real y, Real z);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::getRotation(from,to)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void rotate(const vec3& from, const vec3& to);
  };
}

#endif
