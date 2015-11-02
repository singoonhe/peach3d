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

uint64_t            gLastTime = 0ULL;
double              gFixedFrameTime = 0.0;
// This is the renderer output callback function
static CVReturn gameDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp* now, const CVTimeStamp* outputTime, CVOptionFlags flagsIn, CVOptionFlags* flagsOut, void* displayLinkContext)
{
    // make opengl view update
    [(__bridge EAGLViewMAC*)displayLinkContext setNeedsDisplay:TRUE];
    return kCVReturnSuccess;
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
    
    // calculate the fixed frame delay time
    gFixedFrameTime = 1.0 / platform->getCreationParams().maxFramsePerSecond;
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
    uint64_t nowTime = CVGetCurrentHostTime();
    // When window resume from miniaturize, drawRect will called by MAC.
    // But timer is not valid, ignore this drawRect.
    if (gLastTime > 0ULL) {
        static double fpsTime = 0.0;
        // calculate the deltaTime
        fpsTime += (nowTime - gLastTime)/1000000000.0;
        
        if (fpsTime >= gFixedFrameTime) {
            // render one frame
            IPlatform::getSingletonPtr()->renderOneFrame((float)fpsTime);
            // flush window
            [[self openGLContext] flushBuffer];
            
            fpsTime = 0.0;
        }
    }
    gLastTime = nowTime;
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
        gLastTime = 0ULL;
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