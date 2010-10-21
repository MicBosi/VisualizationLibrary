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

#include <vl/VisualizationLibrary.hpp>
#include <vl/FontManager.hpp>
#include <vl/Log.hpp>
#include <vl/Say.hpp>
#include <vl/Time.hpp>
#include <vl/Log.hpp>
#include <vl/Rendering.hpp> 
#include <vl/RenderQueue.hpp>
#include <vl/SceneManager.hpp>
#include <vl/FileSystem.hpp>
#include <vl/FontManager.hpp>
#include <vl/DiskDirectory.hpp>
#include <vl/ZippedDirectory.hpp>
#include <vl/MemoryDirectory.hpp>
#include <vl/LoadWriterManager.hpp>
#include <vl/GeometryLoadCallback.hpp>
#include <vl/quat.hpp>
#include <vl/Object.hpp>
#include <vl/KeyValues.hpp>
#include <cassert>

#include "ft2build.h"
#include FT_FREETYPE_H

#if defined(IO_MODULE_JPG)
  #include <vl/vlJPG.hpp>
#endif
#if defined(IO_MODULE_PNG)
  #include <vl/vlPNG.hpp>
#endif
#if defined(IO_MODULE_TIFF)
  #include <vl/vlTIFF.hpp>
#endif
#if defined(IO_MODULE_TGA)
  #include <vl/vlTGA.hpp>
#endif
#if defined(IO_MODULE_DAT)
  #include <vl/vlDAT.hpp>
#endif
#if defined(IO_MODULE_DDS)
  #include <vl/vlDDS.hpp>
#endif
#if defined(IO_MODULE_BMP)
  #include <vl/vlBMP.hpp>
#endif
#if defined(IO_MODULE_3DS)
  #include <vl/vl3DS.hpp>
#endif
#if defined(IO_MODULE_OBJ)
  #include <vl/vlOBJ.hpp>
#endif
#if defined(IO_MODULE_AC3D)
  #include <vl/vlAC3D.hpp>
#endif
#if defined(IO_MODULE_PLY)
  #include <vl/vlPLY.hpp>
#endif
#if defined(IO_MODULE_STL)
  #include <vl/vlSTL.hpp>
#endif
#if defined(IO_MODULE_MD2)
  #include <vl/vlMD2.hpp>
#endif
#if defined(IO_MODULE_DICOM)
  #include <vl/vlDICOM.hpp>
#endif

#if defined(_WIN32)
  // console includes
  #include <stdio.h>
  #include <io.h>
  #include <fcntl.h>
  #include <commdlg.h>
#endif

using namespace vl;

//------------------------------------------------------------------------------
VL_COMPILE_TIME_CHECK( sizeof(wchar_t) >= 2 )
VL_COMPILE_TIME_CHECK( sizeof(vec2)    == sizeof(Real)*2 )
VL_COMPILE_TIME_CHECK( sizeof(vec3)    == sizeof(Real)*3 )
VL_COMPILE_TIME_CHECK( sizeof(vec4)    == sizeof(Real)*4 )
VL_COMPILE_TIME_CHECK( sizeof(mat2)    == sizeof(Real)*2*2 )
VL_COMPILE_TIME_CHECK( sizeof(mat3)    == sizeof(Real)*3*3 )
VL_COMPILE_TIME_CHECK( sizeof(mat4)    == sizeof(Real)*4*4 )
VL_COMPILE_TIME_CHECK( sizeof(quat)    == sizeof(Real)*4 )
VL_COMPILE_TIME_CHECK( sizeof(AABB)    == sizeof(Real)*6 )
VL_COMPILE_TIME_CHECK( sizeof(Sphere)  == sizeof(Real)*4 )
//------------------------------------------------------------------------------
class VisualizationLibraryInstance: public Object
{
  static ref<VisualizationLibraryInstance> mSingleton;
  VisualizationLibraryInstance()
  {
    mFreeTypeLibrary   = NULL;
    mVersionString     = String( Say("%n.%n.%n") << VL_Major << VL_Minor << VL_Build ).toStdString();
    mRenderingAbstract = new Rendering;
    mFontManager       = new FontManager;
    mFileSystem        = new FileSystem;
    mLoadWriterManager = new LoadWriterManager;
    mStandardLogger    = new StandardLog;
    mGlobalSettings    = new GlobalSettings;
    mEnvVars           = new KeyValues;
    mCertificate       = "[Visualization Library BSD License]";
    mInitialized       = false;
  }
public:
  static VisualizationLibraryInstance* singleton()
  {
    if(!mSingleton)
      mSingleton = new VisualizationLibraryInstance;
    return mSingleton.get();
  }
  static void setSingleton(VisualizationLibraryInstance* data) { mSingleton = data; }

  FT_Library       mFreeTypeLibrary;
  ref<RenderingAbstract> mRenderingAbstract;
  ref<LoadWriterManager> mLoadWriterManager;
  ref<FontManager> mFontManager;
  ref<FileSystem>  mFileSystem;
  ref<StandardLog> mStandardLogger;
  ref<GlobalSettings> mGlobalSettings;
  ref<KeyValues>   mEnvVars;
  std::string      mVersionString;
  const char* mCertificate;
  bool mInitialized;
};
//------------------------------------------------------------------------------
ref<VisualizationLibraryInstance> VisualizationLibraryInstance::mSingleton;
//------------------------------------------------------------------------------
bool VisualizationLibrary::initialized()             { return VisualizationLibraryInstance::singleton()->mInitialized; }
//------------------------------------------------------------------------------
RenderingAbstract* VisualizationLibrary::rendering() { return VisualizationLibraryInstance::singleton()->mRenderingAbstract.get(); }
void VisualizationLibrary::setRendering(RenderingAbstract* rendering) { VisualizationLibraryInstance::singleton()->mRenderingAbstract = rendering; }
//------------------------------------------------------------------------------
void* VisualizationLibrary::freeTypeLibrary()        { return VisualizationLibraryInstance::singleton()->mFreeTypeLibrary; }
//------------------------------------------------------------------------------
StandardLog* VisualizationLibrary::logger()          { return VisualizationLibraryInstance::singleton()->mStandardLogger.get(); }
//------------------------------------------------------------------------------
GlobalSettings* VisualizationLibrary::globalSettings() { return VisualizationLibraryInstance::singleton()->mGlobalSettings.get(); }
//------------------------------------------------------------------------------
const char* VisualizationLibrary::versionString()    { return VisualizationLibraryInstance::singleton()->mVersionString.c_str(); }
//------------------------------------------------------------------------------
FileSystem* VisualizationLibrary::fileSystem()       { return VisualizationLibraryInstance::singleton()->mFileSystem.get(); }
//------------------------------------------------------------------------------
FontManager* VisualizationLibrary::fontManager()     { return VisualizationLibraryInstance::singleton()->mFontManager.get(); }
//------------------------------------------------------------------------------
LoadWriterManager* VisualizationLibrary::loadWriterManager() { return VisualizationLibraryInstance::singleton()->mLoadWriterManager.get(); }
//------------------------------------------------------------------------------
KeyValues* VisualizationLibrary::envVars()           { return VisualizationLibraryInstance::singleton()->mEnvVars.get(); }
//------------------------------------------------------------------------------
ref<VirtualFile> vl::locateFile(const String& path)  { return VisualizationLibrary::fileSystem()->locateFile(path); } 
//------------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadResource(const String& path, bool quick) { return VisualizationLibrary::loadWriterManager()->loadResource(path,quick); }
//------------------------------------------------------------------------------
ref<ResourceDatabase> vl::loadResource(VirtualFile* file, bool quick)  { return VisualizationLibrary::loadWriterManager()->loadResource(file,quick); }
//------------------------------------------------------------------------------
bool vl::canLoad(const String& path)  { return VisualizationLibrary::loadWriterManager()->canLoad(path);  }
//------------------------------------------------------------------------------
bool vl::canWrite(const String& path) { return VisualizationLibrary::loadWriterManager()->canWrite(path); }
//------------------------------------------------------------------------------
bool vl::canLoad(VirtualFile* file)   { return VisualizationLibrary::loadWriterManager()->canLoad(file);  }
//------------------------------------------------------------------------------
bool vl::canWrite(VirtualFile* file)  { return VisualizationLibrary::loadWriterManager()->canWrite(file); }
//------------------------------------------------------------------------------
void VisualizationLibrary::init()
{
  initEnvVars();
  logger()->setLogFile( envVars()->value("VL_LOGFILE_PATH") );
  Log::setLogger( logger() );

#if defined(_MSC_VER)
  std::string compiler = "MSVC";
#elif defined(__GNUG__)
  std::string compiler = "GCC";
#else
  std::string compiler = "UNKNOWN";
#endif

  Time time;
  Log::print( Say("Visualization Library v%n.%n.%n\n%s - %s - %s compiler.\n\n") << VL_Major << VL_Minor << VL_Build << __DATE__ << __TIME__ << compiler );

  FT_Error error = FT_Init_FreeType( &VisualizationLibraryInstance::singleton()->mFreeTypeLibrary );
  if ( error )
  {
    Log::error("An error occurred during FreeType library initialization!\n");
    VL_TRAP()
  }

  #ifndef NDEBUG
    Log::print("Environment variables:\n");
    envVars()->print();
    Log::print("\n");
  #endif

  // adds default Visualization Library's data directory
  fileSystem()->directories()->push_back( new DiskDirectory( VisualizationLibrary::envVars()->value("VL_DATA_PATH") ) );

  // register I/O plugins
  #if defined(IO_MODULE_JPG)
    loadWriterManager()->registerLoadWriter(new LoadWriterJPG);
  #endif
  #if defined(IO_MODULE_PNG)
    loadWriterManager()->registerLoadWriter(new LoadWriterPNG);
  #endif
  #if defined(IO_MODULE_TIFF)
    loadWriterManager()->registerLoadWriter(new LoadWriterTIFF);
  #endif
  #if defined(IO_MODULE_TGA)
    loadWriterManager()->registerLoadWriter(new LoadWriterTGA);
  #endif
  #if defined(IO_MODULE_BMP)
    loadWriterManager()->registerLoadWriter(new LoadWriterBMP);
  #endif
  #if defined(IO_MODULE_DDS)
    loadWriterManager()->registerLoadWriter(new LoadWriterDDS);
  #endif
  #if defined(IO_MODULE_DAT)
    loadWriterManager()->registerLoadWriter(new LoadWriterDAT);
  #endif
  #if defined(IO_MODULE_OBJ)
    loadWriterManager()->registerLoadWriter(new LoadWriterOBJ);
  #endif
  #if defined(IO_MODULE_3DS)
    loadWriterManager()->registerLoadWriter(new LoadWriter3DS);
  #endif
  #if defined(IO_MODULE_AC3D)
    loadWriterManager()->registerLoadWriter(new LoadWriterAC3D);
  #endif
  #if defined(IO_MODULE_PLY)
    loadWriterManager()->registerLoadWriter(new LoadWriterPLY);
  #endif
  #if defined(IO_MODULE_STL)
    loadWriterManager()->registerLoadWriter(new LoadWriterSTL);
  #endif
  #if defined(IO_MODULE_MD2)
    loadWriterManager()->registerLoadWriter(new LoadWriterMD2);
  #endif
  #if defined(IO_MODULE_DICOM)
    loadWriterManager()->registerLoadWriter(new LoadWriterDICOM);
  #endif

  VisualizationLibrary::loadWriterManager()->loadCallbacks()->push_back( new vl::GeometryLoadCallback );
  // initialized = on
  VisualizationLibraryInstance::singleton()->mInitialized = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdown()
{
  // initialized = off
  VisualizationLibraryInstance::singleton()->mInitialized = false;

  // release Rendering hierarchy
  VisualizationLibraryInstance::singleton()->mRenderingAbstract = NULL;
  // release font resources
  fontManager()->releaseAllFonts();
  VisualizationLibraryInstance::singleton()->mFontManager = NULL;
  VL_CHECK(VisualizationLibrary::freeTypeLibrary());
  FT_Done_FreeType(VisualizationLibraryInstance::singleton()->mFreeTypeLibrary);
  VisualizationLibraryInstance::singleton()->mFreeTypeLibrary = NULL;
  // release file system
  VisualizationLibraryInstance::singleton()->mFileSystem = NULL;
  // release load-writer manager
  VisualizationLibraryInstance::singleton()->mLoadWriterManager = NULL;  
  // others
  VisualizationLibraryInstance::singleton()->mEnvVars = NULL;
  // say goodbye even to the logger!
  Log::print("**************************************\n");
  Log::print("*  VisualizationLibrary::shutdown()  *\n");
  Log::print("**************************************\n");
  VisualizationLibraryInstance::singleton()->mStandardLogger = NULL;

  // delete VisualizationLibraryInstance singleton
  VisualizationLibraryInstance::setSingleton(NULL);

  Log::setLogger( NULL );
}
//------------------------------------------------------------------------------
void VisualizationLibrary::initEnvVars()
{
  char* val = NULL;

  val = getenv("VL_DATA_PATH");
  if (val)
    envVars()->set("VL_DATA_PATH") = val;
  else
    envVars()->set("VL_DATA_PATH") = "../data";

  val = getenv("VL_LOGFILE_PATH");
  if (val)
    envVars()->set("VL_LOGFILE_PATH") = val;
  else
    envVars()->set("VL_LOGFILE_PATH") = "log.txt";
}
//------------------------------------------------------------------------------
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
int vl::glcheck(const char* file, int line)
{
  unsigned int glerr = glGetError();
  if (glerr != GL_NO_ERROR)
  {
    String msg( (char*)gluErrorString(glerr) );
    Log::error( Say("glGetError() [%s:%n]: %s\n") << file << line << msg );
  }
  return glerr;
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
