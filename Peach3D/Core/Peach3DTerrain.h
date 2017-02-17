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
#include "Peach3DLight.h"

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
        static Terrain* create(int width, int height, float pace, const float* data, const uint* uvdata, const std::vector<TexturePtr>& texl);
        //! query current height info for position
        float getCurrentHeight(const Vector3& pos);
        
        void setPosition(const Vector3& pos) { mTerrainPos = pos; }
        const Vector3& getPosition() { return mTerrainPos; }
        /** Change render program before rendering. */
        void prepareForRender(float lastFrameTime);
        
        /** Set node draw mode, Points/Lines/Trangles. */
        void setDrawMode(DrawMode mode) { mDrawMode = mode; }
        DrawMode getDrawMode() {return mDrawMode;}
        /** Set is terrain will show shadow. */
        void setAcceptShadow(bool enable) { mAcceptShadow = enable; setLightingStateNeedUpdate();}
        bool isAcceptShadow() { return mAcceptShadow; }
        /** Auto set terrain lighting enable. */
        void setLightingEnabled(bool enable) { mLightEnable = enable; setLightingStateNeedUpdate();}
        bool isLightingEnabled() { return mLightEnable; }
        /** Lights used will be update before render, called when lights changed. */
        void setLightingStateNeedUpdate() { mIsLightingDirty = true; }
        
        void setName(const std::string& name) { mName = name; }
        ObjectPtr getObject() { return mTerrainObj; }
        const std::vector<TexturePtr>& getBrushes() { return mBrushes; }
        const std::vector<TexturePtr>& getAlphaMaps() { return mAlphaMap; }
        const std::vector<LightPtr>& getRenderLights() { return mRenderLights; }
        const std::vector<LightPtr>& getShadowLights() { return mShadowLights; }
        ProgramPtr getProgramForRender()const {return mRenderProgram;}
        
    protected:
        Terrain(int width, int height, float pace, const float* data);
        ~Terrain();
        //! create terrain object(vertex, normal, uv)
        void buildTerrain(const uint* uvdata, const std::vector<TexturePtr>& texl);
        
    private:
        int         mWidthCount;    // width vertex count
        int         mHeightCount;   // height vertex count
        float       mPerPace;       // distance between two vertex
        float*      mHighData;      // saved data for query current height
        
        std::string mName;
        DrawMode    mDrawMode;
        ProgramPtr  mRenderProgram; // render program
        ObjectPtr   mTerrainObj;
        Vector3     mTerrainPos;    // terrain current position
        Vector3     mTerrainLength; // terrain x and z length
        std::vector<TexturePtr> mBrushes;   // terrain brush textures
        std::vector<TexturePtr> mAlphaMap;  // terrain alpha map textures
        
        bool        mAcceptShadow;  // is terrain accept shadow
        bool        mLightEnable;   // is lighting enabled, default is true
        bool        mIsLightingDirty;   // is lighting state need update
        std::vector<std::string> mIgnoreLights; // need ignore lights
        std::vector<LightPtr>   mRenderLights;  // valid lights
        std::vector<LightPtr>   mShadowLights;  // valid shadow lights, setting by parent
    };
}

#endif /* PEACH3D_TERRAIN_H */
