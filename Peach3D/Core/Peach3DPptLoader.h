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
#include "Peach3DVector2.h"
#include "Peach3DVector3.h"
#include "Peach3DParticle.h"
#include "tinyxml2/tinyxml2.h"

using namespace tinyxml2;
namespace Peach3D
{
    class Pp2Loader
    {
    public:
        /* *.pp2 is Peach3D 2D Particle Text file. */
        static bool pptParticleDataParse(uchar* orignData, ulong length, const char* dir, const ParticlePtr& pp);
        
    private:
        /* Read object data, include vertex/index/material. */
//        static void objDataParse(const XMLElement* objEle, const char* dir, const ParticlePtr& pp);
//        /* Read object vertex data. */
//        static const XMLElement* objVertexDataParse(const XMLElement* prevEle, uint verType, const ObjectPtr& obj);
//        /* Read object index data. */
//        static const XMLElement* objIndexDataParse(const XMLElement* prevEle, const ObjectPtr& obj);
//        /* Read object material data. */
//        static const XMLElement* objMaterialDataParse(const XMLElement* prevEle, const char* dir, const ObjectPtr& obj);
    };
    
    class Pp3Loader
    {
    public:
        /* *.pp3 is Peach3D 3D Particle Text file. */
        static bool pptParticleDataParse(uchar* orignData, ulong length, const char* dir, const ParticlePtr& pp);
        
    private:
        /* Read object data, include vertex/index/material. */
        //        static void objDataParse(const XMLElement* objEle, const char* dir, const ParticlePtr& pp);
        //        /* Read object vertex data. */
        //        static const XMLElement* objVertexDataParse(const XMLElement* prevEle, uint verType, const ObjectPtr& obj);
        //        /* Read object index data. */
        //        static const XMLElement* objIndexDataParse(const XMLElement* prevEle, const ObjectPtr& obj);
        //        /* Read object material data. */
        //        static const XMLElement* objMaterialDataParse(const XMLElement* prevEle, const char* dir, const ObjectPtr& obj);
    };
}

#endif /* PEACH3D_PPTLOADER_H */
