/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://visualizationlibrary.org                                                   */
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
#include <vlCore/glsl_math.hpp>
#include <vlGraphics/GLSL.hpp>
#include <vlGraphics/Light.hpp>
#include <vlGraphics/Text.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlCore/VisualizationLibrary.hpp>
#include <vlCore/FileSystem.hpp>
#include <vlCore/DiskDirectory.hpp>
#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/LoadWriterManager.hpp>
#include <vlGraphics/TriangleStripGenerator.hpp>
#include <vlGraphics/DoubleVertexRemover.hpp>
#include <vlGraphics/FontManager.hpp>
#include <vlGraphics/plugins/ioVLX.hpp>

// #include <nvModel.h>
// #include <nvShaderUtils.h>
// #include <nvSDKPath.h>
// #include "GLSLProgramObject.h"
// #include "Timer.h"
// #include "OSD.h"
// #include <GL/glew.h>
// #include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <time.h>

#define FOVY 30.0
#define ZNEAR 0.0001
#define ZFAR 10.0
#define FPS_TIME_WINDOW 1
#define MAX_DEPTH 1.0

int g_numPasses = 4;
int g_imageWidth = 1024;
int g_imageHeight = 768;

// nv::Model *g_model;
GLuint g_quadDisplayList;
GLuint g_vboId;
GLuint g_eboId;

bool g_useOQ = true;
GLuint g_queryId;

#define MODEL_FILENAME "/depth-peeling/monkey.3ds"
#define SHADER_PATH "/depth-peeling/glsl/"

// static nv::SDKPath sdkPath;

vl::ref<vl::GLSLProgram> g_shaderDualInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderDualPeel = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderDualBlend = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderDualFinal = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontPeel = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontBlend = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderFrontFinal = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderAverageInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderAverageFinal = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderWeightedSumInit = new vl::GLSLProgram();
vl::ref<vl::GLSLProgram> g_shaderWeightedSumFinal = new vl::GLSLProgram();

vl::ref<vl::Uniform> g_uniformAlpha = new vl::Uniform("Alpha");
vl::ref<vl::Uniform> g_uniformBackgroundColor = new vl::Uniform("BackgroundColor");

enum {
	DUAL_PEELING_MODE,
	F2B_PEELING_MODE,
	WEIGHTED_AVERAGE_MODE,
	WEIGHTED_SUM_MODE
};

float g_opacity = 0.6;
char g_mode = DUAL_PEELING_MODE;
bool g_showOsd = true;
bool g_bShowUI = true;
unsigned g_numGeoPasses = 0;

int g_rotating = 0;
int g_panning = 0;
int g_scaling = 0;
int g_oldX, g_oldY;
int g_newX, g_newY;
float g_bbScale = 1.0;
vl::fvec3 g_bbTrans(0.0, 0.0, 0.0);
vl::fvec2 g_rot(0.0, 45.0);
vl::fvec3 g_pos(0.0, 0.0, 2.0);

float g_white[3] = {1.0,1.0,1.0};
float g_black[3] = {0.0};
float *g_backgroundColor = g_white;

GLuint g_dualBackBlenderFboId;
GLuint g_dualPeelingSingleFboId;
GLuint g_dualDepthTexId[2];
GLuint g_dualFrontBlenderTexId[2];
GLuint g_dualBackTempTexId[2];
GLuint g_dualBackBlenderTexId;

GLuint g_frontFboId[2];
GLuint g_frontDepthTexId[2];
GLuint g_frontColorTexId[2];
GLuint g_frontColorBlenderTexId;
GLuint g_frontColorBlenderFboId;

GLuint g_accumulationTexId[2];
GLuint g_accumulationFboId;

GLenum g_drawBuffers[] = {GL_COLOR_ATTACHMENT0,
					     GL_COLOR_ATTACHMENT1,
					     GL_COLOR_ATTACHMENT2,
					     GL_COLOR_ATTACHMENT3,
					     GL_COLOR_ATTACHMENT4,
					     GL_COLOR_ATTACHMENT5,
					     GL_COLOR_ATTACHMENT6
};

#if 0
#define CHECK_GL_ERRORS  \
{ \
    GLenum err = glGetError(); \
    if (err) \
        printf( "Error %x at line %d\n", err, __LINE__); \
}
#else
#define CHECK_GL_ERRORS {}
#endif

// using namespace vl;

class App_DepthPeeling: public BaseDemo
{
public:
  //--------------------------------------------------------------------------
  void InitDualPeelingRenderTargets()
  {
    glGenTextures(2, g_dualDepthTexId);
    glGenTextures(2, g_dualFrontBlenderTexId);
    glGenTextures(2, g_dualBackTempTexId);
    vl::glGenFramebuffers(1, &g_dualPeelingSingleFboId);
    for (int i = 0; i < 2; i++)
    {
	    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[i]);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RG32F, g_imageWidth, g_imageHeight,
				     0, GL_RGB, GL_FLOAT, 0);

	    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[i]);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, g_imageWidth, g_imageHeight,
				     0, GL_RGBA, GL_FLOAT, 0);

	    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualBackTempTexId[i]);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, g_imageWidth, g_imageHeight,
				     0, GL_RGBA, GL_FLOAT, 0);
    }

    glGenTextures(1, &g_dualBackBlenderTexId);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_dualBackBlenderTexId);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, g_imageWidth, g_imageHeight,
			     0, GL_RGB, GL_FLOAT, 0);

    vl::glGenFramebuffers(1, &g_dualBackBlenderFboId);
    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualBackBlenderFboId);
    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						      GL_TEXTURE_RECTANGLE_ARB, g_dualBackBlenderTexId, 0);

    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingSingleFboId);

    int j = 0;
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							      GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[j], 0);
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
							      GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[j], 0);
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
							      GL_TEXTURE_RECTANGLE_ARB, g_dualBackTempTexId[j], 0);

    j = 1;
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
							      GL_TEXTURE_RECTANGLE_ARB, g_dualDepthTexId[j], 0);
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4,
							      GL_TEXTURE_RECTANGLE_ARB, g_dualFrontBlenderTexId[j], 0);
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5,
							      GL_TEXTURE_RECTANGLE_ARB, g_dualBackTempTexId[j], 0);

    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6,
						      GL_TEXTURE_RECTANGLE_ARB, g_dualBackBlenderTexId, 0);

    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void DeleteDualPeelingRenderTargets()
  {
    vl::glDeleteFramebuffers(1, &g_dualBackBlenderFboId);
    vl::glDeleteFramebuffers(1, &g_dualPeelingSingleFboId);
    glDeleteTextures(2, g_dualDepthTexId);
    glDeleteTextures(2, g_dualFrontBlenderTexId);
    glDeleteTextures(2, g_dualBackTempTexId);
    glDeleteTextures(1, &g_dualBackBlenderTexId);
  }

  //--------------------------------------------------------------------------
  void InitFrontPeelingRenderTargets()
  {
    glGenTextures(2, g_frontDepthTexId);
    glGenTextures(2, g_frontColorTexId);
    vl::glGenFramebuffers(2, g_frontFboId);

    for (int i = 0; i < 2; i++)
    {
	    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_frontDepthTexId[i]);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_DEPTH_COMPONENT32F,
				     g_imageWidth, g_imageHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_frontColorTexId[i]);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, g_imageWidth, g_imageHeight,
				     0, GL_RGBA, GL_FLOAT, 0);

	    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontFboId[i]);
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
							      GL_TEXTURE_RECTANGLE_ARB, g_frontDepthTexId[i], 0);
	    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							      GL_TEXTURE_RECTANGLE_ARB, g_frontColorTexId[i], 0);
    }

    glGenTextures(1, &g_frontColorBlenderTexId);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_frontColorBlenderTexId);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, g_imageWidth, g_imageHeight,
			     0, GL_RGBA, GL_FLOAT, 0);

    vl::glGenFramebuffers(1, &g_frontColorBlenderFboId);
    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontColorBlenderFboId);
    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
						      GL_TEXTURE_RECTANGLE_ARB, g_frontDepthTexId[0], 0);
    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						      GL_TEXTURE_RECTANGLE_ARB, g_frontColorBlenderTexId, 0);
    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void DeleteFrontPeelingRenderTargets()
  {
    vl::glDeleteFramebuffers(2, g_frontFboId);
    vl::glDeleteFramebuffers(1, &g_frontColorBlenderFboId);
    glDeleteTextures(2, g_frontDepthTexId);
    glDeleteTextures(2, g_frontColorTexId);
    glDeleteTextures(1, &g_frontColorBlenderTexId);
  }

  //--------------------------------------------------------------------------
  void InitAccumulationRenderTargets()
  {
    glGenTextures(2, g_accumulationTexId);

    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_accumulationTexId[0]);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA16F_ARB,
			     g_imageWidth, g_imageHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, g_accumulationTexId[1]);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_R32F,
			     g_imageWidth, g_imageHeight, 0, GL_RGBA, GL_FLOAT, NULL);

    vl::glGenFramebuffers(1, &g_accumulationFboId);
    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_accumulationFboId);
    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
						      GL_TEXTURE_RECTANGLE_ARB, g_accumulationTexId[0], 0);
    vl::glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
						      GL_TEXTURE_RECTANGLE_ARB, g_accumulationTexId[1], 0);

    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void DeleteAccumulationRenderTargets()
  {
    vl::glDeleteFramebuffers(1, &g_accumulationFboId);
    glDeleteTextures(2, g_accumulationTexId);
  }

  //--------------------------------------------------------------------------
  void MakeFullScreenQuad()
  {
    g_quadDisplayList = glGenLists(1);
    glNewList(g_quadDisplayList, GL_COMPILE);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, 1.0, 0.0, 1.0);
    glBegin(GL_QUADS);
    {
	    glVertex2f(0.0, 0.0); 
	    glVertex2f(1.0, 0.0);
	    glVertex2f(1.0, 1.0);
	    glVertex2f(0.0, 1.0);
    }
    glEnd();
    glPopMatrix();

    glEndList();
  }

  //--------------------------------------------------------------------------
  /*void LoadModel(const char *model_filename)
  {
    g_model = new nv::Model;
    printf("loading OBJ...\n");

      std::string resolved_path;

      if (sdkPath.getFilePath( model_filename, resolved_path)) {
        if (!g_model->loadModelFromFile(resolved_path.c_str())) {
	        fprintf(stderr, "Error loading model '%s'\n", model_filename);
	        exit(1);
        }
      }
      else {
          fprintf(stderr, "Failed to find model '%s'\n", model_filename);
	    exit(1);
      }

    printf("compiling mesh...\n");
    g_model->compileModel();

    printf("%d vertices\n", g_model->getPositionCount());
    printf("%d triangles\n", g_model->getIndexCount()/3);
    int totalVertexSize = g_model->getCompiledVertexCount() * g_model->getCompiledVertexSize() * sizeof(GLfloat);
    int totalIndexSize = g_model->getCompiledIndexCount() * sizeof(GLuint);

    glGenBuffers(1, &g_vboId);
    glBindBuffer(GL_ARRAY_BUFFER, g_vboId);
      glBufferData(GL_ARRAY_BUFFER, totalVertexSize, g_model->getCompiledVertices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &g_eboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_eboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndexSize, g_model->getCompiledIndices(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    vl::fvec3 modelMin, modelMax;
    g_model->computeBoundingBox(modelMin, modelMax);

    vl::fvec3 diag = modelMax - modelMin;
    g_bbScale = 1.0 / diag.length() * 1.5;
    g_bbTrans = -g_bbScale * (modelMin + 0.5f * (modelMax - modelMin));
  }*/

  //--------------------------------------------------------------------------
  /*void DrawModel()
  {
    glBindBuffer(GL_ARRAY_BUFFER, g_vboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_eboId);
    int stride = g_model->getCompiledVertexSize() * sizeof(GLfloat);
    int normalOffset = g_model->getCompiledNormalOffset() * sizeof(GLfloat);
    glVertexPointer(g_model->getPositionSize(), GL_FLOAT, stride, NULL);
    glNormalPointer(GL_FLOAT, stride, (GLubyte *)NULL + normalOffset);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    glDrawElements(GL_TRIANGLES, g_model->getCompiledIndexCount(), GL_UNSIGNED_INT, NULL);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);

    g_numGeoPasses++;
  }*/

  //--------------------------------------------------------------------------
  void BuildShaders()
  {
    printf("\nloading shaders...\n");

    g_shaderDualInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_init_vertex.glsl" ) );
    g_shaderDualInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_init_fragment.glsl" ) );
    g_shaderDualInit->linkProgram();

    g_shaderDualPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
    g_shaderDualPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_peel_vertex.glsl" ) );
    g_shaderDualPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
    g_shaderDualPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_peel_fragment.glsl" ) );
    g_shaderDualPeel->linkProgram();

    g_shaderDualBlend->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_blend_vertex.glsl" ) );
    g_shaderDualBlend->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_blend_fragment.glsl" ) );
    g_shaderDualBlend->linkProgram();

    g_shaderDualFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "dual_peeling_final_vertex.glsl" ) );
    g_shaderDualFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "dual_peeling_final_fragment.glsl" ) );
    g_shaderDualFinal->linkProgram();

    g_shaderFrontInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
    g_shaderFrontInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_init_vertex.glsl" ) );
    g_shaderFrontInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
    g_shaderFrontInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_init_fragment.glsl" ) );
    g_shaderFrontInit->linkProgram();

    g_shaderFrontPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
    g_shaderFrontPeel->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_peel_vertex.glsl" ) );
    g_shaderFrontPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
    g_shaderFrontPeel->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_peel_fragment.glsl" ) );
    g_shaderFrontPeel->linkProgram();

    g_shaderFrontBlend->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_blend_vertex.glsl" ) );
    g_shaderFrontBlend->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_blend_fragment.glsl" ) );
    g_shaderFrontBlend->linkProgram();

    g_shaderFrontFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "front_peeling_final_vertex.glsl" ) );
    g_shaderFrontFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "front_peeling_final_fragment.glsl" ) );
    g_shaderFrontFinal->linkProgram();

    g_shaderAverageInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
    g_shaderAverageInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wavg_init_vertex.glsl" ) );
    g_shaderAverageInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
    g_shaderAverageInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wavg_init_fragment.glsl" ) );
    g_shaderAverageInit->linkProgram();

    g_shaderAverageFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wavg_final_vertex.glsl" ) );
    g_shaderAverageFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wavg_final_fragment.glsl" ) );
    g_shaderAverageFinal->linkProgram();

    g_shaderWeightedSumInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "shade_vertex.glsl" ) );
    g_shaderWeightedSumInit->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wsum_init_vertex.glsl" ) );
    g_shaderWeightedSumInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "shade_fragment.glsl" ) );
    g_shaderWeightedSumInit->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wsum_init_fragment.glsl" ) );
    g_shaderWeightedSumInit->linkProgram();

    g_shaderWeightedSumFinal->attachShader( new vl::GLSLVertexShader( SHADER_PATH "wsum_final_vertex.glsl" ) );
    g_shaderWeightedSumFinal->attachShader( new vl::GLSLFragmentShader(SHADER_PATH "wsum_final_fragment.glsl" ) );
    g_shaderWeightedSumFinal->linkProgram();
  }

  //--------------------------------------------------------------------------
  /*void DestroyShaders()
  {
    g_shaderDualInit.destroy();
    g_shaderDualPeel.destroy();
    g_shaderDualBlend.destroy();
    g_shaderDualFinal.destroy();

    g_shaderFrontInit.destroy();
    g_shaderFrontPeel.destroy();
    g_shaderFrontBlend.destroy();
    g_shaderFrontFinal.destroy();

    g_shaderAverageInit.destroy();
    g_shaderAverageFinal.destroy();

    g_shaderWeightedSumInit.destroy();
    g_shaderWeightedSumFinal.destroy();
  }*/

  //--------------------------------------------------------------------------
  void ReloadShaders()
  {
    // DestroyShaders();
    // BuildShaders();
  }

  //--------------------------------------------------------------------------
  void InitGL()
  { 
    // Allocate render targets first
    InitDualPeelingRenderTargets();
    InitFrontPeelingRenderTargets();
    InitAccumulationRenderTargets();
    vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);

    BuildShaders();
    // LoadModel(MODEL_FILENAME); // FIXME
    MakeFullScreenQuad();

    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_NORMALIZE);

    vl::glGenQueries(1, &g_queryId);
  }

  //--------------------------------------------------------------------------

  void bindTexture(vl::GLSLProgram* glsl, GLenum target, std::string texname, GLuint texid, int texunit)
  {
	  // setTextureUnit(texname, texunit);
    int location = glsl->getUniformLocation(texname.c_str());
	  if (location == -1) {
		  std::cerr << "Warning: Invalid texture " << texname << std::endl;
		  return;
	  }
    vl::glUniform1i(location, texunit);

    glActiveTexture(GL_TEXTURE0 + texunit);
	  glBindTexture(target, texid);
	  glActiveTexture(GL_TEXTURE0);
  }

  //--------------------------------------------------------------------------

  void RenderDualPeeling()
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    // ---------------------------------------------------------------------
    // 1. Initialize Min-Max Depth Buffer
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingSingleFboId);

    // Render targets 1 and 2 store the front and back colors
    // Clear to 0.0 and use MAX blending to filter written color
    // At most one front color and one back color can be written every pass
    vl::glDrawBuffers(2, &g_drawBuffers[1]);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render target 0 stores (-minDepth, maxDepth, alphaMultiplier)
    glDrawBuffer(g_drawBuffers[0]);
    glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBlendEquation(GL_MAX);

    g_shaderDualInit->useProgram();
    g_shaderDualInit->applyUniformSet();
    // DrawModel(); // FIXME
    vl::glUseProgram(0); // g_shaderDualInit.unbind();

    CHECK_GL_ERRORS;

    // ---------------------------------------------------------------------
    // 2. Dual Depth Peeling + Blending
    // ---------------------------------------------------------------------

    // Since we cannot blend the back colors in the geometry passes,
    // we use another render target to do the alpha blending
    //vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualBackBlenderFboId);
    glDrawBuffer(g_drawBuffers[6]);
    glClearColor(g_backgroundColor[0], g_backgroundColor[1], g_backgroundColor[2], 0);
    glClear(GL_COLOR_BUFFER_BIT);

    int currId = 0;

    for (int pass = 1; g_useOQ || pass < g_numPasses; pass++) {
	    currId = pass % 2;
	    int prevId = 1 - currId;
	    int bufId = currId * 3;
		
	    //vl::glBindFramebuffer(GL_FRAMEBUFFER, g_dualPeelingFboId[currId]);

	    vl::glDrawBuffers(2, &g_drawBuffers[bufId+1]);
	    glClearColor(0, 0, 0, 0);
	    glClear(GL_COLOR_BUFFER_BIT);

	    glDrawBuffer(g_drawBuffers[bufId+0]);
	    glClearColor(-MAX_DEPTH, -MAX_DEPTH, 0, 0);
	    glClear(GL_COLOR_BUFFER_BIT);

	    // Render target 0: RG32F MAX blending
	    // Render target 1: RGBA MAX blending
	    // Render target 2: RGBA MAX blending
	    vl::glDrawBuffers(3, &g_drawBuffers[bufId+0]);
	    glBlendEquation(GL_MAX);

	    g_shaderDualPeel->useProgram();
	    bindTexture(g_shaderDualPeel.get(), GL_TEXTURE_RECTANGLE_ARB, "DepthBlenderTex", g_dualDepthTexId[prevId], 0);
	    bindTexture(g_shaderDualPeel.get(), GL_TEXTURE_RECTANGLE_ARB, "FrontBlenderTex", g_dualFrontBlenderTexId[prevId], 1);
	    
      // g_shaderDualPeel.setUniform("Alpha", (float*)&g_opacity, 1);
      g_shaderDualPeel->setUniform(g_uniformAlpha.get());
      g_uniformAlpha->setUniform(1, (float*)&g_opacity);
      g_shaderDualPeel->applyUniformSet();
      // DrawModel(); // FIXME
	    vl::glUseProgram(0); // g_shaderDualPeel.unbind();

	    CHECK_GL_ERRORS;

	    // Full screen pass to alpha-blend the back color
	    glDrawBuffer(g_drawBuffers[6]);

	    glBlendEquation(GL_FUNC_ADD);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	    if (g_useOQ) {
        vl::glBeginQuery(GL_SAMPLES_PASSED_ARB, g_queryId);
	    }

	    g_shaderDualBlend->useProgram();
	    bindTexture(g_shaderDualBlend.get(), GL_TEXTURE_RECTANGLE_ARB, "TempTex", g_dualBackTempTexId[currId], 0);
      g_shaderDualBlend->applyUniformSet();
	    glCallList(g_quadDisplayList);
	    vl::glUseProgram(0); // g_shaderDualBlend.unbind();

	    CHECK_GL_ERRORS;

	    if (g_useOQ) {
		    vl::glEndQuery(GL_SAMPLES_PASSED_ARB);
		    GLuint sample_count;
		    vl::glGetQueryObjectuiv(g_queryId, GL_QUERY_RESULT_ARB, &sample_count);
		    if (sample_count == 0) {
			    break;
		    }
	    }
    }

    glDisable(GL_BLEND);

    // ---------------------------------------------------------------------
    // 3. Final Pass
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    g_shaderDualFinal->useProgram();
    bindTexture(g_shaderDualFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "DepthBlenderTex", g_dualDepthTexId[currId], 0);
    bindTexture(g_shaderDualFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "FrontBlenderTex", g_dualFrontBlenderTexId[currId], 1);
    bindTexture(g_shaderDualFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "BackBlenderTex", g_dualBackBlenderTexId, 2);
    g_shaderDualFinal->applyUniformSet();
    glCallList(g_quadDisplayList);
    vl::glUseProgram(0); // g_shaderDualFinal.unbind();

    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void RenderFrontToBackPeeling()
  {
    // ---------------------------------------------------------------------
    // 1. Initialize Min Depth Buffer
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontColorBlenderFboId);
    glDrawBuffer(g_drawBuffers[0]);

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    g_shaderFrontInit->useProgram();
    // g_shaderFrontInit.setUniform("Alpha", (float*)&g_opacity, 1);
    g_shaderFrontInit->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_opacity);    
    g_shaderFrontInit->applyUniformSet();
    // DrawModel(); // FIXME
    vl::glUseProgram(0); // g_shaderFrontInit.unbind();

    CHECK_GL_ERRORS;

    // ---------------------------------------------------------------------
    // 2. Depth Peeling + Blending
    // ---------------------------------------------------------------------

    int numLayers = (g_numPasses - 1) * 2;
    for (int layer = 1; g_useOQ || layer < numLayers; layer++) {
	    int currId = layer % 2;
	    int prevId = 1 - currId;

	    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontFboId[currId]);
	    glDrawBuffer(g_drawBuffers[0]);

	    glClearColor(0, 0, 0, 0);
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	    glDisable(GL_BLEND);
	    glEnable(GL_DEPTH_TEST);

	    if (g_useOQ) {
		    vl::glBeginQuery(GL_SAMPLES_PASSED_ARB, g_queryId);
	    }

	    g_shaderFrontPeel->useProgram();
	    bindTexture(g_shaderFrontPeel.get(), GL_TEXTURE_RECTANGLE_ARB, "DepthTex", g_frontDepthTexId[prevId], 0);
      // g_shaderFrontPeel.setUniform("Alpha", (float*)&g_opacity, 1);
      g_shaderFrontPeel->setUniform(g_uniformAlpha.get());
      g_uniformAlpha->setUniform(1, (float*)&g_opacity);    
	    g_shaderFrontPeel->applyUniformSet();
      // DrawModel(); // FIXME
	    vl::glUseProgram(0); // g_shaderFrontPeel.unbind();

	    if (g_useOQ) {
		    vl::glEndQuery(GL_SAMPLES_PASSED_ARB);
	    }

	    CHECK_GL_ERRORS;

	    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_frontColorBlenderFboId);
	    glDrawBuffer(g_drawBuffers[0]);

	    glDisable(GL_DEPTH_TEST);
	    glEnable(GL_BLEND);

	    glBlendEquation(GL_FUNC_ADD);
	    vl::glBlendFuncSeparate(GL_DST_ALPHA, GL_ONE,
						    GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		
	    g_shaderFrontBlend->useProgram();
	    bindTexture(g_shaderFrontBlend.get(), GL_TEXTURE_RECTANGLE_ARB, "TempTex", g_frontColorTexId[currId], 0);
      g_shaderFrontBlend->applyUniformSet();
	    glCallList(g_quadDisplayList);
	    vl::glUseProgram(0); // g_shaderFrontBlend.unbind();

	    glDisable(GL_BLEND);

	    CHECK_GL_ERRORS;

	    if (g_useOQ) {
		    GLuint sample_count;
		    vl::glGetQueryObjectuiv(g_queryId, GL_QUERY_RESULT_ARB, &sample_count);
		    if (sample_count == 0) {
			    break;
		    }
	    }
    }

    // ---------------------------------------------------------------------
    // 3. Final Pass
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);
    glDisable(GL_DEPTH_TEST);

    g_shaderFrontFinal->useProgram();

    // g_shaderFrontFinal.setUniform("BackgroundColor", g_backgroundColor, 3);
    g_shaderFrontFinal->setUniform(g_uniformBackgroundColor.get());
    g_uniformBackgroundColor->setUniform(3, g_backgroundColor);

    bindTexture(g_shaderFrontFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "ColorTex", g_frontColorBlenderTexId, 0);
    g_shaderFrontFinal->applyUniformSet();
    glCallList(g_quadDisplayList);
    vl::glUseProgram(0); // g_shaderFrontFinal.unbind();

    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void RenderAverageColors()
  {
    glDisable(GL_DEPTH_TEST);

    // ---------------------------------------------------------------------
    // 1. Accumulate Colors and Depth Complexity
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_accumulationFboId);
    vl::glDrawBuffers(2, g_drawBuffers);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);

    g_shaderAverageInit->useProgram();
    
    // g_shaderAverageInit.setUniform("Alpha", (float*)&g_opacity, 1);
    g_shaderAverageInit->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_opacity);    
    g_shaderAverageInit->applyUniformSet();
    // DrawModel(); // FIXME
    vl::glUseProgram(0); // g_shaderAverageInit.unbind();

    glDisable(GL_BLEND);

    CHECK_GL_ERRORS;

    // ---------------------------------------------------------------------
    // 2. Approximate Blending
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    g_shaderAverageFinal->useProgram();
    // g_shaderAverageFinal.setUniform("BackgroundColor", g_backgroundColor, 3);
    g_shaderAverageFinal->setUniform(g_uniformBackgroundColor.get());
    g_uniformBackgroundColor->setUniform(3, g_backgroundColor);

    bindTexture(g_shaderAverageFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "ColorTex0", g_accumulationTexId[0], 0);
    bindTexture(g_shaderAverageFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "ColorTex1", g_accumulationTexId[1], 1);
    g_shaderAverageFinal->applyUniformSet();
    glCallList(g_quadDisplayList);
    vl::glUseProgram(0); // g_shaderAverageFinal.unbind();

    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void RenderWeightedSum()
  {
    glDisable(GL_DEPTH_TEST);

    // ---------------------------------------------------------------------
    // 1. Accumulate (alpha * color) and (alpha)
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, g_accumulationFboId);
    glDrawBuffer(g_drawBuffers[0]);

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);

    g_shaderWeightedSumInit->useProgram();
    // g_shaderWeightedSumInit.setUniform("Alpha", (float*)&g_opacity, 1);
    g_shaderWeightedSumInit->setUniform(g_uniformAlpha.get());
    g_uniformAlpha->setUniform(1, (float*)&g_opacity);    
    g_shaderWeightedSumInit->applyUniformSet();
    // DrawModel(); // FIXME
    vl::glUseProgram(0); // g_shaderWeightedSumInit.unbind();

    glDisable(GL_BLEND);

    CHECK_GL_ERRORS;

    // ---------------------------------------------------------------------
    // 2. Weighted Sum
    // ---------------------------------------------------------------------

    vl::glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    g_shaderWeightedSumFinal->useProgram();
    // g_shaderWeightedSumFinal.setUniform("BackgroundColor", g_backgroundColor, 3);
    g_shaderWeightedSumFinal->setUniform(g_uniformBackgroundColor.get());
    g_uniformBackgroundColor->setUniform(3, g_backgroundColor);

    bindTexture(g_shaderWeightedSumFinal.get(), GL_TEXTURE_RECTANGLE_ARB, "ColorTex", g_accumulationTexId[0], 0);
    g_shaderWeightedSumFinal->applyUniformSet();
    glCallList(g_quadDisplayList);
    vl::glUseProgram(0); // g_shaderWeightedSumFinal.unbind();

    CHECK_GL_ERRORS;
  }

  //--------------------------------------------------------------------------
  void display()
  {
    static double s_t0 = vl::Time::currentTime();
    g_numGeoPasses = 0;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(g_pos.x(), g_pos.y(), g_pos.z(), g_pos.x(), g_pos.y(), 0, 0, 1, 0);
    glRotatef(g_rot.x(), 1, 0, 0);
    glRotatef(g_rot.y(), 0, 1, 0);
    glTranslatef(g_bbTrans.x(), g_bbTrans.y(), g_bbTrans.z());
    glScalef(g_bbScale, g_bbScale, g_bbScale);

    switch (g_mode) {
	    case DUAL_PEELING_MODE:
		    RenderDualPeeling();
		    break;
	    case F2B_PEELING_MODE:
		    RenderFrontToBackPeeling();
		    break;
	    case WEIGHTED_AVERAGE_MODE:
		    RenderAverageColors();
		    break;
	    case WEIGHTED_SUM_MODE:
		    RenderWeightedSum();
		    break;
    }

    // ---------------------------------------------------------------------

    static unsigned s_N = 0;
    s_N++;

    static float s_fps;
    double t1 = vl::Time::currentTime();
    double elapsedTime = t1 - s_t0;
    if (elapsedTime > FPS_TIME_WINDOW) {
	    s_fps = (float)s_N / elapsedTime;
	    s_N = 0;
	    s_t0 = t1;
    }

    if (g_showOsd) {
	    // DrawOsd(g_mode, g_opacity, g_numGeoPasses, s_fps);
    }

    // glutSwapBuffers(); // FIXME
  }

  //--------------------------------------------------------------------------
  void reshape(int w, int h)
  {
    if (g_imageWidth != w || g_imageHeight != h)
    {
	    g_imageWidth = w;
	    g_imageHeight = h;

	    DeleteDualPeelingRenderTargets();
	    InitDualPeelingRenderTargets();

	    DeleteFrontPeelingRenderTargets();
	    InitFrontPeelingRenderTargets();

	    DeleteAccumulationRenderTargets();
	    InitAccumulationRenderTargets();
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOVY, (float)g_imageWidth/(float)g_imageHeight, ZNEAR, ZFAR);
    glMatrixMode(GL_MODELVIEW);

    glViewport(0, 0, g_imageWidth, g_imageHeight);
  }

  //--------------------------------------------------------------------------
  void idle()
  {
       // // glutPostRedisplay(); // FIXME
  }

  //--------------------------------------------------------------------------
  //void mouseFunc(int button, int state, int x, int y)
  //{
  //	g_newX = x; g_newY = y;
  //
  //	if (button == GLUT_LEFT_BUTTON)
  //	{
  //		int mod = glutGetModifiers();
  //		if (mod == GLUT_ACTIVE_SHIFT) {
  //			g_scaling = !state;
  //		} else if (mod == GLUT_ACTIVE_CTRL) {
  //			g_panning = !state;
  //		} else {
  //			g_rotating = !state;
  //		}
  //	}
  //}

  //--------------------------------------------------------------------------
  //void motionFunc(int x, int y)
  //{
  //	g_oldX = g_newX; g_oldY = g_newY;
  //	g_newX = x;      g_newY = y;
  //
  //	float rel_x = (g_newX - g_oldX) / (float)g_imageWidth;
  //	float rel_y = (g_newY - g_oldY) / (float)g_imageHeight;
  //	if (g_rotating)
  //	{
  //		g_rot.y += (rel_x * 180);
  //		g_rot.x += (rel_y * 180);
  //	}
  //	else if (g_panning)
  //	{
  //		g_pos.x -= rel_x;
  //		g_pos.y += rel_y;
  //	}
  //	else if (g_scaling)
  //	{
  //		g_pos.z -= rel_y * g_pos.z;
  //	}
  //
  //	// glutPostRedisplay(); // FIXME
  //}

  //--------------------------------------------------------------------------
  void SaveFramebuffer()
  {
    std::cout << "Writing image.ppm... " << std::flush;
    float *pixels = new float[3*g_imageWidth*g_imageHeight];
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, g_imageWidth, g_imageHeight, GL_RGB, GL_FLOAT, pixels);

    char *filename = "image.ppm";
    std::ofstream fp(filename);
    fp << "P3\n" << g_imageWidth << " " << g_imageHeight << std::endl << "255\n";
    for (int i = g_imageHeight-1; i >=0; i--)
    {
	    for (int j = 0; j < g_imageWidth; j++)
	    {
		    int pixelPos = (i*g_imageWidth+j)*3;
		    int r = (int)(pixels[pixelPos+0]*255.0);
		    int g = (int)(pixels[pixelPos+1]*255.0);
		    int b = (int)(pixels[pixelPos+2]*255.0);
		    fp << r << " " << g << " " << b << std::endl;
	    }
    }
    fp.close();
    std::cout << "done!\n";
  }

  //--------------------------------------------------------------------------
  void keyboardFunc(unsigned char key, int x, int y)
  {
    key = (unsigned char)tolower(key);
    switch(key)
    {
	    case 8:
		    g_bShowUI = !g_bShowUI;
		    break;
	    case 'q':
		    g_useOQ = !g_useOQ;
		    break;
	    case '+':
		    g_numPasses++;
		    break;
	    case '-':
		    g_numPasses--;
		    break;
	    case 'b':
		    g_backgroundColor = (g_backgroundColor == g_white) ? g_black : g_white;
		    break;
	    case 'c':
		    SaveFramebuffer();
		    break;
	    case 'o':
		    g_showOsd = !g_showOsd;
		    break;
	    case 'r':
		    ReloadShaders();
		    break;
	    case '1':
		    g_mode = DUAL_PEELING_MODE;
		    break;
	    case '2':
		    g_mode = F2B_PEELING_MODE;
		    break;
	    case '3':
		    g_mode = WEIGHTED_AVERAGE_MODE;
		    break;
	    case '4':
		    g_mode = WEIGHTED_SUM_MODE;
		    break;
	    case 'a':
		    g_opacity -= 0.05;
        g_opacity = vl::max(g_opacity, 0.0f);
		    break;
	    case 'd':
		    g_opacity += 0.05;
		    g_opacity = vl::min(g_opacity, 1.0f);
		    break;
	    case 27:
		    exit(0);
		    break;
    }
    
    // glutPostRedisplay(); // FIXME
  }

  //--------------------------------------------------------------------------
  //void menuFunc(int i)
  //{
  //	keyboardFunc((unsigned char) i, 0, 0);
  //}

  //--------------------------------------------------------------------------
  //void InitMenus()
  //{
  //    int objectMenuId = glutCreateMenu(menuFunc);
  //    {
  //        glutCreateMenu(menuFunc);
  //        glutAddMenuEntry("'1' - Dual peeling mode", '1');
  //        glutAddMenuEntry("'2' - Front peeling mode", '2');
  //        glutAddMenuEntry("'3' - Weighted average mode", '3');
  //        glutAddMenuEntry("'4' - Weighted sum mode", '4');
  //        glutAddMenuEntry("'A' - dec uniform opacity", 'A');
  //        glutAddMenuEntry("'D' - inc uniform opacity", 'D');
  //        glutAddMenuEntry("'R' - Reload shaders", 'R');
  //        glutAddMenuEntry("'B' - Change background color", 'B');
  //        glutAddMenuEntry("'Q' - Toggle occlusion queries", 'Q');
  //        glutAddMenuEntry("'-' - dec number of geometry passes", '-');
  //        glutAddMenuEntry("'+' - inc number of geometry passes", '+');
  //        glutAddMenuEntry("Quit (esc)", '\033');
  //        glutAttachMenu(GLUT_RIGHT_BUTTON);
  //    }
  //}

  //--------------------------------------------------------------------------
  //int main(int argc, char *argv[])
  //{
  //  printf("dual_depth_peeling - sample comparing multiple order independent transparency techniques\n");
  //    printf("  Commands:\n");
  //    printf("     A/D       - Change uniform opacity\n");
  //    printf("     1         - Dual peeling mode\n");
  //    printf("     2         - Front to back peeling mode\n");
  //    printf("     3         - Weighted average mode\n");
  //    printf("     4         - Weighted sum mode\n");
  //    printf("     R         - Reload all shaders\n");
  //    printf("     B         - Change background color\n");
  //    printf("     Q         - Toggle occlusion queries\n");
  //  printf("     +/-       - Change number of geometry passes\n\n");

  //  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
  //  glutInitWindowSize(g_imageWidth, g_imageHeight);
  //  glutInit(&argc, argv);
  //  glutCreateWindow("Dual Depth Peeling");

  //  if (glewInit() != GLEW_OK)
  //  {
	 //   printf("glewInit failed. Exiting...\n");
	 //   exit(1);
  //  }

  //  printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
  //  printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
  //  printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
  //  printf("GL_SHADING_LANGUAGE_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  //  if (!glewIsSupported( 
  //        "GL_VERSION_2_0 "
  //        "GL_ARB_texture_rectangle "
		//			"GL_ARB_texture_float "
		//			// "GL_NV_float_buffer "
  //        // "GL_NV_depth_buffer_float " 
  //        ))
  //  {
  //    printf("Unable to load the necessary extensions\n");
  //    printf("This sample requires:\n");
  //    printf("OpenGL version 2.0\n");
  //    printf("GL_ARB_texture_rectangle\n");
  //    printf("GL_ARB_texture_float\n");
  //    // printf("GL_NV_float_buffer\n");
  //    // printf("GL_NV_depth_buffer_float\n");
  //    printf("Exiting...\n");
  //    exit(1);
  //  }

  //  InitGL();
  //  //InitMenus();
  //  // glutDisplayFunc(display);
  //  // glutReshapeFunc(reshape);
  //  //glutIdleFunc(idle);
  //  //glutMouseFunc(mouseFunc);
  //  //glutMotionFunc(motionFunc);
  //  //glutKeyboardFunc(keyboardFunc);

  //  // glutMainLoop();
  //  return 0;
  //}

////////////////////////////////////////////////////////////////////////////////////////

  void initEvent()
  {
    vl::Log::print( vl::Say("GL_VERSION: %s\n") << glGetString(GL_VERSION));
    vl::Log::print( vl::Say("GL_RENDERER: %s\n\n") << glGetString(GL_RENDERER));

    vl::Log::notify(appletInfo());
    openglContext()->setContinuousUpdate(false);
    rendering()->as<vl::Rendering>()->setNearFarClippingPlanesOptimized(true);
    loadModel("/models/3ds/monkey.3ds");
  }

  void loadModel(const char* file) {
    std::vector<vl::String> files;
    files.push_back(file);
    loadModel(files);
  }

  void loadModel(const std::vector<vl::String>& files)
  {
    sceneManager()->tree()->actors()->clear();

    // default effects

    vl::ref<vl::Light> camera_light = new vl::Light;
    vl::ref<vl::Effect> fx_lit = new vl::Effect;
    fx_lit->shader()->enable(vl::EN_DEPTH_TEST);
    fx_lit->shader()->enable(vl::EN_LIGHTING);
    fx_lit->shader()->setRenderState(camera_light.get(), 0);

    vl::ref<vl::Effect> fx_solid = new vl::Effect;
    fx_solid->shader()->enable(vl::EN_DEPTH_TEST);

    mEffects.clear();

    for(unsigned int i=0; i<files.size(); ++i)
    {
      vl::ref<vl::ResourceDatabase> resource_db = vl::loadResource(files[i], false);

      if (!resource_db || resource_db->count<vl::Actor>() == 0)
      {
        VL_LOG_ERROR << "No data found.\n";
        continue;
      }

#if 0
      // VLX save
      String save_path = files[i].extractPath() + files[i].extractFileName() + ".vlb";
      saveVLB(save_path, resource_db.get());
#endif

      showStatistics(resource_db);

      for(size_t i=0; i<resource_db->resources().size(); ++i)
      {
        vl::Actor* act = resource_db->resources()[i]->as<vl::Actor>();

        if (!act)
          continue;

        if (act->effect() == NULL)
          act->setEffect(fx_lit.get());

        vl::Geometry* geom = act->lod(0)->as<vl::Geometry>();
        geom->computeNormals();

        sceneManager()->tree()->addActor(act);

        if (geom && geom->normalArray())
        {
          act->effect()->shader()->enable(vl::EN_LIGHTING);
          act->effect()->shader()->gocLightModel()->setTwoSide(true);
        }

        if (geom && !geom->normalArray())
        {
          act->effect()->shader()->disable(vl::EN_LIGHTING);
        }

        if ( act->effect()->shader()->isEnabled(vl::EN_LIGHTING) && !act->effect()->shader()->getLight(0) )
          act->effect()->shader()->setRenderState(camera_light.get(), 0);

        VL_CHECK(act);
        VL_CHECK(act->effect());

        mEffects.insert( act->effect() );
      }
    }

    // position the camera to nicely see the objects in the scene
    trackball()->adjustView( rendering()->as<vl::Rendering>(), vl::vec3(0,0,1), vl::vec3(0,1,0), 1.0f );

    // throttle ghost camera manipulator speed based on the scene size, using a simple euristic formula
    sceneManager()->computeBounds();
    const vl::AABB& scene_aabb = sceneManager()->boundingBox();
    vl::real speed = (scene_aabb.width() + scene_aabb.height() + scene_aabb.depth()) / 20.0f;
    ghostCameraManipulator()->setMovementSpeed(speed);
  }

  void showStatistics(vl::ref<vl::ResourceDatabase> res_db)
  {
    std::set<vl::Geometry*> geometries;
    std::vector< vl::ref<vl::Geometry> > geom_db;
    std::vector< vl::ref<vl::Actor> > actor_db;

    res_db->get<vl::Actor>(actor_db);
    res_db->get<vl::Geometry>(geom_db);

    // find number of unique geometries

    for(size_t i=0; i<geom_db.size(); ++i)
      geometries.insert( geom_db[i].get() );

    for(size_t i=0; i<actor_db.size(); ++i)
    {
      vl::Geometry* geom = actor_db[i]->lod(0)->as<vl::Geometry>();
      if (geom)
        geometries.insert( geom );
    }

    int total_triangles = 0;
    int total_draw_calls = 0;
    for( std::set<vl::Geometry*>::iterator it = geometries.begin(); it != geometries.end(); ++it )
    {
      total_draw_calls += (*it)->drawCalls()->size();
      for(int i=0; i < (*it)->drawCalls()->size(); ++i )
        total_triangles += (*it)->drawCalls()->at(i)->countTriangles();
    }

    VL_LOG_PRINT << "Statistics:\n";
    VL_LOG_PRINT << "+ Total triangles  = " << total_triangles << "\n";
    VL_LOG_PRINT << "+ Total draw calls = " << total_draw_calls << "\n";
    VL_LOG_PRINT << "+ Actors           = " << actor_db.size() << "\n";
    VL_LOG_PRINT << "+ Geometries       = " << geometries.size() << "\n";
  }

  void fileDroppedEvent(const std::vector<vl::String>& files)
  {
    if (!loadShaders(files))
    {
      loadModel(files);
      loadShaders(mLastShaders);
    }

    // update the rendering
    openglContext()->update();
  }

  bool loadShaders(const std::vector<vl::String>& files)
  {
    vl::ref<vl::GLSLProgram> glsl = new vl::GLSLProgram;
    for(size_t i=0; i<files.size(); ++i)
    {
      if (files[i].endsWith(".fs"))
        glsl->attachShader( new vl::GLSLFragmentShader( files[i] ) );
      else
      if (files[i].endsWith(".vs"))
        glsl->attachShader( new vl::GLSLVertexShader( files[i] ) );
      else
      if (files[i].endsWith(".gs"))
        glsl->attachShader( new vl::GLSLGeometryShader( files[i] ) );
      else
      if (files[i].endsWith(".tcs"))
        glsl->attachShader( new vl::GLSLTessControlShader( files[i] ) );
      else
      if (files[i].endsWith(".tes"))
        glsl->attachShader( new vl::GLSLTessEvaluationShader( files[i] ) );
    }

    if ( glsl->shaderCount() && glsl->linkProgram())
    {
      for( std::set< vl::ref<vl::Effect> >::iterator it = mEffects.begin(); it != mEffects.end() ; ++it )
        it->get_writable()->shader()->setRenderState( glsl.get() );
      mLastShaders = files;
    }

    return glsl->shaderCount() != 0;
  }

protected:
  std::set< vl::ref<vl::Effect> > mEffects;
  std::vector<vl::String> mLastShaders;
};

// Have fun!

BaseDemo* Create_App_DepthPeeling() { return new App_DepthPeeling; }
