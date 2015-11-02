
#include "Peach3DObjectDX.h"
#include "Peach3DProgramDX.h"
#include "Peach3DIPlatform.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    ID3D11Buffer*   ObjectDX::mAABBVertexBuffer = nullptr;
    ID3D11Buffer*   ObjectDX::mAABBIndexBuffer = nullptr;
    IProgram*       ObjectDX::mAABBProgram = nullptr;

    bool ObjectDX::setVertexBuffer(const void* data, uint size, uint type)
    {
        // base object setVertexBuffer
        bool result = IObject::setVertexBuffer(data, size, type);

        // delete old vertex buffer
        cleanObjectVertexBuffer();

        // create vertex buffer
        if (data && size > 0 && result)
        {
            D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
            vertexBufferDesc.ByteWidth = size;
            vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vertexBufferDesc.CPUAccessFlags = 0;
            vertexBufferDesc.MiscFlags = 0;
            vertexBufferDesc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA vertexBufferData;
            vertexBufferData.pSysMem = data;
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;
            mD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &mVertexBuffer);

            // only set draw tranglestrip when vertex count is 4
            if ((mVertexBufferSize / mVertexDataStride) == 4)
            {
                mDrawMode = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            }
        }
        return result && mVertexBuffer;
    }

    void ObjectDX::setIndexBuffer(const void*data, uint size, IndexType type)
    {
        // base object setIndexBuffer
        IObject::setIndexBuffer(data, size, type);

        // delete old index buffer
        cleanObjectIndexBuffer();

        // create index buffer
        if (data && size > 0)
        {
            D3D11_BUFFER_DESC indexBufferDesc = { 0 };
            indexBufferDesc.ByteWidth = size;
            indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            indexBufferDesc.CPUAccessFlags = 0;
            indexBufferDesc.MiscFlags = 0;
            indexBufferDesc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA indexBufferData;
            indexBufferData.pSysMem = data;
            indexBufferData.SysMemPitch = 0;
            indexBufferData.SysMemSlicePitch = 0;
            mD3DDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &mIndexBuffer);
        }
    }

    void ObjectDX::render(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime)
    {
        // check is need choose preset program
        IObject::render(attrs, mtl, lastFrameTime);

        if (mVertexBuffer && mRenderProgram && mRenderProgram->useAsRenderProgram())
        {
            // bind vertex buffer and index buffer
            this->bindBaseAttrBuffer(attrs, mtl, lastFrameTime);

            // enable depth bias
            float biasFactor = attrs->depthBias;
            ID3D11RasterizerState *oldRasterizerState = nullptr;
            if (biasFactor > FLT_EPSILON || biasFactor < -FLT_EPSILON) {
                mDeviceContext->RSGetState(&oldRasterizerState);
                if (!mRenderState) {
                    // create render state
                    D3D11_RASTERIZER_DESC rasterizerDesc;
                    oldRasterizerState->GetDesc(&rasterizerDesc);
                    rasterizerDesc.DepthBias = INT(100.0f * biasFactor);
                    rasterizerDesc.SlopeScaledDepthBias = 0.0f;
#if PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_WINDESK
                    rasterizerDesc.DepthBiasClamp = 0.0001f;
#endif
                    mD3DDevice->CreateRasterizerState(&rasterizerDesc, &mRenderState);
                }
                mDeviceContext->RSSetState(mRenderState);
            }
            if (mIndexBuffer)
            {
                uint indexStride = (mIndexDataType == IndexType::eUShort) ? sizeof(ushort) : sizeof(uint);
                // draw as triangles when indexs exist
                mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                mDeviceContext->DrawIndexed(mIndexBufferSize / indexStride, 0, 0);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE(mIndexBufferSize / (indexStride * 3));
            }
            else
            {
                // draw triangles array
                mDeviceContext->IASetPrimitiveTopology(mDrawMode);
                mDeviceContext->Draw(mVertexBufferSize / mVertexDataStride, 0);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE(mVertexBufferSize / (mVertexDataStride * 3));
            }
            // disable depth bias after rendering
            if (biasFactor > FLT_EPSILON || biasFactor < -FLT_EPSILON) {
                mDeviceContext->RSSetState(oldRasterizerState);
            }

            // check AABB need show
            if (attrs->showAABB) {
                renderAABB(attrs);
            }
        }
    }

    void ObjectDX::bindBaseAttrBuffer(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime)
    {
        // enable vertex buffer
        UINT offset = 0;
        mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexDataStride, &offset);
        // enable index buffer
        if (mIndexBuffer)
        {
            if (mIndexDataType == IndexType::eUShort)
            {
                mDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
            }
            else if (mIndexDataType == IndexType::eUInt)
            {
                mDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
            }
        }
        // set object uniforms and material, global uniforms has update before rendering
        mRenderProgram->updateObjectUnifroms(attrs, mtl, lastFrameTime);
    }

    void ObjectDX::generateAABBBuffers(ComPtr<ID3D11Device2> device, ComPtr<ID3D11DeviceContext2> context)
    {
        // generate vertex buffer for AABB rendering
        if (!mAABBVertexBuffer && !mAABBIndexBuffer) {
            float globalVertex[] = { -0.5, -0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5, -0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5, 0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                -0.5, 0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                -0.5, -0.5, 0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5, -0.5, 0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5, 0.5, 0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                -0.5, 0.5, 0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a };
            D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
            vertexBufferDesc.ByteWidth = sizeof(globalVertex);
            vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vertexBufferDesc.CPUAccessFlags = 0;
            vertexBufferDesc.MiscFlags = 0;
            vertexBufferDesc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA vertexBufferData;
            vertexBufferData.pSysMem = globalVertex;
            vertexBufferData.SysMemPitch = 0;
            vertexBufferData.SysMemSlicePitch = 0;
            device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &mAABBVertexBuffer);

            ushort globalIndex[] = {0, 1, 2, 3, 0, 4, 5, 6, 7, 4,  1, 5, 2, 6, 3, 7};
            D3D11_BUFFER_DESC indexBufferDesc = { 0 };
            indexBufferDesc.ByteWidth = sizeof(globalIndex);
            indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
            indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            indexBufferDesc.CPUAccessFlags = 0;
            indexBufferDesc.MiscFlags = 0;
            indexBufferDesc.StructureByteStride = 0;

            D3D11_SUBRESOURCE_DATA indexBufferData;
            indexBufferData.pSysMem = globalIndex;
            indexBufferData.SysMemPitch = 0;
            indexBufferData.SysMemSlicePitch = 0;
            device->CreateBuffer(&indexBufferDesc, &indexBufferData, &mAABBIndexBuffer);

            // generate program
            Material AABBMtl;
            mAABBProgram = ResourceManager::getSingleton().getObjectPresetProgram(VertexTypePosition3 | VertexTypeColor4, AABBMtl);
        }
    }

    void ObjectDX::renderAABB(RenderObjectAttr* attrs)
    {
        // generate AABB buffers if needed
        generateAABBBuffers(mD3DDevice, mDeviceContext);
        const UINT offset = 0, vertexStride = 7 * sizeof(float);
        mDeviceContext->IASetVertexBuffers(0, 1, &mAABBVertexBuffer, &vertexStride, &offset);
        mDeviceContext->IASetIndexBuffer(mAABBIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

        // update AABB model matrix
        AABB nodeAABB = getAABB(*attrs->modelMatrix);
        if (nodeAABB.isValid()) {
            // update AABB model matrix
            Vector3 AABBSize = Vector3(nodeAABB.max.x - nodeAABB.min.x,
                nodeAABB.max.y - nodeAABB.min.y,
                nodeAABB.max.z - nodeAABB.min.z);
            Vector3 AABBCenter = Vector3((nodeAABB.max.x + nodeAABB.min.x) / 2.0f,
                (nodeAABB.max.y + nodeAABB.min.y) / 2.0f,
                (nodeAABB.max.z + nodeAABB.min.z) / 2.0f);
            Matrix4 AABBTranslateMat = Matrix4::createTranslation(AABBCenter.x, AABBCenter.y, AABBCenter.z);
            Matrix4 AABBScaleMat = Matrix4::createScaling(AABBSize.x, AABBSize.y, AABBSize.z);
            Matrix4 AABBMat = AABBTranslateMat * AABBScaleMat;
            attrs->modelMatrix = &AABBMat;

            // set program
            mAABBProgram->useAsRenderProgram();
            mAABBProgram->updateObjectUnifroms(attrs, nullptr, 0.0f);
            // modelMatrix not enabled, it point to a local Matrix4.
            attrs->modelMatrix = nullptr;

            // draw AABB
            mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
            mDeviceContext->DrawIndexed(10, 0, 0);
            PD_ADD_DRAWCALL(1);
            mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
            mDeviceContext->DrawIndexed(6, 10, 0);
            PD_ADD_DRAWCALL(1);
        }
    }

    void ObjectDX::deleteAABBBuffers()
    {
        if (mAABBVertexBuffer) {
            mAABBVertexBuffer->Release();
            mAABBVertexBuffer = nullptr;
        }
        if (mAABBIndexBuffer) {
            mAABBIndexBuffer->Release();
            mAABBIndexBuffer = nullptr;
        }
    }

    void ObjectDX::cleanObjectVertexBuffer()
    {
        if (mVertexBuffer)
        {
            // delete object
            mVertexBuffer->Release();
            mVertexBuffer = nullptr;
            // set vertex size and stride to zero
            mVertexBufferSize = 0;
            mVertexDataStride = 0;
        }
    }

    void ObjectDX::cleanObjectIndexBuffer()
    {
        if (mIndexBuffer)
        {
            // delete object
            mIndexBuffer->Release();
            mIndexBuffer = nullptr;
            // set vertex size and stride to zero
            mIndexBufferSize = 0;
        }
    }

    ObjectDX::~ObjectDX()
    {
        // delete dx object
        cleanObjectVertexBuffer();
        cleanObjectIndexBuffer();
        if (mRenderState) {
            mRenderState->Release();
            mRenderState = nullptr;
        }
    }
}