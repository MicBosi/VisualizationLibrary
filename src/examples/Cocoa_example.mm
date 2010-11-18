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

#include "vlCocoa/CGLContext.hpp"

#import "vlCocoa/vlCocoaView.h"
#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

#include "vl/Log.hpp"
#include "vl/OpenGLContext.hpp"
#include "vl/String.hpp"
#include "vl/VisualizationLibrary.hpp"

#include "Applets/App_RotatingCube.hpp"

@interface Cocoa_example : NSObject
{
   NSWindow* _window;
   vlCocoaView* theView;
}

-(id) init;

-(void) prepareVLProgram: (vlCocoaView*)view;

@end

using namespace vl;
using namespace vlCGL;

@implementation Cocoa_example

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
   format.setMultisampleSamples(16);
   format.setMultisample(true);
   
   NSOpenGLPixelFormat* nsFmt = [vlCocoaView getFormatFromContext: &format];
   [view setPixelFormat: nsFmt];
   
   CGLContextObj context = (CGLContextObj)[[view openGLContext] CGLContextObj];
   
   CGLLockContext (context);

   /* create the applet to be run */
   vl::ref<vlut::Applet> applet = new App_RotatingCube;
   applet->initialize();
   /* create a native CGL drawing context */
   vl::ref<vlCGL::CGLContext> mCGLContext = new vlCGL::CGLContext;
   /* bind the applet so it receives all the GUI events related to the OpenGLContext */
   mCGLContext->addEventListener(applet.get());
   /* target the window so we can render on it */
   vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->setRenderTarget(mCGLContext->renderTarget());
   /* black background */
   vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->viewport()->setClearColor( vlut::black );
   /* define the camera position and orientation */
   vl::vec3 eye    = vl::vec3(0,10,35); // camera position
   vl::vec3 center = vl::vec3(0,0,0);   // point the camera is looking at
   vl::vec3 up     = vl::vec3(0,1,0);   // up direction
   vl::mat4 view_mat = vl::mat4::lookAt(eye, center, up);
   vl::VisualizationLibrary::rendering()->as<vl::Rendering>()->camera()->setInverseViewMatrix( view_mat );
   /* Initialize the OpenGL context and window properties */
   int x = 0;
   int y = 0;
   int width = 512;
   int height= 512;
   mCGLContext->initCGLContext("Visualization Library on Cocoa - Rotating Cube", context, format, x, y, width, height);
   
   NSString* nsTitle = [NSString stringWithCString: "Visualization Library on Cocoa - Rotating Cube" encoding: NSASCIIStringEncoding];
   [[view window] setTitle: nsTitle];
   
   [view setContext: mCGLContext];
   CGLUnlockContext (context);
}

@end

int main(int argc, char *argv[])
{
   NSAutoreleasePool* pool = [NSAutoreleasePool new];
   NSApplication* NSApp = [NSApplication sharedApplication];
   
   Cocoa_example* tests = [[[Cocoa_example alloc] init] autorelease]; 
        
   [NSApp run];

   [pool release];
   return 0;   
}

