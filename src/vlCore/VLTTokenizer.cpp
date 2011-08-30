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

#include <vlCore/VLTTokenizer.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <ctime>

using namespace vl;

bool VLTTokenizer::getToken(VLTToken& token)
{
  token.mType = VLTToken::TOKEN_ERROR;
  token.mString.clear();

  if (mRawtextBlock)
    return getRawtextBlock(token);

  // read chars skipping spaces
  char ch1=0, ch2=0;
  do 
  {
    if (!readTextChar(ch1))
    {
      token.mType = VLTToken::TOKEN_EOF;
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
          Log::error( Say("Line %n : unexpected character '%c' after '/'.\n") << mLineNumber << ch2 );
          return false;
        }
      }
      else
      {
        Log::error( Say("Line %n : unexpected end of file in comment.\n") << mLineNumber);
        return false;
      }
      continue;
    }

  } while(ch1 == ' ' || ch1 == '\t' || ch1 == '\n');

  switch(ch1)
  {
  case '(':
    token.mType = VLTToken::LeftRoundBracket;
    token.mString = "(";
    return true;
    
  case ')':
    token.mType = VLTToken::RightRoundBracket;
    token.mString = ")";
    return true;

  case '[':
    token.mType = VLTToken::LeftSquareBracket;
    token.mString = "[";
    return true;
    
  case ']':
    token.mType = VLTToken::RightSquareBracket;
    token.mString = "]";
    return true;

  case '{':
    if(readTextChar(ch2) && ch2 == '<')
    {
      // actual data starts at the next new line
      // eat all the spaces until the end of the current line
      while(ch2 != '\n' && readTextChar(ch2)) 
      { 
        switch(ch2)
        {
        case '\f':
        case '\b':
        case '\v':
        case '\t':
        case ' ':
          continue;

        case '\n':
          ++mLineNumber;
          break;

        default:
          token.mString = ch2;
          return false;
        }
      }

      if (ch2 == '\n')
      {
        token.mType = VLTToken::LeftFancyBracket;
        token.mString = "{<";
        mRawtextBlock = true;
        return true;
      }
      else
      {
        token.mString = ch2;
        return false;
      }
    }
    else
    {
      token.mType = VLTToken::LeftCurlyBracket;
      token.mString = "{";
      if(!isEndOfFile())
        ungetToken(ch2);
    }
    return true;

  case '}':
    token.mType = VLTToken::RightCurlyBracket;
    token.mString = "}";
    return true;

  case '>':
    if(readTextChar(ch2))
    {
      if(ch2 == '}')
      {
        token.mType = VLTToken::RightFancyBracket;
        token.mString = ">}";
        return true;
      }
      else
      {
        Log::error( Say("Line %n : expected '}' instead of '%c' after '>'.\n") << mLineNumber << ch2 );
        return false;
      }
    }
    else
    {
        Log::error( Say("Line %n : unexpected end of file.\n") << mLineNumber );
        return false;
    }

  case '=':
    token.mType = VLTToken::Equals; 
    token.mString = "=";
    return true;

  case '<':
    token.mString = "<";
    while(readTextChar(ch1) && ch1 != '>')
    {
      if ( (ch1 >= 'a' && ch1 <= 'z') || (ch1 >= 'A' && ch1 <= 'Z') || (ch1 >= '0' && ch1 <= '9') || ch1 == '_' || ch1 == ':' )
        token.mString.push_back(ch1);
      else
      {
        Log::error( Say("Line %n : unexpected character '%c'.\n") << mLineNumber << ch1 );
        return false;
      }
    }
    token.mString.push_back('>');
    if (isEndOfFile())
    {
      Log::error( Say("Line %n : unexpected end of file while reading object header.\n") << mLineNumber );
      return false;
    }
    token.mType = VLTToken::TagHeader;
    return true;

  case '#':
    token.mString = "#";
    while(readTextChar(ch1))
    {
      if ( (ch1 >= 'a' && ch1 <= 'z') || (ch1 >= 'A' && ch1 <= 'Z') || (ch1 >= '0' && ch1 <= '9') || ch1 == '_' )
        token.mString.push_back(ch1);
      else
      {
        ungetToken(ch1);
        break;
      }
    }
    if (token.mString == "#_")
    {
      Log::error( Say("Line %n : illegal id '#_' found.\n") << mLineNumber );
      return false;
    }
    token.mType = VLTToken::ID;
    return true;

  case '"':
    while(readTextChar(ch1))
    {
      // end string
      if (ch1 == '"')
        break;
      else
      // return found before end of string
      if (ch1 == '\n')
      {
        Log::error( Say("Line %n : end of line found before end of string, did you forget a \"?.\n") << mLineNumber );
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
      Log::error( Say("Line %n : end of file found before end of string, did you forget a \"?.\n") << mLineNumber );
      return false;
    }
    else
    {
      token.mType = VLTToken::String;
      return true;
    }

  default:
    // identifier
    if ( (ch1 >= 'a' && ch1 <= 'z') || (ch1 >= 'A' && ch1 <= 'Z') || ch1 == '_' )
    {
      token.mString.push_back(ch1);
      while(readTextChar(ch1))
      {
        if ( (ch1 >= 'a' && ch1 <= 'z') || (ch1 >= 'A' && ch1 <= 'Z') || (ch1 >= '0' && ch1 <= '9') || ch1 == '_' )
          token.mString.push_back(ch1);
        else
        {
          ungetToken(ch1);
          break;
        }
      }
      if (token.mString == "_")
      {
        Log::error( Say("Line %n : unexpected character '_'.\n") << mLineNumber );
        return false;
      }
      else
      {
        // check if it's a boolean
        if (token.mString == "true" || token.mString == "false")
          token.mType = VLTToken::Boolean;
        else
          token.mType = VLTToken::Identifier;
        return true;
      }
    }
    else
    // Integer / Real
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
    if ( (ch1 >= '0' && ch1 <= '9') || ch1 == '.' || ch1 == '+' || ch1 == '-' )
    {
      token.mType = VLTToken::TOKEN_ERROR;
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
            token.mType = VLTToken::Integer;
            ungetToken(ch1);
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
            Log::error( Say("Line %n :unexpected character '%c'.\n") << mLineNumber << ch1 );
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
            token.mType = VLTToken::Integer;
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
            Log::error( Say("Line %n :unexpected character '%c'.\n") << mLineNumber << ch1 );
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
            token.mType = VLTToken::Real;
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
            Log::error( Say("Line %n :unexpected character '%c'.\n") << mLineNumber << ch1 );
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
            Log::error( Say("Line %n :unexpected character '%c'.\n") << mLineNumber << ch1 );
            return false;
          }
          break;

        case sEXP:
          if (ch1 >= '0' && ch1 <= '9')
            token.mString.push_back(ch1);
          else
          {
            token.mType = VLTToken::Real;
            ungetToken(ch1);
            return true;
          }
          break;
        }
      }
      // reached TOKEN_EOF in the middle of the parsing so we check where we were, note that it cannot be a Integer or a Real
      if (state == sINT)
      {
        token.mType = VLTToken::Integer;
        return true;
      }
      else
      if (state == sFRAC || state == sEXP)
      {
        token.mType = VLTToken::Real;
        return true;
      }
      else
        return false;
    }
    else
    {
      Log::error( Say("Line %n : unexpected character '%c'.\n") << mLineNumber << ch1 );
      return false;
    }
  }
}
//-----------------------------------------------------------------------------
bool VLTTokenizer::getRawtextBlock(VLTToken& token)
{
  mRawtextBlock = false;

  token.mString.clear();
  token.mType = VLTToken::TOKEN_ERROR;

  char ch =0;
  while(readTextChar(ch))
  {
    if (ch == '\n')
      ++mLineNumber;

    if (ch == '>')
    {
      // check for rawtext block end >}
      char ch2 = 0;
      if (readTextChar(ch2))
      {
        if(ch2 == '}')
        {
          // check if it was escaped
          if (!token.mString.empty() && token.mString[ token.mString.size() - 1 ] == '\\')
          {
            token.mString.resize( token.mString.size() - 1 );
            token.mString += ">}";
            continue;
          }
          else
          {
            token.mType = VLTToken::RawtextBlock;
            ungetToken('}');
            ungetToken('>');
            return true;
          }
        }
        else
          ungetToken(ch2);
      }
    }
    
    token.mString.push_back(ch);
  }

  return false;
}
//-----------------------------------------------------------------------------
