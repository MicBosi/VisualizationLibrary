/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2011, Michele Bosi                                             */
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

#ifndef TypInfo_INCLUDE_ONCE
#define TypInfo_INCLUDE_ONCE

/**
 * \file TypeInfo.hpp
 * Set of macros and templates implementing a simple portable RTTI system.
*/

//! Represents a class type.
struct TypeInfo
{
  TypeInfo(const char* name): Name(name) 
  {
    // just for debugging
    // printf("Initializing TypeInfo \"%s\" at %p\n", name, this);
  }
  const char* Name;
};

#define INSTRUMENT_BASE_CLASS(ClassName)                                                                    \
public:                                                                                                     \
  /* static functions */                                                                                    \
  /** Returns the name of the class. */                                                                     \
  static const char* staticName() { return #ClassName; }                                                    \
  /** Returns the TypeInfo of the class. */                                                                 \
  static const TypeInfo* staticType() { static const TypeInfo class_type(#ClassName); return &class_type; } \
                                                                                                            \
  /* virtual functions */                                                                                   \
  /** Returns the name of the object's class. */                                                            \
  virtual const char* className() const { return #ClassName; }                                              \
  /** Returns the TypeInfo of the object's class. */                                                        \
  virtual const TypeInfo* classType() const { return staticType(); }                                        \
  /** Returns \a true if \a type matches the object's class type. */                                        \
  virtual bool isOfType(const TypeInfo* type) const                                                         \
  {                                                                                                         \
    return type == staticType();                                                                            \
  }                                                                                                         \
private:

#define INSTRUMENT_CLASS(ClassName, BaseClass)                                                              \
private:                                                                                                    \
  typedef BaseClass super;                                                                                  \
public:                                                                                                     \
  /* static functions */                                                                                    \
  /** Returns the name of the class. */                                                                     \
  static const char* staticName() { return #ClassName; }                                                    \
  /** Returns the TypeInfo of the class. */                                                                 \
  static const TypeInfo* staticType() { static const TypeInfo class_type(#ClassName); return &class_type; } \
                                                                                                            \
  /* virtual functions */                                                                                   \
  /** Returns the name of the object's class. */                                                            \
  virtual const char* className() const { return #ClassName; }                                              \
  /** Returns the TypeInfo of the object's class. */                                                        \
  virtual const TypeInfo* classType() const { return staticType(); }                                        \
  /** Returns \a true if \a type matches the object's class type. */                                        \
  virtual bool isOfType(const TypeInfo* type) const                                                         \
  {                                                                                                         \
    return type == staticType() || super::isOfType(type);                                                   \
  }                                                                                                         \
private:

#define INSTRUMENT_CLASS2(ClassName, BaseClass1, BaseClass2)                                                \
private:                                                                                                    \
  typedef BaseClass1 super1;                                                                                \
  typedef BaseClass2 super2;                                                                                \
public:                                                                                                     \
  /* static functions */                                                                                    \
  /** Returns the name of the class. */                                                                     \
  static const char* staticName() { return #ClassName; }                                                    \
  /** Returns the TypeInfo of the class. */                                                                 \
  static const TypeInfo* staticType() { static const TypeInfo class_type(#ClassName); return &class_type; } \
                                                                                                            \
  /* virtual functions */                                                                                   \
  /** Returns the name of the object's class. */                                                            \
  virtual const char* className() const { return #ClassName; }                                              \
  /** Returns the TypeInfo of the object's class. */                                                        \
  virtual const TypeInfo* classType() const { return staticType(); }                                        \
  /** Returns \a true if \a type matches the object's class type. */                                        \
  virtual bool isOfType(const TypeInfo* type) const                                                         \
  {                                                                                                         \
    return type == staticType() || super1::isOfType(type) || super2::isOfType(type);                        \
  }                                                                                                         \
private:

template<class B, class A>
B* vl_cast(A obj)
{
  if(obj->isOfType(B::staticType()))
    return static_cast<B*>(obj);
  else
    return NULL;
}

template<class B, class A>
const B* vl_const_cast(const A obj)
{
  if(obj->isOfType(B::staticType()))
    return static_cast<const B*>(obj);
  else
    return NULL;
}

/******************************************************************************
 USAGE EXAMPLES 
*******************************************************************************

class Base
{
  INSTRUMENT_BASE_CLASS(Base)
};

class ClassA: public Base
{
  INSTRUMENT_CLASS(vl::ClassA, Base)
};

class ClassB: public ClassA
{
  INSTRUMENT_CLASS(vl::ClassB, ClassA)
};

class ClassC: public Base
{
  INSTRUMENT_CLASS(vl::ClassC, Base)
};

class ClassD: public ClassC
{
  INSTRUMENT_CLASS(vl::ClassD, ClassC)
};

class ClassBD: public ClassB, public ClassD
{
  INSTRUMENT_CLASS2(vl::ClassBD, ClassB, ClassD)
};

*******************************************************************************/

#endif
