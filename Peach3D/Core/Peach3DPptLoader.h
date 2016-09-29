//
//  Peach3DPptLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 5/15/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PPTLOADER_H
#define PEACH3D_PPTLOADER_H

#include "Peach3DCompile.h"
#include "Peach3DTypes.h"
#include "Peach3DParticle.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;
namespace Peach3D
{
    class PptLoader
    {
    public:
        /* *.ppt is Peach3D Particle Text file(2d and 3d particle). */
        static void* pptParticleDataParse(const ResourceLoaderInput& input);
        
    private:
        /* Read 2d emitter data. */
        static void emitter2DDataParse(XMLElement* emitterEle, Particle2D* handler);
        /* Read 3d emitter data. */
        static void emitter3DDataParse(XMLElement* emitterEle, Particle3D* handler);
    };
}

#endif /* PEACH3D_PPTLOADER_H */
