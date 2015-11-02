//
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
        mIsLocked = true;
        mLockPos = center;
        mCameraUp = up;
    }
    
    Camera::Camera(const Vector3& pos, const Vector3& rotate)
    {
        mIsDataDirty = false;
        mCameraPos = pos;
        setRotation(rotate);
    }
    
    const Matrix4& Camera::getViewMatrix()
    {
        if (mIsDataDirty == true) {
            if (mIsLocked) {
                // locked camera
                mViewMatrix = Matrix4::createLookAt(mCameraPos, mLockPos, mCameraUp);
                
                // position will change camera rotate(x,y) for locked camera
                if (mIsLocked) {
                    float dis = mCameraPos.distance(mLockPos);
                    if (dis > FLT_EPSILON && !FLOAT_EQUAL(dis, mCameraPos.y)) {
                        // x in (-90~90)
                        mRotation.x = -asinf(mCameraPos.y/dis);
                        // y in (-180~180)
                        mRotation.y = asinf(mCameraPos.x/(sqrtf(dis*dis - mCameraPos.y*mCameraPos.y)));
                        if (mCameraPos.z < 0) {
                            mRotation.y = PD_PI - mRotation.y;
                        }
                    }
                }
            }
            else {
                // free camera, z is default to downward(I don't know why?)
                Matrix4 totate = Matrix4::createRotationPitchYawRoll(mRotation.x, mRotation.y, PD_PI + mRotation.z);
                Matrix4 trans = Matrix4::createTranslation(-mCameraPos.x, -mCameraPos.y, -mCameraPos.z);
                mViewMatrix = totate * trans;
                // update forward and up
                mCameraUp = Vector3(0, 1, 0) * totate;
                mCameraForward = Vector3(0, 0, -1) * totate;
            }
            
            mIsDataDirty = false;
        }
        return mViewMatrix;
    }
    
    void Camera::setPosition(const Vector3& pos)
    {
        if (mCameraPos != pos) {
            mCameraPos = pos;
            mIsDataDirty = true;
        }
    }
    
    void Camera::setRotation(const Vector3& rotation)
    {
        // make rotation unique
        while (mRotation.y > PD_PI) {
            mRotation.y = mRotation.y - 2*PD_PI;
        }
        while (mRotation.y < -PD_PI) {
            mRotation.y = mRotation.y + 2*PD_PI;
        }
        while (mRotation.z > PD_PI) {
            mRotation.z = mRotation.z - 2*PD_PI;
        }
        while (mRotation.z < -PD_PI) {
            mRotation.z = mRotation.z + 2*PD_PI;
        }
        mRotation = rotation;
        mIsDataDirty = true;
        
        // rotate will change camera pos for locked camera
        if (mIsLocked) {
            // rotate sequence: Y first , X second , last Z (Z<-X<-Y)
            Matrix4 picthMat = Matrix4::createRotationX(mRotation.x);
            Matrix4 yawMat = Matrix4::createRotationY(mRotation.y);
            mCameraPos = Vector3(0, 0, mCameraPos.distance(mLockPos)) * picthMat * yawMat;
            // roll control up vector
            Matrix4 rollMat = Matrix4::createRotationZ(mRotation.z);
            mCameraUp = Vector3(0, mCameraUp.distance(Vector3Zero), 0) * rollMat;
        }
    }
    
    Vector3 Camera::getRotation()
    {
        if (mIsLocked) {
            getViewMatrix();
        }
        return mRotation;
    }
    
    void Camera::lockToPosition(const Vector3& pos)
    {
        if (pos != mCameraPos) {
            mLockPos = pos;
            mIsLocked = true;
            mIsDataDirty = true;
        }
    }
    
    Vector3 Camera::getForward()
    {
        if (mIsLocked) {
            Vector3 forward = mLockPos - mCameraPos;
            forward.normalize();
            return forward;
        }
        else {
            getViewMatrix();
            return mCameraForward;
        }
    }
}
