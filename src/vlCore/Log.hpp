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

#include <vlCore/String.hpp>
#include <vlCore/IMutex.hpp>
#include <fstream>

namespace vl
{
  //------------------------------------------------------------------------------
  // Log
  //-----------------------------------------------------------------------------
  /** Utility class to generate logs. */
  class VLCORE_EXPORT Log: public Object
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
    virtual const char* className() { return "vl::Log"; }

    Log()
    {
      #ifndef NDEBUG
        mObjectName = className();
      #endif
    }

  protected:
    virtual void printImplementation(ELogLevel level, const String& message) = 0;

    // ---  static methods ---

  public:
    //! The mutex used to synchronize concurrent calls to the log functions.
    //! You should always install a log mutex when using VL in multi-threaded applications.
    static void setLogMutex(IMutex* mutex) { mLogMutex = mutex; }

    //! The mutex used to synchronize concurrent calls to the log functions.
    static IMutex* logMutex() { return mLogMutex; }

    /* Logs the specified message.
     * \note Log generated only if verbosity level != vl::VEL_VERBOSITY_SILENT */
    static void print(const String& message);
    
    /** Use this function to provide extra information useful to the end user.
      * The string "Info: " is prepended to the \p message. 
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_NORMAL */
    static void info(const String& message);

    /** Use this function to provide extra information useful to investigate and solve problems.
      * The string "Debug: " is prepended to the \p message. 
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_DEBUG */
    static void debug(const String& message);

    /** Use this function to provide information about situations that might lead to errors or loss of data.
      * The string "Warning: " is prepended to the \p message. 
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_ERROR */
    static void warning(const String& message);

    /** Use this function to provide information about run-time errors: file not found, out of memory, OpenGL version too old etc.
      * The string "Error: " is prepended to the \p message. 
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_ERROR */
    static void error(const String& message);

    /** Use this function to provide information about programming errors: wrong parameter initialization, division by zero, imminent crash, inconsistent program state etc.
      * The string "Bug: " is prepended to the \p message. 
      * \note Log generated only if verbosity level >= vl::VEL_VERBOSITY_ERROR */
    static void bug(const String& message);

    //! Logs VL and system information.
    static void logSystemInfo();

  private:
    static IMutex* mLogMutex;
  };

  //-----------------------------------------------------------------------------
  // Default logger
  //-----------------------------------------------------------------------------
  //! Installs the default logger used by Visualization Library. Setting this to NULL will disable logging.
  VLCORE_EXPORT void setDefLogger(Log* logger);

  //! Returns the currently installed default logger.
  VLCORE_EXPORT Log* defLogger();

  //-----------------------------------------------------------------------------
  // StandardLog
  //-----------------------------------------------------------------------------
  /** The StandardLog class outputs the log messages on the stdout device and optionally also on a specified file. */
  class VLCORE_EXPORT StandardLog: public Log
  {
  public:
    virtual const char* className() { return "vl::StandardLog"; }
    void setLogFile(const String& file);
    const String& logFile() const { return mLogFile; }

  protected:
    virtual void printImplementation(ELogLevel level, const String& message);
    String mLogFile;
    std::ofstream mFile;
  };
}

#endif
