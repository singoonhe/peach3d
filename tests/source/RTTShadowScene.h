//
//  RTTShadowScene.h
//  test
//
//  Created by singoon.he on 3/29/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef RTTSHADOW_SCENE_H
#define RTTSHADOW_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class RTTShadowScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class RTTSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class ShadowSample : public BaseSample
{
    void init(Widget* parentWidget);
    void update(float lastFrameTime);
    virtual ~ShadowSample();
    
private:
    LightPtr    mDotLight;
    SceneNode*  mDotNode;
    float       mRotate;
};

#endif /* RTTSHADOW_SCENE_H */
