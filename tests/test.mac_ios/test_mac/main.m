//
//  main.m
//  test
//
//  Created by singoon.he on 9/1/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[])
{
    AppDelegate *appDelegate = [[AppDelegate alloc] init];
    [[NSApplication sharedApplication] setDelegate:appDelegate];
    return NSApplicationMain(argc, argv);
}
