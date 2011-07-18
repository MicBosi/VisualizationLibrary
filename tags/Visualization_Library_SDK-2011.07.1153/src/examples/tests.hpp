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

#ifndef TEST_PROGRAMS_INCLUDE_ONCE
#define TEST_PROGRAMS_INCLUDE_ONCE

#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/Colors.hpp>
#include <vlGraphics/GeometryLoadCallback.hpp>
#include "Applets/BaseDemo.hpp"

BaseDemo* Create_App_BlindTests();
BaseDemo* Create_App_Billboards();
BaseDemo* Create_App_ClipPlanes();
BaseDemo* Create_App_Deformer();
BaseDemo* Create_App_DrawPixels();
BaseDemo* Create_App_Framebuffer_Object(int);
BaseDemo* Create_App_GeometryInstancing();
BaseDemo* Create_App_GLSL();
BaseDemo* Create_App_GLSL_Bumpmapping();
BaseDemo* Create_App_GLSLImageProcessing();
BaseDemo* Create_App_Lights();
BaseDemo* Create_App_Fractals();
BaseDemo* Create_App_MiniEarth();
BaseDemo* Create_App_ModelProfiler();
BaseDemo* Create_App_MorphAnimation();
BaseDemo* Create_App_MultipleCameras();
BaseDemo* Create_App_PointSplatting();
BaseDemo* Create_App_PolyDepthSorting(const vl::String&);
BaseDemo* Create_App_PolygonReduction(const vl::String&);
BaseDemo* Create_App_RenderOrder();
BaseDemo* Create_App_RotatingCube();
BaseDemo* Create_App_ScatterPlot3D(int);
BaseDemo* Create_App_ShaderMultiPassLODAnim();
BaseDemo* Create_App_ShaderMultiPassAlpha();
BaseDemo* Create_App_GeomLODAnim();
BaseDemo* Create_App_Terrain();
BaseDemo* Create_App_TextRendering(int);
BaseDemo* Create_App_ImageFunctions();
BaseDemo* Create_App_Texturing();
BaseDemo* Create_App_Transforms();
BaseDemo* Create_App_VolumeRaycast();
BaseDemo* Create_App_VolumeSliced();
BaseDemo* Create_App_VectorGraphics();
BaseDemo* Create_App_KdTreeView();
BaseDemo* Create_App_CullingBenchmark();
BaseDemo* Create_App_MarchingCubes();
BaseDemo* Create_App_Interpolators();
BaseDemo* Create_App_Extrusion();
BaseDemo* Create_App_Tessellator();
BaseDemo* Create_App_TessellationShader();
BaseDemo* Create_App_Molecules();
BaseDemo* Create_App_EdgeRendering();
BaseDemo* Create_App_PortalCulling();
BaseDemo* Create_App_OcclusionCulling();
BaseDemo* Create_App_BezierSurfaces();
BaseDemo* Create_App_Picking();
BaseDemo* Create_App_NearFarOptimization();
BaseDemo* Create_App_EffectOverride();
BaseDemo* Create_App_ShaderOverride();
BaseDemo* Create_App_Primitives();
BaseDemo* Create_App_DrawCalls();

// win32 console for sdtout output
#if defined(WIN32) && !defined(NDEBUG)
  #include <io.h>
  #include <fcntl.h>
#endif

class TestBattery
{
public:
  virtual void runGUI(const vl::String& title, BaseDemo* applet, vl::OpenGLContextFormat format, int x, int y, int width, int height, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center) = 0;

  void setupApplet(BaseDemo* applet, vl::OpenGLContext* oglcontext, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center)
  {
    /* log test title */
    vl::Log::print( "******* " + applet->appletName() + " *******\n\n");
    /* initialize the applet */
    applet->initialize();
    /* bind the applet so it receives all the GUI events related to the OpenGLContext */
    oglcontext->addEventListener(applet);
    /* target the window so we can render on it */
    applet->rendering()->as<vl::Rendering>()->renderer()->setRenderTarget( oglcontext->renderTarget() );
    /* black background */
    applet->rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( bk_color );
    /* define the camera position and orientation */
    vl::vec3 up = vl::vec3(0,1,0);
    vl::mat4 view_mat = vl::mat4::getLookAt(eye, center, up);
    applet->rendering()->as<vl::Rendering>()->camera()->setLocalMatrix( view_mat );
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

  void run(int test, const vl::OpenGLContextFormat& format)
  {
    TestEntry tests[] = 
    {
      { "Blind Tests", Create_App_BlindTests(), 10,10, 512, 512, vl::black, vl::vec3(0,0,1), vl::vec3(0,0,0) }, 
      { "Primitives", Create_App_Primitives(), 10,10, 512, 512, vl::white, vl::vec3(0,0.75,-2.5), vl::vec3(0,0,-4) },
      // { "DrawCall Tests", Create_App_DrawCalls(), 10,10, 512, 512, vl::white, vl::vec3(0,0,30), vl::vec3(0,0,0) },
      { "Near/Far Planes Optimization", Create_App_NearFarOptimization(), 10, 10, 512, 512, vl::black, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Effect Override", Create_App_EffectOverride(), 10,10, 512, 512, vl::white, vl::vec3(0,0,30), vl::vec3(0,0,0) },
      { "Shader Override", Create_App_ShaderOverride(), 10,10, 512, 512, vl::white, vl::vec3(0,0,30), vl::vec3(0,0,0) },
      { "Shader Multipass+LOD+Animation", Create_App_ShaderMultiPassLODAnim(), 10, 10, 512, 512, vl::black, vl::vec3(0,40,40), vl::vec3(0,0,0) }, 
      { "Shader Alpha Multipass", Create_App_ShaderMultiPassAlpha(), 10, 10, 512, 512, vl::black, vl::vec3(0,40,40), vl::vec3(0,0,0) }, 
      { "Geometry LOD+Animation", Create_App_GeomLODAnim(), 10, 10, 512, 512, vl::black, vl::vec3(0,40,40), vl::vec3(0,0,0) }, 
      { "Render Order", Create_App_RenderOrder(), 10, 10, 512, 512, vl::white, vl::vec3(0,1,5), vl::vec3(0,0,0) }, 
      { "Robot Transform", Create_App_Transforms(), 10, 10, 512, 512, vl::white, vl::vec3(0,40,60), vl::vec3(0,15,0) }, 
      { "Billboards", Create_App_Billboards(), 10, 10, 512, 512, vl::royalblue, vl::vec3(0,1,10), vl::vec3(0,1,0) }, 
      { "Multiple Cameras", Create_App_MultipleCameras(), 10, 10, 512, 512, vl::black, vl::vec3(0,10,15), vl::vec3(0,0,0) }, 
      { "Lights", Create_App_Lights(), 10,10, 512, 512, vl::black, vl::vec3(-10,10,10), vl::vec3(0,0,0) }, 
      { "Clipping Planes", Create_App_ClipPlanes(), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,5), vl::vec3(0,0,0) }, 
      { "DrawPixels", Create_App_DrawPixels(), 10,10, 512, 512, vl::black, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "Image Functions", Create_App_ImageFunctions(), 10,10, 512, 512, vl::darkturquoise, vl::vec3(0,0,20), vl::vec3(0,0,0) }, 
      { "Texturing", Create_App_Texturing(), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,20), vl::vec3(0,0,0) }, 
      { "100.000 Scatter Plot", Create_App_ScatterPlot3D(0), 10, 10, 512, 512, vl::black, vl::vec3(0,500,1200), vl::vec3(0,0,0) }, 
      { "100.000 Scatter Plot & PointSprite", Create_App_ScatterPlot3D(1), 10, 10, 512, 512, vl::black, vl::vec3(0,500,1200), vl::vec3(0,0,0) }, 
      { "GPU Morph. 1000", Create_App_MorphAnimation(), 10, 10, 512, 512, vl::black, vl::vec3(0,1000,1500), vl::vec3(0,0,0) }, 
      { "Text - The Raven", Create_App_TextRendering(0), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - Alignment", Create_App_TextRendering(1), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - Rotation", Create_App_TextRendering(2), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - Multilingual", Create_App_TextRendering(3), 10, 10, 512, 512, vl::gold, vl::vec3(0,0,30), vl::vec3(0,0,0) }, 
      { "Text - The Solar System", Create_App_TextRendering(4), 10, 10, 512, 512, vl::black, vl::vec3(0,35,40), vl::vec3(0,0,0) }, 
      { "GLSL", Create_App_GLSL(), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,7), vl::vec3(0,0,0) }, 
      { "GLSL Bump Mapping", Create_App_GLSL_Bumpmapping(), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "GLSL Image Processing", Create_App_GLSLImageProcessing(), 10,10, 512, 512, vl::black, vl::vec3(0,0,35), vl::vec3(0,0,0) }, 
      { "GLSL Mandelbrot f32", Create_App_Fractals(), 10,10, 512, 512, vl::black, vl::vec3(0,0,35), vl::vec3(0,0,0) }, 
      { "Legacy Render To Texture", Create_App_Framebuffer_Object(5), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Render To Texture", Create_App_Framebuffer_Object(0), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Render To Texture MRT", Create_App_Framebuffer_Object(1), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Copy To Texture From Color Buffer", Create_App_Framebuffer_Object(2), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Framebuffer Blit Multisample", Create_App_Framebuffer_Object(3), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "FBO Render To Multisample Texture", Create_App_Framebuffer_Object(4), 10, 10, 512, 512, vl::skyblue, vl::vec3(0,0,100), vl::vec3(0,0,0) }, 
      { "Geometry Instancing", Create_App_GeometryInstancing(), 10,10, 512, 512, vl::black, vl::vec3(45/2,60,90), vl::vec3(45/2,45/2,45/2) }, 
      { "Polygon Depth Sorting", Create_App_PolyDepthSorting("/models/3ds/monkey.3ds"), 10,10, 512, 512, vl::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Polygon Reduction", Create_App_PolygonReduction("/models/3ds/monkey.3ds"), 10,10, 512, 512, vl::black, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "Simple Terrain", Create_App_Terrain(), 10,10, 512, 512, vl::black, vl::vec3(0,5,0), vl::vec3(0,2,-10) }, 
      { "Vector Graphics", Create_App_VectorGraphics(), 10,10, 512, 512, vl::lightgray, vl::vec3(0,0,10), vl::vec3(0,0,0) }, 
      { "Culling Benchmark", Create_App_CullingBenchmark(), 10,10, 512, 512, vl::black, vl::vec3(0,500,-250), vl::vec3(0,500,-250-1) }, 
      { "ActorKdTree View", Create_App_KdTreeView(), 10,10, 512, 512, vl::black, vl::vec3(10,10,-10), vl::vec3(0,0,0) }, 
      { "Model Profiler", Create_App_ModelProfiler(), 10,10, 640, 480, vl::skyblue, vl::vec3(0,0,0), vl::vec3(0,0,-1) }, 
      { "Deformer", Create_App_Deformer(), 10,10, 512, 512, vl::black, vl::vec3(0,0,35), vl::vec3(0,0,0) }, 
      { "Volume Point Splatting", Create_App_PointSplatting(), 10,10, 512, 512, vl::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Volume Sliced", Create_App_VolumeSliced(), 10,10, 512, 512, vl::black, vl::vec3(0,10,35), vl::vec3(0,0,0) }, 
      { "Volume Raycast", Create_App_VolumeRaycast(), 10,10, 512, 512, vl::black, vl::vec3(0,10,35), vl::vec3(0,0,0) },      
      { "Marching Cubes", Create_App_MarchingCubes(), 10,10, 512, 512, vl::black, vl::vec3(0,10,50), vl::vec3(0,0,0) }, 
#if defined(VL_OPENGL)
      { "Tessellator", Create_App_Tessellator(), 10,10, 512, 512, vl::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Extrusion", Create_App_Extrusion(), 10,10, 512, 512, vl::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
#endif
      { "Interpolator", Create_App_Interpolators(), 10,10, 512, 512, vl::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Molecule", Create_App_Molecules(), 10,10, 512, 512, vl::black, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Edge Enhancement", Create_App_EdgeRendering(), 10,10, 512, 512, vl::white, vl::vec3(0,0,20), vl::vec3(0,0,0) },
      { "Portal Culling", Create_App_PortalCulling(), 10,10, 512, 512, vl::white, vl::vec3(20*7/2.0f-10,0,20*7/2.0f-10), vl::vec3(20*7/2.0f-10,0,20*7/2.0f-1.0f) },
      { "Occlusion Culling", Create_App_OcclusionCulling(), 10,10, 512, 512, vl::gray, vl::vec3(0,25,575), vl::vec3(0,0,0) },
      { "Bezier Patches", Create_App_BezierSurfaces(), 10,10, 512, 512, vl::black, vl::vec3(4.5f,5,13), vl::vec3(4.5f,0,0) },
      { "Picking", Create_App_Picking(), 10,10, 512, 512, vl::black, vl::vec3(0,0,10), vl::vec3(0,0,0) },
      { "Tessellation Shader", Create_App_TessellationShader(), 10,10, 512, 512, vl::skyblue, vl::vec3(300,40,0), vl::vec3(1000,0,0) },
      // { "Mini Earth", Create_App_MiniEarth(), 10,10, 512, 512, vl::black, vl::vec3(0,0,4), vl::vec3(0,0,0) },
    };

    int test_count = int( sizeof(tests)/sizeof(TestEntry) );
    if (test > 0 && test-1 < test_count)
    {
      /* open a console so we can see the applet's output on stdout */
      vl::showWin32Console();

      /* init Visualization Library */
      vl::VisualizationLibrary::init();

      /* install GeometryLoadCallback for computing normals and converting geometry to GLES friendly format */
      vl::ref<vl::GeometryLoadCallback> glc = new vl::GeometryLoadCallback;
#if defined(VL_OPENGL_ES1) || defined(VL_OPENGL_ES2)
      glc->setMakeGLESFriendly(true);
#endif
      vl::defLoadWriterManager()->loadCallbacks()->push_back(glc.get());

      /* run test */
      runGUI(tests[test-1].title, tests[test-1].applet.get(), format, tests[test-1].x, tests[test-1].y, tests[test-1].width, tests[test-1].height, tests[test-1].bk_color, tests[test-1].eye, tests[test-1].center );

      /* shutdown Visualization Library */
      vl::VisualizationLibrary::shutdown();
    }
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

#if VL_DEBUG_LIVING_OBJECTS
    std::set< vl::Object* >* obj_database = vl::Object::debug_living_objects() ;
    printf( "Living objects: %d\n", obj_database->size() );
    std::set< vl::Object* >::iterator it = obj_database->begin();
    for( ; it != obj_database->end(); ++it )
    {
      printf( "- %s\n", (*it)->objectName().c_str() );
    }

    while( obj_database->size() ) { /*infinite loop*/ }
#endif

  }
};

#endif
