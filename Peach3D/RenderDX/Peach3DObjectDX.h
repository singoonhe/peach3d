#pragma once

#include "Peach3DCommonDX.h"
#include "Peach3DIObject.h"

using namespace Microsoft::WRL;
namespace Peach3D
{
    class ObjectDX : public IObject
    {
    public:
        ObjectDX(ComPtr<ID3D12Device> device, const char* name)
            : IObject(name), mD3DDevice(device), mRenderState(nullptr),
            mVertexBuffer(nullptr), mIndexBuffer(nullptr), mDrawMode(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
        virtual ~ObjectDX();
        /**
         * See IObject::setVertexBuffer. 
         */
        virtual bool setVertexBuffer(const void* data, uint size, uint type);
        /**
        * See IObject::setIndexBuffer.
        */
        virtual void setIndexBuffer(const void*data, uint size, IndexType type);

        /**
        * @brief Render widget list, only for GL3 and DX, called by SceneManager.
        */
        virtual void render(const std::vector<Widget*>& renderList);
        /**
        * @brief Render scene node list, only for GL3 and DX, called by SceneManager.
        */
        virtual void render(const std::vector<RenderNode*>& renderList);
        /**
        * @brief Render OBB list, called by SceneManager.
        */
        virtual void render(const std::vector<OBB*>& renderList);
        /**
        * @brief Render a AABB using object matrix, object AABB data use to scale global AABB.
        * @params attrs Object rendering attr, include matrixes.
        */
        //void renderAABB(RenderObjectAttr* attrs);
        /**
        * Delete AABB vertex buffers, it could be called by RenderDX.
        */
        static void deleteAABBBuffers();

    protected:
        /**
        * Delete dx object vertex buffer.
        */
        void cleanObjectVertexBuffer();
        /**
        * Delete dx object index buffer.
        */
        void cleanObjectIndexBuffer();
        /**
        * Generate AABB vertex buffers if SceneNode needed.
        */
        static void generateAABBBuffers(ComPtr<ID3D12Device> device, ComPtr<ID3D11DeviceContext2> context);

    private:
        ComPtr<ID3D12Device>          mD3DDevice;
        ComPtr<ID3D11DeviceContext2>   mDeviceContext;
        ID3D11Buffer*                  mVertexBuffer;
        ID3D11Buffer*                  mIndexBuffer;
        D3D11_PRIMITIVE_TOPOLOGY       mDrawMode;       // draw mode, trangles or tranglestrip
        ID3D11RasterizerState*         mRenderState;    // object render state, set depth bias

        static ID3D11Buffer*           mAABBVertexBuffer;   // vertex buffer for AABB rendering
        static ID3D11Buffer*           mAABBIndexBuffer;    // index buffer for AABB rendering
        static IProgram*               mAABBProgram;        // program for AABB rendering
    };
}