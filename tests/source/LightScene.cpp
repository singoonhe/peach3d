//
//  LightScene.cpp
//  test
//
//  Created by singoon.he on 3/8/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "LightScene.h"

bool LightScene::init()
{
    // init sample list
    mSampleList.push_back([]()->BaseSample* {return new LightSample();});
    
    // init base scene
    BaseScene::init();
    return true;
}

void LightSample::init(Widget* parentWidget)
{
    const float fixWidth = 40.f;
    // set ortho projective
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    float calcHeight = screenSize.y * fixWidth / screenSize.x;
    SceneManager::getSingleton().setOrthoProjection(- fixWidth / 2.f, fixWidth / 2.f, - calcHeight / 2.f, calcHeight / 2.f, -10, 10);
    // add a direction light
    mDirLight = Light(Vector3(0.f, -1.f, -1.f), Color3Gray, Color3Gray);
    SceneManager::getSingleton().addNewLight(mDirLight);
    // only set title here
    mTitle = "Direction Light";
    mDesc = "*.pmt using vertex normal, *.obj using face normal";
    
    // create notice label
    std::string noticeList[] = {"Specular\n+Shininess", "Diffuse", "Material Ambient\n+Light Ambient"};
    auto rootWidget = SceneManager::getSingleton().getRootWidget();
    for (auto i=0; i<3; ++i) {
        Label* noticeLabel = Label::create(noticeList[i], 20 * LayoutManager::getSingleton().getMinScale());
        noticeLabel->setPosition(Vector2(10.f, i * 105.f + 195.f));
        noticeLabel->setAnchorPoint(Vector2(0.f, 0.5f));
        rootWidget->addChild(noticeLabel);
    }
    
    // create spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    auto objMesh = ResourceManager::getSingleton().addMesh("sphere.obj");
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    for (auto i=0; i<4; ++i) {
        float compareX = i * 7.f - 7.f;
        // add ambient spheres
        auto ambientNode = rootNode->createChild(Vector3(compareX, 5.f, 0.f));
        ambientNode->attachMesh(sphereMesh);
        auto ambientObj = ambientNode->getRenderNode("Sphere.001");
        if (ambientObj) {
            float item = i * 0.1f + 0.3f;
            ambientObj->setAmbient(Color3(item, item, item));
        }
        // add diffuse spheres
        auto diffuseNode = rootNode->createChild(Vector3(compareX, 0.f, 0.f));
        diffuseNode->attachMesh(sphereMesh);
        auto diffuseObj = diffuseNode->getRenderNode("Sphere.001");
        if (diffuseObj) {
            float item = i * 0.2f + 0.2f;
            diffuseObj->setDiffuse(Color3(item, item, item));
        }
        // add specular spheres
        auto objNode = rootNode->createChild(Vector3(compareX, -5.f, 0.f));
        objNode->attachMesh(objMesh);
        auto specularObj = objNode->getRenderNode("Sphere");
        if (specularObj) {
            float item = i * 0.2f + 0.2f;
            specularObj->setSpecular(Color3(item, item, item));
            specularObj->setShininess(96.0f - 22.f * i);
        }
    }
}

void LightSample::update(float lastFrameTime)
{
    static float curItem = 0.2f;
    static bool isPlus = true;
    curItem += (lastFrameTime / 5.f) * (isPlus ? 1 : -1);
    if (curItem > 0.8f || curItem < 0.2f) {
        isPlus = !isPlus;
        curItem = (curItem > 0.8f) ? 0.8f : curItem;
        curItem = (curItem < 0.f) ? 0.f : curItem;
    }
    mDirLight.ambient = Color3(curItem, curItem, curItem);
    SceneManager::getSingleton().setLight("pd_Light0", mDirLight);
}

LightSample::~LightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
    // reset projective
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    SceneManager::getSingleton().setPerspectiveProjection(90.f, screenSize.x/screenSize.y);
}