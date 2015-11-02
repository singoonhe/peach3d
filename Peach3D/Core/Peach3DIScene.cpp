//
//  Peach3DIScene.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DIScene.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    IScene::~IScene()
    {
        // delete self keyboard event listener
        EventDispatcher::getSingletonPtr()->deleteKeyboardListener(this);
    }
}