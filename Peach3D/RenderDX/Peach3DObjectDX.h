#pragma once

#include "Peach3DCommonDX.h"
#include "Peach3DIObject.h"

using namespace Microsoft::WRL;
namespace Peach3D
{
    class ObjectDX : public IObject
    {
        //! declare class RenderDX is friend class, so RenderDX can call constructor function.
        friend class RenderDX;
    public:
        /**
         * See IObject::setVertexBuffer. 
         */
        virtual bool setVertexBuffer(const void* data, uint size, uint type);
        /**
        * See IObject::setIndexBuffer.
        */
        virtual void setIndexBuffer(const void*data, uint size, IndexType type);

        //! ebable buffers for render
        virtual void bindBaseAttrBuffer(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime);
        /**
        * See IObject::render.
        */
        virtual void render(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime);
		/**
		* @brief Render widget list, only for GL3 and DX, called by SceneManager.
		*/
		virtual void render(const std::vector<Widget*>& renderList);
		/**
		* @brief Render scene node list, only for GL3 and DX, called by Mesh.
		*/
		virtual void render(std::vector<SceneNode*> renderList);

    protected:
        ObjectDX(ComPtr<ID3D12Device> device, const char* name)
            : IObject(name), mD3DDevice(device), mRenderState(nullptr),
            mVertexBuffer(nullptr), mIndexBuffer(nullptr), mDrawMode(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) {}
        virtual ~ObjectDX();

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
        /**
        * @brief Render a AABB using object matrix, object AABB data use to scale global AABB.
        * @params attrs Object rendering attr, include matrixes.
        */
        void renderAABB(RenderObjectAttr* attrs);
        /**
        * Delete AABB vertex buffers, it could be called by RenderDX.
        */
        static void deleteAABBBuffers();

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