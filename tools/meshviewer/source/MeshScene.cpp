//
//  MeshScene.cpp
//  test
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "MeshScene.h"
#if PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX11
extern void openChooseFilePannel(const std::vector<std::string>& extList, std::function<void(const char* file)> chooseCallFunc);
#else
#include "ChooseFilePanel.h"
#endif

bool MeshScene::init()
{
    IPlatform::getSingleton().getRender()->setRenderClearColor(Color4(0.5f, 0.5f, 0.5f));
    
    const PlatformCreationParams& params = IPlatform::getSingleton().getCreationParams();
    SceneManager* sceneMgr = IPlatform::getSingleton().getSceneManager();
    sceneMgr->getActiveCamera()->setPosition(Vector3(0.0f, 0.0f, 40.0f));
    Widget* rootNode = sceneMgr->getRootWidget();
    // create notice label
    mNoticeLabel = Label::create("Notice: click open button to choose model file", 15);
    mNoticeLabel->setPosition(Vector2(0, params.height - 45.0f));
    mNoticeLabel->setAnchorPoint(Vector2Zero);
    rootNode->addChild(mNoticeLabel);
    
    // create open file button
    mOpenFile = Button::create("btn_normal.png", "btn_down.png");
    mOpenFile->setPosition(Vector2(35, params.height - 15.0f));
    mOpenFile->setTitleText("Open");
    mOpenFile->setTitleColor(Color4Black);
    mOpenFile->setTitleFontSize(15.0f);
    rootNode->addChild(mOpenFile);
    // add button click action
    mOpenFile->setClickedAction([&](Button*, const Vector2&){
        std::vector<std::string> supportExts;
        supportExts.push_back("obj");
        // open file choose panel
        openChooseFilePannel(supportExts, [&](const char* file){
            if (strlen(file) > 0) {
                mChoosedFile = file;
                mNoticeLabel->setText("Loading mesh ...");
                mOpenFile->setEnabled(false);
            }
        });
    });
    
    // register mouse message to control node
    IPlatform::getSingleton().getEventDispatcher()->addClickEventListener(rootNode, [&](ClickEvent event, const std::vector<Vector2>& pos){
        if (mMeshNode && mMeshNode->isAttachedMesh())
        {
            static Vector2 lastDragPos, wheelPos;
            if (event == ClickEvent::eDrag) {
                // calc moved relative dis
                if (lastDragPos != Vector2Zero) {
                    Vector2 dis = pos[0] - lastDragPos;
                    this->setNodeRotation(Vector3(mNodeRotation.x + dis.y / 100.0f, mNodeRotation.y - dis.x / 100.0f, mNodeRotation.z));
                }
                lastDragPos = pos[0];
            }
            else if (event == ClickEvent::eUp) {
                lastDragPos = Vector2Zero;
            }
            else if (event == ClickEvent::eScrollWheel) {
                float delta = (pos[0].y * mNodeScale.y)/20.0f;
                this->setNodeScale(Vector3(mNodeScale.x + delta, mNodeScale.y + delta, mNodeScale.z + delta));
            }
        }
    });
    
    // reset scene manager
    reset();
    return true;
}

void MeshScene::update(float lastFrameTime)
{
    if (mChoosedFile.size() > 0) {
        Mesh* model = ResourceManager::getSingleton().addMesh(mChoosedFile.c_str());
        if (model) {
            // delete old scene node
            if (!mMeshNode) {
                SceneManager* sceneMgr = IPlatform::getSingleton().getSceneManager();
                mMeshNode = sceneMgr->getRootSceneNode()->createChild();
            }
            mMeshNode->attachMesh(model);
            // reset scene manager
            reset();
            // show current file path
            char curFile[256];
            sprintf(curFile, "File: %s", mChoosedFile.c_str());
            mNoticeLabel->setText(curFile);
        }
        else {
            this->mNoticeLabel->setText("File load failed");
        }
        mOpenFile->setEnabled(true);
        mChoosedFile.clear();
    }
}

void MeshScene::reset()
{
    // reset scale and scene node
    setNodeRotation(Vector3Zero);
    setNodeScale(Vector3(1.0, 1.0, 1.0));
}

void MeshScene::setNodeScale(const Vector3& scale)
{
    if (mMeshNode && scale.x > FLT_EPSILON && scale.y > FLT_EPSILON && scale.z > FLT_EPSILON) {
        mNodeScale = scale;
        mMeshNode->setScale(mNodeScale);
        // update text
        const PlatformCreationParams& params = IPlatform::getSingleton().getCreationParams();
        SceneManager* sceneMgr = IPlatform::getSingleton().getSceneManager();
        char strPos[256];
        sprintf(strPos, "Scale: (%f, %f, %f)", mNodeScale.x, mNodeScale.y, mNodeScale.z);
        if (!mScaleLabel) {
            mScaleLabel = Label::create("", 15);
            mScaleLabel->setPosition(Vector2(0, params.height - 60.0f));
            mScaleLabel->setAnchorPoint(Vector2Zero);
            sceneMgr->getRootWidget()->addChild(mScaleLabel);
        }
        mScaleLabel->setText(strPos);
    }
}

void MeshScene::setNodeRotation(const Vector3& rotate)
{
    if (mMeshNode) {
        mNodeRotation = rotate;
        mMeshNode->setRotation(mNodeRotation);
        // update text
        const PlatformCreationParams& params = IPlatform::getSingleton().getCreationParams();
        SceneManager* sceneMgr = IPlatform::getSingleton().getSceneManager();
        char strRotate[256];
        sprintf(strRotate, "Rotate: (%f, %f, %f)", mNodeRotation.x, mNodeRotation.y, mNodeRotation.z);
        if (!mRotationLabel) {
            mRotationLabel = Label::create("", 15);
            mRotationLabel->setPosition(Vector2(0, params.height - 75.0f));
            mRotationLabel->setAnchorPoint(Vector2Zero);
            sceneMgr->getRootWidget()->addChild(mRotationLabel);
        }
        mRotationLabel->setText(strRotate);
    }
}