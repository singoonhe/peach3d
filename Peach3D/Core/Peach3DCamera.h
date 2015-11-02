//
//  Peach3DCamera.h
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_CAMERA_H
#define PEACH3D_CAMERA_H

#include "Peach3DCompile.h"
#include "Peach3DVector3.h"
#include "Peach3DMatrix4.h"
#include "Peach3DQuaternion.h"
#include "Peach3DActionImplement.h"

namespace Peach3D
{
    class PEACH3D_DLL Camera : public ActionImplement
    {
    public:
        // return camera transform matrix
        const Matrix4& getViewMatrix();
        
        //! set camera position
        void setPosition(const Vector3& pos);
        //! get camera position
        Vector3 getPosition() {return mCameraPos;}
        //! translate camera position
        void translate(const Vector3& trans) {setPosition(mCameraPos + trans);}
        //! set camera rotation, it will effect position when lock to position
        void setRotation(const Vector3& rotation);
        //! get camera rotation
        Vector3 getRotation();
        //! rotate camera
        void rotate(const Vector3& rotate) {setRotation(mRotation + rotate);}
        //! rotate camera around x-axis
        void pitch(float radia) {rotate(Vector3(radia, 0.0f, 0.0f));}
        //! rotate camera around y-axis
        void yaw(float radia) {rotate(Vector3(0.0f, radia, 0.0f));}
        //! rotate camera around z-axis
        void roll(float radia) {rotate(Vector3(0.0f, 0.0f, radia));}
        
        //! set camera lock to look at position
        void lockToPosition(const Vector3& pos);
        //! get locked position
        Vector3 getLockedPosition() {return mLockPos;}
        //! is camera locked
        bool isLocked() {return mIsLocked;}
        //! unlock camera from mLockPos
        void unLock() {mIsLocked = false; mIsDataDirty = true;}
        
        //! get camera forward, it only valid for free camera
        Vector3 getForward();
        //! get camera up
        Vector3 getUp() {getViewMatrix(); return mCameraUp;}
        
    protected:
        //! init camera and lock to center
        Camera(const Vector3& eye, const Vector3& center, const Vector3& up);
        //! init free camera and set position/rotation
        Camera(const Vector3& pos, const Vector3& rotate);
        //! delete camera by SceneManager, user can't call destructor function.
        ~Camera() {}

    protected:
        Matrix4     mViewMatrix;    // camera transform matrix
        bool        mIsDataDirty;   // matrix will update if data is dirty
        
        Vector3     mCameraPos;     // camera position
        Vector3     mRotation;      // camera rotation
        Vector3     mLockPos;       // locked position for locked camera
        Vector3     mCameraUp;      // camera up
        Vector3     mCameraForward; // forward for free camera
        bool        mIsLocked;      // is camera locked to position(free camera or locked camera)
        
        friend class SceneManager;  // SceneManager can call destructor function
    };
}

#endif // PEACH3D_CAMERA_H
