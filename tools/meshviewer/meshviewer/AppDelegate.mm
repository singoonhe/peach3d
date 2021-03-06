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
    params.width = winSize.width;
    params.height = winSize.height;
    params.MSAA = 1;
    params.delegate = &gGameDelegate;
    gPlatform.initWithParams(params);
    [self.window setContentView:(__bridge NSOpenGLView*)gPlatform.getCreationParams().window];
    
    // show window
    [self.window makeKeyAndOrderFront:self];
    [self.window center];
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

@end
