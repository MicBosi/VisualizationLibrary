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

#include <vlCore/Log.hpp>
#include <vlCore/checks.hpp>
#include <vlCore/VLSettings.hpp>
#include <vlCore/Vector3.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/version.hpp>
#include <iostream>
#include <cstdlib>

using namespace vl;

namespace
{
#ifdef _WIN32
  struct ScopedColor
  {
    CONSOLE_SCREEN_BUFFER_INFO screen_info;
    WORD color;
    ScopedColor(WORD c): color(c)
    {
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      GetConsoleScreenBufferInfo(
        hConsole,
        &screen_info
      );
      SetConsoleTextAttribute(hConsole, c);
    }
    ~ScopedColor()
    {
      // restore the color
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      SetConsoleTextAttribute(hConsole,screen_info.wAttributes);  
    }
  };
  #define SET_TEXT_COLOR(color) ScopedColor set_scoped_color(color);
#else
  #define SET_TEXT_COLOR(color) ScopedColor(color);
#endif
}

//-----------------------------------------------------------------------------
// Log
//-----------------------------------------------------------------------------
void Log::print(const String& log) 
{ 
  if(defLogger() && globalSettings()->verbosityLevel() != vl::VEL_VERBOSITY_SILENT)
    defLogger()->printImplementation(LogNormal, log); 
}
//-----------------------------------------------------------------------------
void Log::debug(const String& log) 
{ 
  if(defLogger() && globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_DEBUG)
    defLogger()->printImplementation(LogDebug, log); 
}
//-----------------------------------------------------------------------------
void Log::info(const String& log) 
{ 
  if(defLogger() && globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_NORMAL)
    defLogger()->printImplementation(LogInfo, log); 
}
//-----------------------------------------------------------------------------
void Log::warning(const String& log) 
{ 
  SET_TEXT_COLOR(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY)
  if(defLogger() && globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_ERROR)
    defLogger()->printImplementation(LogWarning, log); 
}
//-----------------------------------------------------------------------------
void Log::error(const String& log) 
{ 
  SET_TEXT_COLOR(FOREGROUND_RED|FOREGROUND_INTENSITY)
  if(defLogger() && globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_ERROR)
    defLogger()->printImplementation(LogError, log); 
}
//-----------------------------------------------------------------------------
void Log::bug(const String& log) 
{ 
  SET_TEXT_COLOR(FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_INTENSITY)
  if(defLogger() && globalSettings()->verbosityLevel() >= vl::VEL_VERBOSITY_ERROR)
    defLogger()->printImplementation(LogBug, log); 
}
//------------------------------------------------------------------------------
void Log::logSystemInfo()
{
  #if defined(_MSC_VER)
    const char* compiler = "MSVC";
  #elif defined(__GNUG__)
    const char* compiler = "GCC";
  #else
    const char* compiler = "UNKNOWN";
  #endif

  #if defined(DEBUG) || !defined(NDEBUG)
    const char* build_type = "DEBUG";
  #else
    const char* build_type = "RELEASE";
  #endif

  print( Say("Visualization Library v%n.%n.%n [%s]\n%s - %s - %s compiler [%s] [%s]\n") 
    << VL_Major << VL_Minor << VL_Build 
    << (sizeof(vec3) == sizeof(fvec3) ? "f32" : "f64")
    << __DATE__ << __TIME__ << compiler << build_type 
    << (sizeof(void*) == 4 ? "x32" : "x64") );

  print("\n --- Environment ---\n");
  const char* val = getenv("VL_LOGFILE_PATH");
  if (val)
    print( Say("VL_LOGFILE_PATH = %s\n") << val );
  else
    print("VL_LOGFILE_PATH <not present>\n");

  val = getenv("VL_DATA_PATH");
  if (val)
    print( Say("VL_DATA_PATH = %s\n") << val );
  else
    print("VL_DATA_PATH <not present>\n");

  val = getenv("VL_VERBOSITY_LEVEL");
  if (val)
    print( Say("VL_VERBOSITY_LEVEL = %s\n") << val );
  else
    print("VL_VERBOSITY_LEVEL <not present>\n");

  val = getenv("VL_CHECK_GL_STATES");
  if (val)
    print( Say("VL_CHECK_GL_STATES = %s\n") << val );
  else
    print("VL_CHECK_GL_STATES <not present>\n");

  print("\n --- Global Settings --- \n");
  print( Say("Log file  = %s\n") << globalSettings()->defaultLogPath() );
  print( Say("Data path = %s\n") << globalSettings()->defaultDataPath() );
  print("Verbosity level = ");
  switch(globalSettings()->verbosityLevel())
  {
    /*case vl::VEL_VERBOSITY_SILENT: print("SILENT\n"); break;*/
    case vl::VEL_VERBOSITY_ERROR:  print("ERROR\n"); break;
    case vl::VEL_VERBOSITY_NORMAL: print("NORMAL\n"); break;
    case vl::VEL_VERBOSITY_DEBUG:  print("DEBUG\n"); break;
    default: break;
  }
  print( Say("Check OpenGL States = %s\n") << (globalSettings()->checkOpenGLStates()?"YES":"NO") );
  print( Say("Check Transform Siblings = %s\n") << (globalSettings()->checkTransformSiblings()?"YES":"NO") );

  print("\n");
}
//------------------------------------------------------------------------------
void vl::log_failed_check(const char* expr, const char* file, int line)
{
  Log::error( Say("Condition \"%s\" failed at %s:%n\n") << expr << file << line );
  fflush(stdout);
  fflush(stderr);

  #if _WIN32 && VL_MESSAGEBOX_CHECK == 1
     String msg = Say("Condition \"%s\" failed.\n\n%s:%n\n") << expr << file << line;
     MessageBox(NULL, (wchar_t*)msg.ptr(), L"Visualization Library Debug", MB_OK | MB_ICONEXCLAMATION);
  #endif
}
//-----------------------------------------------------------------------------
// StandardLog
//-----------------------------------------------------------------------------
void StandardLog::setLogFile(const String& file) 
{ 
  mLogFile = file; 

  if (mFile.is_open())
    mFile.close();

  if (!file.empty())
    mFile.open(file.toStdString().c_str());
}
//-----------------------------------------------------------------------------
void StandardLog::printImplementation(ELogLevel, const String& log)
{
  if (log.empty())
    return;

  std::string stdstr = log.toStdString();
  std::cout << stdstr << std::flush;

  if (mFile.is_open())
    mFile << stdstr << std::flush;
}
//-----------------------------------------------------------------------------
