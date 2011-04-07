/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi, Brent Fulgham                              */
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

#include "vlCocoa/CGLContext.hpp"

#import "vlCocoa/vlCocoaView.h"
#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

#include "vl/Log.hpp"
#include "vl/OpenGLContext.hpp"
#include "vl/String.hpp"
#include "vl/VisualizationLibrary.hpp"

#include "tests.hpp"

@interface Cocoa_tests : NSObject
{
   IBOutlet NSWindow* _window;
   IBOutlet vlCocoaView* theView;
}

-(id) init;

-(void) prepareVLProgram: (vlCocoaView*)view;

@end

using namespace vl;
using namespace vlCGL;

class TestBatteryCocoa: public TestBattery
{
public:
   TestBatteryCocoa (CGLContextObj context) : mCGLContextObj(context)
   {
   }
      
   vl::ref<vlCGL::CGLContext> context () { return mCGLContext; }
   vl::String                 title () { return mTitle; }
      
   void runGUI(float secs, const vl::String& title, BaseDemo* applet, vl::OpenGLContextFormat format, int x, int y, int width, int height, vl::fvec4 bk_color, vl::vec3 eye, vl::vec3 center)
   {
      mTitle = title;
         
      /* used to display the application title next to FPS counter */
      applet->setApplicationName(title);
         
      /* create a CGL context */
      mCGLContext = new vlCGL::CGLContext;
         
      setupApplet(applet, mCGLContext.get(), secs, bk_color, eye, center);
         
      /* Initialize the OpenGL context and window properties */
      mCGLContext->initCGLContext(title, mCGLContextObj, format, x, y, width, height);
         
      VL_CHECK_OGL()
   }
      
protected:
   CGLContextObj              mCGLContextObj;
   vl::String                 mTitle;
   vl::ref<vlCGL::CGLContext> mCGLContext;
};

@implementation Cocoa_tests

-(id)init
{
   if (self = [super init])
   {
      _window = [[NSWindow alloc] initWithContentRect: NSMakeRect (0, 0, 512, 512)
                                             styleMask: NSBorderlessWindowMask | NSTitledWindowMask | NSClosableWindowMask
                                               backing: NSBackingStoreBuffered
                                                 defer: NO];
      
      // Set up a typical set of conditions:
      GLuint attribs[] = 
      {
         NSOpenGLPFANoRecovery,
         NSOpenGLPFAWindow,
         NSOpenGLPFAAccelerated,
         NSOpenGLPFADoubleBuffer,
         NSOpenGLPFAColorSize, 24,
         NSOpenGLPFAAlphaSize, 8,
         NSOpenGLPFADepthSize, 24,
         0
      };   
      
      NSOpenGLPixelFormat* fmt = [[NSOpenGLPixelFormat alloc] initWithAttributes: (NSOpenGLPixelFormatAttribute*) attribs];       
      if (!fmt)
         NSLog(@"No OpenGL pixel format");
      
      theView = [[vlCocoaView alloc] initWithFrame:[_window frame] pixelFormat: [fmt autorelease]];
      [theView setDelegate: self];
      
      [_window setContentView: theView];
      [_window setDelegate: theView];
      [_window makeKeyAndOrderFront: nil];
   }
   
   return self;
}

/**
 * Prepare the VL Program to be run.
 * This is typicaly the only thing that needs to be changed in this class (should probably be a delegate?)
 */
- (void)prepareVLProgram: (vlCocoaView*)view
{
   /* setup the OpenGL context ctxi */
   vl::OpenGLContextFormat format;
   format.setDoubleBuffer(true);
   format.setRGBABits( 8,8,8,8 );
   format.setDepthBufferBits(24);
   format.setStencilBufferBits(8);
   format.setFullscreen(false);
   //format.setMultisampleSamples(16);
   //format.setMultisample(false);
   
   NSOpenGLPixelFormat* nsFmt = [vlCocoaView getFormatFromContext: &format];
   [view setPixelFormat: nsFmt];
   
   CGLContextObj context = (CGLContextObj)[[view openGLContext] CGLContextObj];
   
   int test = [[NSUserDefaults standardUserDefaults] integerForKey: @"test"];
   float secs = [[NSUserDefaults standardUserDefaults] floatForKey: @"time"];

   CGLLockContext (context);  
   TestBatteryCocoa test_battery (context);
   test_battery.run (test, secs, format);
  
   std::string narrowTitle = test_battery.title ().toStdString ();
   NSString* nsTitle = [NSString stringWithCString: &narrowTitle[0] encoding: NSASCIIStringEncoding];
   NSLog(@"Got title = %@", nsTitle);
   [[view window] setTitle: nsTitle];
   
   [view setContext: test_battery.context ()];
   CGLUnlockContext (context);
}

@end

int main(int argc, char *argv[])
{
   NSAutoreleasePool* pool = [NSAutoreleasePool new];
   NSApplication* NSApp = [NSApplication sharedApplication];
   
   Cocoa_tests* tests = [[[Cocoa_tests alloc] init] autorelease]; 
   
   [NSApp run];
   
   [pool release];
   return 0;   
}

