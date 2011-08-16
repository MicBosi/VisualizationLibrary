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

#ifndef SRF_INCLUDE_ONCE
#define SRF_INCLUDE_ONCE

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
  class SRF_Structure;
  class SRF_List;
  class SRF_Array;
  class SRF_ArrayString;
  class SRF_ArrayIdentifier;
  class SRF_ArrayUID;
  class SRF_ArrayInt32;
  class SRF_ArrayInt64;
  class SRF_ArrayFloat;
  class SRF_ArrayDouble;
  //-----------------------------------------------------------------------------
  class SRF_Visitor: public Object
  {
  public:
    virtual void visitStructur(SRF_Structure*) {}
    virtual void visitList(SRF_List*) {}
    virtual void visitArray(SRF_ArrayUID*) {}
    virtual void visitArray(SRF_ArrayInt32*) {}
    virtual void visitArray(SRF_ArrayInt64*) {}
    virtual void visitArray(SRF_ArrayFloat*) {}
    virtual void visitArray(SRF_ArrayDouble*) {}
    virtual void visitArray(SRF_ArrayString*) {}
    virtual void visitArray(SRF_ArrayIdentifier*) {}

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
  class SRF_AbstractComplexValue: public Object
  {
    VL_INSTRUMENT_CLASS(vl::SRF_AbstractComplexValue, Object)

  public:
    SRF_AbstractComplexValue(): mLineNumber(0) {}

    virtual ~SRF_AbstractComplexValue() {}

    int lineNumber() const { return mLineNumber; }

    void setLineNumber(int line) { mLineNumber = line; }

    virtual void acceptVisitor(SRF_Visitor*) = 0;
  
  private:
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // Arrays
  //-----------------------------------------------------------------------------
  class SRF_Array: public SRF_AbstractComplexValue 
  { 
    VL_INSTRUMENT_CLASS(vl::SRF_Array, SRF_AbstractComplexValue)

  public:
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayInt32: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayInt32, SRF_Array)

  public:
    typedef int scalar_type;

  public:
    SRF_ArrayInt32() {}

    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<int>& value() { return mValue; }

    const std::vector<int>& value() const { return mValue; }

    int* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }
    
    const int* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  private:
    std::vector<int> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayInt64: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayInt64, SRF_Array)

  public:
    typedef long long scalar_type;

  public:
    SRF_ArrayInt64() {}
    
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<long long>& value() { return mValue; }
    
    const std::vector<long long>& value() const { return mValue; }

    long long* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }
    
    const long long* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  private:
    std::vector<long long> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayFloat: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayFloat, SRF_Array)

  public:
    typedef float scalar_type;

  public:
    SRF_ArrayFloat() {}
    
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<float>& value() { return mValue; }
    
    const std::vector<float>& value() const { return mValue; }

    float* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }
    
    const float* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

    fvec2 getFloat2() const { VL_CHECK(mValue.size() == 2); return fvec2(mValue[0], mValue[1]); }
    
    fvec3 getFloat3() const { VL_CHECK(mValue.size() == 3); return fvec3(mValue[0], mValue[1], mValue[2]); }
    
    fvec4 getFloat4() const { VL_CHECK(mValue.size() == 4); return fvec4(mValue[0], mValue[1], mValue[2], mValue[3]); }

  private:
    std::vector<float> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayDouble: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayDouble, SRF_Array)

  public:
    typedef double scalar_type;

  public:
    SRF_ArrayDouble() {}
    
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<double>& value() { return mValue; }
    
    const std::vector<double>& value() const { return mValue; }

    double* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }
    
    const double* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

    dvec2 getDouble2() const { VL_CHECK(mValue.size() == 2); return dvec2(mValue[0], mValue[1]); }
    
    dvec3 getDouble3() const { VL_CHECK(mValue.size() == 3); return dvec3(mValue[0], mValue[1], mValue[2]); }
    
    dvec4 getDouble4() const { VL_CHECK(mValue.size() == 4); return dvec4(mValue[0], mValue[1], mValue[2], mValue[3]); }

  public:
    std::vector<double> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayUID: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayUID, SRF_Array)

  public:
    SRF_ArrayUID() {}

    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    class Value
    {
    public:
      Value(const char* uid): mUID(uid) {}

      void setUID(const char* uid) { mUID = uid; }

      const char* uid() const { return mUID.c_str(); }

      void setStructure(SRF_Structure* obj) { mObj = obj; }

      SRF_Structure* object() { return mObj.get(); }

      const SRF_Structure* object() const { return mObj.get(); }

    private:
      std::string mUID; // the UID string
      ref<SRF_Structure> mObj; // the linked object
    };

    std::vector<Value>& value() { return mValue; }

    const std::vector<Value>& value() const { return mValue; }

    Value* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const Value* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<Value> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayIdentifier: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayIdentifier, SRF_Array)

  public:
    SRF_ArrayIdentifier() {}
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayString: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayString, SRF_Array)

  public:
    SRF_ArrayString() {}
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<std::string>& value() { return mValue; }

    const std::vector<std::string>& value() const { return mValue; }

    std::string* ptr() { if (mValue.empty()) return NULL; else return &mValue[0]; }

    const std::string* ptr() const { if (mValue.empty()) return NULL; else return &mValue[0]; }

  public:
    std::vector<std::string> mValue;
  };
  //-----------------------------------------------------------------------------
  // SRF_Value
  //-----------------------------------------------------------------------------
  class SRF_Value
  {
  public:
    enum EType 
    {
      Bool,
      Int64,
      Double,
      String,
      Identifier,
      UID,
      List,
      Structure,
      ArrayString,
      ArrayIdentifier,
      ArrayUID,
      ArrayInt32,
      ArrayInt64,
      ArrayFloat,
      ArrayDouble
    };

  private:
    void release();

  public:
    SRF_Value()
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;
    }

    SRF_Value(SRF_Structure* obj)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setStructure(obj);
    }

    SRF_Value(SRF_List* list)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setList(list);
    }

    SRF_Value(SRF_Array* arr)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray(arr);
    }

    SRF_Value(long long i)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = i;
    }

    SRF_Value(double d)
    {
      mLineNumber = 0;
      mType = Double;
      mUnion.mDouble  = d;
    }

    SRF_Value(const char* str, EType type)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

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

    SRF_Value(bool boolean)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setBool(boolean);
    }

    SRF_Value(const fvec4& vec)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray( new SRF_ArrayFloat );
      getArrayFloat()->value().resize(4);
      memcpy(&getArrayFloat()->value()[0], vec.ptr(), sizeof(vec));
    }

    SRF_Value(const fvec3& vec)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray( new SRF_ArrayFloat );
      getArrayFloat()->value().resize(3);
      memcpy(&getArrayFloat()->value()[0], vec.ptr(), sizeof(vec));
    }

    SRF_Value(const fvec2& vec)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray( new SRF_ArrayFloat );
      getArrayFloat()->value().resize(2);
      memcpy(&getArrayFloat()->value()[0], vec.ptr(), sizeof(vec));
    }

    SRF_Value(const dvec4& vec)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray( new SRF_ArrayDouble );
      getArrayDouble()->value().resize(4);
      memcpy(&getArrayDouble()->value()[0], vec.ptr(), sizeof(vec));
    }

    SRF_Value(const dvec3& vec)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray( new SRF_ArrayDouble );
      getArrayDouble()->value().resize(3);
      memcpy(&getArrayDouble()->value()[0], vec.ptr(), sizeof(vec));
    }

    SRF_Value(const dvec2& vec)
    {
      mLineNumber = 0;
      mType = Int64;
      mUnion.mInt64 = 0;

      setArray( new SRF_ArrayDouble );
      getArrayDouble()->value().resize(2);
      memcpy(&getArrayDouble()->value()[0], vec.ptr(), sizeof(vec));
    }

    SRF_Value(const SRF_Value& other)
    {
      mType = Int64;
      mUnion.mInt64 = 0;
      mLineNumber = 0;

      *this = other;
    }

    ~SRF_Value() { release(); }

    SRF_Value& operator=(const SRF_Value& other);

    EType type() const { return mType; }

    // object

    SRF_Structure* setStructure(SRF_Structure*);

    SRF_Structure* getStructure() { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    const SRF_Structure* getStructure() const { VL_CHECK(mType == Structure); return mUnion.mStructure; }

    // list

    SRF_List* setList(SRF_List*);

    SRF_List* getList() { VL_CHECK(mType == List); return mUnion.mList; }

    const SRF_List* getList() const { VL_CHECK(mType == List); return mUnion.mList; }

    // array

    SRF_Array* setArray(SRF_Array*);

    SRF_ArrayString* getArrayString() { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<SRF_ArrayString>(); }
    const SRF_ArrayString* getArrayString() const { VL_CHECK(mType == ArrayString); return mUnion.mArray->as<SRF_ArrayString>(); }

    SRF_ArrayIdentifier* getArrayIdentifier() { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<SRF_ArrayIdentifier>(); }
    const SRF_ArrayIdentifier* getArrayIdentifier() const { VL_CHECK(mType == ArrayIdentifier); return mUnion.mArray->as<SRF_ArrayIdentifier>(); }

    SRF_ArrayUID* getArrayUID() { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<SRF_ArrayUID>(); }
    const SRF_ArrayUID* getArrayUID() const { VL_CHECK(mType == ArrayUID); return mUnion.mArray->as<SRF_ArrayUID>(); }

    SRF_ArrayInt32* getArrayInt32() { VL_CHECK(mType == ArrayInt32); return mUnion.mArray->as<SRF_ArrayInt32>(); }
    const SRF_ArrayInt32* getArrayInt32() const { VL_CHECK(mType == ArrayInt32); return mUnion.mArray->as<SRF_ArrayInt32>(); }

    SRF_ArrayInt64* getArrayInt64() { VL_CHECK(mType == ArrayInt64); return mUnion.mArray->as<SRF_ArrayInt64>(); }
    const SRF_ArrayInt64* getArrayInt64() const { VL_CHECK(mType == ArrayInt64); return mUnion.mArray->as<SRF_ArrayInt64>(); }

    SRF_ArrayFloat* getArrayFloat() { VL_CHECK(mType == ArrayFloat); return mUnion.mArray->as<SRF_ArrayFloat>(); }
    const SRF_ArrayFloat* getArrayFloat() const { VL_CHECK(mType == ArrayFloat); return mUnion.mArray->as<SRF_ArrayFloat>(); }

    SRF_ArrayDouble* getArrayDouble() { VL_CHECK(mType == ArrayDouble); return mUnion.mArray->as<SRF_ArrayDouble>(); }
    const SRF_ArrayDouble* getArrayDouble() const { VL_CHECK(mType == ArrayDouble); return mUnion.mArray->as<SRF_ArrayDouble>(); }

    const char*  setString(const char* str)
    {
      release();
      mType = String;
      return mUnion.mString = _strdup(str);
    }

    const char* getString() const { VL_CHECK(mType == String); return mUnion.mString; }

    const char* setIdentifier(const char* str)
    {
      release();
      mType = Identifier;
      return mUnion.mString = _strdup(str);
    }

    const char* getIdentifier() const { VL_CHECK(mType == Identifier); return mUnion.mString; }

    const char* setUID(const char* str)
    {
      release();
      mType = UID;
      return mUnion.mString = _strdup(str);
    }

    const char* getUID() const { VL_CHECK(mType == UID); return mUnion.mString; }

    long long  setInt64(long long val)
    {
      release();
      mType = Int64;
      return mUnion.mInt64 = val;
    }

    long long getInt64() const { VL_CHECK(mType == Int64); return mUnion.mInt64; }

    double setDouble(double val)
    {
      release();
      mType = Double;
      return mUnion.mDouble = val;
    }

    double getDouble() const { VL_CHECK(mType == Double); return mUnion.mDouble; }

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
      long long mInt64;
      double mDouble;
      const char* mString;
      SRF_Structure* mStructure;
      SRF_List* mList;
      SRF_Array* mArray;
    } mUnion;

  private:
    EType mType;
    int mLineNumber; // the line number coming from the tokenizer
  };
  //-----------------------------------------------------------------------------
  // SRF_Structure
  //-----------------------------------------------------------------------------
  class SRF_Structure: public SRF_AbstractComplexValue
  {
    VL_INSTRUMENT_CLASS(vl::SRF_Structure, SRF_AbstractComplexValue)

  public:
    SRF_Structure()
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
    }

    SRF_Structure(const char* tag)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID("#NULL");
      setTag(tag);
    }

    SRF_Structure(const char* tag, const std::string& uid)
    {
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      setUID(uid.c_str());
      setTag(tag);
    }

    virtual void acceptVisitor(SRF_Visitor* v) { v->visitStructur(this); }

    class Value
    {
      friend class SRF_Structure;

    public:
      Value() {}
      Value(const char* key, SRF_Value value): mKey(key), mValue(value) {}

      std::string& key() { return mKey; }
      const std::string& key() const { return mKey; }
      void setKey(const char* key) { mKey = key; }

      SRF_Value& value() { return mValue; }
      const SRF_Value& value() const { return mValue; }
      void setValue(const SRF_Value& value) { mValue = value; }

    private:
      std::string mKey;
      SRF_Value mValue;
    };

    void setTag(const char* tag) { mTag = tag; }

    const std::string& tag() const { return mTag; }

    void setUID(const char* uid) { mUID = uid; }

    const std::string& uid() const { return mUID; }

    std::vector<Value>& value() { return mKeyValue; }

    const std::vector<Value>& value() const { return mKeyValue; }

    // mic fixme: we can speed this guys up with multimaps
    SRF_Value* getValue(const char* key)
    {
      for(size_t i=0; i<mKeyValue.size(); ++i)
        if (mKeyValue[i].key() == key)
          return &mKeyValue[i].value();
      return NULL;
    }

    const SRF_Value* getValue(const char* key) const
    {
      for(size_t i=0; i<mKeyValue.size(); ++i)
        if (mKeyValue[i].key() == key)
          return &mKeyValue[i].value();
      return NULL;
    }

  private:
    // mic fixme: add a multimap for quick access
    std::string mTag;
    std::string mUID;
    std::vector<Value> mKeyValue;
  };
  //-----------------------------------------------------------------------------
  // SRF_List
  //-----------------------------------------------------------------------------
  class SRF_List: public SRF_AbstractComplexValue
  {
    VL_INSTRUMENT_CLASS(vl::SRF_List, SRF_AbstractComplexValue)

  public:
    SRF_List()
    {
      // mic fixme: reenable
      // mValue.reserve(16);
    }

    virtual void acceptVisitor(SRF_Visitor* v) { v->visitList(this); }

    std::vector< SRF_Value >& value() { return mValue; }

    const std::vector< SRF_Value >& value() const { return mValue; }

  private:
    std::vector< SRF_Value > mValue;
  };
  //-----------------------------------------------------------------------------
  // SRF_TextExportVisitor
  //-----------------------------------------------------------------------------
  class SRF_TextExportVisitor: public SRF_Visitor
  {
  public:
    SRF_TextExportVisitor()
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

    virtual void visitStructur(SRF_Structure* obj)
    {
      // mic fixme: check this
      if (isVisited(obj))
      {
        indent(); format("%s\n", obj->uid().c_str());
        return;
      }

      indent(); format("%s\n", obj->tag().c_str());
      indent(); output("{\n");
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
        case SRF_Value::Structure:
          mAssign = true;
          obj->value()[i].value().getStructure()->acceptVisitor(this);
          break;
        case SRF_Value::List:
          mAssign = true;
          obj->value()[i].value().getList()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayString:
          mAssign = true;
          obj->value()[i].value().getArrayString()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayIdentifier:
          mAssign = true;
          obj->value()[i].value().getArrayIdentifier()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayUID:
          mAssign = true;
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayInt32:
          mAssign = true;
          obj->value()[i].value().getArrayInt32()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayInt64:
          mAssign = true;
          obj->value()[i].value().getArrayInt64()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayFloat:
          mAssign = true;
          obj->value()[i].value().getArrayFloat()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayDouble:
          mAssign = true;
          obj->value()[i].value().getArrayDouble()->acceptVisitor(this);
          break;
        case SRF_Value::String:
          format("%s\n", obj->value()[i].value().getString()); VL_CHECK( strlen(obj->value()[i].value().getString()) )
          break;
        case SRF_Value::Identifier:
          format("%s\n", obj->value()[i].value().getIdentifier()); VL_CHECK( strlen(obj->value()[i].value().getIdentifier()) )
          break;
        case SRF_Value::UID:
          format("%s\n", obj->value()[i].value().getUID()); VL_CHECK( strlen(obj->value()[i].value().getUID()) )
          break;
        case SRF_Value::Bool:
          format("%s\n", obj->value()[i].value().getBool() ? "true" : "false");
          break;
        case SRF_Value::Int64:
          format("%lld\n", obj->value()[i].value().getInt64());
          break;
        case SRF_Value::Double:
          format("%Lf\n", obj->value()[i].value().getDouble());
          break;
        }
      }
      mIndent--;
      indent(); output("}\n");
    }

    virtual void visitList(SRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("SRF_TextExportVisitor: cycle detected on SRF_List.\n");
        return;
      }

      indent(); output("[\n");
      mIndent++;
      for(size_t i=0; i<list->value().size(); ++i)
      {
        switch(list->value()[i].type())
        {
        case SRF_Value::Structure:
          list->value()[i].getStructure()->acceptVisitor(this);
          break;
        case SRF_Value::List:
          list->value()[i].getList()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayString:
          list->value()[i].getArrayString()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayIdentifier:
          list->value()[i].getArrayIdentifier()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayUID:
          list->value()[i].getArrayUID()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayInt32:
          list->value()[i].getArrayInt32()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayInt64:
          list->value()[i].getArrayInt64()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayFloat:
          list->value()[i].getArrayFloat()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayDouble:
          list->value()[i].getArrayDouble()->acceptVisitor(this);
          break;
        case SRF_Value::String:
          format("%s\n", list->value()[i].getString()); VL_CHECK( strlen(list->value()[i].getString()) )
          break;
        case SRF_Value::Identifier:
          format("%s\n", list->value()[i].getIdentifier()); VL_CHECK( strlen(list->value()[i].getIdentifier()) )
          break;
        case SRF_Value::UID:
          format("%s\n", list->value()[i].getUID()); VL_CHECK( strlen(list->value()[i].getUID()) )
          break;
        case SRF_Value::Bool:
          format("%s\n", list->value()[i].getBool() ? "true" : "false");
          break;
        case SRF_Value::Int64:
          format("%lld\n", list->value()[i].getInt64());
          break;
        case SRF_Value::Double:
          format("%Lf\n", list->value()[i].getDouble());
          break;
        }
      }
      mIndent--;
      indent(); output("]\n");
    }

    virtual void visitArray(SRF_ArrayInt32* arr)
    {
      indent(); output("( ");
      // output in chunks of 10 numbers
      int i = 0;
      int size = arr->value().size() - 10;
      for( ; i < size; i += 10)
      {
        format("%d %d %d %d %d %d %d %d %d %d ",
          arr->value()[i+0], arr->value()[i+1], arr->value()[i+2], arr->value()[i+3], arr->value()[i+4],
          arr->value()[i+5], arr->value()[i+6], arr->value()[i+7], arr->value()[i+8], arr->value()[i+9] );
      }
      for( ; i < (int)arr->value().size(); ++i )
        format("%d ", arr->value()[i]);
      VL_CHECK( i == (int)arr->value().size() )
      output(")\n");
    }

    virtual void visitArray(SRF_ArrayInt64* arr)
    {
      indent(); output("( ");
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

    virtual void visitArray(SRF_ArrayFloat* arr)
    {
      indent(); output("( ");
      // output in chunks of 10 numbers
      int i = 0;
      int size = arr->value().size() - 10;
      for( ; i < size; i += 10)
      {
        format("%f %f %f %f %f %f %f %f %f %f ",
          arr->value()[i+0], arr->value()[i+1], arr->value()[i+2], arr->value()[i+3], arr->value()[i+4],
          arr->value()[i+5], arr->value()[i+6], arr->value()[i+7], arr->value()[i+8], arr->value()[i+9] );
      }
      for( ; i < (int)arr->value().size(); ++i )
        format("%f ", arr->value()[i]);
      VL_CHECK( i == (int)arr->value().size() )
      output(")\n"); 
    }

    virtual void visitArray(SRF_ArrayDouble* arr)
    {
      indent(); output("( ");
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

    virtual void visitArray(SRF_ArrayIdentifier* arr)
    {
      indent(); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        format("%s ", arr->value()[i].c_str());
      output(")\n");
    }

    virtual void visitArray(SRF_ArrayString* arr)
    {
      indent(); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        output(std::string("\"") + encodeString(arr->value()[i].c_str()) + "\" ");
      output(")\n");
    }

    virtual void visitArray(SRF_ArrayUID* arr)
    {
      indent(); output("( ");
      for(size_t i=0 ;i<arr->value().size(); ++i)
        format("%s ", arr->value()[i].uid());
      output(")\n");
    }

    // mic fixme: test this both as input and as output!
    // support \xHH hex notation both input and output.
    std::string encodeString(const char* str)
    {
      std::string out;
      for(size_t i=0; str[i]; ++i)
      {
        if (str[i] == '"')
          out += "\"";
        else
        if (str[i] == '\\')
          out += "\\";
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
      mText += str;
    }

    void setHeader()
    {
      mText = "Simple_Runtime_Format_Text\n"
              "Version = 100\n"
              "Encoding = ascii\n"
              "\n";
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
  // SRF_LinkMapperVisitor: 
  // Compiles the link-map which associates an SRF_Structure to it's UID.
  // Can be called multiple times
  //-----------------------------------------------------------------------------
  class SRF_LinkMapperVisitor: public SRF_Visitor
  {
  public:
    typedef enum 
    {
      NoError,
      DuplicateUID
    } EError;

  public:
    SRF_LinkMapperVisitor(std::map< std::string, ref<SRF_Structure> >* map)
    {
      mLinkMap = map;
      mError = NoError;
    }

    void setLinkMap(std::map< std::string, ref<SRF_Structure> >* map)
    {
      mLinkMap = map;
    }

    void declareUID(SRF_Structure* obj)
    {
      if (obj->uid() != "#NULL")
      {
        const std::map< std::string, ref<SRF_Structure> >::const_iterator it = mLinkMap->find(obj->uid());
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

    virtual void visitStructur(SRF_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      declareUID(obj);

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == SRF_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
      }
    }

    virtual void visitList(SRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("SRF_LinkMapperVisitor: cycle detected on SRF_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == SRF_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
      }
    }

    virtual void visitArray(SRF_ArrayUID*) {}

    virtual void visitArray(SRF_ArrayIdentifier*) {}

    virtual void visitArray(SRF_ArrayString*)  {}

    virtual void visitArray(SRF_ArrayInt32*)  {}

    virtual void visitArray(SRF_ArrayInt64*)  {}

    virtual void visitArray(SRF_ArrayFloat*)  {}

    virtual void visitArray(SRF_ArrayDouble*)  {}

    EError error() const { return mError; }

    void setError(EError err) { mError = err; }

  private:
    std::map< std::string, ref<SRF_Structure> >* mLinkMap;
    EError mError;
  };
  //-----------------------------------------------------------------------------
  // SRF_LinkVisitor:
  // Translates UIDs into SRF_Structures
  //-----------------------------------------------------------------------------
  class SRF_LinkVisitor: public SRF_Visitor
  {
  public:
    typedef enum 
    {
      NoError,
      UnresolvedUID
    } EError;

  public:
    SRF_LinkVisitor(const std::map< std::string, ref<SRF_Structure> >* map)
    {
      mLinkMap = map;
      mError = NoError;
    }

    void setLinkMap(const std::map< std::string, ref<SRF_Structure> >* map)
    {
      mLinkMap = map;
    }

    SRF_Structure* link(const std::string& uid)
    {
      VL_CHECK(mLinkMap)
      VL_CHECK(!uid.empty())
      std::map< std::string, ref<SRF_Structure> >::const_iterator it = mLinkMap->find(uid);
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

    virtual void visitStructur(SRF_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == SRF_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::ArrayUID)
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::UID)
        {
          // transform UID -> Structure
          SRF_Structure* lnk_obj = link( obj->value()[i].value().getUID() );
          obj->value()[i].value().setStructure( lnk_obj );
        }
      }
    }

    virtual void visitList(SRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("SRF_LinkVisitor: cycle detected on SRF_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == SRF_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == SRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::UID)
        {
          // transform UID -> Structure
          SRF_Structure* lnk_obj = link( list->value()[i].getUID() );
          list->value()[i].setStructure( lnk_obj );
        }
      }
    }

    virtual void visitArray(SRF_ArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        arr->value()[i].setStructure ( link(arr->value()[i].uid()) );
    }

    virtual void visitArray(SRF_ArrayIdentifier*) {}

    virtual void visitArray(SRF_ArrayString*)  {}

    virtual void visitArray(SRF_ArrayInt32*)  {}

    virtual void visitArray(SRF_ArrayInt64*)  {}

    virtual void visitArray(SRF_ArrayFloat*)  {}

    virtual void visitArray(SRF_ArrayDouble*)  {}

    EError error() const { return mError; }

    void setError(EError err) { mError = err; }

  private:
    const std::map< std::string, ref<SRF_Structure> >* mLinkMap;
    EError mError;
  };
  //-----------------------------------------------------------------------------
  // SRF_UIDCollectorVisitor:
  // Sets to #NULL the UID of those objects that are not referenced by anybody.
  // Useful before exporting.
  //-----------------------------------------------------------------------------
  class SRF_UIDCollectorVisitor: public SRF_Visitor
  {
  public:
    SRF_UIDCollectorVisitor(): mUIDSet(NULL) {}

    virtual void visitStructur(SRF_Structure* obj)
    {
      // mic fixme: test this
      if (isVisited(obj))
        return;

      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].value().type() == SRF_Value::Structure)
          obj->value()[i].value().getStructure()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::List)
          obj->value()[i].value().getList()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::ArrayUID)
          obj->value()[i].value().getArrayUID()->acceptVisitor(this);
        else
        if (obj->value()[i].value().type() == SRF_Value::UID)
          mUIDSet->insert(obj->value()[i].value().getUID());
      }
    }

    virtual void visitList(SRF_List* list)
    {
      // this should happen only if the user manually creates loops
      if (isVisited(list))
      {
        Log::warning("SRF_UIDCollectorVisitor: cycle detected on SRF_List.\n");
        return;
      }

      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == SRF_Value::Structure)
          list->value()[i].getStructure()->acceptVisitor(this);
        if (list->value()[i].type() == SRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::UID)
          mUIDSet->insert(list->value()[i].getUID());
      }
    }

    virtual void visitArray(SRF_ArrayUID* arr)
    {
      // retrieves the assigned Structure
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mUIDSet->insert(arr->value()[i].uid());
    }

    virtual void visitArray(SRF_ArrayIdentifier*) {}

    virtual void visitArray(SRF_ArrayString*)  {}

    virtual void visitArray(SRF_ArrayInt32*)  {}

    virtual void visitArray(SRF_ArrayInt64*)  {}

    virtual void visitArray(SRF_ArrayFloat*)  {}

    virtual void visitArray(SRF_ArrayDouble*)  {}

    void setUIDSet(std::set< std::string >* uids) { mUIDSet = uids; }

    std::set< std::string >* uidSet() { return mUIDSet; }

    const std::set< std::string >* uidSet() const { return mUIDSet; }

  private:
    std::set< std::string >* mUIDSet;
  };
  //-----------------------------------------------------------------------------
  // Links several hierachies also resolving UIDs across them.
  //-----------------------------------------------------------------------------
  class SRF_Linker
  {
  public:
    void add(SRF_AbstractComplexValue* module)
    {
      mModules.push_back(module);
    }

    bool link()
    {
      std::map< std::string, ref<SRF_Structure> > link_map;

      // map all the UIDs to the appropriate SRF_Structures
      SRF_LinkMapperVisitor link_mapper(&link_map);
      for(size_t i=0; i<mModules.size(); ++i)
        mModules[i]->acceptVisitor(&link_mapper);

      if (link_mapper.error())
        return false;

      // link all the UIDs to the associated SRF_Structure
      SRF_LinkVisitor linker(&link_map);
      for(size_t i=0; i<mModules.size(); ++i)
        mModules[i]->acceptVisitor(&linker);

      if (linker.error())
        return false;

      return true;
    }

    std::vector< ref<SRF_AbstractComplexValue> >& modules() { return mModules; }

    const std::vector< ref<SRF_AbstractComplexValue> >& modules() const { return mModules; }

  public:
    std::vector< ref<SRF_AbstractComplexValue> > mModules;
  };
  //-----------------------------------------------------------------------------
  class SRF_Token
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
      Int32,              //  +123
      Int64,              //  +123
      Float,              //  +123.456e+10
      Double,             //  +123.456e+10
      StructureHeader          //  <StructureHeader>

    } EType;

    SRF_Token(): mType(TOKEN_ERROR) {}

    std::string mString;
    EType mType;
  };
  //-----------------------------------------------------------------------------
  class SRF_Tokenizer: public BufferedStream<char, 128*1024>
  {
  public:
    SRF_Tokenizer(): mLineNumber(1) {}

    bool getToken(SRF_Token& token);

    int lineNumber() const { return mLineNumber; }

  private:
    int mLineNumber;
  };
  //-----------------------------------------------------------------------------
  // SRF_Parser
  //-----------------------------------------------------------------------------
  class SRF_Parser
  {
  public:
    SRF_Parser()
    {
      mTokenizer = new SRF_Tokenizer;
    }

    bool getToken(SRF_Token& token) { return mTokenizer->getToken(token); }

    std::string exportToText()
    {
      if (mRoot)
      {
        std::set< std::string > uid_set;
        SRF_UIDCollectorVisitor uid_collector;
        uid_collector.setUIDSet(&uid_set);
        mRoot->acceptVisitor(&uid_collector);

        SRF_TextExportVisitor text_export_visitor;
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
        SRF_Linker linker;
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
      if (!getToken(mToken) || mToken.mType != SRF_Token::Identifier || mToken.mString != "Simple_Runtime_Format_Text")
      {
        Log::error("'Simple_Runtime_Format_Text' header not found!\n");
        return false;
      }

      // Version
      if (!getToken(mToken) || mToken.mType != SRF_Token::Identifier || mToken.mString != "Version")
        return false;

      if (!getToken(mToken) || mToken.mType != SRF_Token::Equals)
        return false;

      if (!getToken(mToken) || mToken.mType != SRF_Token::Int32 || mToken.mString != "100")
        return false;
      else
        version = atoi( mToken.mString.c_str() );

      // Encoding
      if (!getToken(mToken) || mToken.mType != SRF_Token::Identifier || mToken.mString != "Encoding")
        return false;

      if (!getToken(mToken) || mToken.mType != SRF_Token::Equals)
        return false;

      if (!getToken(mToken) || mToken.mType != SRF_Token::Identifier || mToken.mString != "ascii")
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
      if(getToken(mToken) && mToken.mType == SRF_Token::StructureHeader)
      {
        mRoot = new SRF_Structure;
        mRoot->setLineNumber( tokenizer()->lineNumber() );
        mRoot->setTag(mToken.mString.c_str());
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

    bool parseStructure(SRF_Structure* object)
    {
      if (!getToken(mToken) || mToken.mType != SRF_Token::LeftCurlyBracket)
        return false;
      while(getToken(mToken))
      {
        if (mToken.mType == SRF_Token::RightCurlyBracket)
        {
          return true;
        }
        else
        if (mToken.mType == SRF_Token::Identifier)
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
              if (!getToken(mToken) || mToken.mType != SRF_Token::Equals)
                return false;

              // #identifier
              if (getToken(mToken) && mToken.mType == SRF_Token::UID)
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
          object->value().push_back( SRF_Structure::Value() );
          SRF_Structure::Value& name_value = object->value().back();

          // Key
          name_value.setKey( mToken.mString.c_str() );

          // Equals
          if (!getToken(mToken) || mToken.mType != SRF_Token::Equals)
            return false;

          // Member value
          if (getToken(mToken))
          {
            name_value.value().setLineNumber( tokenizer()->lineNumber() );

            // A new <Structure>
            if (mToken.mType == SRF_Token::StructureHeader)
            {
              ref<SRF_Structure> object = new SRF_Structure;
              object->setLineNumber( tokenizer()->lineNumber() );
              object->setTag(mToken.mString.c_str());
              name_value.value().setStructure(object.get());
              if (!parseStructure( object.get() ) )
                return false;
            }
            else
            // An [ list ]
            if (mToken.mType == SRF_Token::LeftSquareBracket)
            {
              ref<SRF_List> list = new SRF_List;
              list->setLineNumber( tokenizer()->lineNumber() );
              name_value.value().setList(list.get());
              if ( !parseList( list.get() ) )
                return false;
            }
            else
            // An ( array )
            if (mToken.mType == SRF_Token::LeftRoundBracket)
            {
              ref<SRF_Array> arr;
              if ( parseArray( arr ) )
                name_value.value().setArray(arr.get());
              else
                return false;
            }
            else
            // A "string"
            if (mToken.mType == SRF_Token::String)
            {
              name_value.value().setString(mToken.mString.c_str());
            }
            else
            // An Identifier
            if (mToken.mType == SRF_Token::Identifier)
            {
              name_value.value().setIdentifier(mToken.mString.c_str());
            }
            else
            // An #id
            if (mToken.mType == SRF_Token::UID)
            {
              name_value.value().setUID(mToken.mString.c_str());
            }
            else
            // A boolean (true/false)
            if (mToken.mType == SRF_Token::Boolean)
            {
              name_value.value().setBool(mToken.mString == "true");
            }
            else
            // An integer
            if (mToken.mType == SRF_Token::Int32 || mToken.mType == SRF_Token::Int64)
            {
              name_value.value().setInt64( atoll(mToken.mString.c_str()) );
            }
            else
            // An float
            if (mToken.mType == SRF_Token::Float || mToken.mType == SRF_Token::Double)
            {
              name_value.value().setDouble( atof(mToken.mString.c_str()) );
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

    bool parseList(SRF_List* list)
    {
      while(getToken(mToken))
      {
        if (mToken.mType == SRF_Token::RightSquareBracket)
          return true;
        else
        {
          SRF_Value value;
          value.setLineNumber( tokenizer()->lineNumber() );
          switch( mToken.mType )
          {
            // object
            case SRF_Token::StructureHeader:
              {
                ref<SRF_Structure> object = new SRF_Structure;
                object->setLineNumber( tokenizer()->lineNumber() );
                object->setTag(mToken.mString.c_str());
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
            case SRF_Token::LeftSquareBracket:
              {
                ref<SRF_List> sub_list = new SRF_List;
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
            case SRF_Token::LeftRoundBracket:
              {
                ref<SRF_Array> arr;
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
            case SRF_Token::String:
              value.setString( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // identifier
            case SRF_Token::Identifier:
              value.setIdentifier( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // UID
            case SRF_Token::UID:
              value.setUID( mToken.mString.c_str() ); list->value().push_back( value );
              break;

            // boolean
            case SRF_Token::Boolean:
              value.setBool( mToken.mString == "true" ); list->value().push_back( value );
              break;

            // int
            case SRF_Token::Int32:
            case SRF_Token::Int64:
              value.setInt64( atoll(mToken.mString.c_str()) ); list->value().push_back( value );
              break;

            // float
            case SRF_Token::Float:
            case SRF_Token::Double:
              value.setDouble( atof(mToken.mString.c_str()) ); list->value().push_back( value );
              break;

          default:
            return false;
          }
        }
      }
      return false;
    }

    bool parseArray(ref<SRF_Array>& arr)
    {
      if(getToken(mToken))
      {
        // (1) from the fist token we decide what kind of array it is going to be
        // (2) empty arrays default to empty SRF_ArrayInt32

        if (mToken.mType == SRF_Token::RightRoundBracket)
        {
          arr = new SRF_ArrayInt32;
          return true;
        }
        else
        if (mToken.mType == SRF_Token::UID)
        {
          ref<SRF_ArrayUID> arr_uid;
          arr = arr_uid = new SRF_ArrayUID;
          do
            arr_uid->mValue.push_back(mToken.mString.c_str());
          while(getToken(mToken) && mToken.mType == SRF_Token::UID);
          return mToken.mType == SRF_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == SRF_Token::String)
        {
          ref<SRF_ArrayString> arr_string;
          arr = arr_string = new SRF_ArrayString;
          do 
            arr_string->mValue.push_back(mToken.mString);
          while(getToken(mToken) && mToken.mType == SRF_Token::String);
          return mToken.mType == SRF_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == SRF_Token::Identifier)
        {
          ref<SRF_ArrayIdentifier> arr_identifier;
          arr = arr_identifier = new SRF_ArrayIdentifier;
          do 
            arr_identifier->mValue.push_back(mToken.mString);
          while(getToken(mToken) && mToken.mType == SRF_Token::Identifier);
          return mToken.mType == SRF_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == SRF_Token::Int32)
        {
          ref<SRF_ArrayInt32> arr_int32;
          arr = arr_int32 = new SRF_ArrayInt32;
          do
            arr_int32->value().push_back( atoi( mToken.mString.c_str() ) );
          while(getToken(mToken) && mToken.mType == SRF_Token::Int32);
          return mToken.mType == SRF_Token::RightRoundBracket;
        }
        else
        if (mToken.mType == SRF_Token::Int64)
        {
          ref<SRF_ArrayInt64> arr_int64;
          arr = arr_int64 = new SRF_ArrayInt64;
          do
          {
            switch(mToken.mType)
            {
            case SRF_Token::Int32:
            case SRF_Token::Int64: arr_int64->value().push_back( atoll( mToken.mString.c_str() ) ); break;
            case SRF_Token::RightRoundBracket: return true;
            default:
              return false;
            }
          }
          while(getToken(mToken));
          return false;
        }
        else
        if (mToken.mType == SRF_Token::Float)
        {
          ref<SRF_ArrayFloat> arr_float;
          arr = arr_float = new SRF_ArrayFloat;
          do
          {
            switch(mToken.mType)
            {
            case SRF_Token::Int32:
            case SRF_Token::Int64:
            case SRF_Token::Float: arr_float->value().push_back( (float)atof( mToken.mString.c_str() ) ); break;
            case SRF_Token::RightRoundBracket: return true;
            default:
              return false;
            }
          }
          while(getToken(mToken));
          return false;
        }
        else
        if (mToken.mType == SRF_Token::Double)
        {
          ref<SRF_ArrayDouble> arr_double;
          arr = arr_double = new SRF_ArrayDouble;
          do
          {
            switch(mToken.mType)
            {
            case SRF_Token::Int32:
            case SRF_Token::Int64:
            case SRF_Token::Float: 
            case SRF_Token::Double: arr_double->mValue.push_back( atof( mToken.mString.c_str() ) ); break;
            case SRF_Token::RightRoundBracket: return true;
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
      while(getToken(mToken) && mToken.mType != SRF_Token::TOKEN_EOF)
      {
        switch(mToken.mType)
        {
          case SRF_Token::LeftRoundBracket:   printf("LeftSquareBracket (\n"); break;
          case SRF_Token::RightRoundBracket:  printf("RightSquareBracket )\n"); break;
          case SRF_Token::LeftSquareBracket:  printf("LeftSquareBracket [\n"); break;
          case SRF_Token::RightSquareBracket: printf("RightSquareBracket ]\n"); break;
          case SRF_Token::LeftCurlyBracket:   printf("LeftCurlyBracket {\n"); break;
          case SRF_Token::RightCurlyBracket:  printf("RightCurlyBracket } \n"); break;
          case SRF_Token::LeftFancyBracket:   printf("LeftFancyBracket >}\n"); break;
          case SRF_Token::RightFancyBracket:  printf("RightFancyBracket {< \n"); break;
          case SRF_Token::Equals:             printf("Equals =\n"); break;
          case SRF_Token::String:             printf("String = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::UID:                printf("UID = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::Identifier:         printf("Identifier = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::Float:              printf("Float = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::Double:             printf("Double = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::Int32:              printf("Int32 = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::Int64:              printf("Int64 = %s\n", mToken.mString.c_str()); break;
          case SRF_Token::StructureHeader:       printf("StructureHeader = %s\n", mToken.mString.c_str()); break;
          default:
            break;
        }
      }
      if (mToken.mType != SRF_Token::TOKEN_EOF)
      {
        printf("Line %d: syntax error : '%s'.\n", mTokenizer->lineNumber(), mToken.mString.c_str());
      }
    }

    SRF_Tokenizer* tokenizer() { return mTokenizer.get(); }

    const SRF_Tokenizer* tokenizer() const { return mTokenizer.get(); }

    const std::map< std::string, ref<SRF_Structure> >& linkMap() const { return mLinkMap; }

    const SRF_Structure* root() const { return mRoot.get(); }

  private:
    ref<SRF_Structure> mRoot;
    std::map< std::string, ref<SRF_Structure> > mLinkMap;
    ref<SRF_Tokenizer> mTokenizer;
    SRF_Token mToken;
  };
  //-----------------------------------------------------------------------------
}

#endif
