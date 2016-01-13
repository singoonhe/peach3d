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
#include "Peach3DAABB.h"
#include "Peach3DOBB.h"

namespace Peach3D
{
    // define vertex type, use combined
    namespace VertexType
    {
        const uint Point2 = 0x0001;   // vertex position is float2
        const uint Point3 = 0x0002;   // vertex position is float3, ePosition2 will be discard
        const uint Normal = 0x0004;   // vertex have normal, float3
        const uint PSize  = 0x0008;   // vertex have point size, usually be used for particle
        const uint UV     = 0x0010;   // vertex have UV coord
    }

    // define index type
    enum class PEACH3D_DLL IndexType
    {
        eUShort,    // index data use short, only 16 bit per index
        eUInt       // index data use int, 32 bit per index
    };
    
    class Widget;
    class RenderNode;
    class PEACH3D_DLL IObject
    {
    public:
        /** Set template program, all using current Object Node will use current program first. */
        void setProgram(IProgram* program) { mObjectProgram = program; }
        IProgram* getProgram() { return mObjectProgram; }

        /**
        * @brief Set vertex buffer data for render object.
        * @params data Data of vertex buffer.
        * @params size Bit size of data.
        * @params type Vertex data type, must be one type of VertexDataType or combine with "|".
        */
        virtual bool setVertexBuffer(const void* data, uint size, uint type);
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
        
        /**
         * @brief Render widget list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<Widget*>& renderList) = 0;
        /**
         * @brief Render scene node list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<RenderNode*>& renderList) = 0;
        /**
         * @brief Render OBB list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<OBB*>& renderList) = 0;
        
        /**
         * Return object AABB, it should be called after "setVertexBuffer".
         * @params transform Base AABB transform matrix, it will recalc AABB.
         */
        AABB getAABB(const Matrix4& transform);
        
        const std::string& getName() { return mObjectName; }
        const Vector3& getBorderMax() { return mBorderMax; }
        const Vector3& getBorderMin() { return mBorderMin; }

    protected:
        //! create object by IRender, user can't call constructor function.
        IObject(const char* name);
        //! delete object by IRender, user can't call destructor function.
        virtual ~IObject() {}

    protected:
        std::string    mObjectName;        // name of object
        IProgram*      mObjectProgram;     // current used template program
        Material       mObjectMtl;         // object material, scene node template material

        uint           mIndexBufferSize;   // index buffer byte count
        uint           mVertexBufferSize;  // vertex buffer byte count
        uint           mVertexDataStride;  // vertex data format stride
        
        IndexType      mIndexDataType;     // index data type, short or int
        uint           mVertexDataType;    // vertex data format type
        Vector3        mBorderMax;         // object base border max
        Vector3        mBorderMin;         // object base border min
        AABB           mObjectAABB;        // object AABB
        static Color4  mAABBColor;         // AABB render color

        friend class   IRender;
    };
}

#endif // PEACH3D_IOBJECT_H
