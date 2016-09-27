//
//  Peach3DPstLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 5/13/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PSTLOADER_H
#define PEACH3D_PSTLOADER_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DTypes.h"
#include "Peach3DSkeleton.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;
namespace Peach3D
{
    class PstLoader
    {
    public:
        /* *.pst is Peach3D Skeleton Text file. */
        static void* pstSkeletonDataParse(const ResourceLoaderInput& input);

    private:
        /* Read bones data from skeleton file. */
        static Bone* pstBonesDataParse(const XMLElement* boneEle, Bone* parent);
        /* Read a animation data from skeleton file. */
        static void pstAnimationDataParse(const XMLElement* animEle, const SkeletonPtr& sk);
    };
}

#endif /* PEACH3D_PSTLOADER_H */
