//
//  PlatformDelegate.cpp
//  test
//
//  Created by singoon.he on 9/5/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3D.h"
#include "MeshScene.h"
#include "PlatformDelegate.h"

using namespace Peach3D;

bool PlatformDelegate::appDidFinishLaunching()
{
    // show debug draw stats
    IPlatform::getSingleton().setIsDrawStats(true);
    // start with TestScene
    IPlatform::getSingleton().replaceWithNewScene(new MeshScene());
    
    return true;
}

void PlatformDelegate::appDidEnterBackground()
{
    // pause platform render loop
    IPlatform::getSingletonPtr()->pauseAnimating();
}

void PlatformDelegate::appWillEnterForeground()
{
    // resume platform render loop
    IPlatform::getSingletonPtr()->resumeAnimating();
}