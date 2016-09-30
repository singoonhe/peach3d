//
//  ParticleScene.h
//  test
//
//  Created by singoon.he on 9/29/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PARTICLE_SCENE_H
#define PARTICLE_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class ParticleScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class Particle2DSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class Particle3DSample : public BaseSample
{
    void init(Widget* parentWidget);
};

#endif /* PARTICLE_SCENE_H */
