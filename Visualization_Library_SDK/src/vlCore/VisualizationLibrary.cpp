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

#include <vlGraphics/GeometryLoadCallback.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/Rendering.hpp> 
#include <vlGraphics/RenderQueue.hpp>
#include <vlGraphics/SceneManager.hpp>

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/Say.hpp>
#include <vlCore/Time.hpp>
#include <vlCore/Log.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/DiskDirectory.hpp>
#include <vlCore/ZippedDirectory.hpp>
#include <vlCore/MemoryDirectory.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlCore/Quaternion.hpp>
#include <vlCore/Object.hpp>
#include <vlCore/KeyValues.hpp>
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

#if defined(_WIN32)
  // console includes
  #include <stdio.h>
  #include <io.h>
  #include <fcntl.h>
  #include <commdlg.h>
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
class VisualizationLibraryInstance: public Object
{
  static ref<VisualizationLibraryInstance> mSingleton;
  VisualizationLibraryInstance()
  {
    mVersionString     = String( Say("%n.%n.%n") << VL_Major << VL_Minor << VL_Build ).toStdString();
    mRenderingAbstract = new Rendering;
    mFontManager       = new FontManager;
    mFileSystem        = new FileSystem;
    mLoadWriterManager = new LoadWriterManager;
    mStandardLogger    = new StandardLog;
    mGlobalSettings    = new Settings;
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

  ref<RenderingAbstract> mRenderingAbstract;
  ref<LoadWriterManager> mLoadWriterManager;
  ref<FontManager> mFontManager;
  ref<FileSystem>  mFileSystem;
  ref<StandardLog> mStandardLogger;
  ref<Settings> mGlobalSettings;
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
StandardLog* VisualizationLibrary::logger()          { return VisualizationLibraryInstance::singleton()->mStandardLogger.get(); }
//------------------------------------------------------------------------------
Settings* VisualizationLibrary::settings() { return VisualizationLibraryInstance::singleton()->mGlobalSettings.get(); }
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
  logger()->setLogFile( settings()->defaultLogPath() );
  Log::setLogger( logger() );

  if (settings()->verbosityLevel())
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

    Time time;
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
    Log::print( Say("Log file  = %s\n") << settings()->defaultLogPath() );
    Log::print( Say("Data path = %s\n") << settings()->defaultDataPath() );
    Log::print("Verbosity level = ");
    switch(settings()->verbosityLevel())
    {
      /*case vl::VEL_VERBOSITY_SILENT: Log::print("SILENT\n"); break;*/
      case vl::VEL_VERBOSITY_ERROR:  Log::print("ERROR\n"); break;
      case vl::VEL_VERBOSITY_NORMAL: Log::print("NORMAL\n"); break;
      case vl::VEL_VERBOSITY_DEBUG:  Log::print("DEBUG\n"); break;
      default: break;
    }
    Log::print( Say("Check OpenGL States = %s\n") << (settings()->checkOpenGLStates()?"YES":"NO") );
    Log::print( Say("Check Transform Siblings = %s\n") << (settings()->checkTransformSiblings()?"YES":"NO") );

    Log::print("\n");
  }

  // adds default Visualization Library's data directory
  fileSystem()->directories()->push_back( new DiskDirectory( settings()->defaultDataPath() ) );

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
  // release file system
  VisualizationLibraryInstance::singleton()->mFileSystem = NULL;
  // release load-writer manager
  VisualizationLibraryInstance::singleton()->mLoadWriterManager = NULL;  
  // others
  VisualizationLibraryInstance::singleton()->mEnvVars = NULL;
  // say goodbye even to the logger!
  if (settings()->verbosityLevel())
  {
    Log::print("Visualization Library shutdown.\n");
  }
  VisualizationLibraryInstance::singleton()->mStandardLogger = NULL;

  // delete VisualizationLibraryInstance singleton
  VisualizationLibraryInstance::setSingleton(NULL);

  Log::setLogger( NULL );
}
//------------------------------------------------------------------------------
void VisualizationLibrary::initEnvVars()
{
  char* val = NULL;

  // log file

  val = getenv("VL_LOGFILE_PATH");
  if (val)
    settings()->mDefaultLogPath = val;
  else
    settings()->mDefaultLogPath = "log.txt";

  // data path

  val = getenv("VL_DATA_PATH");
  if (val)
    settings()->mDefaultDataPath = val;
  else
    settings()->mDefaultDataPath = "../data";

  // verbosity level

  val = getenv("VL_VERBOSITY_LEVEL");
  if (val)
  {
    if ( String(val).toUpperCase() == "SILENT")
      settings()->setVerbosityLevel(vl::VEL_VERBOSITY_SILENT);
    else
    if ( String(val).toUpperCase() == "ERROR")
      settings()->setVerbosityLevel(vl::VEL_VERBOSITY_ERROR);
    else
    if ( String(val).toUpperCase() == "NORMAL")
      settings()->setVerbosityLevel(vl::VEL_VERBOSITY_NORMAL);
    else
    if ( String(val).toUpperCase() == "DEBUG")
      settings()->setVerbosityLevel(vl::VEL_VERBOSITY_DEBUG);
    else
    {
      // no log here yet.
      fprintf(stderr,"VL_VERBOSITY_LEVEL variable has unknown value %s! Legal values: SILENT, ERROR, NORMAL, DEBUG\n\n", val);
    }
  }

  // opengl state checks

  val = getenv("VL_CHECK_GL_STATES");
  if (val)
  {
    if ( String(val).toUpperCase() == "YES" )
      settings()->setCheckOpenGLStates(true);
    else
    if ( String(val).toUpperCase() == "NO" )
      settings()->setCheckOpenGLStates(false);
    else
    {
      // no log here yet.
      fprintf(stderr,"VL_CHECK_GL_STATES variable has unknown value '%s'! Legal values: YES, NO.\n\n", val);
    }
  }
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
