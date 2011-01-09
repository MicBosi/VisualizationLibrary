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

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Quaternion.hpp>
#include <vlCore/version.hpp>
#include <cassert>

#if defined(IO_MODULE_JPG)
  #include <vlGraphics/vlJPG.hpp>
#endif
#if defined(IO_MODULE_PNG)
  #include <vlGraphics/vlPNG.hpp>
#endif
#if defined(IO_MODULE_TIFF)
  #include <vlGraphics/vlTIFF.hpp>
#endif
#if defined(IO_MODULE_TGA)
  #include <vlGraphics/vlTGA.hpp>
#endif
#if defined(IO_MODULE_DAT)
  #include <vlGraphics/vlDAT.hpp>
#endif
#if defined(IO_MODULE_DDS)
  #include <vlGraphics/vlDDS.hpp>
#endif
#if defined(IO_MODULE_BMP)
  #include <vlGraphics/vlBMP.hpp>
#endif
#if defined(IO_MODULE_3DS)
  #include <vlGraphics/vl3DS.hpp>
#endif
#if defined(IO_MODULE_OBJ)
  #include <vlGraphics/vlOBJ.hpp>
#endif
#if defined(IO_MODULE_AC3D)
  #include <vlGraphics/vlAC3D.hpp>
#endif
#if defined(IO_MODULE_PLY)
  #include <vlGraphics/vlPLY.hpp>
#endif
#if defined(IO_MODULE_STL)
  #include <vlGraphics/vlSTL.hpp>
#endif
#if defined(IO_MODULE_MD2)
  #include <vlGraphics/vlMD2.hpp>
#endif
#if defined(IO_MODULE_DICOM)
  #include <vlGraphics/vlDICOM.hpp>
#endif

using namespace vl;

//------------------------------------------------------------------------------
VL_COMPILE_TIME_CHECK( sizeof(double)    == 8 )
VL_COMPILE_TIME_CHECK( sizeof(float)     == 4 )
VL_COMPILE_TIME_CHECK( sizeof(long long) == 8 )
VL_COMPILE_TIME_CHECK( sizeof(int)       == 4 )
VL_COMPILE_TIME_CHECK( sizeof(short)     == 2 )
VL_COMPILE_TIME_CHECK( sizeof(char)      == 1 )
VL_COMPILE_TIME_CHECK( sizeof(wchar_t) >= 2 )
VL_COMPILE_TIME_CHECK( sizeof(vec2)      == sizeof(Real)*2 )
VL_COMPILE_TIME_CHECK( sizeof(vec3)      == sizeof(Real)*3 )
VL_COMPILE_TIME_CHECK( sizeof(vec4)      == sizeof(Real)*4 )
VL_COMPILE_TIME_CHECK( sizeof(mat2)      == sizeof(Real)*2*2 )
VL_COMPILE_TIME_CHECK( sizeof(mat3)      == sizeof(Real)*3*3 )
VL_COMPILE_TIME_CHECK( sizeof(mat4)      == sizeof(Real)*4*4 )
VL_COMPILE_TIME_CHECK( sizeof(quat)      == sizeof(Real)*4 )
VL_COMPILE_TIME_CHECK( sizeof(AABB)      == sizeof(Real)*6 )
VL_COMPILE_TIME_CHECK( sizeof(Sphere)    == sizeof(Real)*4 )
//------------------------------------------------------------------------------
namespace
{
  std::string gVersionString = "Visualization Library not initialized!";
  const char* gCertificate = "[Visualization Library BSD License]";
  bool gInitialized = false;
};
//------------------------------------------------------------------------------
bool VisualizationLibrary::initialized() { return gInitialized; }
//------------------------------------------------------------------------------
const char* VisualizationLibrary::versionString() { return gVersionString.c_str(); }
//------------------------------------------------------------------------------
void VisualizationLibrary::init()
{
  gVersionString = String( Say("%n.%n.%n") << VL_Major << VL_Minor << VL_Build ).toStdString();

  // install logger
  ref<StandardLog> logger = new StandardLog;
  logger->setLogFile( globalSettings()->defaultLogPath() );
  Log::setLogger( logger.get() );

  // log some information
  if (globalSettings()->verbosityLevel())
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

    Log::print( Say("Visualization Library v%n.%n.%n [%s]\n%s - %s - %s compiler [%s] [%s]\n") 
      << VL_Major << VL_Minor << VL_Build 
      << (sizeof(vec3) == sizeof(fvec3) ? "f32" : "f64")
      << __DATE__ << __TIME__ << compiler << build_type 
      << (sizeof(void*) == 4 ? "x32" : "x64") );

    Log::print("\n --- Environment ---\n");
    const char* val = getenv("VL_LOGFILE_PATH");
    if (val)
      Log::print( Say("VL_LOGFILE_PATH = %s\n") << val );
    else
      Log::print("VL_LOGFILE_PATH <not present>\n");

    val = getenv("VL_DATA_PATH");
    if (val)
      Log::print( Say("VL_DATA_PATH = %s\n") << val );
    else
      Log::print("VL_DATA_PATH <not present>\n");

    val = getenv("VL_VERBOSITY_LEVEL");
    if (val)
      Log::print( Say("VL_VERBOSITY_LEVEL = %s\n") << val );
    else
      Log::print("VL_VERBOSITY_LEVEL <not present>\n");

    val = getenv("VL_CHECK_GL_STATES");
    if (val)
      Log::print( Say("VL_CHECK_GL_STATES = %s\n") << val );
    else
      Log::print("VL_CHECK_GL_STATES <not present>\n");

    Log::print("\n --- Global Settings --- \n");
    Log::print( Say("Log file  = %s\n") << globalSettings()->defaultLogPath() );
    Log::print( Say("Data path = %s\n") << globalSettings()->defaultDataPath() );
    Log::print("Verbosity level = ");
    switch(globalSettings()->verbosityLevel())
    {
      /*case vl::VEL_VERBOSITY_SILENT: Log::print("SILENT\n"); break;*/
      case vl::VEL_VERBOSITY_ERROR:  Log::print("ERROR\n"); break;
      case vl::VEL_VERBOSITY_NORMAL: Log::print("NORMAL\n"); break;
      case vl::VEL_VERBOSITY_DEBUG:  Log::print("DEBUG\n"); break;
      default: break;
    }
    Log::print( Say("Check OpenGL States = %s\n") << (globalSettings()->checkOpenGLStates()?"YES":"NO") );
    Log::print( Say("Check Transform Siblings = %s\n") << (globalSettings()->checkTransformSiblings()?"YES":"NO") );

    Log::print("\n");
  }

  // register I/O plugins
  #if defined(IO_MODULE_JPG)
    registerLoadWriter(new LoadWriterJPG);
  #endif
  #if defined(IO_MODULE_PNG)
    registerLoadWriter(new LoadWriterPNG);
  #endif
  #if defined(IO_MODULE_TIFF)
    registerLoadWriter(new LoadWriterTIFF);
  #endif
  #if defined(IO_MODULE_TGA)
    registerLoadWriter(new LoadWriterTGA);
  #endif
  #if defined(IO_MODULE_BMP)
    registerLoadWriter(new LoadWriterBMP);
  #endif
  #if defined(IO_MODULE_DDS)
    registerLoadWriter(new LoadWriterDDS);
  #endif
  #if defined(IO_MODULE_DAT)
    registerLoadWriter(new LoadWriterDAT);
  #endif
  #if defined(IO_MODULE_OBJ)
    registerLoadWriter(new LoadWriterOBJ);
  #endif
  #if defined(IO_MODULE_3DS)
    registerLoadWriter(new LoadWriter3DS);
  #endif
  #if defined(IO_MODULE_AC3D)
    registerLoadWriter(new LoadWriterAC3D);
  #endif
  #if defined(IO_MODULE_PLY)
    registerLoadWriter(new LoadWriterPLY);
  #endif
  #if defined(IO_MODULE_STL)
    registerLoadWriter(new LoadWriterSTL);
  #endif
  #if defined(IO_MODULE_MD2)
    registerLoadWriter(new LoadWriterMD2);
  #endif
  #if defined(IO_MODULE_DICOM)
    registerLoadWriter(new LoadWriterDICOM);
  #endif

  // initialized = on
  gInitialized = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdown()
{
  // initialized = off
  gInitialized = false;

  if (globalSettings()->verbosityLevel())
  {
    Log::print("Visualization Library shutdown.\n");
  }

  Log::setLogger( NULL );
}
//------------------------------------------------------------------------------
#if defined(_WIN32)
  // console includes
  #include <stdio.h>
  #include <io.h>
  #include <fcntl.h>
  #include <commdlg.h>
#endif

void vl::showWin32Console()
{
  #if defined(_WIN32)
    AllocConsole();
    // stdout
    HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
    int hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
    FILE* hf_out = _fdopen(hCrt, "w"); 
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stdout = *hf_out;
    // stderr
    handle_out = GetStdHandle(STD_ERROR_HANDLE);
    hCrt = _open_osfhandle((intptr_t)handle_out, _O_TEXT);
    hf_out = _fdopen(hCrt, "w");
    setvbuf(hf_out, NULL, _IONBF, 1);
    *stderr = *hf_out;
  #endif
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
//------------------------------------------------------------------------------
