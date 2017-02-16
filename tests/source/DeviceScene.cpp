//
//  DeviceScene.cpp
//  test
//
//  Created by singoon.he on 1/26/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "DeviceScene.h"

bool DeviceScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new DeviceSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void DeviceSample::init(Widget* parentWidget)
{
    // only set title here
    mTitle = "Some device info";
    
    auto platform = IPlatform::getSingletonPtr();
    std::vector<std::string> textList;
    textList.push_back(Utils::formatString("Engine version : %s", PEACH3D_SDK_VERSION));
    textList.push_back(Utils::formatString("Local langague : %s", IPlatform::LanguageTypeNames[platform->getLocalLanguage()].c_str()));
    textList.push_back(Utils::formatString("Device name : %s", platform->getDeviceName().c_str()));
    textList.push_back(Utils::formatString("OS version : %s", platform->getOSVersionString().c_str()));
    textList.push_back(Utils::formatString("Video card : %s", IRender::getSingleton().getVideoCard().c_str()));
    std::string rlName = "";
    auto rlType = platform->getRenderFeatureLevel();
    if (rlType == RenderFeatureLevel::eGL3) {
        rlName = "OpenGL 3+ or OpenGL ES 3+";
    }
    else if (rlType == RenderFeatureLevel::eGL2) {
        rlName = "OpenGL 2+ or OpenGL ES 2+";
    }
    else if (rlType == RenderFeatureLevel::eDX) {
        rlName = "DirectX 12";
    }
    textList.push_back(Utils::formatString("Render level : %s", rlName.c_str()));
    
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    float minScale = LayoutManager::getSingleton().getMinScale();
    int index = 0;
    for (auto iter=textList.rbegin(); iter!=textList.rend(); ++iter) {
        // create desc label
        auto textLabel = Label::create(*iter, 24.0f * minScale);
        textLabel->setPosition(Vector2(screenSize.x / 2.0f, screenSize.y / 2.0f + 30.f * minScale * (index - textList.size() / 2.f)));
        parentWidget->addChild(textLabel);
        index++;
    }
}
