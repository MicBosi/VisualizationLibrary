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

#import "GL/glew.h"

#include "vl/OpenGLContext.hpp"

#include "vlCocoa/CGLContext.hpp"

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

@interface vlCocoaView : NSOpenGLView
{
   CVDisplayLinkRef           displayLink;  // For managing the rendering thread
   NSRect                     bounds;
   vl::ref<vlCGL::CGLContext> _GLView;
   
   IBOutlet id                _delegate;
}

-(void) awakeFromNib;
-(BOOL) acceptsFirstResponder;

-(id) initWithFrame: (NSRect) frame;
-(void) dealloc;

-(vl::ref<vlCGL::CGLContext>) context;
-(void) setContext: (vl::ref<vlCGL::CGLContext>) ctxt;

-(void) prepareVLProgram: (vlCocoaView*)view;
-(void) prepareOpenGL;

// Animation support
-(CVReturn) getFrameForTime: (const CVTimeStamp*) outputTime;

// Delegation
-(id) delegate;
-(void) setDelegate: (id)newDelegate;

+(NSOpenGLPixelFormat*) getFormatFromContext: (vl::OpenGLContextFormat*) info;

@end

@interface NSObject (VLProgramDelegate)
-(void) prepareVLProgram: (vlCocoaView*)view;
@end
