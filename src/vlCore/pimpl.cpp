/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2020, Michele Bosi                                             */
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

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/GlobalSettings.hpp>
#include <vlX/Registry.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Time.hpp>
#include <vlCore/Quaternion.hpp>
#include <vlCore/AABB.hpp>
#include <vlCore/Sphere.hpp>
#include <vlCore/MersenneTwister.hpp>
#include <cassert>

using namespace vl;

namespace
{
  ref<GlobalSettings> gSettings = NULL;
}
GlobalSettings* vl::globalSettings()
{
  return gSettings.get();
}
void vl::setGlobalSettings(GlobalSettings* gs)
{
  gSettings = gs;
}
//------------------------------------------------------------------------------
// Default logger
//------------------------------------------------------------------------------
namespace
{
  ref<Log> gDefaultLogger;
}
void vl::setDefLogger(Log* logger)
{
  gDefaultLogger = logger;
}
Log* vl::defLogger()
{
  return gDefaultLogger.get();
}
//------------------------------------------------------------------------------
// Default LoadWriterManager
//------------------------------------------------------------------------------
namespace
{
  ref<LoadWriterManager> gDefaultLoadWriterManager = NULL;
}
LoadWriterManager* vl::defLoadWriterManager()
{
  return gDefaultLoadWriterManager.get();
}
void vl::setDefLoadWriterManager(LoadWriterManager* lwm)
{
  gDefaultLoadWriterManager = lwm;
}
//-----------------------------------------------------------------------------
// Default FileSystem
//-----------------------------------------------------------------------------
namespace
{
  ref<FileSystem> gDefaultFileSystem = NULL;;
}
FileSystem* vl::defFileSystem()
{
  return gDefaultFileSystem.get();
}
void vl::setDefFileSystem(FileSystem* fs)
{
  gDefaultFileSystem = fs;
}
//-----------------------------------------------------------------------------
// Default MersenneTwister
//-----------------------------------------------------------------------------
namespace
{
  ref<MersenneTwister> gDefaultMersenneTwister = NULL;
}
MersenneTwister* vl::defMersenneTwister()
{
  return gDefaultMersenneTwister.get();
}
void vl::setDefMersenneTwister(MersenneTwister* reg)
{
  gDefaultMersenneTwister= reg;
}
//------------------------------------------------------------------------------
void vl::abort_vl()
{
  vl::Time::sleep(3000);
  exit(1);
}
// ------------------------------------------------------------------------------
namespace
{
  std::string gVersionString = String( Say("%n.%n.%s") << VL_Major << VL_Minor << VL_Patch ).toStdString();
};
// ------------------------------------------------------------------------------
const char* vl::versionString() { return gVersionString.c_str(); }
//------------------------------------------------------------------------------
#if defined(VL_PLATFORM_WINDOWS)
  // console includes
  #include <io.h>
  #include <fcntl.h>
#endif
void vl::showWin32Console()
{
  #if defined(VL_PLATFORM_WINDOWS)
    if (AllocConsole() == 0)
        return;
    FILE* f_new_stdout = nullptr;
    FILE* f_new_stderr = nullptr;
    FILE* f_new_stdin = nullptr;
    ::freopen_s(&f_new_stdout, "CONOUT$", "w", stdout);
    ::freopen_s(&f_new_stderr, "CONOUT$", "w", stderr);
    ::freopen_s(&f_new_stdin, "CONIN$", "r", stdin);
    std::cout.clear();
    std::cerr.clear();
    std::cin.clear();
    std::wcout.clear();
    std::wcerr.clear();
    std::wcin.clear();
  #endif
}
//------------------------------------------------------------------------------
void vl::logSystemInfo()
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

  vl::Log::print( Say("Visualization Library v%s [%s]\n%s - %s - %s compiler [%s] [%s]\n")
	<< vl::versionString()
    << (sizeof(vec3) == sizeof(fvec3) ? "f32" : "f64")
    << __DATE__ << __TIME__ << compiler << build_type
    << (sizeof(void*) == 4 ? "x32" : "x64") );

  vl::Log::print("\n --- Environment ---\n");
  const char* val = getenv("VL_LOGFILE_PATH");
  if (val)
    vl::Log::print( Say("VL_LOGFILE_PATH = %s\n") << val );
  else
    vl::Log::print("VL_LOGFILE_PATH <not present>\n");

  val = getenv("VL_DATA_PATH");
  if (val)
    vl::Log::print( Say("VL_DATA_PATH = %s\n") << val );
  else
    vl::Log::print("VL_DATA_PATH <not present>\n");

  val = getenv("VL_VERBOSITY_LEVEL");
  if (val)
    vl::Log::print( Say("VL_VERBOSITY_LEVEL = %s\n") << val );
  else
    vl::Log::print("VL_VERBOSITY_LEVEL <not present>\n");

  val = getenv("VL_CHECK_GL_STATES");
  if (val)
    vl::Log::print( Say("VL_CHECK_GL_STATES = %s\n") << val );
  else
    vl::Log::print("VL_CHECK_GL_STATES <not present>\n");

  vl::Log::print("\n --- Global Settings --- \n");
  vl::Log::print( Say("Log file  = %s\n") << globalSettings()->defaultLogPath() );
  vl::Log::print( Say("Data path = %s\n") << globalSettings()->defaultDataPath() );
  vl::Log::print("Verbosity level = ");
  switch(globalSettings()->verbosityLevel())
  {
    /*case vl::VEL_VERBOSITY_SILENT: print("SILENT\n"); break;*/
    case vl::VEL_VERBOSITY_ERROR:  vl::Log::print("ERROR\n"); break;
    case vl::VEL_VERBOSITY_NORMAL: vl::Log::print("NORMAL\n"); break;
    case vl::VEL_VERBOSITY_DEBUG:  vl::Log::print("DEBUG\n"); break;
    default: break;
  }
  vl::Log::print( Say("Check OpenGL States = %s\n") << (globalSettings()->checkOpenGLStates()?"YES":"NO") );

  vl::Log::print("\n");
}