//
//  Peach3DTerrain.h
//  Peach3DLib
//
//  Created by singoon.he on 09/02/2017.
//  Copyright © 2017 singoon.he. All rights reserved.
//

#ifndef PEACH3D_TERRAIN_H
#define PEACH3D_TERRAIN_H

#include "Peach3DIObject.h"

namespace Peach3D
{
    class PEACH3D_DLL Terrain
    {
    public:
        static Terrain* create(int count, const float* data);
        //! query current height info for position
        float getCurrentHeight(const Vector3& pos);
        
        ObjectPtr getObject() { return mTerrainObj; }
        
    protected:
        Terrain() : mHighCount(0), mHighData(nullptr), mPerPace(0) {}
        ~Terrain();
        //! create terrain object(vertex, normal, uv)
        bool buildTerrain();
        
    private:
        int         mHighCount;     // high data cont per line
        float       mPerPace;       // distance between two vertex
        float*      mHighData;      // saved data for query current height
        ObjectPtr   mTerrainObj;
    };
}

#endif /* PEACH3D_TERRAIN_H */
