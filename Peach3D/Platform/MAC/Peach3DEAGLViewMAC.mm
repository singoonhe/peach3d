//
//  Peach3DEAGLViewMAC.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/3/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#import "Peach3DEAGLViewMAC.h"
#import "Peach3DCommonGL.h"
#import "Peach3DPlatformMAC.h"

using namespace Peach3D;
@implementation EAGLViewMAC

// This is the renderer output callback function
static CVReturn gameDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    // make opengl view update
    CVReturn result = [(__bridge EAGLViewMAC*)displayLinkContext getFrameForTime:outputTime];
    return result;
}

- (id)initWithFrame:(NSRect)frameRect pixelFormat:(NSOpenGLPixelFormat *)format
{
    mLastFrameTime = 0.0;
    mDisplayLink = NULL;
    self = [super initWithFrame:frameRect pixelFormat:format];
    
    // add mouse move event
    NSTrackingAreaOptions options = (NSTrackingActiveAlways | NSTrackingInVisibleRect |
                                     NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved);
    NSTrackingArea *area = [[NSTrackingArea alloc] initWithRect:[self bounds]
                                                        options:options
                                                          owner:self
                                                       userInfo:nil];
    [self addTrackingArea:area];
    return self;
}

- (void)prepareOpenGL
{
    [super prepareOpenGL];
    // init render
    PlatformMAC* platform = static_cast<PlatformMAC*>(IPlatform::getSingletonPtr());
    platform->initRenderGL();
    
    // Create a display link and set callback
    CVDisplayLinkCreateWithActiveCGDisplays(&mDisplayLink);
    CVDisplayLinkSetOutputCallback(mDisplayLink, &gameDisplayLinkCallback, (__bridge void*)self);
    // Set the display link for the current renderer
    CGLContextObj cglContext = [[self openGLContext] CGLContextObj];
    CGLPixelFormatObj cglPixelFormat = [[self pixelFormat] CGLPixelFormatObj];
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(mDisplayLink, cglContext, cglPixelFormat);
    
    // start render loop
    [self resume];
}

- (void)dealloc
{
    if (mDisplayLink) {
        // Release the display link
        CVDisplayLinkRelease(mDisplayLink);
        mDisplayLink = NULL;
    }
}

- (void)drawRect:(NSRect)dirtyRect
{
    // Ignore if the display link is still running
    if (!CVDisplayLinkIsRunning(mDisplayLink))
        [self drawView:0.f];
}

- (void)drawView:(float)currentTime
{
    // This method will be called on both the main thread (through -drawRect:) and a secondary thread (through the display link rendering loop)
    // Also, when resizing the view, -reshape is called on the main thread, but we may be drawing on a secondary thread
    // Add a mutex around to avoid the threads accessing the context simultaneously
    CGLLockContext([[self openGLContext] CGLContextObj]);
    
    // Make sure we draw to the right context
    [[self openGLContext] makeCurrentContext];
    // render one frame
    IPlatform::getSingletonPtr()->renderOneFrame(currentTime);
    [[self openGLContext] flushBuffer];
    
    CGLUnlockContext([[self openGLContext] CGLContextObj]);
}

- (CVReturn)getFrameForTime:(const CVTimeStamp*)outputTime
{
    // calculate the fixed frame delay time
    static CFAbsoluteTime fixedFrameTime = 1.0 / IPlatform::getSingleton().getCreationParams().maxFPS;
    static CFAbsoluteTime multiFFT = 10.0 * fixedFrameTime;
    
    CFAbsoluteTime nowTime = CFAbsoluteTimeGetCurrent();
    // When window resume from miniaturize, drawRect will called by MAC.
    // But timer is not valid, ignore this drawRect.
    if (mLastFrameTime > 0.0) {
        static double fpsTime = 0.0;
        // calculate the deltaTime
        fpsTime += (nowTime - mLastFrameTime);
        
        if (fpsTime >= multiFFT) {
            // render one frame with time
            [self drawView:fpsTime];
            fpsTime = 0.0;
        }
        else if (fpsTime >= fixedFrameTime) {
            // render one frame with time
            [self drawView:fixedFrameTime];
            fpsTime -= fixedFrameTime;
        }
    }
    mLastFrameTime = nowTime;
    
    return kCVReturnSuccess;
}

- (void)mouseDown:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eDown];
}

- (void)rightMouseDown:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eRightDown];
}

- (void)mouseUp:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eUp];
}

- (void)rightMouseUp:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eRightUp];
}

- (void)mouseMoved:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eMoved];
}

- (void)mouseDragged:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eDrag];
}

- (void)scrollWheel:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eScrollWheel];
}

- (void)rightMouseDragged:(NSEvent *)theEvent
{
    [self triggerEvent:theEvent withType:ClickEvent::eDrag];
}

- (void)triggerEvent:(NSEvent *)theEvent withType:(ClickEvent)type
{
    // get current position and trigger event
    CGPoint point = [theEvent locationInWindow];
    if (type == ClickEvent::eScrollWheel) {
        point = CGPointMake(theEvent.deltaX, theEvent.deltaY);
    }
    std::vector<uint> clickIds = {1};
    std::vector<Vector2> poss = {Vector2(point.x, point.y)};
    EventDispatcher::getSingletonPtr()->triggerClickEvent(type, clickIds, poss);
}

- (void)pause
{
    if (CVDisplayLinkIsRunning(mDisplayLink)) {
        // stop the display link
        CVDisplayLinkStop(mDisplayLink);
    }
}

- (void)resume
{
    if (!CVDisplayLinkIsRunning(mDisplayLink)) {
        mLastFrameTime = 0.0;
        // Activate the display link
        CVDisplayLinkStart(mDisplayLink);
    }
}

@end


@implementation EAGLViewWindow

- (void)keyDown:(NSEvent *)theEvent
{
    EventDispatcher::getSingletonPtr()->triggerKeyboardEvent(KeyboardEvent::eKeyDown, (KeyCode)theEvent.keyCode);
}

- (void)keyUp:(NSEvent *)theEvent
{
    EventDispatcher::getSingletonPtr()->triggerKeyboardEvent(KeyboardEvent::eKeyUp, (KeyCode)theEvent.keyCode);
}

- (BOOL)canBecomeKeyWindow
{
    return YES;
}

- (BOOL)canBecomeMainWindow
{
    return YES;
}

@end