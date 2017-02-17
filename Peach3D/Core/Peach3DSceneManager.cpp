//
//  Peach3DSceneManager.cpp
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DSceneManager.h"
#include "Peach3DIPlatform.h"
#include "Peach3DLabel.h"
#include "Peach3DVector4.h"
#include "Peach3DUtils.h"
#include "Peach3DLayoutManager.h"
#include "Peach3DResourceManager.h"
#include "Peach3DParticle.h"

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(SceneManager);
    
    void SceneManager::init()
    {
        IRender* curRender = IRender::getSingletonPtr();
        // default set clear color to gray
        curRender->setRenderClearColor(Color4Gray);
        // set default OBB color to green
        curRender->setRenderOBBColor(Color4Green);
        // set default lines width
        curRender->setRenderLineWidth(2.0f);
        
        // create root scene node
        mRootSceneNode = new SceneNode();
        // create default main camera
        mActiveCamera = createLockedCamera();
        // make base object (size 1.f) in camera's field
        mActiveCamera->setPosition(Vector3(0.f, 0.f, 10.f));
        
        const Vector2& winSize = LayoutManager::getSingleton().getScreenSize();
        // init object render projection
        setPerspectiveProjection(60.0f, winSize.x/winSize.y);
        
        // create root widget and set to center
        mRootWidget = Widget::create();
        mRootWidget->setAnchorPoint(Vector2Zero);
        // create draw stats node
        if (IPlatform::getSingleton().getIsDrawStats()) {
            createDrawStatsNode();
        }
        
        // create a IObject for widget rendering
        createPresetObjects();
    }

    SceneManager::~SceneManager()
    {
        // clean all Camera
        for (auto& iter : mCameraList) {
            delete iter;
        }
        mCameraList.clear();
        mActiveCamera = nullptr;
        // clean root scene node and all child node
        if (mRootSceneNode) {
            delete mRootSceneNode;
            mRootSceneNode = nullptr;
        }
        // delete all lights and free memory
        deleteAllLights();
        // delete draw stats update schedule
        if (mDrawUpdateSchedule) {
            IPlatform::getSingletonPtr()->deleteScheduler(mDrawUpdateSchedule);
            mDrawUpdateSchedule = nullptr;
        }
        mDebugDrawNode = nullptr;
        // clean root widget and all child widget
        if (mRootWidget) {
            delete mRootWidget;
            mRootWidget = nullptr;
        }
        // mWidgetObject and mOBBObject will auto clean
        mWidgetObject = nullptr;
        mOBBObject = nullptr;
        mParticle2DObject = nullptr;
        mParticle3DObject = nullptr;
    }
    
    void SceneManager::reset()
    {
        // delete all SceneNode
        mRootSceneNode->deleteAllChildren();
        // delete all widget except DebugWidget
        mRootWidget->tranverseChildNode([&](size_t, Node* child){
            if (static_cast<Widget*>(child)->getLocalZOrder() < NOT_CLEAR_WIDGET_ZORDER) {
                mRootWidget->deleteChild(child);
            }
        });
        // also delete root widget listener if exsited
        EventDispatcher::getSingleton().deleteClickEventListener(mRootWidget);
        // delete all lights if exsited
        deleteAllLights();
    }
    
    void SceneManager::createDrawStatsNode()
    {
        if (!mDebugDrawNode) {
            // create labels
            mDebugDrawNode = Label::create("FPS: 0\nDraw count: 0\nTriangle count: 0", 14);
            mDebugDrawNode->setAnchorPoint(Vector2Zero);
            mDebugDrawNode->setPosition(Vector2Zero);
            mDebugDrawNode->setLocalZOrder(NOT_CLEAR_WIDGET_ZORDER);
            // add to root widget
            mRootWidget->addChild(mDebugDrawNode);
            
            // create a timer to update
            if (mDrawUpdateSchedule) {
                IPlatform::getSingletonPtr()->deleteScheduler(mDrawUpdateSchedule);
                mDrawUpdateSchedule = nullptr;
            }
            mDrawUpdateSchedule = IPlatform::getSingleton().addScheduler([&](float interval){
                static char infoText[128];
                IPlatform* platform = IPlatform::getSingletonPtr();
                sprintf(infoText, "FPS: %d\nDraw count: %d\nTriangle count: %d",
                        int(platform->getCurrentFPSAndClear() / interval),
                        platform->getDrawCount(),
                        platform->getTriangleCount());
                static_cast<Label*>(mDebugDrawNode)->setText(infoText);
            }, 1.0f);
        }
    }
    
    void SceneManager::createPresetObjects()
    {
        if (!mWidgetObject) {
            // create IObject for widget rendering
            mWidgetObject = IRender::getSingleton().createObject("pd_WidgetObject");
            float fixedVertexData[] = {-1.0f,1.0f, -1.0f,-1.0f, 1.0f,1.0f, 1.0f,-1.0f};
            ushort fixedIndexData[] = {0, 1, 3, 0, 3, 2};
            mWidgetObject->setVertexBuffer(fixedVertexData, sizeof(fixedVertexData), VertexType::Point2);
            mWidgetObject->setIndexBuffer(fixedIndexData, sizeof(fixedIndexData));
        }
        if (!mOBBObject) {
            // create IObject for OBB rendering
            mOBBObject = IRender::getSingleton().createObject("pd_OBBObject");
            float fixedVertexData[] = {-0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
                0.5f, 0.5f, -0.5f,  -0.5f, 0.5f, -0.5f,
                -0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 0.5f,
                0.5f, 0.5f, 0.5f,  -0.5f, 0.5f, 0.5f};
            ushort fixedIndexData[] = {0, 1, 1, 2, 2, 3, 0, 3,  4, 5, 5, 6, 6, 7, 4, 7,  0, 4, 1, 5, 2, 6, 3, 7};
            mOBBObject->setVertexBuffer(fixedVertexData, sizeof(fixedVertexData), VertexType::Point3);
            mOBBObject->setIndexBuffer(fixedIndexData, sizeof(fixedIndexData));
        }
        if (!mParticle2DObject) {
            // create IObject for 2D particle
            mParticle2DObject = IRender::getSingleton().createObject("pd_Particle2DObject");
            float fixedVertexData[] = {0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f};
            mParticle2DObject->setVertexBuffer(fixedVertexData, sizeof(fixedVertexData), VertexType::Point2|VertexType::Color|VertexType::PSprite);
        }
        if (!mParticle3DObject) {
            // create IObject for 3D particle
            mParticle3DObject = IRender::getSingleton().createObject("pd_Particle3DObject");
            float fixedVertexData[] = {0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 0.f};
            mParticle3DObject->setVertexBuffer(fixedVertexData, sizeof(fixedVertexData), VertexType::Point3|VertexType::Color|VertexType::PSprite);
        }
    }
    
    Camera* SceneManager::createFreeCamera(const Vector3& pos, const Vector3& rotate)
    {
        Camera* newCamera = new Camera(pos, rotate);
        mCameraList.push_back(newCamera);
        // set active camera if there no active camera
        if (mActiveCamera == nullptr) {
            setActiveCamera(newCamera);
        }
        return newCamera;
    }

    Camera* SceneManager::createLockedCamera(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        Camera* newCamera = new Camera(eye, center, up);
        mCameraList.push_back(newCamera);
        // set active camera if there no active camera
        if (mActiveCamera == nullptr) {
            setActiveCamera(newCamera);
        }
        return newCamera;
    }
    
    void SceneManager::deleteCamera(Camera* camera)
    {
        Peach3DAssert(camera!=mActiveCamera, "Can't delete the active camera");
        delete camera;
    }
    
    void SceneManager::setActiveCamera(Camera* newCamera)
    {
        if (newCamera!=mActiveCamera) {
            mActiveCamera = newCamera;
        }
    }
    
    void SceneManager::setPerspectiveProjection(float fovY, float asPect, float zNear, float zFar)
    {
        Peach3DAssert(zNear > FLT_EPSILON, "zNear must bigger than 0!");
        mProjectionMatrix = Matrix4::createPerspectiveProjection(DEGREE_TO_RADIANS(fovY), asPect, zNear, zFar);
    }
    
    void SceneManager::setOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        mProjectionMatrix = Matrix4::createOrthoProjection(left, right, bottom, top, nearVal, farVal);
    }
    
    LightPtr SceneManager::addNewLight(const char* name)
    {
        std::string insertName = name;
        if (insertName.empty() || mLightList.find(insertName) != mLightList.end()) {
            static uint lightAutoIndex = 0; // lighting auto increase index
            insertName = Utils::formatString("pd_Light%d", lightAutoIndex++);
        }
        // create new null light
        LightPtr newL(new Light(insertName.c_str()));
        mLightList[insertName] = newL;
        // lighting need be updated if new light
        setUpdateLighting();
        return newL;
    }

    LightPtr SceneManager::getLight(const char* name)
    {
        if (name && mLightList.find(name) != mLightList.end()) {
            return mLightList[name];
        }
        return nullptr;
    }
    
    void SceneManager::deleteLight(const char* name)
    {
        auto findIter = mLightList.find(name);
        // light will autorelease, just erase it
        if (findIter != mLightList.end()) {
            mLightList.erase(findIter);
            // delete a light, node need update valid name
            setUpdateLighting();
        }
    }
    
    void SceneManager::deleteAllLights()
    {
        // lights will autorelease
        mLightList.clear();
        // disable all lighting
        setUpdateLighting();
    }
    
    void SceneManager::setUpdateLighting()
    {
        // update all scene node
        mRootSceneNode->setLightingStateNeedUpdate();
        // update all terrains
        for (auto ter : mTerrainList) {
            ter->setLightingStateNeedUpdate();
        }
    }
    
    void SceneManager::tranverseLights(std::function<void(const std::string& name, const LightPtr& l)> callFunc, bool onlyEnabled)
    {
        for (auto& iter : mLightList) {
            if ((!onlyEnabled || iter.second->isEnabled()) && iter.second->getType() != LightType::eUnknow) {
                callFunc(iter.first, iter.second);
            }
        }
    }

    /*
#include "Peach3DCommonGL.h"
    Ray globalRay;
    GLuint rayVertexArrayId = 0;
    GLuint rayVertexBuffer = 0;
    Color4 rayColor = Color4Black;
    ProgramPtr rayProgram = nullptr;
    void genRay()
    {
        if (rayVertexArrayId > 0) {
            return ;
        }
        glGenVertexArrays(1, &rayVertexArrayId);
        glBindVertexArray(rayVertexArrayId);
        
        glGenBuffers(1, &rayVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, rayVertexBuffer);
        GLfloat globalVertex[] = {-0.5, -0.5, -0.5, rayColor.r, rayColor.g, rayColor.b, rayColor.a,
            0.5, -0.5, -0.5, rayColor.r, rayColor.g, rayColor.b, rayColor.a};
        glBufferData(GL_ARRAY_BUFFER, sizeof(globalVertex), globalVertex, GL_DYNAMIC_DRAW);
        const GLsizei vStride = 7*sizeof(float);
        glEnableVertexAttribArray(DefaultAttrLocation::eVertex);
        glVertexAttribPointer(DefaultAttrLocation::eVertex, 3, GL_FLOAT, GL_FALSE, vStride, 0);
        glEnableVertexAttribArray(DefaultAttrLocation::eColor);
        glVertexAttribPointer(DefaultAttrLocation::eColor, 4, GL_FLOAT, GL_FALSE, vStride, PEACH3D_BUFFER_OFFSET(3 * sizeof(float)));
        
        Material rayMtl;
        rayProgram = ResourceManager::getSingleton().getObjectPresetProgram(VertexType::Point3|VertexTypeColor4, rayMtl);
    }
     */
    
    void SceneManager::renderOncePass(Render3DPassContent* content)
    {
        // update all scene node
        Render3DPassContent newContent;
        if (!content) {
            // update all lights
            for (auto l : mLightList) {
                l.second->prepareForRender();
            }
            // get render content first
            mRootSceneNode->tranverseChildNode([&](size_t, Node* childNode) {
                this->addSceneNodeToCacheList(childNode, 0.f, &newContent, false);
            });
            content = &newContent;
        }
        // reupdate global uniforms for GL3, befor rendering may modify camera
        IRender::getSingleton().prepareForObjectRender();
        // draw all terrains
        for (auto trr : mTerrainList) {
            trr->getObject()->render(trr);
        }
        // draw all scene node
        RenderNode* lastRenderNode = nullptr;
        std::vector<RenderNode*> curNodeList;
        for (auto& iter : content->nodeMap) {
            RenderNode* curNode = iter.second;
            if (lastRenderNode && lastRenderNode->getRenderHash() != curNode->getRenderHash()){
                // render current objects
                lastRenderNode->getObject()->render(curNodeList);
                curNodeList.clear();
            }
            // add object to cache list for next rendering
            curNodeList.push_back(curNode);
            lastRenderNode = curNode;
        }
        if (curNodeList.size() > 0) {
            // render last objects
            lastRenderNode->getObject()->render(curNodeList);
        }
        // draw all OBB
        if (content->OBBList.size() > 0) {
            mOBBObject->render(content->OBBList);
        }
        // draw all particles
        for (auto& particle : content->particles) {
            mParticle3DObject->render(particle);
        }
    }
    
    void SceneManager::renderForRTT(const TexturePtr& rtt, Render3DPassContent* content)
    {
        // clean frame before render
        rtt->beforeRendering();
        // render RTT pass, not cache click list
        renderOncePass(content);
        // restore render state
        rtt->afterRendering();
    }
    
    void SceneManager::render(float lastFrameTime)
    {
        // clear picking scene node list once, only add in main pass
        mPickSceneNodeList.clear();
        for (auto& camera : mCameraList) {
            camera->prepareForRender(lastFrameTime);
        }
        // update all lights
        for (auto l : mLightList) {
            l.second->prepareForRender();
        }
        /* traverse all nodes, activate used RTT.
         root scene node must update, children may need delete. */
        mRootSceneNode->prepareForRender(lastFrameTime);
        // make all RTT inactive first
        ResourceManager::getSingleton().inactiveRenderTextures();
        // active RTT, calc main content and cache clicked node
        Render3DPassContent mainContent;
        mRootSceneNode->tranverseChildNode([&](size_t, Node* childNode) {
            addSceneNodeToCacheList(childNode, lastFrameTime, &mainContent, true);
        });
        // sort widgets, activate used RTT
        mRenderWidgetList.clear();
        int currentZOrder = 0;
        addWidgetToCacheList(&currentZOrder, mRootWidget, lastFrameTime);
        
        // is node render shadow
        bool isUpdateDepth = false;
        if (mLightList.size() > 0) {
            Render3DPassContent depthContent = mainContent;
            for (auto& l : mLightList) {
                auto shadowTex = l.second->getShadowTexture();
                if (shadowTex) {
                    if (!isUpdateDepth) {
                        // erase can't bring shadow node
                        for (auto iter=depthContent.nodeMap.begin(); iter!=depthContent.nodeMap.end();) {
                            if (!iter->second->isBringShadow()) {
                                iter = depthContent.nodeMap.erase(iter);
                            }
                            else {
                                // mark render shadow now
                                iter->second->setRenderForShadow(true);
                                ++iter;
                            }
                        }
                        // depth not need render OBB
                        depthContent.OBBList.clear();
                        isUpdateDepth = true;
                    }
                    renderForRTT(shadowTex, &depthContent);
                }
            }
        }
        // if shadow rendering, mark render color now
        if (isUpdateDepth) {
            for (auto& nodeIter : mainContent.nodeMap) {
                nodeIter.second->setRenderForShadow(false);
            }
        }
        
        // we can update global uniforms here, OBB programe had generate
        IRender* mainRender = IRender::getSingletonPtr();
        // draw render target
        auto rttList = ResourceManager::getSingleton().getRenderTextureList();
        for (auto& tex : rttList) {
            if (tex->isActived() && tex->getFormat() != TextureFormat::eDepth) {
                // generate new content to render, because RTT may update node state
                renderForRTT(tex, nullptr);
            }
        }
        // clean frame before render
        mainRender->prepareForMainRender();
        // render main pass
        renderOncePass(&mainContent);
        // present after draw over
        mainRender->finishForMainRender();
        
        /*
        // draw rays
        if (rayVertexArrayId > 0) {
            
            glBindVertexArray(rayVertexArrayId);
            glBindBuffer(GL_ARRAY_BUFFER, rayVertexBuffer);
            float *cmData = (float*)glMapBufferRange(GL_ARRAY_BUFFER, 0, 2 * 7 * sizeof(float), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
            Vector3 endpos = globalRay.startPos + globalRay.direction * 10;
            Vector3 startPos = globalRay.startPos + globalRay.direction*3;
            cmData[0] = startPos.x; cmData[1] = startPos.y; cmData[2] = startPos.z;
            cmData[7] = endpos.x; cmData[8] = endpos.y; cmData[9] = endpos.z;
            glUnmapBuffer(GL_ARRAY_BUFFER);
            
            
            Matrix4 objAttrs;
            mGlobalObjectAttrs.modelMatrix = &objAttrs;
            // set program
            rayProgram->useAsRenderProgram();
            rayProgram->updateObjectUniforms(&mGlobalObjectAttrs, nullptr, 0.0f);
            // draw ray
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1, 0);
            glDrawArrays(GL_LINES, 0, 2);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }*/
        
        // must after widget prepare, because may be no program created.
        mainRender->prepareForWidgetRender();
        // render widgets. Notice: GL3 need create IObject for diff program, hash code change can't cause program generating VBO.
        Widget* lastRenderWidget = nullptr;
        std::vector<Widget*> curList;
        for (size_t i = 0; i < mRenderWidgetList.size(); ++i) {
            Widget* curWidget = dynamic_cast<Widget*>(mRenderWidgetList[i]);
            Particle2D* curParticle = dynamic_cast<Particle2D*>(mRenderWidgetList[i]);
            if (curWidget) {
                if (lastRenderWidget && lastRenderWidget->getRenderStateHash() != curWidget->getRenderStateHash()){
                    // render current widgets
                    mWidgetObject->render(curList);
                    curList.clear();
                }
                // add widget to cache list for next rendering
                curList.push_back(curWidget);
                lastRenderWidget = curWidget;
            }
            else if (curParticle) {
                // render last widgets
                if (curList.size() > 0) {
                    mWidgetObject->render(curList);
                    curList.clear();
                    lastRenderWidget = nullptr;
                }
                // render particle
                mParticle2DObject->render(curParticle);
            }
        }
        if (curList.size() > 0) {
            // render last widgets
            mWidgetObject->render(curList);
        }
    }
    
    void SceneManager::addSceneNodeToCacheList(Node* node, float lastFrameTime, Render3DPassContent* content, bool pickEnabled)
    {
        // prepare render first for update "NeedRender"
        node->prepareForRender(lastFrameTime);
        
        SceneNode* rNode = dynamic_cast<SceneNode*>(node);
        if (content) {
            Particle3D* pNode = dynamic_cast<Particle3D*>(node);
            if (rNode) {
                // add node to render list.
                if (rNode->isNeedRender()) {
                    rNode->tranverseRenderNode([&](const char*, RenderNode* node) {
                        content->nodeMap.insert(std::make_pair(node->getRenderHash(), node));
                    });
                }
                // cache OBB node
                if (rNode->getOBBEnabled()) {
                    rNode->tranverseRenderNode([&](const char*, RenderNode* node) {
                        if (node->getOBBEnabled()) {
                            content->OBBList.push_back(node->getRenderOBB());
                        }
                    });
                }
            }
            else if (pNode) {
                // cache particle list
                content->particles.push_back(pNode);
            }
        }
        
        // cache pick node in main pass, just update once for each frame
        if (pickEnabled && rNode) {
            bool pickEnabled = rNode->isPickingEnabled();
            if (pickEnabled && (node->isNeedRender() || rNode->isPickingAlways())) {
                mPickSceneNodeList.push_back(rNode);
            }
        }
        
        node->tranverseChildNode([&](size_t, Node* child) {
            // add all children scene node
            this->addSceneNodeToCacheList(child, lastFrameTime, content, pickEnabled);
        });
    }
    
    void SceneManager::addWidgetToCacheList(int* zOrder, Node* child, float lastFrameTime)
    {
        // add a widget
        child->prepareForRender(lastFrameTime);
        if (dynamic_cast<Widget*>(child)) {
            dynamic_cast<Widget*>(child)->setGlobalZOrder(*zOrder);
        }
        if (child->isNeedRender()) {
            mRenderWidgetList.push_back(child);
        }
        // bigger zorder, late render
        (*zOrder)++;
        
        // add all child to cache
        if (child->getChildrenCount() > 0) {
            child->tranverseChildNode([&](size_t, Node* node) {
                // deal with children
                this->addWidgetToCacheList(zOrder, node, lastFrameTime);
            });
        }
    }
    
    void SceneManager::updateSceneSize(const Vector2& size)
    {
        mRootWidget->setContentSize(size);
        mRootWidget->setNeedUpdateRenderingAttributes();
        // modify projective aspect
        if (mProjectionMatrix.mat[11] < -0.5f) {
            mProjectionMatrix.mat[0] = (mProjectionMatrix.mat[5] * size.y) / size.x;
        }
    }
    
    SceneNode* SceneManager::getWindowClickedNode(const Vector2& clickedPos, RenderNode** outNode)
    {
        if (mActiveCamera) {
            Ray clickedRay;
            clickedRay.startPos = mActiveCamera->getPosition();
            // convert to NDZ coord
            const Vector2& winSize = LayoutManager::getSingleton().getScreenSize();
            float x = (2.0f * clickedPos.x) / winSize.x - 1.0f;
            float y = (2.0f * clickedPos.y) / winSize.y - 1.0f;
            // use zFar to calc ray
            Vector4 ray_nds(x, y, 1.0f, 1.0f);
            // convert to view coord
            Matrix4 projectiveInv;
            mProjectionMatrix.getInverse(&projectiveInv);
            Vector4 ray_eye = ray_nds * projectiveInv;
            // convert to world coord
            Matrix4 viewMatrix = mActiveCamera->getViewMatrix(), viewInv;
            viewMatrix.getInverse(&viewInv);
            Vector4 ray_world = ray_eye * viewInv;
            if (ray_world.w !=0.0f) {
                Vector3 ray3(ray_world.x / ray_world.w, ray_world.y / ray_world.w, ray_world.z / ray_world.w);
                clickedRay.delta = ray3 - clickedRay.startPos;
                
                // check is node cross with clickedRay
                for (auto& node : mPickSceneNodeList) {
                    RenderNode* pickRenderNode = node->isRayIntersect(clickedRay);
                    if (pickRenderNode) {
                        if (outNode) {
                            *outNode = pickRenderNode;
                        }
                        return node;
                    }
                }
            }
        }
        return nullptr;
    }
}
