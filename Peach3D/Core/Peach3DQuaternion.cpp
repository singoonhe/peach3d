//
//  Peach3DQuaternion.cpp
//  TestPeach3D
//
//  Created by singoon.he on 12-8-19.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DQuaternion.h"

namespace Peach3D
{
    Quaternion Quaternion::createByRotateWithX(float theta)
    {
        Quaternion resQuternion;
        resQuternion.w = cos(theta*0.5f);
        resQuternion.x = sin(theta*0.5f);
        resQuternion.y = 0.0f;
        resQuternion.z = 0.0f;
        return resQuternion;
    }

    Quaternion Quaternion::createByRotateWithY(float theta)
    {
        Quaternion resQuternion;
        resQuternion.w = cos(theta*0.5f);
        resQuternion.x = 0.0f;
        resQuternion.y = sin(theta*0.5f);
        resQuternion.z = 0.0f;
        return resQuternion;
    }

    Quaternion Quaternion::createByRotateWithZ(float theta)
    {
        Quaternion resQuternion;
        resQuternion.w = cos(theta*0.5f);
        resQuternion.x = 0.0f;
        resQuternion.y = 0.0f;
        resQuternion.z = sin(theta*0.5f);
        return resQuternion;
    }

    Quaternion Quaternion::createByRotateWithAxis(const Vector3& axis, float theta)
    {
        Peach3DAssert((fabs(axis.dot(axis) - 1.0f) < FLT_EPSILON), "Rotation axis vector must be normolized!");

        Quaternion resQuternion;
        float sinThetaOver2 = sin(theta*0.5f);
        resQuternion.w = cos(theta*0.5f);
        resQuternion.x = axis.x * sinThetaOver2;
        resQuternion.y = axis.y * sinThetaOver2;
        resQuternion.z = axis.z * sinThetaOver2;
        return resQuternion;
    }
    
    Quaternion Quaternion::createByEulerAngle(const Vector3& ea)
    {
        Quaternion resQuternion;
        double c1 = cos(ea.y * 0.5f);
        double s1 = sin(ea.y * 0.5f);
        double c2 = cos(ea.z * 0.5f);
        double s2 = sin(ea.z * 0.5f);
        double c3 = cos(ea.x * 0.5f);
        double s3 = sin(ea.x * 0.5f);
        double c1c2 = c1*c2;
        double s1s2 = s1*s2;
        resQuternion.w = float(c1c2*c3 - s1s2*s3);
        resQuternion.x = float(c1c2*s3 + s1s2*c3);
        resQuternion.y = float(s1*c2*c3 + c1*s2*s3);
        resQuternion.z = float(c1*s2*c3 - s1*c2*s3);
        return resQuternion;
    }

    void Quaternion::normalize()
    {
        float mag = (float)sqrt(w*w + x*x + y*y + z*z);
        if (mag > 0.0f)
        {
            float oneOverMag = 1.0f / mag;
            w *= oneOverMag;
            x *= oneOverMag;
            y *= oneOverMag;
            z *= oneOverMag;
        }
        else
        {
            // quaternion is zero ?
            Peach3DAssert(false, "Zero quaternion can't be normalized!");
            // return the identity quaternion
            *this = QuaternionIdentity;
        }
    }

    Quaternion Quaternion::pow(float exponent)
    {
        // calculate pow of 1 always is 1
        if (fabs(w) > (1 - FLT_EPSILON))
        {
            return *this;
        }

        // calculate new alpha
        float alpha = acos(w);
        float newAlpha = alpha * exponent;

        // new quaternion
        Quaternion result;
        result.w = cos(newAlpha);
        float mult = sin(newAlpha) / sin(alpha);
        result.x = x * mult;
        result.y = x * mult;
        result.z = z * mult;
        return result;
    }

    Quaternion Quaternion::slerp(const Quaternion& dist, float t)
    {
        // check t in (0,1)
        if (t <= 0.0f)
        {
            return *this;
        }
        else if (t >= 1.0f)
        {
            return dist;
        }

        // calculate cos(theta)
        float cosOmega = dot(dist);

        Quaternion distTemp = dist;
        if (cosOmega < 0.0f)
        {
            distTemp.w = -distTemp.w;
            distTemp.x = -distTemp.x;
            distTemp.y = -distTemp.y;
            distTemp.z = -distTemp.z;
            cosOmega = -cosOmega;
        }
        // blender only make six valid float
        Peach3DAssert(cosOmega < 1.000001f, "Not valid quaternion data");

        // calculate slerp
        float k0, k1;
        if (cosOmega > (1 - FLT_EPSILON))
        {
            // near to dist
            k0 = 1.0f - t;
            k1 = t;
        }
        else
        {
            float sinOmega = sqrt(1.0f - cosOmega*cosOmega);
            float omega = atan2(sinOmega, cosOmega);
            float oneOverSinOmega = 1.0f / sinOmega;

            // calculate slerp keys
            k0 = sin((1.0f - t) * omega) * oneOverSinOmega;
            k1 = sin(t * omega) * oneOverSinOmega;
        }

        Quaternion result;
        result.x = k0*x + k1*distTemp.x;
        result.y = k0*y + k1*distTemp.y;
        result.z = k0*z + k1*distTemp.z;
        result.w = k0*w + k1*distTemp.w;
        return result;
    }

    float Quaternion::getRotationAngle() const
    {
        float thetaOver2 = 0.0f;
        if (w <= -1.0f)
        {
            thetaOver2 = PD_PI;
        }
        else if (w >= 1.0f)
        {
            thetaOver2 = 0.0f;
        }
        else
        {
            thetaOver2 = acos(w);
        }
        return thetaOver2 * 2.0f;
    }

    Vector3 Quaternion::getRotationAxis() const
    {
        // sin^2(x) + cos^2(x) = 1
        float sinThetaOver2Sq = 1.0f - w*w;
        if (sinThetaOver2Sq <= 0.0f)
        {
            return Vector3(1.0f, 0.0f, 0.0f);
        }

        // calculate 1/sin(theta/2)
        float oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);

        return Vector3(x*oneOverSinThetaOver2, y*oneOverSinThetaOver2, z*oneOverSinThetaOver2);
    }
    
    Vector3 Quaternion::getEulerAngle()
    {
        float vx, vy, vz;
        double sqw = w*w;
        double sqx = x*x;
        double sqy = y*y;
        double sqz = z*z;
        double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
        double test = x*y + z*w;
        if (test > 0.499*unit) { // singularity at north pole
            vy = 2 * atan2(x,w);
            vz = PD_PI/2;
            vx = 0;
            return Vector3(vx, vy, vz);
        }
        if (test < -0.499*unit) { // singularity at south pole
            vy = -2 * atan2(x,w);
            vz = -PD_PI/2;
            vx = 0;
            return Vector3(vx, vy, vz);
        }
        vy = (float)atan2(2*y*w-2*x*z , sqx - sqy - sqz + sqw);
        vz = (float)asin(2 * test / unit);
        vx = (float)atan2(2 * x*w - 2 * y*z, -sqx + sqy - sqz + sqw);
        return Vector3(vx, vy, vz);
    }
}