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

#ifndef VLX_INCLUDE_ONCE
#define VLX_INCLUDE_ONCE

#include <vlCore/Object.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/BufferedStream.hpp>
#include <vlCore/Vector4.hpp>
#include <map>
#include <set>
#include <cstdlib>
#include <cstdarg>
#include <cstring>

// mic fixme
#ifdef _MSC_VER
#define atoll _atoi64
#endif

namespace vl
{
  //-----------------------------------------------------------------------------
  class VLX_Structure;
  class VLX_List;
  class VLX_RawtextBlock;
  class VLX_Array;
  class VLX_ArrayInteger;
  class VLX_ArrayReal;
  /*
  class VLX_ArrayString;
  class VLX_ArrayIdentifier;
  class VLX_ArrayUID;
  */
  //-----------------------------------------------------------------------------
  class VLX_Visitor: public Object
  {
  public:
    virtual void visitStructure(VLX_Structure*) {}
    virtual void visitList(VLX_List*) {}
    virtual void visitRawtextBlock(VLX_RawtextBlock*) {}
    virtual void visitArray(VLX_ArrayInteger*) {}
    virtual void visitArray(VLX_ArrayReal*) {}
    /*
    virtual void visitArray(VLX_ArrayString*) {}
    virtual void visitArray(VLX_ArrayIdentifier*) {}
    virtual void visitArray(VLX_ArrayUID*) {}
    */

    bool isVisited(void* node)
    {
      std::set< void* >::iterator it = mVisited.find(node);
      if (it == mVisited.end())
      {
        mVisited.insert(node);
        return false;
      }
      else 
        return true;
    }
    
    void resetVisitedNodes() { mVisited.clear(); };

  private:
    std::set< void* > mVisited;
  };
  //-----------------------------------------------------------------------------
  class VLX_TaggedValue: public Object
  {
    VL_INSTRUMENT_CLASS(vl::VLX_TaggedValue, Object)

  public:
    VLX_TaggedValue(const char* tag=NULL): mLineNumber(0) 
    {
      if (tag)
        mTag = tag;
    }

    virtual ~VLX_TaggedValue() {}

    int lineNumber() const { return mLineNumber; }

    void setLineNumber(int line) { mLineNumber = line; }

    virtual void acceptVisitor(VLX_Visitor*) = 0;
  
    void setTag(const char* tag) { mTag = tag; }

    const std::string& tag() const { return mTag; }

  private:
    std::string mTag;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // RawtextBlock
  //-----------------------------------------------------------------------------
  class VLX_RawtextBlock: public VLX_TaggedValue 
  { 
    VL_INSTRUMENT_CLASS(vl::VLX_RawtextBlock, VLX_TaggedValue)

  public:
    VLX_RawtextBlock(const char* tag=NULL, const char* value=NULL): VLX_TaggedValue(tag) 
    {
      if (value)
        mValue = value;
    }

    virtual void acceptVisitor(VLX_Visitor* v) { v->visitRawtextBlock(this); }

    std::string& value() { return mValue; }

    const std::string& value() const { return mValue; }

    void setValue(const char* value) { mValue = value; }

  private:
    std::string mValue;
  };
  //-----------------------------------------------------------------------------
  // Arrays
  //-----------------------------------------------------------------------------
  class VLX_Array: public VLX_TaggedValue 
  { 
    VL_INSTRUMENT_CLASS(vl::VLX_Array, VLX_TaggedValue)

  public:
    VLX_Array(const char* tag=NULL): VLX_TaggedValue(tag) {}

  };
  //-----------------------------------------------------------------------------
  class VLX_ArrayInteger: public VLX_Array
  {
    VL_INSTRUMENT_CLASS(vl::VLX_ArrayInteger, VLX_Array)

  public:
    typedef long long scalar_type;

  public:
    VLX_ArrayInteger(const char* tag=NULL): VLX_Array(tag) { }
    
    virtual void acceptVisitor(VLX_Visitor* v) { v->visitArray(this); }

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
  class VLX_ArrayReal: public VLX_Array
  {
    VL_INSTRUMENT_CLASS(vl::VLX_ArrayReal, VLX_Array)

  public:
    typedef double scalar_type;

  public:
    VLX_ArrayReal(const char* tag=NULL): VLX_Array(tag) { }
    
    virtual void acceptVisitor(VLX_Visitor* v) { v->visitArray(this); }

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
  class VLX_ArrayString: public VLX_Array
  {
    VL_INSTRUMENT_CLASS(vl::VLX_ArrayString, VLX_Array)

  public:
    VLX_ArrayString(const char* tag=NULL): VLX_Array(tag) { }

    virtual void acceptVisitor(VLX_Visitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class VLX_ArrayIdentifier: public VLX_Array
  {
    VL_INSTRUMENT_CLASS(vl::VLX_ArrayIdentifier, VLX_Array)

  public:
    VLX_ArrayIdentifier(const char* tag=NULL): VLX_Array(tag) { }

    virtual void acceptVisitor(VLX_Visitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class VLX_ArrayUID: public VLX_Array
  {
    VL_INSTRUMENT_CLASS(vl::VLX_ArrayUID, VLX_Array)

  public:
    VLX_ArrayUID(const char* tag=NULL): VLX_Array(tag) { }

    virtual void acceptVisitor(VLX_Visitor* v) { v->visitArray(this); }

    class Value
    {
    public:
      Value(const char* uid): mUID(uid) {}

      void setUID(const char* uid) { mUID = uid; }

      const char* uid() const { return mUID.c_str(); }

      void setStructure(VLX_Structure* obj) { mObj = obj; }

      VLX_Structure* object() { return mObj.get(); }

      const VLX_Structure* object() const { return mObj.get(); }

    private:
      std::string mUID; // the UID string
      ref<VLX_Structure> mObj; // the linked object
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
  // VLX_Value
  //-----------------------------------------------------------------------------
  class VLX_Value
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
    VLX_Value()
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
    }

    VLX_Value(VLX_Structure* obj)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setStructure(obj);
    }

    VLX_Value(VLX_List* list)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setList(list);
    }

    VLX_Value(VLX_RawtextBlock* rawtext)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setRawtextBlock(rawtext);
    }

    VLX_Value(VLX_ArrayInteger* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayInteger(arr);
    }

    VLX_Value(VLX_ArrayReal* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayReal(arr);
    }

    /*
    VLX_Value(VLX_ArrayString* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayString(arr);
    }

    VLX_Value(VLX_ArrayIdentifier* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayIdentifier(arr);
    }

    VLX_Value(VLX_ArrayUID* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayUID(arr);
    }
    */

    VLX_Value(long long i)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = i;
    }

    VLX_Value(double d)
    {
      mLineNumber = 0;
      mType = Real;
      mUnion.mReal  = d;
    }

    VLX_Value(const char* str, EType type)
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

    VLX_Value(bool boolean)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setBool(boolean);
    }

    VLX_Value(const VLX_Value& other)
    {
      mType = Integer;
      mUnion.mInteger = 0;
      mLineNumber = 0;

      *this = other;
    }

    ~VLX_Value() { release(); }

    VLCORE_EXPORT VLX_Value& operator=(const VLX_Value& other);

    EType type() const { return mType; }

    // object

    VLCORE_EXPORT VLX_Structure* setStructure(VLX_Structure*);

    VLX_Structure* getStructure() { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    const VLX_Structure* getStructure() const { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    // list

    VLCORE_EXPORT VLX_List* setList(VLX_List*);

    VLX_List* getList() { VL_CHECK(mType == List); return mUnion.mList; }

    const VLX_List* getList() const { VL_CHECK(mType == List); return mUnion.mList; }

    // rawtext block

    VLCORE_EXPORT VLX_RawtextBlock* setRawtextBlock(VLX_RawtextBlock*);

    VLX_RawtextBlock* getRawtextBlock() { VL_CHECK(mType == RawtextBlock); return mUnion.mRawtextBlock; }

    const VLX_RawtextBlock* getRawtextBlock() const { VL_CHECK(mType == RawtextBlock); return mUnion.mRawtextBlock; }

    // array

    VLCORE_EXPORT VLX_Array*           setArray(VLX_Array*);
    VLCORE_EXPORT VLX_ArrayInteger*    setArrayInteger(VLX_ArrayInteger*);
    VLCORE_EXPORT VLX_ArrayReal*       setArrayReal(VLX_ArrayReal*);
    /*
    VLCORE_EXPORT VLX_ArrayString*     setArrayString(VLX_ArrayString*);
    VLCORE_EXPORT VLX_ArrayIdentifier* setArrayIdentifier(VLX_ArrayIdentifier*);
    VLCORE_EXPORT VLX_ArrayUID*        setArrayUID(VLX_ArrayUID*);
    */

    /*
    VLX_ArrayString* getArrayString() { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<VLX_ArrayString>(); }
    const VLX_ArrayString* getArrayString() const { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<VLX_ArrayString>(); }

    VLX_ArrayIdentifier* getArrayIdentifier() { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<VLX_ArrayIdentifier>(); }
    const VLX_ArrayIdentifier* getArrayIdentifier() const { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<VLX_ArrayIdentifier>(); }

    VLX_ArrayUID* getArrayUID() { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<VLX_ArrayUID>(); }
    const VLX_ArrayUID* getArrayUID() const { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<VLX_ArrayUID>(); }
    */

    VLX_ArrayInteger* getArrayInteger() { VL_CHECK(mType == ArrayInteger); return mUnion.mArray->as<VLX_ArrayInteger>(); }
    const VLX_ArrayInteger* getArrayInteger() const { VL_CHECK(mType == ArrayInteger); return mUnion.mArray->as<VLX_ArrayInteger>(); }

    VLX_ArrayReal* getArrayReal() { VL_CHECK(mType == ArrayReal); return mUnion.mArray->as<VLX_ArrayReal>(); }
    const VLX_ArrayReal* getArrayReal() const { VL_CHECK(mType == ArrayReal); return mUnion.mArray->as<VLX_ArrayReal>(); }

    // string

    const char* setString(const char* str)
    {
      release();
      mType = String;
      return mUnion.mString = strdup(str);
    }

    const char* getString() const { VL_CHECK(mType == String); return mUnion.mString; }

    // identifier

    const char* setIdentifier(const char* str)
    {
      release();
      mType = Identifier;
      return mUnion.mString = strdup(str);
    }

    const char* getIdentifier() const { VL_CHECK(mType == Identifier); return mUnion.mString; }

    // uid

    const char* setUID(const char* str)
    {
      release();
      mType = UID;
      return mUnion.mString = strdup(str);
    }

    const char* getUID() const { VL_CHECK(mType == UID); return mUnion.mString; }

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
      const char* mString;
      VLX_Structure* mStructure;
      VLX_List* mList;
      VLX_Array* mArray;
      VLX_RawtextBlock* mRawtextBlock;
    } mUnion;

  private:
    EType mType;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // VLX_Structure
  //-----------------------------------------------------------------------------
  class VLX_Structure: public VLX_TaggedValue
  {
    VL_INSTRUMENT_CLASS(vl::VLX_Structure, VLX_TaggedValue)

  public:
    VLX_Structure()
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
    }

    VLX_Structure(const char* tag)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
      setTag(tag);
    }

    VLX_Structure(const char* tag, const std::string& uid)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID(uid.c_str());
      setTag(tag);
    }

    virtual void acceptVisitor(VLX_Visitor* v) { v->visitStructure(this); }

    VLX_Structure& operator<<(const char* str)
    {
      value().resize( value().size() + 1 );
      value().back().setKey(str);
      return *this;
    }

    VLX_Structure& operator<<(const VLX_Value& val)
    {
      value().back().setValue(val);
      return *this;
    }

    class Value
    {
      friend class VLX_Structure;

    public:
      Value() {}
      Value(const char* key, VLX_Value value): mKey(key), mValue(value) {}

      std::string& key() { return mKey; }
      const std::string& key() const { return mKey; }
      void setKey(const char* key) { mKey = key; }

      VLX_Value& value() { return mValue; }
      const VLX_Value& value() const { return mValue; }
      void setValue(const VLX_Value& value) { mValue = value; }

    private:
      std::string mKey;
      VLX_Value mValue;
    };

    void setUID(const char* uid) { mUID = uid; }

    const std::string& uid() const { return mUID; }

    std::vector<Value>& value() { return mKeyValue; }

    const std::vector<Value>& value() const { return mKeyValue; }

    // mic fixme: we can speed this guys up with multimaps
    VLX_Value* getValue(const char* key)
    {
      for(size_t i=0; i<mKeyValue.size(); ++i)
        if (mKeyValue[i].key() == key)
          return &mKeyValue[i].value();
      return NULL;
    }

    const VLX_Value* getValue(const char* key) const
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
  // VLX_List
  //-----------------------------------------------------------------------------
  class VLX_List: public VLX_TaggedValue
  {
    VL_INSTRUMENT_CLASS(vl::VLX_List, VLX_TaggedValue)

  public:
    VLX_List(const char* tag=NULL): VLX_TaggedValue(tag)
    {
      // mic fixme: reenable
      // mValue.reserve(16);
    }

    VLX_List& operator<<(const VLX_Value& val)
    {
      value().push_back( val );
      return *this;
    }

    virtual void acceptVisitor(VLX_Visitor* v) { v->visitList(this); }

    std::vector< VLX_Value >& value() { return mValue; }

    const std::vector< VLX_Value >& value() const { return mValue; }

  private:
    std::vector< VLX_Value > mValue;
  };
  //-----------------------------------------------------------------------------
  // VLX_TextExportVisitor
  //-----------------------------------------------------------------------------
  class VLX_TextExportVisitor: public VLX_Visitor
  {
  public:
    VLX_TextExportVisitor()
    {
      mIndent = 0;
      mAssign = false;
      mUIDSet = NULL;
      mFormatBuffer.resize(4096);
      setHeader();
    }

    bool isUsed(const std::string& uid)
    {
      if (mUIDSet)
      {
        std::map< std::string, int >::iterator it = mUIDSet->find(uid);
        if (it != mUIDSet->end())
          return it->second > 1;
        else
        {
          // should not happen
          VL_TRAP()
          return false;
        }
      }
      else
        return true;
    }

    void indent()
    {
      if (mAssign)
        mAssign = false;
      else
      {
        switch(mIndent)
        {
        case 0: break;
        case 1: output("\t"); break;
        case 2: output("\t\t"); break;
        case 3: output("\t\t\t"); break;
        case 4: output("\t\t\t\t"); break;
        case 5: output("\t\t\t\t\t"); break;
        case 6: output("\t\t\t\t\t\t"); break;
        case 7: output("\t\t\t\t\t\t\t"); break;
        case 8: output("\t\t\t\t\t\t\t\t"); break;
        case 9: output("\t\t\t\t\t\t\t\t\t"); break;
        default:
          output("\t\t\t\t\t\t\t\t\t");
          for(int i=9; i<mIndent; ++i)
            output("\t");
        }
      }
    }

    void format(const char* fmt, ...)
    {
      mFormatBuffer[0] = 0;

      va_list ap;
      va_start(ap, fmt);
      vsnprintf(&mFormatBuffer[0], mFormatBuffer.size(), fmt, ap);
      va_end(ap);

      output(&mFormatBuffer[0]);
    }

    virtual void visitStructure(VLX_Structure* obj)
    {
      // mic fixme: check this
      if (isVisited(obj))
      {
        indent(); format("%s\n", obj->uid().c_str());
        return;
      }

      // header tag
      if (obj->tag().empty())
      {
        if (mAssign)
        {
          mAssign = false;
          output("\n");
        }
      }
      else
      {
        indent();
        format("%s", obj->tag().c_str()); 
        output("\n");
      }
      indent();
      output("{\n");

      mIndent++;
      if ( obj->uid().length() && obj->uid() != "#NULL" && isUsed(obj->uid()) )
      {
        indent(); format("ID = %s\n", obj->uid().c_str());
      }

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        indent(); format("%s = ", obj->value()[i].key().c_str());
        switch(obj->value()[i].value().type())
        {

        case VLX_Value::Structure:
          mAssign = true;
          obj->value()[i].value().getStructure()->acceptVisitor(this);
          break;

        case VLX_Value::List:
          mAssign = true;
          obj->value()[i].value().getList()->acceptVisitor(this);
          break;

        /*
        case VLX_Value::ArrayString:
          mAssign = true;
          obj->value()[i].value().getArrayString()->acceptVisitor(this);
          break;

        case VLX_Value::ArrayIdentifier:
          mAssign = true;
          obj->value()[i].value().getArrayIdentifier()->acceptVisitor(this);
          break;

        case VLX_Value::ArrayUID:
          mAssign = true;
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
          break;
        */

        case VLX_Value::ArrayInteger:
          mAssign = true;
          obj->value()[i].value().getArrayInteger()->acceptVisitor(this);
          break;

        case VLX_Value::ArrayReal:
          mAssign = true;
          obj->value()[i].value().getArrayReal()->acceptVisitor(this);
          break;

        case VLX_Value::RawtextBlock:
        {
          VLX_RawtextBlock* fblock = obj->value()[i].value().getRawtextBlock();
          if (!fblock->tag().empty())
            format("%s", fblock->tag().c_str());
          output("\n"); indent(); format("{<\n%s>}\n", rawtextEncode(fblock->value().c_str()).c_str());
        }
        break;

        case VLX_Value::String:
          format("\"%s\"\n", stringEncode(obj->value()[i].value().getString()).c_str());
          break;

        case VLX_Value::Identifier:
          format("%s\n", obj->value()[i].value().getIdentifier()); VL_CHECK( strlen(obj->value()[i].value().getIdentifier()) )
          break;

        case VLX_Value::UID:
          format("%s\n", obj->value()[i].value().getUID()); VL_CHECK( strlen(obj->value()[i].value().getUID()) )
          break;

        case VLX_Value::Bool:
          format("%s\n", obj->value()[i].value().getBool() ? "true" : "false");
          break;

        case VLX_Value::Integer:
          format("%lld\n", obj->value()[i].value().getInteger());
          break;

        case VLX_Value::Real:
          format("%Lf\n", obj->value()[i].value().getReal());
          break;
        }
      }
      mIndent--;
      indent(); output("}\n");
    }

    virtual void visitList(VLX_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VLX_TextExportVisitor: cycle detected on VLX_List.\n");
        return;
      }

      if (list->value().size() == 0)
      {
        indent(); output("[ ]\n");
        return;
      }

      // header tag
      if (list->tag().empty())
      {
        if (mAssign)
        {
          mAssign = false;
          output("\n");
        }
      }
      else
      {
        indent();
        format("%s", list->tag().c_str()); 
        output("\n");
      }
      indent();
      output("[\n");

      mIndent++;
      for(size_t i=0; i<list->value().size(); ++i)
      {
        switch(list->value()[i].type())
        {

        case VLX_Value::Structure:
          list->value()[i].getStructure()->acceptVisitor(this);
          break;

        case VLX_Value::List:
          list->value()[i].getList()->acceptVisitor(this);
          break;

        /*
        case VLX_Value::ArrayString:
          list->value()[i].getArrayString()->acceptVisitor(this);
          break;

        case VLX_Value::ArrayIdentifier:
          list->value()[i].getArrayIdentifier()->acceptVisitor(this);
          break;

        case VLX_Value::ArrayUID:
          list->value()[i].getArrayUID()->acceptVisitor(this);
          break;
        */

        case VLX_Value::ArrayInteger:
          list->value()[i].getArrayInteger()->acceptVisitor(this);
          break;

        case VLX_Value::ArrayReal:
          list->value()[i].getArrayReal()->acceptVisitor(this);
          break;

        case VLX_Value::String:
          indent(); format("\"%s\"\n", stringEncode(list->value()[i].getString()).c_str());
          break;

        case VLX_Value::Identifier:
          indent(); format("%s\n", list->value()[i].getIdentifier()); VL_CHECK( strlen(list->value()[i].getIdentifier()) )
          break;

        case VLX_Value::UID:
          indent(); format("%s\n", list->value()[i].getUID()); VL_CHECK( strlen(list->value()[i].getUID()) )
          break;

        case VLX_Value::RawtextBlock:
        {
          VLX_RawtextBlock* fblock = list->value()[i].getRawtextBlock();
          if (!fblock->tag().empty())
          {
            indent(); format("%s\n", fblock->tag().c_str());
          }
          indent(); format("{<\n%s>}\n", rawtextEncode(fblock->value().c_str()).c_str());
        }
        break;

        case VLX_Value::Bool:
          indent(); format("%s\n", list->value()[i].getBool() ? "true" : "false");
          break;

        case VLX_Value::Integer:
          indent(); format("%lld\n", list->value()[i].getInteger());
          break;

        case VLX_Value::Real:
          indent(); format("%Lf\n", list->value()[i].getReal());
          break;
        }
      }
      mIndent--;
      indent(); output("]\n");
    }

    virtual void visitArray(VLX_ArrayInteger* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      // output in chunks of 10 numbers
      int i = 0;
      int size = arr->value().size() - 10;
      for( ; i < size; i += 10)
      {
        format("%lld %lld %lld %lld %lld %lld %lld %lld %lld %lld ",
          arr->value()[i+0], arr->value()[i+1], arr->value()[i+2], arr->value()[i+3], arr->value()[i+4],
          arr->value()[i+5], arr->value()[i+6], arr->value()[i+7], arr->value()[i+8], arr->value()[i+9] );
      }
      for( ; i < (int)arr->value().size(); ++i )
        format("%lld ", arr->value()[i]);
      VL_CHECK( i == (int)arr->value().size() )
      output(")\n");
    }

    virtual void visitArray(VLX_ArrayReal* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      // output in chunks of 10 numbers
      int i = 0;
      int size = arr->value().size() - 10;
      for( ; i < size; i += 10)
      {
        format("%Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf %Lf ",
          arr->value()[i+0], arr->value()[i+1], arr->value()[i+2], arr->value()[i+3], arr->value()[i+4],
          arr->value()[i+5], arr->value()[i+6], arr->value()[i+7], arr->value()[i+8], arr->value()[i+9] );
      }
      for( ; i < (int)arr->value().size(); ++i )
        format("%Lf ", arr->value()[i]);
      VL_CHECK( i == (int)arr->value().size() )
      output(")\n");
    }

    /*
    virtual void visitArray(VLX_ArrayString* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        output(std::string("\"") + stringEncode(arr->value()[i].c_str()) + "\" ");
      output(")\n");
    }

    virtual void visitArray(VLX_ArrayIdentifier* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        format("%s ", arr->value()[i].c_str());
      output(")\n");
    }

    virtual void visitArray(VLX_ArrayUID* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        format("%s ", arr->value()[i].uid());
      output(")\n");
    }
    */

    std::string rawtextEncode(const char* str)
    {
      std::string out;
      out.reserve(32);

      for(size_t i=0; str[i]; ++i)
      {
        if ( str[i] == '}' && !out.empty() && out[ out.size()-1 ] == '>')
        {
          out.resize( out.size() - 1 );
          out += "\\>}";
        }
        else
          out.push_back( str[i] );
      }
      return out;
    }

    // mic fixme: test this both as input and as output!
    // support \xHH hex notation both input and output.
    std::string stringEncode(const char* str)
    {
      std::string out;
      for(size_t i=0; str[i]; ++i)
      {
        if (str[i] == '"')
          out += "\\\"";
        else
        if (str[i] == '\\')
          out += "\\\\";
        else
        if (str[i] == '\b')
          out += "\\b";
        else
        if (str[i] == '\f')
          out += "\\f";
        else
        if (str[i] == '\n')
          out += "\\n";
        else
        if (str[i] == '\r')
          out += "\\r";
        else
        if (str[i] == '\t')
          out += "\\t";
        else
          out += str[i];
      }
      return out;
    }

    const std::string& text() const { return mText; }

    std::string& text() { return mText; }

    virtual void output(const std::string& str)
    {
      output(str.c_str());
    }

    virtual void output(const char* str)
    {
      // printf(str);
      mText += str;
    }

    void setHeader()
    {
      mText = "VLX_TEXT version=100 encoding=ascii\n\n";
    }

    void setUIDSet(std::map< std::string, int >* uids) { mUIDSet = uids; }

    std::map< std::string, int >* uidSet() { return mUIDSet; }

    const std::map< std::string, int >* uidSet() const { return mUIDSet; }

  private:
    int mIndent;
    bool mAssign;
    std::string mText;
    std::map< std::string, int >* mUIDSet;
    std::vector<char> mFormatBuffer;
  };
  //-----------------------------------------------------------------------------
  // VLX_LinkMapperVisitor: 
  // Compiles the link-map which associates an VLX_Structure to it's UID.
  // Can be called multiple times
  //-----------------------------------------------------------------------------
  class VLX_LinkMapperVisitor: public VLX_Visitor
  {
  public:
    typedef enum 
    {
      NoError,
      DuplicateUID
    } EError;

  public:
    VLX_LinkMapperVisitor(std::map< std::string, ref<VLX_Structure> >* map)
    {
      mLinkMap = map;
      mError = NoError;
    }

    void setLinkMap(std::map< std::string, ref<VLX_Structure> >* map)
    {
      mLinkMap = map;
    }

    void declareUID(VLX_Structure* obj)
    {
      if (obj->uid() != "#NULL")
      {
        const std::map< std::string, ref<VLX_Structure> >::const_iterator it = mLinkMap->find(obj->uid());
        if (it == mLinkMap->end())
          (*mLinkMap)[obj->uid()] = obj;
        else
        {
          if ( it->second != obj )
          {
            mError = DuplicateUID;
            Log::error( Say("UID '%s' used by '%s' is already assigned to another node '%s'!\n") << obj->uid() << obj->tag() << it->second->tag() );
          }
        }
      }
    }

    virtual void visitStructure(VLX_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      declareUID(obj);

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == VLX_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == VLX_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
      }
    }

    virtual void visitList(VLX_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VLX_LinkMapperVisitor: cycle detected on VLX_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VLX_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        else
        if (list->value()[i].type() == VLX_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
      }
    }

    /*
    virtual void visitArray(VLX_ArrayString*)  {}

    virtual void visitArray(VLX_ArrayIdentifier*) {}

    virtual void visitArray(VLX_ArrayUID*) {}
    */

    virtual void visitArray(VLX_ArrayInteger*)  {}

    virtual void visitArray(VLX_ArrayReal*)  {}

    EError error() const { return mError; }

    void setError(EError err) { mError = err; }

  private:
    std::map< std::string, ref<VLX_Structure> >* mLinkMap;
    EError mError;
  };
  //-----------------------------------------------------------------------------
  // VLX_LinkVisitor:
  // Translates UIDs into VLX_Structures
  //-----------------------------------------------------------------------------
  class VLX_LinkVisitor: public VLX_Visitor
  {
  public:
    typedef enum 
    {
      NoError,
      UnresolvedUID
    } EError;

  public:
    VLX_LinkVisitor(const std::map< std::string, ref<VLX_Structure> >* map)
    {
      mLinkMap = map;
      mError = NoError;
    }

    void setLinkMap(const std::map< std::string, ref<VLX_Structure> >* map)
    {
      mLinkMap = map;
    }

    VLX_Structure* link(const std::string& uid)
    {
      VL_CHECK(mLinkMap)
      VL_CHECK(!uid.empty())
      std::map< std::string, ref<VLX_Structure> >::const_iterator it = mLinkMap->find(uid);
      if( it != mLinkMap->end() )
      {
        // this should never happen
        VL_CHECK(uid != "#NULL")

        // mic fixme: just for debugging
        printf( "- UID '%s' linked to '%s'.\n", uid.c_str(), it->second->tag().c_str() );
        return it->second.get_writable();
      }
      else
      {
        if (uid != "#NULL")
        {
          mError = UnresolvedUID;
          Log::error( Say("Could not link UID '%s' to anything!\n") << uid );
        }
        return NULL;
      }
    }

    virtual void visitStructure(VLX_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == VLX_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == VLX_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
        else
        /*
        if (obj->value()[i].value().type() == VLX_Value::ArrayUID)
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
        else
        */
        if (obj->value()[i].value().type() == VLX_Value::UID)
        {
          // transform UID -> Structure
          VLX_Structure* lnk_obj = link( obj->value()[i].value().getUID() );
          obj->value()[i].value().setStructure( lnk_obj );
        }
      }
    }

    virtual void visitList(VLX_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VLX_LinkVisitor: cycle detected on VLX_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VLX_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == VLX_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        /*
        if (list->value()[i].type() == VLX_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        */
        if (list->value()[i].type() == VLX_Value::UID)
        {
          // transform UID -> Structure
          VLX_Structure* lnk_obj = link( list->value()[i].getUID() );
          list->value()[i].setStructure( lnk_obj );
        }
      }
    }

    /*
    virtual void visitArray(VLX_ArrayString*)  {}

    virtual void visitArray(VLX_ArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        arr->value()[i].setStructure ( link(arr->value()[i].uid()) );
    }

    virtual void visitArray(VLX_ArrayIdentifier*) {}
    */

    virtual void visitArray(VLX_ArrayInteger*)  {}

    virtual void visitArray(VLX_ArrayReal*)  {}

    EError error() const { return mError; }

    void setError(EError err) { mError = err; }

  private:
    const std::map< std::string, ref<VLX_Structure> >* mLinkMap;
    EError mError;
  };
  //-----------------------------------------------------------------------------
  // VLX_UIDCollectorVisitor:
  // Sets to #NULL the UID of those objects that are not referenced by anybody.
  // Useful before exporting.
  //-----------------------------------------------------------------------------
  class VLX_UIDCollectorVisitor: public VLX_Visitor
  {
  public:
    VLX_UIDCollectorVisitor(): mUIDSet(NULL) {}

    virtual void visitStructure(VLX_Structure* obj)
    {
      if(!obj->uid().empty() && obj->uid() != "#NULL")
        (*mUIDSet)[obj->uid()]++;

      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        VLX_Structure::Value& keyval = obj->value()[i];
        if (keyval.value().type() == VLX_Value::Structure)
          keyval.value().getStructure()->acceptVisitor(this);
        else
        if (keyval.value().type() == VLX_Value::List)
          keyval.value().getList()->acceptVisitor(this);
        else
        /*
        if (keyval.value().type() == VLX_Value::ArrayUID)
          keyval.value().getArrayUID()->acceptVisitor(this);
        else
        */
        if (keyval.value().type() == VLX_Value::UID)
          (*mUIDSet)[keyval.value().getUID()]++;
      }
    }

    virtual void visitList(VLX_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VLX_UIDCollectorVisitor: cycle detected on VLX_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VLX_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == VLX_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        /*
        if (list->value()[i].type() == VLX_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        */
        if (list->value()[i].type() == VLX_Value::UID)
          (*mUIDSet)[list->value()[i].getUID()]++;
      }
    }

    /*
    virtual void visitArray(VLX_ArrayString*)  {}

    virtual void visitArray(VLX_ArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        (*mUIDSet)[arr->value()[i].uid()]++;
    }

    virtual void visitArray(VLX_ArrayIdentifier*) {}
    */

    virtual void visitArray(VLX_ArrayInteger*)  {}

    virtual void visitArray(VLX_ArrayReal*)  {}

    void setUIDSet(std::map< std::string, int >* uids) { mUIDSet = uids; }

    std::map< std::string, int >* uidSet() { return mUIDSet; }

    const std::map< std::string, int >* uidSet() const { return mUIDSet; }

  private:
    std::map< std::string, int >* mUIDSet;
  };
  //-----------------------------------------------------------------------------
  // Links several hierachies also resolving UIDs across them.
  //-----------------------------------------------------------------------------
  class VLX_Linker
  {
  public:
    void add(VLX_TaggedValue* module)
    {
      mModules.push_back(module);
    }

    bool link()
    {
      std::map< std::string, ref<VLX_Structure> > link_map;

      // map all the UIDs to the appropriate VLX_Structures
      VLX_LinkMapperVisitor link_mapper(&link_map);
      for(size_t i=0; i<mModules.size(); ++i)
        mModules[i]->acceptVisitor(&link_mapper);

      if (link_mapper.error())
        return false;

      // link all the UIDs to the associated VLX_Structure
      VLX_LinkVisitor linker(&link_map);
      for(size_t i=0; i<mModules.size(); ++i)
        mModules[i]->acceptVisitor(&linker);

      if (linker.error())
        return false;

      return true;
    }

    std::vector< ref<VLX_TaggedValue> >& modules() { return mModules; }

    const std::vector< ref<VLX_TaggedValue> >& modules() const { return mModules; }

  public:
    std::vector< ref<VLX_TaggedValue> > mModules;
  };
  //-----------------------------------------------------------------------------
  class VLX_Token
  {
  public:
    typedef enum
    {
      TOKEN_ERROR,
      TOKEN_EOF,

      LeftRoundBracket,   //  (
      RightRoundBracket,  //  )
      LeftSquareBracket,  //  [
      RightSquareBracket, //  ]
      LeftCurlyBracket,   //  {
      RightCurlyBracket,  //  }
      LeftFancyBracket,   //  {<
      RightFancyBracket,  //  >}
      Equals,             //  =
      String,             //  "nel mezzo del cammin di nostra vita\n"
      UID,                //  #unique_id123
      Identifier,         //  Identifier_123
      Boolean,            //  true | false
      Integer,            //  +123
      Real,               //  +123.456e+10
      TagHeader,          //  <TagHeader>
      RawtextBlock,         // {< blabla >}

    } EType;

    VLX_Token(): mType(TOKEN_ERROR) {}

    std::string mString;
    EType mType;
  };
  //-----------------------------------------------------------------------------
  class VLX_Tokenizer: public BufferedStream<char, 128*1024>
  {
  public:
    VLX_Tokenizer(): mLineNumber(1), mRawtextBlock(false) {}

    VLCORE_EXPORT bool getToken(VLX_Token& token);

    VLCORE_EXPORT bool getRawtextBlock(VLX_Token& token);

    int lineNumber() const { return mLineNumber; }

  private:
    int mLineNumber;
    bool mRawtextBlock;
  };
  //-----------------------------------------------------------------------------
  // VLX_Parser
  //-----------------------------------------------------------------------------
  class VLX_Parser
  {
  public:
    VLX_Parser()
    {
      mTokenizer = new VLX_Tokenizer;
    }

    bool getToken(VLX_Token& token) { return mTokenizer->getToken(token); }

    std::string exportToText()
    {
      if (mRoot)
      {
        std::map< std::string, int > uid_set;
        VLX_UIDCollectorVisitor uid_collector;
        uid_collector.setUIDSet(&uid_set);
        mRoot->acceptVisitor(&uid_collector);

        VLX_TextExportVisitor text_export_visitor;
        text_export_visitor.setUIDSet(&uid_set);
        mRoot->acceptVisitor(&text_export_visitor);

        return text_export_visitor.text();
      }
      else
        return "";
    }

    bool link()
    {
      if (mRoot)
      {
        VLX_Linker linker;
        linker.add(mRoot.get());
        return linker.link();
      }
      else
        return false;
    }

    bool parseHeader(int& version, std::string& encoding)
    {
      version = 0;
      encoding = "";

      // VLX_TEXT
      if (!getToken(mToken) || mToken.mType != VLX_Token::Identifier || mToken.mString != "VLX_TEXT")
      {
        Log::error("'VLX_TEXT' header not found!\n");
        return false;
      }

      // version
      if (!getToken(mToken) || mToken.mType != VLX_Token::Identifier || mToken.mString != "version")
        return false;

      if (!getToken(mToken) || mToken.mType != VLX_Token::Equals)
        return false;

      if (!getToken(mToken) || mToken.mType != VLX_Token::Integer || mToken.mString != "100")
        return false;
      else
        version = atoi( mToken.mString.c_str() );

      // encoding
      if (!getToken(mToken) || mToken.mType != VLX_Token::Identifier || mToken.mString != "encoding")
        return false;

      if (!getToken(mToken) || mToken.mType != VLX_Token::Equals)
        return false;

      if (!getToken(mToken) || mToken.mType != VLX_Token::Identifier || mToken.mString != "ascii")
        return false;
      else
        encoding = mToken.mString;

      return true;
    }

    bool parse()
    {
      class CloseFileClass
      {
      public:
        CloseFileClass(VirtualFile* f): mFile(f) {}
        ~CloseFileClass()
        {
          if (mFile)
            mFile->close();
        }
      private:
        ref<VirtualFile> mFile;
      } CloseFile(tokenizer()->inputFile());

      int version = 0;
      std::string encoding;

      if (!parseHeader(version, encoding))
      {
        Log::error( Say("Line %n : error parsing header at '%s'.\n") << tokenizer()->lineNumber() << mToken.mString );
        return false;
      }

      if (version != 100)
      {
        Log::error("VLX version not supported.\n");
        return false;
      }

      if (encoding != "ascii")
      {
        Log::error("Encoding not supported.\n");
        return false;
      }

      mRoot = NULL;
      if(getToken(mToken))
      {
        if(mToken.mType == VLX_Token::TagHeader)
        {
          mLastTag = mToken.mString;
          
          if(getToken(mToken) && mToken.mType == VLX_Token::LeftCurlyBracket)
          {
            mRoot = new VLX_Structure;
            mRoot->setLineNumber( tokenizer()->lineNumber() );
            if (parseStructure(mRoot.get()))
            {
              return true;
            }
            else
            {
              mRoot = NULL;
              if (mToken.mString.length())
                Log::error( Say("Line %n : parse error at '%s'.\n") << mTokenizer->lineNumber() << mToken.mString.c_str() );
              else
                Log::error( Say("Line %n : parse error.\n") << mTokenizer->lineNumber() );
              return false;
            }
          }
          else
          {
            Log::error("No root object found!\n");
            return false;
          }
        }
        else
        {
          Log::error("No root object found!\n");
          return false;
        }
      }
      else
        return false;
    }

    bool parseStructure(VLX_Structure* object)
    {
      // consume last tag if there was one
      if (!mLastTag.empty())
      {
        object->setTag(mLastTag.c_str());
        mLastTag.clear();
      }

      while(getToken(mToken))
      {
        if (mToken.mType == VLX_Token::RightCurlyBracket)
        {
          return true;
        }
        else
        if (mToken.mType == VLX_Token::Identifier)
        {
          // ID field requires a proper #identifier
          if (mToken.mString.length() == 2)
          {
            if (mToken.mString == "ID")
            {
              // Check if ID has already been set
              if (!object->uid().empty() && object->uid() != "#NULL")
              {
                Log::error("ID already set.\n");
                return false;
              }

              // Equals
              if (!getToken(mToken) || mToken.mType != VLX_Token::Equals)
                return false;

              // #identifier
              if (getToken(mToken) && mToken.mType == VLX_Token::UID)
              {
                object->setUID(mToken.mString.c_str());

                // UID to Structure Map, #NULL is not mapped to anything
                if( object->uid() != "#NULL")
                {
                  if (mLinkMap.find(object->uid()) == mLinkMap.end())
                    mLinkMap[ object->uid() ] = object;
                  else
                  {
                    Log::error( Say("Duplicate UID = '%s'.\n") << object->uid() );
                    return false;
                  }
                }
                continue;
              }
              else
                return false;
            }
            else
            // ID is a reserved keyword: all the other case combinations are illegal
            if (mToken.mString == "Id" || mToken.mString == "iD" || mToken.mString == "id")
              return false;
          }

          // non-ID key-values
          object->value().push_back( VLX_Structure::Value() );
          VLX_Structure::Value& name_value = object->value().back();

          // Key
          name_value.setKey( mToken.mString.c_str() );

          // Equals
          if (!getToken(mToken) || mToken.mType != VLX_Token::Equals)
            return false;

          // Member value
          if (getToken(mToken))
          {
            name_value.value().setLineNumber( tokenizer()->lineNumber() );

            // A new <Tag>
            if (mToken.mType == VLX_Token::TagHeader)
            {
              if (mLastTag.empty())
              {
                mLastTag = mToken.mString;
                if (!getToken(mToken))
                  return false;
              }
              else
                return false;
            }

            // A new { Structure }
            if (mToken.mType == VLX_Token::LeftCurlyBracket)
            {
              ref<VLX_Structure> object = new VLX_Structure;
              object->setLineNumber( tokenizer()->lineNumber() );
              name_value.value().setStructure(object.get());
              if (!parseStructure( object.get() ) )
                return false;
            }
            else
            // An [ list ]
            if (mToken.mType == VLX_Token::LeftSquareBracket)
            {
              ref<VLX_List> list = new VLX_List;
              list->setLineNumber( tokenizer()->lineNumber() );
              name_value.value().setList(list.get());
              if ( !parseList( list.get() ) )
                return false;
            }
            else
            // An ( array )
            if (mToken.mType == VLX_Token::LeftRoundBracket)
            {
              ref<VLX_Array> arr;
              if ( parseArray( arr ) )
                name_value.value().setArray(arr.get());
              else
                return false;
            }
            else
            // A {< rawtext block >}
            if (mToken.mType == VLX_Token::LeftFancyBracket)
            {
              if(!getToken(mToken) || mToken.mType != VLX_Token::RawtextBlock)
                return false;
              name_value.value().setRawtextBlock( new VLX_RawtextBlock(mLastTag.c_str()) );
              name_value.value().getRawtextBlock()->setValue( mToken.mString.c_str() );
              // consume the tag
              mLastTag.clear();
              if(!getToken(mToken) || mToken.mType != VLX_Token::RightFancyBracket)
                return false;
            }
            else
            // A "string"
            if (mToken.mType == VLX_Token::String)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setString(mToken.mString.c_str());
            }
            else
            // An Identifier
            if (mToken.mType == VLX_Token::Identifier)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setIdentifier(mToken.mString.c_str());
            }
            else
            // An #id
            if (mToken.mType == VLX_Token::UID)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setUID(mToken.mString.c_str());
            }
            else
            // A boolean true/false
            if (mToken.mType == VLX_Token::Boolean)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setBool(mToken.mString == "true");
            }
            else
            // An integer
            if (mToken.mType == VLX_Token::Integer)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setInteger( atoll(mToken.mString.c_str()) );
            }
            else
            // A float
            if (mToken.mType == VLX_Token::Real)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setReal( atof(mToken.mString.c_str()) );
            }
            else
              return false;
          }
        }
        else
          return false;
      }
      return false;
    }

    bool parseList(VLX_List* list)
    {
      // consume last tag if there was one
      if (!mLastTag.empty())
      {
        list->setTag(mLastTag.c_str());
        mLastTag.clear();
      }

      while(getToken(mToken))
      {
        if (mToken.mType == VLX_Token::RightSquareBracket)
          return true;
        else
        {
          VLX_Value value;
          value.setLineNumber( tokenizer()->lineNumber() );
          switch( mToken.mType )
          {
            // <tag>
            case VLX_Token::TagHeader:
              {
                if (mLastTag.empty())
                  mLastTag = mToken.mString;
                else
                  return false;
                break;
              }

            // object
            case VLX_Token::LeftCurlyBracket:
              {
                ref<VLX_Structure> object = new VLX_Structure;
                object->setLineNumber( tokenizer()->lineNumber() );
                if ( parseStructure( object.get() ) )
                {
                  value.setStructure(object.get());
                  list->value().push_back( value );
                }
                else
                  return false;
                break;
              }

            // list
            case VLX_Token::LeftSquareBracket:
              {
                ref<VLX_List> sub_list = new VLX_List;
                sub_list->setLineNumber( tokenizer()->lineNumber() );
                if ( parseList( sub_list.get() ) )
                {
                  value.setList( sub_list.get() );
                  list->value().push_back( value );
                }
                else
                  return false;
                break;
              }

            // array
            case VLX_Token::LeftRoundBracket:
              {
                ref<VLX_Array> arr;
                if (parseArray(arr))
                {
                  value.setArray(arr.get());
                  list->value().push_back(value);
                }
                else
                  return false;
                break;
              }

            // string
            case VLX_Token::String:
              if (!mLastTag.empty())
                return false;
              value.setString( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // identifier
            case VLX_Token::Identifier:
              if (!mLastTag.empty())
                return false;
              value.setIdentifier( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // A {< rawtext block >}
            case VLX_Token::LeftFancyBracket:
            {
              if(!getToken(mToken) || mToken.mType != VLX_Token::RawtextBlock)
                return false;
              
              value.setRawtextBlock( new VLX_RawtextBlock(mLastTag.c_str()) );
              value.getRawtextBlock()->setValue( mToken.mString.c_str() );
              list->value().push_back( value );
              // consume the tag
              mLastTag.clear();

              if(!getToken(mToken) || mToken.mType != VLX_Token::RightFancyBracket)
                return false;
              break;
            }

            // UID
            case VLX_Token::UID:
              if (!mLastTag.empty())
                return false;
              value.setUID( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // boolean
            case VLX_Token::Boolean:
              if (!mLastTag.empty())
                return false;
              value.setBool( mToken.mString == "true" ); list->value().push_back( value );
              break;

            // int
            case VLX_Token::Integer:
              if (!mLastTag.empty())
                return false;
              value.setInteger( atoll(mToken.mString.c_str()) ); list->value().push_back( value );
              break;

            // float
            case VLX_Token::Real:
              if (!mLastTag.empty())
                return false;
              value.setReal( atof(mToken.mString.c_str()) ); list->value().push_back( value );
              break;

          default:
            return false;
          }
        }
      }
      return false;
    }

    bool parseArray(ref<VLX_Array>& arr)
    {
      // consume last tag if there was one
      struct struct_consume_tag
      {
        struct_consume_tag(ref<VLX_Array>* p1, std::string* p2): p_arr(p1), p_tag(p2) {}

       ~struct_consume_tag()
        {
          if ((*p_arr).get() && !p_tag->empty())
          {
            (*p_arr)->setTag(p_tag->c_str());
            p_tag->clear();
          }
        }

        ref<VLX_Array>* p_arr;
        std::string* p_tag;
      } consume_tag(&arr, &mLastTag);

      if(getToken(mToken))
      {
        // (1) from the fist token we decide what kind of array it is going to be
        // (2) empty arrays default to empty VLX_ArrayInteger

        if (mToken.mType == VLX_Token::RightRoundBracket)
        {
          arr = new VLX_ArrayInteger;
          return true;
        }
        /*
        else
        if (mToken.mType == VLX_Token::String)
        {
          ref<VLX_ArrayString> arr_string;
          arr = arr_string = new VLX_ArrayString;
          do 
            arr_string->mValue.push_back(mToken.mString);
          while(getToken(mToken) && mToken.mType == VLX_Token::String);
          return mToken.mType == VLX_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == VLX_Token::Identifier)
        {
          ref<VLX_ArrayIdentifier> arr_identifier;
          arr = arr_identifier = new VLX_ArrayIdentifier;
          do 
            arr_identifier->mValue.push_back(mToken.mString);
          while(getToken(mToken) && mToken.mType == VLX_Token::Identifier);
          return mToken.mType == VLX_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == VLX_Token::UID)
        {
          ref<VLX_ArrayUID> arr_uid;
          arr = arr_uid = new VLX_ArrayUID;
          do
            arr_uid->mValue.push_back(mToken.mString.c_str());
          while(getToken(mToken) && mToken.mType == VLX_Token::UID);
          return mToken.mType == VLX_Token::RightRoundBracket;
        }
        */
        else
        if (mToken.mType == VLX_Token::Integer)
        {
          ref<VLX_ArrayInteger> arr_integer;
          arr = arr_integer = new VLX_ArrayInteger;
          do
          {
            switch(mToken.mType)
            {
            case VLX_Token::Integer: arr_integer->value().push_back( atoll( mToken.mString.c_str() ) ); break;
            case VLX_Token::RightRoundBracket: return true;
            default:
              return false;
            }
          }
          while(getToken(mToken));
          return false;
        }
        else
        if (mToken.mType == VLX_Token::Real)
        {
          ref<VLX_ArrayReal> arr_floating;
          arr = arr_floating = new VLX_ArrayReal;
          do
          {
            switch(mToken.mType)
            {
            case VLX_Token::Integer:
            case VLX_Token::Real: arr_floating->mValue.push_back( atof( mToken.mString.c_str() ) ); break;
            case VLX_Token::RightRoundBracket: return true;
            default:
              return false;
            }
          }
          while(getToken(mToken));
          return false;
        }
        else
          return false;
      }

      return false;
    }

    void listTokens()
    {
      while(getToken(mToken) && mToken.mType != VLX_Token::TOKEN_EOF)
      {
        switch(mToken.mType)
        {
          case VLX_Token::LeftRoundBracket:   printf("LeftSquareBracket (\n"); break;
          case VLX_Token::RightRoundBracket:  printf("RightSquareBracket )\n"); break;
          case VLX_Token::LeftSquareBracket:  printf("LeftSquareBracket [\n"); break;
          case VLX_Token::RightSquareBracket: printf("RightSquareBracket ]\n"); break;
          case VLX_Token::LeftCurlyBracket:   printf("LeftCurlyBracket {\n"); break;
          case VLX_Token::RightCurlyBracket:  printf("RightCurlyBracket } \n"); break;
          case VLX_Token::LeftFancyBracket:   printf("LeftFancyBracket >}\n"); break;
          case VLX_Token::RightFancyBracket:  printf("RightFancyBracket {< \n"); break;
          case VLX_Token::Equals:             printf("Equals =\n"); break;
          case VLX_Token::String:             printf("String = %s\n", mToken.mString.c_str()); break;
          case VLX_Token::UID:                printf("UID = %s\n", mToken.mString.c_str()); break;
          case VLX_Token::Identifier:         printf("Identifier = %s\n", mToken.mString.c_str()); break;
          case VLX_Token::RawtextBlock:       printf("RawtextBlock = %s\n", mToken.mString.c_str()); break;
          case VLX_Token::Real:               printf("Real = %s\n", mToken.mString.c_str()); break;
          case VLX_Token::Integer:            printf("Integer = %s\n", mToken.mString.c_str()); break;
          case VLX_Token::TagHeader:          printf("TagHeader = %s\n", mToken.mString.c_str()); break;
          default:
            break;
        }
      }
      if (mToken.mType != VLX_Token::TOKEN_EOF)
      {
        printf("Line %d: syntax error : '%s'.\n", mTokenizer->lineNumber(), mToken.mString.c_str());
      }
    }

    VLX_Tokenizer* tokenizer() { return mTokenizer.get(); }

    const VLX_Tokenizer* tokenizer() const { return mTokenizer.get(); }

    const std::map< std::string, ref<VLX_Structure> >& linkMap() const { return mLinkMap; }

    VLX_Structure* root() { return mRoot.get(); }

    const VLX_Structure* root() const { return mRoot.get(); }

  private:
    std::string mLastTag;
    ref<VLX_Structure> mRoot;
    std::map< std::string, ref<VLX_Structure> > mLinkMap;
    ref<VLX_Tokenizer> mTokenizer;
    VLX_Token mToken;
  };
  //-----------------------------------------------------------------------------
}

#endif
