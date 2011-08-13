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
#include <vlCore\String.hpp>
#include <vlCore\Say.hpp>
#include <vlCore\Log.hpp>
#include <vlCore\BufferedStream.hpp>

// mic fixme
#ifdef _MSC_VER
#define atoll _atoi64
#endif

namespace vl
{
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
      ObjectHeader        //  <ObjectHeader>

    } EType;

    SRF_Token(): mType(TOKEN_ERROR) {}

    std::string mString;
    EType mType;
  };
  //-----------------------------------------------------------------------------
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
      token.mType = SRF_Token::TOKEN_ERROR;
      token.mString.clear();

      // read chars skipping spaces
      char ch1=0, ch2=0;
      do {
        if (!readTextChar(ch1))
        {
          token.mType = SRF_Token::TOKEN_EOF;
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
        token.mType = SRF_Token::LeftRoundBracket;
        token.mString = "(";
        return true;
    
      case ')':
        token.mType = SRF_Token::RightRoundBracket;
        token.mString = ")";
        return true;

      case '[':
        token.mType = SRF_Token::LeftSquareBracket;
        token.mString = "[";
        return true;
    
      case ']':
        token.mType = SRF_Token::RightSquareBracket;
        token.mString = "]";
        return true;

      case '{':
        if(readTextChar(ch2) && ch2 == '<')
        {
          token.mType = SRF_Token::LeftFancyBracket;
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
          token.mType = SRF_Token::LeftCurlyBracket;
          token.mString = "{";
          if(!isEndOfFile())
            ungetToken(ch2);
        }
        return true;

      case '}':
        token.mType = SRF_Token::RightCurlyBracket;
        token.mString = "}";
        return true;

      case '>':
        if(readTextChar(ch2))
        {
          if(ch2 == '}')
          {
            token.mType = SRF_Token::RightFancyBracket;
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
        token.mType = SRF_Token::Equals; 
        token.mString = "=";
        return true;

      case '<':
        token.mType = SRF_Token::ObjectHeader;
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
        token.mType = SRF_Token::UID;
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
        token.mType = SRF_Token::String;
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
          token.mType = SRF_Token::Identifier;
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
              token.mType = SRF_Token::Boolean;
            return true;
          }
        }
        else
        // Int32 / Int64 / Float / Double
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
        // 123.123E
        // 123.123e+
        if ( ch1 >= '0' && ch1 <= '9' || ch1 == '.' || ch1 == '+' || ch1 == '-' )
        {
          token.mType = SRF_Token::TOKEN_ERROR;
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

          // for simplicity we don't deal with the cases when we reach TOKEN_EOF after +/-/./E/E+-
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
                  token.mType = SRF_Token::Int64;
                else
                {
                  token.mType = SRF_Token::Int32;
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
                  token.mType = SRF_Token::Int64;
                else
                {
                  token.mType = SRF_Token::Int32;
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
                  token.mType = SRF_Token::Double;
                else
                {
                  token.mType = SRF_Token::Float;
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
                  token.mType = SRF_Token::Double;
                else
                {
                  token.mType = SRF_Token::Float;
                  ungetToken(ch1);
                }
                return true;
              }
              break;
            }
          }
          // reached TOKEN_EOF in the middle of the parsing so we check where we were, note that it cannot be a Int64 or a Double
          if (state == sINT)
          {
            token.mType = SRF_Token::Int32;
            return true;
          }
          else
          if (state == sFRAC || state == sEXP)
          {
            token.mType = SRF_Token::Float;
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

    int lineNumber() const { return mLineNumber; }

  private:
    int mLineNumber;
  };
  //-----------------------------------------------------------------------------
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
  //-----------------------------------------------------------------------------
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
  //-----------------------------------------------------------------------------
  class SRF_ArrayInt32: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayInt32, SRF_Array)

  public:
    SRF_ArrayInt32() {}
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<int>& value() { return mValue; }
    const std::vector<int>& value() const { return mValue; }

  private:
    std::vector<int> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayInt64: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayInt64, SRF_Array)

  public:
    SRF_ArrayInt64() {}
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<long long>& value() { return mValue; }
    const std::vector<long long>& value() const { return mValue; }

  private:
    std::vector<long long> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayFloat: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayFloat, SRF_Array)

  public:
    SRF_ArrayFloat() {}
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<float>& value() { return mValue; }
    const std::vector<float>& value() const { return mValue; }

  private:
    std::vector<float> mValue;
  };
  //-----------------------------------------------------------------------------
  class SRF_ArrayDouble: public SRF_Array
  {
    VL_INSTRUMENT_CLASS(vl::SRF_ArrayDouble, SRF_Array)

  public:
    SRF_ArrayDouble() {}
    virtual void acceptVisitor(SRF_Visitor* v) { v->visitArray(this); }

    std::vector<double>& value() { return mValue; }
    const std::vector<double>& value() const { return mValue; }

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

    struct Value
    {
      Value(const std::string& uid): mUID(uid) {}
      std::string mUID; // the UID string
      ref<SRF_Object> mPtr; // the linked object
    };

    std::vector<Value>& value() { return mValue; }
    const std::vector<Value>& value() const { return mValue; }

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
      // mic fixme: reenable
      // mKeyValue.reserve(16);
      mUID = "#NULL";
    }

    virtual void acceptVisitor(SRF_Visitor* v) { v->visitObject(this); }

    struct Value
    {
      std::string mKey;
      SRF_Value mValue;
    };

    void setName(const std::string& name) { mName = name; }
    const std::string& name() const { return mName; }

    void setUID(const std::string& uid) { mUID = uid; }
    const std::string& uid() const { return mUID; }

    std::vector<Value>& value() { return mKeyValue; }
    const std::vector<Value>& value() const { return mKeyValue; }

  private:
    // mic fixme: add a multimap for quick access
    std::string mName;
    std::string mUID;
    std::vector<Value> mKeyValue;
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
    mType = other.mType;

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
    if (obj)
      mUnion.mObject->incReference();
  }
  //-----------------------------------------------------------------------------
  void SRF_Value::setList(SRF_List* list)
  {
    VL_CHECK(list);

    release();
    mType = List;
    mUnion.mList = list;
    mUnion.mList->incReference();
  }
  //-----------------------------------------------------------------------------
  void SRF_Value::setArray(SRF_Array* arr)
  {
    release();

    VL_CHECK(arr);

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
          mDump += "\t";
      }
    }

    virtual void visitObject(SRF_Object* obj)
    {
      indent(); mDump += String::printf("%s\n", obj->name().c_str());
      indent(); mDump += "{\n";
      mIndent++;
      if (obj->uid().length() && obj->uid() != "#NULL")
      {
        indent(); mDump += String::printf("ID = %s\n", obj->uid().c_str());
      }
      for(size_t i=0; i<obj->value().size(); ++i)
      {
        indent(); mDump += String::printf("%s = ", obj->value()[i].mKey.c_str());
        switch(obj->value()[i].mValue.type())
        {
        case SRF_Value::Object:
          mAssign = true;
          obj->value()[i].mValue.getObject()->acceptVisitor(this);
          break;
        case SRF_Value::List:
          mAssign = true;
          obj->value()[i].mValue.getList()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayString:
          mAssign = true;
          obj->value()[i].mValue.getArrayString()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayIdentifier:
          mAssign = true;
          obj->value()[i].mValue.getArrayIdentifier()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayUID:
          mAssign = true;
          obj->value()[i].mValue.getArrayUID()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayInt32:
          mAssign = true;
          obj->value()[i].mValue.getArrayInt32()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayInt64:
          mAssign = true;
          obj->value()[i].mValue.getArrayInt64()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayFloat:
          mAssign = true;
          obj->value()[i].mValue.getArrayFloat()->acceptVisitor(this);
          break;
        case SRF_Value::ArrayDouble:
          mAssign = true;
          obj->value()[i].mValue.getArrayDouble()->acceptVisitor(this);
          break;
        case SRF_Value::String:
          mDump += String::printf("%s\n", obj->value()[i].mValue.getString());
          break;
        case SRF_Value::Identifier:
          mDump += String::printf("%s\n", obj->value()[i].mValue.getIdentifier());
          break;
        case SRF_Value::UID:
          mDump += String::printf("%s\n", obj->value()[i].mValue.getUID());
          break;
        case SRF_Value::Bool:
          mDump += String::printf("%s\n", obj->value()[i].mValue.getBool() ? "true" : "false");
          break;
        case SRF_Value::Int64:
          mDump += String::printf("%lld\n", obj->value()[i].mValue.getInt64());
          break;
        case SRF_Value::Double:
          mDump += String::printf("%Lf\n", obj->value()[i].mValue.getDouble());
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
      for(size_t i=0; i<list->value().size(); ++i)
      {
        switch(list->value()[i].type())
        {
        case SRF_Value::Object:
          list->value()[i].getObject()->acceptVisitor(this);
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
          mDump += String::printf("%s\n", list->value()[i].getString());
          break;
        case SRF_Value::Identifier:
          mDump += String::printf("%s\n", list->value()[i].getIdentifier());
          break;
        case SRF_Value::UID:
          mDump += String::printf("%s\n", list->value()[i].getUID());
          break;
        case SRF_Value::Bool:
          mDump += String::printf("%s\n", list->value()[i].getBool() ? "true" : "false");
          break;
        case SRF_Value::Int64:
          mDump += String::printf("%lld\n", list->value()[i].getInt64());
          break;
        case SRF_Value::Double:
          mDump += String::printf("%Lf\n", list->value()[i].getDouble());
          break;
        }
      }
      mIndent--;
      indent(); mDump += "]\n";
    }

    virtual void visitArray(SRF_ArrayInt32* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += String::printf("%d ", arr->value()[i]);
      mDump += ")\n";
    }

    virtual void visitArray(SRF_ArrayInt64* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += String::printf("%lld ", arr->value()[i]);
      mDump += ")\n";
    }

    virtual void visitArray(SRF_ArrayFloat* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += String::printf("%f ", arr->value()[i]);
      mDump += ")\n";
    }

    virtual void visitArray(SRF_ArrayDouble* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += String::printf("%Lf ", arr->value()[i]);
      mDump += ")\n";
    }

    virtual void visitArray(SRF_ArrayIdentifier* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += String::printf("%s ", arr->value()[i].c_str());
      mDump += ")\n";
    }

    virtual void visitArray(SRF_ArrayString* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += "\"" + encodeString(arr->value()[i]) + "\" ";
      mDump += ")\n";
    }

    virtual void visitArray(SRF_ArrayUID* arr)
    {
      indent(); mDump += "( ";
      for(size_t i=0 ;i<arr->value().size(); ++i)
        mDump += String::printf("%s ", arr->value()[i].mUID.c_str());
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
  // SRF_LinkVisitor
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
      for(size_t i=0; i<obj->value().size(); ++i)
      {
        if (obj->value()[i].mValue.type() == SRF_Value::Object)
          obj->value()[i].mValue.getObject()->acceptVisitor(this);
        else
        if (obj->value()[i].mValue.type() == SRF_Value::List)
          obj->value()[i].mValue.getList()->acceptVisitor(this);
        else
        if (obj->value()[i].mValue.type() == SRF_Value::ArrayUID)
          obj->value()[i].mValue.getArrayUID()->acceptVisitor(this);
        else
        if (obj->value()[i].mValue.type() == SRF_Value::UID)
        {
          SRF_Object* ptr = link( obj->value()[i].mValue.getUID() );
          obj->value()[i].mValue.setObject( ptr );
        }
      }
    }

    virtual void visitList(SRF_List* list)
    {
      for(size_t i=0; i<list->value().size(); ++i)
      {
        if (list->value()[i].type() == SRF_Value::Object)
          list->value()[i].getObject()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::List)
          list->value()[i].getList()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::ArrayUID)
          list->value()[i].getArrayUID()->acceptVisitor(this);
        else
        if (list->value()[i].type() == SRF_Value::UID)
        {
          SRF_Object* obj = link( list->value()[i].getUID() );
          list->value()[i].setObject( obj );
        }
      }
    }

    virtual void visitArray(SRF_ArrayUID* arr)
    {
      for(size_t i=0 ;i<arr->value().size(); ++i)
        arr->value()[i].mPtr = link(arr->value()[i].mUID);
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
    SRF_Parser()
    {
      mTokenizer = new SRF_Tokenizer;
    }

    bool getToken(SRF_Token& token) { return mTokenizer->getToken(token); }

    bool parse()
    {
      mRoot = NULL;
      if(getToken(mToken) && mToken.mType == SRF_Token::ObjectHeader)
      {
        mRoot = new SRF_Object;
        mRoot->setName(mToken.mString);
        if (parseObject(mRoot.get()))
        {
          return true;
        }
        else
        {
          mRoot = NULL;
          if (mToken.mString.length())
            Log::error( Say("Line %n: parse error at '%s'.\n") << mTokenizer->lineNumber() << mToken.mString.c_str() );
          else
            Log::error( Say("Line %n: parse error.\n") << mTokenizer->lineNumber() );
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
                object->setUID(mToken.mString);

                // UID to Object Map, #NULL is not mapped to anything
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
          object->value().push_back( SRF_Object::Value() );
          SRF_Object::Value& name_value = object->value().back();

          // Key
          name_value.mKey = mToken.mString;

          // Equals
          if (!getToken(mToken) || mToken.mType != SRF_Token::Equals)
            return false;

          // Member value
          if (getToken(mToken))
          {
            // A new <Object>
            if (mToken.mType == SRF_Token::ObjectHeader)
            {
              ref<SRF_Object> object = new SRF_Object;
              object->setName(mToken.mString);
              name_value.mValue.setObject(object.get());
              if (!parseObject( object.get() ) )
                return false;
            }
            else
            // An [ list ]
            if (mToken.mType == SRF_Token::LeftSquareBracket)
            {
              ref<SRF_List> list = new SRF_List;
              name_value.mValue.setList(list.get());
              if ( !parseList( list.get() ) )
                return false;
            }
            else
            // An ( array )
            if (mToken.mType == SRF_Token::LeftRoundBracket)
            {
              ref<SRF_Array> arr;
              if ( parseArray( arr ) )
                name_value.mValue.setArray(arr.get());
              else
                return false;
            }
            else
            // A "string"
            if (mToken.mType == SRF_Token::String)
            {
              name_value.mValue.setString(mToken.mString.c_str());
            }
            else
            // An Identifier
            if (mToken.mType == SRF_Token::Identifier)
            {
              name_value.mValue.setIdentifier(mToken.mString.c_str());
            }
            else
            // An #id
            if (mToken.mType == SRF_Token::UID)
            {
              name_value.mValue.setUID(mToken.mString.c_str());
            }
            else
            // A boolean (true/false)
            if (mToken.mType == SRF_Token::Boolean)
            {
              name_value.mValue.setBool(mToken.mString == "true");
            }
            else
            // An integer
            if (mToken.mType == SRF_Token::Int32 || mToken.mType == SRF_Token::Int64)
            {
              name_value.mValue.setInt64( atoll(mToken.mString.c_str()) );
            }
            else
            // An float
            if (mToken.mType == SRF_Token::Float || mToken.mType == SRF_Token::Double)
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
        if (mToken.mType == SRF_Token::RightSquareBracket)
          return true;
        else
        {
          SRF_Value value;
          switch( mToken.mType )
          {
            // object
            case SRF_Token::ObjectHeader:
              {
                ref<SRF_Object> object = new SRF_Object;
                object->setName(mToken.mString);
                if ( parseObject( object.get() ) )
                {
                  value.setObject(object.get());
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
            arr_uid->mValue.push_back(mToken.mString);
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
          case SRF_Token::ObjectHeader:       printf("ObjectHeader = %s\n", mToken.mString.c_str()); break;
          default:
            break;
        }
      }
      if (mToken.mType != SRF_Token::TOKEN_EOF)
      {
        printf("Line %d: syntax error : '%s'.\n", mTokenizer->lineNumber(), mToken.mString.c_str());
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
        SRF_LinkVisitor linker(&mLinkMap);
        mRoot->acceptVisitor(&linker);
      }
    }
  
    SRF_Tokenizer* tokenizer() { return mTokenizer.get(); }

    const SRF_Tokenizer* tokenizer() const { return mTokenizer.get(); }

    const std::map< std::string, ref<SRF_Object> >& linkMap() const { return mLinkMap; }

  private:
    ref<SRF_Object> mRoot;
    std::map< std::string, ref<SRF_Object> > mLinkMap;
    ref<SRF_Tokenizer> mTokenizer;
    SRF_Token mToken;
  };
  //-----------------------------------------------------------------------------
}

#endif