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

typedef enum
{
  TT_ERROR_TOKEN,
  TT_EOF,

  TT_LeftSquareBracket, // [
  TT_RightSquareBracket, // ]
  TT_LeftCurlyBracket, // {
  TT_RightCurlyBracket, // }
  TT_LeftFancyBracket, // {<
  TT_RightFancyBracket, // >}
  TT_Equals, // =
  TT_String, // "ciccio \npippo]!!>" -> 'C' escape sequences are escaped
  TT_UID, // #Identifier_type001
  TT_Identifier, // Identifier_1001
  TT_Number, // +123.456e+10
  TT_ObjectHeader // <ObjectHeader>

} ETokenType;

class Token
{
public:
  Token(): mType(TT_ERROR_TOKEN) {}
  std::string mString;
  ETokenType mType;
};

class Tokenizer: public BufferedStream<char, 128*1024>
{
public:
  Tokenizer()
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

  bool getToken(Token& token) 
  {
    token.mType = TT_ERROR_TOKEN;
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
    case '[':
      token.mType = TT_LeftSquareBracket; 
      return true;
    
    case ']':
      token.mType = TT_RightSquareBracket; 
      return true;

    case '{':
      if(readTextChar(ch2) && ch2 == '<')
      {
        token.mType = TT_LeftFancyBracket;
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
        if(!isEndOfFile())
          ungetToken(ch2);
      }
      return true;

    case '}':
      token.mType = TT_RightCurlyBracket;
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
      return true;

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
          return true;
      }
      else
      // TT_Number - int, float and exponent
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
        token.mType = TT_Number;
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
              Log::error( Say("Line %n:unexpected character '%c'.\n") << mLineNumber << ch1 );
              return false;
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
              ungetToken(ch1);
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
              ungetToken(ch1);
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
              ungetToken(ch1);
              return true;
            }
            break;
          }
        }
        return state == sINT || state == sFRAC || state == sEXP;
      }
      else
      {
        Log::error( Say("Line %n:unexpected character '%c'.\n") << mLineNumber << ch1 );
        return false;
      }
    }
  }

  // mic fixme
  int mLineNumber;
};

struct SRF_NameValue
{
  std::string mName;
  ref<Object> mValue;
};

struct SRF_String: public Object
{
  SRF_String() {}
  SRF_String(const std::string& str): mValue(str) {}
  std::string mValue;
};

struct SRF_Scalar: public Object
{
  SRF_Scalar(const std::string& str): mValue(str) {}
  std::string mValue;
};

struct SRF_ID: public Object
{
  SRF_ID(const std::string& str): mValue(str) {}
  std::string mValue;
};

struct SRF_Object: public Object
{
  SRF_Object()
  {
    mNameValues.reserve(32);
  }

  std::string mName;
  std::string mID;
  std::vector<SRF_NameValue> mNameValues;
};

struct SRF_Array: public Object
{
  SRF_Array()
  {
    mValues.reserve(32);
  }

  std::vector< ref<Object> > mValues;
  std::string mValuesSimple; // array values as a simple string when no objects are encountered.
};

class Parser
{
public:

  bool getToken(Token& token) { return mTokenizer->getToken(token); }

  bool parse()
  {
    if(getToken(mToken) && mToken.mType == TT_ObjectHeader)
    {
      ref<SRF_Object> object = new SRF_Object;
      object->mName = mToken.mString;
      if (parseObject(object.get()))
        return true;
      else
      {
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
        mObjects.push_back(object);
        return true;
      }
      else
      if (mToken.mType == TT_Identifier)
      {
        object->mNameValues.resize( object->mNameValues.size() + 1 );
        SRF_NameValue& name_value = object->mNameValues.back();

        // Member name
        name_value.mName = mToken.mString;

        // = 
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
            if (!parseObject(object.get()))
              return false;
          }
          else
          // An [ array ]
          if (mToken.mType == TT_LeftSquareBracket)
          {
            ref<SRF_Array> arr = new SRF_Array;
            name_value.mValue = arr;
            if (!parseArray(arr.get()))
              return false;
          }
          else
          // An #id
          if (mToken.mType == TT_UID)
          {
            name_value.mValue = new SRF_ID(mToken.mString);
          }
          else
          // A "string"
          if (mToken.mType == TT_String)
          {
            name_value.mValue = new SRF_String(mToken.mString);
          }
          else
          // Get all the chars till the end of the line, allowing for \ line continuation.
          {
            ref<SRF_String> srf_str = new SRF_String;
            name_value.mValue = srf_str;
            if (!mTokenizer->getLine(srf_str->mValue))
              return false;
          }
        }
      }
    }
    return false;
  }

  bool parseArray(SRF_Array* arr)
  {
    while(getToken(mToken))
    {
      if (mToken.mType == TT_RightSquareBracket)
        return true;
      else
      {
        switch( mToken.mType )
        {
          case TT_String:
            arr->mValues.push_back( new SRF_String(mToken.mString) );
            break;

          case TT_Identifier:
            arr->mValues.push_back( new SRF_Scalar(mToken.mString) );
            break;

          // mic fixme
          // this can be later replaced with a SRF_Object
          case TT_UID:
            arr->mValues.push_back( new SRF_ID(mToken.mString) );
            break;

          case TT_ObjectHeader:
          {
          ref<SRF_Object> object = new SRF_Object;
          object->mName = mToken.mString;
          if (parseObject(object.get()))
            arr->mValues.push_back( object );
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

  void listTokens()
  {
    while(getToken(mToken) && mToken.mType != TT_EOF)
    {
      switch(mToken.mType)
      {
        case TT_LeftSquareBracket:  printf("TT_LeftSquareBracket [\n"); break;
        case TT_RightSquareBracket: printf("TT_RightSquareBracket ]\n"); break;
        case TT_LeftCurlyBracket:   printf("TT_LeftCurlyBracket {\n"); break;
        case TT_RightCurlyBracket:  printf("TT_RightCurlyBracket } \n"); break;
        case TT_LeftFancyBracket:   printf("TT_LeftFancyBracket >}\n"); break;
        case TT_RightFancyBracket:  printf("TT_RightFancyBracket {< \n"); break;
        case TT_Equals:             printf("TT_Equals =\n"); break;
        case TT_String:             printf("TT_String = %s\n", mToken.mString.c_str()); break;
        case TT_UID:                printf("TT_UID = %s\n", mToken.mString.c_str()); break;
        case TT_Identifier:         printf("TT_Identifier = %s\n", mToken.mString.c_str()); break;
        case TT_Number:             printf("TT_Number = %s\n", mToken.mString.c_str()); break;
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

  std::vector< ref<SRF_Object> > mObjects;
  ref<Tokenizer> mTokenizer;
  Token mToken;
};

class App_VLSRF: public BaseDemo
{
public:
  virtual void initEvent()
  {
    Log::notify(appletInfo());

    Parser parser;
    parser.mTokenizer = new Tokenizer;
    parser.mTokenizer->setInputFile( new DiskFile("D:/VL/test.vl") );

    // parser.parse();
    parser.listTokens();

    int a = 0;
    Time::sleep(1000*5);
    exit(0);
  }

protected:
};

// Have fun!

BaseDemo* Create_App_VLSRF() { return new App_VLSRF; }
