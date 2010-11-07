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

#ifndef TEST_PROGRAMS_INCLUDE_ONCE
#define TEST_PROGRAMS_INCLUDE_ONCE

#include <vl/VisualizationLibrary.hpp>
#include "Applets/BaseDemo.hpp"
#include "Applets/App_Billboards.hpp"
#include "Applets/App_RenderToTextureClassic.hpp"
#include "Applets/App_ClipPlanes.hpp"
#include "Applets/App_Deformer.hpp"
#include "Applets/App_DrawPixels.hpp"
#include "Applets/App_FBO_MRT_GLSL.hpp"
#include "Applets/App_GeometryInstancing.hpp"
#include "Applets/App_GLSL.hpp"
#include "Applets/App_GLSL_Bumpmapping.hpp"
#include "Applets/App_GLSLImageProcessing.hpp"
#include "Applets/App_Lights.hpp"
#include "Applets/App_Fractals.hpp"
#include "Applets/App_ModelProfiler.hpp"
#include "Applets/App_MorphAnimation.hpp"
#include "Applets/App_MultipleCameras.hpp"
#include "Applets/App_PointSplatting.hpp"
#include "Applets/App_PolyDepthSorting.hpp"
#include "Applets/App_PolygonReduction.hpp"
#include "Applets/App_RenderOrder.hpp"
#include "Applets/App_RotatingCube.hpp"
#include "Applets/App_ScatterPlot3D.hpp"
#include "Applets/App_ShaderLOD.hpp"
#include "Applets/App_Terrain.hpp"
#include "Applets/App_TextRendering.hpp"
#include "Applets/App_ImageFunctions.hpp"
#include "Applets/App_Texturing.hpp"
#include "Applets/App_Transforms.hpp"
#include "Applets/App_VirtualFileSystemTest.hpp"
#include "Applets/App_VolumeRendering.hpp"
#include "Applets/App_VectorGraphics.hpp"
#include "Applets/App_KdTreeView.hpp"
#include "Applets/App_CullingBenchmark.hpp"
#include "Applets/App_MarchingCubes.hpp"
#include "Applets/App_Interpolators.hpp"
#include "Applets/App_Extrusion.hpp"
#include "Applets/App_Tessellator.hpp"
#include "Applets/App_Molecules.hpp"
#include "Applets/App_EdgeRendering.hpp"
#include "Applets/App_PortalCulling.hpp"
#include "Applets/App_OcclusionCulling.hpp"
#include "Applets/App_BezierSurfaces.hpp"
#include "Applets/App_Picking.hpp"
#include "Applets/App_NearFarOptimization.hpp"
#include "Applets/App_EffectOverride.hpp"
#include "Applets/App_ShaderOverride.hpp"
#include "Applets/App_DrawCalls.hpp"
#include "Applets/App_Primitives.hpp"

// win32 console for sdtout output
#if defined(WIN32) && !defined(NDEBUG)
  #include <io.h>
  #include <fcntl.h>
#endif

class TestBattery
{
public:
  virtual void runGUI(float secs, const vl::String& title, BaseDemo* applet, vl::OpenGLContextFormat format, int x, int y, int width, int height, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center) = 0;

  void setupApplet(BaseDemo* applet, vl::OpenGLContext* oglcontext, float secs, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center)
  {
    /* initialize the applet */
    applet->initialize();
    /* how long the test runs */
    applet->setMaxTime(secs);
    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    oglcontext->addEventListener(applet);
    /* target the window so we can render on it */
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->renderer()->setRenderTarget( oglcontext->renderTarget() );
    /* black background */
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( bk_color );
    /* define the camera position and orientation */
    vl::vec3 up = vl::vec3(0,1,0);
    vl::mat4 view_mat = vl::mat4::lookAt(eye, center, up);
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setInverseViewMatrix( view_mat );
  }

  class TestEntry
  {
  public:
    vl::String title;
    vl::ref<BaseDemo> applet;
    int x, y, width, height;
    vl::fvec4 bk_color;
    vl::vec3 eye;
    vl::vec3 center;
  };

  void run(int test, float secs, const vl::OpenGLContextFormat& format)
  {
    TestEntry tests[] = 
    {
      { "Virtual File System", new App_VirtualFileSystemTest, 10,10, 512, 512, vlut::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Primitives", new App_Primitives, 10,10, 512, 512, vlut::white, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "DrawCall Tests", new App_DrawCalls, 10,10, 512, 512, vlut::white, vl::vec3(0,0,30), vl::vec3(0,0,0) },
      { "Near/Far Planes Optimization", new App_NearFarOptimization, 10, 10, 512, 512, vlut::black, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Effect Override", new App_EffectOverride, 10,10, 512, 512, vlut::white, vl::vec3(0,0,30), vl::vec3(0,0,0) },
      { "Shader Override", new App_ShaderOverride, 10,10, 512, 512, vlut::white, vl::vec3(0,0,30), vl::vec3(0,0,0) },
      { "Shader Multipassing-LOD-Animation", new App_ShaderLOD(4), 10, 10, 512, 512, vlut::black, vl::vec3(0,40,40), vl::vec3(0,0,0) }, 
      { "Geometry Multipassing-LOD-Animation", new App_ShaderLOD(3), 10, 10, 512, 512, vlut::black, vl::vec3(0,40,40), vl::vec3(0,0,0) }, 
      { "Alpha Multipassing", new App_ShaderLOD(0), 10, 10, 512, 512, vlut::black, vl::vec3(0,40,40), vl::vec3(0,0,0) }, 
      { "Render Order - Effect & Actor Ranks", new App_RenderOrder(0), 10, 10, 512, 512, vlut::white, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Render Order - Alpha Z-Sort", new App_RenderOrder(1), 10, 10, 512, 512, vlut::white, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Render Order - Always Z-Sort", new App_RenderOrder(2), 10, 10, 512, 512, vlut::white, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Render Order - Occlusion Z-Sort", new App_RenderOrder(3), 10, 10, 512, 512, vlut::white, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Robot Transform", new App_Transforms, 10, 10, 512, 512, vlut::white, vl::vec3(0,40,60), vl::vec3(0,15,0) }, 
      { "Billboards", new App_Billboards, 10, 10, 512, 512, vlut::royalblue, vl::vec3(0,1,10), vl::vec3(0,1,0) }, 
      { "Multiple Cameras", new App_MultipleCameras, 10, 10, 512, 512, vlut::black, vl::vec3(0,10,15), vl::vec3(0,0,0) }, 
      { "Lights", new App_Lights, 10,10, 512, 512, vlut::black, vl::vec3(-10,10,10), vl::vec3(0,0,0) }, 
      { "Clipping Planes", new App_ClipPlanes, 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,5), vl::vec3(0,0,0) }, 
      { "DrawPixels", new App_DrawPixels, 10,10, 512, 512, vlut::black, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "Image Functions", new App_ImageFunctions, 10,10, 512, 512, vlut::darkturquoise, vl::vec3(0,0,20), vl::vec3(0,0,0) }, 
      { "Texturing", new App_Texturing, 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,20), vl::vec3(0,0,0) }, 
      { "100.000 Scatter Plot", new App_ScatterPlot3D(0), 10, 10, 512, 512, vlut::black, vl::vec3(0,500,1200), vl::vec3(0,0,0) }, 
      { "100.000 Scatter Plot & PointSprite", new App_ScatterPlot3D(1), 10, 10, 512, 512, vlut::black, vl::vec3(0,500,1200), vl::vec3(0,0,0) }, 
      { "MD2 GPU Animation 40", new App_MorphAnimation(40), 10, 10, 512, 512, vlut::black, vl::vec3(0,1000,1500), vl::vec3(0,0,0) }, 
      { "MD2 GPU Animation 400", new App_MorphAnimation(400), 10, 10, 512, 512, vlut::black, vl::vec3(0,1000,1500), vl::vec3(0,0,0) }, 
      { "MD2 GPU Animation 4000", new App_MorphAnimation(4000), 10, 10, 512, 512, vlut::black, vl::vec3(0,1000,1500), vl::vec3(0,0,0) }, 
      { "Text - The Raven", new App_TextRendering(0), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - Alignment", new App_TextRendering(1), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - Rotation", new App_TextRendering(2), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - Multilingual", new App_TextRendering(3), 10, 10, 512, 512, vlut::gold, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - The Solar System", new App_TextRendering(4), 10, 10, 512, 512, vlut::black, vl::vec3(0,35,40), vl::vec3(0,0,0) }, 
      { "GLSL", new App_GLSL, 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,7), vl::vec3(0,0,0) }, 
      { "GLSL Bump Mapping", new App_GLSL_Bumpmapping, 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "GLSL Image Processing", new App_GLSLImageProcessing, 10,10, 512, 512, vlut::black, vl::vec3(0,0,35), vl::vec3(0,0,0) }, 
      { "Legacy Render To Texture", new App_RenderToTextureClassic, 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Render To Texture", new App_FBO_MRT_GLSL(0), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Render To Color Buffer And Copy To Texture", new App_FBO_MRT_GLSL(2), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Framebuffer Blit/Multisample", new App_FBO_MRT_GLSL(3), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Render To Texture MRT", new App_FBO_MRT_GLSL(1), 10, 10, 512, 512, vlut::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "Volume Point Splatting", new App_PointSplatting, 10,10, 512, 512, vlut::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Geometry Instancing", new App_GeometryInstancing, 10,10, 512, 512, vlut::black, vl::vec3(45/2,60,90), vl::vec3(45/2,45/2,45/2) }, 
      { "Polygon Depth Sorting", new App_PolyDepthSorting("/models/3ds/monkey.3ds"), 10,10, 512, 512, vlut::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Polygon Reduction", new App_PolygonReduction("/models/3ds/monkey.3ds"), 10,10, 512, 512, vlut::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Simple Terrain", new App_Terrain, 10,10, 512, 512, vlut::black, vl::vec3(0,5,0), vl::vec3(0,2,-10) }, 
      { "Vector Graphics", new App_VectorGraphics, 10,10, 512, 512, vlut::lightgray, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "Culling Benchmark", new App_CullingBenchmark, 10,10, 512, 512, vlut::black, vl::vec3(0,500,-250), vl::vec3(0,500,-250-1) }, 
      { "ActorKdTree View", new App_KdTreeView, 10,10, 512, 512, vlut::black, vl::vec3(10,10,-10), vl::vec3(0,0,0) }, 
      { "Model Profiler", new App_ModelProfiler, 10,10, 640, 480, vlut::skyblue, vl::vec3(0,0,0), vl::vec3(0,0,-1) }, 
      { "Mandelbrot", new App_Fractals, 10,10, 512, 512, vlut::black, vl::vec3(0,0,35), vl::vec3(0,0,0) }, 
      { "Deformer", new App_Deformer, 10,10, 512, 512, vlut::black, vl::vec3(0,0,35), vl::vec3(0,0,0) }, 
      { "Marching Cubes", new App_MarchingCubes, 10,10, 512, 512, vlut::black, vl::vec3(0,10,50), vl::vec3(0,0,0) }, 
      { "Volume View", new App_VolumeRendering, 10,10, 512, 512, vlut::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Tessellator", new App_Tessellator, 10,10, 512, 512, vlut::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Extrusion", new App_Extrusion, 10,10, 512, 512, vlut::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Interpolator", new App_Interpolators, 10,10, 512, 512, vlut::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Molecule", new App_Molecules, 10,10, 512, 512, vlut::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Edge Enhancement", new App_EdgeRendering, 10,10, 512, 512, vlut::white, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Portal Culling", new App_PortalCulling, 10,10, 512, 512, vlut::white, vl::vec3(20*7/2.0f-10,0,20*7/2.0f-10), vl::vec3(20*7/2.0f-10,0,20*7/2.0f-1.0f) },
      { "Occlusion Culling", new App_OcclusionCulling, 10,10, 512, 512, vlut::gray, vl::vec3(0,25,575), vl::vec3(0,0,0) },
      { "Bezier Patches", new App_BezierSurfaces, 10,10, 512, 512, vlut::black, vl::vec3(4.5f,5,13), vl::vec3(4.5f,0,0) },
      { "Picking", new App_Picking, 10,10, 512, 512, vlut::black, vl::vec3(0,0,10), vl::vec3(0,0,0) },
    };

    int test_count = int( sizeof(tests)/sizeof(TestEntry) );
    if (test > 0 && test-1 < test_count)
      runGUI(secs, tests[test-1].title + " - " + vl::String("Visualization Library ") + vl::VisualizationLibrary::versionString(), tests[test-1].applet.get(), format, tests[test-1].x, tests[test-1].y, tests[test-1].width, tests[test-1].height, tests[test-1].bk_color, tests[test-1].eye, tests[test-1].center );
    else
    {
      vl::showWin32Console();
      printf( "Test #%d does not exist.\n", test );
      for(int i=0; i<test_count; ++i)
        printf("% 2d - %s\n", i+1, tests[i].title.toStdString().c_str());
      printf("\nUsage example:");
      printf("\nWin32_tests N - runs test #N using the Win32 gui bindings.\n");
      #if _WIN32
        MessageBox(NULL, L"Please specify a test number.", L"Please specify a test number.", MB_OK | MB_ICONASTERISK);
      #endif
      exit(1);
    }
  }
};
#endif
