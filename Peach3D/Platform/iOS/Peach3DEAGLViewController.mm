//
//  Peach3DEAGLViewController.m
//  Peach3DLib
//
//  Created by singoon.he on 9/15/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#import "Peach3DEAGLViewController.h"
#import "Peach3DLogPrinter.h"
#import "Peach3DLayoutManager.h"

using namespace Peach3D;
@implementation EAGLViewController

- (id)init
{
    self = [super init];
    mTouchList = [NSMutableDictionary dictionaryWithCapacity:10];
    
    if ([[UIDevice currentDevice].systemVersion floatValue] < 7.0) {
        // always use OpenGL ES 2.0 under 7.0
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        mEnabledVersion = RenderFeatureLevel::eGL2;
    }
    else {
        // try to use OpenGl ES 3.0
        self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if (!self.context) {
            // ES 3.0 failed, use ES 2.0
            self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
            mEnabledVersion = RenderFeatureLevel::eGL2;
        }
        else {
            mEnabledVersion = RenderFeatureLevel::eGL3;
        }
    }
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    
    // set render params
    const PlatformCreationParams& params = IPlatform::getSingleton().getCreationParams();
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    if (params.zBits == 16) {
        view.drawableDepthFormat = GLKViewDrawableDepthFormat16;
    }
    else if (params.zBits != 24) {
        Peach3DLog(LogLevel::eWarn, "Depth format %d not support, use default bits 24", params.zBits);
    }
    view.drawableStencilFormat = GLKViewDrawableStencilFormat8;
    if (params.sBits == 0) {
        view.drawableStencilFormat = GLKViewDrawableStencilFormatNone;
    }
    else if (params.sBits != 8) {
        Peach3DLog(LogLevel::eWarn, "Stencil format %d not support, use default bits 8", params.sBits);
    }
    // use msaa
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    if (params.MSAA == 0) {
        view.drawableMultisample = GLKViewDrawableMultisampleNone;
    }
    else {
        Peach3DLog(LogLevel::eWarn, "MSAA level %d not support, enable the default level 4", params.MSAA);
    }
    // enable multi touch
    [view setMultipleTouchEnabled:TRUE];
    
    // set current context
    [EAGLContext setCurrentContext:self.context];
    
    // set frame per second
    [self setPreferredFramesPerSecond:params.maxFPS];
    [self resume];
    return self;
}

- (int)addTouchToList:(UITouch*)touch
{
    uint index = 1;
    while (1) {
        BOOL isIndexExisted = NO;
        for (NSNumber* key in [mTouchList allKeys]) {
            if ([key unsignedIntValue] == index) {
                isIndexExisted = YES;
                break;
            }
        }
        // find valid index
        if (!isIndexExisted) {
            break;
        }
        else {
            index ++;
        }
    }
    // add touch with valid index
    [mTouchList setObject:touch forKey:[NSNumber numberWithUnsignedInt:index]];
    
    return index;
}

- (int)getTouchIndex:(UITouch*)touch
{
    uint index = 1;
    for (NSNumber* key in [mTouchList allKeys]) {
        if ([mTouchList objectForKey:key] == touch) {
            index = [key unsignedIntValue];
            break;
        }
    }
    return index;
}

- (void)deleteTouch:(UITouch*)touch
{
    for (NSNumber* key in [mTouchList allKeys]) {
        if ([mTouchList objectForKey:key] == touch) {
            [mTouchList removeObjectForKey:key];
            break;
        }
    }
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self triggerEvent:touches withType:ClickEvent::eDown];
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self triggerEvent:touches withType:ClickEvent::eDrag];
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self triggerEvent:touches withType:ClickEvent::eUp];
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    [self triggerEvent:touches withType:ClickEvent::eCancel];
}

- (void)triggerEvent:(NSSet *)touches withType:(ClickEvent)type
{
    if ([touches count] < 1) return;
    
    const float screenScale = self.view.contentScaleFactor;
    const float screenHeight = LayoutManager::getSingleton().getScreenSize().y;
    std::vector<uint> clickIds;
    std::vector<Vector2> poss;
    for (UITouch* touch in touches)
    {
        // get current position and trigger event
        if (type == ClickEvent::eDown) {
            uint index = [self addTouchToList:touch];
            clickIds.push_back(index);
        }
        else {
            uint index = [self getTouchIndex:touch];
            clickIds.push_back(index);
        }
        CGPoint point = [touch locationInView:[self view]];
        poss.push_back(Vector2(point.x*screenScale, screenHeight - point.y*screenScale));
        
        // remove saved touch handler
        if (type == ClickEvent::eUp || type == ClickEvent::eCancel) {
            [self deleteTouch:touch];
        }
    }
    EventDispatcher::getSingletonPtr()->triggerClickEvent(type, clickIds, poss);
}

- (void)dealloc
{
    // release context
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (RenderFeatureLevel)getOpenGLESVersion
{
    return mEnabledVersion;
}

- (void)update
{
    if (mLastTime > FLT_EPSILON) {
        // render one frame
        IPlatform* platform = IPlatform::getSingletonPtr();
        platform->renderOneFrame(self.timeSinceLastUpdate);
    }
    mLastTime = self.timeSinceLastUpdate;
}

- (void)pause
{
}

- (void)resume
{
    // reset last draw time
    mLastTime = 0.0f;
}

// For ios6, use supportedInterfaceOrientations & shouldAutorotate instead
- (NSUInteger) supportedInterfaceOrientations{
    UIInterfaceOrientation orientation = [UIApplication sharedApplication].statusBarOrientation;
    if (UIInterfaceOrientationIsLandscape(orientation)) {
        return UIInterfaceOrientationMaskLandscape;
    }
    else {
        return UIInterfaceOrientationMaskPortrait;
    }
}

- (BOOL) shouldAutorotate
{
    return YES;
}

//fix not hide status on ios7
- (BOOL)prefersStatusBarHidden
{
    return YES;
}

@end
