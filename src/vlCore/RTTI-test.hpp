#pragma once

struct TypeInfo
{
  TypeInfo(const char* name): Name(name) 
  {
    printf("Initializing TypeInfo \"%s\" at %p\n", name, this);
  }
  const char* Name;
};

#define INSTRUMENT_CLASS(ClassName, BaseClass)                                                              \
private:                                                                                                    \
  typedef BaseClass super;                                                                                  \
public:                                                                                                     \
  /* static functions */                                                                                    \
  static const char* className() { return #ClassName; }                                                     \
  static const TypeInfo* staticType() { static const TypeInfo class_type(#ClassName); return &class_type; }  \
  /* virtual functions */                                                                                   \
  virtual const char* objectClassName() const { return #ClassName; }                                        \
  virtual const TypeInfo* objectType() const { return staticType(); }                                   \
  virtual bool isOfType(const TypeInfo* type) const                                                         \
  {                                                                                                         \
    return type == staticType() || super::isOfType(type);                                                    \
  }                                                                                                         \
private:

#define INSTRUMENT_BASE_CLASS(ClassName)                                                                    \
public:                                                                                                     \
  /* static functions */                                                                                    \
  static const char* className() { return #ClassName; }                                                     \
  static const TypeInfo* staticType() { static const TypeInfo class_type(#ClassName); return &class_type; }  \
  /* virtual functions */                                                                                   \
  virtual const char* objectClassName() const { return #ClassName; }                                        \
  virtual const TypeInfo* objectType() const { return staticType(); }                                   \
  virtual bool isOfType(const TypeInfo* type) const                                                         \
  {                                                                                                         \
    return type == staticType();                                                                             \
  }                                                                                                         \
private:

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
