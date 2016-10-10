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
        // color
        float timeInterval = point->lifeTime - point->time;
        auto disColor = (point->endColor - point->color) / timeInterval;
        point->color += disColor * lastFrameTime;
        // size
        auto disSize = (point->endSize - point->size) / timeInterval;
        point->size += disSize * lastFrameTime;
        // rotation
        auto disRotate = (point->endRotate - point->rotate) / timeInterval;
        point->rotate += disRotate * lastFrameTime;
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
        point->endColor = endColor;
        if (endColorVariance.r > FLT_EPSILON) {
            point->endColor.r += Utils::rand(-endColorVariance.r, endColorVariance.r);
        }
        if (endColorVariance.g > FLT_EPSILON) {
            point->
            endColor.g += Utils::rand(-endColorVariance.g, endColorVariance.g);
        }
        if (endColorVariance.b > FLT_EPSILON) {
            point->endColor.b += Utils::rand(-endColorVariance.b, endColorVariance.b);
        }
        if (endColorVariance.a > FLT_EPSILON) {
            point->endColor.a += Utils::rand(-endColorVariance.a, endColorVariance.a);
        }
        point->endColor.standard();
        // size
        point->size = startSize;
        if (startSizeVariance > FLT_EPSILON) {
            point->size +=  Utils::rand(-startSizeVariance, startSizeVariance);
        }
        point->size = std::max(0.f, point->size);
        point->endSize = endSize;
        if (endSizeVariance > FLT_EPSILON) {
            point->endSize +=  Utils::rand(-endSizeVariance, endSizeVariance);
        }
        point->endSize = std::max(0.f, point->endSize);
        // rotation
        point->rotate = startRotate;
        if (startRotateVariance > FLT_EPSILON) {
            point->rotate +=  Utils::rand(-startRotateVariance, startRotateVariance);
        }
        point->endRotate = endRotate;
        if (endRotateVariance > FLT_EPSILON) {
            point->endRotate +=  Utils::rand(-endRotateVariance, endRotateVariance);
        }
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
            mData = (float*)malloc(mPointStride * maxCount * sizeof(float));
        }
        
        // copy point attributes to cache data
        int index = 0;
        for (auto i=0; i<mPoints.size(); ++i) {
            ParticlePoint2D* point = (ParticlePoint2D*)mPoints[i];
            if (point->lifeTime > FLT_EPSILON) {
                mData[index * mPointStride + 0] = point->pos.x + curPos.x;
                mData[index * mPointStride + 1] = point->pos.y + curPos.y;
                
                mData[index * mPointStride + 2] = point->color.r;
                mData[index * mPointStride + 3] = point->color.g;
                mData[index * mPointStride + 4] = point->color.b;
                mData[index * mPointStride + 5] = point->color.a;
                
                mData[index * mPointStride + 6] = point->size;
                mData[index * mPointStride + 7] = point->rotate;
                index++;
            }
        }
        // current valid data size
        mDataValidSize = index * mPointStride;
    }
    
    void Emitter2D::updatePointAttributes(ParticlePoint* point, float lastFrameTime)
    {
        Emitter::updatePointAttributes(point, lastFrameTime);
        
        // position
        ParticlePoint2D* point2D = static_cast<ParticlePoint2D*>(point);
        point2D->pos.x = point2D->dir.x * point2D->time + gravity.x * point2D->time * point2D->time * 0.5f;
        point2D->pos.y = point2D->dir.y * point2D->time + gravity.y * point2D->time * point2D->time * 0.5f;
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
    }
}
