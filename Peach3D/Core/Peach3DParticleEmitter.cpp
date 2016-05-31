//
//  Peach3DParticleEmitter.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/25/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DParticleEmitter.h"
#include "Peach3DUtils.h"

namespace Peach3D
{
    void Emitter::start()
    {
        mValidCount = 0;
        mRunningTime = 0.f;
        mPoints.clear();
        mIsRunning = true;
    }
    
    void Emitter::update(float lastFrameTime)
    {
        if (lastFrameTime < FLT_EPSILON) {
            return;
        }
        if (!mIsRunning && mValidCount == 0) {
            return;
        }
        
        mRunningTime += lastFrameTime;
        if (mRunningTime > duration && duration > 0.f) {
            // particle finish
            end();
        }
        // update particles, some points may be alive
        for (ParticlePoint& point : mPoints) {
            point.time += lastFrameTime;
            if (point.time > point.lifeTime) {
                // set current point not valid
                point.lifeTime = 0.f;
                mValidCount--;
                continue;
            }
            // color
            float timeInterval = point.lifeTime - point.time;
            auto disColor = (point.endColor - point.color) / timeInterval;
            point.color += disColor * lastFrameTime;
            // size
            auto disSize = (point.endSize - point.size) / timeInterval;
            point.size += disSize * lastFrameTime;
            // rotation
            auto disRotate = (point.endRotate - point.rotate) / timeInterval;
            point.rotate += disRotate * lastFrameTime;
            
            updatePointAttributes(point, lastFrameTime);
        }
        // if need add points
        if (mIsRunning && mValidCount < maxCount) {
            if (mRunningTime > 1.f) {
                generatePaticles(maxCount - mValidCount);
            }
            else {
                generatePaticles(mRunningTime * lastFrameTime - mValidCount);
            }
        }
    }
    
    void Emitter::end()
    {
        // stop emitter
        mIsRunning = false;
    }
    
    void Emitter::generatePaticles(int number)
    {
        int remainNum = number;
        for (ParticlePoint& point : mPoints) {
            if (remainNum == 0) {
                break;
            }
            if (point.lifeTime < FLT_EPSILON) {
                remainNum--;
                point = generateRandPaticles();
            }
        }
        if (remainNum > 0) {
            for (auto i=0; i<remainNum; ++i) {
                mPoints.push_back(generateRandPaticles());
            }
        }
    }
    
    ParticlePoint Emitter::generateRandPaticles()
    {
        mValidCount++;
        
        ParticlePoint point;
        // life time
        point.lifeTime = lifeTime;
        if (lifeVariance > FLT_EPSILON) {
            point.lifeTime +=  Utils::rand(-lifeVariance, lifeVariance);
        }
        // color
        point.color = startColor;
        if (startColorVariance.r > FLT_EPSILON) {
            point.color.r += Utils::rand(-startColorVariance.r, startColorVariance.r);
        }
        if (startColorVariance.g > FLT_EPSILON) {
            point.color.g += Utils::rand(-startColorVariance.g, startColorVariance.g);
        }
        if (startColorVariance.b > FLT_EPSILON) {
            point.color.b += Utils::rand(-startColorVariance.b, startColorVariance.b);
        }
        if (startColorVariance.a > FLT_EPSILON) {
            point.color.a += Utils::rand(-startColorVariance.a, startColorVariance.a);
        }
        point.color.standard();
        // end color
        point.endColor = endColor;
        if (endColorVariance.r > FLT_EPSILON) {
            point.endColor.r += Utils::rand(-endColorVariance.r, endColorVariance.r);
        }
        if (endColorVariance.g > FLT_EPSILON) {
            point.endColor.g += Utils::rand(-endColorVariance.g, endColorVariance.g);
        }
        if (endColorVariance.b > FLT_EPSILON) {
            point.endColor.b += Utils::rand(-endColorVariance.b, endColorVariance.b);
        }
        if (endColorVariance.a > FLT_EPSILON) {
            point.endColor.a += Utils::rand(-endColorVariance.a, endColorVariance.a);
        }
        point.endColor.standard();
        // size
        point.size = startSize;
        if (startSizeVariance > FLT_EPSILON) {
            point.size +=  Utils::rand(-startSizeVariance, startSizeVariance);
        }
        point.endSize = endSize;
        if (endSizeVariance > FLT_EPSILON) {
            point.endSize +=  Utils::rand(-endSizeVariance, endSizeVariance);
        }
        // rotation
        point.rotate = startRotate;
        if (startRotateVariance > FLT_EPSILON) {
            point.rotate +=  Utils::rand(-startRotateVariance, startRotateVariance);
        }
        point.endRotate = endRotate;
        if (endRotateVariance > FLT_EPSILON) {
            point.endRotate +=  Utils::rand(-endRotateVariance, endRotateVariance);
        }
        return point;
    }
    
    void Emitter2D::updatePointAttributes(ParticlePoint& point, float lastFrameTime)
    {
        ParticlePoint2D& point2D = (ParticlePoint2D&)point;
        // position
        point2D.pos = point2D.pos + point2D.dir * lastFrameTime;
    }
    
    ParticlePoint Emitter2D::generateRandPaticles()
    {
        ParticlePoint2D point;
        point.copy(Emitter::generateRandPaticles());
        // position
        point.pos = emitPos;
        if (emitPosVariance.x > FLT_EPSILON) {
            point.pos.x += Utils::rand(-emitPosVariance.x, emitPosVariance.x);
        }
        if (emitPosVariance.y > FLT_EPSILON) {
            point.pos.y += Utils::rand(-emitPosVariance.y, emitPosVariance.y);
        }
        // direction
        float finalAngle = emitAngle;
        if (emitAngleVariance > FLT_EPSILON) {
            finalAngle +=  Utils::rand(-emitAngleVariance, emitAngleVariance);
        }
        point.dir = Vector2(cos(DEGREE_TO_RADIANS(finalAngle)), sin(DEGREE_TO_RADIANS(finalAngle)));
        return point;
    }
}