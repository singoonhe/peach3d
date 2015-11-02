//
//  TestScene.h
//  test
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "Peach3DIScene.h"
#include "Peach3D.h"

using namespace Peach3D;
class TestScene : public Peach3D::IScene
{
public:
    //! add a cube scene node for test
    bool init();
    //! update cube rotation
    void update(float lastFrameTime);
    
private:
};

#endif // TEST_SCENE_H
