//
//  Peach3DPstLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/13/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DPstLoader.h"
#include "Peach3DLogPrinter.h"

namespace Peach3D
{
    bool PstLoader::pstSkeletonDataParse(uchar* orignData, ulong length, const SkeletonPtr& sk)
    {
        bool loadRes = false;
        XMLDocument readDoc;
        if (readDoc.Parse((const char*)orignData, length) == XML_SUCCESS) {
            do {
                XMLElement* rootEle = readDoc.FirstChildElement("Skeleton");
                IF_BREAK(!rootEle, "Skeleton file format error");
                
                // read bone element
                XMLElement* boneEle = rootEle->FirstChildElement();
                if (boneEle) {
                    auto rootBone = PstLoader::pstBonesDataParse(boneEle, nullptr);
                    sk->addRootBone(rootBone);
                    // set bone over, cache bones list
                    sk->addBonesOver();
                    // read animation elements
                    auto animEle = boneEle->NextSiblingElement();
                    while (animEle) {
                        PstLoader::pstAnimationDataParse(animEle, sk);
                        animEle = animEle->NextSiblingElement();
                    }
                }
                loadRes = true;
            } while (0);
        }
        return loadRes;
    }
    
    Bone* PstLoader::pstBonesDataParse(const XMLElement* boneEle, Bone* parent)
    {
        auto transformEle = boneEle->FirstChildElement();
        if (transformEle) {
            // create new Bone
            Bone* newBone = new Bone(boneEle->Attribute("name"), atoi(boneEle->Attribute("index")));
            Quaternion rotate;
            sscanf(transformEle->GetText(), "%f,%f,%f,%f", &rotate.x, &rotate.y, &rotate.z, &rotate.w);
            Vector3 translate, scale;
            auto scaleEle = transformEle->NextSiblingElement();
            sscanf(scaleEle->GetText(), "%f,%f,%f", &scale.x, &scale.y, &scale.z);
            auto translateEle = scaleEle->NextSiblingElement();
            sscanf(translateEle->GetText(), "%f,%f,%f", &translate.x, &translate.y, &translate.z);
            // calc inverse transform
            auto rotateM = Matrix4::createRotationQuaternion(rotate);
            auto scaleM = Matrix4::createScaling(scale);
            auto translateM = Matrix4::createTranslation(translate);
            Matrix4 transformMat = scaleM * rotateM * translateM;
            Matrix4 outMat;
            transformMat.getInverse(&outMat);
            newBone->setInverseTransform(outMat);
            // add new bone to parent
            if (parent) {
                parent->addChild(newBone);
            }
            // fine children
            auto childEle = transformEle->NextSiblingElement();
            while (childEle) {
                PstLoader::pstBonesDataParse(childEle, newBone);
                childEle = childEle->NextSiblingElement();
            }
            return newBone;
        }
        return nullptr;
    }
    
    void PstLoader::pstAnimationDataParse(const XMLElement* animEle, const SkeletonPtr& sk)
    {
        auto rootBoneList = sk->getRootBoneList();
        do {
            IF_BREAK(rootBoneList.size() == 0, "Root bones must be set before reading animations");
            auto animName = animEle->Attribute("name");
            IF_BREAK(!animName, "Animation name must be valid");
            sk->addAnimateTime(animName, atof(animEle->Attribute("length")));
            // read all animation
            auto frameEle = animEle->FirstChildElement();
            while (frameEle) {
                float keyTime = atof(frameEle->Attribute("time"));
                // read all bone state
                auto bonesEle = frameEle->FirstChildElement();
                while (bonesEle) {
                    auto curBone = sk->findBone(bonesEle->Attribute("name"));
                    IF_BREAK(!curBone, "Can't find bone in bones list");
                    // read keyframe transform
                    Quaternion rotate;
                    auto rotateEle = bonesEle->FirstChildElement();
                    sscanf(rotateEle->GetText(), "%f,%f,%f,%f", &rotate.x, &rotate.y, &rotate.z, &rotate.w);
                    Vector3 scale, translate;
                    auto scaleEle = rotateEle->NextSiblingElement();
                    sscanf(scaleEle->GetText(), "%f,%f,%f", &scale.x, &scale.y, &scale.z);
                    auto translateEle = scaleEle->NextSiblingElement();
                    sscanf(translateEle->GetText(), "%f,%f,%f", &translate.x, &translate.y, &translate.z);
                    BoneKeyFrame frame(keyTime, rotate, scale, translate);
                    curBone->addKeyFrame(animName, frame);
                    
                    bonesEle = bonesEle->NextSiblingElement();
                }
                frameEle = frameEle->NextSiblingElement();
            }
        } while (0);
    }
}