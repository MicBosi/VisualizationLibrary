/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
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

#ifndef Log_INCLUDE_ONCE
#define Log_INCLUDE_ONCE

#include <vl/String.hpp>

namespace vl
{
  //------------------------------------------------------------------------------
  typedef enum 
  { 
    LogNormal,  //!< Default logging level.
    LogDebug,   //!< Information useful for the programmer.
    LogInfo,    //!< Information userful for the end user.
    LogWarning, //!< Information about situations that might lead to errors.
    LogError,   //!< Information about a run-time error: file not found, out of memory etc.
    LogBug      //!< Information about a programming error: wrong parameter initialization, division by zero etc.
  } ELogLevel;
  //-----------------------------------------------------------------------------
  // Log: a simple class to manage logs
  //-----------------------------------------------------------------------------
  /**
   * The Log class is the abstract base class used to generate logging data.
  */
  class Log: public Object
  {
  public:
    virtual const char* className() { return "Log"; }
    Log()
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

    static void setLogger(Log* logger) { mLogger = logger; }
    static Log* logger() { return mLogger.get(); }

    static void print(ELogLevel level, const String& log);
    static void print(const String& log);
    static void debug(const String& log);
    static void info(const String& log);
    static void warning(const String& log);
    static void error(const String& log);
    static void bug(const String& log);

  protected:
    static ref<Log> mLogger;
    virtual void printImplementation(ELogLevel level, const String& log) = 0;
  };
  //-----------------------------------------------------------------------------
  // StandardLog
  //-----------------------------------------------------------------------------
  /**
   * The StandardLog class outputs the log messages on the stdout device and optionally also on a specified file.
  */
  class StandardLog: public Log
  {
  public:
    virtual const char* className() { return "StandardLog"; }
    void setLogFile(const String& file) { mLogFile = file; }
    const String& logFile() const { return mLogFile; }
  protected:
    virtual void printImplementation(ELogLevel level, const String& log);
    String mLogFile;
  };
}

#endif
