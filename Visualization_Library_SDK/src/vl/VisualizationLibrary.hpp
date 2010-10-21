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

#ifndef VisualizationLibrary_INCLUDE_ONCE
#define VisualizationLibrary_INCLUDE_ONCE

#include <vl/VirtualFile.hpp>
#include <vl/version.hpp>

namespace vl
{
  class FileSystem;
  class ResourceDatabase;
  class KeyValues;
  class StandardLog;
  class FontManager;
  class LoadWriterManager;
  class RenderingAbstract;

  //! Global application settings controlling how Visualization Library behaves.
  class GlobalSettings: public Object
  {
  public:
    typedef enum { 
      VERBOSITY_ERROR,  //!<< Outputs critical and runtime error messages.
      VERBOSITY_NORMAL, //!<< Outputs normal information messages, plus all error messages.
      VERBOSITY_DEBUG   //!<< Outputs extra information messages useful for debugging, plus all normal and error messages.
    } EVerbosityLevel;

  public:
    GlobalSettings()
    {
      // mic fixme:
      // - These settings are overridden by environment variables.
      // - Documenta env vars su articolo: Configuring VL
      // - ApplyGlobalSettings() funciton?
      // - Alla fine non sono tutti global settings i componenti di VL?
      // - Testa double precision pipeline.
      mDataDirectory   = "../data";
      mLogFile         = "log.txt";
      #ifndef NDEBUG
        mVerbosityLevel  = VERBOSITY_NORMAL;
        mCheckCleanState = true;
      #else
        mVerbosityLevel  = VERBOSITY_ERROR;
        mCheckCleanState = false;
      #endif
    }

    const String& dataDirectory() const { return mDataDirectory; }
    // mic fixme: settare questa variabile deve avere un riscontro immediato su loadFile() & co.
    void setDataDirectory(const String& data_dir) { mDataDirectory = data_dir; }

    const String& logFile() const { return mLogFile; }
    // mic fixme: settare questa variabile deve avere un riscontro immediato sul logging.
    void setLogFile(const String& data_dir) { mLogFile = data_dir; }

    bool checkCleanState() const { return mCheckCleanState; }
    void setCheckcleanState(bool check_clean) { mCheckCleanState = check_clean; }

    EVerbosityLevel verbosityLevel() const { return mVerbosityLevel; }
    void setVerbosityLevel(EVerbosityLevel verb_level) { mVerbosityLevel = verb_level; }

  protected:
    String mDataDirectory; // mic fixme: inizializzato da environment variable
    String mLogFile;       // mic fixme: inizializzato da environment variable
    EVerbosityLevel mVerbosityLevel;
    bool mCheckCleanState;
  };

  //! Used to initialize/shutdown VisualizationLibrary and to access important global data.
  class VisualizationLibrary
  {
  public:
    //! Initializes Visualization Library's internal data.
    static void init();

    //! Releases all the resources acquired by Visualization Library.
    static void shutdown();

    //! Returns true if init() has been called and shutdown() has not been called yet.
    static bool initialized();

    //! Returns the Visualization Library's version string.
    static const char* versionString();

    //! Returns Visualization Library's rendering root.
    static RenderingAbstract* rendering();
    static void setRendering(RenderingAbstract* rendering);

    //! Returns Visualization Library's default FileSystem.
    static FileSystem* fileSystem();

    //! Returns Visualization Library's default FontManager.
    static FontManager* fontManager();

    //! Returns Visualization Library's default LoadWriterManager.
    static LoadWriterManager* loadWriterManager();

    //! Returns the environment variables used by Visualization Library.
    //! Can also be used to globally pass parameters across different parts of an application.
    static KeyValues* envVars();

    //! Returns the default Log object.
    static StandardLog* logger();

    //! Returns the global settings of VL.
    static GlobalSettings* globalSettings();

    //! For internal use only
    static void* freeTypeLibrary();

  protected:
    //! Initializes the environment variables
    static void initEnvVars();
  };

  //! Shows a console window that displays the standard output. This function is meant to be used only under Windows.
  void showWin32Console();
}

#endif
