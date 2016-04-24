//
//  AppDelegate.m
//  test
//
//  Created by singoon.he on 9/1/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#import "AppDelegate.h"
#import "PlatformDelegate.h"
#import "MAC/Peach3DPlatformMAC.h"

Peach3D::PlatformMAC gPlatform;
PlatformDelegate      gGameDelegate;
@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // init window
    NSSize winSize = NSMakeSize(800, 600);
    NSInteger styleMask = NSResizableWindowMask|NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask;
    self.window = [[EAGLViewWindow alloc] initWithContentRect:NSMakeRect(0, 0, winSize.width, winSize.height)
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    [self.window setDelegate:self];
    
    // init mac platform
    Peach3D::PlatformCreationParams params;
    params.winSize = Peach3D::Vector2(winSize.width, winSize.height);
    //params.MSAA = 1;
    params.logPath = "pdtestlog.txt";
    params.delegate = &gGameDelegate;
    gPlatform.initWithParams(params);
    [self.window setContentView:(__bridge NSOpenGLView*)gPlatform.getCreationParams().window];
    
    // set window delegate, windowShouldClose will be called
    [self.window setDelegate:self];
    // show window
    [self.window makeKeyAndOrderFront:self];
    [self.window center];
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // exit game engine here
    gPlatform.terminate();
}

- (BOOL)windowShouldClose:(id)sender
{
    // Insert code here to tear down your application
    Peach3D::EventDispatcher::getSingleton().triggerKeyboardEvent(Peach3D::KeyboardEvent::eKeyDown, Peach3D::KeyCode::eESC);
    return NO;
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{
    return YES;
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
    gGameDelegate.appDidEnterBackground();
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    // There is not willDeminiaturize, use didDeminiaturize is all right
    gGameDelegate.appWillEnterForeground();
}

- (void)windowDidEndLiveResize:(NSNotification *)notification
{
    NSSize contentSize = [[_window contentView] frame].size;
    gGameDelegate.appWindowSizeChanged(Peach3D::Vector2(contentSize.width, contentSize.height));
}

@end
