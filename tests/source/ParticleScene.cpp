//
//  ParticleScene.cpp
//  test
//
//  Created by singoon.he on 9/29/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "ParticleScene.h"
#include "Peach3DParticle.h"

bool ParticleScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new Particle2DSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void Particle2DSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "2D fire particle";
    
    // also support plist texture, width and height must be power of 2
    //std::vector<TextureFrame> fileLogoList;
    //Peach3DAssert(ResourceManager::getSingleton().addTextureFrames("peach3d_log.xml", &fileLogoList), "Load frame file failed");
    
    // read frame action from file
    auto winSize = LayoutManager::getSingleton().getScreenSize();
    // create fire particle from file
    Particle2D* fire = Particle2D::create("2dfire.ppt");
    fire->setPosition(Vector2(winSize.x / 2.f, winSize.y / 4.f));
    // add particle to scene widget (root widget will not clean when changing sample)
    parentWidget->addChild(fire);
}

void Particle3DSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "3D particle";
    //mDesc = "";
}
