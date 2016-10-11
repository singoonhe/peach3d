//
//  DollScene.h
//  test
//
//  Created by singoon.he on 3/23/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef DOLL_SCENE_H
#define DOLL_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class DollScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class DollSample : public BaseSample
{
    void init(Widget* parentWidget);
    
private:
    ObjectPtr   mHeadObj;
    ObjectPtr   mHead1Obj;
    SceneNode*  mGirlNode;
    SceneNode*  mWeaponNode;
};

#endif /* DOLL_SCENE_H */
