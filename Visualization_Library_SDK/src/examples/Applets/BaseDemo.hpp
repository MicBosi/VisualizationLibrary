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

#ifndef BaseDemo_INCLUDE_ONCE
#define BaseDemo_INCLUDE_ONCE

#include "vlut/Applet.hpp"
#include "vlGraphics/Rendering.hpp"
#include "vlCore/VisualizationLibrary.hpp"
#include "vlGraphics/ReadPixels.hpp"
#include "vlCore/Time.hpp"

class BaseDemo: public vl::Applet
{
public:
  BaseDemo()
  {
    mMaxTime = 0;
    mReadPixels = new vl::ReadPixels;
  }

  void initEvent() 
  {
    trackball()->setPivot(vl::vec3(0,0,0));
    trackball()->setTransform(NULL);
  }
  
  void run() {}
  
  void shutdown() {}

  void keyPressEvent(unsigned short, vl::EKey key)
  {
    if (key == vl::Key_F5)
    {
      mReadPixels->setup( 0, 0, openglContext()->width(), openglContext()->height(), vl::RDB_BACK_LEFT, false );
      vl::VisualizationLibrary::rendering()->onFinishedCallbacks()->push_back( mReadPixels.get() );
      mReadPixels->setRemoveAfterCall(true);
      vl::String filename = vl::Say( applicationName() + "-%n.jpg") << (int)vl::Time::currentTime();
      mReadPixels ->setSavePath( filename );
      vl::Log::print( vl::Say("Screenshot: '%s'\n") << filename );
    }
  }

  void setMaxTime(float time) { mMaxTime = time; }

  void runEvent()
  {
    vl::Applet::runEvent();

    if ( !mFPSTimer.isStarted() )
      mFPSTimer.start();
    else
    if (mFPSTimer.elapsed() > 1)
    {
      mFPSTimer.start();
      openglContext()->setWindowTitle( vl::Say("%s [%.1n]") << applicationName() << fps() );
      vl::Log::print( vl::Say("FPS=%.1n\n") << fps() );
    }

    if (mMaxTime != 0)
    {
      if ( !mApplicatinLifeTime.isStarted() )
        mApplicatinLifeTime.start();
      else
      if (mApplicatinLifeTime.elapsed() > mMaxTime)
        openglContext()->destroy();
    }
  }

  const vl::String& applicationName() const { return mApplicationName; }

  void setApplicationName(const vl::String& app_name) { mApplicationName = app_name; } 

protected:
  vl::ref<vl::ReadPixels> mReadPixels;
  vl::Time mApplicatinLifeTime;
  vl::Time mFPSTimer;
  float mMaxTime;
  vl::String mApplicationName;
};

#endif
