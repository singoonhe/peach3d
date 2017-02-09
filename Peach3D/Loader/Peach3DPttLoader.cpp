//
//  Peach3DPttLoader.cpp
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#include "Peach3DPttLoader.h"

namespace Peach3D
{
    Terrain* PttLoader::pttTerrainParse(const char* file)
    {
        bool loadRes = false;
        XMLDocument readDoc;
//        if (readDoc.Parse((const char*)input.data, input.length) == XML_SUCCESS) {
//            do {
//                XMLElement* rootEle = readDoc.FirstChildElement("Skeleton");
//                IF_BREAK(!rootEle, "Skeleton file format error");
//                
//                // read bone element
//                XMLElement* boneEle = rootEle->FirstChildElement();
//                if (boneEle) {
//                    auto rootBone = PstLoader::pstBonesDataParse(boneEle, nullptr);
//                    SkeletonPtr* sk = static_cast<SkeletonPtr*>(input.handler);
//                    (*sk)->addRootBone(rootBone);
//                    // set bone over, cache bones list
//                    (*sk)->addBonesOver();
//                    // read animation elements
//                    auto animEle = boneEle->NextSiblingElement();
//                    while (animEle) {
//                        PstLoader::pstAnimationDataParse(animEle, (*sk));
//                        animEle = animEle->NextSiblingElement();
//                    }
//                }
//                loadRes = true;
//            } while (0);
//        }
        return loadRes ? nullptr : nullptr;
    }
}
