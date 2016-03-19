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
    mSampleList.push_back([]()->BaseSample* {return new DirLightSample();});
    mSampleList.push_back([]()->BaseSample* {return new DotLightSample();});
    mSampleList.push_back([]()->BaseSample* {return new SpotLightSample();});
    mSampleList.push_back([]()->BaseSample* {return new MultiLightsSample();});
    
    // set ortho projective
    const float fixWidth = 40.f;
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    float calcHeight = screenSize.y * fixWidth / screenSize.x;
    SceneManager::getSingleton().setOrthoProjection(- fixWidth / 2.f, fixWidth / 2.f, - calcHeight / 2.f, calcHeight / 2.f, -10, 10);
    
    // init base scene
    BaseScene::init();
    return true;
}

void LightScene::exit()
{
    // reset projective
    auto screenSize = LayoutManager::getSingleton().getScreenSize();
    SceneManager::getSingleton().setPerspectiveProjection(90.f, screenSize.x/screenSize.y);
}

void DirLightSample::init(Widget* parentWidget)
{
    // add a direction light
    mDirLight = Light(Vector3(0.f, -1.f, -1.f), Color3Gray, Color3Gray);
    SceneManager::getSingleton().addNewLight(mDirLight);
    // only set title here
    mTitle = "Direction Light";
    mDesc = "*.pmt using vertex normal, *.obj using face normal";
    
    // create notice label
    std::string noticeList[] = {"Specular\n+Shininess", "Diffuse", "Material Ambient\n+Light Ambient"};
    for (auto i=0; i<3; ++i) {
        Label* noticeLabel = Label::create(noticeList[i], 20 * LayoutManager::getSingleton().getMinScale());
        noticeLabel->setPosition(Vector2(10.f, i * 105.f + 195.f));
        noticeLabel->setAnchorPoint(Vector2(0.f, 0.5f));
        parentWidget->addChild(noticeLabel);
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
            float item = i * 0.15f + 0.3f;
            ambientObj->setAmbient(Color3(item));
        }
        // add diffuse spheres
        auto diffuseNode = rootNode->createChild(Vector3(compareX, 0.f, 0.f));
        diffuseNode->attachMesh(sphereMesh);
        auto diffuseObj = diffuseNode->getRenderNode("Sphere.001");
        if (diffuseObj) {
            float item = i * 0.2f + 0.2f;
            diffuseObj->setDiffuse(Color3(item));
        }
        // add specular spheres
        auto objNode = rootNode->createChild(Vector3(compareX, -5.f, 0.f));
        objNode->attachMesh(objMesh);
        auto specularObj = objNode->getRenderNode("Sphere");
        if (specularObj) {
            float item = i * 0.2f + 0.2f;
            specularObj->setSpecular(Color3(item));
            specularObj->setShininess(96.0f - 22.f * i);
        }
    }
}

void DirLightSample::update(float lastFrameTime)
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
    SceneManager::getSingleton().resetLight("pd_Light0", mDirLight);
}

DirLightSample::~DirLightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}

void DotLightSample::init(Widget* parentWidget)
{
    // add a dot light
    Vector3 initPos(-15.f, 0.f, 6.f);
    mDotLight = Light(initPos, Vector3(1.f, 0.1f, 0.f));
    mDotLight.name = "DotLight";
    SceneManager::getSingleton().addNewLight(mDotLight);
    // add light node
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto cubeMesh = ResourceManager::getSingleton().addMesh("peach.obj");
    mDotNode = rootNode->createChild(initPos);
    mDotNode->attachMesh(cubeMesh);
    mDotNode->setScale(Vector3(0.25f));
    mDotNode->setLightingEnabled(false);
    /*auto activeCamera = SceneManager::getSingleton().getActiveCamera();
    activeCamera->setPosition(Vector3(0.f, 1.f, 1.f));
    activeCamera->lockToPosition(Vector3Zero);*/
    // only set title here
    mTitle = "Dot Light";
    
    // create notice label
    std::string noticeList[] = {"Specular\n+Shininess", "Diffuse", "Material Ambient"};
    for (auto i=0; i<3; ++i) {
        Label* noticeLabel = Label::create(noticeList[i], 20 * LayoutManager::getSingleton().getMinScale());
        noticeLabel->setPosition(Vector2(10.f, i * 105.f + 195.f));
        noticeLabel->setAnchorPoint(Vector2(0.f, 0.5f));
        parentWidget->addChild(noticeLabel);
    }
    
    // create spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    for (auto i=0; i<4; ++i) {
        float compareX = i * 7.f - 7.f;
        // add ambient spheres
        auto ambientNode = rootNode->createChild(Vector3(compareX, 5.f, 0.f));
        ambientNode->attachMesh(sphereMesh);
        auto ambientObj = ambientNode->getRenderNode("Sphere.001");
        if (ambientObj) {
            float item = i * 0.15f + 0.3f;
            ambientObj->setAmbient(Color3(item));
        }
        // add diffuse spheres
        auto diffuseNode = rootNode->createChild(Vector3(compareX, 0.f, 0.f));
        diffuseNode->attachMesh(sphereMesh);
        auto diffuseObj = diffuseNode->getRenderNode("Sphere.001");
        if (diffuseObj) {
            float item = i * 0.2f + 0.2f;
            diffuseObj->setDiffuse(Color3(item));
        }
        // add specular spheres
        auto specularNode = rootNode->createChild(Vector3(compareX, -5.f, 0.f));
        specularNode->attachMesh(sphereMesh);
        auto specularObj = diffuseNode->getRenderNode("Sphere.001");
        if (specularObj) {
            float item = i * 0.2f + 0.2f;
            specularObj->setSpecular(Color3(item));
            specularObj->setShininess(96.0f - 22.f * i);
        }
    }
}

void DotLightSample::update(float lastFrameTime)
{
    const float rang = 15.f;
    static float curItem = mDotLight.pos.x;
    static bool isPlus = true;
    curItem += lastFrameTime * (isPlus ? 3.f : -3.f);
    if (curItem > rang || curItem < -rang) {
        isPlus = !isPlus;
        curItem = (curItem > rang) ? rang : curItem;
        curItem = (curItem < -rang) ? -rang : curItem;
    }
    mDotLight.pos.x = curItem;
    SceneManager::getSingleton().resetLight("DotLight", mDotLight);
    // also update light node position
    mDotNode->setPosition(mDotLight.pos);
}

DotLightSample::~DotLightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}

void SpotLightSample::init(Widget* parentWidget)
{
    // add a spot light, pos/dir/attenuate
    mSpotPos = Vector3(0.f, 0.f, 60.f);
    mLockPos = Vector3(5.f, 0.f, 0.f);
    auto lightDir = mLockPos - mSpotPos;
    mSpotLight = Light(Vector3(0.f, 0.f, 6.f), lightDir, Vector3(1.f, 0.1f, 0.f));
    mSpotLight.name = "SpotLight";
    SceneManager::getSingleton().addNewLight(mSpotLight);
    // only set title here
    mTitle = "Spot Light";
    
    // create spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    for (auto i=0; i<7; ++i) {
        float compareX = i * 3.5f - 7.f;
        for (auto j=0; j<5; ++j) {
            auto sphereNode = rootNode->createChild(Vector3(compareX, (j - 2) * 3.f, 0.f));
            sphereNode->attachMesh(sphereMesh);
        }
    }
}

void SpotLightSample::update(float lastFrameTime)
{
    const float rang = 15.f;
    static float curItem = mLockPos.x;
    static bool isPlus = true;
    curItem += lastFrameTime * (isPlus ? 3.f : -3.f);
    if (curItem > rang || curItem < -rang) {
        isPlus = !isPlus;
        curItem = (curItem > rang) ? rang : curItem;
        curItem = (curItem < -rang) ? -rang : curItem;
    }
    auto lightDir = Vector3(curItem, mLockPos.y, mLockPos.z) - mSpotLight.pos;
    lightDir.normalize();
    mSpotLight.dir = lightDir;
    SceneManager::getSingleton().resetLight("SpotLight", mSpotLight);
}

SpotLightSample::~SpotLightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}

void MultiLightsSample::init(Widget* parentWidget)
{
    // add a direction light
    Light dirLight(Vector3(0.f, -1.f, -1.f), Color3(0.5f, 0.f, 0.f), Color3Gray);
    SceneManager::getSingleton().addNewLight(dirLight);
    
    // add a dot light
    Vector3 initPos(-15.f, 0.f, 6.f);
    mDotLight = Light(initPos, Vector3(1.f, 0.1f, 0.f));
    mDotLight.name = "DotLight";
    mDotLight.color = Color3(0.f, 0.5f, 0.f);
    SceneManager::getSingleton().addNewLight(mDotLight);
    // add light node
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto cubeMesh = ResourceManager::getSingleton().addMesh("peach.obj");
    mDotNode = rootNode->createChild(initPos);
    mDotNode->attachMesh(cubeMesh);
    mDotNode->setScale(Vector3(0.25f));
    mDotNode->setLightingEnabled(false);

    // add a spot light, pos/dir/attenuate
    mSpotPos = Vector3(0.f, 0.f, 6.f);
    mLockPos = Vector3(5.f, 0.f, 0.f);
    auto lightDir = mLockPos - mSpotPos;
    mSpotLight = Light(Vector3(0.f, 0.f, 6.f), lightDir, Vector3(1.f, 0.1f, 0.f));
    mSpotLight.name = "SpotLight";
    mSpotLight.color = Color3(0.f, 0.f, 0.5f);
    SceneManager::getSingleton().addNewLight(mSpotLight);
    // only set title here
    mTitle = "Multi Lights";
    
    // create spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    for (auto i=0; i<7; ++i) {
        float compareX = i * 3.5f - 7.f;
        for (auto j=0; j<5; ++j) {
            auto sphereNode = rootNode->createChild(Vector3(compareX, (j - 2) * 3.f, 0.f));
            sphereNode->attachMesh(sphereMesh);
        }
    }
}

void MultiLightsSample::update(float lastFrameTime)
{
    const float rang = 15.f;
    static float curItem = 0.f;
    static bool isPlus = true;
    curItem += lastFrameTime * (isPlus ? 3.f : -3.f);
    if (curItem > rang || curItem < -rang) {
        isPlus = !isPlus;
        curItem = (curItem > rang) ? rang : curItem;
        curItem = (curItem < -rang) ? -rang : curItem;
    }
    // reset dot light position
    mDotLight.pos.x = curItem;
    SceneManager::getSingleton().resetLight("DotLight", mDotLight);
    // also update light node position
    mDotNode->setPosition(mDotLight.pos);
    // reset spot light direction
    auto lightDir = Vector3(-curItem, mLockPos.y, mLockPos.z) - mSpotLight.pos;
    lightDir.normalize();
    mSpotLight.dir = lightDir;
    SceneManager::getSingleton().resetLight("SpotLight", mSpotLight);
}

MultiLightsSample::~MultiLightsSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}