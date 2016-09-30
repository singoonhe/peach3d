//
//  Peach3DAction.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/5/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DAction.h"
#include "Peach3DSprite.h"
#include "Peach3DSceneNode.h"
#include "Peach3DCamera.h"
#if (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX)
#include <stdarg.h>
#endif

namespace Peach3D
{
    // define add an action to list macro
#define ADD_ACTION_TO_LIST(actionList, oneAction) \
    Peach3DAssert(oneAction && !oneAction->isAdded(), "One Action can't added twice or not end list with NULL!"); \
    if (oneAction && !oneAction->isAdded()) {   \
        actionList.push_back(oneAction);    \
        oneAction->setAdded();  \
    }
    
    /************************************** Base Actions *************************************/
    
    Spawn* Spawn::createList(IAction* oneAction, ...)
    {
        std::vector<IAction*> actionList;
        ADD_ACTION_TO_LIST(actionList, oneAction)
        va_list arg_ptr;
        va_start(arg_ptr, oneAction);
        IAction* newAction = va_arg(arg_ptr, IAction*);
        while (newAction) {
            ADD_ACTION_TO_LIST(actionList, newAction)
            newAction = va_arg(arg_ptr, IAction*);
        }
        va_end(arg_ptr);
        
        Spawn* action = new Spawn(actionList);
        return action;
    }
    
    void Spawn::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished) {
            bool isAllFinished = true;
            // update all actions in list
            for (auto& inAction : mActionList) {
                if (!inAction->isActionFinished()) {
                    inAction->update(target, lastFrameTime);
                    if (!inAction->isActionFinished()) {
                        isAllFinished = false;
                    }
                }
            }
            mIsFinished = isAllFinished;
        }
    }
    
    Spawn::~Spawn()
    {
        for (auto& inAction : mActionList) {
            delete inAction;
        }
        mActionList.clear();
    }
    
    Repeat*  Repeat::createListForever(IAction* oneAction, ...)
    {
        std::vector<IAction*> actionList;
        ADD_ACTION_TO_LIST(actionList, oneAction)
        va_list arg_ptr;
        va_start(arg_ptr, oneAction);
        IAction* newAction = va_arg(arg_ptr, IAction*);
        while (newAction) {
            ADD_ACTION_TO_LIST(actionList, newAction)
            newAction = va_arg(arg_ptr, IAction*);
        }
        va_end(arg_ptr);
        
        Repeat* action = new Repeat(actionList, -1);
        return action;
    }
    
    Repeat* Repeat::createList(int count, ...)
    {
        std::vector<IAction*> actionList;
        va_list arg_ptr;
        va_start(arg_ptr, count);
        IAction* newAction = va_arg(arg_ptr, IAction*);
        while (newAction) {
            ADD_ACTION_TO_LIST(actionList, newAction)
            newAction = va_arg(arg_ptr, IAction*);
        }
        va_end(arg_ptr);
        Repeat* action = new Repeat(actionList, count);
        return action;
    }
    
    void Repeat::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished) {
            IAction* curAction = mActionList[mCurIndex];
            curAction->update(target, lastFrameTime);
            if (curAction->isActionFinished()) {
                mCurIndex = mCurIndex + 1;
                // update first action
                if (mCurIndex == (int)mActionList.size()) {
                    mCurIndex = 0;
                    mRepeatCount--;
                }
                mActionList[mCurIndex]->rebirth();
            }
            if (mRepeatCount == 0) {
                mIsFinished = true;
            }
        }
    }
    
    Repeat::~Repeat()
    {
        for (auto& inAction : mActionList) {
            delete inAction;
        }
        mActionList.clear();
    }
    
    void Visible::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished) {
            Node* tnode = dynamic_cast<Node*>(target);
            if (tnode) {
                tnode->setVisible(mIsVisible);
            }
            mIsFinished = true;
        }
    }
    
    void CallFunc::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished && mCallBackFunc) {
            mCallBackFunc();
            mIsFinished = true;
        }
    }
    
    void Delay::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished && mTotleTime > 0.0f) {
            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                mIsFinished = true;
            }
        }
    }
    
    void FadeIn::update(ActionImplement* target, float lastFrameTime)
    {
        Node* nNode = dynamic_cast<Node*>(target);
        if (!mIsFinished && nNode && mTotleTime > 0.0f) {
            float curAlpha = nNode->getAlpha();
            float nomalDis = (mFinalAlpha - curAlpha) / (mTotleTime - mCurTime);
            curAlpha = curAlpha + nomalDis * lastFrameTime;
            
            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                curAlpha = mFinalAlpha;
                mIsFinished = true;
            }
            nNode->setAlpha(curAlpha);
        }
    }
    
    /************************************** 2D Actions ***************************************/
    
    void MoveBy2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            mCurTime += lastFrameTime;
            Vector2 curPos = wNode->getPosition();
            if (mCurTime >= mTotleTime) {
                curPos = curPos + mMoveDis * ((mTotleTime - mCurTime + lastFrameTime) / mTotleTime);
                mIsFinished = true;
            }
            else {
                curPos = curPos + mMoveDis * (lastFrameTime / mTotleTime);
            }
            wNode->setPosition(curPos);
        }
    }
        
    void MoveTo2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            Vector2 curPos = wNode->getPosition();
            Vector2 nomalDis = (mDesPos - curPos) / (mTotleTime - mCurTime);
            curPos = curPos + nomalDis * lastFrameTime;

            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                curPos = mDesPos;
                mIsFinished = true;
            }
            wNode->setPosition(curPos);
        }
    }
    
    void Bezier2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            if (!mStartPosValid) {
                mControlPoss.insert(mControlPoss.begin(), wNode->getPosition());
                mStartPosValid = true;
            }
            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                wNode->setPosition(mControlPoss.back());
                mIsFinished = true;
            }
            else {
                std::vector<Vector2> tempPoss = mControlPoss;
                size_t i = tempPoss.size() - 1;
                while (i > 0) {
                    for (size_t j = 0; j < i; j++) {
                        tempPoss[j] = tempPoss[j] + (tempPoss[j+1] - tempPoss[j]) * (mCurTime / mTotleTime);
                    }
                    i--;
                }
                wNode->setPosition(tempPoss[0]);
            }
        }
    }
    
    void ScaleBy2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            mCurTime += lastFrameTime;
            Vector2 curScale = wNode->getScale();
            if (mCurTime >= mTotleTime) {
                curScale = curScale + mScaleDis * ((mTotleTime - mCurTime + lastFrameTime) / mTotleTime);
                mIsFinished = true;
            }
            else {
                curScale = curScale + mScaleDis * (lastFrameTime / mTotleTime);
            }
            wNode->setScale(curScale);
        }
    }
        
    void ScaleTo2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            Vector2 curScale = wNode->getScale();
            Vector2 nomalDis = (mDesScale - curScale) / (mTotleTime - mCurTime);
            curScale = curScale + nomalDis * lastFrameTime;
            
            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                curScale = mDesScale;
                mIsFinished = true;
            }
            wNode->setScale(curScale);
        }
    }

    void RotateBy2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            mCurTime += lastFrameTime;
            float curRotate = wNode->getRotation();
            if (mCurTime >= mTotleTime) {
                curRotate = curRotate + mRotateDis * ((mTotleTime - mCurTime + lastFrameTime) / mTotleTime);
                mIsFinished = true;
            }
            else {
                curRotate = curRotate + mRotateDis * (lastFrameTime / mTotleTime);
            }
            wNode->setRotation(curRotate);
        }
    }

    void RotateTo2D::update(ActionImplement* target, float lastFrameTime)
    {
        Widget* wNode = dynamic_cast<Widget*>(target);
        if (!mIsFinished && wNode && mTotleTime > 0.0f) {
            float curRotate = wNode->getRotation();
            float nomalDis = (mDesRotate - curRotate) / (mTotleTime - mCurTime);
            curRotate = curRotate + nomalDis * lastFrameTime;

            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                curRotate = mDesRotate;
                mIsFinished = true;
            }
            wNode->setRotation(curRotate);
        }
    }
    
    void Frame2D::update(ActionImplement* target, float lastFrameTime)
    {
        Sprite* sNode = dynamic_cast<Sprite*>(target);
        if (!mIsFinished && sNode) {
            mCurTime += lastFrameTime;
            if (mCurTime >= mFrameInterval) {
                if (mFrameIndex < (mFrameList.size() - 1)) {
                    mFrameIndex ++;
                    sNode->setTextureFrame(mFrameList[mFrameIndex]);
                    mCurTime -= mFrameInterval;
                }
                else {
                    mIsFinished = true;
                }
            }
            
            // save target node
            if (!mTarget) {
                mTarget = target;
            }
        }
    }
    
    void Frame2D::rebirth()
    {
        IAction::rebirth();
        mCurTime = 0.0f;
        mFrameIndex = 0;
        
        // reset first texture frame
        Sprite* sNode = dynamic_cast<Sprite*>(mTarget);
        if (sNode) {
            sNode->setTextureFrame(mFrameList[mFrameIndex]);
        }
    }
    
    /************************************** 3D Actions ***************************************/
    
    void MoveBy3D::update(ActionImplement* target, float lastFrameTime)
    {
        SceneNode* sNode = dynamic_cast<SceneNode*>(target);
        Camera* cNode = dynamic_cast<Camera*>(target);
        if (!mIsFinished && (sNode || cNode) && mTotleTime > 0.0f) {
            mCurTime += lastFrameTime;
            Vector3 curPos = sNode ? sNode->getPosition() : cNode->getPosition();
            if (mCurTime >= mTotleTime) {
                curPos = curPos + mMoveDis * ((mTotleTime - mCurTime + lastFrameTime) / mTotleTime);
                mIsFinished = true;
            }
            else {
                curPos = curPos + mMoveDis * (lastFrameTime / mTotleTime);
            }
            sNode ? sNode->setPosition(curPos) : cNode->setPosition(curPos);
        }
    }
        
    void MoveTo3D::update(ActionImplement* target, float lastFrameTime)
    {
        SceneNode* sNode = dynamic_cast<SceneNode*>(target);
        Camera* cNode = dynamic_cast<Camera*>(target);
        if (!mIsFinished && (sNode || cNode) && mTotleTime > 0.0f) {
            Vector3 curPos = sNode ? sNode->getPosition() : cNode->getPosition();
            Vector3 nomalDis = (mDesPos - curPos) / (mTotleTime - mCurTime);
            curPos = curPos + nomalDis * lastFrameTime;

            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                curPos = mDesPos;
                mIsFinished = true;
            }
            sNode ? sNode->setPosition(curPos) : cNode->setPosition(curPos);
        }
    }
    
    void Bezier3D::update(ActionImplement* target, float lastFrameTime)
    {
        SceneNode* sNode = dynamic_cast<SceneNode*>(target);
        Camera* cNode = dynamic_cast<Camera*>(target);
        if (!mIsFinished && (sNode || cNode) && mTotleTime > 0.0f) {
            if (!mStartPosValid) {
                mControlPoss.insert(mControlPoss.begin(), sNode ? sNode->getPosition() : cNode->getPosition());
                mStartPosValid = true;
            }
            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                sNode ? sNode->setPosition(mControlPoss.back()) : cNode->setPosition(mControlPoss.back());
                mIsFinished = true;
            }
            else {
                std::vector<Vector3> tempPoss = mControlPoss;
                size_t i = tempPoss.size() - 1;
                while (i > 0) {
                    for (size_t j = 0; j < i; j++) {
                        tempPoss[j] = tempPoss[j] + (tempPoss[j+1] - tempPoss[j]) * (mCurTime / mTotleTime);
                    }
                    i--;
                }
                sNode ? sNode->setPosition(tempPoss[0]) : cNode->setPosition(tempPoss[0]);
            }
        }
    }
    
    void ScaleBy3D::update(ActionImplement* target, float lastFrameTime)
    {
        SceneNode* sNode = dynamic_cast<SceneNode*>(target);
        if (!mIsFinished && sNode && mTotleTime > 0.0f) {
            mCurTime += lastFrameTime;
            Vector3 curScale = sNode->getScale();
            if (mCurTime >= mTotleTime) {
                curScale = curScale + mScaleDis * ((mTotleTime - mCurTime + lastFrameTime) / mTotleTime);
                mIsFinished = true;
            }
            else {
                curScale = curScale + mScaleDis * (lastFrameTime / mTotleTime);
            }
            sNode->setScale(curScale);
        }
    }
        
    void ScaleTo3D::update(ActionImplement* target, float lastFrameTime)
    {
        SceneNode* sNode = dynamic_cast<SceneNode*>(target);
        if (!mIsFinished && sNode && mTotleTime > 0.0f) {
            Vector3 curScale = sNode->getScale();
            Vector3 nomalDis = (mDesScale - curScale) / (mTotleTime - mCurTime);
            curScale = curScale + nomalDis * lastFrameTime;
            
            mCurTime += lastFrameTime;
            if (mCurTime >= mTotleTime) {
                curScale = mDesScale;
                mIsFinished = true;
            }
            sNode->setScale(curScale);
        }
    }

    void RotateBy3D::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished && mTotleTime > 0.0f) {
            SceneNode* sNode = dynamic_cast<SceneNode*>(target);
            Camera* cNode = dynamic_cast<Camera*>(target);
            if (sNode && !mIsUseEular) {
                if (mDesQuat.w >= (FLT_MAX - FLT_EPSILON)) {
                    mStartQuat = Quaternion::createByEulerAngle(sNode->getRotation());
                    mDesQuat = Quaternion::createByEulerAngle(sNode->getRotation() + mRotateDis);
                }
                mCurTime += lastFrameTime;
                if (mCurTime >= mTotleTime) {
                    sNode->setRotateQuaternion(mDesQuat);
                    mIsFinished = true;
                }
                else {
                    Quaternion middleQuat = mStartQuat.slerp(mDesQuat, mCurTime / mTotleTime);
                    sNode->setRotateQuaternion(middleQuat);
                }
            }
            else if (cNode || sNode) {
                mCurTime += lastFrameTime;
                Vector3 curRotate = sNode ? sNode->getRotation() : cNode->getRotation();
                if (mCurTime >= mTotleTime) {
                    curRotate = curRotate + mRotateDis * ((mTotleTime - mCurTime + lastFrameTime) / mTotleTime);
                    mIsFinished = true;
                }
                else {
                    curRotate = curRotate + mRotateDis * (lastFrameTime / mTotleTime);
                }
                sNode ? sNode->setRotation(curRotate) : cNode->setRotation(curRotate);
            }
        }
    }

    void RotateTo3D::update(ActionImplement* target, float lastFrameTime)
    {
        if (!mIsFinished && mTotleTime > 0.0f) {
            SceneNode* sNode = dynamic_cast<SceneNode*>(target);
            Camera* cNode = dynamic_cast<Camera*>(target);
            if (sNode && !mIsUseEular) {
                if (mDesQuat.w >= (FLT_MAX - FLT_EPSILON)) {
                    mStartQuat = Quaternion::createByEulerAngle(sNode->getRotation());
                    mDesQuat = Quaternion::createByEulerAngle(mDesRotate);
                }
                mCurTime += lastFrameTime;
                if (mCurTime >= mTotleTime) {
                    sNode->setRotateQuaternion(mDesQuat);
                    mIsFinished = true;
                }
                else {
                    sNode->setRotateQuaternion(mStartQuat.slerp(mDesQuat, mCurTime / mTotleTime));
                }
            }
            else if (cNode || sNode) {
                Vector3 curRotate = sNode ? sNode->getRotation() : cNode->getRotation();
                Vector3 nomalDis = (mDesRotate - curRotate) / (mTotleTime - mCurTime);
                curRotate = curRotate + nomalDis * lastFrameTime;
                
                mCurTime += lastFrameTime;
                if (mCurTime >= mTotleTime) {
                    curRotate = mDesRotate;
                    mIsFinished = true;
                }
                sNode ? sNode->setRotation(curRotate) : cNode->setRotation(curRotate);
            }
        }
    }
}
