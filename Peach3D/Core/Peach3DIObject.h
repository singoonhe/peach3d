//
//  Peach3DIObject.h
//  TestPeach3D
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_IOBJECT_H
#define PEACH3D_IOBJECT_H

#include "Peach3DCompile.h"
#include "Peach3DIProgram.h"
#include "Peach3DColor4.h"
#include "Peach3DVector3.h"
#include "Peach3DMatrix4.h"
#include "Peach3DMaterial.h"
#include "Peach3DOBB.h"

namespace Peach3D
{
    // define vertex type, use combined
    namespace VertexType
    {
        const uint Point2   = 0x0001;   // vertex position is float2
        const uint Point3   = 0x0002;   // vertex position is float3, ePosition2 will be ignore
        const uint Color    = 0x0004;   // vertex have color(float4), used for particle
        const uint Normal   = 0x0008;   // vertex have normal, float3
        const uint PSprite  = 0x0010;   // vertex have point size and rotation, usually be used for particle
        const uint UV       = 0x0020;   // vertex have UV coord
        const uint BWidget  = 0x0040;   // vertex have bone widget data, float4
        const uint BIndex   = 0x0080;   // vertex have bone index data, float4
    }

    // define index type
    enum class PEACH3D_DLL IndexType
    {
        eUShort,    // index data use short, only 16 bit per index
        eUInt       // index data use int, 32 bit per index
    };
    
    class Widget;
    class RenderNode;
    class Particle2D;
    class Particle3D;
    class PEACH3D_DLL IObject
    {
    public:
        /**
         * @params name Object itself name.
         * @params meshName Mesh name, (object+mesh) is object unique name.
        */
        IObject(const char* name, const char* meshName);
        virtual ~IObject() {}
        
        /** Set template program, all using current Object Node will use current program first. */
        void setProgram(const ProgramPtr& program) { mObjectProgram = program; }
        const ProgramPtr& getProgram() { return mObjectProgram; }

        /**
        * @brief Set vertex buffer data for render object.
        * @params data Data of vertex buffer.
        * @params size Bit size of data.
        * @params type Vertex data type, must be one type of VertexDataType or combine with "|".
        * @params isDynamic Is vertex buffer alway need modify.
        */
        virtual bool setVertexBuffer(const void* data, uint size, uint type, bool isDynamic = false);
        /**
        * @brief Set index buffer data for drawing triangles.
        *  Triangles also will be drawed with array if index buffer isn't existed.
        * @params data Data of index buffer.
        * @params size Bit size of data.
        * @params type Index data type, sizeof(USHORT) or sizeof(UINT) per index data.
        */
        virtual void setIndexBuffer(const void*data, uint size, IndexType type = IndexType::eUShort);
        
        /** Set object template material, modify RenderNode material for each node. */
        void setMaterial(const Material& mtl) { mObjectMtl = mtl; }
        const Material& getMaterial()const { return mObjectMtl; }
        
        /** Set object used bones, used for skeleton animation. */
        void setUsedBones(const std::vector<std::string>& list) { mUsedBones = list; }
        const std::vector<std::string>& getUsedBones()const { return mUsedBones; }
        
        /**
         * @brief Render widget list, called by SceneManager.
         */
        virtual void render(const std::vector<Widget*>& renderList) = 0;
        /**
         * @brief Render scene node list, called by SceneManager.
         */
        virtual void render(const std::vector<RenderNode*>& renderList) = 0;
        /**
         * @brief Render OBB list, called by SceneManager.
         */
        virtual void render(const std::vector<OBB*>& renderList) = 0;
        /**
         * @brief Render Particle2D, called by SceneManager.
         */
        virtual void render(Particle2D* particle) = 0;
        /**
         * @brief Render Particle3D, called by SceneManager.
         */
        virtual void render(Particle3D* particle) = 0;
        
        const std::string& getName() { return mObjectName; }
        const std::string& getUniqueName() { return mUniqueName; }
        const Vector3& getBorderMax() { return mBorderMax; }
        const Vector3& getBorderMin() { return mBorderMin; }
        uint getVertexType() { return mVertexDataType; }

    protected:
        std::string    mObjectName;        // name of object
        std::string    mUniqueName;        // unique object name, important for render hash
        ProgramPtr     mObjectProgram;     // current used template program
        Material       mObjectMtl;         // object material, scene node template material

        uint           mIndexBufferSize;   // index buffer byte count
        uint           mVertexBufferSize;  // vertex buffer byte count
        uint           mVertexDataStride;  // vertex data format stride
        
        IndexType      mIndexDataType;     // index data type, short or int
        uint           mVertexDataType;    // vertex data format type
        Vector3        mBorderMax;         // object base border max
        Vector3        mBorderMin;         // object base border min
        
        std::vector<std::string>    mUsedBones; // used bones name list for animation
    };
    
    // make shared object simple
    using ObjectPtr = std::shared_ptr<IObject>;
}

#endif // PEACH3D_IOBJECT_H
