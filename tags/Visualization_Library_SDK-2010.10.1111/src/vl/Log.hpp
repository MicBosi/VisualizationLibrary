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
  // Log
  //-----------------------------------------------------------------------------
  /** Utility class to generate logs. */
  class Log: public Object
  {
  protected:
    typedef enum 
    { 
      LogBug,
      LogError,
      LogWarning,
      LogNormal,
      LogInfo,
      LogDebug,
    } ELogLevel;

  public:
    virtual const char* className() { return "Log"; }
    Log()
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

  protected:
    virtual void printImplementation(ELogLevel level, const String& message) = 0;

    // ---  statics ---

  public:
    /** Installs a new logger. Set this to NULL to disable logging. */
    static void setLogger(Log* logger) { mLogger = logger; }

    /** Returns the currently installed logger. */
    static Log* logger() { return mLogger.get(); }

    /** Logs the specified string regardless of the current verbosity level. */
    static void force_print(const String& message);
    
    /* Logs the specified message.
     * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_NORMAL */
    static void print(const String& message);
    
    /** The string "info: " is prepended to the \p message. 
      * Use this function to provide extra information useful to the end user.
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_NORMAL */
    static void info(const String& message);
    
    /** The string "debug: " is prepended to the \p message. 
      * Use this function to provide information information useful to the programmer.
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_DEBUG */
    static void debug(const String& message);
    
    /** The string "warning: " is prepended to the \p message. 
      * Use this function to provide information about situations that might lead to errors or loss of data.
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_ERROR */
    static void warning(const String& message);
    
    /** The string "error: " is prepended to the \p message. 
      * Use this function to provide information about a run-time error: file not found, out of memory etc.
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_ERROR */
    static void error(const String& message);
    
    /** The string "bug: " is prepended to the \p message. 
      * Use this function to provide information about a critical programming error: wrong parameter initialization, division by zero, imminent crash etc.
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_ERROR */
    static void bug(const String& message);

  protected:
    static void print(ELogLevel level, const String& message);
    static ref<Log> mLogger;
  };
  //-----------------------------------------------------------------------------
  // StandardLog
  //-----------------------------------------------------------------------------
  /** The StandardLog class outputs the log messages on the stdout device and optionally also on a specified file. */
  class StandardLog: public Log
  {
  public:
    virtual const char* className() { return "StandardLog"; }
    void setLogFile(const String& file) { mLogFile = file; }
    const String& logFile() const { return mLogFile; }
  protected:
    virtual void printImplementation(ELogLevel level, const String& message);
    String mLogFile;
  };
}

#endif
