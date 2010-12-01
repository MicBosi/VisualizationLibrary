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

#include <vlVG/VectorGraphics.hpp>

using namespace vlVG;

//-----------------------------------------------------------------------------
VectorGraphics::VectorGraphics()
{
  mDefaultEffect = new vl::Effect;
  mDefaultEffect->shader()->enable(vl::EN_BLEND);
  mActors.setAutomaticDelete(false);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawLine(double x1, double y1, double x2, double y2)
{
  std::vector<vl::dvec2> ln;
  ln.push_back(vl::dvec2(x1,y1));
  ln.push_back(vl::dvec2(x2,y2));
  return drawLines(ln);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawLines(const std::vector<vl::dvec2>& ln)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(ln);
  // generate texture coords
  if (mState.mImage)
  {
    vl::ref<vl::ArrayFloat> tex_array = new vl::ArrayFloat;
    tex_array->resize(geom->vertexArray()->size());
    float u1 = 1.0f / mState.mImage->width() * 0.5f;
    float u2 = 1.0f - 1.0f / mState.mImage->width() * 0.5f;
    for(size_t i=0; i<tex_array->size(); i+=2)
    {
      tex_array->at(i+0) = u1;
      tex_array->at(i+1) = u2;
    }
    // generate geometry
    geom->setTexCoordArray(0, tex_array.get());
  }
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_LINES, 0, (int)ln.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawLineStrip(const std::vector<vl::dvec2>& ln)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(ln);
  // generate texture coords
  generateLinearTexCoords(geom.get());
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_LINE_STRIP, 0, (int)ln.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawLineLoop(const std::vector<vl::dvec2>& ln)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(ln);
  // generate texture coords
  generateLinearTexCoords(geom.get());
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_LINE_LOOP, 0, (int)ln.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillPolygon(const std::vector<vl::dvec2>& poly)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(poly);
  // generate texture coords
  generatePlanarTexCoords(geom.get(), poly);
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_POLYGON, 0, (int)poly.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillTriangles(const std::vector<vl::dvec2>& triangles)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(triangles);
  // generate texture coords
  generatePlanarTexCoords(geom.get(), triangles);
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_TRIANGLES, 0, (int)triangles.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillTriangleFan(const std::vector<vl::dvec2>& fan)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(fan);
  // generate texture coords
  generatePlanarTexCoords(geom.get(), fan);
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_TRIANGLE_FAN, 0, (int)fan.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillTriangleStrip(const std::vector<vl::dvec2>& strip)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(strip);
  // generate texture coords
  generatePlanarTexCoords(geom.get(), strip);
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_TRIANGLE_STRIP, 0, (int)strip.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillQuads(const std::vector<vl::dvec2>& quads)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(quads);
  // generate texture coords
  generateQuadsTexCoords(geom.get(), quads);
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_QUADS, 0, (int)quads.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillQuadStrip(const std::vector<vl::dvec2>& quad_strip)
{
  // fill the vertex position array
  vl::ref<vl::Geometry> geom = prepareGeometry(quad_strip);
  // generate texture coords
  generatePlanarTexCoords(geom.get(), quad_strip);
  // issue the primitive
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_QUAD_STRIP, 0, (int)quad_strip.size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawPoint(double x, double y)
{
  std::vector<vl::dvec2> pt;
  pt.push_back(vl::dvec2(x,y));
  return drawPoints(pt);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawPoints(const std::vector<vl::dvec2>& pt)
{
  // transform the points
  vl::ref<vl::ArrayFVec3> pos_array = new vl::ArrayFVec3;
  pos_array->resize(pt.size());
  // transform done using high precision
  for(unsigned i=0; i<pt.size(); ++i)
  {
    pos_array->at(i) = (vl::fvec3)(matrix() * vl::dvec3(pt[i].x(), pt[i].y(), 0));
    // needed for pixel/perfect rendering
    if (mState.mPointSize % 2 == 0)
    {
      pos_array->at(i).s() += 0.5;
      pos_array->at(i).t() += 0.5;
    }
  }
  // generate geometry
  vl::ref< vl::Geometry > geom = new vl::Geometry;
  geom->setVertexArray(pos_array.get());
  geom->drawCalls()->push_back( new vl::DrawArrays(vl::PT_POINTS, 0, (int)pos_array->size()) );
  // add the actor
  return addActor( new vl::Actor(geom.get(), currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawEllipse(double origx, double origy, double xaxis, double yaxis, int segments)
{
  std::vector<vl::dvec2> points;
  points.resize(segments);
  for(int i=0; i<segments; ++i)
  {
    double t = (double)i/(segments-1) * vl::dPi * 2.0 + vl::dPi * 0.5;
    points[i] = vl::dvec2(cos(t)*xaxis*0.5+origx, sin(t)*yaxis*0.5+origy);
  }
  return drawLineStrip(points);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillEllipse(double origx, double origy, double xaxis, double yaxis, int segments)
{
  std::vector<vl::dvec2> points;
  points.resize(segments);
  for(int i=0; i<segments; ++i)
  {
    double t = (double)i/segments * vl::dPi * 2.0 + vl::dPi * 0.5;
    points[i] = vl::dvec2(cos(t)*xaxis*0.5+origx, sin(t)*yaxis*0.5+origy);
  }
  return fillPolygon(points);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawQuad(double left, double bottom, double right, double top)
{
  std::vector<vl::dvec2> quad;
  quad.push_back(vl::dvec2(left,bottom));
  quad.push_back(vl::dvec2(left,top));
  quad.push_back(vl::dvec2(right,top));
  quad.push_back(vl::dvec2(right,bottom));
  return drawLineLoop(quad);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::fillQuad(double left, double bottom, double right, double top)
{
  std::vector<vl::dvec2> quad;
  quad.push_back(vl::dvec2(left,bottom));
  quad.push_back(vl::dvec2(left,top));
  quad.push_back(vl::dvec2(right,top));
  quad.push_back(vl::dvec2(right,bottom));
  return fillQuads(quad);
}
//-----------------------------------------------------------------------------
void VectorGraphics::continueDrawing()
{
  /*mActors.clear();*/ // keep the currently drawn actors

  /*mVGToEffectMap.clear();*/      // keeps cached resources
  /*mImageToTextureMap.clear();*/  // keeps cached resources
  /*mRectToScissorMap.clear();*/   // keeps cached resources

  // restore the default states
  mState  = State();
  mMatrix = vl::dmat4();
  mMatrixStack.clear();
  mStateStack.clear();
}
//-----------------------------------------------------------------------------
void VectorGraphics::endDrawing(bool release_cache)
{
  if (release_cache)
  {
    mVGToEffectMap.clear();
    mImageToTextureMap.clear();
    mRectToScissorMap.clear();
  }
  /*mState  = State();
  mMatrix = vl::dmat4();*/
  mMatrixStack.clear();
  mStateStack.clear();
}
//-----------------------------------------------------------------------------
void VectorGraphics::clear()
{
  // remove all the actors
  mActors.clear();

  // reset everything
  mVGToEffectMap.clear();
  mImageToTextureMap.clear();
  mRectToScissorMap.clear();

  // restore the default states
  mState  = State();
  mMatrix = vl::dmat4();
  mMatrixStack.clear();
  mStateStack.clear();
}
//-----------------------------------------------------------------------------
void VectorGraphics::setLineStipple(ELineStipple stipple) 
{
  switch(stipple)
  {
    case LineStipple_Solid: mState.mLineStipple = 0xFFFF; break;
    case LineStipple_Dot:   mState.mLineStipple = 0xAAAA; break;
    case LineStipple_Dash:  mState.mLineStipple = 0xCCCC; break;
    case LineStipple_Dash4: mState.mLineStipple = 0xF0F0; break;
    case LineStipple_Dash8: mState.mLineStipple = 0xFF00; break;
    case LineStipple_DashDot: mState.mLineStipple = 0xF840; break;
    case LineStipple_DashDotDot: mState.mLineStipple = 0xF888; break;
  }
}
//-----------------------------------------------------------------------------
void VectorGraphics::setPolygonStipple(EPolygonStipple stipple) 
{
  unsigned char solid_stipple[] = {
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
    0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF };
  unsigned char hline_stipple[] = {
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 
    0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00, 0xFF,0xFF,0xFF,0xFF, 0x00,0x00,0x00,0x00 };
  unsigned char vline_stipple[] = {
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA };
  unsigned char chain_stipple[] = {
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 0xAA,0xAA,0xAA,0xAA, 
    0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55, 0x55,0x55,0x55,0x55 };
  unsigned char dot_stipple[] = {
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 
    0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55, 0xAA,0xAA,0xAA,0xAA, 0x55,0x55,0x55,0x55 };
  switch(stipple)
  {
    case PolygonStipple_Solid: setPolygonStipple(solid_stipple); break;
    case PolygonStipple_Dot: setPolygonStipple(dot_stipple); break;
    case PolygonStipple_Chain: setPolygonStipple(chain_stipple); break;
    case PolygonStipple_HLine: setPolygonStipple(hline_stipple); break;
    case PolygonStipple_VLine: setPolygonStipple(vline_stipple); break;
  }
}
//-----------------------------------------------------------------------------
void VectorGraphics::setBlendFunc(vl::EBlendFactor src_rgb, vl::EBlendFactor dst_rgb, vl::EBlendFactor src_alpha, vl::EBlendFactor dst_alpha)
{
  mState.mBlendFactorSrcRGB   = src_rgb;
  mState.mBlendFactorDstRGB   = dst_rgb;
  mState.mBlendFactorSrcAlpha = src_alpha;
  mState.mBlendFactorDstAlpha = dst_alpha;
}
//-----------------------------------------------------------------------------
void VectorGraphics::getBlendFunc(vl::EBlendFactor& src_rgb, vl::EBlendFactor& dst_rgb, vl::EBlendFactor& src_alpha, vl::EBlendFactor& dst_alpha) const
{
  src_rgb   = mState.mBlendFactorSrcRGB;
  dst_rgb   = mState.mBlendFactorDstRGB;
  src_alpha = mState.mBlendFactorSrcAlpha;
  dst_alpha = mState.mBlendFactorDstAlpha;
}
//-----------------------------------------------------------------------------
void VectorGraphics::setBlendEquation( vl::EBlendEquation rgb_eq, vl::EBlendEquation alpha_eq ) 
{
  mState.mBlendEquationRGB   = rgb_eq;
  mState.mBlendEquationAlpha = alpha_eq;
}//-----------------------------------------------------------------------------

void VectorGraphics::getBlendEquation( vl::EBlendEquation& rgb_eq, vl::EBlendEquation& alpha_eq ) const
{
  rgb_eq   = mState.mBlendEquationRGB;
  alpha_eq = mState.mBlendEquationAlpha;
}
//-----------------------------------------------------------------------------
void VectorGraphics::setStencilOp(vl::EStencilOp sfail, vl::EStencilOp dpfail, vl::EStencilOp dppass)
{
  mState.mStencil_SFail  = sfail;
  mState.mStencil_DpFail = dpfail;
  mState.mStencil_DpPass = dppass;
}
//-----------------------------------------------------------------------------
void VectorGraphics::getStencilOp(vl::EStencilOp& sfail, vl::EStencilOp& dpfail, vl::EStencilOp& dppass)
{
  sfail  = mState.mStencil_SFail;
  dpfail = mState.mStencil_DpFail;
  dppass = mState.mStencil_DpPass;
}
//-----------------------------------------------------------------------------
void VectorGraphics::setStencilFunc(vl::EFunction func, int refval, unsigned int mask)
{
  mState.mStencil_Function     = func;
  mState.mStencil_RefValue     = refval;
  mState.mStencil_FunctionMask = mask;
}
//-----------------------------------------------------------------------------
void VectorGraphics::getStencilFunc(vl::EFunction& func, int& refval, unsigned int& mask)
{
  func   = mState.mStencil_Function;
  refval = mState.mStencil_RefValue;
  mask   = mState.mStencil_FunctionMask;
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::clearColor(const vl::fvec4& color, int x, int y, int w, int h)
{
  vl::ref<vl::Clear> clear = new vl::Clear;
  clear->setClearColorBuffer(true);
  clear->setClearColorValue(color);
  clear->setScissorBox(x,y,w,h);
  return addActor( new vl::Actor( clear.get(), /*mDefaultEffect.get()*/currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::clearStencil(int clear_val, int x, int y, int w, int h)
{
  vl::ref<vl::Clear> clear = new vl::Clear;
  clear->setClearStencilBuffer(true);
  clear->setClearStencilValue(clear_val);
  clear->setScissorBox(x,y,w,h);
  return addActor( new vl::Actor( clear.get(), /*mDefaultEffect.get()*/currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawText(vl::Text* text)
{
  if (text->font() == NULL)
    text->setFont(mState.mFont.get());
  return addActor( new vl::Actor(text, /*mDefaultEffect.get()*/currentEffect(), NULL) );
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawText(int x, int y, const vl::String& text, int alignment)
{
  pushMatrix();
  mMatrix = vl::dmat4::translation(x,y,0) * mMatrix;
  vl::Actor* act = drawText(text, alignment);
  popMatrix();
  return act;
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawText(const vl::String& text, int alignment)
{
  vl::ref<vl::Text> t = new vl::Text;
  t->setText( text );
  t->setAlignment(alignment);
  t->setViewportAlignment(vl::AlignBottom|vl::AlignLeft);
  t->setColor( mState.mColor );
  t->setMatrix( (vl::fmat4)matrix() );
  return drawText(t.get());
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawActor(vl::Actor* actor, vl::Transform* transform, bool keep_effect)
{
  VL_CHECK(actor->effect())
  if (!keep_effect || !actor->effect())
    actor->setEffect(currentEffect());
  if (transform != NULL)
    actor->setTransform(transform);
  return addActor(actor);
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::drawActorCopy(vl::Actor* actor, vl::Transform* transform)
{
  vl::ref<vl::Actor> copy = new vl::Actor(*actor);
  copy->setTransform(transform);
  drawActor(copy.get());
  return copy.get();
}
//-----------------------------------------------------------------------------
void VectorGraphics::rotate(double deg) 
{ 
  mMatrix = mMatrix * vl::dmat4::rotation(deg, 0,0,1.0); 
}
//-----------------------------------------------------------------------------
void VectorGraphics::translate(double x, double y, double z)
{ 
  mMatrix = mMatrix * vl::dmat4::translation(x,y,z); 
}
//-----------------------------------------------------------------------------
void VectorGraphics::scale(double x, double y, double z) 
{ 
  mMatrix = mMatrix * vl::dmat4::scaling(x,y,z); 
}
//-----------------------------------------------------------------------------
void VectorGraphics::popMatrix() 
{ 
  if (mMatrixStack.empty())
  {
    vl::Log::error("VectorGraphics::popMatrix() matrix stack underflow!\n");
    return;
  }
  setMatrix(mMatrixStack.back());
  mMatrixStack.pop_back();
}
//-----------------------------------------------------------------------------
void VectorGraphics::pushState() 
{ 
  mStateStack.push_back(mState); 
  pushMatrix();
}
//-----------------------------------------------------------------------------
void VectorGraphics::popState() 
{ 
  popMatrix();
  if (mStateStack.empty())
  {
    vl::Log::error("VectorGraphics::popState() matrix stack underflow!\n");
    return;
  }
  mState = mStateStack.back();
  mStateStack.pop_back();
}
//-----------------------------------------------------------------------------
void VectorGraphics::pushScissor(int x, int y, int w, int h) 
{
  mScissorStack.push_back(mScissor.get()); 
  vl::RectI newscissor = mScissor ? mScissor->scissorRect().intersected(vl::RectI(x,y,w,h)) : vl::RectI(x,y,w,h);
  setScissor(newscissor.x(), newscissor.y(), newscissor.width(), newscissor.height());
}
//-----------------------------------------------------------------------------
void VectorGraphics::popScissor() 
{ 
  if (mScissorStack.empty())
  {
    vl::Log::error("VectorGraphics::popScissor() scissor stack underflow!\n");
    return;
  }
  mScissor = mScissorStack.back();
  mScissorStack.pop_back();
}
//-----------------------------------------------------------------------------
void VectorGraphics::generateQuadsTexCoords(vl::Geometry* geom, const std::vector<vl::dvec2>& points)
{
  // generate only if there is an image active
  if (mState.mImage)
  {
    vl::ref<vl::ArrayFVec2> tex_array = new vl::ArrayFVec2;
    tex_array->resize(geom->vertexArray()->size());
    geom->setTexCoordArray(0, tex_array.get());
    if (mState.mTextureMode == TextureMode_Clamp)
    {
      float du = 1.0f / mState.mImage->width()  / 2.0f;
      float dv = mState.mImage->height() ? (1.0f / mState.mImage->height() / 2.0f) : 0.5f;
      //  1----2
      //  |    |
      //  |    |
      //  0    3
      vl::fvec2 texc[] = { vl::fvec2(du,dv), vl::fvec2(du,1.0f-dv), vl::fvec2(1.0f-du,1.0f-dv), vl::fvec2(1.0f-du,dv) };
      for(unsigned i=0; i<points.size(); ++i)
      {
        float s = texc[i%4].s();
        float t = texc[i%4].t();
        tex_array->at(i).s() = s;
        tex_array->at(i).t() = t;
      }
    }
    else
    {
      vl::AABB aabb;
      for(unsigned i=0; i<points.size(); ++i)
        aabb.addPoint( (vl::vec3)geom->vertexArray()->vectorAsVec4(i).xyz() );
      for(unsigned i=0; i<points.size(); ++i)
      {
        vl::vec4 v = geom->vertexArray()->vectorAsVec4(i);
        double s = (geom->vertexArray()->vectorAsVec4(i).s()-aabb.minCorner().s()) / (mState.mImage->width() );
        double t = (geom->vertexArray()->vectorAsVec4(i).t()-aabb.minCorner().t()) / (mState.mImage->height());
        tex_array->at(i).s() = (float)s;
        tex_array->at(i).t() = (float)t;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void VectorGraphics::generatePlanarTexCoords(vl::Geometry* geom, const std::vector<vl::dvec2>& points)
{
  // generate only if there is an image active
  if (mState.mImage)
  {
    // generate uv coordinates based on the aabb
    vl::ref<vl::ArrayFVec2> tex_array = new vl::ArrayFVec2;
    tex_array->resize(geom->vertexArray()->size());
    geom->setTexCoordArray(0, tex_array.get());
    if (mState.mTextureMode == TextureMode_Clamp)
    {
      // compute aabb
      vl::AABB aabb;
      for(unsigned i=0; i<points.size(); ++i)
        aabb.addPoint( (vl::vec3)vl::dvec3(points[i],0.0) );
      for(unsigned i=0; i<points.size(); ++i)
      {
        float s = float((points[i].x() - aabb.minCorner().x()) / aabb.width() );
        float t = float((points[i].y() - aabb.minCorner().y()) / aabb.height());
        tex_array->at(i).s() = s;
        tex_array->at(i).t() = t;
      }
    }
    else
    {
      vl::AABB aabb;
      for(unsigned i=0; i<points.size(); ++i)
        aabb.addPoint( (vl::vec3)geom->vertexArray()->vectorAsVec4(i).xyz()+vl::vec3(0.5f,0.5f,0.0f) );
      for(unsigned i=0; i<points.size(); ++i)
      {
        vl::vec4 v = geom->vertexArray()->vectorAsVec4(i);
        double s = (geom->vertexArray()->vectorAsVec4(i).s()-aabb.minCorner().s()) / mState.mImage->width();
        double t = (geom->vertexArray()->vectorAsVec4(i).t()-aabb.minCorner().t()) / mState.mImage->height();
        tex_array->at(i).s() = (float)s;
        tex_array->at(i).t() = (float)t;
      }
    }
  }
}
//-----------------------------------------------------------------------------
void VectorGraphics::generateLinearTexCoords(vl::Geometry* geom)
{
  if (mState.mImage)
  {
    vl::ref<vl::ArrayFloat> tex_array = new vl::ArrayFloat;
    tex_array->resize(geom->vertexArray()->size());
    float u1 = 1.0f / mState.mImage->width() * 0.5f;
    float u2 = 1.0f - 1.0f / mState.mImage->width() * 0.5f;
    for(size_t i=0; i<tex_array->size(); ++i)
    {
      float t = (float)i/(tex_array->size()-1);
      tex_array->at(i) = u1 * (1.0f-t) + u2 * t;
    }
    // generate geometry
    geom->setTexCoordArray(0, tex_array.get());
  }
}
//-----------------------------------------------------------------------------
vl::ref<vl::Geometry> VectorGraphics::prepareGeometry(const std::vector<vl::dvec2>& ln)
{
  // transform the lines
  vl::ref<vl::ArrayFVec3> pos_array = new vl::ArrayFVec3;
  pos_array->resize(ln.size());
  // transform done using high precision
  for(unsigned i=0; i<ln.size(); ++i)
    pos_array->at(i) = (vl::fvec3)(matrix() * vl::dvec3(ln[i].x(), ln[i].y(), 0));
  // generate geometry
  vl::ref< vl::Geometry > geom = new vl::Geometry;
  geom->setVertexArray(pos_array.get());
  return geom;
}
//-----------------------------------------------------------------------------
vl::Scissor* VectorGraphics::resolveScissor(int x, int y, int width, int height)
{
  vl::ref<vl::Scissor> scissor = mRectToScissorMap[vl::RectI(x,y,width,height)];
  if (!scissor)
  {
    scissor = new vl::Scissor(x,y,width,height);
    mRectToScissorMap[vl::RectI(x,y,width,height)] = scissor;
  }
  return scissor.get();
}
//-----------------------------------------------------------------------------
vl::Texture* VectorGraphics::resolveTexture(vl::Image* image)
{
  vl::Texture* texture = mImageToTextureMap[ImageState(image,mState.mTextureMode)].get();
  if (!texture)
  {
    texture = new vl::Texture( image, vl::TF_RGBA, true, false);
    texture->getTexParameter()->setMinFilter(vl::TPF_LINEAR_MIPMAP_LINEAR);
    texture->getTexParameter()->setMagFilter(vl::TPF_LINEAR);
    #if 0
      texture->getTexParameter()->setBorderColor(vl::fvec4(1,0,1,1)); // for debuggin purposes
    #else
      texture->getTexParameter()->setBorderColor(vl::fvec4(1,1,1,0)); // transparent white
    #endif
    if (mState.mTextureMode == vlVG::TextureMode_Repeat)
    {
      texture->getTexParameter()->setWrapS(vl::TPW_REPEAT);
      texture->getTexParameter()->setWrapT(vl::TPW_REPEAT);
    }
    else
    {
      texture->getTexParameter()->setWrapS(vl::TPW_CLAMP);
      texture->getTexParameter()->setWrapT(vl::TPW_CLAMP);
    }
    mImageToTextureMap[ImageState(image,mState.mTextureMode)] = texture;
  }
  return texture;
}
//-----------------------------------------------------------------------------
vl::Effect* VectorGraphics::currentEffect(const State& vgs)
{
  vl::Effect* effect = mVGToEffectMap[vgs].get();
  // create a Shader reflecting the current VectorGraphics state machine state
  if (!effect)
  {
    effect = new vl::Effect;
    mVGToEffectMap[vgs] = effect;
    vl::Shader* shader = effect->shader();
    /*shader->disable(vl::EN_DEPTH_TEST);*/
    shader->enable(vl::EN_BLEND);
    // color
    shader->enable(vl::EN_LIGHTING);
    shader->gocMaterial()->setFlatColor(vgs.mColor);
    // point size
    shader->gocPointSize()->set((float)vgs.mPointSize);
    // logicop
    if (vgs.mLogicOp != vl::LO_COPY)
    {
      shader->gocLogicOp()->set(vgs.mLogicOp);
      shader->enable(vl::EN_COLOR_LOGIC_OP);
    }
    // line stipple
    if ( vgs.mLineStipple != 0xFFFF )
    {
      shader->gocLineStipple()->set(1, vgs.mLineStipple);
      shader->enable(vl::EN_LINE_STIPPLE);
    }
    // line width
    if (vgs.mLineWidth != 1.0f)
      shader->gocLineWidth()->set(vgs.mLineWidth);
    // point smooth
    if (vgs.mPointSmoothing)
    {
      shader->gocHint()->setPointSmoothHint(vl::HM_NICEST);
      shader->enable(vl::EN_POINT_SMOOTH);
    }
    // line smooth
    if (vgs.mLineSmoothing)
    {
      shader->gocHint()->setLineSmoothHint(vl::HM_NICEST);
      shader->enable(vl::EN_LINE_SMOOTH);
    }
    // polygon smooth
    if (vgs.mPolygonSmoothing)
    {
      shader->gocHint()->setPolygonSmoohtHint(vl::HM_NICEST);
      shader->enable(vl::EN_POLYGON_SMOOTH);
    }
    // poly stipple
    unsigned char solid_stipple[] = {
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 
      0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF, 0xFF,0xFF,0xFF,0xFF
    };
    if ( memcmp(vgs.mPolyStipple, solid_stipple, 32*32/8) != 0 )
    {
      shader->gocPolygonStipple()->set(vgs.mPolyStipple);
      shader->enable(vl::EN_POLYGON_STIPPLE);
    }
    // blending equation and function
    shader->gocBlendEquation()->set(vgs.mBlendEquationRGB, vgs.mBlendEquationAlpha);
    shader->gocBlendFunc()->set(vgs.mBlendFactorSrcRGB, vgs.mBlendFactorDstRGB, vgs.mBlendFactorSrcAlpha, vgs.mBlendFactorDstAlpha);
    if (vgs.mAlphaFunc != vl::FU_ALWAYS)
    {
      shader->enable(vl::EN_ALPHA_TEST);
      shader->gocAlphaFunc()->set(vgs.mAlphaFunc, vgs.mAlphaFuncRefValue);
    }
    // masks (by default they are all 'true')
    if (vgs.mColorMask != vl::ivec4(1,1,1,1) )
      shader->gocColorMask()->set(vgs.mColorMask.r()?true:false,vgs.mColorMask.g()?true:false,vgs.mColorMask.b()?true:false,vgs.mColorMask.a()?true:false);
    // stencil
    if (vgs.mStencilTestEnabled)
    {
      shader->enable(vl::EN_STENCIL_TEST);
      shader->gocStencilMask()->set(vl::PF_FRONT_AND_BACK, vgs.mStencilMask);
      shader->gocStencilOp()->set(vl::PF_FRONT_AND_BACK, vgs.mStencil_SFail, vgs.mStencil_DpFail, vgs.mStencil_DpPass);
      shader->gocStencilFunc()->set(vl::PF_FRONT_AND_BACK, vgs.mStencil_Function, vgs.mStencil_RefValue, vgs.mStencil_FunctionMask);
    }
    /*if (!vgs.mDepthMask)
      shader->gocDepthMask()->set(false);*/
    // texture
    if (vgs.mImage)
    {
      shader->gocTextureUnit(0)->setTexture( resolveTexture(vgs.mImage.get()) );
      if (GLEW_ARB_point_sprite || GLEW_VERSION_2_0)
      {
        shader->gocTexEnv(0)->setPointSpriteCoordReplace(true);
        shader->enable(vl::EN_POINT_SPRITE);
      }
      else
        vl::Log::error("GL_ARB_point_sprite not supported.\n");
    }
  }
  return effect;
}
//-----------------------------------------------------------------------------
vl::Actor* VectorGraphics::addActor(vl::Actor* actor) 
{ 
  actor->setScissor(mScissor.get());
  mActors.push_back(actor);
  return actor;
}
//-----------------------------------------------------------------------------