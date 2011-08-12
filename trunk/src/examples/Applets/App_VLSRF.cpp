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
          if (ch2 == 't')
            ch1 = '\t';
          else
          if (ch2 == 'n')
            ch1 = '\n';
          else
          if (ch2 == 'r')
            ch1 = '\r';
          else
          if (ch2 == '"')
            ch1 = '"';
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

struct SRF_Boolean;
struct SRF_String;
struct SRF_NumberOrIdentifier;
struct SRF_UID;
struct SRF_Object;
struct SRF_List;
// struct SRF_ArrayBoolean; // use ArrayInt32 which makes the files also shorter
struct SRF_ArrayUID;
struct SRF_ArrayInt32;
struct SRF_ArrayInt64;
struct SRF_ArrayFloat;
struct SRF_ArrayDouble;
struct SRF_ArrayString;
struct SRF_ArrayIdentifier;

struct SRF_Visitor: public Object
{
  virtual void visitBoolean(SRF_Boolean*) {}
  virtual void visitString(SRF_String*) {}
  virtual void visitNumberOrIdentifier(SRF_NumberOrIdentifier*) {}
  virtual void visitUID(SRF_UID*) {}
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

struct SRF_Value: public Object
{
  virtual void acceptVisitor(SRF_Visitor*) = 0;
};

struct SRF_Boolean: public SRF_Value
{
  SRF_Boolean(): mValue(false) {}
  SRF_Boolean(bool value): mValue(value) {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitBoolean(this); }

  bool mValue;
};

struct SRF_String: public SRF_Value
{
  SRF_String() {}
  SRF_String(const std::string& str): mValue(str) {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitString(this); }

  std::string mValue;
};

struct SRF_NumberOrIdentifier: public SRF_Value
{
  SRF_NumberOrIdentifier(const std::string& str, ETokenType type): mValue(str), mType(type) {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitNumberOrIdentifier(this); }

  float getFloat() const
  {
    VL_CHECK(mValue.length());
    // this is ok for int32, int64, float and double
    return (float)atof(mValue.c_str());
  }

  double getDouble() const
  {
    VL_CHECK(mValue.length());
    // this is ok for int32, int64, float and double
    return atof(mValue.c_str());
  }

  int getInt32() const
  {
    VL_CHECK(mValue.length());
    VL_CHECK(mType == TT_Int32 || mType == TT_Int64)
    return atoi(mValue.c_str());
  }

  long long getInt64() const
  {
    VL_CHECK(mValue.length());
    VL_CHECK(mType == TT_Int32 || mType == TT_Int64)
    return atoll(mValue.c_str());
  }

  ETokenType mType;
  std::string mValue;
};

struct SRF_UID: public SRF_Value
{
  SRF_UID(const std::string& str) { mValue.mUID = str; }
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitUID(this); }

  struct ValueElem
  {
    std::string mUID; // the UID string
    ref<SRF_Value> mPtr; // the linked object
  } mValue;
};

struct SRF_NameValue
{
  std::string mName;
  ref<SRF_Value> mValue;
};

struct SRF_Object: public SRF_Value
{
  SRF_Object()
  {
    mNameValues.reserve(10);
  }
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitObject(this); }


  std::string mName;
  std::string mUID;
  std::vector<SRF_NameValue> mNameValues;
};

struct SRF_List: public SRF_Value
{
  SRF_List()
  {
    mValue.reserve(10);
  }
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitList(this); }

  std::vector< ref<SRF_Value> > mValue;
};

struct SRF_ArrayInt32: public SRF_Value
{
  SRF_ArrayInt32() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<int> mValue;
};

struct SRF_ArrayInt64: public SRF_Value
{
  SRF_ArrayInt64() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<long long> mValue;
};

struct SRF_ArrayFloat: public SRF_Value
{
  SRF_ArrayFloat() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<float> mValue;
};

struct SRF_ArrayDouble: public SRF_Value
{
  SRF_ArrayDouble() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<double> mValue;
};

struct SRF_ArrayUID: public SRF_Value
{
  SRF_ArrayUID() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  struct ValueElem
  {
    ValueElem(const std::string& uid): mUID(uid) {}
    std::string mUID; // the UID string
    ref<SRF_Value> mPtr; // the linked object
  };

  std::vector<ValueElem> mValue;
};

struct SRF_ArrayIdentifier: public SRF_Value
{
  SRF_ArrayIdentifier() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<std::string> mValue;
};

struct SRF_ArrayString: public SRF_Value
{
  SRF_ArrayString() {}
  virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

  std::vector<std::string> mValue;
};
//-----------------------------------------------------------------------------
// SRF_DumpVisitor
//-----------------------------------------------------------------------------
struct SRF_DumpVisitor: public SRF_Visitor
{
  SRF_DumpVisitor()
  {
    mIndent = 0;
    mAssignment = false;
  }

  void indent()
  {
    if (mAssignment)
      mAssignment = false;
    else
    {
      for(int i=0; i<mIndent; ++i)
        printf("  ");
    }
  }

  virtual void visitBoolean(SRF_Boolean* str) 
  {
    indent(); printf("bool: %s\n", str->mValue ? "true" : "false");
  }

  virtual void visitString(SRF_String* str) 
  {
    indent(); printf("\"%s\"\n", str->mValue.c_str());
  }

  virtual void visitNumberOrIdentifier(SRF_NumberOrIdentifier* data) 
  {
    indent(); printf("%s\n", data->mValue.c_str());
  }

  virtual void visitUID(SRF_UID* uid) 
  {
    indent(); printf("%s\n", uid->mValue.mUID.c_str());
  }
  
  virtual void visitObject(SRF_Object* obj) 
  {
    indent(); printf("%s\n", obj->mName.c_str());
    indent(); printf("{\n");
    mIndent++;
    if (obj->mUID.length())
    {
      indent(); printf("ID = %s\n", obj->mUID.c_str());
    }
    for(size_t i=0; i<obj->mNameValues.size(); ++i)
    {
      indent(); printf("%s = ", obj->mNameValues[i].mName.c_str());
      mAssignment = true;
      obj->mNameValues[i].mValue->acceptVisitor(this);
    }
    mIndent--;
    indent(); printf("}\n");
  }

  virtual void visitList(SRF_List* list) 
  {
    indent(); printf("[\n");
    mIndent++;
    for(size_t i=0; i<list->mValue.size(); ++i)
      list->mValue[i]->acceptVisitor(this);
    mIndent--;
    indent(); printf("]\n");
  }
  
  virtual void visitArray(SRF_ArrayInt32* arr) 
  {
    indent(); printf("( int32: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("%d ", arr->mValue[i]);
    printf(")\n");
  }

  virtual void visitArray(SRF_ArrayInt64* arr) 
  {
    indent(); printf("( int64: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("%lld ", arr->mValue[i]);
    printf(")\n");
  }

  virtual void visitArray(SRF_ArrayFloat* arr) 
  {
    indent(); printf("( float: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("%f ", arr->mValue[i]);
    printf(")\n");
  }

  virtual void visitArray(SRF_ArrayDouble* arr) 
  {
    indent(); printf("( double: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("%llf ", arr->mValue[i]);
    printf(")\n");
  }

  virtual void visitArray(SRF_ArrayIdentifier* arr) 
  {
    indent(); printf("( identifier: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("%s ", arr->mValue[i].c_str());
    printf(")\n");
  }

  virtual void visitArray(SRF_ArrayString* arr) 
  {
    indent(); printf("( string: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("\"%s\" ", arr->mValue[i].c_str());
    printf(")\n");
  }

  virtual void visitArray(SRF_ArrayUID* arr) 
  {
    indent(); printf("( uid: ");
    for(size_t i=0 ;i<arr->mValue.size(); ++i)
      printf("%s ", arr->mValue[i].mUID.c_str());
    printf(")\n");
  }

private:
  int mIndent;
  bool mAssignment;
};
//-----------------------------------------------------------------------------
// SRF_DumpVisitor
//-----------------------------------------------------------------------------
struct SRF_LinkVisitor: public SRF_Visitor
{
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

  virtual void visitBoolean(SRF_Boolean*) {}

  virtual void visitString(SRF_String*) {}

  virtual void visitNumberOrIdentifier(SRF_NumberOrIdentifier*) {}

  virtual void visitUID(SRF_UID* uid)
  {
    uid->mValue.mPtr = link(uid->mValue.mUID);
  }

  virtual void visitObject(SRF_Object* obj)
  {
    for(size_t i=0; i<obj->mNameValues.size(); ++i)
      obj->mNameValues[i].mValue->acceptVisitor(this);
  }

  virtual void visitList(SRF_List* list) 
  {
    for(size_t i=0; i<list->mValue.size(); ++i)
      list->mValue[i]->acceptVisitor(this);
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
            if (!object->mUID.empty())
              return false;

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
        object->mNameValues.resize( object->mNameValues.size() + 1 );
        SRF_NameValue& name_value = object->mNameValues.back();

        // Key
        name_value.mName = mToken.mString;

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
            name_value.mValue = object;
            if (!parseObject( object.get() ) )
              return false;
          }
          else
          // An [ list ]
          if (mToken.mType == TT_LeftSquareBracket)
          {
            ref<SRF_List> list = new SRF_List;
            name_value.mValue = list;
            if ( !parseList( list.get() ) )
              return false;
          }
          else
          // An ( array )
          if (mToken.mType == TT_LeftRoundBracket)
          {
            ref<SRF_Value> arr;
            if ( parseArray( arr ) )
              name_value.mValue = arr;
            else
              return false;
          }
          else
          // An #id
          if (mToken.mType == TT_UID)
          {
            name_value.mValue = new SRF_UID(mToken.mString);
          }
          else
          // A "string"
          if (mToken.mType == TT_String)
          {
            name_value.mValue = new SRF_String(mToken.mString);
          }
          else
          // A boolean (true/false)
          if (mToken.mType == TT_Boolean)
          {
            name_value.mValue = new SRF_Boolean(mToken.mString == "true");
          }
          else
          // An Identifier or Integer or Float
          if (mToken.mType == TT_Identifier || mToken.mType == TT_Int32 || mToken.mType == TT_Int64 || mToken.mType == TT_Float || mToken.mType == TT_Double)
          {
            name_value.mValue = new SRF_NumberOrIdentifier(mToken.mString, mToken.mType);
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
        switch( mToken.mType )
        {
          // boolean
          case TT_Boolean:
            list->mValue.push_back( new SRF_Boolean(mToken.mString == "true") );
            break;

          // string
          case TT_String:
            list->mValue.push_back( new SRF_String(mToken.mString) );
            break;

          // int, float, identifier
          case TT_Int32:
          case TT_Int64:
          case TT_Float:
          case TT_Double:
          case TT_Identifier:
            list->mValue.push_back( new SRF_NumberOrIdentifier(mToken.mString, mToken.mType) );
            break;

          // UID
          case TT_UID:
            list->mValue.push_back( new SRF_UID(mToken.mString) );
            break;

          // list
          case TT_LeftSquareBracket:
            {
              ref<SRF_List> sub_list = new SRF_List;
              if ( parseList( sub_list.get() ) )
                list->mValue.push_back( sub_list );
              else
                return false;
              break;
            }

          // array
          case TT_LeftRoundBracket:
            {
              ref<SRF_Value> arr;
              if (parseArray(arr))
                list->mValue.push_back(arr);
              else
                return false;
              break;
            }

          // object
          case TT_ObjectHeader:
            {
              ref<SRF_Object> object = new SRF_Object;
              object->mName = mToken.mString;
              if ( parseObject( object.get() ) )
                list->mValue.push_back( object );
              else
                return false;
              break;
            }

        default:
          return false;
        }
      }
    }
    return false;
  }

  bool parseArray(ref<SRF_Value>& arr)
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

  void dump()
  {
    if (mRoot)
    {
      SRF_DumpVisitor dump_visitor;
      mRoot->acceptVisitor(&dump_visitor);
    }
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
  virtual void initEvent()
  {
    Log::notify(appletInfo());

    SRF_Parser parser;
    parser.mTokenizer = new SRF_Tokenizer;
    parser.mTokenizer->setInputFile( new DiskFile("D:/VL/test.vl") );

    parser.parse();
    parser.dump();
    parser.link();

    ref<Geometry> geom = makeBox( vec3(0,0,0), 10, 10, 10 );
    geom->computeNormals();

    ref<Effect> fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->setRenderState( new Light, 0 );

    sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);

    geom->drawCalls()->push_back( geom->drawCalls()->at(0) );
    geom->setColorArray( geom->vertexArray() );
    // geom->makeGLESFriendly();
    geom->convertToVertexAttribs();

    // export init
    mUIDCounter = 0;
    mEquals = 0;
    mIndent = 0;

    srfExport_Geometry(geom.get());
    std::fstream fout;
    fout.open("D:/VL/srf_export.vl", std::ios::out);
    fout.write( mSRFString.toStdString().c_str(), mSRFString.length() );
    fout.close();

    exit(0);
  }

  void srfExport_Geometry(Geometry* geom)
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
      generateUID(geom->drawCalls()->at(i), "drawcall_");

    // --- actual export ---
    mSRFString += indent() + "<Geometry>\n";
    mSRFString += indent() + "{\n"; 
    ++mIndent;
    // vertex arrays
    if (geom->vertexArray()) { mSRFString += indent() + "VertexArray = ";  mEquals = true; srfExport_Array(geom->vertexArray()); }
    if (geom->normalArray()) { mSRFString += indent() + "NormalArray = ";  mEquals = true; srfExport_Array(geom->normalArray()); }
    if (geom->colorArray()) { mSRFString += indent() + "ColorArray = ";  mEquals = true; srfExport_Array(geom->colorArray()); }
    if (geom->secondaryColorArray()) { mSRFString += indent() + "SecondaryArray = ";  mEquals = true; srfExport_Array(geom->secondaryColorArray()); }
    if (geom->fogCoordArray()) { mSRFString += indent() + "FogCoordArray = ";  mEquals = true; srfExport_Array(geom->fogCoordArray()); }
    for( int i=0; i<VL_MAX_TEXTURE_UNITS; ++i)
      if (geom->texCoordArray(i)) { mSRFString += indent() + String::printf("TexCoordArray%d = ", i);  mEquals = true; srfExport_Array(geom->texCoordArray(i)); }
    for(size_t i=0; i<VL_MAX_GENERIC_VERTEX_ATTRIB; ++i)
      if (geom->vertexAttribArray(i))
      {
        if (geom->vertexAttribArray(i)) { mSRFString += indent() + String::printf("VertexAttribArray%d = ", i);  mEquals = true; srfExport_VertexAttribInfo(geom->vertexAttribArray(i)); }
      }
      // draw calls
    for(int i=0; i<geom->drawCalls()->size(); ++i)
    {
      mSRFString += indent() + "DrawCall = ";  mEquals = true; srfExport_DrawCall(geom->drawCalls()->at(i));
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
        mSRFString += indent() + "\tID = " + getUID(info) + "\n";
      mSRFString += indent() + String::printf("AttribLocation = %d\n", info->attribLocation());
      mSRFString += indent() + "Data = "; mEquals = true; srfExport_Array(info->data());
      mSRFString += indent() + "Normalize = " + (info->normalize() ? "true\n" : "false\n");
      mSRFString += indent() + "DataBehavior = ";
      switch(info->dataBehavior())
      {
      case VAB_NORMAL: mSRFString += "VAB_NORMAL\n"; break;
      case VAB_PURE_INTEGER: mSRFString += "VAB_PURE_INTEGER\n"; break;
      case VAB_PURE_DOUBLE: mSRFString += "VAB_PURE_DOUBLE\n"; break;
      }
      --mIndent;
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

    // mic fixme: support other arrays
    if(arr_abstract->as<ArrayFloat1>())
    {
      ArrayFloat1* arr = arr_abstract->as<ArrayFloat1>();
      mSRFString += indent() + "<ArrayFloat1>\n";
      mSRFString += indent() + "{\n";
      if (mObject_Ref_Count[arr] > 1)
        mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
      if (arr->size()) // allow empty arrays this way
      {
        mSRFString += indent() + "\tValue = (\n\t\t" + indent();
        for(size_t i=0; i<arr->size(); ++i)
        {
          mSRFString += String::printf("%f ", arr->at(i) );
          if (i && (i % 10 == 0) && i != arr->size()-1)
            mSRFString += "\n\t\t" + indent();
        }
        mSRFString += "\n" + indent() + "\t)\n";
      }
      mSRFString += indent() + "}\n";
    }
    else
    if(arr_abstract->as<ArrayFloat2>())
    {
      ArrayFloat2* arr = arr_abstract->as<ArrayFloat2>();
      mSRFString += indent() + "<ArrayFloat2>\n";
      mSRFString += indent() + "{\n";
      if (mObject_Ref_Count[arr] > 1)
        mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
      if (arr->size()) // allow empty arrays this way
      {
        mSRFString += indent() + "\tValue = (\n\t\t" + indent();
        for(size_t i=0; i<arr->size(); ++i)
        {
          mSRFString += String::printf("%f %f ", arr->at(i).x(), arr->at(i).y() );
          if (i && (i % 10 == 0) && i != arr->size()-1)
            mSRFString += "\n\t\t" + indent();
        }
        mSRFString += "\n" + indent() + "\t)\n";
      }
      mSRFString += indent() + "}\n";
    }
    else
    if(arr_abstract->as<ArrayFloat3>())
    {
      ArrayFloat3* arr = arr_abstract->as<ArrayFloat3>();
      mSRFString += indent() + "<ArrayFloat3>\n";
      mSRFString += indent() + "{\n";
      if (mObject_Ref_Count[arr] > 1)
        mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
      if (arr->size()) // allow empty arrays this way
      {
        mSRFString += indent() + "\tValue = (\n\t\t" + indent();
        for(size_t i=0; i<arr->size(); ++i)
        {
          mSRFString += String::printf("%f %f %f ", arr->at(i).x(), arr->at(i).y(), arr->at(i).z() );
          if (i && (i % 10 == 0) && i != arr->size()-1)
            mSRFString += "\n\t\t" + indent();
        }
        mSRFString += "\n" + indent() + "\t)\n";
      }
      mSRFString += indent() + "}\n";
    }
    else
    if(arr_abstract->as<ArrayFloat4>())
    {
      ArrayFloat4* arr = arr_abstract->as<ArrayFloat4>();
      mSRFString += indent() + "<ArrayFloat4>\n";
      mSRFString += indent() + "{\n";
      if (mObject_Ref_Count[arr] > 1)
        mSRFString += indent() + "\tID = " + getUID(arr) + "\n";
      if (arr->size()) // allow empty arrays this way
      {
        mSRFString += indent() + "\tValue = (\n\t\t" + indent();
        for(size_t i=0; i<arr->size(); ++i)
        {
          mSRFString += String::printf("%f %f %f %f", arr->at(i).x(), arr->at(i).y(), arr->at(i).z(), arr->at(i).w() );
          if (i && (i % 10 == 0) && i != arr->size()-1)
            mSRFString += "\n\t\t" + indent();
        }
        mSRFString += "\n" + indent() + "\t)\n";
      }
      mSRFString += indent() + "}\n";
    }
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
      if (mObject_Ref_Count[da] > 1)
        mSRFString += indent() + "\tID = " + getUID(da) + "\n";
      mSRFString += indent() + String::printf("\tStart = %d\n", da->start());
      mSRFString += indent() + String::printf("\tCount= %d\n", da->count());
      mSRFString += indent() + String::printf("\tInstances = %d\n", da->instances());
      mSRFString += indent() + "}\n";
    }
  }


  String indent()
  {
    String str;
    if (mEquals)  
      mEquals = false;
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

protected:
  std::map< ref<Object>, String > mObject_To_UID;
  std::map< ref<Object>, int > mObject_Ref_Count;
  std::set< ref<Object> > mAlreadyDefined;
  int mUIDCounter;
  String mSRFString;
  int mIndent;
  bool mEquals;
};

// Have fun!

BaseDemo* Create_App_VLSRF() { return new App_VLSRF; }
