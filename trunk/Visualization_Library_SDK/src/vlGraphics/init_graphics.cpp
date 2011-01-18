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
#include <vlGraphics/Rendering.hpp>
#include <vlGraphics/FontManager.hpp>

using namespace vl;

#if defined(VL_IO_3D_3DS)
  #include <vlGraphics/vl3DS.hpp>
#endif
#if defined(VL_IO_3D_OBJ)
  #include <vlGraphics/vlOBJ.hpp>
#endif
#if defined(VL_IO_3D_AC3D)
  #include <vlGraphics/vlAC3D.hpp>
#endif
#if defined(VL_IO_3D_PLY)
  #include <vlGraphics/vlPLY.hpp>
#endif
#if defined(VL_IO_3D_STL)
  #include <vlGraphics/vlSTL.hpp>
#endif
#if defined(VL_IO_3D_MD2)
  #include <vlGraphics/vlMD2.hpp>
#endif

//------------------------------------------------------------------------------
// VL misc
//------------------------------------------------------------------------------
namespace
{
  bool gInitializedGraphics = false;
};
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
void VisualizationLibrary::initGraphics()
{
  VL_CHECK(!gInitializedGraphics);
  if (gInitializedGraphics)
  {
    Log::bug("VisualizationLibrary::initGraphics(): Visualization Library Graphics is already initialized!\n");
    return;
  }

  // --- Init Graphics ---

  // Install default Rendering
  gDefaultRendering = new Rendering;

  // Install default FontManager
  gDefaultFontManager = new FontManager;

  // Register 3D modules
  #if defined(VL_IO_3D_OBJ)
    registerLoadWriter(new LoadWriterOBJ);
  #endif
  #if defined(VL_IO_3D_3DS)
    registerLoadWriter(new LoadWriter3DS);
  #endif
  #if defined(VL_IO_3D_AC3D)
    registerLoadWriter(new LoadWriterAC3D);
  #endif
  #if defined(VL_IO_3D_PLY)
    registerLoadWriter(new LoadWriterPLY);
  #endif
  #if defined(VL_IO_3D_STL)
    registerLoadWriter(new LoadWriterSTL);
  #endif
  #if defined(VL_IO_3D_MD2)
    registerLoadWriter(new LoadWriterMD2);
  #endif

  // ---

  // Initialized = on
  gInitializedGraphics = true;
}
//------------------------------------------------------------------------------
void VisualizationLibrary::shutdownGraphics()
{
  if (gInitializedGraphics)
  {
    gInitializedGraphics = false;

    // --- Dispose Graphics ---

    // Dispose default Rendering
    gDefaultRendering = NULL;

    // Dispose default FontManager
    gDefaultFontManager->releaseAllFonts();
    gDefaultFontManager = NULL;
  }
}
//------------------------------------------------------------------------------
#if defined(VL_MODULE_GRAPHICS)
  void VisualizationLibrary::init()
  {
    initCore();
    initGraphics();
  }
//------------------------------------------------------------------------------
  void VisualizationLibrary::shutdown()
  {
    shutdownGraphics();
    shutdownCore();
  }
//------------------------------------------------------------------------------
  bool VisualizationLibrary::isGraphicsInitialized() { return gInitializedGraphics; }
#endif
//------------------------------------------------------------------------------
