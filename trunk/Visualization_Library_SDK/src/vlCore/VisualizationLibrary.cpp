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
#include <vlCore/version.hpp>
#include <cassert>

#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/FontManager.hpp>

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
  const char* gCertificate = "[Visualization Library BSD License]";
  bool gInitialized = false;
};
//------------------------------------------------------------------------------
bool VisualizationLibrary::initialized() { return gInitialized; }
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
// Default Rendering
//------------------------------------------------------------------------------
namespace
{
  ref<RenderingAbstract> gDefaultRendering = NULL;
}
RenderingAbstract* vl::defRendering()
{
  return gDefaultRendering.get();
}
void vl::setDefRendering(RenderingAbstract* ra)
{
  gDefaultRendering = ra;
}
//-----------------------------------------------------------------------------
// Default FontManager
//-----------------------------------------------------------------------------
namespace
{
  ref<FontManager> gDefaultFontManager = NULL;
}
FontManager* vl::defFontManager()
{
  return gDefaultFontManager.get();
}
void vl::setDefFontManager(FontManager* fm)
{
  gDefaultFontManager = fm;
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
void vl::seDefFileSystem(FileSystem* fs)
{
  gDefaultFileSystem = fs;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::init()
{
  VL_CHECK(!gInitialized);
  if (gInitialized)
  {
    Log::bug("VisualizationLibrary::init(): VisualizationLibrary is already initialized!\n");
    return;
  }

  // Install globabl settings
  gSettings = new VLSettings;

  // Install default logger
  ref<StandardLog> logger = new StandardLog;
  logger->setLogFile( globalSettings()->defaultLogPath() );
  setDefLogger( logger.get() );

  // Install default Rendering
  gDefaultRendering = new Rendering;

  // Install default FontManager
  gDefaultFontManager = new FontManager;

  // Install default LoadWriterManager
  gDefaultLoadWriterManager = new LoadWriterManager;

  // Install default FileSystem
  gDefaultFileSystem = new FileSystem;
  gDefaultFileSystem->directories()->push_back( new DiskDirectory( globalSettings()->defaultDataPath() ) );
  
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

  // Log VL and system information.
  if (globalSettings()->verbosityLevel())
    Log::logSystemInfo();

  // Initialized = on
  gInitialized = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdown()
{
  if (gInitialized)
  {
    // Initialized = off
    gInitialized = false;

    if (globalSettings()->verbosityLevel())
    {
      Log::print("Visualization Library shutdown.\n");
    }

    // Dispose globabl settings
    gSettings = NULL;

    // Dispose default Rendering
    gDefaultRendering = NULL;

    // Dispose default FontManager
    gDefaultFontManager->releaseAllFonts();
    gDefaultFontManager = NULL;

    // Dispose default LoadWriterManager
    gDefaultLoadWriterManager->loadCallbacks()->clear();
    gDefaultLoadWriterManager->writeCallbacks()->clear();
    gDefaultLoadWriterManager->loadWriters()->clear();
    gDefaultLoadWriterManager = NULL;

    // Dispose default FileSystem
    gDefaultFileSystem->directories()->clear();
    gDefaultFileSystem = NULL;
    
    // Dispose default logger
    setDefLogger( NULL );
  }
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
