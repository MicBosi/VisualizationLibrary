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

#ifndef Object_INCLUDE_ONCE
#define Object_INCLUDE_ONCE

#include <vl/checks.hpp>
#include <string>

#if VL_DEBUG_LIVING_OBJECTS && !defined(NDEBUG)
  #include <set>
#endif

namespace vl
{
  template<class T> class ref;
  //------------------------------------------------------------------------------
  // VirtualMutex
  //------------------------------------------------------------------------------
  /**
   * A base class to implement simple platform-independent mutexes.
  */
  class VirtualMutex
  {
  public:

    //! Locks the mutex.
    virtual void lock() = 0;

    //! Unlocks the mutex.
    virtual void unlock() = 0;

    //! Returns 1 if locked, 0 if non locked, -1 if unknown.
    virtual int isLocked() const = 0;
  };

  //------------------------------------------------------------------------------
  // Object
  //------------------------------------------------------------------------------
  /**
   * The base class for all the reference counted objects.
   * See also vl::ref.
  */
  class Object
  {
  public:

    //! Returns the name of the class.
    virtual const char* className() { return "Object"; }

    Object()
    {
      mMutex = NULL;
      mReferenceCount = 0;
      mAutomaticDelete = true;
      #ifndef NDEBUG
        mObjectName = className();
      #endif
      #if VL_DEBUG_LIVING_OBJECTS && !defined(NDEBUG)
        debug_living_objects()->insert(this);
        // mDebug_LivingObjects.insert(this);
      #endif
    }

    Object(const Object& other)
    {
      // copy the name only
      mObjectName = other.mObjectName;
      // mMutex, mReferenceCount and mAutomaticDelete are not copiable.
      mMutex = NULL;
      mReferenceCount  = 0;
      mAutomaticDelete = true;
      // debug living object
      #if VL_DEBUG_LIVING_OBJECTS && !defined(NDEBUG)
        debug_living_objects()->insert(this);
        // mDebug_LivingObjects.insert(this);
      #endif
    }

    Object& operator=(const Object& other) 
    { 
      /* mMutex, mReferenceCount and mAutomaticDelete is not copied */ 
      mObjectName = other.mObjectName; 
      return *this; 
    }

    const std::string& objectName() const { return mObjectName; }
    
    void setObjectName(const std::string& name) { mObjectName = name; }

    //! Set the mutex used to protect the reference counting of this object across multiple threads.
    void setMutex(VirtualMutex* mutex) { mMutex = mutex; }
    
    //! Returns the mutex used to protect the reference counting of this object across multiple threads.
    VirtualMutex* mutex() { return mMutex; }
    
    //! Returns the mutex used to protect the reference counting of this object across multiple threads.
    const VirtualMutex* mutex() const { return mMutex; }

    int referenceCount() const 
    { 
      return mReferenceCount; 
    }

    void incReference()
    {
      if (mutex())
        mutex()->lock();
      ++mReferenceCount;
      if(mutex())
        mutex()->unlock();
    }

    void decReference()
    {
      // Save local copy in case of deletion.
      VirtualMutex* mutex = mMutex;
      if (mutex)
        mutex->lock();
      VL_CHECK(mReferenceCount)
      --mReferenceCount;
      if (mReferenceCount == 0 && automaticDelete())
        delete this;
      if (mutex)
        mutex->unlock();
    }

    //! If set to true the Object is deleted when its reference count reaches 0
    void setAutomaticDelete(bool autodel_on) { mAutomaticDelete = autodel_on; }

    //! If set to true the Object is deleted when its reference count reaches 0
    bool automaticDelete() const { return mAutomaticDelete; }

    //! Casts an Object to the specified class.
    template<class T>
    T* as() { return dynamic_cast<T*>(this); }

    //! Casts an Object to the specified class.
    template<class T>
    const T* as() const { return dynamic_cast<const T*>(this); }

#if VL_OBJECT_USER_DATA
  public:
    void* userData() { return mUserData; }
    const void* userData() const { return mUserData; }
    void setUserData(void* user_data) { mUserData = user_data; }

  private:
    void* mUserData;
#endif

  protected:
    virtual ~Object();
    std::string mObjectName;

    VirtualMutex* mMutex;
    int mReferenceCount;
    bool mAutomaticDelete;

  // debugging facilities

  public:
  #if VL_DEBUG_LIVING_OBJECTS && !defined(NDEBUG)
    static std::set< Object* >* mDebug_LivingObjects;
    static std::set< Object* >* debug_living_objects() 
    { 
      if (!mDebug_LivingObjects)
        mDebug_LivingObjects = new std::set< Object* >;
      return mDebug_LivingObjects;
    }
  #endif
  };
  //------------------------------------------------------------------------------
  // ref
  //------------------------------------------------------------------------------
  /**
   * The ref<> class is used to reference-count an Object.
   * When the last ref<> that points to an Object is deallocated also the pointed Object is deallocated.
  */
  template<class T>
  class ref
  {
  public:
    ref(T* object=NULL)
    {
      mObject = object;
      if (mObject)
        mObject->incReference();
    }
    ref(const ref& other)
    {
      mObject = NULL;
      *this = other;
    }
    template<class T2> ref(const ref<T2>& other)
    {
      mObject = NULL;
      *this = other;
    }
    ~ref() 
    {
      if (mObject)
        mObject->decReference();
      mObject = NULL;
    }
    ref& operator=(const ref& other)
    {
      if (other)
        other->incReference();
      if (mObject)
        mObject->decReference();
      mObject = other.get();
      return *this;
    }
    template<class T2> ref& operator=(const ref<T2> other)
    {
      if (other)
        other->incReference();
      if (mObject)
        mObject->decReference();
      mObject = other.get();
      return *this;
    }
    ref& operator=(T* other)
    {
      if (other)
        other->incReference();
      if (mObject)
        mObject->decReference();
      mObject = other;
      return *this;
    }
    template<class T2> ref& operator=(T2* other)
    {
      if (other)
        other->incReference();
      if (mObject)
        mObject->decReference();
      mObject = other;
      return *this;
    }
    void swap(ref& other)
    {
      T* tmp = other.get();
      other = mObject; 
      mObject = tmp;
    }
    T* get() const { return mObject; }
    T* operator->() const { VL_CHECK(mObject); return mObject; }
    T& operator*() const { VL_CHECK(mObject); return *mObject; }
    bool operator<(const ref& other) const { return mObject < other.get(); }
    operator bool() const { return mObject != NULL; }

  protected:
    T* mObject;
  };
  // interaction with the other types
  template<class T1, class T2> inline bool operator==(const ref<T1> & o1, const ref<T2> & o2) { return o1.get() == o2.get(); }
  template<class T1, class T2> inline bool operator!=(const ref<T1> & o1, const ref<T2> & o2) { return o1.get() != o2.get(); }
  template<class T1, class T2> inline bool operator==(const ref<T1> & o1, T2 * o2) { return o1.get() == o2; }
  template<class T1, class T2> inline bool operator!=(const ref<T1> & o1, T2 * o2) { return o1.get() != o2; }
  template<class T1, class T2> inline bool operator==(T1 * o1, const ref<T2> & o2) { return o1 == o2.get(); }
  template<class T1, class T2> inline bool operator!=(T1 * o1, const ref<T2> & o2) { return o1 != o2.get(); }
}

#endif
