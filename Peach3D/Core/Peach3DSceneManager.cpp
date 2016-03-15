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
        
        const Vector2& winSize = IPlatform::getSingleton().getCreationParams().winSize;
        // init object render projection
        setPerspectiveProjection(90.0f, winSize.x/winSize.y);
        
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
        for (auto iter : mCameraList) {
            delete iter;
        }
        mCameraList.clear();
        mActiveCamera = nullptr;
        // clean root scene node and all child node
        if (mRootSceneNode) {
            delete mRootSceneNode;
            mRootSceneNode = nullptr;
        }
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
        // clean preset objects
        if (mWidgetObject) {
            IRender::getSingleton().deleteObject(mWidgetObject);
            mWidgetObject = nullptr;
        }
        if (mOBBObject) {
            IRender::getSingleton().deleteObject(mOBBObject);
            mOBBObject = nullptr;
        }
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
    }
    
    void SceneManager::updateSceneNodeLighting(Node* sNode)
    {
        ((SceneNode*)sNode)->updateLightingState();
        // auto update children
        sNode->tranverseChildNode([&](size_t, Node* cNode){
            this->updateSceneNodeLighting(cNode);
        });
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
        mProjectionMatrix = Matrix4::createPerspectiveProjection(fovY, asPect, zNear, zFar);
    }
    
    void SceneManager::setOrthoProjection(float left, float right, float bottom, float top, float nearVal, float farVal)
    {
        mProjectionMatrix = Matrix4::createOrthoProjection(left, right, bottom, top, nearVal, farVal);
    }
    
    void SceneManager::addNewLight(const Light& l)
    {
        Peach3DAssert(l.type != LightType::eUnknow, "Can't add unknow type light");
        if (l.type != LightType::eUnknow) {
            bool isAddLight = false;
            if (l.name.size() > 0) {
                if (mLightList.find(l.name) == mLightList.end()) {
                    mLightList[l.name] = l;
                    isAddLight = true;
                }
            }
            else {
                static uint lightAutoIndex = 0; // lighting auto increase index
                auto lName = Utils::formatString("pd_Light%d", lightAutoIndex);
                mLightList[lName] = l;
                mLightList[lName].name = lName;
                isAddLight = true;
            }
            if (isAddLight) {
                mRootSceneNode->tranverseChildNode([&](size_t, Node* cNode){
                    this->updateSceneNodeLighting(cNode);
                });
            }
        }
    }

    bool SceneManager::getLight(const char* name, Light* outL)
    {
        if (mLightList.find(name) != mLightList.end()) {
            *outL = mLightList[name];
            return true;
        }
        return false;
    }
    
    void SceneManager::resetLight(const char* name, const Light& inL)
    {
        if (mLightList.find(name) != mLightList.end()) {
            mLightList[name] = inL;
            return ;
        }
        Peach3DWarnLog("Can't find light named \"%s\"", name);
    }
    
    void SceneManager::deleteLight(const char* name)
    {
        auto findIter = mLightList.find(name);
        if (findIter != mLightList.end()) {
            mLightList.erase(findIter);
        }
    }
    
    void SceneManager::tranverseLights(std::function<void(const std::string& name, const Light& l)> callFunc)
    {
        for (auto iter : mLightList) {
            callFunc(iter.first, iter.second);
        }
    }

    /*
#include "Peach3DCommonGL.h"
    Ray globalRay;
    GLuint rayVertexArrayId = 0;
    GLuint rayVertexBuffer = 0;
    Color4 rayColor = Color4Black;
    IProgram* rayProgram = nullptr;
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
    
    void SceneManager::render(float lastFrameTime)
    {
        // update camera
        mActiveCamera->prepareForRender(lastFrameTime);
        
        // clear render and picking scene node list
        mRenderNodeMap.clear();
        mRenderOBBList.clear();
        mPickSceneNodeList.clear();
        // update root node and add children to render list
        mRootSceneNode->prepareForRender(lastFrameTime);
        mRootSceneNode->tranverseChildNode([&](size_t, Node* childNode) {
            this->addSceneNodeToCacheList(childNode, lastFrameTime);
        });
        
        IRender* mainRender = IRender::getSingletonPtr();
        mainRender->prepareForObjectRender();
        // draw all scene node
        RenderNode* lastRenderNode = nullptr;
        std::vector<RenderNode*> curNodeList;
        for (auto iter : mRenderNodeMap) {
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
        if (mRenderOBBList.size() > 0) {
            mOBBObject->render(mRenderOBBList);
        }
        
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
            rayProgram->updateObjectUnifroms(&mGlobalObjectAttrs, nullptr, 0.0f);
            // draw ray
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1, 0);
            glDrawArrays(GL_LINES, 0, 2);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }*/
        
        // sort widgets
        mRenderWidgetList.clear();
        int currentZOrder = 0;
        addWidgetToCacheList(&currentZOrder, mRootWidget, lastFrameTime);
        
        // must after widget prepare, because may be no program created.
        mainRender->prepareForWidgetRender();
        // render widgets. Notice: GL3 need create IObject for diff program, hash code change can't cause program generating VBO.
        Widget* lastRenderWidget = nullptr;
        std::vector<Widget*> curList;
        for (size_t i = 0; i < mRenderWidgetList.size(); ++i) {
            Widget* curWidget = mRenderWidgetList[i];
            if (lastRenderWidget && lastRenderWidget->getRenderStateHash() != curWidget->getRenderStateHash()){
                // render current widgets
                mWidgetObject->render(curList);
                curList.clear();
            }
            // add widget to cache list for next rendering
            curList.push_back(curWidget);
            lastRenderWidget = curWidget;
        }
        if (curList.size() > 0) {
            // render last widgets
            mWidgetObject->render(curList);
        }
    }
    
    void SceneManager::addSceneNodeToCacheList(Node* node, float lastFrameTime)
    {
        SceneNode* rNode = static_cast<SceneNode*>(node);
        // prepare render first for update "NeedRender"
        rNode->prepareForRender(lastFrameTime);
        if (node->isNeedRender()) {
            // cache all RenderNode from SceneNode
            rNode->tranverseRenderNode([&](const char*, RenderNode* node) {
                mRenderNodeMap.insert(std::make_pair(node->getRenderHash(), node));
            });
        }
        
        // cache pick node
        bool pickEnabled = rNode->isPickingEnabled();
        if (pickEnabled && (node->isNeedRender() || rNode->isPickingAlways())) {
            mPickSceneNodeList.push_back(rNode);
        }
        
        // cache OBB node
        if (rNode->getOBBEnabled()) {
            rNode->tranverseRenderNode([&](const char*, RenderNode* node) {
                if (node->getOBBEnabled()) {
                    mRenderOBBList.push_back(node->getRenderOBB());
                }
            });
        }
        
        node->tranverseChildNode([&](size_t, Node* child) {
            // add all children scene node
            this->addSceneNodeToCacheList(child, lastFrameTime);
        });
    }
    
    void SceneManager::addWidgetToCacheList(int* zOrder, Widget* widget, float lastFrameTime)
    {
        // add a widget
        widget->prepareForRender(lastFrameTime);
        widget->setGlobalZOrder(*zOrder);
        if (widget->isNeedRender()) {
            mRenderWidgetList.push_back(widget);
        }
        // bigger zorder, late render
        (*zOrder)++;
        
        // add all child to cache
        if (widget->getChildrenCount() > 0) {
            widget->tranverseChildNode([&](size_t, Node* node) {
                // deal with children
                this->addWidgetToCacheList(zOrder, static_cast<Widget*>(node), lastFrameTime);
            });
        }
    }
    
    SceneNode* SceneManager::getWindowClickedNode(const Vector2& clickedPos, RenderNode** outNode)
    {
        if (mActiveCamera) {
            Ray clickedRay;
            clickedRay.startPos = mActiveCamera->getPosition();
            // convert to NDZ coord
            const Vector2& winSize = IPlatform::getSingleton().getCreationParams().winSize;
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
                for (auto node : mPickSceneNodeList) {
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