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
#include <vlCore/VLSettings.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Quaternion.hpp>
#include <vlCore/AABB.hpp>
#include <vlCore/Sphere.hpp>
#include <vlCore/version.hpp>
#include <cassert>

using namespace vl;

#if defined(VL_IO_2D_JPG)
  #include "plugins/vlJPG.hpp"
#endif
#if defined(VL_IO_2D_PNG)
  #include "plugins/vlPNG.hpp"
#endif
#if defined(VL_IO_2D_TIFF)
  #include "plugins/vlTIFF.hpp"
#endif
#if defined(VL_IO_2D_TGA)
  #include "plugins/vlTGA.hpp"
#endif
#if defined(VL_IO_2D_DAT)
  #include "plugins/vlDAT.hpp"
#endif
#if defined(VL_IO_2D_DDS)
  #include "plugins/vlDDS.hpp"
#endif
#if defined(VL_IO_2D_BMP)
  #include "plugins/vlBMP.hpp"
#endif
#if defined(VL_IO_2D_DICOM)
  #include "plugins/vlDICOM.hpp"
#endif

//------------------------------------------------------------------------------
VL_COMPILE_TIME_CHECK( sizeof(double)    == 8 )
VL_COMPILE_TIME_CHECK( sizeof(float)     == 4 )
VL_COMPILE_TIME_CHECK( sizeof(long long) == 8 )
VL_COMPILE_TIME_CHECK( sizeof(int)       == 4 )
VL_COMPILE_TIME_CHECK( sizeof(short)     == 2 )
VL_COMPILE_TIME_CHECK( sizeof(char)      == 1 )
VL_COMPILE_TIME_CHECK( sizeof(wchar_t)   >= 2 )
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
// VL misc
//------------------------------------------------------------------------------
namespace
{
  std::string gVersionString = String( Say("%n.%n.%n") << VL_Major << VL_Minor << VL_Build ).toStdString();
  bool gInitializedCore = false;
};
//------------------------------------------------------------------------------
bool VisualizationLibrary::isCoreInitialized() { return gInitializedCore; }
//------------------------------------------------------------------------------
const char* VisualizationLibrary::versionString() { return gVersionString.c_str(); }
//------------------------------------------------------------------------------
// Global VLSettings
//------------------------------------------------------------------------------
namespace
{
  ref<VLSettings> gSettings = NULL;
}
VLSettings* vl::globalSettings()
{
  return gSettings.get();
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
//------------------------------------------------------------------------------
void VisualizationLibrary::initCore(bool log_info)
{
  VL_CHECK(!gInitializedCore);
  if (gInitializedCore)
  {
    Log::bug("VisualizationLibrary::initCore(): Visualization Library Core is already initialized!\n");
    return;
  }

  // --- Init Core ---

  // Install globabl settings
  gSettings = new VLSettings;

  // Install default logger
  ref<StandardLog> logger = new StandardLog;
  logger->setLogFile( globalSettings()->defaultLogPath() );
  setDefLogger( logger.get() );

  // Install default LoadWriterManager
  gDefaultLoadWriterManager = new LoadWriterManager;

  // Install default FileSystem
  gDefaultFileSystem = new FileSystem;
  gDefaultFileSystem->directories()->push_back( new DiskDirectory( globalSettings()->defaultDataPath() ) );
  
  // Register 2D modules
  #if defined(VL_IO_2D_JPG)
    registerLoadWriter(new LoadWriterJPG);
  #endif
  #if defined(VL_IO_2D_PNG)
    registerLoadWriter(new LoadWriterPNG);
  #endif
  #if defined(VL_IO_2D_TIFF)
    registerLoadWriter(new LoadWriterTIFF);
  #endif
  #if defined(VL_IO_2D_TGA)
    registerLoadWriter(new LoadWriterTGA);
  #endif
  #if defined(VL_IO_2D_BMP)
    registerLoadWriter(new LoadWriterBMP);
  #endif
  #if defined(VL_IO_2D_DDS)
    registerLoadWriter(new LoadWriterDDS);
  #endif
  #if defined(VL_IO_2D_DAT)
    registerLoadWriter(new LoadWriterDAT);
  #endif
  #if defined(VL_IO_2D_DICOM)
    registerLoadWriter(new LoadWriterDICOM);
  #endif

  // Log VL and system information.
  if (globalSettings()->verbosityLevel() && log_info)
    Log::logSystemInfo();

  // Initialized = on
  gInitializedCore = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdownCore()
{
  // Initialized = off
  gInitializedCore = false;

  // --- Dispose Core ---

  // Dispose default LoadWriterManager
  gDefaultLoadWriterManager->loadCallbacks()->clear();
  gDefaultLoadWriterManager->writeCallbacks()->clear();
  gDefaultLoadWriterManager->loadWriters()->clear();
  gDefaultLoadWriterManager = NULL;

  // Dispose default FileSystem
  gDefaultFileSystem->directories()->clear();
  gDefaultFileSystem = NULL;

  // Dispose default logger
  if (globalSettings()->verbosityLevel())
  {
    Log::print("Visualization Library shutdown.\n");
  }
  // we keep the logger alive as much as we can.
  // setDefLogger( NULL );

  // keep global settings (used by logger)
  // gSettings = NULL;
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
