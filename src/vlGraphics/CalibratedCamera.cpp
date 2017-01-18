#include <vlGraphics/CalibratedCamera.hpp>

#undef near
#undef far

using namespace vl;

//-----------------------------------------------------------------------------
// CalibratedCamera
//-----------------------------------------------------------------------------
CalibratedCamera::CalibratedCamera()
: mImageWidth(0)
, mImageHeight(0)
, mScreenWidth(0)
, mScreenHeight(0)
, mPixelAspectRatio(1.0)
, mFx(1)
, mFy(1)
, mCx(0)
, mCy(0)
{
  VL_DEBUG_SET_OBJECT_NAME()
}
//-----------------------------------------------------------------------------
void CalibratedCamera::setCalibratedImageSize(int width, int height, float pixelaspect)
{
  mImageWidth = width;
  mImageHeight = height;
  mPixelAspectRatio = pixelaspect;
}
//----------------------------------------------------------------------------
void CalibratedCamera::setScreenSize(int width, int height)
{
  mScreenWidth = width;
  mScreenHeight = height;
}
//----------------------------------------------------------------------------
void CalibratedCamera::setIntrinsicParameters(float fx, float fy, float cx, float cy)
{
  mFx = fx;
  mFy = fy;
  mCx = cx;
  mCy = cy;
}
//----------------------------------------------------------------------------
void CalibratedCamera::updateCalibration()
{
  double znear = mNearPlane;
  double zfar = mFarPlane;

  // Inspired by: http://strawlab.org/2011/11/05/augmented-reality-with-OpenGL/
  /*
   [2*K00/width, -2*K01/width,    (width - 2*K02 + 2*x0)/width,                            0]
   [          0, 2*K11/height, (-height + 2*K12 + 2*y0)/height,                            0]
   [          0,            0,  (-zfar - znear)/(zfar - znear), -2*zfar*znear/(zfar - znear)]
   [          0,            0,                              -1,                            0]
   */

  dmat4 proj;
  proj.setNull();
  proj.e(0, 0) = 2 * mFx / mImageWidth;
  proj.e(0, 1) = -2 * 0 / mImageWidth;
  proj.e(0, 2) = ( mImageWidth - 2 * mCx ) / mImageWidth;
  proj.e(1, 1) = 2 * ( mFy / mPixelAspectRatio ) / ( mImageHeight / mPixelAspectRatio );
  proj.e(1, 2) = ( - ( mImageHeight / mPixelAspectRatio ) + 2 * ( mCy / mPixelAspectRatio ) ) / ( mImageHeight / mPixelAspectRatio );
  proj.e(2, 2) = ( - zfar - znear ) / ( zfar - znear );
  proj.e(2, 3) = - 2 * zfar * znear / ( zfar - znear );
  proj.e(3, 2) = -1;

  setProjectionMatrix((mat4)proj, vl::PMT_UserProjection);

  double widthScale  = (double) mScreenWidth  / (double) mImageWidth;
  double heightScale  = (double) mScreenHeight  / ( (double) mImageHeight / mPixelAspectRatio );

  int vpw = mScreenWidth;
  int vph = mScreenHeight;

  if ( widthScale < heightScale ) {
    vph = (int) ( ( (double) mImageHeight / mPixelAspectRatio ) * widthScale );
  }
  else {
    vpw = (int) ( (double) mImageWidth * heightScale );
  }

  int vpx = mScreenWidth  / 2 - vpw / 2;
  int vpy = mScreenHeight / 2 - vph / 2;

  viewport()->set( vpx, vpy, vpw, vph );

  // debug
#if 0
  printf("\nVL Calibration params:\n");
  printf("Cxy: %.2f, %.2f\n", mCx, mCy);
  printf("Fxy: %.2f, %.2f\n", mFx, mFy);
  printf("Ixy: %d, %d, aspect: %.2f\n", mImageWidth, mImageHeight, mPixelAspectRatio);
  printf("Z near & far: %.2f, %.2f\n", znear, zfar);
  printf("VP: %d, %d, %d, %d\n", vpx, vpy, vpw, vph);
  printf("\nProjection matrix:\n");
  double* m = proj.ptr();
  for(int i=0; i<4; ++i, m += 4) {
    printf("%.2f, %.2f, %.2f, %.2f\n", m[0], m[1], m[2], m[3]);
  }
#endif

  // Scissor test can be enabled externally by the caller
}

