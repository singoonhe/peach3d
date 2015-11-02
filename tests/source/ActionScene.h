//
//  ActionScene.h
//  test
//
//  Created by singoon.he on 8/5/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef ACTION_SCENE_H
#define ACTION_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class ActionScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class SingleSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class SequenceSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class CallFuncSample : public BaseSample
{
    void init(Widget* parentWidget);
};

#endif /* defined(ACTION_SCENE_H) */
