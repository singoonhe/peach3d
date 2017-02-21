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
    auto terr = PttLoader::pttTerrainParse("terrain1.ptt");
    SceneManager::getSingleton().addTerrain(terr);
    
    auto winSize = LayoutManager::getSingleton().getScreenSize();
}
