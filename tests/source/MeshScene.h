//
//  MeshScene.h
//  test
//
//  Created by singoon.he on 1/4/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef MESH_SCENE_H
#define MESH_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class MeshScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class ObjSample : public BaseSample
{
    void init(Widget* parentWidget);
};

#endif /* defined(MESH_SCENE_H) */
