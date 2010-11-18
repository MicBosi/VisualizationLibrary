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

#ifndef Transform_INCLUDE_ONCE
#define Transform_INCLUDE_ONCE

#include <vl/vlnamespace.hpp>
#include <vl/Object.hpp>
#include <vl/Matrix4.hpp>
#include <vector>

namespace vl
{
  class Camera;

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
  class Transform: public Object
  {
  public:
    virtual const char* className() { return "Transform"; }

    /** Constructor. */
    Transform(): mParent(NULL), mWorldMatrixUpdateTick(0), mAssumeIdentityWorldMatrix(false) 
    { 
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    /** Constructor. The \p matrix parameter is used to set both the local and world matrix. */
    Transform(const mat4& matrix): mParent(NULL), mAssumeIdentityWorldMatrix(false) 
    { 
      setLocalMatrix(matrix);
      setWorldMatrix(matrix);
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    /** Returns the children Transforms. */
    const std::vector< ref<Transform> >& children() const { return mChildren; }

    /** Adds a child transform. */
    void addChild(Transform* child);
    
    /** Adds \p count children transforms. */
    void addChildren(Transform* const *, size_t count);
    
    /** Adds \p count children transforms. */
    void addChildren(const ref<Transform>*, size_t count);
    
    /** Adds the specified \p children transforms. */
    void addChildren(const std::vector< Transform* >& children);
    
    /** Adds the specified \p children transforms. */
    void addChildren(const std::vector< ref<Transform> >& children);

    /** Minimizes the amount of memory used to store the children Transforms. */
    void shrink();

    /** Minimizes recursively the amount of memory used to store the children Transforms. */
    void shrinkRecursive();

    /** Reserves space for \p count children. This function is very useful when you need to add one by one a large number of children transforms. 
      * \note This function does not affect the number of children, it only reallocates the buffer used to store them 
      * so that it's large enough to \p eventually contain \p count of them. This will make subsequent calls to addChild() quicker
      * as fewer or no reallocations of the buffer will be needed. */
    void reserveChildren(size_t count) { mChildren.reserve(count); }

    /** Sets the \p index-th child. */
    void setChild(int index, Transform* child);
    
    /** Returns the number of children a Transform has. */
    int childCount() const;
    
    /** Returns the i-th child Transform. */
    Transform* child(int i);
    
    /** Returns the last child. */
    Transform* lastChild();
    
    /** Removes the given \p child Transform. */
    void eraseChild(Transform* child);
    
    /** Removes \p count children Transforms starting at position \p index. */
    void eraseChildren(int index, int count);
    
    /** Removes all the children of a Transform. */
    void eraseAllChildren();
    
    /** Removes all the children of a Transform recursively descending the hierachy. */
    void eraseAllChildrenRecursive();

    /** Returns the parent of a Transform. */
    const Transform* parent() const { return mParent; }
    
    /** Returns the parent of a Transform. */
    Transform* parent() { return mParent; }
    
    /** Returns the matrix computed concatenating this Transform's local matrix with its parents' local matrices. */
    mat4 getComputedWorldMatrix();

    /** Returns the local matrix. */
    const mat4& localMatrix() const;

    /** Sets the local matrix. 
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void setLocalMatrix(const mat4& matrix);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::translation(x,y,z)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void translate(Real x, Real y, Real z);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::translation(t)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void translate(const vec3& t);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::scaling(x,y,z)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void scale(Real x, Real y, Real z);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::rotation(degrees,x,y,z)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void rotate(Real degrees, Real x, Real y, Real z);
    
    /** Utility function equivalent to \p setLocalMatrix( mat4::rotation(from,to)*localMatrix() ).
      * After calling this you might want to call computeWorldMatrix() or computeWorldMatrixRecursive(). */
    void rotate(const vec3& from, const vec3& to);

    /** Sets both the local and the world matrices.
      * This function is useful to quickly set those Transforms that do not have a parent, for which
      * is equivalent to: \p setLocalMatrix(matrix); \p computeWorldMatrix(NULL); */
    void setLocalAndWorldMatrix(const mat4& matrix)
    { 
      mLocalMatrix = matrix;
      setWorldMatrix(matrix);
    }

    /** Computes the world matrix by concatenating the parent's world matrix with this' local matrix. */
    virtual void computeWorldMatrix(Camera* camera=NULL);

    /** Computes the world matrix by concatenating the parent's world matrix with this' local matrix, recursively descending to the children. */
    void computeWorldMatrixRecursive(Camera* camera=NULL);

    /** Returns the world matrix used for rendering. */
    const mat4& worldMatrix() const;

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
    bool hasDuplicatedChildren() const;

    /** Normally you should not use directly this function, call it only if you are sure you cannot do otherwise. 
      * Calling this function will also increment the worldMatrixUpdateTick(). */
    void setWorldMatrix(const mat4& matrix);

  protected:
    mat4 mWorldMatrix; 
    mat4 mLocalMatrix;
    std::vector< ref<Transform> > mChildren;
    Transform* mParent;
    long long mWorldMatrixUpdateTick;
    bool mAssumeIdentityWorldMatrix;
  };
}

#endif
