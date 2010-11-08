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

#include "GL/glew.h"

#include "CGLContext.hpp"
#import "vlCocoaView.h"

#include "vl/Vector4.hpp"
#include "vl/Matrix4.hpp"
#include "vl/Log.hpp"
#include "vl/Say.hpp"
#include "vl/String.hpp"
#include "vl/VisualizationLibrary.hpp"

// Helper routines for converting between Cocoa and VL formats
vl::OpenGLContextFormat getContextFromFormat (NSOpenGLPixelFormat* format);

@implementation vlCocoaView

-(void)awakeFromNib
{
   vl::VisualizationLibrary::init();

   // We accept images dragged to the view
   [self registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
}

// Accept keyboard/mouse events
-(BOOL) acceptsFirstResponder
{
   return YES;
}

- (id) delegate
{
   return _delegate;
}

- (void) setDelegate: (id)newDelegate
{
   _delegate = newDelegate;
}

-(vl::ref<vlCGL::CGLContext>) context
{
   return _GLView;
}

-(void) setContext: (vl::ref<vlCGL::CGLContext>) ctxt;
{
   _GLView = ctxt;
}

#pragma mark -
#pragma mark Initialization Routines
/**
 * Standard initialization for a child of NSView.
 * We need to specify the proper conditions for OpenGL to use.
 *
 * @param frame Region to use for display of the OpenGL scene.
 * @return The initialized view.
 */
- (id)initWithFrame:(NSRect)frame
{
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

	return [super initWithFrame:frame pixelFormat: [fmt autorelease]];
}

- (void)dealloc
{
   // Release the display link
   CVDisplayLinkRelease(displayLink);

   [super dealloc];
}


#pragma mark -
#pragma mark Animation Support functions

// This is the renderer output callback function
static CVReturn displayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
   CVReturn result = [(vlCocoaView*)displayLinkContext getFrameForTime: outputTime];
   return result;
}

//#include "TestBatteryCocoa.hpp"   // For testing
/**
 * Prepare the VL Program to be run.
 * This is typicaly the only thing that needs to be changed in this class (should probably be a delegate?)
 */
- (void)prepareVLProgram: (vlCocoaView*)view
{
   // Read command line arguments
   int test = [[NSUserDefaults standardUserDefaults] integerForKey: @"test"];
   float secs = [[NSUserDefaults standardUserDefaults] floatForKey: @"time"];

   if (!test)
      test = 46;

   vl::Log::print( vl::Say("Test #%n %.1ns-------------------------------------------------------------\n\n") << test << secs );

   // setup the OpenGL context format
   vl::OpenGLContextFormat format;
   format.setDoubleBuffer(true);
   format.setRGBABits( 8,8,8,8 );
   format.setDepthBufferBits(24);
   format.setStencilBufferBits(8);
   format.setFullscreen(false);
   format.setMultisampleSamples(16);
   format.setMultisample(false);

   NSOpenGLPixelFormat* nsFmt = [vlCocoaView getFormatFromContext: &format];
   [view setPixelFormat: nsFmt];

   CGLContextObj context = (CGLContextObj)[[view openGLContext] CGLContextObj];
   /*  // For testing
   TestBatteryCocoa test_battery (context);
   test_battery.run (test, secs, format);

   std::string narrowTitle = test_battery.title ().toStdString ();
   NSString* nsTitle = [NSString stringWithCString: &narrowTitle[0] encoding: NSASCIIStringEncoding];
   NSLog(@"Got title = %@", nsTitle);
   [[view window] setTitle: nsTitle];

   [view setContext: test_battery.context ()];
   */
}


/**
 * Prepare the OpenGL context to be drawn.
 */
- (void) prepareOpenGL
{
   // Synchronize buffer swaps with vertical refresh rate
   GLint swapInt = 1;
   [[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];

   // Create a display link capable of being used with all active displays
   CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);

   // Set the renderer output callback function
   CVDisplayLinkSetOutputCallback(displayLink, &displayLinkCallback, self);

   // Set the display link for the current renderer
   CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
   CGLLockContext(cglContext);
   CGLSetCurrentContext(cglContext);

   CGLPixelFormatObj cglPixelFormat =  (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];

   vl::VisualizationLibrary::init();

   CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);

   if ([self.delegate respondsToSelector: @selector(prepareVLProgram:)])
      [self.delegate prepareVLProgram: self];
   else
      [self prepareVLProgram: self];

   if (_GLView)
      _GLView->dispatchRunEvent();

   VL_CHECK_OGL()

   CGLUnlockContext (cglContext);

   // Activate the display link
   CVDisplayLinkStart(displayLink);
}

- (CVReturn) getFrameForTime: (const CVTimeStamp*) outputTime
{
   if (_GLView)
   {
      CGLLockContext (_GLView->getContext ());
      _GLView->makeCurrent ();
      _GLView->dispatchRunEvent();
      CGLUnlockContext (_GLView->getContext ());
   }

   return kCVReturnSuccess;
}

- (void)reshape
{
   if (_GLView) {
      CGLLockContext (_GLView->getContext ());
      NSRect boundBox = [self bounds];
      _GLView->makeCurrent ();
      _GLView->dispatchResizeEvent (boundBox.size.width, boundBox.size.height);
      CGLUnlockContext (_GLView->getContext ());
   }

   [super reshape];
}

- (void) drawRect: (NSRect) rect
{
   if (_GLView)
   {
      // Have VL render the scene
      CGLLockContext (_GLView->getContext ());
      _GLView->makeCurrent ();

      [super drawRect: rect];

      CGLUnlockContext (_GLView->getContext ());
   }
   else
      [super drawRect: rect];
}

#pragma mark -
#pragma mark Key Event Handlers

-(void) keyDown: (NSEvent*) theEvent
{
   NSString* characters = [theEvent characters];
   unsigned int modifiers = [theEvent modifierFlags];
   if ([characters length])
   {
      unichar character = [characters characterAtIndex:0];
      CGLLockContext (_GLView->getContext ());
      _GLView->makeCurrent ();
      _GLView->translateKeyPress (character, modifiers);
      CGLUnlockContext (_GLView->getContext ());
	}
}

-(void) keyUp: (NSEvent*) theEvent
{
   NSString* characters = [theEvent characters];
   unsigned int modifiers = [theEvent modifierFlags];
   if ([characters length])
   {
      unichar character = [characters characterAtIndex:0];
      CGLLockContext (_GLView->getContext ());
      _GLView->makeCurrent ();
      _GLView->translateKeyRelease (character, modifiers);
      CGLUnlockContext (_GLView->getContext ());
	}
}

#pragma mark -
#pragma mark Mouse Event Handlers

-(void) mouseDown: (NSEvent*) theEvent
{
   NSPoint mousePos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
   vl::EMouseButton bt = vl::NoButton;
   switch([theEvent type])
   {
      case NSLeftMouseDown:  bt = vl::LeftButton; break;
      case NSRightMouseDown: bt = vl::RightButton; break;
      case NSOtherMouseDown:
         bt = vl::MiddleButton;
         break;

      default:
         bt = vl::UnknownButton; break;
   }

   NSRect frame = [self frame];
   CGFloat newY = (frame.origin.y + frame.size.height) - mousePos.y;

   CGLLockContext (_GLView->getContext ());
   _GLView->makeCurrent ();
   _GLView->dispatchMouseDownEvent (bt, mousePos.x,newY);
   CGLUnlockContext (_GLView->getContext ());
}

-(void) mouseUp: (NSEvent*) theEvent
{
   NSPoint mousePos = [self convertPoint:[theEvent locationInWindow] fromView:nil];
   vl::EMouseButton bt = vl::NoButton;
   switch([theEvent type])
   {
      case NSLeftMouseUp:  bt = vl::LeftButton; break;
      case NSRightMouseUp: bt = vl::RightButton; break;
      case NSOtherMouseUp:
         bt = vl::MiddleButton;
         break;

      default:
         bt = vl::UnknownButton; break;
   }

   NSRect frame = [self frame];
   CGFloat newY = (frame.origin.y + frame.size.height) - mousePos.y;

   CGLLockContext (_GLView->getContext ());
   _GLView->makeCurrent ();
   _GLView->dispatchMouseUpEvent (bt, mousePos.x, newY);
   CGLUnlockContext (_GLView->getContext ());
}

-(void) mouseDragged: (NSEvent*) theEvent
{
   NSPoint mousePos = [self convertPoint:[theEvent locationInWindow] fromView:nil];

   NSRect frame = [self frame];
   CGFloat newY = (frame.origin.y + frame.size.height) - mousePos.y;

   CGLLockContext (_GLView->getContext ());
   _GLView->makeCurrent ();
   _GLView->dispatchMouseMoveEvent (mousePos.x, newY);
   CGLUnlockContext (_GLView->getContext ());
}

-(void) scrollWheel: (NSEvent*)theEvent
{
   NSPoint delta;
   delta.x = [theEvent deltaX];
   delta.y = [theEvent deltaY];

   NSPoint mouseChange = [self convertPoint:delta fromView:nil];

   // VL Only handles Y-wheel events...
   CGLLockContext (_GLView->getContext ());
   _GLView->makeCurrent ();
   _GLView->dispatchMouseWheelEvent (-mouseChange.y);
   CGLUnlockContext (_GLView->getContext ());
}

#pragma mark -
#pragma mark Drag/Drop Event Handlers

- (NSDragOperation) draggingEntered: (id <NSDraggingInfo>) sender
{
   NSPasteboard* pboard = [sender draggingPasteboard];

   if ( [[pboard types] containsObject: NSFilenamesPboardType] ) {
      return NSDragOperationGeneric;
   }
   return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
   NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
   NSPasteboard* pboard = [sender draggingPasteboard];

   if ( [[pboard types] containsObject:NSFilenamesPboardType] ) {
      NSArray* files = [pboard propertyListForType:NSFilenamesPboardType];

      std::vector<vl::String> vlFiles;
      for (unsigned i = 0; i < [files count]; ++i)
      {
         NSString* filename = [files objectAtIndex: i];
         vlFiles.push_back (vl::String (reinterpret_cast<const wchar_t*>([filename cStringUsingEncoding: NSUTF32StringEncoding])));
      }

      CGLLockContext (_GLView->getContext ());
      _GLView->makeCurrent ();
      _GLView->dispatchFileDroppedEvent (vlFiles);
      CGLUnlockContext (_GLView->getContext ());
   }

   return YES;
}

#pragma mark -
#pragma mark Helper routines
//
//  Helper routines for VL<-->Cocoa formats
//
vl::OpenGLContextFormat getContextFromFormat (NSOpenGLPixelFormat* format)
{
   vl::OpenGLContextFormat info;

   if (!format)
      return info;

   GLint swapInterval = 0, doubleBuffer = 0, stereo = 0, fullScreen = 0, rgbSize = 0, alphaSize = 0;
   GLint stencilSize = 0, multiSample = 0, multiSamples = 0, accumSize = 0, depthSize = 0;
   [format getValues:&swapInterval forAttribute: NSOpenGLCPSwapInterval  forVirtualScreen: 0];
   [format getValues:&doubleBuffer forAttribute: NSOpenGLPFADoubleBuffer forVirtualScreen: 0];
   [format getValues:&stereo       forAttribute: NSOpenGLPFAStereo       forVirtualScreen: 0];
   [format getValues:&fullScreen   forAttribute: NSOpenGLPFAFullScreen   forVirtualScreen: 0];
   [format getValues:&rgbSize      forAttribute: NSOpenGLPFAColorSize    forVirtualScreen: 0];
   [format getValues:&alphaSize    forAttribute: NSOpenGLPFAAlphaSize    forVirtualScreen: 0];
   [format getValues:&stencilSize  forAttribute: NSOpenGLPFAStencilSize  forVirtualScreen: 0];
   [format getValues:&multiSample  forAttribute: NSOpenGLPFAMultisample  forVirtualScreen: 0];
   [format getValues:&multiSamples forAttribute: NSOpenGLPFASamples      forVirtualScreen: 0];
   [format getValues:&accumSize    forAttribute: NSOpenGLPFAAccumSize    forVirtualScreen: 0];
   [format getValues:&depthSize    forAttribute: NSOpenGLPFADepthSize    forVirtualScreen: 0];

   size_t indSize = rgbSize / 3;

   info.setDoubleBuffer(doubleBuffer != 0);
   info.setStereo(stereo != 0);
   info.setFullscreen(fullScreen != 0);
   info.setRGBABits( indSize,indSize,indSize,alphaSize );
   info.setStencilBufferBits(stencilSize);
   info.setMultisample(multiSample != 0);
   info.setMultisampleSamples(multiSamples);
   info.setAccumRGBABits( accumSize,accumSize,accumSize,accumSize );
   info.setDepthBufferBits(depthSize);
   info.setVSync(swapInterval != 0);

   return info;
}

+(NSOpenGLPixelFormat*) getFormatFromContext: (vl::OpenGLContextFormat*) info
{
   std::vector<GLuint> attribs;

   attribs.push_back(NSOpenGLPFAAccelerated);

   if (info->doubleBuffer())
      attribs.push_back(NSOpenGLPFADoubleBuffer);
   if (info->stereo())
      attribs.push_back(NSOpenGLPFAStereo);
   if (info->fullscreen())
      attribs.push_back(NSOpenGLPFAFullScreen);

   size_t rgbBits = info->rgbaBits().r() + info->rgbaBits().g() + info->rgbaBits().b();
   attribs.push_back(NSOpenGLPFAColorSize);
   attribs.push_back(rgbBits);

   attribs.push_back(NSOpenGLPFAAlphaSize);
   attribs.push_back(info->rgbaBits().a());

   // swap interval / v-sync
   if (info->vSync()) {
      attribs.push_back(NSOpenGLCPSwapInterval);
      attribs.push_back(1);
   }

   // accumulation buffer
   int accum = vl::max( info->accumRGBABits().r(), info->accumRGBABits().g() );
   accum = vl::max( accum, info->accumRGBABits().b() );
   accum = vl::max( accum, info->accumRGBABits().a() );
   if (accum != 0) {
      attribs.push_back(NSOpenGLPFAAccumSize);
      attribs.push_back(accum);
   }

   // depth buffer
   if (info->depthBufferBits() != 0) {
      attribs.push_back(NSOpenGLPFADepthSize);
      attribs.push_back(info->depthBufferBits());
   }

   // stencil buffer
   if (info->stencilBufferBits() != 0) {
      attribs.push_back(NSOpenGLPFAStencilSize);
      attribs.push_back(info->stencilBufferBits());
   }

   // multisampling
   if (info->multisample()) {
      attribs.push_back(NSOpenGLPFAMultisample);
      attribs.push_back(NSOpenGLPFASamples);
      attribs.push_back(info->multisampleSamples());
   }

   attribs.push_back (0);  // Null terminate!

   return [[[NSOpenGLPixelFormat alloc] initWithAttributes: &attribs[0]] autorelease];
}

@end
