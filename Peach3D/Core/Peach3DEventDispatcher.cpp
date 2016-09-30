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
        
        Widget* const rootNode = sceneMgr->getRootWidget();
        std::vector<uint>    rootNodeIds;
        std::vector<Vector2> rootNodePoss;
        // save last clicked node for double clicked event
        static Node* lastClickedNode = nullptr;
        
        // start deal widget event
        for (uint i=0; i<clickIds.size(); ++i) {
            // sign is current click event dealed
            bool isClickDealed = false;
            bool isEventStrike = false;
            
            SceneNode*  sceneEventNode = nullptr;
            bool        isCheckNode = false;
            for (auto& node : mNodeList) {
                SceneNode* csNode = dynamic_cast<SceneNode*>(node);
                Widget* cwNode = dynamic_cast<Widget*>(node);
                // calc the pos target SceneNode if need
                if (csNode && !isCheckNode) {
                    sceneEventNode = SceneManager::getSingleton().getWindowClickedNode(poss[i]);
                    isCheckNode = true;
                }
                
                // pos in widget or in SceneNode
                if (rootNode != node && ((cwNode && cwNode->isPointInZone(poss[i])) || (csNode && node == sceneEventNode))) {
                    isClickDealed = true;
                    // get current focus clickId
                    if (!mFocusClickId) {
                        mFocusClickId = clickIds[i];
                    }
                    
                    if (mFocusClickId == clickIds[i]) {
                        // save last point
                        mLastPoint = poss[i];
                        std::vector<Vector2> eventPoss = {mLastPoint};
                        
                        // deal with mouse move event
                        if (event == ClickEvent::eMoved) {
                            mClickNodeMap[node](event, eventPoss);
                            if (mMovedNode != node) {
                                if (mMovedNode) {
                                    mClickNodeMap[mMovedNode](ClickEvent::eMoveOut, eventPoss);
                                }
                                mClickNodeMap[node](ClickEvent::eMoveIn, eventPoss);
                                mMovedNode = node;
                            }
                            // start hold scheduler
                            mHoldScheduler->start();
                            mCurHoldTime = 0.0f;
                        }
                        // deal with mouse down or touch down
                        else if (event == ClickEvent::eDown) {
                            // release move node if it exist
                            mMovedNode = nullptr;
                            mBeginPoint = mLastPoint;
                            // set focus node
                            mFocusNode = node;
                            mDragNode = node;
                            mClickNodeMap[node](event, eventPoss);
                            // start hold scheduler
                            mHoldScheduler->start();
                            mCurHoldTime = 0.0f;
                        }
                        else if (event == ClickEvent::eUp) {
                            mClickNodeMap[node](event, eventPoss);
                            // trigger click event
                            if (mFocusNode == node) {
                                mClickNodeMap[node](ClickEvent::eClicked, eventPoss);
                                if (mCurClickedTime < FLT_EPSILON || mCurClickedTime > mEventDClickTime) {
                                    lastClickedNode = node;
                                    // start click time for double clicked event
                                    mCurClickedTime = 0.0f;
                                    mDClickScheduler->start();
                                }
                                else if (mCurClickedTime < mEventDClickTime && mCurClickedTime > FLT_EPSILON && lastClickedNode == node) {
                                    lastClickedNode = nullptr;
                                    mClickNodeMap[node](ClickEvent::eDClicked, eventPoss);
                                    // pause click time for double clicked event
                                    mCurClickedTime = 0.0f;
                                    mDClickScheduler->pause();
                                }
                            }
                            else if (mFocusNode) {
                                // send event when up in other node. There is no drag event if quickly on IOS.
                                mClickNodeMap[mFocusNode](ClickEvent::eCancel, eventPoss);
                                mFocusNode = nullptr;
                            }
                        }
                        else if (event == ClickEvent::eDrag) {
                            // set scroll view to clicked node if button not swallow and drag a distance
                            if (isEventStrike) {
                                mFocusNode = node;
                            }
                            // deal dragIn and dragOut event
                            if (mDragNode != node) {
                                if (mDragNode) {
                                    mClickNodeMap[mDragNode](ClickEvent::eDragOut, eventPoss);
                                }
                                mClickNodeMap[node](ClickEvent::eDragIn, eventPoss);
                                mDragNode = node;
                            }
                            if (mFocusNode) {
                                // drag event always send to click focus node
                                mClickNodeMap[mFocusNode](event, eventPoss);
                                // node not swallow event, pass event to parent node if drag for a distence
                                Vector2 dis = mLastPoint - mBeginPoint;
                                if (dis.length() > 8) {
                                    if (!mFocusNode->isSwallowEvents()) {
                                        // transmit event to next widget if node not swallowed
                                        mClickNodeMap[mFocusNode](ClickEvent::eCancel, eventPoss);
                                        isEventStrike = true;
                                        isClickDealed = false;
                                    }
                                    // start hold scheduler, hold event only valid when drag in clicked focus node
                                    else if (mFocusNode == node) {
                                        mBeginPoint = mLastPoint;
                                        mHoldScheduler->start();
                                        mCurHoldTime = 0.0f;
                                    }
                                }
                            }
                        }
                        else if (event == ClickEvent::eScrollWheel) {
                            // just trigger scroll wheel event
                            mClickNodeMap[node](event, eventPoss);
                        }
                        else if (event == ClickEvent::eCancel) {
                            mClickNodeMap[node](event, eventPoss);
                            // release focus node
                            mFocusNode = nullptr;
                        }
                    }
                    
                    // release gesture click when up or cancel in GUI widget (Sometimes, click drag from root widget to GUI widget).
                    if ((event == ClickEvent::eCancel || event == ClickEvent::eUp) && mClickNodeMap.find(rootNode)!=mClickNodeMap.end()) {
                        std::vector<uint>    rootCancelIds = {clickIds[i]};
                        std::vector<Vector2> rootCancelPoss = {poss[i]};
                        triggerGestureEvent(ClickEvent::eCancel, rootCancelIds, rootCancelPoss);
                    }
                }
                else if (rootNode == node) {
                    // just add clicked pos to root node, this may need trigger gesture
                    rootNodeIds.push_back(clickIds[i]);
                    rootNodePoss.push_back(poss[i]);
                }
                // break if event had been dealed
                if (isClickDealed) {
                    break;
                }
            }
            
            if (mFocusClickId == clickIds[i]) {
                // release focus clickId if last click up
                if ((event == ClickEvent::eUp || event == ClickEvent::eCancel)) {
                    // release focus node no matter where Up event trigger
                    mFocusClickId = 0;
                    mFocusNode = nullptr;
                    mDragNode = nullptr;
                }
                
                std::vector<Vector2> eventPoss = {mLastPoint};
                if (!isClickDealed && mMovedNode) {
                    // set node move out and release moved node
                    mClickNodeMap[mMovedNode](ClickEvent::eMoveOut, eventPoss);
                    mMovedNode = nullptr;
                    // not hold node, pause scheduler
                    mCurHoldTime = 0.0f;
                    mHoldScheduler->pause();
                }
                if (!isClickDealed && mDragNode) {
                    mClickNodeMap[mDragNode](ClickEvent::eDragOut, eventPoss);
                    mDragNode = nullptr;
                    // not hold node pause sheduler
                    mCurHoldTime = 0.0f;
                    mHoldScheduler->pause();
                }
            }
        }
        
        // trigger multi-touch for root widget
        if (rootNodePoss.size() > 0 && rootNodeIds.size() > 0) {
            triggerGestureEvent(event, rootNodeIds, rootNodePoss);
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
                
                float oldDis = gesturePos.distance(focusPos);
                float curDis = curGesturePos.distance(curFocusPos);
                if (!FLOAT_EQUAL(oldDis, curDis)) {
                    std::vector<Vector2> pinchPoss;
                    pinchPoss.push_back(Vector2(curDis - oldDis, 0.0f));
                    mClickNodeMap[rootNode](ClickEvent::ePinch, pinchPoss);
                    
                    // trigger rotation event, x is two figures rotation
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
                        std::vector<Vector2> pinchPoss;
                        pinchPoss.push_back(poss[i]);
                        mClickNodeMap[rootNode](event, pinchPoss);
                        break;
                    }
                }
            }
        }
        else if (event != ClickEvent::eDrag) {
            if (event == ClickEvent::eUp && isClickValid == true) {
                // trigger clicked event if near draged
                mClickNodeMap[rootNode](ClickEvent::eClicked, poss);
            }
            // trigger other events
            mClickNodeMap[rootNode](event, poss);
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
        std::vector<Vector2> eventPoss = {mLastPoint};
        // send move hold event if time enough
        if (mCurHoldTime >= mEventHoldTime) {
            if (mMovedNode) {
                mClickNodeMap[mMovedNode](ClickEvent::eMoveHold, eventPoss);
            }
            else if (mDragNode) {
                // only clicked focus node could get hold event
                mClickNodeMap[mDragNode](ClickEvent::eDragHold, eventPoss);
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
