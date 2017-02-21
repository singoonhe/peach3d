//
//  TerrainScene.h
//  test
//
//  Created by singoon.he on 20/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#ifndef PARTICLE_TERRAIN_H
#define PARTICLE_TERRAIN_H

#include "BaseScene.h"

using namespace Peach3D;
class TerrainScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class TerrainSample : public BaseSample
{
    void init(Widget* parentWidget);
};

#endif /* PARTICLE_TERRAIN_H */
