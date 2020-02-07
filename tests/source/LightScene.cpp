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
    mSampleList.push_back([]()->BaseSample* {return new LightTextureSample();});
    mSampleList.push_back([]()->BaseSample* {return new MultiLightsSample();});
    mSampleList.push_back([]()->BaseSample* {return new LightControlSample();});
    
    // set ortho projective
    mSysPMat = SceneManager::getSingleton().getProjectionMatrix();
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
    BaseScene::exit();
    // reset projective
    SceneManager::getSingleton().setProjectionMatrix(mSysPMat);
}

void DirLightSample::init(Widget* parentWidget)
{
    // add a direction light
    mDirLight = SceneManager::getSingleton().addNewLight();
    mDirLight->usingAsDirection(Vector3(0.f, -1.f, -1.f));
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
    mDirLight->setColor(Color3(curItem));
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
    mDotLight = SceneManager::getSingleton().addNewLight();
    mDotLight->usingAsDot(initPos);
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
    auto lastPos = mDotLight->getPosition();
    static float curItem = lastPos.x;
    static bool isPlus = true;
    curItem += lastFrameTime * (isPlus ? 3.f : -3.f);
    if (curItem > rang || curItem < -rang) {
        isPlus = !isPlus;
        curItem = (curItem > rang) ? rang : curItem;
        curItem = (curItem < -rang) ? -rang : curItem;
    }
    lastPos.x = curItem;
    mDotLight->setPosition(lastPos);
    // also update light node position
    mDotNode->setPosition(lastPos);
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
    mSpotLight = SceneManager::getSingleton().addNewLight();
    mSpotLight->usingAsSpot(Vector3(0.f, 0.f, 6.f), lightDir);
    // only set title here
    mTitle = "Spot Light";
    
    // create spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    for (auto i=0; i<7; ++i) {
        float compareX = i * 4.1f - 7.f;
        for (auto j=0; j<5; ++j) {
            auto sphereNode = rootNode->createChild(Vector3(compareX, (j - 2) * 4.1f, 0.f));
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
    auto lightDir = Vector3(curItem, mLockPos.y, mLockPos.z) - mSpotLight->getPosition();
    lightDir.normalize();
    mSpotLight->setDirection(lightDir);
}

SpotLightSample::~SpotLightSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}

void LightTextureSample::init(Widget* parentWidget)
{
    // add a direction light
    auto dirLight = SceneManager::getSingleton().addNewLight();
    dirLight->usingAsDirection(Vector3(0.f, -1.f, -1.f), Color3(0.f, 0.5f, 0.f));
    // set title and desc
    mTitle = "Lighting Texture";
    mDesc = "Texture mesh with direction light";
    
    // create peach
    auto peachMesh = ResourceManager::getSingleton().addMesh("peach.pmt");
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    for (auto i=0; i<5; ++i) {
        float compareX = i * 5.1f - 7.f;
        for (auto j=0; j<4; ++j) {
            auto peachNode = rootNode->createChild(Vector3(compareX, (j - 2) * 5.1f, 0.f));
            peachNode->attachMesh(peachMesh);
            peachNode->setLightingEnabled(j > 0);
            peachNode->runAction(Repeat::createForever(RotateBy3D::create(Vector3(0.f, PD_PI, 0.f), 5.f)));
        }
    }
}

LightTextureSample::~LightTextureSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}

void MultiLightsSample::init(Widget* parentWidget)
{
    // add a direction light
    auto dirLight = SceneManager::getSingleton().addNewLight();
    dirLight->usingAsDirection(Vector3(0.f, -1.f, -1.f), Color3(0.5f, 0.f, 0.f));
    
    // add a dot light
    Vector3 initPos(-15.f, 0.f, 6.f);
    mDotLight = SceneManager::getSingleton().addNewLight();
    mDotLight->usingAsDot(initPos);
    mDotLight->setColor(Color3(0.f, 0.5f, 0.f));
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
    mSpotLight = SceneManager::getSingleton().addNewLight();
    mSpotLight->usingAsSpot(Vector3(0.f, 0.f, 6.f), lightDir);
    mSpotLight->setColor(Color3(0.f, 0.f, 0.5f));
    // only set title here
    mTitle = "Multi Lights";
    mDesc = "Red direction light, green dot light and blue spot light";
    
    // create spheres
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    for (auto i=0; i<7; ++i) {
        float compareX = i * 4.1f - 7.f;
        for (auto j=0; j<5; ++j) {
            auto sphereNode = rootNode->createChild(Vector3(compareX, (j - 2) * 4.1f, 0.f));
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
    auto dotPos = mDotLight->getPosition();
    mDotLight->setPosition(Vector3(curItem, dotPos.y, dotPos.z));
    // also update light node position
    mDotNode->setPosition(dotPos);
    // reset spot light direction
    auto lightDir = Vector3(-curItem, mLockPos.y, mLockPos.z) - mSpotLight->getPosition();
    lightDir.normalize();
    mSpotLight->setDirection(lightDir);
}

MultiLightsSample::~MultiLightsSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}

void LightControlSample::init(Widget* parentWidget)
{
    mControlNode = nullptr;
    // add a direction light
    auto dirLight = SceneManager::getSingleton().addNewLight();
    dirLight->usingAsDirection(Vector3(0.f, -1.f, -1.f), Color3(0.2f, 0.2f, 0.f));
    // add a dot light
    Vector3 initPos(5.f, 0.f, 6.f);
    mDotLight = SceneManager::getSingleton().addNewLight();
    mDotLight->usingAsDot(initPos);
    mDotLight->setColor(Color3(0.f, 0.5f, 0.5f));
    // create open/close button
    const Vector2&  screenSize  = LayoutManager::getSingleton().getScreenSize();
    Button* lcButton = Button::create("press_normal.png");
    lcButton->setPosition(Vector2(100.f, screenSize.y / 3.0f));
    lcButton->setTitleText("Close");
    lcButton->setClickedAction([&, lcButton](const Vector2&){
        auto enable = mDotLight->isEnabled();
        enable = !enable;
        mDotLight->setEnabled(enable);
        lcButton->setTitleText(enable ? "Close" : "Open");
    });
    parentWidget->addChild(lcButton);
    // create enable/disable button
    Button* ncButton = Button::create("press_normal.png");
    ncButton->setPosition(Vector2(100.f, screenSize.y * 2 / 3.0f));
    ncButton->setTitleText("Enable");
    ncButton->setClickedAction([&, ncButton](const Vector2&){
        auto enable = mControlNode->isLightingEnabled();
        enable = !enable;
        ncButton->setTitleText(enable ? "Disable" : "Enable");
        if (mControlNode) {
            mControlNode->setLightingEnabled(enable);
        }
    });
    parentWidget->addChild(ncButton);
    // only set title here
    mTitle = "Light control";
    mDesc = "use button to open/close dot light and  enable/disable node lighting";
    
    // create spheres
    auto rootNode = SceneManager::getSingleton().getRootSceneNode();
    auto sphereMesh = ResourceManager::getSingleton().addMesh("sphere.pmt");
    for (auto i=0; i<7; ++i) {
        float compareX = i * 4.1f - 7.f;
        for (auto j=0; j<5; ++j) {
            auto sphereNode = rootNode->createChild(Vector3(compareX, (j - 2) * 4.1f, 0.f));
            sphereNode->attachMesh(sphereMesh);
            sphereNode->setLightingEnabled(j != 4);
            if (i == 6 && j == 4) {
                mControlNode = sphereNode;
            }
        }
    }
}

LightControlSample::~LightControlSample()
{
    // delete all lights
    SceneManager::getSingleton().deleteAllLights();
}
