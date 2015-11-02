//
//  main.m
//  meshviewer
//
//  Created by singoon.he on 1/22/15.
//  Copyright (c) 2015 singoon. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[])
{
    AppDelegate *appDelegate = [[AppDelegate alloc] init];
    [[NSApplication sharedApplication] setDelegate:appDelegate];
    return NSApplicationMain(argc, argv);
}
