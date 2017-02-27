//
//  TerrainScene.cpp
//  test
//
//  Created by singoon.he on 20/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "TerrainScene.h"

bool TerrainScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new TerrainSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void TerrainSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "work on terrain";
    
    // create terrain
    auto terr = PttLoader::pttTerrainParse("terr_walk.ptt");
    SceneManager::getSingleton().addTerrain(terr);
    
    // transform camera pos
    auto mainCamera = SceneManager::getSingleton().getActiveCamera();
    mainCamera->setPosition(Vector3(0.f, 2.f, -1.f));
    
    auto winSize = LayoutManager::getSingleton().getScreenSize();
}
