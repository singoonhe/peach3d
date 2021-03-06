﻿//
//  Peach3DCamera.cpp
//  TestPeach3D
//
//  Created by singoon.he on 12-10-14.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DCamera.h"
#include "Peach3DIPlatform.h"
#include "Peach3DSceneManager.h"

namespace Peach3D
{
    Camera::Camera(const Vector3& eye, const Vector3& center, const Vector3& up)
    {
        // init camera matrix
        mIsDataDirty = false;
        mState.isLocked = true;
        mState.lockPos = center;
        mState.up = up;
    }
    
    Camera::Camera(const Vector3& pos, const Vector3& rotate)
    {
        mIsDataDirty = false;
        mState.pos = pos;
        setRotation(rotate);
    }
    
    const Matrix4& Camera::getViewMatrix()
    {
        if (mIsDataDirty == true) {
            if (mState.isLocked) {
                // locked camera
                mViewMatrix = Matrix4::createLookAt(mState.pos, mState.lockPos, mState.up);
            }
            else {
                // free camera, z is default to downward(I don't know why?)
                auto rotation = mState.rotation;
                rotation.z += PD_PI;
                Matrix4 totate = Matrix4::createRotationPitchYawRoll(rotation);
                Matrix4 trans = Matrix4::createTranslation(-mState.pos);
                mViewMatrix = totate * trans;
                // update forward and up
                mState.up = Vector3(0, 1, 0) * totate;
                mState.forward = Vector3(0, 0, -1) * totate;
            }
            
            mIsDataDirty = false;
        }
        return mViewMatrix;
    }
    
    void Camera::setPosition(const Vector3& pos)
    {
        if (mState.pos != pos) {
            mState.pos = pos;
            calcLockCameraRotation();
            mIsDataDirty = true;
        }
    }
    
    void Camera::setRotation(const Vector3& rotation)
    {
        Vector3 changeRotate = rotation - mState.rotation;
        /*// make rotation unique
        while (mState.rotation.y > PD_PI) {
            mState.rotation.y = mState.rotation.y - 2*PD_PI;
        }
        while (mState.rotation.y < -PD_PI) {
            mState.rotation.y = mState.rotation.y + 2*PD_PI;
        }
        while (mState.rotation.z > PD_PI) {
            mState.rotation.z = mState.rotation.z - 2*PD_PI;
        }
        while (mState.rotation.z < -PD_PI) {
            mState.rotation.z = mState.rotation.z + 2*PD_PI;
        }*/
        mState.rotation = rotation;
        mIsDataDirty = true;
        
        // rotate will change camera pos for locked camera
        if (mState.isLocked) {
            // rotate sequence: Y first , X second , last Z (Z<-X<-Y)
            Matrix4 picthMat = Matrix4::createRotationX(changeRotate.x);
            Matrix4 yawMat = Matrix4::createRotationY(changeRotate.y);
            // use lock pos as original pos
            Vector3 diffV = mState.pos - mState.lockPos;
            Vector3 rotateV = diffV * picthMat * yawMat;
            mState.pos = rotateV + mState.lockPos;
            // roll control up vector
            Matrix4 rollMat = Matrix4::createRotationZ(changeRotate.z);
            mState.up = Vector3(0, mState.up.distance(Vector3Zero), 0) * rollMat;
        }
    }
    
    void Camera::lockToPosition(const Vector3& pos)
    {
        if (pos != mState.pos) {
            mState.lockPos = pos;
            mState.isLocked = true;
            calcLockCameraRotation();
            mIsDataDirty = true;
        }
    }
    
    Vector3 Camera::getForward()
    {
        if (mState.isLocked) {
            Vector3 forward = mState.lockPos - mState.pos;
            forward.normalize();
            return forward;
        }
        else {
            getViewMatrix();
            return mState.forward;
        }
    }
    
    void Camera::calcLockCameraRotation()
    {
        // position will change camera rotate(x,y) for locked camera
        if (mState.isLocked) {
            float dis = mState.pos.distance(mState.lockPos);
            if (dis > FLT_EPSILON && !FLOAT_EQUAL(dis, mState.pos.y)) {
                // x in (-90~90)
                mState.rotation.x = -asinf(mState.pos.y/dis);
                // y in (-180~180)
                mState.rotation.y = asinf(mState.pos.x/(sqrtf(dis*dis - mState.pos.y*mState.pos.y)));
                if (mState.pos.z < 0) {
                    mState.rotation.y = PD_PI - mState.rotation.y;
                }
            }
        }
    }
}
