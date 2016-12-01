//
//  Peach3DEventDispatcher.h
//  Peach3DLib
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_EVENTDISPATCHER_H
#define PEACH3D_EVENTDISPATCHER_H

#include "Peach3DCompile.h"
#include "Peach3DScheduler.h"
#include "Peach3DSingleton.h"
#include "Peach3DTypes.h"

namespace Peach3D
{
    class Node;
    class Widget;
    class IScene;
    // redefine mouse and touch event listener function, it's too long
    /* Touch listener, widget clicked touch will not be added in multi-touch event */
    typedef std::function<void(ClickEvent, const Vector2&)> ClickListenerFunction;
    // redefine keyboard event listener function, it's too long
    typedef std::function<void(KeyboardEvent, KeyCode)> KeyboardListenerFunction;
    
    class PEACH3D_DLL EventDispatcher : public Singleton < EventDispatcher >
    {
    public:
        EventDispatcher();
        ~EventDispatcher();
        
        // add mouse and touch event listener
        void addClickEventListener(Node* target, ClickListenerFunction func);
        // delete touch event listener, this func will auto called when widget deleted
        void deleteClickEventListener(Node* target);
        // add keyboard event listener
        void addKeyboardListener(IScene* target, KeyboardListenerFunction func);
        // delete keyboard event listener
        void deleteKeyboardListener(IScene* target);
        
        // set event hold time interal
        void setEventHoldTimeInterval(float interval);
        // trigger mouse and touch event from any platform, clickId only one in touch event
        void triggerClickEvent(ClickEvent event, std::vector<uint> clickIds, const std::vector<Vector2>& poss);
        // trigger keyboard event from any platform(android, mac, windesk)
        /* use IPlatform::isFlagKeyDown can */
        void triggerKeyboardEvent(KeyboardEvent event, KeyCode code);
        
    protected:
        /** Sort all event node depend on zorder, put SceneNode back. */
        void sortEventNodes();
        // hold time scheduler callback
        void holdSchedulerCallback(float interval);
        // click time scheduler callback
        void clickSchedulerCallback(float interval);
        // trigger node event
        void triggerNodeEvent(Widget* rootNode, ClickEvent event, uint clickId, const Vector2& pos);
        // trigger gesture event for root widget
        void triggerGestureEvent(Widget* rootNode, ClickEvent event, std::vector<uint> clickIds, const std::vector<Vector2>& poss);
        
    private:
        std::map<Node*, ClickListenerFunction>  mClickNodeMap;      // node event map
        std::vector<Node*>                      mNodeList;          // all event node list
        IScene*                                 mKeyboardScene;
        KeyboardListenerFunction                mKeyboardListener;
        
        float       mCurClickedTime;    // two clicked event interval
        float       mEventDClickTime;   // max interal time when EventDClick trigger, default is 0.5s
        Scheduler*  mDClickScheduler;   // dclicked hold timer
        
        float       mCurHoldTime;       // EventHold will trigger if mCurHoldTime>=mEventHoldTime
        float       mEventHoldTime;     // time need hold when EventHold trigger, default is 1s
        Scheduler*  mHoldScheduler;     // event hold timer
        
        uint        mFocusClickId;      // global focus clickId, valid if value > 0
        Node*       mFocusNode;         // global clicked focus node, mouse and touch just have one node
        Node*       mDragNode;          // global drag focus node, mouse and touch just have one node
        Vector2     mLastPoint;         // global focus last event pos
        Vector2     mBeginPoint;        // global focus down pos
        Node*       mMovedNode;         // global mouse node current moved
    };
}

#endif // PEACH3D_EVENTDISPATCHER_H
