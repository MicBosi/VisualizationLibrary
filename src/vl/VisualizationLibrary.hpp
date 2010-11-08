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
  class Settings: public Object
  {
    friend class VisualizationLibrary;
  public:
    Settings()
    {
      #ifndef NDEBUG
        mVerbosityLevel  = vl::VEL_VERBOSITY_NORMAL;
        mCheckOpenGLStates = true;
        mCheckTransformSiblings = true;
      #else
        mVerbosityLevel  = vl::VEL_VERBOSITY_ERROR;
        mCheckOpenGLStates = false;
        mCheckTransformSiblings = false;
      #endif
    }

    /** If \p true VL checks at the beginning of each rendering that the OpenGL states are
      * clean and ready to be used by VL. If the test fails it can mean that either there 
      * is a bug in VL or that the user did not restore the OpenGL states to a VL friendly
      * state after modifying them.
      * \note This can slow down the rendering. Enabled by default in DEBUG mode only. */
    void setCheckOpenGLStates(bool check_clean) { mCheckOpenGLStates = check_clean; }

    /** If \p true VL checks at the beginning of each rendering that the OpenGL states are
      * clean and ready to be used by VL. If the test fails it can mean that either there 
      * is a bug in VL or that the user did not restore the OpenGL states to a VL friendly
      * state after modifying them.
      * \note This can slow down the rendering. Enabled by default in DEBUG mode only. */
    bool checkOpenGLStates() const { return mCheckOpenGLStates; }

    /** If \p true VL checks that there are no duplicates in a Transform children list. 
      * \note This can slow down considerably the insertion of new child Transforms. 
      * Enabled by default in DEBUG mode only. */
    void setCheckTransformSiblings(bool check_on) { mCheckTransformSiblings = check_on; }

    /** If \p true VL checks that there are no duplicates in a Transform children list. 
      * \note This can slow down considerably the insertion of new child Transforms. 
      * Enabled by default in DEBUG mode only. */
    bool checkTransformSiblings() const { return mCheckTransformSiblings; }

    /** The verbosity level of VL. This applies to all the logs generated via vl::Log::*. */
    void setVerbosityLevel(EVerbosityLevel verb_level) { mVerbosityLevel = verb_level; }

    /** The verbosity level of VL. This applies to all the logs generated via vl::Log::*. */
    EVerbosityLevel verbosityLevel() const { return mVerbosityLevel; }

    /** The path of the default log file. */
    const String& defaultLogPath() const { return mDefaultLogPath; }

    /** The path of the default data directory. */
    const String& defaultDataPath() const { return mDefaultDataPath; }

  protected:
    EVerbosityLevel mVerbosityLevel;
    bool mCheckOpenGLStates;
    bool mCheckTransformSiblings;
    String mDefaultLogPath;
    String mDefaultDataPath;
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
    static Settings* settings();

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
