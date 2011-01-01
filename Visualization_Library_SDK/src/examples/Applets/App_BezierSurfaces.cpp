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

#include "BaseDemo.hpp"
#include "vlCore/Light.hpp"
#include "vlCore/BezierSurface.hpp"

class App_BezierSurfaces: public BaseDemo
{
public:
  App_BezierSurfaces(): mDetail(10) {}

  void initEvent()
  {
    BaseDemo::initEvent();

    /* 2 pass shader: 1 = solid, 2 = wireframe */

    vl::ref<vl::Effect> fx = new vl::Effect;
    fx->lod(0)->push_back( new vl::Shader);

    fx->shader()->enable(vl::EN_LIGHTING);
    fx->shader()->gocLightModel()->setTwoSide(true);
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->gocLight(0)->setLinearAttenuation(0.025f);

    fx->shader(0,1)->enable(vl::EN_DEPTH_TEST);
    fx->shader(0,1)->gocPolygonOffset()->set(-0.5f, -0.5f);
    fx->shader(0,1)->enable(vl::EN_POLYGON_OFFSET_LINE);
    fx->shader(0,1)->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
    fx->shader(0,1)->gocDepthMask()->set(false);

    /* Generate random Bézier patches

    - The simplest bicubic Bézier patch requires 4x4 = 16 control points: A, B, C, D, E, F, G, H, I, L, M, N, O, P, Q, R
    - The Bézier surface is guaranteed to touch only the 4 corner control points A, D, O and R.

    A---B---C---D
    |   |   |   |
    E---F---G---H
    |   |   |   |
    I---L---M---N
    |   |   |   |
    O---P---Q---R

    - You can concatenate two bicubic Bézier patches to form a larger suface by sharing their control points like this:

       patch 1     patch 2
    A---+---+---B---+---+---G
    |   |   |   |   |   |   |
    +---+---+---C---+---+---+
    |   |   |   |   |   |   |
    +---+---+---D---+---+---+
    |   |   |   |   |   |   |
    F---+---+---E---+---+---H

    - In this case the two patches share the control points B, C, D and E.
    - As we can see the total control points needed are 28 = (2 (patches along x) * 3 + 1) * (1 (patches along y) * 3 + 1)
    - Also in this case the Bézier surface is guaranteed to touch only the 6 corner control points A, B, E, F, G and H.

    */

    // This concatenated patch will actually contain 3*2 = 6 bicubic Bézier patches for a total of 70 control points!
    int x = 3;
    int y = 2;
    // We use the formula seen above to compute the number of control points required in each direction.
    vl::ref<vl::BezierPatch> patch1 = new vl::BezierPatch(x*3+1,y*3+1);
    for(int y=0;y<patch1->y();++y)
      for(int x=0;x<patch1->x();++x)
        patch1->points()[x][y] = vl::dvec3(x,vl::randomMinMax(-2.0,+2.0),y);

    /* Add Bézier patches to our Bézier surface */

    // Instance our Bézier surface
    mBezier = new vl::BezierSurface;
    // We can actually add multiple Bézier patches
    mBezier->patches().push_back(patch1.get());
    // Define the subdivision detail
    mBezier->setDetail(mDetail);
    // Generate the actual geometry using the current patches and detail
    mBezier->updateBezierSurface(false);
    // Compute the normals as we have lighting activated
    mBezier->computeNormals();
    // Used by the line rendering
    mBezier->setColor(vl::blue);
    // Add the Bézier surface to our scene
    sceneManager()->tree()->addActor(mBezier.get(), fx.get(), NULL);

    /* Show the control points */

    showPatchControlPoints(mBezier.get());
  }

  /* 
    up/down arrow = increase/decrease the Bézier surface tessellation detail
    space         = toggle control points visibility
  */
  void keyPressEvent(unsigned short ch, vl::EKey key)
  {
    BaseDemo::keyPressEvent(ch,key);
    if (key == vl::Key_Up)
    {
      ++mDetail;
      mDetail = vl::clamp(mDetail, 2, 64);
      mBezier->setDetail(mDetail);
      mBezier->updateBezierSurface(false);
      mBezier->computeNormals();
      mBezier->setVBODirty(true);
    }
    else
    if (key == vl::Key_Down)
    {
      --mDetail;
      mDetail = vl::clamp(mDetail, 2, 64);
      mBezier->setDetail(mDetail);
      mBezier->updateBezierSurface(false);
      mBezier->computeNormals();
      mBezier->setVBODirty(true);
    }
    else
    if(key == vl::Key_Space)
    {
      if (sceneManager()->tree()->actors()->find(mCtrlPoints_Actor.get()) == -1)
        sceneManager()->tree()->actors()->push_back(mCtrlPoints_Actor.get());
      else
        sceneManager()->tree()->actors()->erase(mCtrlPoints_Actor.get());
    }
  }

  /* Generates the geometry to render the control points */
  void showPatchControlPoints(vl::BezierSurface* bezier)
  {
    vl::ref<vl::Effect> fx = new vl::Effect;
    fx->shader()->enable(vl::EN_DEPTH_TEST);
    fx->shader()->gocPolygonMode()->set(vl::PM_LINE, vl::PM_LINE);
    fx->shader()->gocLineWidth()->set(1.0f);
    fx->shader()->gocPointSize()->set(5.0f);

    vl::ref<vl::Geometry> geom = new vl::Geometry;

    int istart = 0;
    std::vector<vl::fvec3> verts;
    std::vector<vl::fvec4> colos;
    for(unsigned ipatch=0; ipatch<bezier->patches().size(); ++ipatch)
    {
      const vl::BezierPatch::Points& p = bezier->patches()[ipatch]->points();
      for(unsigned ix=0; ix<p.size()-3;     ix+=3)
      for(unsigned iy=0; iy<p[ix].size()-3; iy+=3, istart+=16)
      {
        verts.push_back((vl::fvec3)p[ix+0][iy+0]); colos.push_back(vl::red);
        verts.push_back((vl::fvec3)p[ix+0][iy+1]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+0][iy+2]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+0][iy+3]); colos.push_back(vl::red);

        verts.push_back((vl::fvec3)p[ix+1][iy+0]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+1][iy+1]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+1][iy+2]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+1][iy+3]); colos.push_back(vl::white);

        verts.push_back((vl::fvec3)p[ix+2][iy+0]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+2][iy+1]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+2][iy+2]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+2][iy+3]); colos.push_back(vl::white);

        verts.push_back((vl::fvec3)p[ix+3][iy+0]); colos.push_back(vl::red);
        verts.push_back((vl::fvec3)p[ix+3][iy+1]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+3][iy+2]); colos.push_back(vl::white);
        verts.push_back((vl::fvec3)p[ix+3][iy+3]); colos.push_back(vl::red);

        vl::ref<vl::DrawArrays> da = new vl::DrawArrays(vl::PT_POINTS,istart,16);
        vl::ref<vl::DrawElementsUInt> de = new vl::DrawElementsUInt(vl::PT_QUADS);
        de->indices()->resize(4*9);
        unsigned int quads[] = { 0,1,5,4, 4,5,9,8, 8,9,13,12, 1,2,6,5, 5,6,10,9, 9,10,14,13, 2,3,7,6, 6,7,11,10, 10,11,15,14 };
        for(int q=0; q<4*9; ++q)
          quads[q] += istart;
        memcpy(de->indices()->ptr(), quads, sizeof(quads));
        geom->drawCalls()->push_back(de.get());
        geom->drawCalls()->push_back(da.get());
      }
    }

    vl::ref<vl::ArrayFloat3> vert_array = new vl::ArrayFloat3;
    geom->setVertexArray(vert_array.get());
    *vert_array = verts;

    vl::ref<vl::ArrayFloat4> cols_array = new vl::ArrayFloat4;
    geom->setColorArray(cols_array.get());
    *cols_array = colos;

    mCtrlPoints_Actor = sceneManager()->tree()->addActor(geom.get(), fx.get(), NULL);
  }

protected:
  vl::ref<vl::BezierSurface> mBezier;
  vl::ref<vl::Actor> mCtrlPoints_Actor;
  int mDetail;
};

// Have fun!

BaseDemo* Create_App_BezierSurfaces() { return new App_BezierSurfaces; }
