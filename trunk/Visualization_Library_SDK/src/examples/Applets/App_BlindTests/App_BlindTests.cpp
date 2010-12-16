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

#include "../BaseDemo.hpp"
#include "vl/Text.hpp"
#include "vl/FontManager.hpp"
#include "vl/TextStream.hpp"
#include "vl/Geometry.hpp"
#include <time.h>

namespace blind_tests
{
  bool test_signal_slot();
}

class App_BlindTests: public BaseDemo
{

public:
  virtual void shutdown() {}
  virtual void run() {}
  void initEvent()
  {
    BaseDemo::initEvent();

    bool test_passed = blind_tests::test_signal_slot();

    // display test pass/failure information

    vl::Log::print(test_passed ? "\n*** test passed ***\n\n" : "\n*** TEST FAILED ***\n\n");

    vl::ref<vl::Text> text = new vl::Text;
    text->setFont( vl::VisualizationLibrary::fontManager()->acquireFont("/font/bitstream-vera/VeraMono.ttf", 12) );
    text->setText(test_passed ? "Blind Test Passed" : "Blind Test FAILED");
    text->setAlignment( vl::AlignHCenter | vl::AlignVCenter);
    text->setViewportAlignment( vl::AlignHCenter | vl::AlignVCenter );
    vl::ref<vl::Effect> effect = new vl::Effect;
    effect->shader()->enable(vl::EN_BLEND);
    sceneManager()->tree()->addActor(text.get(), effect.get());
  }

};

BaseDemo* Create_App_BlindTests() { return new App_BlindTests; }
