/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.org                                               */
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

#include "BaseDemo.hpp"
#include <vlCore/Colors.hpp>
#include <vlGraphics/Terrain.hpp>

class App_Terrain: public BaseDemo
{
public:
  virtual void initEvent()
  {
    vl::Log::notify(appletInfo());

    if (!vl::Has_Multitexture)
    {
      vl::Log::error("This test requres multi texturing.\n");
      vl::Time::sleep(2000);
      exit(1);
    }

    ghostCameraManipulator()->setMovementSpeed(5);
    // allocate terrain scene manager
    vl::ref<vl::Terrain> terrain = new vl::Terrain;
    // use GLSL?
    terrain->setUseGLSL( vl::Has_GL_ARB_shading_language_100 ? true : false );
    // dimensions of the terrain
    terrain->setWidth(100);
    terrain->setDepth(100);
    terrain->setHeight(5.0f);
    // heightmap texture size used by the GLSL program
    if (vl::Has_GL_ATI_texture_float || vl::Has_GL_ARB_texture_float)
      terrain->setHeightmapTextureFormat(vl::TF_LUMINANCE16F);
    else
      terrain->setHeightmapTextureFormat(vl::TF_LUMINANCE);
    // origin of the terrain
    terrain->setOrigin(vl::vec3(0,0,0));
    // define textures
    terrain->setHeightmapTexture("/images/ps_height_4k.jpg");
    terrain->setTerrainTexture("/images/ps_texture_4k.jpg");
    terrain->setDetailTexture("/images/noise.png");
    terrain->setDetailRepetitionCount(8);
    // define shaders to be used to render the terrain
    terrain->setFragmentShader("/glsl/terrain.fs");
    terrain->setVertexShader("/glsl/terrain.vs");
    // initialize the terrain
    terrain->init();
    // add the terrain scene manager to the rendering
    rendering()->as<vl::Rendering>()->sceneManagers()->push_back( terrain.get() );

    // adds fog if we are not using GLSL but the fixed function pipeline
    if (!terrain->useGLSL())
    {
      // set sky to white
      rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor(vl::white);
      // set fog render state
      vl::ref<vl::Fog> fog = new vl::Fog;
      fog->setColor(vl::white);
      fog->setDensity(0.045f);
      fog->setMode(vl::FM_EXP);
      // install and enable fog
      terrain->shaderNode()->setRenderState(fog.get());
      terrain->shaderNode()->setEnable(vl::EN_FOG,true);
      terrain->shaderNode()->updateHierarchy();
    }

    // for debugging purposes
    #if 0
      showBoundingVolumes(1,0);
    #endif
  }
};

// Have fun!

BaseDemo* Create_App_Terrain() { return new App_Terrain; }
