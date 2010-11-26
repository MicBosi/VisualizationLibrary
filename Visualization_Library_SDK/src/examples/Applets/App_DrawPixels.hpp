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

#include "vl/DrawPixels.hpp"

class App_DrawPixels: public BaseDemo
{
public:
  virtual void shutdown() {}

  virtual void run()
  {
    vl::ivec2 pos;
    //pos.x() = openglContext()->size().x() / 2.0;
    //pos.y() = openglContext()->size().y() / 2.0;
    vl::ivec2 pos1 = pos + vl::ivec2(vl::fvec2(-1,-1) * (float)((1+sin(vl::Time::currentTime()*vl::fPi*2.0f/5.0f))*100.0f));
    vl::ivec2 pos2 = pos + vl::ivec2(vl::fvec2(+1,+1) * (float)((1+sin(vl::Time::currentTime()*vl::fPi*2.0f/5.0f))*100.0f));
    vl::ivec2 pos3 = pos + vl::ivec2(vl::fvec2(+1,-1) * (float)((1+sin(vl::Time::currentTime()*vl::fPi*2.0f/5.0f))*100.0f));
    vl::ivec2 pos4 = pos + vl::ivec2(vl::fvec2(-1,+1) * (float)((1+sin(vl::Time::currentTime()*vl::fPi*2.0f/5.0f))*100.0f));
    mPixels1->setPosition( pos1 );
    mPixels2->setPosition( pos2 );
    mPixels3->setPosition( pos3 );
    mPixels4->setPosition( pos4 );

    vl::mat4 mat = vl::mat4::rotation( (float)vl::Time::currentTime()*45.0f, 1,1,1 );
    mat.translate( sin(vl::Time::currentTime()*vl::fPi*2.0f/2.0f)*2.0f, 0, 0 );
    mCube->transform()->setLocalMatrix( mat );
  }

  virtual void initEvent()
  {
    BaseDemo::initEvent();
    vl::ref<vl::Image> star = new vl::Image("/images/star.png");
    vl::ref<vl::Image> circle16 = new vl::Image("/images/circle16.png");
    int w = star->width() / 2;

    mStar = new vl::DrawPixels;
    mPoints = new vl::DrawPixels;

    mPixels1 = new vl::DrawPixels::Pixels( star.get(), 0, 0, 0, 0, w, w );
    mPixels2 = new vl::DrawPixels::Pixels( star.get(), 0, 0, w, w, w, w );
    mPixels3 = new vl::DrawPixels::Pixels( star.get(), 0, 0, w, 0, w, w);
    mPixels4 = new vl::DrawPixels::Pixels( star.get(), 0, 0, 0, w, w, w);

    mPixels1->setAlign(vl::AlignRight|vl::AlignTop);
    mPixels2->setAlign(vl::AlignLeft|vl::AlignBottom);
    mPixels3->setAlign(vl::AlignLeft|vl::AlignTop);
    mPixels4->setAlign(vl::AlignRight|vl::AlignBottom);

    mStar->draws()->push_back( mPixels1.get() );
    mStar->draws()->push_back( mPixels2.get() );
    mStar->draws()->push_back( mPixels3.get() );
    mStar->draws()->push_back( mPixels4.get() );

    for(int i=0; i<1000; ++i)
    {
      vl::ref<vl::DrawPixels::Pixels> pixels = new vl::DrawPixels::Pixels(circle16.get(), 0,0);
      mPoints->draws()->push_back( pixels.get() );
      vl::ivec2 pos;
      pos.x() = int(openglContext()->size().x() / 2.0 + rand()%300 - 150);
      pos.y() = int(openglContext()->size().y() / 2.0 + rand()%300 - 150);
      pixels->setPosition(pos);
      pixels->setAlign(vl::AlignHCenter | vl::AlignVCenter);
    }

    mPoints->generatePixelBufferObjects(vl::GBU_STATIC_DRAW, true);
    mStar->generatePixelBufferObjects(vl::GBU_STATIC_DRAW, true);

    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    effect->shader()->enable(vl::EN_ALPHA_TEST);
    effect->shader()->enable(vl::EN_DEPTH_TEST);
    effect->shader()->enable(vl::EN_LIGHTING);
    effect->shader()->setRenderState( new vl::Light(0) );
    effect->shader()->gocAlphaFunc()->set(vl::FU_GEQUAL, 0.9f);

    vl::ref<vl::Geometry> cube = vlut::makeBox( vl::vec3(0,0,0), 1, 1, 1 );
    cube->computeNormals();
    cube->setColor(vlut::red);
    mCube = sceneManager()->tree()->addActor(cube.get(), effect.get(), new vl::Transform);
    vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->transform()->addChild(mCube->transform());

    sceneManager()->tree()->addActor( mPoints.get(), effect.get() );
    vl::Actor* star_act   = sceneManager()->tree()->addActor( mStar.get(),   effect.get() );
    star_act->setTransform(mCube->transform());

  }

protected:
  vl::ref<vl::Actor> mCube;
  vl::ref<vl::DrawPixels> mStar;
  vl::ref<vl::DrawPixels> mPoints;
  vl::ref<vl::DrawPixels::Pixels> mPixels1;
  vl::ref<vl::DrawPixels::Pixels> mPixels2;
  vl::ref<vl::DrawPixels::Pixels> mPixels3;
  vl::ref<vl::DrawPixels::Pixels> mPixels4;
};

// Have fun!
