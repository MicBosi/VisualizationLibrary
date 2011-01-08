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

#include <vlCore/Settings.hpp>

namespace vl
{
  class FileSystem;
  class ResourceDatabase;
  class KeyValues;
  class StandardLog;
  class FontManager;
  class LoadWriterManager;
  class RenderingAbstract;

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

    //! Returns Visualization Library's default FileSystem.
    static FileSystem* fileSystem();

    //! Returns Visualization Library's default LoadWriterManager.
    static LoadWriterManager* loadWriterManager();

    //! Returns the environment variables used by Visualization Library.
    //! Can also be used to globally pass parameters across different parts of an application.
    static KeyValues* envVars();

    //! Returns the default Log object.
    static StandardLog* logger();

    //! Returns the global settings of VL.
    static Settings* settings();

    //! Returns Visualization Library's rendering root.
    static RenderingAbstract* rendering();
    static void setRendering(RenderingAbstract* rendering);

    //! Returns Visualization Library's default FontManager.
    static FontManager* fontManager();

  protected:
    //! Initializes the environment variables
    static void initEnvVars();
  };

  //! Shows a console window that displays the standard output. This function is meant to be used only under Windows.
  void showWin32Console();
}

#endif
