//
//  Peach3DAction.h
//  Peach3DLib
//
//  Created by singoon.he on 5/5/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_ACTION_H
#define PEACH3D_ACTION_H

#include "Peach3DVector3.h"
#include "Peach3DQuaternion.h"
#include "Peach3DActionImplement.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    class PEACH3D_DLL IAction
    {
    public:
        virtual void update(ActionImplement* target, float lastFrameTime) = 0;
        bool isActionFinished() {return mIsFinished;}
        void setAdded() {mIsAdded = true;}
        bool isAdded() {return mIsAdded;}
        /** Make Action enabled again, used for Repeat. */
        virtual void rebirth() {mIsFinished = false;}
        virtual ~IAction() {}
        
    protected:
        IAction() : mIsFinished(false), mIsAdded(false) {}
        
    protected:
        bool mIsFinished;   // sign is Action finished
        bool mIsAdded;      // sign is added to ActionImplement, avoid a Action add twice
    };
    
    /************************************** Base Actions ***************************************/
    
    class PEACH3D_DLL Spawn : public IAction
    {
    public:
        // Mix all actions update at the same time, must be end with "NULL".
        static Spawn* createList(IAction* oneAction, ...);
        void update(ActionImplement* target, float lastFrameTime);
        virtual ~Spawn();
        
    private:
        Spawn(const std::vector<IAction*>& list) : mActionList(list) {}
        
        std::vector<IAction*>   mActionList;
    };
    
    class PEACH3D_DLL Repeat : public IAction
    {
    public:
        static Repeat* createForever(IAction* oneAction) { return Repeat::createListForever(oneAction, NULL); }
        // Repeat forever list actions, must be end with "NULL".
        static Repeat* createListForever(IAction* oneAction, ...);
        static Repeat* create(int count, IAction* oneAction) { return Repeat::createList(count, oneAction, NULL); }
        // Repeat times list actions, must be end with "NULL".
        static Repeat* createList(int count, ...);
        void update(ActionImplement* target, float lastFrameTime);
        virtual ~Repeat();
        
    private:
        Repeat(const std::vector<IAction*>& list, int count) : mActionList(list), mRepeatCount(count), mCurIndex(0) {}
        
        int                     mRepeatCount;   // action list repeat count, -1 for forever
        int                     mCurIndex;      // current updated Action index in llist
        std::vector<IAction*>   mActionList;
    };
    
    class PEACH3D_DLL Visible : public IAction
    {
    public:
        static Visible* create(bool isVisible) { return new Visible(isVisible); }
        virtual void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        Visible(bool isVisible) : mIsVisible(isVisible) {}
        bool mIsVisible;
    };
    
    class PEACH3D_DLL CallFunc : public IAction
    {
    public:
        static CallFunc* create(const std::function<void()>& callback) { return new CallFunc(callback); }
        virtual void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        CallFunc(const std::function<void()>& callback) : mCallBackFunc(callback) {}
        std::function<void()> mCallBackFunc;
    };
    
    class PEACH3D_DLL Delay : public IAction
    {
    public:
        static Delay* create(float time) { return new Delay(time); }
        virtual void update(ActionImplement* target, float lastFrameTime);
        virtual void rebirth() {IAction::rebirth(); mCurTime = 0.0f;}
        
    protected:
        Delay(float time) : mTotleTime(time), mCurTime(0.0f) {}
        
    protected:
        float   mTotleTime;
        float   mCurTime;
    };
    
    /************************************** 2D Actions ***************************************/
    
    class PEACH3D_DLL FadeIn : public Delay
    {
    public:
        static FadeIn* create(float finalAlpha, float time) { return new FadeIn(finalAlpha, time); }
        virtual void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        FadeIn(float finalAlpha, float time) : Delay(time), mFinalAlpha(finalAlpha) {}
        
    protected:
        float mFinalAlpha;
    };
    
    // the same as FadeIn
    class PEACH3D_DLL FadeOut : public FadeIn
    {
    public:
        static FadeOut* create(float finalAlpha, float time) { return new FadeOut(finalAlpha, time); }
        
    protected:
        FadeOut(float finalAlpha, float time) : FadeIn(finalAlpha, time) {}
    };
    
    class PEACH3D_DLL MoveBy2D : public Delay
    {
    public:
        static MoveBy2D* create(const Vector2& moveDis, float time) { return new MoveBy2D(moveDis, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        MoveBy2D(const Vector2& moveDis, float time) : Delay(time), mMoveDis(moveDis) {}
        
        Vector2 mMoveDis;
    };
    
    class PEACH3D_DLL MoveTo2D : public Delay
    {
    public:
        static MoveTo2D* create(const Vector2& desPos, float time) { return new MoveTo2D(desPos, time); }
        void update(ActionImplement* target, float lastFrameTime);

    protected:
        MoveTo2D(const Vector2& desPos, float time) : Delay(time), mDesPos(desPos) {}

        Vector2 mDesPos;
    };
    
    class PEACH3D_DLL Bezier2D : public Delay
    {
    public:
        static Bezier2D* create(const Vector2& desPos, const std::vector<Vector2>& ctrlPoss, float time) {
            return new Bezier2D(desPos, ctrlPoss, time);
        }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        Bezier2D(const Vector2& desPos, const std::vector<Vector2>& ctrlPoss, float time) : Delay(time),
            mControlPoss(ctrlPoss), mStartPosValid(false) { mControlPoss.push_back(desPos); }
        
        bool                    mStartPosValid;
        std::vector<Vector2>    mControlPoss;
    };
    
    class PEACH3D_DLL ScaleBy2D : public Delay
    {
    public:
        static ScaleBy2D* create(const Vector2& scaleDis, float time) { return new ScaleBy2D(scaleDis, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        ScaleBy2D(const Vector2& scaleDis, float time) : Delay(time), mScaleDis(scaleDis) {}
        
        Vector2 mScaleDis;
    };
    
    class PEACH3D_DLL ScaleTo2D : public Delay
    {
    public:
        static ScaleTo2D* create(const Vector2& desScale, float time) { return new ScaleTo2D(desScale, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        ScaleTo2D(const Vector2& desScale, float time) : Delay(time), mDesScale(desScale) {}
        
        Vector2 mDesScale;
    };

    class PEACH3D_DLL RotateBy2D : public Delay
    {
    public:
        static RotateBy2D* create(float rotateDis, float time) { return new RotateBy2D(rotateDis, time); }
        void update(ActionImplement* target, float lastFrameTime);

    protected:
        RotateBy2D(float rotateDis, float time) : Delay(time), mRotateDis(rotateDis) {}

        float mRotateDis;
    };

    class PEACH3D_DLL RotateTo2D : public Delay
    {
    public:
        static RotateTo2D* create(float desRotate, float time) { return new RotateTo2D(desRotate, time); }
        void update(ActionImplement* target, float lastFrameTime);

    protected:
        RotateTo2D(float desRotate, float time) : Delay(time), mDesRotate(desRotate) {}

        float mDesRotate;
    };
    
    class PEACH3D_DLL Frame2D : public IAction
    {
    public:
        static Frame2D* create(const std::vector<TextureFrame>& fList, float interval) { return new Frame2D(fList, interval); }
        void update(ActionImplement* target, float lastFrameTime);
        void rebirth();
        
    protected:
        Frame2D(const std::vector<TextureFrame>& fList, float interval) : mFrameInterval(interval), mCurTime(0.0f),
            mFrameIndex(0), mTarget(nullptr), mFrameList(fList) {}
        
        float   mCurTime;
        float   mFrameInterval;
        size_t  mFrameIndex;
        ActionImplement*            mTarget;
        std::vector<TextureFrame>   mFrameList;
    };
    
    /************************************** 3D Actions ***************************************/
    
    // used for SceneNode and Camera
    class PEACH3D_DLL MoveBy3D : public Delay
    {
    public:
        static MoveBy3D* create(const Vector3& moveDis, float time) { return new MoveBy3D(moveDis, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        MoveBy3D(const Vector3& moveDis, float time) : Delay(time), mMoveDis(moveDis) {}
        
        Vector3 mMoveDis;
    };
    
    // used for SceneNode and Camera
    class PEACH3D_DLL MoveTo3D : public Delay
    {
    public:
        static MoveTo3D* create(const Vector3& desPos, float time) { return new MoveTo3D(desPos, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        MoveTo3D(const Vector3& desPos, float time) : Delay(time), mDesPos(desPos) {}
        
        Vector3 mDesPos;
    };
    
    class PEACH3D_DLL Bezier3D : public Delay
    {
    public:
        static Bezier3D* create(const Vector3& desPos, const std::vector<Vector3>& ctrlPoss, float time) {
            return new Bezier3D(desPos, ctrlPoss, time);
        }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        Bezier3D(const Vector3& desPos, const std::vector<Vector3>& ctrlPoss, float time) : Delay(time),
            mControlPoss(ctrlPoss), mStartPosValid(false) { mControlPoss.push_back(desPos); }
        
        bool                    mStartPosValid;
        std::vector<Vector3>    mControlPoss;
    };
    
    class PEACH3D_DLL ScaleBy3D : public Delay
    {
    public:
        static ScaleBy3D* create(const Vector3& scaleDis, float time) { return new ScaleBy3D(scaleDis, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        ScaleBy3D(const Vector3& scaleDis, float time) : Delay(time), mScaleDis(scaleDis) {}
        
        Vector3 mScaleDis;
    };
    
    class PEACH3D_DLL ScaleTo3D : public Delay
    {
    public:
        static ScaleTo3D* create(const Vector3& desScale, float time) { return new ScaleTo3D(desScale, time); }
        void update(ActionImplement* target, float lastFrameTime);
        
    protected:
        ScaleTo3D(const Vector3& desScale, float time) : Delay(time), mDesScale(desScale) {}
        
        Vector3 mDesScale;
    };

    /** Two modes(Eular or Quaternion) for SceneNode and Camera rotate action. */
    class PEACH3D_DLL RotateBy3D : public Delay
    {
    public:
        static RotateBy3D* create(const Vector3& rotateDis, float time, bool usEular = true) { return new RotateBy3D(rotateDis, time, usEular); }
        void update(ActionImplement* target, float lastFrameTime);
        /** Set des quaternion invalid. */
        virtual void rebirth() {Delay::rebirth(); mDesQuat.w = FLT_MAX;}

    protected:
        RotateBy3D(const Vector3& rotateDis, float time, bool usEular) : Delay(time), mRotateDis(rotateDis),
            mIsUseEular(usEular) { mDesQuat.w = FLT_MAX; }
        
        Quaternion  mStartQuat;
        Quaternion  mDesQuat;
        bool        mIsUseEular;
        Vector3     mRotateDis;
    };
    
    /** Two modes(Eular or Quaternion) for SceneNode and Camera rotate action. */
    class PEACH3D_DLL RotateTo3D : public Delay
    {
    public:
        static RotateTo3D* create(const Vector3& desRotate, float time, bool usEular = true) { return new RotateTo3D(desRotate, time, usEular); }
        void update(ActionImplement* target, float lastFrameTime);
        /** Set des quaternion invalid. */
        virtual void rebirth() {Delay::rebirth(); mDesQuat.w = FLT_MAX;}

    protected:
        RotateTo3D(const Vector3& desRotate, float time, bool usEular) : Delay(time), mDesRotate(desRotate),
            mIsUseEular(usEular) { mDesQuat.w = FLT_MAX; }
        
        Quaternion  mStartQuat;
        Quaternion  mDesQuat;
        bool        mIsUseEular;
        Vector3     mDesRotate;
    };
}

#endif /* defined(PEACH3D_ACTION_H) */
