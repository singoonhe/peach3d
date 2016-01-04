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

namespace Peach3D
{
    // define vertex type, vertex data sequence must like this
#define VertexTypePosition2     0x0001  // vertex position is float2
#define VertexTypePosition3     0x0002  // vertex position is float3, ePosition2 will be discard
#define VertexTypeColor4        0x0004  // vertex have color, float4
#define VertexTypeNormal        0x0008  // vertex have normal, float3
#define VertexTypePSize         0x0010  // vertex have point size, usually be used for particle
#define VertexTypeUV            0x0020  // vertex have UV coord
#define VertexTypeMatrix        0x0040  // model matrix, only for gl3 draw instance

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
        //! Set this object use program. If tihs not be set, system will auto choose program.
        virtual void useProgramForRender(IProgram* program);

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
        
        //! get object material
        //! object material is template, only mesh loader could modify.
        //! modify scene node material to control displayed object
        virtual const Material& getObjectMaterial()const { return mObjectMtl; }
        //! set object material, only mesh loader should call it
        virtual void setMaterial(const Material& mtl) { mObjectMtl = mtl; }
        //! add texture to object material, this must be called before node attached
        virtual void addTextureToMaterial(ITexture* texture);
        //! set index texture to uv scroll, should be call after addTextureToMaterial
        //! user can set this index to enable uv scroll
        virtual void useIndexTextureToScroll(uint index);
        
        /**
         * @brief Render widget list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<Widget*>& renderList) = 0;
        /**
         * @brief Render scene node list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<RenderNode*>& renderList) = 0;
        
        /**
         * Return object AABB, it should be called after "setVertexBuffer".
         * @params transform Base AABB transform matrix, it will recalc AABB.
         */
        AABB getAABB(const Matrix4& transform);
        
        /** Reture object name. */
        const std::string& getName() { return mObjectName; }

    protected:
        //! create object by IRender, user can't call constructor function.
        IObject(const char* name);
        //! delete object by IRender, user can't call destructor function.
        virtual ~IObject() {}

    protected:
        std::string    mObjectName;        // name of object
        IProgram*      mRenderProgram;     // current used program's name, will be discard
        bool           mIsPresetProgram;   // is preset program used, will be discard
        Material       mObjectMtl;         // object material, scene node template material

        uint           mIndexBufferSize;   // index buffer byte count
        uint           mVertexBufferSize;  // vertex buffer byte count
        uint           mVertexDataStride;  // vertex data format stride
        
        IndexType      mIndexDataType;     // index data type, short or int
        uint           mVertexDataType;    // vertex data format type
        AABB           mObjectAABB;        // object AABB
        static Color4  mAABBColor;         // AABB render color

        friend class   IRender;
    };
}

#endif // PEACH3D_IOBJECT_H
