//
//  PlatformDelegate.cpp
//  test
//
//  Created by singoon.he on 9/5/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3D.h"
#include "TestScene.h"
#include "BaseScene.h"
#include "PlatformDelegate.h"

using namespace Peach3D;

bool PlatformDelegate::appDidFinishLaunching()
{
    // show drawing stats, only valid on debug
    IPlatform::getSingleton().setIsDrawStats(true);

    // start with TestScene
    IPlatform::getSingleton().replaceWithNewScene(new TestScene());
    
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