//
//  CostumeScene.h
//  test
//
//  Created by singoon.he on 3/23/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef COSTUME_SCENE_H
#define COSTUME_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class CostumeScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class CostumeSample : public BaseSample
{
    void init(Widget* parentWidget);
};

#endif /* COSTUME_SCENE_H */
