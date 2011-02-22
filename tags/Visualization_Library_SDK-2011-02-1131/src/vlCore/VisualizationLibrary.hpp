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

namespace vl
{
  //! Used to initialize/shutdown VisualizationLibrary and to access important global data.
  class VisualizationLibrary
  {
  public:
    //! Initializes VLCore or VLCore and VLGraphics libraries (if VL_MODULE_GRAPHICS was enabled during CMake configuration).
    //! Call initCore() instead of init() when using only VLCore if VL_MODULE_GRAPHICS was enabled during CMake configuration.
    static void init();

    //! Releases all the resources acquired by VLCore and VLGraphics.
    //! Call shutdownCore() instead of shutdown() when using only VLCore if VL_MODULE_GRAPHICS was enabled during CMake configuration.
    static void shutdown();

    //! Initializes only VLCore library, equivalent to init() when VL_MODULE_GRAPHICS is disabled during CMake configuration.
    //! Call initCore() instead of init() ONLY when using VLCore alone (i.e. no VLGraphics) and if VL_MODULE_GRAPHICS was enabled during CMake configuration.
    static void initCore();

    //! Releases all the resources acquired by Visualization Library Core
    //! Call shutdownCore() instead of shutdown() ONLY when using VLCore alone (i.e. no VLGraphics) and if VL_MODULE_GRAPHICS was enabled during CMake configuration.
    static void shutdownCore();

    //! Returns true if VLCore library is initialized and shutdown has not been called.
    static bool isCoreInitialized();

    //! Returns true if VLGraphics library is initialized and shutdown has not been called.
    static bool isGraphicsInitialized();

    //! Returns the Visualization Library's version string.
    static const char* versionString();
  
  private:
    static void initGraphics();
    static void shutdownGraphics();
  };

  //! Shows a console window that displays the standard output. This function is meant to be used only under Windows only.
  void showWin32Console();
}

#endif
