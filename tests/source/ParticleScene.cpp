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
    mSampleList.push_back([]()->BaseSample* {return new Particle2DFireSample();});
    mSampleList.push_back([]()->BaseSample* {return new Particle2DCloudSample();});
    mSampleList.push_back([]()->BaseSample* {return new Particle3DSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void Particle2DFireSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "2D fire particle, gravity mode";
    
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

void Particle2DCloudSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "2D cloud particle, radius mode";
    
    // also support plist texture, width and height must be power of 2
    //std::vector<TextureFrame> fileLogoList;
    //Peach3DAssert(ResourceManager::getSingleton().addTextureFrames("peach3d_log.xml", &fileLogoList), "Load frame file failed");
    
    // read frame action from file
    auto winSize = LayoutManager::getSingleton().getScreenSize();
    // create cloud particle from file
    Particle2D* cloud = Particle2D::create("2dcloud.ppt");
    cloud->setPosition(Vector2(winSize.x / 2.f, winSize.y / 2.f));
    // add particle to scene widget (root widget will not clean when changing sample)
    parentWidget->addChild(cloud);
}

void Particle3DSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "3D fire particle, gravity mode";
    
    // also support plist texture, width and height must be power of 2
    //std::vector<TextureFrame> fileLogoList;
    //Peach3DAssert(ResourceManager::getSingleton().addTextureFrames("peach3d_log.xml", &fileLogoList), "Load frame file failed");
    
    auto rootSceneNode = SceneManager::getSingleton().getRootSceneNode();
    // create fire particle from file
    Particle3D* fire = Particle3D::create("3dfire.ppt");
    rootSceneNode->addChild(fire);
    
    // make camera faraway
    auto mainCamera = SceneManager::getSingleton().getActiveCamera();
    mDefaultPos = mainCamera->getPosition();
    mainCamera->setPosition(Vector3(0, 0, 100));
}

Particle3DSample::~Particle3DSample()
{
    // restore camera pos
    auto mainCamera = SceneManager::getSingleton().getActiveCamera();
    mainCamera->setPosition(mDefaultPos);
}
