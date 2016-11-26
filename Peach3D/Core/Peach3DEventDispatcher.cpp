//
//  Peach3DEventDispatcher.cpp
//  Peach3DLib
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DEventDispatcher.h"
#include "Peach3DWidget.h"
#include "Peach3DIPlatform.h"
#include "Peach3DSceneManager.h"

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(EventDispatcher);
    
    EventDispatcher::EventDispatcher() : mCurHoldTime(0.0f), mEventHoldTime(2.0f), mHoldScheduler(nullptr),
        mCurClickedTime(0.0f), mEventDClickTime(0.5f), mDClickScheduler(nullptr), mMovedNode(nullptr),
        mFocusNode(nullptr), mDragNode(nullptr), mFocusClickId(0), mKeyboardScene(nullptr)
    {
        // add scheduler for calc hold time and double click time, not auto start
        mHoldScheduler = IPlatform::getSingleton().addScheduler(std::bind(&EventDispatcher::holdSchedulerCallback, this, std::placeholders::_1), 0.0f, -1, false);
        mDClickScheduler = IPlatform::getSingleton().addScheduler(std::bind(&EventDispatcher::clickSchedulerCallback, this, std::placeholders::_1), 0.0f, -1, false);
    }
    
    EventDispatcher::~EventDispatcher()
    {
        if (mHoldScheduler) {
            IPlatform::getSingleton().deleteScheduler(mHoldScheduler);
            mHoldScheduler = nullptr;
        }
        if (mDClickScheduler) {
            IPlatform::getSingleton().deleteScheduler(mDClickScheduler);
            mDClickScheduler = nullptr;
        }
    }
    
    void EventDispatcher::addClickEventListener(Node* target, ClickListenerFunction func)
    {
        if (target) {
            if (mClickNodeMap.find(target)==mClickNodeMap.end()) {
                mClickNodeMap[target] = func;
            }
            if (std::find(mNodeList.begin(), mNodeList.end(), target)==mNodeList.end()) {
                mNodeList.push_back(target);
            }
        }
    }
    
    void EventDispatcher::deleteClickEventListener(Node* target)
    {
        if (target) {
            auto clickIter = mClickNodeMap.find(target);
            if (clickIter != mClickNodeMap.end()) {
                mClickNodeMap.erase(clickIter);
            }
            auto nodeIter = std::find(mNodeList.begin(), mNodeList.end(), target);
            if (nodeIter != mNodeList.end()) {
                mNodeList.erase(nodeIter);
            }
            
            // clean current focus node
            if (mFocusNode == target) {
                mFocusNode = nullptr;
            }
            if (mDragNode == target) {
                mDragNode = nullptr;
            }
            if (mMovedNode == target) {
                mMovedNode = nullptr;
            }
        }
    }
    
    void EventDispatcher::addKeyboardListener(IScene* target, KeyboardListenerFunction func)
    {
        mKeyboardListener = func;
        mKeyboardScene = target;
    }
    
    void EventDispatcher::deleteKeyboardListener(IScene* target)
    {
        if (target == mKeyboardScene) {
            mKeyboardListener = nullptr;
            mKeyboardScene = nullptr;
        }
    }
    
    void EventDispatcher::triggerClickEvent(ClickEvent event, std::vector<uint> clickIds, const std::vector<Vector2>& poss)
    {
        // sort all node first depend on zorder or type
        sortEventNodes();
        // event maybe trigged after engine destroyed
        auto sceneMgr = SceneManager::getSingletonPtr();
        if (!sceneMgr) return;
        
        std::vector<uint>    rootNodeIds;
        std::vector<Vector2> rootNodePoss;
        // start deal widget event
        for (uint i=0; i<clickIds.size(); ++i) {
            // get current focus clickId
            if (!mFocusClickId) {
                mFocusClickId = clickIds[i];
            }
            if (mFocusClickId == clickIds[i]) {
                triggerNodeEvent(event, clickIds[i], poss[i]);
            }
            else {
                rootNodeIds.push_back(clickIds[i]);
                rootNodePoss.push_back(poss[i]);
            }
        }
        
        // trigger multi-touch for root widget
        if (rootNodePoss.size() > 0 && rootNodeIds.size() > 0) {
            triggerGestureEvent(event, rootNodeIds, rootNodePoss);
        }
    }
    
    void EventDispatcher::triggerNodeEvent(ClickEvent event, uint clickId, const Vector2& pos)
    {
        mLastPoint = pos;
        
        // find all nodes which event pos inside
        const Widget* rootNode = SceneManager::getSingleton().getRootWidget();
        std::vector<Node*> eventNodes;
        Node* swallowNode = nullptr;
        for (auto& node : mNodeList) {
            Widget* cwNode = dynamic_cast<Widget*>(node);
            if (cwNode && cwNode->isPointInZone(pos) && cwNode != rootNode) {
                eventNodes.push_back(cwNode);
                // not continue finding follow node
                if (cwNode->isSwallowEvents()) {
                    swallowNode = cwNode;
                    break;
                }
            }
        }
        // finding 3D node which event pos inside, add to back of list
        SceneNode*  sceneEventNode = SceneManager::getSingleton().getWindowClickedNode(pos);
        if (sceneEventNode) {
            eventNodes.push_back(sceneEventNode);
            // set swallow 3d node
            if (sceneEventNode->isSwallowEvents() && !swallowNode) {
                swallowNode = sceneEventNode;
            }
        }
        // get default focus node
        Node* firstNode = eventNodes.size() ? eventNodes[0] : nullptr;
        
        if (event == ClickEvent::eMoved) {
            // trigger move out when move to other node or null space
            if (mMovedNode != firstNode) {
                if (mMovedNode) {
                    mClickNodeMap[mMovedNode](ClickEvent::eMoveOut, pos);
                    mMovedNode = nullptr;
                    // not hold node, pause scheduler
                    mCurHoldTime = 0.0f;
                    mHoldScheduler->pause();
                }
                if (firstNode) {
                    mClickNodeMap[firstNode](ClickEvent::eMoveIn, pos);
                    mMovedNode = firstNode;
                    // start hold scheduler
                    mHoldScheduler->start();
                    mCurHoldTime = 0.0f;
                }
            }
            else if (firstNode) {
                mClickNodeMap[firstNode](event, pos);
            }
        }
        else if (event == ClickEvent::eDown && firstNode) {
            // release move node if it exist
            mMovedNode = nullptr;
            mBeginPoint = mLastPoint;
            // set focus node
            mFocusNode = firstNode;
            mDragNode = firstNode;
            mClickNodeMap[firstNode](event, pos);
            // start hold scheduler
            mHoldScheduler->start();
            mCurHoldTime = 0.0f;
        }
        else if (event == ClickEvent::eUp) {
            // save last clicked node for double clicked event
            static Node* lastClickedNode = nullptr;
            if (mFocusNode) {
                mClickNodeMap[mFocusNode](event, pos);
                
                // trigger click event
                if (mFocusNode == firstNode) {
                    mClickNodeMap[mFocusNode](ClickEvent::eClicked, pos);
                    if (mCurClickedTime < FLT_EPSILON || mCurClickedTime > mEventDClickTime) {
                        lastClickedNode = firstNode;
                        // start click time for double clicked event
                        mCurClickedTime = 0.0f;
                        mDClickScheduler->start();
                    }
                    else if (mCurClickedTime < mEventDClickTime && mCurClickedTime > FLT_EPSILON && lastClickedNode == firstNode) {
                        lastClickedNode = nullptr;
                        mClickNodeMap[mFocusNode](ClickEvent::eDClicked, pos);
                        // pause click time for double clicked event
                        mCurClickedTime = 0.0f;
                        mDClickScheduler->pause();
                    }
                }
                mDragNode = nullptr;
                mFocusNode = nullptr;
            }
            mFocusClickId = 0;
        }
        else if (event == ClickEvent::eDrag) {
            // deal dragIn and dragOut event
            if (mDragNode != firstNode) {
                if (mDragNode) {
                    mClickNodeMap[mDragNode](ClickEvent::eDragOut, pos);
                    // not hold node pause sheduler
                    mCurHoldTime = 0.0f;
                    mHoldScheduler->pause();
                }
                if (firstNode) {
                    mClickNodeMap[firstNode](ClickEvent::eDragIn, pos);
                    mDragNode = firstNode;
                    // start hold scheduler
                    mCurHoldTime = 0.0f;
                    mHoldScheduler->start();
                }
            }
            
            if (mFocusNode) {
                mClickNodeMap[mFocusNode](event, pos);
                if (mFocusNode == firstNode) {
                    if (!mFocusNode->isSwallowEvents()) {
                        // node not swallow event, pass event to parent node if drag for a distence
                        Vector2 dis = mLastPoint - mBeginPoint;
                        if (dis.length() > 8 && swallowNode) {
                            // transmit event to next widget if node not swallowed
                            mClickNodeMap[mFocusNode](ClickEvent::eCancel, pos);
                            mFocusNode = swallowNode;
                        }
                    }
                    else {
                        mBeginPoint = mLastPoint;
                        mHoldScheduler->start();
                        mCurHoldTime = 0.0f;
                    }
                }
            }
        }
        else if (event == ClickEvent::eScrollWheel && swallowNode) {
            // just trigger scroll wheel event
            mClickNodeMap[swallowNode](event, pos);
        }
        else if (event == ClickEvent::eCancel && mFocusNode) {
            mClickNodeMap[mFocusNode](event, pos);
            // release focus node
            mFocusNode = nullptr;
            mDragNode = nullptr;
        }
    }
    
    void EventDispatcher::triggerGestureEvent(ClickEvent event, std::vector<uint> clickIds, const std::vector<Vector2>& poss)
    {
        Widget* const rootNode = SceneManager::getSingletonPtr()->getRootWidget();
        static bool isClickValid = false;
        static Vector2 focusPos, gesturePos;
        static uint focusId = 0, gestureId = 0;
        
        for (size_t i=0; i<clickIds.size(); ++i) {
            if (event == ClickEvent::eDown) {
                // set focusId and gestureId
                if (!focusId) {
                    focusId = clickIds[i];
                    focusPos = poss[i];
                    // set click event valid
                    isClickValid = true;
                }
                else if (!gestureId) {
                    gestureId = clickIds[i];
                    gesturePos = poss[i];
                    // click event not valid, gesture event will be generate
                    isClickValid = false;
                }
            }
            else if (event == ClickEvent::eUp || event == ClickEvent::eCancel) {
                // delete focusId or gestureId
                if (focusId == clickIds[i] || gestureId == clickIds[i]) {
                    if (focusId == clickIds[i]) {
                        focusId = gestureId;
                        focusPos = gesturePos;
                    }
                    gestureId = 0;
                    gesturePos = Vector2Zero;
                }
            }
        }
        
        if (event == ClickEvent::eDrag && focusId) {
            if (gestureId) {
                // gesture event generate
                Vector2 curFocusPos = focusPos, curGesturePos = gesturePos;
                for (size_t i = 0; i<clickIds.size(); ++i) {
                    if (clickIds[i] == focusId) {
                        curFocusPos = poss[i];
                    }
                    else if (clickIds[i] == gestureId){
                        curGesturePos = poss[i];
                    }
                }
                
                auto oldOffset = gesturePos - focusPos;
                auto curOffset = curGesturePos - curFocusPos;
                // check is need trigger pinch event
                float oldLength = oldOffset.length();
                float curLength = curOffset.length();
                float offsetDiff = curLength - oldLength;
                if (fabsf(offsetDiff) > FLT_EPSILON) {
                    mClickNodeMap[rootNode](ClickEvent::ePinch, Vector2(offsetDiff, 0.0f));
                }
                // check is need trigger rotation event
                double oldCosValue = oldOffset.y / oldLength;
                CLAMP(oldCosValue, -1, 1);
                double curCosValue = curOffset.y / curLength;
                CLAMP(curCosValue, -1, 1);
                float oldAngle = acos(curCosValue);
                float curAngle = acos(oldCosValue);
                float angleOffset = curAngle - oldAngle;
                if (oldOffset.x < 0 && curOffset.x < 0) {
                    angleOffset *= -1.f;
                }
                if (fabsf(angleOffset) > FLT_EPSILON) {
                    mClickNodeMap[rootNode](ClickEvent::eRotation, Vector2(-angleOffset, 0.0f));
                }
                
                focusPos = curFocusPos;
                gesturePos = curGesturePos;
            }
            else if (isClickValid == true) {
                // deal focusId drag distance
                for (size_t i = 0; i<clickIds.size(); ++i) {
                    if (clickIds[i] == focusId) {
                        Vector2 dis = poss[i] - focusPos;
                        if (dis.length() > 8) {
                            // touch moved too long, click event not valid
                            isClickValid = false;
                        }
                        break;
                    }
                }
            }
            else {
                // trigger drag event if focusId moved and update focusPos
                for (size_t i=0; i<clickIds.size(); ++i) {
                    if (clickIds[i] == focusId) {
                        focusPos = poss[i];
                        mClickNodeMap[rootNode](event, poss[i]);
                        break;
                    }
                }
            }
        }
        else if (event != ClickEvent::eDrag) {
            if (event == ClickEvent::eUp && isClickValid == true) {
                // trigger clicked event if near draged
                mClickNodeMap[rootNode](ClickEvent::eClicked, poss[0]);
            }
            // trigger other events
            mClickNodeMap[rootNode](event, poss[0]);
        }
    }

    void EventDispatcher::triggerKeyboardEvent(KeyboardEvent event, KeyCode code)
    {
        if (mKeyboardListener && mKeyboardScene) {
            mKeyboardListener(event, code);
        }
    }
    
    void EventDispatcher::sortEventNodes()
    {
        // sort all node first to decide which one trigger preferentially
        std::sort(mNodeList.begin(), mNodeList.end(), [](Node* a, Node* b) {
            Widget* wa = dynamic_cast<Widget*>(a);
            Widget* wb = dynamic_cast<Widget*>(b);
            if (wa && wb) {
                return wa->getGlobalZOrder() > wb->getGlobalZOrder();
            }
            else if (wa && !wb) {
                return true;
            }
            else { // if (!wa && wb) || if (!wa && !wb)
                return false;
            }
        });
    }
    
    void EventDispatcher::setEventHoldTimeInterval(float interval)
    {
        Peach3DAssert(interval > 0.0f, "Hold time interval must bigger than 0.0");
        if (interval > 0) {
            mEventHoldTime = interval;
        }
    }
    
    void EventDispatcher::holdSchedulerCallback(float interval)
    {
        mCurHoldTime += interval;
        // send move hold event if time enough
        if (mCurHoldTime >= mEventHoldTime) {
            if (mMovedNode) {
                mClickNodeMap[mMovedNode](ClickEvent::eMoveHold, mLastPoint);
            }
            else if (mDragNode) {
                // only clicked focus node could get hold event
                mClickNodeMap[mDragNode](ClickEvent::eDragHold, mLastPoint);
            }
            mCurHoldTime = 0.0f;
            mHoldScheduler->pause();
        }
    }
    
    void EventDispatcher::clickSchedulerCallback(float interval)
    {
        mCurClickedTime += interval;
        // pause scheduler when have waited for enough time
        if (mCurClickedTime > (mEventDClickTime)) {
            mCurClickedTime = 0.0f;
            mDClickScheduler->pause();
        }
    }
}
