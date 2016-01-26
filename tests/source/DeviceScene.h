//
//  DeviceScene.h
//  test
//
//  Created by singoon.he on 1/26/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef DEVICE_SCENE_H
#define DEVICE_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class DeviceScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class DeviceSample : public BaseSample
{
    void init(Widget* parentWidget);
};

#endif /* DEVICE_SCENE_H */
