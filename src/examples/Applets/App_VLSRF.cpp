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

// mic fixme
#include <vlCore/SRF_Tools.hpp>

#include "BaseDemo.hpp"
#include <vlGraphics/GeometryPrimitives.hpp>
#include <vlGraphics/plugins/vlSRF.hpp>
//#include <vlGraphics/DrawPixels.hpp>
//#include <vlGraphics/Light.hpp>
//#include <vlGraphics/Geometry.hpp>
//#include <vlCore/BufferedStream.hpp>
//#include <vlCore/DiskFile.hpp>
//#include <vlGraphics/MultiDrawElements.hpp>
//#include <vlGraphics/TriangleStripGenerator.hpp>
//#include <vlCore/ResourceDatabase.hpp>

using namespace vl;

class App_VLSRF: public BaseDemo
{
public:
  virtual void initEvent()
  {
    Log::notify(appletInfo());

    // ref<Geometry> geom = makeBox( vec3(0,0,0), 10, 10, 10 );
    // ref<Geometry> geom = makeIcosphere( vec3(0,0,0), 10, 0 );
    ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 4 );
    geom->computeNormals();

    geom->setColorArray( geom->normalArray() );
    // geom->setSecondaryColorArray( geom->normalArray() );
    // TriangleStripGenerator::stripfy(geom.get(), 22, false, false, true);
    // geom->mergeDrawCallsWithPrimitiveRestart(PT_TRIANGLE_STRIP);
    // geom->mergeDrawCallsWithMultiDrawElements(PT_TRIANGLE_STRIP);
    // mic fixme: this does no realizes that we are using primitive restart
    // mic fixme: make this manage also MultiDrawElements
    // geom->makeGLESFriendly();
    geom->drawCalls()->push_back( geom->drawCalls()->back() );

    ref<Effect> fx = new Effect;
    fx->shader()->enable(EN_LIGHTING);
    fx->shader()->enable(EN_DEPTH_TEST);
    fx->shader()->gocMaterial()->setColorMaterialEnabled(true);
    fx->shader()->setRenderState( new Light, 0 );

    // sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);

    ref<ResourceDatabase> res_db = new ResourceDatabase;
#if 0
    for (int i=0; i<100; ++i)
    {
      ref<Geometry> geom = makeTeapot( vec3(0,0,0), 10, 16 );
      geom->computeNormals();
      res_db->resources().push_back( geom.get() );
    }
#else
    res_db->resources().push_back( geom.get() );
#endif

    // bool ok = writeSRF("D:/VL/srf_export.vl", res_db.get());
    // VL_CHECK(ok);

    res_db = loadSRF("D:/VL/srf_export.vl");
    VL_CHECK(res_db);

    geom = res_db->get<Geometry>(0);
    VL_CHECK(geom)
    sceneManager()->tree()->addActor( geom.get(), fx.get(), NULL);
  }
};

// Have fun!

BaseDemo* Create_App_VLSRF() { return new App_VLSRF; }
