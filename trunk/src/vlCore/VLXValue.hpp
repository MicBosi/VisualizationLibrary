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

#ifndef VLXValue_INCLUDE_ONCE
#define VLXValue_INCLUDE_ONCE

#include <vlCore/VLXVisitor.hpp>

namespace vl
{
  class VLXTaggedValue: public Object
  {
    VL_INSTRUMENT_CLASS(vl::VLXTaggedValue, Object)

  public:
    VLXTaggedValue(const char* tag=NULL): mLineNumber(0) 
    {
      if (tag)
        mTag = tag;
    }

    virtual ~VLXTaggedValue() {}

    int lineNumber() const { return mLineNumber; }

    void setLineNumber(int line) { mLineNumber = line; }

    virtual void acceptVisitor(VLXVisitor*) = 0;
  
    void setTag(const char* tag) { mTag = tag; }

    const std::string& tag() const { return mTag; }

  private:
    std::string mTag;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // RawtextBlock
  //-----------------------------------------------------------------------------
  class VLXRawtextBlock: public VLXTaggedValue 
  { 
    VL_INSTRUMENT_CLASS(vl::VLXRawtextBlock, VLXTaggedValue)

  public:
    VLXRawtextBlock(const char* tag=NULL, const char* value=NULL): VLXTaggedValue(tag) 
    {
      if (value)
        mValue = value;
    }

    virtual void acceptVisitor(VLXVisitor* v) { v->visitRawtextBlock(this); }

    std::string& value() { return mValue; }

    const std::string& value() const { return mValue; }

    void setValue(const char* value) { mValue = value; }

  private:
    std::string mValue;
  };
  //-----------------------------------------------------------------------------
  // Arrays
  //-----------------------------------------------------------------------------
  class VLXArray: public VLXTaggedValue 
  { 
    VL_INSTRUMENT_CLASS(vl::VLXArray, VLXTaggedValue)

  public:
    VLXArray(const char* tag=NULL): VLXTaggedValue(tag) {}

  };
  //-----------------------------------------------------------------------------
  class VLXArrayInteger: public VLXArray
  {
    VL_INSTRUMENT_CLASS(vl::VLXArrayInteger, VLXArray)

  public:
    typedef long long scalar_type;

  public:
    VLXArrayInteger(const char* tag=NULL): VLXArray(tag) { }
    
    virtual void acceptVisitor(VLXVisitor* v) { v->visitArray(this); }

    std::vector<long long>& value() { return mValue; }
    
    const std::vector<long long>& value() const { return mValue; }

    long long* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }
    
    const long long* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

    template<typename T> void copyTo(T*ptr) const { for(size_t i=0; i<mValue.size(); ++i, ++ptr) *ptr = (T)mValue[i]; }

    template<typename T> void copyFrom(const T*ptr) { for(size_t i=0; i<mValue.size(); ++i, ++ptr) mValue[i] = (scalar_type)*ptr; }

  private:
    std::vector<long long> mValue;
  };
  //-----------------------------------------------------------------------------
  class VLXArrayReal: public VLXArray
  {
    VL_INSTRUMENT_CLASS(vl::VLXArrayReal, VLXArray)

  public:
    typedef double scalar_type;

  public:
    VLXArrayReal(const char* tag=NULL): VLXArray(tag) { }
    
    virtual void acceptVisitor(VLXVisitor* v) { v->visitArray(this); }

    std::vector<double>& value() { return mValue; }
    
    const std::vector<double>& value() const { return mValue; }

    double* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const double* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

    template<typename T> void copyTo(T*ptr) const { for(size_t i=0; i<mValue.size(); ++i, ++ptr) *ptr = (T)mValue[i]; }

    template<typename T> void copyFrom(const T*ptr) { for(size_t i=0; i<mValue.size(); ++i, ++ptr) mValue[i] = (scalar_type)*ptr; }

  public:
    std::vector<double> mValue;
  };
  //-----------------------------------------------------------------------------
  /*
  class VLXArrayString: public VLXArray
  {
    VL_INSTRUMENT_CLASS(vl::VLXArrayString, VLXArray)

  public:
    VLXArrayString(const char* tag=NULL): VLXArray(tag) { }

    virtual void acceptVisitor(VLXVisitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class VLXArrayIdentifier: public VLXArray
  {
    VL_INSTRUMENT_CLASS(vl::VLXArrayIdentifier, VLXArray)

  public:
    VLXArrayIdentifier(const char* tag=NULL): VLXArray(tag) { }

    virtual void acceptVisitor(VLXVisitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class VLXArrayUID: public VLXArray
  {
    VL_INSTRUMENT_CLASS(vl::VLXArrayUID, VLXArray)

  public:
    VLXArrayUID(const char* tag=NULL): VLXArray(tag) { }

    virtual void acceptVisitor(VLXVisitor* v) { v->visitArray(this); }

    class Value
    {
    public:
      Value(const char* uid): mUID(uid) {}

      void setUID(const char* uid) { mUID = uid; }

      const char* uid() const { return mUID.c_str(); }

      void setStructure(VLXStructure* obj) { mObj = obj; }

      VLXStructure* object() { return mObj.get(); }

      const VLXStructure* object() const { return mObj.get(); }

    private:
      std::string mUID; // the UID string
      ref<VLXStructure> mObj; // the linked object
    };

    std::vector<Value>& value() { return mValue; }

    const std::vector<Value>& value() const { return mValue; }

    Value* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const Value* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<Value> mValue;
  };
  */
  //-----------------------------------------------------------------------------
  // VLXValue
  //-----------------------------------------------------------------------------
  class VLXValue
  {
  public:
    enum EType 
    {
      Bool,
      Integer,
      Real,
      String,
      Identifier,
      UID,
      RawtextBlock,
      List,
      Structure,
      ArrayInteger,
      ArrayReal
      /*
      ArrayString,
      ArrayIdentifier,
      ArrayUID,
      */
    };

  private:
    VLCORE_EXPORT void release();

  public:
    VLXValue()
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
    }

    VLXValue(VLXStructure* obj)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setStructure(obj);
    }

    VLXValue(VLXList* list)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setList(list);
    }

    VLXValue(VLXRawtextBlock* rawtext)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setRawtextBlock(rawtext);
    }

    VLXValue(VLXArrayInteger* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayInteger(arr);
    }

    VLXValue(VLXArrayReal* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayReal(arr);
    }

    /*
    VLXValue(VLXArrayString* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayString(arr);
    }

    VLXValue(VLXArrayIdentifier* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayIdentifier(arr);
    }

    VLXValue(VLXArrayUID* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayUID(arr);
    }
    */

    VLXValue(long long i)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = i;
    }

    VLXValue(double d)
    {
      mLineNumber = 0;
      mType = Real;
      mUnion.mReal  = d;
    }

    VLXValue(const char* str, EType type)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      switch(type)
      {
      case String: setString(str); break;
      case Identifier: setIdentifier(str); break;
      case UID: setUID(str); break;
      default:
        VL_TRAP();
        break;
      }
    }

    VLXValue(bool boolean)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setBool(boolean);
    }

    VLXValue(const VLXValue& other)
    {
      mType = Integer;
      mUnion.mInteger = 0;
      mLineNumber = 0;

      *this = other;
    }

    ~VLXValue() { release(); }

    VLCORE_EXPORT VLXValue& operator=(const VLXValue& other);

    EType type() const { return mType; }

    // object

    VLCORE_EXPORT VLXStructure* setStructure(VLXStructure*);

    VLXStructure* getStructure() { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    const VLXStructure* getStructure() const { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    // list

    VLCORE_EXPORT VLXList* setList(VLXList*);

    VLXList* getList() { VL_CHECK(mType == List); return mUnion.mList; }

    const VLXList* getList() const { VL_CHECK(mType == List); return mUnion.mList; }

    // rawtext block

    VLCORE_EXPORT VLXRawtextBlock* setRawtextBlock(VLXRawtextBlock*);

    VLXRawtextBlock* getRawtextBlock() { VL_CHECK(mType == RawtextBlock); return mUnion.mRawtextBlock; }

    const VLXRawtextBlock* getRawtextBlock() const { VL_CHECK(mType == RawtextBlock); return mUnion.mRawtextBlock; }

    // array

    VLCORE_EXPORT VLXArray*           setArray(VLXArray*);
    VLCORE_EXPORT VLXArrayInteger*    setArrayInteger(VLXArrayInteger*);
    VLCORE_EXPORT VLXArrayReal*       setArrayReal(VLXArrayReal*);
    /*
    VLCORE_EXPORT VLXArrayString*     setArrayString(VLXArrayString*);
    VLCORE_EXPORT VLXArrayIdentifier* setArrayIdentifier(VLXArrayIdentifier*);
    VLCORE_EXPORT VLXArrayUID*        setArrayUID(VLXArrayUID*);
    */

    /*
    VLXArrayString* getArrayString() { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<VLXArrayString>(); }
    const VLXArrayString* getArrayString() const { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<VLXArrayString>(); }

    VLXArrayIdentifier* getArrayIdentifier() { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<VLXArrayIdentifier>(); }
    const VLXArrayIdentifier* getArrayIdentifier() const { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<VLXArrayIdentifier>(); }

    VLXArrayUID* getArrayUID() { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<VLXArrayUID>(); }
    const VLXArrayUID* getArrayUID() const { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<VLXArrayUID>(); }
    */

    VLXArrayInteger* getArrayInteger() { VL_CHECK(mType == ArrayInteger); return mUnion.mArray->as<VLXArrayInteger>(); }
    const VLXArrayInteger* getArrayInteger() const { VL_CHECK(mType == ArrayInteger); return mUnion.mArray->as<VLXArrayInteger>(); }

    VLXArrayReal* getArrayReal() { VL_CHECK(mType == ArrayReal); return mUnion.mArray->as<VLXArrayReal>(); }
    const VLXArrayReal* getArrayReal() const { VL_CHECK(mType == ArrayReal); return mUnion.mArray->as<VLXArrayReal>(); }

    // string

    const std::string& setString(const char* str)
    {
      release();
      mType = String;
      mUnion.mString = new std::string(str);
      return *mUnion.mString;
    }

    const std::string& getString() const { VL_CHECK(mType == String); return *mUnion.mString; }

    // identifier

    const std::string& setIdentifier(const char* str)
    {
      release();
      mType = Identifier;
      mUnion.mString = new std::string(str);
      return *mUnion.mString;
    }

    const std::string& getIdentifier() const { VL_CHECK(mType == Identifier); return *mUnion.mString; }

    // uid

    const std::string& setUID(const char* str)
    {
      release();
      mType = UID;
      mUnion.mString = new std::string(str);
      return *mUnion.mString;
    }

    const std::string& getUID() const { VL_CHECK(mType == UID); return *mUnion.mString; }

    // integer

    long long  setInteger(long long val)
    {
      release();
      mType = Integer;
      return mUnion.mInteger = val;
    }

    long long getInteger() const { VL_CHECK(mType == Integer); return mUnion.mInteger; }
    
    // floating point

    double setReal(double val)
    {
      release();
      mType = Real;
      return mUnion.mReal = val;
    }

    double getReal() const { VL_CHECK(mType == Real); return mUnion.mReal; }

    // bool

    bool setBool(bool val)
    {
      release();
      mType = Bool;
      return mUnion.mBool = val;
    }

    bool getBool() const { VL_CHECK(mType == Bool); return mUnion.mBool; }

    int lineNumber() const { return mLineNumber; }

    void setLineNumber(int line) { mLineNumber = line; }

  private:
    union
    {
      bool mBool;
      long long mInteger;
      double mReal;
      std::string* mString;
      VLXStructure* mStructure;
      VLXList* mList;
      VLXArray* mArray;
      VLXRawtextBlock* mRawtextBlock;
    } mUnion;

    EType mType;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // VLXStructure
  //-----------------------------------------------------------------------------
  class VLXStructure: public VLXTaggedValue
  {
    VL_INSTRUMENT_CLASS(vl::VLXStructure, VLXTaggedValue)

  public:
    VLXStructure()
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
    }

    VLXStructure(const char* tag)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
      setTag(tag);
    }

    VLXStructure(const char* tag, const std::string& uid)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID(uid.c_str());
      setTag(tag);
    }

    virtual void acceptVisitor(VLXVisitor* v) { v->visitStructure(this); }

    VLXStructure& operator<<(const char* str)
    {
      value().resize( value().size() + 1 );
      value().back().setKey(str);
      return *this;
    }

    VLXStructure& operator<<(const VLXValue& val)
    {
      value().back().setValue(val);
      return *this;
    }

    class Value
    {
      friend class VLXStructure;

    public:
      Value() {}
      Value(const char* key, VLXValue value): mKey(key), mValue(value) {}

      std::string& key() { return mKey; }
      const std::string& key() const { return mKey; }
      void setKey(const char* key) { mKey = key; }

      VLXValue& value() { return mValue; }
      const VLXValue& value() const { return mValue; }
      void setValue(const VLXValue& value) { mValue = value; }

    private:
      std::string mKey;
      VLXValue mValue;
    };

    void setUID(const char* uid) { mUID = uid; }

    const std::string& uid() const { return mUID; }

    std::vector<Value>& value() { return mKeyValue; }

    const std::vector<Value>& value() const { return mKeyValue; }

    // mic fixme: we can speed this guys up with multimaps
    VLXValue* getValue(const char* key)
    {
      for(size_t i=0; i<mKeyValue.size(); ++i)
        if (mKeyValue[i].key() == key)
          return &mKeyValue[i].value();
      return NULL;
    }

    const VLXValue* getValue(const char* key) const
    {
      for(size_t i=0; i<mKeyValue.size(); ++i)
        if (mKeyValue[i].key() == key)
          return &mKeyValue[i].value();
      return NULL;
    }

  private:
    std::string mUID;
    std::vector<Value> mKeyValue;
  };
  //-----------------------------------------------------------------------------
  // VLXList
  //-----------------------------------------------------------------------------
  class VLXList: public VLXTaggedValue
  {
    VL_INSTRUMENT_CLASS(vl::VLXList, VLXTaggedValue)

  public:
    VLXList(const char* tag=NULL): VLXTaggedValue(tag)
    {
      // mic fixme: reenable
      // mValue.reserve(16);
    }

    VLXList& operator<<(const VLXValue& val)
    {
      value().push_back( val );
      return *this;
    }

    virtual void acceptVisitor(VLXVisitor* v) { v->visitList(this); }

    std::vector< VLXValue >& value() { return mValue; }

    const std::vector< VLXValue >& value() const { return mValue; }

  private:
    std::vector< VLXValue > mValue;
  };
}

#endif
