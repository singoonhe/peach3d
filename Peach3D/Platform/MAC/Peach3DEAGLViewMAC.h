//
//  Peach3DEAGLViewMAC.h
//  Peach3DLib
//
//  Created by singoon.he on 9/3/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_EAGLVIEWMAC_H
#define PEACH3D_EAGLVIEWMAC_H

#import <AppKit/NSOpenGLView.h>

@interface EAGLViewMAC : NSOpenGLView
{
    double              mLastFrameTime;
    CVDisplayLinkRef    mDisplayLink;
}

- (void)pause;
- (void)resume;

@end

@interface EAGLViewWindow : NSWindow
{
}

@end

#endif // PEACH3D_EAGLVIEWMAC_H
