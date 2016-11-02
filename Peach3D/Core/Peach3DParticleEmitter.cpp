//
//  Peach3DParticleEmitter.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/25/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DParticleEmitter.h"
#include "Peach3DUtils.h"
#include "Peach3DIObject.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    Emitter::~Emitter()
    {
        for (auto& point : mPoints) {
            delete point;
        }
        mPoints.clear();
    }
    
    void Emitter::start()
    {
        mRunningTime = 0.f;
        mIsRunning = true;
    }
    
    void Emitter::end()
    {
        // stop emitter
        mIsRunning = false;
    }
    
    void Emitter::update(float lastFrameTime)
    {
        if (lastFrameTime < FLT_EPSILON || !mIsRunning) {
            return;
        }
        
        mRunningTime += lastFrameTime;
        if (mRunningTime > duration && duration > 0.f) {
            // particle finish
            end();
            return;
        }
        int validCount = 0;
        // update particles, some points may be alive
        for (auto& point : mPoints) {
            if (point->lifeTime < FLT_EPSILON) {
                continue;
            }
            point->time += lastFrameTime;
            if (point->time > point->lifeTime) {
                // set current point not valid
                point->lifeTime = 0.f;
                continue;
            }
            // update current point attributes
            validCount++;
            updatePointAttributes(point, lastFrameTime);
        }
        // if need add points
        if (validCount < maxCount) {
            generatePaticles(maxCount - validCount);
        }
    }
    
    void Emitter::updatePointAttributes(ParticlePoint* point, float lastFrameTime)
    {
        double interval = lastFrameTime / point->lifeTime;
        // color
        point->color += point->lenColor * interval;
        // size
        point->size += point->lenSize * interval;
        // rotation
        point->rotate += point->lenRotate * interval;
    }
    
    void Emitter::generatePaticles(int number)
    {
        int remainNum = number;
        for (auto& point : mPoints) {
            if (remainNum == 0) {
                break;
            }
            // reset particle point data if invalid point exist
            if (point->lifeTime < FLT_EPSILON) {
                remainNum--;
                randPaticlePointAttributes(point);
            }
        }
        if (remainNum > 0) {
            for (auto i=0; i<remainNum; ++i) {
                mPoints.push_back(generateRandPaticles());
            }
        }
    }
    
    void Emitter::randPaticlePointAttributes(ParticlePoint* point)
    {
        point->time = 0.f;
        // life time
        point->lifeTime = lifeTime;
        if (lifeTimeVariance > FLT_EPSILON) {
            point->lifeTime +=  Utils::rand(-lifeTimeVariance, lifeTimeVariance);
        }
        point->lifeTime = std::max(0.f, point->lifeTime);
        // color
        point->color = startColor;
        if (startColorVariance.r > FLT_EPSILON) {
            point->color.r += Utils::rand(-startColorVariance.r, startColorVariance.r);
        }
        if (startColorVariance.g > FLT_EPSILON) {
            point->color.g += Utils::rand(-startColorVariance.g, startColorVariance.g);
        }
        if (startColorVariance.b > FLT_EPSILON) {
            point->color.b += Utils::rand(-startColorVariance.b, startColorVariance.b);
        }
        if (startColorVariance.a > FLT_EPSILON) {
            point->color.a += Utils::rand(-startColorVariance.a, startColorVariance.a);
        }
        point->color.standard();
        // end color
        auto pointEndColor = endColor;
        if (endColorVariance.r > FLT_EPSILON) {
            pointEndColor.r += Utils::rand(-endColorVariance.r, endColorVariance.r);
        }
        if (endColorVariance.g > FLT_EPSILON) {
            pointEndColor.g += Utils::rand(-endColorVariance.g, endColorVariance.g);
        }
        if (endColorVariance.b > FLT_EPSILON) {
            pointEndColor.b += Utils::rand(-endColorVariance.b, endColorVariance.b);
        }
        if (endColorVariance.a > FLT_EPSILON) {
            pointEndColor.a += Utils::rand(-endColorVariance.a, endColorVariance.a);
        }
        pointEndColor.standard();   // end color must valid, so lenColor is valid
        point->lenColor = pointEndColor - point->color;
        // size
        point->size = startSize;
        if (startSizeVariance > FLT_EPSILON) {
            point->size +=  Utils::rand(-startSizeVariance, startSizeVariance);
        }
        point->size = std::max(0.f, point->size);
        auto pointEndSize = endSize;
        if (endSizeVariance > FLT_EPSILON) {
            pointEndSize +=  Utils::rand(-endSizeVariance, endSizeVariance);
        }
        pointEndSize = std::max(0.f, pointEndSize);
        point->lenSize = pointEndSize - point->size;
        // rotation
        point->rotate = startRotate;
        if (startRotateVariance > FLT_EPSILON) {
            point->rotate +=  Utils::rand(-startRotateVariance, startRotateVariance);
        }
        auto pointEndRotate = endRotate;
        if (endRotateVariance > FLT_EPSILON) {
            pointEndRotate +=  Utils::rand(-endRotateVariance, endRotateVariance);
        }
        point->lenRotate = pointEndRotate - point->rotate;
    }
    
    /************************************** 2D particle emitter ***************************************/
    int Emitter2D::mPointStride = 0;
    
    Emitter2D::~Emitter2D()
    {
        if (mData) {
            free(mData);
            mData = nullptr;
        }
    }
    
    void Emitter2D::update(float lastFrameTime, const Vector2& curPos)
    {
        Emitter::update(lastFrameTime);
        
        if (mPointStride == 0) {
            uint vType = VertexType::Point2|VertexType::Color|VertexType::PSprite;
            const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
            // calculate position stride
            for (auto& info : infoList) {
                uint typeValue = info.type, typeSize = info.size;
                if (typeValue & vType) {
                    mPointStride += typeSize;
                }
            }
        }
        // init max count memory
        if (!mData && maxCount > 0) {
            mData = (float*)malloc(mPointStride * maxCount);
        }
        
        // copy point attributes to cache data
        int index = 0;
        const int strideFloatCount = mPointStride / sizeof(float);
        for (auto i=0; i<mPoints.size(); ++i) {
            ParticlePoint2D* point = (ParticlePoint2D*)mPoints[i];
            if (point->lifeTime > FLT_EPSILON) {
                mData[index * strideFloatCount + 0] = point->pos.x + curPos.x;
                mData[index * strideFloatCount + 1] = point->pos.y + curPos.y;

                mData[index * strideFloatCount + 2] = point->color.r;
                mData[index * strideFloatCount + 3] = point->color.g;
                mData[index * strideFloatCount + 4] = point->color.b;
                mData[index * strideFloatCount + 5] = point->color.a;
                
                mData[index * strideFloatCount + 6] = point->size;
                mData[index * strideFloatCount + 7] = point->rotate;
                index++;
            }
        }
        // current valid data size
        mDataValidSize = index * mPointStride;
    }
    
    void Emitter2D::updatePointAttributes(ParticlePoint* point, float lastFrameTime)
    {
        Emitter::updatePointAttributes(point, lastFrameTime);
        
        ParticlePoint2D* point2D = static_cast<ParticlePoint2D*>(point);
        if (emitterMode == Mode::eGravity) {
            Vector2 radial = point2D->pos;
            // radial acceleration
            radial.normalize();
            Vector2 tangential = radial;
            radial = radial * point2D->accelerate.x;
            
            // tangential acceleration
            std::swap(tangential.x, tangential.y);
            tangential = tangential * point2D->accelerate.y;
            
            // (gravity + radial + tangential) * dt
            point2D->dir = point2D->dir + (radial + tangential + gravity) * lastFrameTime;
            point2D->pos = point2D->pos + point2D->dir * lastFrameTime;
        }
        else {
            point2D->angle += point2D->rotatePerSecond * lastFrameTime;
            double interval = lastFrameTime / point->lifeTime;
            point2D->radius += point2D->lenRadius * interval;
            point2D->pos.x = -cosf(point2D->angle) * point2D->radius;
            point2D->pos.y = -sinf(point2D->angle) * point2D->radius;
        }
    }
    
    ParticlePoint* Emitter2D::generateRandPaticles()
    {
        ParticlePoint2D* point = new ParticlePoint2D();
        randPaticlePointAttributes(point);
        return point;
    }
    
    void Emitter2D::randPaticlePointAttributes(ParticlePoint* point)
    {
        Emitter::randPaticlePointAttributes(point);
        ParticlePoint2D* curPoint = static_cast<ParticlePoint2D*>(point);
        // position
        curPoint->pos = emitPos;
        if (emitPosVariance.x > FLT_EPSILON) {
            curPoint->pos.x += Utils::rand(-emitPosVariance.x, emitPosVariance.x);
        }
        if (emitPosVariance.y > FLT_EPSILON) {
            curPoint->pos.y += Utils::rand(-emitPosVariance.y, emitPosVariance.y);
        }
        if (emitterMode == Mode::eGravity) {
            // base direction
            auto finalAngle = emitAngle;
            if (emitAngleVariance > FLT_EPSILON) {
                finalAngle +=  Utils::rand(-emitAngleVariance, emitAngleVariance);
            }
            // moving speed
            auto finalSpeed = speed;
            if (speedVariance > FLT_EPSILON) {
                finalSpeed +=  Utils::rand(-speedVariance, speedVariance);
            }
            // calc direction and speed
            curPoint->dir = Vector2(cos(finalAngle), sin(finalAngle)) * finalSpeed;
            // radial and tangential accelerate
            curPoint->accelerate = accelerate;
            if (accelerateVariance.x > FLT_EPSILON) {
                curPoint->accelerate.x +=  Utils::rand(-accelerateVariance.x, accelerateVariance.x);
            }
            if (accelerateVariance.y > FLT_EPSILON) {
                curPoint->accelerate.y += Utils::rand(-accelerateVariance.y, accelerateVariance.y);
            }
        }
        else {
            // start radius
            curPoint->radius = startRadius;
            if (startRadiusVariance > FLT_EPSILON) {
                curPoint->radius +=  Utils::rand(-startRadiusVariance, startRadiusVariance);
            }
            // radius length
            float finalRadius = endRadius;
            if (endRadiusVariance > FLT_EPSILON) {
                finalRadius +=  Utils::rand(-endRadiusVariance, endRadiusVariance);
            }
            curPoint->lenRadius = finalRadius - curPoint->radius;
            // angle
            curPoint->angle = emitAngle;
            if (emitAngleVariance > FLT_EPSILON) {
                curPoint->angle +=  Utils::rand(-emitAngleVariance, emitAngleVariance);
            }
            // rotate per second
            curPoint->rotatePerSecond = rotatePerSecond;
            if (rotatePerSecondVariance > FLT_EPSILON) {
                curPoint->rotatePerSecond +=  Utils::rand(-rotatePerSecondVariance, rotatePerSecondVariance);
            }
        }
    }
}
