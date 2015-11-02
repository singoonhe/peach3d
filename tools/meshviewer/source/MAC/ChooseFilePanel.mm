//
//  ChooseFilePanel.cpp
//  meshviewer
//
//  Created by singoon.he on 1/30/15.
//  Copyright (c) 2015 singoon. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#include "ChooseFilePanel.h"

void openChooseFilePannel(const std::vector<std::string>& extList, std::function<void(const char* file)> chooseCallFunc)
{
    NSMutableArray* fileTypes = [[NSMutableArray alloc] init];
    for (auto ext : extList) {
        [fileTypes addObject: [NSString stringWithUTF8String:ext.c_str()]];
    }
    if ([fileTypes count] > 0) {
        // open panel
        NSOpenPanel* openDlg = [NSOpenPanel openPanel];
        [openDlg setCanChooseFiles:YES];
        [openDlg setCanChooseDirectories:NO];
        [openDlg setAllowsMultipleSelection:NO];
        [openDlg setPrompt:@"Select"];
        [openDlg setAllowedFileTypes:fileTypes];
        if ( [openDlg runModal] == NSModalResponseOK )
        {
            NSArray* files = [openDlg URLs];
            if ([files count] > 0) {
                NSURL* fileName = [files objectAtIndex:0];
                chooseCallFunc([[fileName path] UTF8String]);
            }
        }
    }
}