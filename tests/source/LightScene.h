//
//  LightScene.h
//  test
//
//  Created by singoon.he on 3/8/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef LIGHT_SCENE_H
#define LIGHT_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class LightScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class LightSample : public BaseSample
{
    void init(Widget* parentWidget);
    void update(float lastFrameTime);
    virtual ~LightSample();
    
private:
    Light   mDirLight;
};

#endif /* LIGHT_SCENE_H */
