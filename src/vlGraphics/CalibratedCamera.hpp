#ifndef CalibratedCamera_INCLUDE_ONCE
#define CalibratedCamera_INCLUDE_ONCE

#include <vlGraphics/Camera.hpp>

namespace vl
{
  //-----------------------------------------------------------------------------
  // CalibratedCamera
  //-----------------------------------------------------------------------------
  /** Calibrated camera for augmented reality.
    *
    * To use the CalibratedCamera keep the following in mind:
    *
    * - on resize: call setScreenSize()
    * - on intrinsic change: call setIntrinsicParameters()
    * - on background image change: call setCalibratedImageSize()
    * - disable automatic near/far plane automatic calculation: vl::Rendering::setNearFarClippingPlanesOptimized( false );
    * - set near/far clipping planes (ex. setNearPlane( 1 ), setFarPlane( 10000 ))
    * - finally call updateCalibration() to recompute the projectionMatrix() based on the above
    * - note: fov() is ignored
    *
    * \see http://strawlab.org/2011/11/05/augmented-reality-with-OpenGL/
    * \see http://sightations.wordpress.com/2010/08/03/simulating-calibrated-cameras-in-opengl/
    * \see http://jamesgregson.blogspot.co.uk/2011/11/matching-calibrated-cameras-with-opengl.html
    * \see http://www.robots.ox.ac.uk/~vgg/hzbook/
    */
  class VLGRAPHICS_EXPORT CalibratedCamera: public Camera
  {
    VL_INSTRUMENT_CLASS(vl::CalibratedCamera, Camera)

  public:
    CalibratedCamera();

    /**
      * Set the size of the image in pixels that was used while calibrating the camera model.
      *
      * Call updateCalibration() to apply the changes to the projection matrix.
      *
      * \param width in pixels.
      * \param height in pixels.
      * \param pixelaspect is the width of a single pixel divided by its height.
      */
    void setCalibratedImageSize(int width, int height, float pixelaspect = 1.0);

    /**
      * Set the intrinsic parameters as determined from camera calibration.
      *
      * Call updateCalibration() to apply the changes to the projection matrix.
      *
      * \param fx scale factor of combined focal length * effective pixel size in horizontal direction.
      * \param fy scale factor of combined focal length * effective pixel size in vertical direction.
      * \param cx centre of projection in x axis.
      * \param cy centre of projection in y axis.
      */
    void setIntrinsicParameters(float fx, float fy, float cx, float cy);

    /**
      * Usually you don't need to call this as the renderer takes care of keeping it up to date
      * with whatever the current framebuffer size is.
      *
      * Set the screen/widget size currently used.
      *
      * Call updateCalibration() to apply the changes to the projection matrix.
      *
      * \param width in pixels.
      * \param height in pixels.
      */
    void setScreenSize(int width, int height);

    /** Image width in pixels. */
    int imageWidth() const { return mImageWidth; }
    
    /** Image height in pixels. */
    int imageHeight() const { return mImageHeight; }
    
    /** Image width in pixels. */
    int screenWidth() const { return mScreenWidth; }
    
    /** Screen height in pixels. */
    int screenHeight() const { return mScreenHeight; }
    
    /** The width of a single pixel divided by its height. */
    float pixelAspectRatio() const { return mPixelAspectRatio; }

    /** Recomputes the *projection matrix* and *viewport* based on the calibration data specified by
      * setCalibratedImageSize(), setScreenSize(), setIntrinsicParameters(), setNearPlane(), setFarPlane()
      * \note Scissor test is not enabled by default but it's a good idea to enabled it probably.
      */
    void updateCalibration();

  protected:
      int mImageWidth;
      int mImageHeight;
      int mScreenWidth;
      int mScreenHeight;
      float mPixelAspectRatio;
      float mFx;
      float mFy;
      float mCx;
      float mCy;
  };
}

#endif
