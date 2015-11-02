//
//  Peach3DGLViewController.h
//  Peach3DLib
//
//  Created by singoon.he on 9/15/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "Peach3DIPlatform.h"

@interface EAGLViewController : GLKViewController
{
    Peach3D::RenderFeatureLevel mEnabledVersion;
    CFTimeInterval              mLastTime;
    NSMutableDictionary*        mTouchList;
}
@property (strong, nonatomic) EAGLContext *context;

// return used opengl es version, 2.0 or 3.0
- (Peach3D::RenderFeatureLevel)getOpenGLESVersion;

- (void)pause;
- (void)resume;
@end
