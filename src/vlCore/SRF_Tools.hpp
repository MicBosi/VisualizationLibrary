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

#ifndef VRF_INCLUDE_ONCE
#define VRF_INCLUDE_ONCE

#include <vlCore\Object.hpp>
#include <vlCore\Say.hpp>
#include <vlCore\Log.hpp>
#include <vlCore\BufferedStream.hpp>
#include <vlCore\Vector4.hpp>
#include <map>
#include <set>

// mic fixme
#ifdef _MSC_VER
#define atoll _atoi64
#endif

namespace vl
{
  //-----------------------------------------------------------------------------
  class VRF_Structure;
  class VRF_List;
  class VRF_RawtextBlock;
  class VRF_Array;
  class VRF_ArrayInteger;
  class VRF_ArrayReal;
  /*
  class VRF_ArrayString;
  class VRF_ArrayIdentifier;
  class VRF_ArrayUID;
  */
  //-----------------------------------------------------------------------------
  class VRF_Visitor: public Object
  {
  public:
    virtual void visitStructure(VRF_Structure*) {}
    virtual void visitList(VRF_List*) {}
    virtual void visitRawtextBlock(VRF_RawtextBlock*) {}
    virtual void visitArray(VRF_ArrayInteger*) {}
    virtual void visitArray(VRF_ArrayReal*) {}
    /*
    virtual void visitArray(VRF_ArrayString*) {}
    virtual void visitArray(VRF_ArrayIdentifier*) {}
    virtual void visitArray(VRF_ArrayUID*) {}
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
  class VRF_TaggedValue: public Object
  {
    VL_INSTRUMENT_CLASS(vl::VRF_TaggedValue, Object)

  public:
    VRF_TaggedValue(const char* tag=NULL): mLineNumber(0) 
    {
      if (tag)
        mTag = tag;
    }

    virtual ~VRF_TaggedValue() {}

    int lineNumber() const { return mLineNumber; }

    void setLineNumber(int line) { mLineNumber = line; }

    virtual void acceptVisitor(VRF_Visitor*) = 0;
  
    void setTag(const char* tag) { mTag = tag; }

    const std::string& tag() const { return mTag; }

  private:
    std::string mTag;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // RawtextBlock
  //-----------------------------------------------------------------------------
  class VRF_RawtextBlock: public VRF_TaggedValue 
  { 
    VL_INSTRUMENT_CLASS(vl::VRF_RawtextBlock, VRF_TaggedValue)

  public:
    VRF_RawtextBlock(const char* tag=NULL, const char* value=NULL): VRF_TaggedValue(tag) 
    {
      if (value)
        mValue = value;
    }

    virtual void acceptVisitor(VRF_Visitor* v) { v->visitRawtextBlock(this); }

    std::string& value() { return mValue; }

    const std::string& value() const { return mValue; }

    void setValue(const char* value) { mValue = value; }

  private:
    std::string mValue;
  };
  //-----------------------------------------------------------------------------
  // Arrays
  //-----------------------------------------------------------------------------
  class VRF_Array: public VRF_TaggedValue 
  { 
    VL_INSTRUMENT_CLASS(vl::VRF_Array, VRF_TaggedValue)

  public:
    VRF_Array(const char* tag=NULL): VRF_TaggedValue(tag) {}

  };
  //-----------------------------------------------------------------------------
  class VRF_ArrayInteger: public VRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::VRF_ArrayInteger, VRF_Array)

  public:
    typedef long long scalar_type;

  public:
    VRF_ArrayInteger(const char* tag=NULL): VRF_Array(tag) { }
    
    virtual void acceptVisitor(VRF_Visitor* v) { v->visitArray(this); }

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
  class VRF_ArrayReal: public VRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::VRF_ArrayReal, VRF_Array)

  public:
    typedef double scalar_type;

  public:
    VRF_ArrayReal(const char* tag=NULL): VRF_Array(tag) { }
    
    virtual void acceptVisitor(VRF_Visitor* v) { v->visitArray(this); }

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
  class VRF_ArrayString: public VRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::VRF_ArrayString, VRF_Array)

  public:
    VRF_ArrayString(const char* tag=NULL): VRF_Array(tag) { }

    virtual void acceptVisitor(VRF_Visitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class VRF_ArrayIdentifier: public VRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::VRF_ArrayIdentifier, VRF_Array)

  public:
    VRF_ArrayIdentifier(const char* tag=NULL): VRF_Array(tag) { }

    virtual void acceptVisitor(VRF_Visitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class VRF_ArrayUID: public VRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::VRF_ArrayUID, VRF_Array)

  public:
    VRF_ArrayUID(const char* tag=NULL): VRF_Array(tag) { }

    virtual void acceptVisitor(VRF_Visitor* v) { v->visitArray(this); }

    class Value
    {
    public:
      Value(const char* uid): mUID(uid) {}

      void setUID(const char* uid) { mUID = uid; }

      const char* uid() const { return mUID.c_str(); }

      void setStructure(VRF_Structure* obj) { mObj = obj; }

      VRF_Structure* object() { return mObj.get(); }

      const VRF_Structure* object() const { return mObj.get(); }

    private:
      std::string mUID; // the UID string
      ref<VRF_Structure> mObj; // the linked object
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
  // VRF_Value
  //-----------------------------------------------------------------------------
  class VRF_Value
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
    VRF_Value()
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
    }

    VRF_Value(VRF_Structure* obj)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setStructure(obj);
    }

    VRF_Value(VRF_List* list)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setList(list);
    }

    VRF_Value(VRF_RawtextBlock* rawtext)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setRawtextBlock(rawtext);
    }

    VRF_Value(VRF_ArrayInteger* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayInteger(arr);
    }

    VRF_Value(VRF_ArrayReal* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayReal(arr);
    }

    /*
    VRF_Value(VRF_ArrayString* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayString(arr);
    }

    VRF_Value(VRF_ArrayIdentifier* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayIdentifier(arr);
    }

    VRF_Value(VRF_ArrayUID* arr)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;
      setArrayUID(arr);
    }
    */

    VRF_Value(long long i)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = i;
    }

    VRF_Value(double d)
    {
      mLineNumber = 0;
      mType = Real;
      mUnion.mReal  = d;
    }

    VRF_Value(const char* str, EType type)
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

    VRF_Value(bool boolean)
    {
      mLineNumber = 0;
      mType = Integer;
      mUnion.mInteger = 0;

      setBool(boolean);
    }

    VRF_Value(const VRF_Value& other)
    {
      mType = Integer;
      mUnion.mInteger = 0;
      mLineNumber = 0;

      *this = other;
    }

    ~VRF_Value() { release(); }

    VLCORE_EXPORT VRF_Value& operator=(const VRF_Value& other);

    EType type() const { return mType; }

    // object

    VLCORE_EXPORT VRF_Structure* setStructure(VRF_Structure*);

    VRF_Structure* getStructure() { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    const VRF_Structure* getStructure() const { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    // list

    VLCORE_EXPORT VRF_List* setList(VRF_List*);

    VRF_List* getList() { VL_CHECK(mType == List); return mUnion.mList; }

    const VRF_List* getList() const { VL_CHECK(mType == List); return mUnion.mList; }

    // rawtext block

    VLCORE_EXPORT VRF_RawtextBlock* setRawtextBlock(VRF_RawtextBlock*);

    VRF_RawtextBlock* getRawtextBlock() { VL_CHECK(mType == RawtextBlock); return mUnion.mRawtextBlock; }

    const VRF_RawtextBlock* getRawtextBlock() const { VL_CHECK(mType == RawtextBlock); return mUnion.mRawtextBlock; }

    // array

    VLCORE_EXPORT VRF_Array*           setArray(VRF_Array*);
    VLCORE_EXPORT VRF_ArrayInteger*    setArrayInteger(VRF_ArrayInteger*);
    VLCORE_EXPORT VRF_ArrayReal*       setArrayReal(VRF_ArrayReal*);
    /*
    VLCORE_EXPORT VRF_ArrayString*     setArrayString(VRF_ArrayString*);
    VLCORE_EXPORT VRF_ArrayIdentifier* setArrayIdentifier(VRF_ArrayIdentifier*);
    VLCORE_EXPORT VRF_ArrayUID*        setArrayUID(VRF_ArrayUID*);
    */

    /*
    VRF_ArrayString* getArrayString() { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<VRF_ArrayString>(); }
    const VRF_ArrayString* getArrayString() const { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<VRF_ArrayString>(); }

    VRF_ArrayIdentifier* getArrayIdentifier() { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<VRF_ArrayIdentifier>(); }
    const VRF_ArrayIdentifier* getArrayIdentifier() const { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<VRF_ArrayIdentifier>(); }

    VRF_ArrayUID* getArrayUID() { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<VRF_ArrayUID>(); }
    const VRF_ArrayUID* getArrayUID() const { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<VRF_ArrayUID>(); }
    */

    VRF_ArrayInteger* getArrayInteger() { VL_CHECK(mType == ArrayInteger); return mUnion.mArray->as<VRF_ArrayInteger>(); }
    const VRF_ArrayInteger* getArrayInteger() const { VL_CHECK(mType == ArrayInteger); return mUnion.mArray->as<VRF_ArrayInteger>(); }

    VRF_ArrayReal* getArrayReal() { VL_CHECK(mType == ArrayReal); return mUnion.mArray->as<VRF_ArrayReal>(); }
    const VRF_ArrayReal* getArrayReal() const { VL_CHECK(mType == ArrayReal); return mUnion.mArray->as<VRF_ArrayReal>(); }

    // string

    const char* setString(const char* str)
    {
      release();
      mType = String;
      return mUnion.mString = _strdup(str);
    }

    const char* getString() const { VL_CHECK(mType == String); return mUnion.mString; }

    // identifier

    const char* setIdentifier(const char* str)
    {
      release();
      mType = Identifier;
      return mUnion.mString = _strdup(str);
    }

    const char* getIdentifier() const { VL_CHECK(mType == Identifier); return mUnion.mString; }

    // uid

    const char* setUID(const char* str)
    {
      release();
      mType = UID;
      return mUnion.mString = _strdup(str);
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
      VRF_Structure* mStructure;
      VRF_List* mList;
      VRF_Array* mArray;
      VRF_RawtextBlock* mRawtextBlock;
    } mUnion;

  private:
    EType mType;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // VRF_Structure
  //-----------------------------------------------------------------------------
  class VRF_Structure: public VRF_TaggedValue
  {
    VL_INSTRUMENT_CLASS(vl::VRF_Structure, VRF_TaggedValue)

  public:
    VRF_Structure()
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
    }

    VRF_Structure(const char* tag)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
      setTag(tag);
    }

    VRF_Structure(const char* tag, const std::string& uid)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID(uid.c_str());
      setTag(tag);
    }

    virtual void acceptVisitor(VRF_Visitor* v) { v->visitStructure(this); }

    class Value
    {
      friend class VRF_Structure;

    public:
      Value() {}
      Value(const char* key, VRF_Value value): mKey(key), mValue(value) {}

      std::string& key() { return mKey; }
      const std::string& key() const { return mKey; }
      void setKey(const char* key) { mKey = key; }

      VRF_Value& value() { return mValue; }
      const VRF_Value& value() const { return mValue; }
      void setValue(const VRF_Value& value) { mValue = value; }

    private:
      std::string mKey;
      VRF_Value mValue;
    };

    void setUID(const char* uid) { mUID = uid; }

    const std::string& uid() const { return mUID; }

    std::vector<Value>& value() { return mKeyValue; }

    const std::vector<Value>& value() const { return mKeyValue; }

    // mic fixme: we can speed this guys up with multimaps
    VRF_Value* getValue(const char* key)
    {
      for(size_t i=0; i<mKeyValue.size(); ++i)
        if (mKeyValue[i].key() == key)
          return &mKeyValue[i].value();
      return NULL;
    }

    const VRF_Value* getValue(const char* key) const
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
  // VRF_List
  //-----------------------------------------------------------------------------
  class VRF_List: public VRF_TaggedValue
  {
    VL_INSTRUMENT_CLASS(vl::VRF_List, VRF_TaggedValue)

  public:
    VRF_List(const char* tag=NULL): VRF_TaggedValue(tag)
    {
      // mic fixme: reenable
      // mValue.reserve(16);
    }

    virtual void acceptVisitor(VRF_Visitor* v) { v->visitList(this); }

    std::vector< VRF_Value >& value() { return mValue; }

    const std::vector< VRF_Value >& value() const { return mValue; }

  private:
    std::vector< VRF_Value > mValue;
  };
  //-----------------------------------------------------------------------------
  // VRF_TextExportVisitor
  //-----------------------------------------------------------------------------
  class VRF_TextExportVisitor: public VRF_Visitor
  {
  public:
    VRF_TextExportVisitor()
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
        return mUIDSet->find(uid) != mUIDSet->end();
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

    virtual void visitStructure(VRF_Structure* obj)
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

        case VRF_Value::Structure:
          mAssign = true;
          obj->value()[i].value().getStructure()->acceptVisitor(this);
          break;

        case VRF_Value::List:
          mAssign = true;
          obj->value()[i].value().getList()->acceptVisitor(this);
          break;

        /*
        case VRF_Value::ArrayString:
          mAssign = true;
          obj->value()[i].value().getArrayString()->acceptVisitor(this);
          break;

        case VRF_Value::ArrayIdentifier:
          mAssign = true;
          obj->value()[i].value().getArrayIdentifier()->acceptVisitor(this);
          break;

        case VRF_Value::ArrayUID:
          mAssign = true;
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
          break;
        */

        case VRF_Value::ArrayInteger:
          mAssign = true;
          obj->value()[i].value().getArrayInteger()->acceptVisitor(this);
          break;

        case VRF_Value::ArrayReal:
          mAssign = true;
          obj->value()[i].value().getArrayReal()->acceptVisitor(this);
          break;

        case VRF_Value::RawtextBlock:
        {
          VRF_RawtextBlock* fblock = obj->value()[i].value().getRawtextBlock();
          if (!fblock->tag().empty())
            format("%s", fblock->tag().c_str());
          output("\n"); indent(); format("{<\n%s>}\n", rawtextEncode(fblock->value().c_str()).c_str());
        }
        break;

        case VRF_Value::String:
          format("\"%s\"\n", stringEncode(obj->value()[i].value().getString()).c_str());
          break;

        case VRF_Value::Identifier:
          format("%s\n", obj->value()[i].value().getIdentifier()); VL_CHECK( strlen(obj->value()[i].value().getIdentifier()) )
          break;

        case VRF_Value::UID:
          format("%s\n", obj->value()[i].value().getUID()); VL_CHECK( strlen(obj->value()[i].value().getUID()) )
          break;

        case VRF_Value::Bool:
          format("%s\n", obj->value()[i].value().getBool() ? "true" : "false");
          break;

        case VRF_Value::Integer:
          format("%lld\n", obj->value()[i].value().getInteger());
          break;

        case VRF_Value::Real:
          format("%Lf\n", obj->value()[i].value().getReal());
          break;
        }
      }
      mIndent--;
      indent(); output("}\n");
    }

    virtual void visitList(VRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VRF_TextExportVisitor: cycle detected on VRF_List.\n");
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

        case VRF_Value::Structure:
          list->value()[i].getStructure()->acceptVisitor(this);
          break;

        case VRF_Value::List:
          list->value()[i].getList()->acceptVisitor(this);
          break;

        /*
        case VRF_Value::ArrayString:
          list->value()[i].getArrayString()->acceptVisitor(this);
          break;

        case VRF_Value::ArrayIdentifier:
          list->value()[i].getArrayIdentifier()->acceptVisitor(this);
          break;

        case VRF_Value::ArrayUID:
          list->value()[i].getArrayUID()->acceptVisitor(this);
          break;
        */

        case VRF_Value::ArrayInteger:
          list->value()[i].getArrayInteger()->acceptVisitor(this);
          break;

        case VRF_Value::ArrayReal:
          list->value()[i].getArrayReal()->acceptVisitor(this);
          break;

        case VRF_Value::String:
          indent(); format("\"%s\"\n", stringEncode(list->value()[i].getString()).c_str());
          break;

        case VRF_Value::Identifier:
          indent(); format("%s\n", list->value()[i].getIdentifier()); VL_CHECK( strlen(list->value()[i].getIdentifier()) )
          break;

        case VRF_Value::UID:
          indent(); format("%s\n", list->value()[i].getUID()); VL_CHECK( strlen(list->value()[i].getUID()) )
          break;

        case VRF_Value::RawtextBlock:
        {
          VRF_RawtextBlock* fblock = list->value()[i].getRawtextBlock();
          if (!fblock->tag().empty())
          {
            indent(); format("%s\n", fblock->tag().c_str());
          }
          indent(); format("{<\n%s>}\n", rawtextEncode(fblock->value().c_str()).c_str());
        }
        break;

        case VRF_Value::Bool:
          indent(); format("%s\n", list->value()[i].getBool() ? "true" : "false");
          break;

        case VRF_Value::Integer:
          indent(); format("%lld\n", list->value()[i].getInteger());
          break;

        case VRF_Value::Real:
          indent(); format("%Lf\n", list->value()[i].getReal());
          break;
        }
      }
      mIndent--;
      indent(); output("]\n");
    }

    virtual void visitArray(VRF_ArrayInteger* arr)
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

    virtual void visitArray(VRF_ArrayReal* arr)
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
    virtual void visitArray(VRF_ArrayString* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        output(std::string("\"") + stringEncode(arr->value()[i].c_str()) + "\" ");
      output(")\n");
    }

    virtual void visitArray(VRF_ArrayIdentifier* arr)
    {
      indent(); if (!arr->tag().empty()) format("%s ", arr->tag().c_str()); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        format("%s ", arr->value()[i].c_str());
      output(")\n");
    }

    virtual void visitArray(VRF_ArrayUID* arr)
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

    const std::string& srfText() const { return mText; }

    std::string& srfText() { return mText; }

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
      mText = "Simple_Runtime_Format_Text Version=100 Encoding=ascii\n\n";
    }

    void setUIDSet(std::set< std::string >* uids) { mUIDSet = uids; }

    std::set< std::string >* uidSet() { return mUIDSet; }

    const std::set< std::string >* uidSet() const { return mUIDSet; }

  private:
    int mIndent;
    bool mAssign;
    std::string mText;
    std::set< std::string >* mUIDSet;
    std::vector<char> mFormatBuffer;
  };
  //-----------------------------------------------------------------------------
  // VRF_LinkMapperVisitor: 
  // Compiles the link-map which associates an VRF_Structure to it's UID.
  // Can be called multiple times
  //-----------------------------------------------------------------------------
  class VRF_LinkMapperVisitor: public VRF_Visitor
  {
  public:
    typedef enum 
    {
      NoError,
      DuplicateUID
    } EError;

  public:
    VRF_LinkMapperVisitor(std::map< std::string, ref<VRF_Structure> >* map)
    {
      mLinkMap = map;
      mError = NoError;
    }

    void setLinkMap(std::map< std::string, ref<VRF_Structure> >* map)
    {
      mLinkMap = map;
    }

    void declareUID(VRF_Structure* obj)
    {
      if (obj->uid() != "#NULL")
      {
        const std::map< std::string, ref<VRF_Structure> >::const_iterator it = mLinkMap->find(obj->uid());
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

    virtual void visitStructure(VRF_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      declareUID(obj);

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == VRF_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == VRF_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
      }
    }

    virtual void visitList(VRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VRF_LinkMapperVisitor: cycle detected on VRF_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VRF_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        else
        if (list->value()[i].type() == VRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
      }
    }

    /*
    virtual void visitArray(VRF_ArrayString*)  {}

    virtual void visitArray(VRF_ArrayIdentifier*) {}

    virtual void visitArray(VRF_ArrayUID*) {}
    */

    virtual void visitArray(VRF_ArrayInteger*)  {}

    virtual void visitArray(VRF_ArrayReal*)  {}

    EError error() const { return mError; }

    void setError(EError err) { mError = err; }

  private:
    std::map< std::string, ref<VRF_Structure> >* mLinkMap;
    EError mError;
  };
  //-----------------------------------------------------------------------------
  // VRF_LinkVisitor:
  // Translates UIDs into VRF_Structures
  //-----------------------------------------------------------------------------
  class VRF_LinkVisitor: public VRF_Visitor
  {
  public:
    typedef enum 
    {
      NoError,
      UnresolvedUID
    } EError;

  public:
    VRF_LinkVisitor(const std::map< std::string, ref<VRF_Structure> >* map)
    {
      mLinkMap = map;
      mError = NoError;
    }

    void setLinkMap(const std::map< std::string, ref<VRF_Structure> >* map)
    {
      mLinkMap = map;
    }

    VRF_Structure* link(const std::string& uid)
    {
      VL_CHECK(mLinkMap)
      VL_CHECK(!uid.empty())
      std::map< std::string, ref<VRF_Structure> >::const_iterator it = mLinkMap->find(uid);
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

    virtual void visitStructure(VRF_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == VRF_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == VRF_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
        else
        /*
        if (obj->value()[i].value().type() == VRF_Value::ArrayUID)
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
        else
        */
        if (obj->value()[i].value().type() == VRF_Value::UID)
        {
          // transform UID -> Structure
          VRF_Structure* lnk_obj = link( obj->value()[i].value().getUID() );
          obj->value()[i].value().setStructure( lnk_obj );
        }
      }
    }

    virtual void visitList(VRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VRF_LinkVisitor: cycle detected on VRF_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VRF_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == VRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        /*
        if (list->value()[i].type() == VRF_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        */
        if (list->value()[i].type() == VRF_Value::UID)
        {
          // transform UID -> Structure
          VRF_Structure* lnk_obj = link( list->value()[i].getUID() );
          list->value()[i].setStructure( lnk_obj );
        }
      }
    }

    /*
    virtual void visitArray(VRF_ArrayString*)  {}

    virtual void visitArray(VRF_ArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        arr->value()[i].setStructure ( link(arr->value()[i].uid()) );
    }

    virtual void visitArray(VRF_ArrayIdentifier*) {}
    */

    virtual void visitArray(VRF_ArrayInteger*)  {}

    virtual void visitArray(VRF_ArrayReal*)  {}

    EError error() const { return mError; }

    void setError(EError err) { mError = err; }

  private:
    const std::map< std::string, ref<VRF_Structure> >* mLinkMap;
    EError mError;
  };
  //-----------------------------------------------------------------------------
  // VRF_UIDCollectorVisitor:
  // Sets to #NULL the UID of those objects that are not referenced by anybody.
  // Useful before exporting.
  //-----------------------------------------------------------------------------
  class VRF_UIDCollectorVisitor: public VRF_Visitor
  {
  public:
    VRF_UIDCollectorVisitor(): mUIDSet(NULL) {}

    virtual void visitStructure(VRF_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == VRF_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == VRF_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
        else
        /*
        if (obj->value()[i].value().type() == VRF_Value::ArrayUID)
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
        else
        */
        if (obj->value()[i].value().type() == VRF_Value::UID)
          mUIDSet->insert(obj->value()[i].value().getUID());
      }
    }

    virtual void visitList(VRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("VRF_UIDCollectorVisitor: cycle detected on VRF_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == VRF_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == VRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        /*
        if (list->value()[i].type() == VRF_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        */
        if (list->value()[i].type() == VRF_Value::UID)
          mUIDSet->insert(list->value()[i].getUID());
      }
    }

    /*
    virtual void visitArray(VRF_ArrayString*)  {}

    virtual void visitArray(VRF_ArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mUIDSet->insert(arr->value()[i].uid());
    }

    virtual void visitArray(VRF_ArrayIdentifier*) {}
    */

    virtual void visitArray(VRF_ArrayInteger*)  {}

    virtual void visitArray(VRF_ArrayReal*)  {}

    void setUIDSet(std::set< std::string >* uids) { mUIDSet = uids; }

    std::set< std::string >* uidSet() { return mUIDSet; }

    const std::set< std::string >* uidSet() const { return mUIDSet; }

  private:
    std::set< std::string >* mUIDSet;
  };
  //-----------------------------------------------------------------------------
  // Links several hierachies also resolving UIDs across them.
  //-----------------------------------------------------------------------------
  class VRF_Linker
  {
  public:
    void add(VRF_TaggedValue* module)
    {
      mModules.push_back(module);
    }

    bool link()
    {
      std::map< std::string, ref<VRF_Structure> > link_map;

      // map all the UIDs to the appropriate VRF_Structures
      VRF_LinkMapperVisitor link_mapper(&link_map);
      for(size_t i=0; i<mModules.size(); ++i)
        mModules[i]->acceptVisitor(&link_mapper);

      if (link_mapper.error())
        return false;

      // link all the UIDs to the associated VRF_Structure
      VRF_LinkVisitor linker(&link_map);
      for(size_t i=0; i<mModules.size(); ++i)
        mModules[i]->acceptVisitor(&linker);

      if (linker.error())
        return false;

      return true;
    }

    std::vector< ref<VRF_TaggedValue> >& modules() { return mModules; }

    const std::vector< ref<VRF_TaggedValue> >& modules() const { return mModules; }

  public:
    std::vector< ref<VRF_TaggedValue> > mModules;
  };
  //-----------------------------------------------------------------------------
  class VRF_Token
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

    VRF_Token(): mType(TOKEN_ERROR) {}

    std::string mString;
    EType mType;
  };
  //-----------------------------------------------------------------------------
  class VRF_Tokenizer: public BufferedStream<char, 128*1024>
  {
  public:
    VRF_Tokenizer(): mLineNumber(1), mRawtextBlock(false) {}

    VLCORE_EXPORT bool getToken(VRF_Token& token);

    VLCORE_EXPORT bool getRawtextBlock(VRF_Token& token);

    int lineNumber() const { return mLineNumber; }

  private:
    int mLineNumber;
    bool mRawtextBlock;
  };
  //-----------------------------------------------------------------------------
  // VRF_Parser
  //-----------------------------------------------------------------------------
  class VRF_Parser
  {
  public:
    VRF_Parser()
    {
      mTokenizer = new VRF_Tokenizer;
    }

    bool getToken(VRF_Token& token) { return mTokenizer->getToken(token); }

    std::string exportToText()
    {
      if (mRoot)
      {
        std::set< std::string > uid_set;
        VRF_UIDCollectorVisitor uid_collector;
        uid_collector.setUIDSet(&uid_set);
        mRoot->acceptVisitor(&uid_collector);

        VRF_TextExportVisitor text_export_visitor;
        text_export_visitor.setUIDSet(&uid_set);
        mRoot->acceptVisitor(&text_export_visitor);

        return text_export_visitor.srfText();
      }
      else
        return "";
    }

    bool link()
    {
      if (mRoot)
      {
        VRF_Linker linker;
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

      // Simple_Runtime_Format_Text
      if (!getToken(mToken) || mToken.mType != VRF_Token::Identifier || mToken.mString != "Simple_Runtime_Format_Text")
      {
        Log::error("'Simple_Runtime_Format_Text' header not found!\n");
        return false;
      }

      // Version
      if (!getToken(mToken) || mToken.mType != VRF_Token::Identifier || mToken.mString != "Version")
        return false;

      if (!getToken(mToken) || mToken.mType != VRF_Token::Equals)
        return false;

      if (!getToken(mToken) || mToken.mType != VRF_Token::Integer || mToken.mString != "100")
        return false;
      else
        version = atoi( mToken.mString.c_str() );

      // Encoding
      if (!getToken(mToken) || mToken.mType != VRF_Token::Identifier || mToken.mString != "Encoding")
        return false;

      if (!getToken(mToken) || mToken.mType != VRF_Token::Equals)
        return false;

      if (!getToken(mToken) || mToken.mType != VRF_Token::Identifier || mToken.mString != "ascii")
        return false;
      else
        encoding = mToken.mString;

      return true;
    }

    bool parse()
    {
      int version = 0;
      std::string encoding;

      if (!parseHeader(version, encoding))
      {
        Log::error( Say("Line %n : error parsing header at '%s'.\n") << tokenizer()->lineNumber() << mToken.mString );
        return false;
      }

      if (version != 100)
      {
        Log::error("SRF version not supported.\n");
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
        if(mToken.mType == VRF_Token::TagHeader)
        {
          mLastTag = mToken.mString;
          
          if(getToken(mToken) && mToken.mType == VRF_Token::LeftCurlyBracket)
          {
            mRoot = new VRF_Structure;
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

    bool parseStructure(VRF_Structure* object)
    {
      // consume last tag if there was one
      if (!mLastTag.empty())
      {
        object->setTag(mLastTag.c_str());
        mLastTag.clear();
      }

      while(getToken(mToken))
      {
        if (mToken.mType == VRF_Token::RightCurlyBracket)
        {
          return true;
        }
        else
        if (mToken.mType == VRF_Token::Identifier)
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
              if (!getToken(mToken) || mToken.mType != VRF_Token::Equals)
                return false;

              // #identifier
              if (getToken(mToken) && mToken.mType == VRF_Token::UID)
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
          object->value().push_back( VRF_Structure::Value() );
          VRF_Structure::Value& name_value = object->value().back();

          // Key
          name_value.setKey( mToken.mString.c_str() );

          // Equals
          if (!getToken(mToken) || mToken.mType != VRF_Token::Equals)
            return false;

          // Member value
          if (getToken(mToken))
          {
            name_value.value().setLineNumber( tokenizer()->lineNumber() );

            // A new <Tag>
            if (mToken.mType == VRF_Token::TagHeader)
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
            if (mToken.mType == VRF_Token::LeftCurlyBracket)
            {
              ref<VRF_Structure> object = new VRF_Structure;
              object->setLineNumber( tokenizer()->lineNumber() );
              name_value.value().setStructure(object.get());
              if (!parseStructure( object.get() ) )
                return false;
            }
            else
            // An [ list ]
            if (mToken.mType == VRF_Token::LeftSquareBracket)
            {
              ref<VRF_List> list = new VRF_List;
              list->setLineNumber( tokenizer()->lineNumber() );
              name_value.value().setList(list.get());
              if ( !parseList( list.get() ) )
                return false;
            }
            else
            // An ( array )
            if (mToken.mType == VRF_Token::LeftRoundBracket)
            {
              ref<VRF_Array> arr;
              if ( parseArray( arr ) )
                name_value.value().setArray(arr.get());
              else
                return false;
            }
            else
            // A {< rawtext block >}
            if (mToken.mType == VRF_Token::LeftFancyBracket)
            {
              if(!getToken(mToken) || mToken.mType != VRF_Token::RawtextBlock)
                return false;
              name_value.value().setRawtextBlock( new VRF_RawtextBlock(mLastTag.c_str()) );
              name_value.value().getRawtextBlock()->setValue( mToken.mString.c_str() );
              // consume the tag
              mLastTag.clear();
              if(!getToken(mToken) || mToken.mType != VRF_Token::RightFancyBracket)
                return false;
            }
            else
            // A "string"
            if (mToken.mType == VRF_Token::String)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setString(mToken.mString.c_str());
            }
            else
            // An Identifier
            if (mToken.mType == VRF_Token::Identifier)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setIdentifier(mToken.mString.c_str());
            }
            else
            // An #id
            if (mToken.mType == VRF_Token::UID)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setUID(mToken.mString.c_str());
            }
            else
            // A boolean true/false
            if (mToken.mType == VRF_Token::Boolean)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setBool(mToken.mString == "true");
            }
            else
            // An integer
            if (mToken.mType == VRF_Token::Integer)
            {
              if (!mLastTag.empty())
                return false;
              name_value.value().setInteger( atoll(mToken.mString.c_str()) );
            }
            else
            // A float
            if (mToken.mType == VRF_Token::Real)
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

    bool parseList(VRF_List* list)
    {
      // consume last tag if there was one
      if (!mLastTag.empty())
      {
        list->setTag(mLastTag.c_str());
        mLastTag.clear();
      }

      while(getToken(mToken))
      {
        if (mToken.mType == VRF_Token::RightSquareBracket)
          return true;
        else
        {
          VRF_Value value;
          value.setLineNumber( tokenizer()->lineNumber() );
          switch( mToken.mType )
          {
            // <tag>
            case VRF_Token::TagHeader:
              {
                if (mLastTag.empty())
                  mLastTag = mToken.mString;
                else
                  return false;
                break;
              }

            // object
            case VRF_Token::LeftCurlyBracket:
              {
                ref<VRF_Structure> object = new VRF_Structure;
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
            case VRF_Token::LeftSquareBracket:
              {
                ref<VRF_List> sub_list = new VRF_List;
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
            case VRF_Token::LeftRoundBracket:
              {
                ref<VRF_Array> arr;
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
            case VRF_Token::String:
              if (!mLastTag.empty())
                return false;
              value.setString( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // identifier
            case VRF_Token::Identifier:
              if (!mLastTag.empty())
                return false;
              value.setIdentifier( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // A {< rawtext block >}
            case VRF_Token::LeftFancyBracket:
            {
              if(!getToken(mToken) || mToken.mType != VRF_Token::RawtextBlock)
                return false;
              
              value.setRawtextBlock( new VRF_RawtextBlock(mLastTag.c_str()) );
              value.getRawtextBlock()->setValue( mToken.mString.c_str() );
              list->value().push_back( value );
              // consume the tag
              mLastTag.clear();

              if(!getToken(mToken) || mToken.mType != VRF_Token::RightFancyBracket)
                return false;
              break;
            }

            // UID
            case VRF_Token::UID:
              if (!mLastTag.empty())
                return false;
              value.setUID( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // boolean
            case VRF_Token::Boolean:
              if (!mLastTag.empty())
                return false;
              value.setBool( mToken.mString == "true" ); list->value().push_back( value );
              break;

            // int
            case VRF_Token::Integer:
              if (!mLastTag.empty())
                return false;
              value.setInteger( atoll(mToken.mString.c_str()) ); list->value().push_back( value );
              break;

            // float
            case VRF_Token::Real:
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

    bool parseArray(ref<VRF_Array>& arr)
    {
      // consume last tag if there was one
      struct struct_consume_tag
      {
        struct_consume_tag(ref<VRF_Array>* p1, std::string* p2): p_arr(p1), p_tag(p2) {}

       ~struct_consume_tag()
        {
          if ((*p_arr).get() && !p_tag->empty())
          {
            (*p_arr)->setTag(p_tag->c_str());
            p_tag->clear();
          }
        }

        ref<VRF_Array>* p_arr;
        std::string* p_tag;
      } consume_tag(&arr, &mLastTag);

      if(getToken(mToken))
      {
        // (1) from the fist token we decide what kind of array it is going to be
        // (2) empty arrays default to empty VRF_ArrayInteger

        if (mToken.mType == VRF_Token::RightRoundBracket)
        {
          arr = new VRF_ArrayInteger;
          return true;
        }
        /*
        else
        if (mToken.mType == VRF_Token::String)
        {
          ref<VRF_ArrayString> arr_string;
          arr = arr_string = new VRF_ArrayString;
          do 
            arr_string->mValue.push_back(mToken.mString);
          while(getToken(mToken) && mToken.mType == VRF_Token::String);
          return mToken.mType == VRF_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == VRF_Token::Identifier)
        {
          ref<VRF_ArrayIdentifier> arr_identifier;
          arr = arr_identifier = new VRF_ArrayIdentifier;
          do 
            arr_identifier->mValue.push_back(mToken.mString);
          while(getToken(mToken) && mToken.mType == VRF_Token::Identifier);
          return mToken.mType == VRF_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == VRF_Token::UID)
        {
          ref<VRF_ArrayUID> arr_uid;
          arr = arr_uid = new VRF_ArrayUID;
          do
            arr_uid->mValue.push_back(mToken.mString.c_str());
          while(getToken(mToken) && mToken.mType == VRF_Token::UID);
          return mToken.mType == VRF_Token::RightRoundBracket;
        }
        */
        else
        if (mToken.mType == VRF_Token::Integer)
        {
          ref<VRF_ArrayInteger> arr_integer;
          arr = arr_integer = new VRF_ArrayInteger;
          do
          {
            switch(mToken.mType)
            {
            case VRF_Token::Integer: arr_integer->value().push_back( atoll( mToken.mString.c_str() ) ); break;
            case VRF_Token::RightRoundBracket: return true;
            default:
              return false;
            }
          }
          while(getToken(mToken));
          return false;
        }
        else
        if (mToken.mType == VRF_Token::Real)
        {
          ref<VRF_ArrayReal> arr_floating;
          arr = arr_floating = new VRF_ArrayReal;
          do
          {
            switch(mToken.mType)
            {
            case VRF_Token::Integer:
            case VRF_Token::Real: arr_floating->mValue.push_back( atof( mToken.mString.c_str() ) ); break;
            case VRF_Token::RightRoundBracket: return true;
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
      while(getToken(mToken) && mToken.mType != VRF_Token::TOKEN_EOF)
      {
        switch(mToken.mType)
        {
          case VRF_Token::LeftRoundBracket:   printf("LeftSquareBracket (\n"); break;
          case VRF_Token::RightRoundBracket:  printf("RightSquareBracket )\n"); break;
          case VRF_Token::LeftSquareBracket:  printf("LeftSquareBracket [\n"); break;
          case VRF_Token::RightSquareBracket: printf("RightSquareBracket ]\n"); break;
          case VRF_Token::LeftCurlyBracket:   printf("LeftCurlyBracket {\n"); break;
          case VRF_Token::RightCurlyBracket:  printf("RightCurlyBracket } \n"); break;
          case VRF_Token::LeftFancyBracket:   printf("LeftFancyBracket >}\n"); break;
          case VRF_Token::RightFancyBracket:  printf("RightFancyBracket {< \n"); break;
          case VRF_Token::Equals:             printf("Equals =\n"); break;
          case VRF_Token::String:             printf("String = %s\n", mToken.mString.c_str()); break;
          case VRF_Token::UID:                printf("UID = %s\n", mToken.mString.c_str()); break;
          case VRF_Token::Identifier:         printf("Identifier = %s\n", mToken.mString.c_str()); break;
          case VRF_Token::RawtextBlock:       printf("RawtextBlock = %s\n", mToken.mString.c_str()); break;
          case VRF_Token::Real:               printf("Real = %s\n", mToken.mString.c_str()); break;
          case VRF_Token::Integer:            printf("Integer = %s\n", mToken.mString.c_str()); break;
          case VRF_Token::TagHeader:          printf("TagHeader = %s\n", mToken.mString.c_str()); break;
          default:
            break;
        }
      }
      if (mToken.mType != VRF_Token::TOKEN_EOF)
      {
        printf("Line %d: syntax error : '%s'.\n", mTokenizer->lineNumber(), mToken.mString.c_str());
      }
    }

    VRF_Tokenizer* tokenizer() { return mTokenizer.get(); }

    const VRF_Tokenizer* tokenizer() const { return mTokenizer.get(); }

    const std::map< std::string, ref<VRF_Structure> >& linkMap() const { return mLinkMap; }

    VRF_Structure* root() { return mRoot.get(); }

    const VRF_Structure* root() const { return mRoot.get(); }

  private:
    std::string mLastTag;
    ref<VRF_Structure> mRoot;
    std::map< std::string, ref<VRF_Structure> > mLinkMap;
    ref<VRF_Tokenizer> mTokenizer;
    VRF_Token mToken;
  };
  //-----------------------------------------------------------------------------
}

#endif
