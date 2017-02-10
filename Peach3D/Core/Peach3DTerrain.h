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
        /**
         * @brief Create a pace of terrain
         * @params count Height count per line.
         * @params pace Distance between two vertex.
         * @params data Height data buffer.
         * @params uvdata Vertex UV data buffer.
         * @params texl Texture list of terrain.
         */
        static Terrain* create(int count, float pace, const float* data, const uint* uvdata, const std::vector<TexturePtr>& texl);
        //! query current height info for position
        float getCurrentHeight(const Vector3& pos);
        
        void setPosition(const Vector3& pos) { mTerrainPos = pos; }
        const Vector3& getPosition() { return mTerrainPos; }
        
        void setName(const std::string& name) { mName = name; }
        ObjectPtr getObject() { return mTerrainObj; }
        
    protected:
        Terrain(int count, float pace, const float* data);
        ~Terrain();
        //! create terrain object(vertex, normal, uv)
        void buildTerrain(const uint* uvdata, const std::vector<TexturePtr>& texl);
        
    private:
        int         mHighCount;     // high data cont per line
        float       mPerPace;       // distance between two vertex
        float*      mHighData;      // saved data for query current height
        
        std::string mName;
        ObjectPtr   mTerrainObj;
        Vector3     mTerrainPos;    // terrain current position
    };
}

#endif /* PEACH3D_TERRAIN_H */
