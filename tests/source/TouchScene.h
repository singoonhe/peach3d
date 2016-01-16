//
//  TouchScene.h
//  test
//
//  Created by singoon.he on 1/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef TOUCH_SCENE_H
#define TOUCH_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class TouchScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class PickSample : public BaseSample
{
    void init(Widget* parentWidget);
    
private:
    SceneNode*  mClickedNode;
};

class SNEventSample : public BaseSample
{
    void init(Widget* parentWidget);
    
private:
    SceneNode*  mClickedNode;
};

#endif /* TOUCH_SCENE_H */
