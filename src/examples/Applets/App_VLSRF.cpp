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

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/DrawPixels.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Geometry.hpp>
#include <vlCore/BufferedStream.hpp>
#include <vlCore/DiskFile.hpp>
#include <vlGraphics/MultiDrawElements.hpp>
#include <vlGraphics/TriangleStripGenerator.hpp>

using namespace vl;

#ifdef _MSC_VER
#define atoll _atoi64
#endif

typedef enum
{
  TT_ERROR,
  TT_EOF,

  TT_LeftRoundBracket, // (
  TT_RightRoundBracket, // )
  TT_LeftSquareBracket, // [
  TT_RightSquareBracket, // ]
  TT_LeftCurlyBracket, // {
  TT_RightCurlyBracket, // }
  TT_LeftFancyBracket, // {<
  TT_RightFancyBracket, // >}
  // TT_Column, // :
  TT_Equals, // =
  TT_String, // "ciccio \npippo]!!>" -> 'C' escape sequences are escaped
  TT_UID, // #Identifier_type001
  TT_Identifier, // Identifier_1001
  TT_Boolean, // true / false
  TT_Int32, // +123
  TT_Int64, // +123
  TT_Float, // +123.456e+10
  TT_Double, // +123.456e+10
  TT_ObjectHeader // <ObjectHeader>

} ETokenType;

class SRF_Token
{
public:
  SRF_Token(): mType(TT_ERROR) {}
  std::string mString;
  ETokenType mType;
};

class SRF_Tokenizer: public BufferedStream<char, 128*1024>
{
public:
  SRF_Tokenizer()
  {
    mLineNumber = 1;
  }

  bool getLine(std::string& str)
  {
    str.clear();
    str.reserve(16);
    char ch;
    while(readTextChar(ch))
    {
      // reached the end of the line
      if (ch == '\n')
      {
        ++mLineNumber;
        return true;
      }
      else
      // line continuation
      if (ch == '\\')
      {
        // eat everything till the end of the line
        while(readTextChar(ch) && ch != '\n')
        {
          if (ch != ' ' && ch != '\t')
          {
            Log::error( Say("Line %n: unexpected character '%c'.\n") << mLineNumber );
            return false;
          }
        }
        if (ch == '\n')
          ++mLineNumber;
      }
      else
        str.push_back(ch);
    }

    return true;
  }

  bool getToken(SRF_Token& token) 
  {
    token.mType = TT_ERROR;
    token.mString.clear();

    // read chars skipping spaces
    char ch1=0, ch2=0;
    do {
      if (!readTextChar(ch1))
      {
        token.mType = TT_EOF;
        return true;
      }

      if (ch1 == '\n')
        ++mLineNumber;
      else
      // eat comments
      if (ch1 == '/')
      {
        if(readTextChar(ch2))
        {
          if (ch2 == '/') // single line comment
          {
            // eat everything till the end of the line
            for(ch1 = 0; readTextChar(ch1) && ch1 != '\n'; )
            {
              // eat everything
            }
            if (ch1 == '\n')
              ++mLineNumber;
          }
          else
          if (ch2 == '*') // multi line comment
          {
            // eat everything till the end of the line
            while(readTextChar(ch1))
            {
              if (ch1 == '*' && readTextChar(ch2) && ch2 == '/')
              {
                ch1 = '\n'; // pretend it's a space to stay in the loop
                break;
              }
              // eat everything
              if (ch1 == '\n')
                ++mLineNumber;
            }
          }
          else
          {
            Log::error( Say("Line %n: unexpected character '%c' after '/'.\n") << mLineNumber << ch2 );
            return false;
          }
        }
        else
        {
          Log::error( Say("Line %n: unexpected end of file in comment.\n") << mLineNumber);
          return false;
        }
        continue;
      }

    } while(ch1 == ' ' || ch1 == '\t' || ch1 == '\n');

    switch(ch1)
    {
    case '(':
      token.mType = TT_LeftRoundBracket;
      token.mString = "(";
      return true;
    
    case ')':
      token.mType = TT_RightRoundBracket;
      token.mString = ")";
      return true;

    case '[':
      token.mType = TT_LeftSquareBracket;
      token.mString = "[";
      return true;
    
    case ']':
      token.mType = TT_RightSquareBracket;
      token.mString = "]";
      return true;

    case '{':
      if(readTextChar(ch2) && ch2 == '<')
      {
        token.mType = TT_LeftFancyBracket;
        token.mString = "{<";
        // eat spaces on the right, nothing must follow {<
        while(readTextChar(ch1))
        {
          if (ch1 == '\n')
          {
            ++mLineNumber;
            break;
          }

          if (ch1 != ' ' && ch1 != '\t')
          {
            Log::error( Say("Line %n: unexpected character '%c' after '{<'. A new line should follow instead.\n") << mLineNumber << ch1 );
            return false;
          }
        }
      }
      else
      {
        token.mType = TT_LeftCurlyBracket;
        token.mString = "{";
        if(!isEndOfFile())
          ungetToken(ch2);
      }
      return true;

    case '}':
      token.mType = TT_RightCurlyBracket;
      token.mString = "}";
      return true;

    case '>':
      if(readTextChar(ch2))
      {
        if(ch2 == '}')
        {
          token.mType = TT_RightFancyBracket;
          return true;
        }
        else
        {
          Log::error( Say("Line %n: expected '}' instead of '%c' after '>'.\n") << mLineNumber << ch2 );
          return false;
        }
      }
      else
      {
          Log::error( Say("Line %n: unexpected end of file.\n") << mLineNumber );
          return false;
      }

    case '=':
      token.mType = TT_Equals; 
      token.mString = "=";
      return true;

    /*case ':':
      token.mType = TT_Column; 
      return true;*/

    case '<':
      token.mType = TT_ObjectHeader;
      token.mString = "<";
      while(readTextChar(ch1) && ch1 != '>')
      {
        if (ch1 >= 'a' && ch1 <= 'z' || ch1 >= 'A' && ch1 <= 'Z' || ch1 >= '0' && ch1 <= '9' || ch1 == '_' )
          token.mString.push_back(ch1);
        else
        {
          Log::error( Say("Line %n: unexpected character '%c'.\n") << mLineNumber << ch1 );
          return false;
        }
      }
      token.mString.push_back('>');
      if (isEndOfFile())
      {
        Log::error( Say("Line %n: unexpected end of file while reading object header.\n") << mLineNumber );
        return false;
      }
      return true;

    case '#':
      token.mType = TT_UID;
      token.mString = "#";
      while(readTextChar(ch1))
      {
        if (ch1 >= 'a' && ch1 <= 'z' || ch1 >= 'A' && ch1 <= 'Z' || ch1 >= '0' && ch1 <= '9' || ch1 == '_' )
          token.mString.push_back(ch1);
        else
        {
          ungetToken(ch1);
          break;
        }
      }
      if (token.mString == "#_")
      {
        Log::error( Say("Line %n: illegal id '#_' found.\n") << mLineNumber );
        return false;
      }
      return true;

    case '"':
      token.mType = TT_String;
      while(readTextChar(ch1))
      {
        // end string
        if (ch1 == '"')
          break;
        else
        // return found before end of string
        if (ch1 == '\n')
        {
          Log::error( Say("Line %n: end of line found before end of string, did you forget a \"?.\n") << mLineNumber );
          return false;
        }
        else
        // escape sequences
        if (ch1 == '\\' && readTextChar(ch2))
        {
          if (ch2 == '"')
            ch1 = '"';
          else
          if (ch2 == '\\')
            ch1 = '\\';
          else
          if (ch2 == 'b')
            ch1 = '\b';
          else
          if (ch2 == 'f')
            ch1 = '\f';
          else
          if (ch2 == 'r')
            ch1 = '\r';
          else
          if (ch2 == 'n')
            ch1 = '\n';
          else
          if (ch2 == 't')
            ch1 = '\t';
          else
            ungetToken(ch2);
          token.mString.push_back(ch1);
        }
        else
        // accept everyhing else
          token.mString.push_back(ch1);
      }
      if (isEndOfFile())
      {
        Log::error( Say("Line %n: end of file found before end of string, did you forget a \"?.\n") << mLineNumber );
        return false;
      }
      else
        return true;

    default:
      // identifier
      if (ch1 >= 'a' && ch1 <= 'z' || ch1 >= 'A' && ch1 <= 'Z' || ch1 == '_' )
      {
        token.mType = TT_Identifier;
        token.mString.push_back(ch1);
        while(readTextChar(ch1))
        {
          if (ch1 >= 'a' && ch1 <= 'z' || ch1 >= 'A' && ch1 <= 'Z' || ch1 >= '0' && ch1 <= '9' || ch1 == '_' )
            token.mString.push_back(ch1);
          else
          {
            ungetToken(ch1);
            break;
          }
        }
        if (token.mString == "_")
        {
          Log::error( Say("Line %n: unexpected character '_'.\n") << mLineNumber );
          return false;
        }
        else
        {
          // check if it's a book
          if (token.mString == "true" || token.mString == "false")
            token.mType = TT_Boolean;
          return true;
        }
      }
      else
      // TT_Int32 / TT_Int64 / TT_Float / TT_Double
      //
      // ACCEPTED:
      // 123
      // +123.123E+10 -123.123e-10
      // +123
      // +.123
      // 0.123
      // 123.123
      //
      // REJECTED:
      // 01234
      // 01.234
      // 123.
      // 123.123E/e
      // 123.123E/e+
      if ( ch1 >= '0' && ch1 <= '9' || ch1 == '.' || ch1 == '+' || ch1 == '-' )
      {
        token.mType = TT_ERROR;
        token.mString.push_back(ch1);

        enum { sZERO, sPLUS_MINUS, sINT, sFRAC, sPOINT, sE, sPLUS_MINUS_EXP, sEXP } state = sINT;

        if ( ch1 >= '1' && ch1 <= '9' )
          state = sINT;
        else
        if (ch1 == '0')
          state = sZERO;
        else
        if (ch1 == '.')
          state = sPOINT;
        else
        if (ch1 == '+' || ch1 == '-')
          state = sPLUS_MINUS;

        // for simplicity we don't deal with the cases when we reach EOF after +/-/./E/E+-
        while(readTextChar(ch1))
        {
          switch(state)
          {
          // if starting with 0 must be 0.0-9
          case sZERO:
            if (ch1 == '.')
            {
              token.mString.push_back(ch1);
              state = sPOINT;
            }
            else
            {
              if (ch1 == 'L')
                token.mType = TT_Int64;
              else
              {
                token.mType = TT_Int32;
                ungetToken(ch1);
              }
              return true;
            }
            break;

          case sPLUS_MINUS:
            if (ch1 == '0')
            {
              token.mString.push_back(ch1);
              state = sZERO;
            }
            else
            if (ch1 >= '1' && ch1 <= '9')
            {
              token.mString.push_back(ch1);
              state = sINT;
            }
            else
            if (ch1 == '.')
            {
              token.mString.push_back(ch1);
              state = sPOINT;
            }
            else
            {
              Log::error( Say("Line %n:unexpected character '%c'.\n") << mLineNumber << ch1 );
              return false;
            }
            break;

          case sINT:
            if (ch1 >= '0' && ch1 <= '9')
              token.mString.push_back(ch1);
            else
            if (ch1 == '.')
            {
              token.mString.push_back(ch1);
              state = sPOINT;
            }
            else
            {
              if (ch1 == 'L')
                token.mType = TT_Int64;
              else
              {
                token.mType = TT_Int32;
                ungetToken(ch1);
              }
              return true;
            }
            break;

          case sPOINT:
            if (ch1 >= '0' && ch1 <= '9')
            {
              token.mString.push_back(ch1);
              state = sFRAC;
            }
            else
            {
              Log::error( Say("Line %n:unexpected character '%c'.\n") << mLineNumber << ch1 );
              return false;
            }
            break;

          case sFRAC:
            if (ch1 >= '0' && ch1 <= '9')
              token.mString.push_back(ch1);
            else
            if (ch1 == 'E' || ch1 == 'e')
            {
              token.mString.push_back(ch1);
              state = sE;
            }
            else
            {
              if (ch1 == 'L')
                token.mType = TT_Double;
              else
              {
                token.mType = TT_Float;
                ungetToken(ch1);
              }
              return true;
            }
            break;

          case sE:
            if (ch1 == '+' || ch1 == '-')
            {
              token.mString.push_back(ch1);
              state = sPLUS_MINUS_EXP;
            }
            else
            {
              Log::error( Say("Line %n:unexpected character '%c'.\n") << mLineNumber << ch1 );
              return false;
            }
            break;

          case sPLUS_MINUS_EXP:
            if (ch1 >= '0' && ch1 <= '9')
            {
              token.mString.push_back(ch1);
              state = sEXP;
            }
            else
            {
              Log::error( Say("Line %n:unexpected character '%c'.\n") << mLineNumber << ch1 );
              return false;
            }
            break;

          case sEXP:
            if (ch1 >= '0' && ch1 <= '9')
              token.mString.push_back(ch1);
            else
            {
              if (ch1 == 'L')
                token.mType = TT_Double;
              else
              {
                token.mType = TT_Float;
                ungetToken(ch1);
              }
              return true;
            }
            break;
          }
        }
        // reached EOF in the middle of the parsing so we check where we were, note that it cannot be a Int64 or a Double
        if (state == sINT)
        {
          token.mType = TT_Int32;
          return true;
        }
        else
        if (state == sFRAC || state == sEXP)
        {
          token.mType = TT_Float;
          return true;
        }
        else
          return false;
      }
      else
      {
        Log::error( Say("Line %n: unexpected character '%c'.\n") << mLineNumber << ch1 );
        return false;
      }
    }
  }

  // mic fixme
  int mLineNumber;
};

class SRF_Object;
class SRF_List;
class SRF_Array;
class SRF_ArrayString;
class SRF_ArrayIdentifier;
class SRF_ArrayUID;
class SRF_ArrayInt32;
class SRF_ArrayInt64;
class SRF_ArrayFloat;
class SRF_ArrayDouble;

class SRF_Visitor: public Object
{
public:
  virtual void visitObject(SRF_Object*) {}
  virtual void visitList(SRF_List*) {}
  virtual void visitArray(SRF_ArrayUID*) {}
  virtual void visitArray(SRF_ArrayInt32*) {}
  virtual void visitArray(SRF_ArrayInt64*) {}
  virtual void visitArray(SRF_ArrayFloat*) {}
  virtual void visitArray(SRF_ArrayDouble*) {}
  virtual void visitArray(SRF_ArrayString*) {}
  virtual void visitArray(SRF_ArrayIdentifier*) {}
};
//-----------------------------------------------------------------------------
// Arrays
//-----------------------------------------------------------------------------
class SRF_Array: public Object 
{ 
  VL_INSTRUMENT_CLASS(vl::SRF_Array, Object)

public:
};

class SRF_ArrayInt32: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayInt32, SRF_Array)

public:
  SRF_ArrayInt32() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<int> mValue;
};

class SRF_ArrayInt64: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayInt64, SRF_Array)

public:
  SRF_ArrayInt64() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<long long> mValue;
};

class SRF_ArrayFloat: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayFloat, SRF_Array)

public:
  SRF_ArrayFloat() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<float> mValue;
};

class SRF_ArrayDouble: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayDouble, SRF_Array)

public:
  SRF_ArrayDouble() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<double> mValue;
};

class SRF_ArrayUID: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayUID, SRF_Array)

public:
  SRF_ArrayUID() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  struct Value
  {
    Value(const std::string& uid): mUID(uid) {}
    std::string mUID; // the UID string
    ref<SRF_Object> mPtr; // the linked object
  };

  std::vector<Value> mValue;
};

class SRF_ArrayIdentifier: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayIdentifier, SRF_Array)

public:
  SRF_ArrayIdentifier() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<std::string> mValue;
};

class SRF_ArrayString: public SRF_Array
{
  VL_INSTRUMENT_CLASS(vl::SRF_ArrayString, SRF_Array)

public:
  SRF_ArrayString() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  // mic fixme: metti sti mValue nascosti e ritorna const char*
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
    Object,
    ArrayString,
    ArrayIdentifier,
    ArrayUID,
    ArrayInt32,
    ArrayInt64,
    ArrayFloat,
    ArrayDouble
  };

private:
  union
  {
    bool mBool;
    long long mInt64;
    double mDouble;
    const char* mString;
    SRF_Object* mObject;
    SRF_List* mList;
    SRF_Array* mArray;
  } mUnion;

private:
  void release();

private:
  EType mType;

public:
  SRF_Value()
  {
    mType = Int64;
    mUnion.mInt64 = 0;
  }

  ~SRF_Value() { release(); }

  SRF_Value(const SRF_Value& other)
  {
    mType = Int64;
    mUnion.mInt64 = 0;
    *this = other;
  }

  SRF_Value& operator=(const SRF_Value& other);

  EType type() const { return mType; }

  // object

  void setObject(SRF_Object* obj);

  SRF_Object* getObject() { VL_CHECK(mType == Object); return mUnion.mObject; }

  const SRF_Object* getObject() const { VL_CHECK(mType == Object); return mUnion.mObject; }

  // list

  void setList(SRF_List* list);

  SRF_List* getList() { VL_CHECK(mType == List); return mUnion.mList; }

  const SRF_List* getList() const { VL_CHECK(mType == List); return mUnion.mList; }

  // array

  void setArray(SRF_Array*);

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

  void setString(const char* str)
  {
    release();
    mType = String;
    mUnion.mString = _strdup(str);
  }

  const char* getString() const { VL_CHECK(mType == String); return mUnion.mString; }

  void setIdentifier(const char* str)
  {
    release();
    mType = Identifier;
    mUnion.mString = _strdup(str);
  }

  const char* getIdentifier() const { VL_CHECK(mType == Identifier); return mUnion.mString; }

  void setUID(const char* str)
  {
    release();
    mType = UID;
    mUnion.mString = _strdup(str);
  }

  const char* getUID() const { VL_CHECK(mType == UID); return mUnion.mString; }

  void setInt64(long long val)
  {
    release();
    mType = Int64;
    mUnion.mInt64 = val;
  }

  long long getInt64() const { VL_CHECK(mType == Int64); return mUnion.mInt64; }

  void setDouble(double val)
  {
    release();
    mType = Double;
    mUnion.mDouble = val;
  }

  double getDouble() const { VL_CHECK(mType == Double); return mUnion.mDouble; }

  void setBool(bool val)
  {
    release();
    mType = Bool;
    mUnion.mBool = val;
  }

  double getBool() const { VL_CHECK(mType == Bool); return mUnion.mBool; }
};
//-----------------------------------------------------------------------------
// SRF_Object
//-----------------------------------------------------------------------------
class SRF_Object: public Object
{
  VL_INSTRUMENT_CLASS(vl::SRF_Object, Object)

public:
  SRF_Object()
  {
    mKeyValue.reserve(10);
    mUID = "#NULL";
  }

  virtual void acceptVisitor(SRF_Visitor* v) { v->visitObject(this); }

  struct Value
  {
    std::string mKey;
    SRF_Value mValue;
  };

  std::string mName;
  std::string mUID;
  std::vector<Value> mKeyValue;
  // mic fixme: add a multimap for quick access
};
//-----------------------------------------------------------------------------
// SRF_List
//-----------------------------------------------------------------------------
class SRF_List: public Object
{
  VL_INSTRUMENT_CLASS(vl::SRF_List, Object)

public:
  SRF_List()
  {
    mValue.reserve(16);
  }

  virtual void acceptVisitor(SRF_Visitor* v) { v->visitList(this); }

  std::vector< SRF_Value > mValue;
};
//-----------------------------------------------------------------------------
void SRF_Value::release()
{
  switch(mType)
  {
  case Object: 
    mUnion.mObject->decReference(); 
    break;

  case List:
    mUnion.mList->decReference(); 
    break;

  case ArrayString:
  case ArrayUID:
  case ArrayIdentifier:
  case ArrayInt32:
  case ArrayInt64:
  case ArrayFloat:
  case ArrayDouble:
    mUnion.mArray->decReference(); 
    break;

  case String:
  case UID:
  case Identifier:
    VL_CHECK(mUnion.mString)
    free((void*)mUnion.mString);
    mUnion.mString = NULL; break;
    break;

  default:
    break;
  }

  mType = Int64;
  mUnion.mInt64 = 0;
}
//-----------------------------------------------------------------------------
SRF_Value& SRF_Value::operator=(const SRF_Value& other)
{
  // must be done first
  switch(other.mType)
  {
  case Object:
    other.mUnion.mObject->incReference(); 
    break;

  case List:
    other.mUnion.mList->incReference(); 
    break;

  case ArrayString:
  case ArrayUID:
  case ArrayIdentifier:
  case ArrayInt32:
  case ArrayInt64:
  case ArrayFloat:
  case ArrayDouble:
    other.mUnion.mArray->incReference(); 
    break;

  default:
    break;
  }

  // must be done after
  release();

  mUnion = other.mUnion;
  mType = mType;

  // make local copy of the string
  if (mType == String || mType == Identifier || mType == UID)
    mUnion.mString = _strdup(mUnion.mString);

  return *this;
}
//-----------------------------------------------------------------------------
void SRF_Value::setObject(SRF_Object* obj)
{
  release();
  mType = Object;
  mUnion.mObject = obj;
  mUnion.mObject->incReference();
}
//-----------------------------------------------------------------------------
void SRF_Value::setList(SRF_List* list)
{
  release();
  mType = List;
  mUnion.mList = list;
  mUnion.mList->incReference();
}
//-----------------------------------------------------------------------------
void SRF_Value::setArray(SRF_Array* arr)
{
  release();

  if(arr->classType() == SRF_ArrayString::Type())
    mType = ArrayString;
  else
  if(arr->classType() == SRF_ArrayIdentifier::Type())
    mType = ArrayIdentifier;
  else
  if(arr->classType() == SRF_ArrayUID::Type())
    mType = ArrayUID;
  else
  if(arr->classType() == SRF_ArrayInt32::Type())
    mType = ArrayInt32;
  else
  if(arr->classType() == SRF_ArrayInt64::Type())
    mType = ArrayInt64;
  else
  if(arr->classType() == SRF_ArrayFloat::Type())
    mType = ArrayFloat;
  else
  if(arr->classType() == SRF_ArrayDouble::Type())
    mType = ArrayDouble;
  else
  {
    VL_TRAP();
    mUnion.mArray = NULL;
  }

  mUnion.mArray = arr;
  mUnion.mArray->incReference();
}
//-----------------------------------------------------------------------------
// SRF_DumpVisitor
//-----------------------------------------------------------------------------
class SRF_DumpVisitor: public SRF_Visitor
{
public:
  SRF_DumpVisitor()
  {
    mIndent = 0;
    mAssign = false;
  }

  void indent()
  {
    if (mAssign)
      mAssign = false;
    else
    {
      for(int i=0; i<mIndent; ++i)
        mDump += "  ";
    }
  }

  virtual void visitObject(SRF_Object* obj)
  {
    indent(); mDump += String::printf("%s\n", obj->mName.c_str());
    indent(); mDump += "{\n";
    mIndent++;
    if (obj->mUID.length() && obj->mUID != "#NULL")
    {
      indent(); mDump += String::printf("ID = %s\n", obj->mUID.c_str());
    }
    for(size_t i=0; i<obj->mKeyValue.size(); ++i)
    {
      indent(); mDump += String::printf("%s = ", obj->mKeyValue[i].mKey.c_str());
      switch(obj->mKeyValue[i].mValue.type())
      {
      case SRF_Value::Object:
        mAssign = true;
        obj->mKeyValue[i].mValue.getObject()->acceptVisitor(this);
        break;
      case SRF_Value::List:
        mAssign = true;
        obj->mKeyValue[i].mValue.getList()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayString:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayString()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayIdentifier:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayIdentifier()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayUID:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayUID()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayInt32:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayInt32()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayInt64:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayInt64()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayFloat:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayFloat()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayDouble:
        mAssign = true;
        obj->mKeyValue[i].mValue.getArrayDouble()->acceptVisitor(this);
        break;
      case SRF_Value::String:
        mDump += String::printf("%s\n", obj->mKeyValue[i].mValue.getString());
        break;
      case SRF_Value::Identifier:
        mDump += String::printf("%s\n", obj->mKeyValue[i].mValue.getIdentifier());
        break;
      case SRF_Value::UID:
        mDump += String::printf("%s\n", obj->mKeyValue[i].mValue.getUID());
        break;
      case SRF_Value::Bool:
        mDump += String::printf("%s\n", obj->mKeyValue[i].mValue.getBool() ? "true" : "false");
        break;
      case SRF_Value::Int64:
        mDump += String::printf("%lld\n", obj->mKeyValue[i].mValue.getInt64());
        break;
      case SRF_Value::Double:
        mDump += String::printf("%Lf\n", obj->mKeyValue[i].mValue.getDouble());
        break;
      }
    }
    mIndent--;
    indent(); mDump += "}\n";
  }

  virtual void visitList(SRF_List* list)
  {
    indent(); mDump += "[\n";
    mIndent++;
    for(size_t i=0; i<list->mValue.size(); ++i)
    {
      switch(list->mValue[i].type())
      {
      case SRF_Value::Object:
        list->mValue[i].getObject()->acceptVisitor(this);
        break;
      case SRF_Value::List:
        list->mValue[i].getList()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayString:
        list->mValue[i].getArrayString()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayIdentifier:
        list->mValue[i].getArrayIdentifier()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayUID:
        list->mValue[i].getArrayUID()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayInt32:
        list->mValue[i].getArrayInt32()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayInt64:
        list->mValue[i].getArrayInt64()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayFloat:
        list->mValue[i].getArrayFloat()->acceptVisitor(this);
        break;
      case SRF_Value::ArrayDouble:
        list->mValue[i].getArrayDouble()->acceptVisitor(this);
        break;
      case SRF_Value::String:
        mDump += String::printf("%s\n", list->mValue[i].getString());
        break;
      case SRF_Value::Identifier:
        mDump += String::printf("%s\n", list->mValue[i].getIdentifier());
        break;
      case SRF_Value::UID:
        mDump += String::printf("%s\n", list->mValue[i].getUID());
        break;
      case SRF_Value::Bool:
        mDump += String::printf("%s\n", list->mValue[i].getBool() ? "true" : "false");
        break;
      case SRF_Value::Int64:
        mDump += String::printf("%lld\n", list->mValue[i].getInt64());
        break;
      case SRF_Value::Double:
        mDump += String::printf("%Lf\n", list->mValue[i].getDouble());
        break;
      }
    }
    mIndent--;
    indent(); mDump += "]\n";
  }

  virtual void visitArray(SRF_ArrayInt32* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += String::printf("%d ", arr->mValue[i]);
    mDump += ")\n";
  }

  virtual void visitArray(SRF_ArrayInt64* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += String::printf("%lld ", arr->mValue[i]);
    mDump += ")\n";
  }

  virtual void visitArray(SRF_ArrayFloat* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += String::printf("%f ", arr->mValue[i]);
    mDump += ")\n";
  }

  virtual void visitArray(SRF_ArrayDouble* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += String::printf("%Lf ", arr->mValue[i]);
    mDump += ")\n";
  }

  virtual void visitArray(SRF_ArrayIdentifier* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += String::printf("%s ", arr->mValue[i].c_str());
    mDump += ")\n";
  }

  virtual void visitArray(SRF_ArrayString* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += "\"" + encodeString(arr->mValue[i]) + "\" ";
    mDump += ")\n";
  }

  virtual void visitArray(SRF_ArrayUID* arr)
  {
    indent(); mDump += "( ";
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      mDump += String::printf("%s ", arr->mValue[i].mUID.c_str());
    mDump += ")\n";
  }

  // mic fixme: test this both as input and as output!
  // support \xHH hex notation both input and output.
  String encodeString(const std::string& str)
  {
    String out;
    for(size_t i=0; i<str.length(); ++i)
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

  const String& dumpString() const { return mDump; }

  String& dumpString() { return mDump; }

  void setDumpString(const String& str) { mDump = str; }

private:
  int mIndent;
  bool mAssign;
  String mDump;
};
//-----------------------------------------------------------------------------
// SRF_DumpVisitor
//-----------------------------------------------------------------------------
class SRF_LinkVisitor: public SRF_Visitor
{
public:
  SRF_LinkVisitor(const std::map< std::string, ref<SRF_Object> >* map)
  {
    mLinkMap = map;
  }

  void setLinkMap(const std::map< std::string, ref<SRF_Object> >* map)
  {
    mLinkMap = map;
  }

  SRF_Object* link(const std::string& uid)
  {
    VL_CHECK(mLinkMap)
      VL_CHECK(!uid.empty())
    std::map< std::string, ref<SRF_Object> >::const_iterator it = mLinkMap->find(uid);
    if( it != mLinkMap->end() )
    {
      printf("linked UID = '%s'.\n", uid.c_str());
      return it->second.get_writable();
    }
    else
    {
      if (uid != "#NULL")
        Log::error( Say("Could not link UID = '%s'.\n") << uid );
      return NULL;
    }
  }

  virtual void visitObject(SRF_Object* obj)
  {
    for(size_t i=0; i<obj->mKeyValue.size(); ++i)
    {
      if (obj->mKeyValue[i].mValue.type() == SRF_Value::Object)
        obj->mKeyValue[i].mValue.getObject()->acceptVisitor(this);
      else
      if (obj->mKeyValue[i].mValue.type() == SRF_Value::List)
        obj->mKeyValue[i].mValue.getList()->acceptVisitor(this);
      else
      if (obj->mKeyValue[i].mValue.type() == SRF_Value::ArrayUID)
        obj->mKeyValue[i].mValue.getArrayUID()->acceptVisitor(this);
      else
      if (obj->mKeyValue[i].mValue.type() == SRF_Value::UID)
      {
        SRF_Object* ptr = link( obj->mKeyValue[i].mValue.getUID() );
        obj->mKeyValue[i].mValue.setObject( ptr );
      }
    }
  }

  virtual void visitList(SRF_List* list)
  {
    for(size_t i=0; i<list->mValue.size(); ++i)
    {
      if (list->mValue[i].type() == SRF_Value::Object)
        list->mValue[i].getObject()->acceptVisitor(this);
      else
      if (list->mValue[i].type() == SRF_Value::List)
        list->mValue[i].getList()->acceptVisitor(this);
      else
      if (list->mValue[i].type() == SRF_Value::ArrayUID)
        list->mValue[i].getArrayUID()->acceptVisitor(this);
      else
      if (list->mValue[i].type() == SRF_Value::UID)
      {
        SRF_Object* obj = link( list->mValue[i].getUID() );
        list->mValue[i].setObject( obj );
      }
    }
  }

  virtual void visitArray(SRF_ArrayUID* arr)
  {
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      arr->mValue[i].mPtr = link(arr->mValue[i].mUID);
  }

  virtual void visitArray(SRF_ArrayIdentifier*) {}

  virtual void visitArray(SRF_ArrayString*)  {}

  virtual void visitArray(SRF_ArrayInt32*)  {}

  virtual void visitArray(SRF_ArrayInt64*)  {}

  virtual void visitArray(SRF_ArrayFloat*)  {}

  virtual void visitArray(SRF_ArrayDouble*)  {}

private:
  const std::map< std::string, ref<SRF_Object> >* mLinkMap;
};
//-----------------------------------------------------------------------------
// SRF_Parser
//-----------------------------------------------------------------------------
class SRF_Parser
{
public:

  bool getToken(SRF_Token& token) { return mTokenizer->getToken(token); }

  bool parse()
  {
    mRoot = NULL;
    if(getToken(mToken) && mToken.mType == TT_ObjectHeader)
    {
      mRoot = new SRF_Object;
      mRoot->mName = mToken.mString;
      if (parseObject(mRoot.get()))
      {
        return true;
      }
      else
      {
        mRoot = NULL;
        if (mToken.mString.length())
          Log::error( Say("Line %n: parse error at '%s'.\n") << mTokenizer->mLineNumber << mToken.mString.c_str() );
        else
          Log::error( Say("Line %n: parse error.\n") << mTokenizer->mLineNumber );
        return false;
      }
    }
    else
    {
      Log::error("No root object found!\n");
      return false;
    }
  }

  bool parseObject(SRF_Object* object)
  {
    if (!getToken(mToken) || mToken.mType != TT_LeftCurlyBracket)
      return false;
    while(getToken(mToken))
    {
      if (mToken.mType == TT_RightCurlyBracket)
      {
        return true;
      }
      else
      if (mToken.mType == TT_Identifier)
      {
        // ID field requires a proper #identifier
        if (mToken.mString.length() == 2)
        {
          if (mToken.mString == "ID")
          {
            // Check if ID has already been set
            if (!object->mUID.empty() && object->mUID != "#NULL")
            {
              Log::error("ID already set.\n");
              return false;
            }

            // Equals
            if (!getToken(mToken) || mToken.mType != TT_Equals)
              return false;

            // #identifier
            if (getToken(mToken) && mToken.mType == TT_UID)
            {
              object->mUID = mToken.mString;

              // UID to Object Map, #NULL is not mapped to anything
              if( object->mUID != "#NULL")
              {
                if (mUID_To_Object_Map.find(object->mUID) == mUID_To_Object_Map.end())
                  mUID_To_Object_Map[ object->mUID ] = object;
                else
                {
                  Log::error( Say("Duplicate UID = '%s'.\n") << object->mUID );
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
        object->mKeyValue.push_back( SRF_Object::Value() );
        SRF_Object::Value& name_value = object->mKeyValue.back();

        // Key
        name_value.mKey = mToken.mString;

        // Equals
        if (!getToken(mToken) || mToken.mType != TT_Equals)
          return false;

        // Member value
        if (getToken(mToken))
        {
          // A new <Object>
          if (mToken.mType == TT_ObjectHeader)
          {
            ref<SRF_Object> object = new SRF_Object;
            object->mName = mToken.mString;
            name_value.mValue.setObject(object.get());
            if (!parseObject( object.get() ) )
              return false;
          }
          else
          // An [ list ]
          if (mToken.mType == TT_LeftSquareBracket)
          {
            ref<SRF_List> list = new SRF_List;
            name_value.mValue.setList(list.get());
            if ( !parseList( list.get() ) )
              return false;
          }
          else
          // An ( array )
          if (mToken.mType == TT_LeftRoundBracket)
          {
            ref<SRF_Array> arr;
            if ( parseArray( arr ) )
              name_value.mValue.setArray(arr.get());
            else
              return false;
          }
          else
          // A "string"
          if (mToken.mType == TT_String)
          {
            name_value.mValue.setString(mToken.mString.c_str());
          }
          else
          // An Identifier
          if (mToken.mType == TT_Identifier)
          {
            name_value.mValue.setIdentifier(mToken.mString.c_str());
          }
          else
          // An #id
          if (mToken.mType == TT_UID)
          {
            name_value.mValue.setUID(mToken.mString.c_str());
          }
          else
          // A boolean (true/false)
          if (mToken.mType == TT_Boolean)
          {
            name_value.mValue.setBool(mToken.mString == "true");
          }
          else
          // An integer
          if (mToken.mType == TT_Int32 || mToken.mType == TT_Int64)
          {
            name_value.mValue.setInt64( atoll(mToken.mString.c_str()) );
          }
          else
          // An float
          if (mToken.mType == TT_Float || mToken.mType == TT_Double)
          {
            name_value.mValue.setDouble( atof(mToken.mString.c_str()) );
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
      if (mToken.mType == TT_RightSquareBracket)
        return true;
      else
      {
        SRF_Value value;
        switch( mToken.mType )
        {
          // object
          case TT_ObjectHeader:
            {
              ref<SRF_Object> object = new SRF_Object;
              object->mName = mToken.mString;
              if ( parseObject( object.get() ) )
              {
                value.setObject(object.get());
                list->mValue.push_back( value );
              }
              else
                return false;
              break;
            }

          // list
          case TT_LeftSquareBracket:
            {
              ref<SRF_List> sub_list = new SRF_List;
              if ( parseList( sub_list.get() ) )
              {
                value.setList( sub_list.get() );
                list->mValue.push_back( value );
              }
              else
                return false;
              break;
            }

          // array
          case TT_LeftRoundBracket:
            {
              ref<SRF_Array> arr;
              if (parseArray(arr))
              {
                value.setArray(arr.get());
                list->mValue.push_back(value);
              }
              else
                return false;
              break;
            }

          // string
          case TT_String:
            value.setString( mToken.mString.c_str() ); list->mValue.push_back( value );
            break;

          // identifier
          case TT_Identifier:
            value.setIdentifier( mToken.mString.c_str() ); list->mValue.push_back( value );
            break;

          // UID
          case TT_UID:
            value.setUID( mToken.mString.c_str() ); list->mValue.push_back( value );
            break;

          // boolean
          case TT_Boolean:
            value.setBool( mToken.mString == "true" ); list->mValue.push_back( value );
            break;

          // int
          case TT_Int32:
          case TT_Int64:
            value.setInt64( atoll(mToken.mString.c_str()) ); list->mValue.push_back( value );
            break;

          // float
          case TT_Float:
          case TT_Double:
            value.setDouble( atof(mToken.mString.c_str()) ); list->mValue.push_back( value );
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
      // from the fist token we decide what kind of array it is going to be
      if (mToken.mType == TT_UID)
      {
        ref<SRF_ArrayUID> arr_uid;
        arr = arr_uid = new SRF_ArrayUID;
        do
          arr_uid->mValue.push_back(mToken.mString);
        while(getToken(mToken) && mToken.mType == TT_UID);
        return mToken.mType == TT_RightRoundBracket;
      }
      else
      if (mToken.mType == TT_String)
      {
        ref<SRF_ArrayString> arr_string;
        arr = arr_string = new SRF_ArrayString;
        do 
          arr_string->mValue.push_back(mToken.mString);
        while(getToken(mToken) && mToken.mType == TT_String);
        return mToken.mType == TT_RightRoundBracket;
      }
      else
      if (mToken.mType == TT_Identifier)
      {
        ref<SRF_ArrayIdentifier> arr_identifier;
        arr = arr_identifier = new SRF_ArrayIdentifier;
        do 
          arr_identifier->mValue.push_back(mToken.mString);
        while(getToken(mToken) && mToken.mType == TT_Identifier);
        return mToken.mType == TT_RightRoundBracket;
      }
      else
      if (mToken.mType == TT_Int32)
      {
        ref<SRF_ArrayInt32> arr_int32;
        arr = arr_int32 = new SRF_ArrayInt32;
        do
          arr_int32->mValue.push_back( atoi( mToken.mString.c_str() ) );
        while(getToken(mToken) && mToken.mType == TT_Int32);
        return mToken.mType == TT_RightRoundBracket;
      }
      else
      if (mToken.mType == TT_Int64)
      {
        ref<SRF_ArrayInt64> arr_int64;
        arr = arr_int64 = new SRF_ArrayInt64;
        do
        {
          switch(mToken.mType)
          {
          case TT_Int32:
          case TT_Int64: arr_int64->mValue.push_back( atoll( mToken.mString.c_str() ) ); break;
          case TT_RightRoundBracket: return true;
          default:
            return false;
          }
        }
        while(getToken(mToken));
        return false;
      }
      else
      if (mToken.mType == TT_Float)
      {
        ref<SRF_ArrayFloat> arr_float;
        arr = arr_float = new SRF_ArrayFloat;
        do
        {
          switch(mToken.mType)
          {
          case TT_Int32:
          case TT_Int64:
          case TT_Float: arr_float->mValue.push_back( (float)atof( mToken.mString.c_str() ) ); break;
          case TT_RightRoundBracket: return true;
          default:
            return false;
          }
        }
        while(getToken(mToken));
        return false;
      }
      else
      if (mToken.mType == TT_Double)
      {
        ref<SRF_ArrayDouble> arr_double;
        arr = arr_double = new SRF_ArrayDouble;
        do
        {
          switch(mToken.mType)
          {
          case TT_Int32:
          case TT_Int64:
          case TT_Float: 
          case TT_Double: arr_double->mValue.push_back( atof( mToken.mString.c_str() ) ); break;
          case TT_RightRoundBracket: return true;
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
    while(getToken(mToken) && mToken.mType != TT_EOF)
    {
      switch(mToken.mType)
      {
        case TT_LeftRoundBracket:   printf("TT_LeftSquareBracket (\n"); break;
        case TT_RightRoundBracket:  printf("TT_RightSquareBracket )\n"); break;
        case TT_LeftSquareBracket:  printf("TT_LeftSquareBracket [\n"); break;
        case TT_RightSquareBracket: printf("TT_RightSquareBracket ]\n"); break;
        case TT_LeftCurlyBracket:   printf("TT_LeftCurlyBracket {\n"); break;
        case TT_RightCurlyBracket:  printf("TT_RightCurlyBracket } \n"); break;
        case TT_LeftFancyBracket:   printf("TT_LeftFancyBracket >}\n"); break;
        case TT_RightFancyBracket:  printf("TT_RightFancyBracket {< \n"); break;
        // case TT_Column:             printf("TT_Column :\n"); break;
        case TT_Equals:             printf("TT_Equals =\n"); break;
        case TT_String:             printf("TT_String = %s\n", mToken.mString.c_str()); break;
        case TT_UID:                printf("TT_UID = %s\n", mToken.mString.c_str()); break;
        case TT_Identifier:         printf("TT_Identifier = %s\n", mToken.mString.c_str()); break;
        case TT_Float:              printf("TT_Float = %s\n", mToken.mString.c_str()); break;
        case TT_Double:             printf("TT_Double = %s\n", mToken.mString.c_str()); break;
        case TT_Int32:              printf("TT_Int32 = %s\n", mToken.mString.c_str()); break;
        case TT_Int64:              printf("TT_Int64 = %s\n", mToken.mString.c_str()); break;
        case TT_ObjectHeader:       printf("TT_ObjectHeader = %s\n", mToken.mString.c_str()); break;
        default:
          break;
      }
    }
    if (mToken.mType != TT_EOF)
    {
      printf("Line %d: syntax error : '%s'.\n", mTokenizer->mLineNumber, mToken.mString.c_str());
    }
  }

  String dump()
  {
    if (mRoot)
    {
      SRF_DumpVisitor dump_visitor;
      mRoot->acceptVisitor(&dump_visitor);
      return dump_visitor.dumpString();
    }
    else
      return "";
  }

  void link()
  {
    if (mRoot)
    {
      SRF_LinkVisitor linker(&mUID_To_Object_Map);
      mRoot->acceptVisitor(&linker);
    }
  }

  ref<SRF_Object> mRoot;
  std::map< std::string, ref<SRF_Object> > mUID_To_Object_Map;
  ref<SRF_Tokenizer> mTokenizer;
  SRF_Token mToken;
};

class App_VLSRF: public BaseDemo
{
public:
  void srfPrelink_Geometry(Geometry* geom)
  {
    // --- generate UIDs ---
    // geometry itself
    generateUID(geom, "geometry_");
    // vertex arrays
    generateUID(geom->vertexArray(), "vertex_array_");
    generateUID(geom->normalArray(), "normal_array");
    generateUID(geom->colorArray(),  "color_array");
    generateUID(geom->secondaryColorArray(), "secondary_color_array");
    generateUID(geom->fogCoordArray(), "fogcoord_array");
    for(size_t i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      generateUID(geom->texCoordArray(i), "texcoord_array_");
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
        generateUID(geom->vertexAttribArray(i)->data(), "vertexattrib_array_");
    // draw elements
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      generateUID(geom->drawCalls()->at(i), "drawcall_");
      generateUID(geom->drawCalls()->at(i)->patchParameter(), "patchparam_");
    }
  }

  const char* getBoolCR(bool ok)
  {
    return ok ? "true\n" : "false\n";
  }

  void srfExport_Renderable(Renderable* ren)
  {
    if (mObject_Ref_Count[ren] > 1)
      mSRFString += indent() + "ID = " + getUID(ren) + "\n";
    mSRFString += indent() + "VBOEnabled = " + getBoolCR(ren->isVBOEnabled());
    mSRFString += indent() + "DisplayListEnabled = " + getBoolCR(ren->isDisplayListEnabled());
    mSRFString += indent() + "AABB = "; mAssign = true; srfExport_AABB(ren->boundingBox());
    mSRFString += indent() + "Sphere= "; mAssign = true; srfExport_Sphere(ren->boundingSphere());
  }

  void srfExport_AABB(const AABB& aabb)
  {
    // AABBs are always inlined
    mSRFString += indent() + "<AABB>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + String::printf("Min = ( %f %f %f )\n", aabb.minCorner().x(), aabb.minCorner().y(), aabb.minCorner().z() );
    mSRFString += indent() + String::printf("Max = ( %f %f %f )\n", aabb.maxCorner().x(), aabb.maxCorner().y(), aabb.maxCorner().z() );
    --mIndent;
    mSRFString += indent() + "}\n"; 
  }

  void srfExport_Sphere(const Sphere& sphere)
  {
    // AABBs are always inlined
    mSRFString += indent() + "<Sphere>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    mSRFString += indent() + String::printf("Center = ( %f %f %f )\n", sphere.center().x(), sphere.center().y(), sphere.center().z() );
    mSRFString += indent() + String::printf("Radius = %f\n", sphere.radius() );
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_Geometry(Geometry* geom)
  {
    if (isDefined(geom))
    {
      mSRFString += indent() + getUID(geom) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(geom);

    mSRFString += indent() + "<Geometry>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    srfExport_Renderable(geom);
    // vertex arrays
    if (geom->vertexArray()) { mSRFString += indent() + "VertexArray = ";  mAssign = true; srfExport_Array(geom->vertexArray()); }
    if (geom->normalArray()) { mSRFString += indent() + "NormalArray = ";  mAssign = true; srfExport_Array(geom->normalArray()); }
    if (geom->colorArray()) { mSRFString += indent() + "ColorArray = ";  mAssign = true; srfExport_Array(geom->colorArray()); }
    if (geom->secondaryColorArray()) { mSRFString += indent() + "SecondaryArray = ";  mAssign = true; srfExport_Array(geom->secondaryColorArray()); }
    if (geom->fogCoordArray()) { mSRFString += indent() + "FogCoordArray = ";  mAssign = true; srfExport_Array(geom->fogCoordArray()); }
    for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      if (geom->texCoordArray(i)) { mSRFString += indent() + String::printf("TexCoordArray%d = ", i);  mAssign = true; srfExport_Array(geom->texCoordArray(i)); }
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
      {
        if (geom->vertexAttribArray(i)) { mSRFString += indent() + String::printf("VertexAttribArray%d = ",i); mAssign = true; srfExport_VertexAttribInfo(geom->vertexAttribArray(i)); }
      }
      // draw calls
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      mSRFString += indent() + "DrawCall = ";  mAssign = true; srfExport_DrawCall(geom->drawCalls()->at(i));
    }
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_VertexAttribInfo(VertexAttribInfo* info)
  {
    if (isDefined(info))
    {
      mSRFString += indent() + getUID(info) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(info);

      mSRFString += indent() + "<VertexAttribInfo>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      if (mObject_Ref_Count[info] > 1)
        mSRFString += indent() + "ID = " + getUID(info) + "\n";
      // mSRFString += indent() + String::printf("AttribLocation = %d\n", info->attribLocation());
      mSRFString += indent() + "Data = "; mAssign = true; srfExport_Array(info->data());
      mSRFString += indent() + "Normalize = " + (info->normalize() ? "true\n" : "false\n");
      mSRFString += indent() + "Interpretation = ";
      switch(info->interpretation())
      {
      case VAI_NORMAL: mSRFString += "VAI_NORMAL\n"; break;
      case VAI_INTEGER: mSRFString += "VAI_INTEGER\n"; break;
      case VAI_DOUBLE: mSRFString += "VAI_DOUBLE\n"; break;
      }
      --mIndent;
      mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array1(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 40)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i) );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array2(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 30)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array3(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 20)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y(), arr->at(i).z() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  template<typename T_Array>
  void srfExport_Array4(ArrayAbstract* arr_abstract, const char* name,const char* format, int elems_per_line = 10)
  {
    T_Array* arr = arr_abstract->as<T_Array>();
    mSRFString += indent() + name + "\n";
    mSRFString += indent() + "{\n";
    if (mObject_Ref_Count[arr] > 1)
      mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
    if (arr->size()) // allow empty arrays this way
    {
      mSRFString += indent() + "\tValue = (\n\t\t" + indent();
      for(size_t i=0; i<arr->size(); ++i)
      {
        mSRFString += String::printf(format, arr->at(i).x(), arr->at(i).y(), arr->at(i).z(), arr->at(i).w() );
        if (i && (i % elems_per_line == 0) && i != arr->size()-1)
          mSRFString += "\n\t\t" + indent();
      }
      mSRFString += "\n" + indent() + "\t)\n";
    }
    mSRFString += indent() + "}\n";
  }

  void srfExport_Array(ArrayAbstract* arr_abstract)
  {
    if (isDefined(arr_abstract))
    {
      mSRFString += indent() + getUID(arr_abstract) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(arr_abstract);

    if(arr_abstract->as<ArrayUInt1>())
      srfExport_Array1<ArrayUInt1>(arr_abstract, "<ArrayUInt1>", "%u "); // mic fixme: these can actually be user specified
    else
    if(arr_abstract->as<ArrayUInt2>())
      srfExport_Array2<ArrayUInt2>(arr_abstract, "<ArrayUInt2>", "%u %u ");
    else
    if(arr_abstract->as<ArrayUInt3>())
      srfExport_Array3<ArrayUInt3>(arr_abstract, "<ArrayUInt3>", "%u %u %u ");
    else
    if(arr_abstract->as<ArrayUInt4>())
      srfExport_Array4<ArrayUInt4>(arr_abstract, "<ArrayUInt4>", "%u %u %u %u ");
    else

    if(arr_abstract->as<ArrayInt1>())
      srfExport_Array1<ArrayInt1>(arr_abstract, "<ArrayInt1>", "%d ");
    else
    if(arr_abstract->as<ArrayInt2>())
      srfExport_Array2<ArrayInt2>(arr_abstract, "<ArrayInt2>", "%d %d ");
    else
    if(arr_abstract->as<ArrayInt3>())
      srfExport_Array3<ArrayInt3>(arr_abstract, "<ArrayInt3>", "%d %d %d ");
    else
    if(arr_abstract->as<ArrayInt4>())
      srfExport_Array4<ArrayInt4>(arr_abstract, "<ArrayInt4>", "%d %d %d %d ");
    else

    if(arr_abstract->as<ArrayUShort1>())
      srfExport_Array1<ArrayUShort1>(arr_abstract, "<ArrayUShort1>", "%u ");
    else
    if(arr_abstract->as<ArrayUShort2>())
      srfExport_Array2<ArrayUShort2>(arr_abstract, "<ArrayUShort2>", "%u %u ");
    else
    if(arr_abstract->as<ArrayUShort3>())
      srfExport_Array3<ArrayUShort3>(arr_abstract, "<ArrayUShort3>", "%u %u %u ");
    else
    if(arr_abstract->as<ArrayUShort4>())
      srfExport_Array4<ArrayUShort4>(arr_abstract, "<ArrayUShort4>", "%u %u %u %u ");
    else

    if(arr_abstract->as<ArrayUShort1>())
      srfExport_Array1<ArrayUShort1>(arr_abstract, "<ArrayUShort1>", "%u ");
    else
    if(arr_abstract->as<ArrayUShort2>())
      srfExport_Array2<ArrayUShort2>(arr_abstract, "<ArrayUShort2>", "%u %u ");
    else
    if(arr_abstract->as<ArrayUShort3>())
      srfExport_Array3<ArrayUShort3>(arr_abstract, "<ArrayUShort3>", "%u %u %u ");
    else
    if(arr_abstract->as<ArrayUShort4>())
      srfExport_Array4<ArrayUShort4>(arr_abstract, "<ArrayUShort4>", "%u %u %u %u ");
    else

    if(arr_abstract->as<ArrayShort1>())
      srfExport_Array1<ArrayShort1>(arr_abstract, "<ArrayShort1>", "%d ");
    else
    if(arr_abstract->as<ArrayShort2>())
      srfExport_Array2<ArrayShort2>(arr_abstract, "<ArrayShort2>", "%d %d ");
    else
    if(arr_abstract->as<ArrayShort3>())
      srfExport_Array3<ArrayShort3>(arr_abstract, "<ArrayShort3>", "%d %d %d ");
    else
    if(arr_abstract->as<ArrayShort4>())
      srfExport_Array4<ArrayShort4>(arr_abstract, "<ArrayShort4>", "%d %d %d %d ");
    else

    if(arr_abstract->as<ArrayUByte1>())
      srfExport_Array1<ArrayUByte1>(arr_abstract, "<ArrayUByte1>", "%u ");
    else
    if(arr_abstract->as<ArrayUByte2>())
      srfExport_Array2<ArrayUByte2>(arr_abstract, "<ArrayUByte2>", "%u %u ");
    else
    if(arr_abstract->as<ArrayUByte3>())
      srfExport_Array3<ArrayUByte3>(arr_abstract, "<ArrayUByte3>", "%u %u %u ");
    else
    if(arr_abstract->as<ArrayUByte4>())
      srfExport_Array4<ArrayUByte4>(arr_abstract, "<ArrayUByte4>", "%u %u %u %u ");
    else

    if(arr_abstract->as<ArrayByte1>())
      srfExport_Array1<ArrayByte1>(arr_abstract, "<ArrayByte1>", "%d ");
    else
    if(arr_abstract->as<ArrayByte2>())
      srfExport_Array2<ArrayByte2>(arr_abstract, "<ArrayByte2>", "%d %d ");
    else
    if(arr_abstract->as<ArrayByte3>())
      srfExport_Array3<ArrayByte3>(arr_abstract, "<ArrayByte3>", "%d %d %d ");
    else
    if(arr_abstract->as<ArrayByte4>())
      srfExport_Array4<ArrayByte4>(arr_abstract, "<ArrayByte4>", "%d %d %d %d ");
    else

    if(arr_abstract->as<ArrayFloat1>())
      srfExport_Array1<ArrayFloat1>(arr_abstract, "<ArrayFloat1>", "%f ");
    else
    if(arr_abstract->as<ArrayFloat2>())
      srfExport_Array2<ArrayFloat2>(arr_abstract, "<ArrayFloat2>", "%f %f ");
    else
    if(arr_abstract->as<ArrayFloat3>())
      srfExport_Array3<ArrayFloat3>(arr_abstract, "<ArrayFloat3>", "%f %f %f ");
    else
    if(arr_abstract->as<ArrayFloat4>())
      srfExport_Array4<ArrayFloat4>(arr_abstract, "<ArrayFloat3>", "%f %f %f %f ");
    else

    if(arr_abstract->as<ArrayDouble1>())
      srfExport_Array1<ArrayDouble1>(arr_abstract, "<ArrayDouble1>", "%Lf ");
    else
    if(arr_abstract->as<ArrayDouble2>())
      srfExport_Array2<ArrayDouble2>(arr_abstract, "<ArrayDouble2>", "%Lf %Lf ");
    else
    if(arr_abstract->as<ArrayDouble3>())
      srfExport_Array3<ArrayDouble3>(arr_abstract, "<ArrayDouble3>", "%Lf %Lf %Lf ");
    else
    if(arr_abstract->as<ArrayDouble4>())
      srfExport_Array4<ArrayDouble4>(arr_abstract, "<ArrayDouble3>", "%Lf %Lf %Lf %Lf ");
    else
    {
      Log::error("Array type not supported for export.\n");
      VL_TRAP();
    }
  }

  void srfExport_DrawCallBase(DrawCall* dcall)
  {
      if (mObject_Ref_Count[dcall] > 1)
        mSRFString += indent() + "ID = " + getUID(dcall) + "\n";
      switch(dcall->primitiveType())
      {
        case PT_POINTS:                   mSRFString += indent() + "PrimitiveType = PT_POINTS\n"; break;
        case PT_LINES:                    mSRFString += indent() + "PrimitiveType = PT_LINES\n"; break;
        case PT_LINE_LOOP:                mSRFString += indent() + "PrimitiveType = PT_LINE_LOOP\n"; break;
        case PT_LINE_STRIP:               mSRFString += indent() + "PrimitiveType = PT_LINE_STRIP\n"; break;
        case PT_TRIANGLES:                mSRFString += indent() + "PrimitiveType = PT_TRIANGLES\n"; break;
        case PT_TRIANGLE_STRIP:           mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_STRIP\n"; break;
        case PT_TRIANGLE_FAN:             mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_FAN\n"; break;
        case PT_QUADS:                    mSRFString += indent() + "PrimitiveType = PT_QUADS\n"; break;
        case PT_QUAD_STRIP:               mSRFString += indent() + "PrimitiveType = PT_QUAD_STRIP\n"; break;
        case PT_POLYGON:                  mSRFString += indent() + "PrimitiveType = PT_POLYGON\n"; break;

        case PT_LINES_ADJACENCY:          mSRFString += indent() + "PrimitiveType = PT_LINES_ADJACENCY\n"; break;
        case PT_LINE_STRIP_ADJACENCY:     mSRFString += indent() + "PrimitiveType = PT_LINE_STRIP_ADJACENCY\n"; break;
        case PT_TRIANGLES_ADJACENCY:      mSRFString += indent() + "PrimitiveType = PT_TRIANGLES_ADJACENCY\n"; break;
        case PT_TRIANGLE_STRIP_ADJACENCY: mSRFString += indent() + "PrimitiveType = PT_TRIANGLE_STRIP_ADJACENCY\n"; break;

        case PT_PATCHES:                  mSRFString += indent() + "PrimitiveType = PT_PATCHES\n"; break;

        case PT_UNKNOWN:                  mSRFString += indent() + "PrimitiveType = PT_UNKNOWN\n"; break;
      }
      mSRFString += indent() + "Enabled = " + (dcall->isEnabled() ? "true" : "false") + "\n";

      if (dcall->patchParameter())
      {
        mSRFString += indent() + "PatchParameter = "; mAssign = true; srfExport_PatchParameter(dcall->patchParameter());
      }
  }

  void srfExport_PatchParameter(PatchParameter* pp)
  {
    if (isDefined(pp))
    {
      mSRFString += indent() + getUID(pp) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(pp);

    mSRFString += indent() + "<PatchParameter>\n";
    mSRFString += indent() + "{\n";
    ++mIndent;
    if (mObject_Ref_Count[pp] > 1)
      mSRFString += indent() + "ID = " + getUID(pp) + "\n";
    mSRFString += indent() + String::printf("PatchVertices = %d\n", pp->patchVertices());
    mSRFString += indent() + String::printf("PatchDefaultOuterLevel = ( %f %f %f %f )\n", pp->patchDefaultOuterLevel().x(), pp->patchDefaultOuterLevel().y(), pp->patchDefaultOuterLevel().z(), pp->patchDefaultOuterLevel().w() );
    mSRFString += indent() + String::printf("PatchDefaultInnerLevel = ( %f %f )\n",       pp->patchDefaultInnerLevel().x(), pp->patchDefaultInnerLevel().y() );
    --mIndent;
    mSRFString += indent() + "}\n";
  }

  void srfExport_DrawCall(DrawCall* dcall)
  {
    if (isDefined(dcall))
    {
      mSRFString += indent() + getUID(dcall) + "\n";
      return;
    }
    else
      mAlreadyDefined.insert(dcall);

    if (dcall->as<DrawArrays>())
    {
      DrawArrays* da = dcall->as<DrawArrays>();
      mSRFString += indent() + "<DrawArrays>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(da);
      mSRFString += indent() + String::printf("Instances = %d\n", da->instances());
      mSRFString += indent() + String::printf("Start = %d\n", da->start());
      mSRFString += indent() + String::printf("Count= %d\n", da->count());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->as<DrawElementsUInt>())
    {
      DrawElementsUInt* de = dcall->as<DrawElementsUInt>();
      mSRFString += indent() + "<DrawElementsUInt>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->as<DrawElementsUShort>())
    {
      DrawElementsUShort* de = dcall->as<DrawElementsUShort>();
      mSRFString += indent() + "<DrawElementsUShort>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->as<DrawElementsUByte>())
    {
      DrawElementsUByte* de = dcall->as<DrawElementsUByte>();
      mSRFString += indent() + "<DrawElementsUByte>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + String::printf("Instances = %d\n", de->instances());
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + String::printf("BaseVertex = %d\n", de->baseVertex());
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->as<MultiDrawElementsUInt>())
    {
      MultiDrawElementsUInt* de = dcall->as<MultiDrawElementsUInt>();
      mSRFString += indent() + "<MultiDrawElementsUInt>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->as<MultiDrawElementsUShort>())
    {
      MultiDrawElementsUShort* de = dcall->as<MultiDrawElementsUShort>();
      mSRFString += indent() + "<MultiDrawElementsUShort>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    if (dcall->as<MultiDrawElementsUByte>())
    {
      MultiDrawElementsUByte* de = dcall->as<MultiDrawElementsUByte>();
      mSRFString += indent() + "<MultiDrawElementsUByte>\n";
      mSRFString += indent() + "{\n";
      ++mIndent;
      srfExport_DrawCallBase(de);
      mSRFString += indent() + "PrimitiveRestartEnabled = " + (de->primitiveRestartEnabled() ? "true\n" : "false\n");
      mSRFString += indent() + "IndexBuffer = "; mAssign = true; srfExport_Array(de->indexBuffer());
      mSRFString += indent() + "CountVector = "; srfExport_vector1(de->countVector(), "%u ");
      mSRFString += indent() + "BaseVertices = "; srfExport_vector1(de->baseVertices(), "%u ");
      --mIndent;
      mSRFString += indent() + "}\n";
    }
    else
    {
      Log::error("DrawCall type not supported for export.\n");
      VL_TRAP();
    }
  }

  template<typename T>
  void srfExport_vector1(const std::vector<T>& vec, const char* format)
  {
    mSRFString += "( ";
    for(size_t i=0; i<vec.size(); ++i)
    {
      mSRFString += String::printf(format, vec[i]);
      /*if (i && (i%10==0) && i!=vec.size()-1)
        mSRFString += indent() + "\n"*/
    }
    mSRFString += " )\n";
  }

  String indent()
  {
    String str;
    if (mAssign)  
      mAssign = false;
    else
      for(int i=0; i<mIndent; ++i)
        str += '\t';
    return str;
  }

  String getUID(Object* object)
  {
    std::map< ref<Object>, String >::iterator it = mObject_To_UID.find(object);
    if( it != mObject_To_UID.end() )
      return it->second;
    else
    {
      VL_TRAP();
      return "";
    }
  }

  void generateUID(Object* object, const char* prefix)
  {
    if (object)
    {
      // add reference count
      ++mObject_Ref_Count[object];
      if (mObject_To_UID.find(object) == mObject_To_UID.end())
      {
        String uid = String::printf("#%sid%d", prefix, ++mUIDCounter);
        mObject_To_UID[object] = uid;
      }
    }
  }

  bool isDefined(Object* obj)
  {
    return mAlreadyDefined.find(obj) != mAlreadyDefined.end();
  }

  virtual void initEvent()
  {
    Log::notify(appletInfo());

    ref<Geometry> geom = makeIcosphere( vec3(0,0,0), 10, 0 );
    // ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 4 );
    geom->computeNormals();
    geom->setColorArray( geom->normalArray() );
    // TriangleStripGenerator::stripfy(geom.get(), 22, false, false, true);
    // mic fixme: this does no realizes that we are using primitive restart
    // mic fixme: make this manage also MultiDrawElements
    // geom->makeGLESFriendly();

    ref<Effect> fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->setRenderState( new Light, 0 );

    sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);

    // export init
    mUIDCounter = 0;
    mAssign = 0;
    mIndent = 0;

    srfPrelink_Geometry(geom.get());
    srfExport_Geometry(geom.get());
    std::fstream fout;
    fout.open("D:/VL/srf_export.vl", std::ios::out);
    fout.write( mSRFString.toStdString().c_str(), mSRFString.length() );
    fout.close();

    SRF_Parser parser;
    parser.mTokenizer = new SRF_Tokenizer;
    parser.mTokenizer->setInputFile( new DiskFile("D:/VL/srf_export.vl") );

    parser.parse();
    String dump = parser.dump();
    parser.link();

    // dump the dump
    fout.open( "D:/VL/srf_export_dump.vl", std::ios::out );
    fout.write( dump.toStdString().c_str(), dump.length() );
    fout.close();

    exit(0);
  }

protected:
  std::map< ref<Object>, String > mObject_To_UID;
  std::map< ref<Object>, int > mObject_Ref_Count;
  std::set< ref<Object> > mAlreadyDefined;
  int mUIDCounter;
  String mSRFString;
  int mIndent;
  bool mAssign;
};

// Have fun!

BaseDemo* Create_App_VLSRF() { return new App_VLSRF; }
