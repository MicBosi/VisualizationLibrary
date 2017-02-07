/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
/*                                                                                    */
/*  Copyright (c) 2005-2017, Michele Bosi                                             */
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
#include <vlX/VLXRegistry.hpp>
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

#if defined(VL_IO_2D_JPG)
  #include <vlCore/plugins/ioJPG.hpp>
#endif
#if defined(VL_IO_2D_PNG)
  #include <vlCore/plugins/ioPNG.hpp>
#endif
#if defined(VL_IO_2D_TIFF)
  #include <vlCore/plugins/ioTIFF.hpp>
#endif
#if defined(VL_IO_2D_TGA)
  #include <vlCore/plugins/ioTGA.hpp>
#endif
#if defined(VL_IO_2D_DAT)
  #include <vlCore/plugins/ioDAT.hpp>
#endif
#if defined(VL_IO_2D_MHD)
  #include <vlCore/plugins/ioMHD.hpp>
#endif
#if defined(VL_IO_2D_DDS)
  #include <vlCore/plugins/ioDDS.hpp>
#endif
#if defined(VL_IO_2D_BMP)
  #include <vlCore/plugins/ioBMP.hpp>
#endif
#if defined(VL_IO_2D_DICOM)
  #include <vlCore/plugins/ioDICOM.hpp>
#endif

// ------------------------------------------------------------------------------
VL_COMPILE_TIME_CHECK( sizeof(double)    == 8 )
VL_COMPILE_TIME_CHECK( sizeof(float)     == 4 )
VL_COMPILE_TIME_CHECK( sizeof(long long) == 8 )
VL_COMPILE_TIME_CHECK( sizeof(int)       == 4 )
VL_COMPILE_TIME_CHECK( sizeof(short)     == 2 )
VL_COMPILE_TIME_CHECK( sizeof(char)      == 1 )
VL_COMPILE_TIME_CHECK( sizeof(wchar_t)   >= 2 )
VL_COMPILE_TIME_CHECK( sizeof(vec2)      == sizeof(real)*2 )
VL_COMPILE_TIME_CHECK( sizeof(vec3)      == sizeof(real)*3 )
VL_COMPILE_TIME_CHECK( sizeof(vec4)      == sizeof(real)*4 )
VL_COMPILE_TIME_CHECK( sizeof(mat2)      == sizeof(real)*2*2 )
VL_COMPILE_TIME_CHECK( sizeof(mat3)      == sizeof(real)*3*3 )
VL_COMPILE_TIME_CHECK( sizeof(mat4)      == sizeof(real)*4*4 )
VL_COMPILE_TIME_CHECK( sizeof(quat)      == sizeof(real)*4 )
VL_COMPILE_TIME_CHECK( sizeof(AABB)      == sizeof(real)*6 )
VL_COMPILE_TIME_CHECK( sizeof(Sphere)    == sizeof(real)*4 )
//// ------------------------------------------------------------------------------
//// VL misc
//// ------------------------------------------------------------------------------
namespace
{
  //std::string gVersionString = String( Say("%n.%n.%s") << VL_Major << VL_Minor << VL_Patch ).toStdString();
  bool gInitializedCore = false;
};
// ------------------------------------------------------------------------------
bool VisualizationLibrary::isCoreInitialized() { return gInitializedCore; }
// ------------------------------------------------------------------------------
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
  vl::setGlobalSettings( new GlobalSettings );

  // Install default logger
  ref<StandardLog> logger = new StandardLog;
  logger->setLogFile( globalSettings()->defaultLogPath() );
  setDefLogger( logger.get() );

  // Install default LoadWriterManager
  vl::setDefLoadWriterManager( new LoadWriterManager );

  // Install default FileSystem
  vl::setDefFileSystem( new FileSystem );
  vl::defFileSystem()->directories().push_back( new DiskDirectory( globalSettings()->defaultDataPath() ) );

  // Install default VLXRegistry
  vl::setDefVLXRegistry( new VLXRegistry );

  // Install default MersenneTwister (seed done automatically)
  vl::setDefMersenneTwister( new MersenneTwister );

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
  #if defined(VL_IO_2D_MHD)
    registerLoadWriter(new LoadWriterMHD);
  #endif
  #if defined(VL_IO_2D_DICOM)
    registerLoadWriter(new LoadWriterDICOM);
  #endif

  // Log VL and system information.
  if (globalSettings()->verbosityLevel() && log_info)
    logSystemInfo();

  // Initialized = on
  gInitializedCore = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdownCore()
{
  // Initialized = off
  gInitializedCore = false;

  // --- Dispose Core ---

  // Dispose default MersenneTwister
  vl::setDefMersenneTwister( NULL );

  // Dispose default VLXRegistry
  setDefVLXRegistry( NULL );

  // Dispose default LoadWriterManager
  defLoadWriterManager()->loadCallbacks().clear();
  defLoadWriterManager()->writeCallbacks().clear();
  defLoadWriterManager()->loadWriters().clear();
  setDefLoadWriterManager( NULL );

  // Dispose default FileSystem
  defFileSystem()->directories().clear();
  setDefFileSystem( NULL );

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
