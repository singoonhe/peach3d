//
//  Peach3DWidgetManagerDX.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DWidgetManagerDX.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    WidgetManagerDX::WidgetManagerDX(ComPtr<ID3D11Device2> device, ComPtr<ID3D11DeviceContext2> context) : mD3DDevice(device), 
        mDeviceContext(context),mVertexBuffer(nullptr),mInstanceBuffer(nullptr), mInstanceData(nullptr), mVertexData(nullptr), mVertexMaxCount(0)
    {
        // init current render state
        mRasterizerDesc.FillMode = D3D11_FILL_SOLID;
        mRasterizerDesc.CullMode = D3D11_CULL_BACK;
        mRasterizerDesc.FrontCounterClockwise = TRUE;
        mRasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        mRasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        mRasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        mRasterizerDesc.DepthClipEnable = TRUE;
        mRasterizerDesc.ScissorEnable = FALSE;
        mRasterizerDesc.MultisampleEnable = IPlatform::getSingleton().getCreationParams().MSAA > 0;
        mRasterizerDesc.AntialiasedLineEnable = FALSE;

        // create fixed index buffer
        ushort fixedIndexData[] = { 0, 1, 3, 0, 3, 2 };
        D3D11_BUFFER_DESC indexBufferDesc = { 0 };
        indexBufferDesc.ByteWidth = sizeof(fixedIndexData);
        indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;
        indexBufferDesc.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA indexBufferData;
        indexBufferData.pSysMem = fixedIndexData;
        indexBufferData.SysMemPitch = 0;
        indexBufferData.SysMemSlicePitch = 0;
        mD3DDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, &mIndexBuffer);

        // create vertex buffer
        createWidgetRenderBuffers(10);
    }

    WidgetManagerDX::~WidgetManagerDX()
    {
        // delete all buffer
        mVertexBuffer->Release();
        mInstanceBuffer->Release();
        mIndexBuffer->Release();
        delete mVertexData;
        delete mInstanceData;
    }

    void WidgetManagerDX::createWidgetRenderBuffers(uint size)
    {
        Peach3DAssert(size > mVertexMaxCount, "new max count must greater than current");
        if (size > mVertexMaxCount) {
            // per vertex data = position2 + uv2
            float *vertexes = (float*)malloc(4 * 4 * sizeof(float) * size);
            // per instance data = color4 + matrix
            float *instances = (float*)malloc(20 * sizeof(float) * size);
            // copy old data
            if (mVertexMaxCount > 0) {
                memcpy(vertexes, mVertexData, mVertexMaxCount * 16 * sizeof(float));
                memcpy(instances, mInstanceData, mVertexMaxCount * 20 * sizeof(float));
                delete mVertexData;
                delete mInstanceData;
            }
            mVertexData = vertexes;
            mInstanceData = instances;
        }
        // save current count
        mVertexMaxCount = size;
        // create vertex buffer
        if (mVertexBuffer) {
            mVertexBuffer->Release();
            mVertexBuffer = nullptr;
        }
        D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
        vertexBufferDesc.ByteWidth = sizeof(float) * mVertexMaxCount * 16;
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        vertexBufferDesc.MiscFlags = 0;
        vertexBufferDesc.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA vertexBufferData;
        vertexBufferData.pSysMem = mVertexData;
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;
        mD3DDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &mVertexBuffer);

        // create instance buffer
        if (mInstanceBuffer) {
            mInstanceBuffer->Release();
            mInstanceBuffer = nullptr;
        }
        D3D11_BUFFER_DESC instanceBufferDesc = { 0 };
        instanceBufferDesc.ByteWidth = sizeof(float) * mVertexMaxCount * 20;
        instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        instanceBufferDesc.MiscFlags = 0;
        instanceBufferDesc.StructureByteStride = 0;
        D3D11_SUBRESOURCE_DATA instanceBufferData;
        instanceBufferData.pSysMem = mInstanceData;
        instanceBufferData.SysMemPitch = 0;
        instanceBufferData.SysMemSlicePitch = 0;
        mD3DDevice->CreateBuffer(&instanceBufferDesc, &instanceBufferData, &mInstanceBuffer);
    }

    void WidgetManagerDX::drawBatchWidget(Widget* widget, uint count)
    {
        // map vertex buffer
        D3D11_MAPPED_SUBRESOURCE subData;
        mDeviceContext->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
        float* vertexBuffer = (float*)subData.pData;
        memcpy(vertexBuffer, mVertexData, count * 16 * sizeof(float));
        mDeviceContext->Unmap(mVertexBuffer, 0);
        // map instance buffer
        mDeviceContext->Map(mInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
        float* instanceBuffer = (float*)subData.pData;
        memcpy(instanceBuffer, mInstanceData, count * 20 * sizeof(float));
        mDeviceContext->Unmap(mInstanceBuffer, 0);

        // enable vertex buffer
        UINT stride[2] = { sizeof(float) * 4, sizeof(float) * 20 };
        UINT offset[2] = { 0, 0 };
        ID3D11Buffer* combinedBuffer[2] = { mVertexBuffer, mInstanceBuffer };
        mDeviceContext->IASetVertexBuffers(0, 2, combinedBuffer, stride, offset);
        // enable index buffer
        mDeviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        // init render program
        IProgram* insProgram = widget->getProgramForRender();
        insProgram->useAsRenderProgram();
        insProgram->updateWidgetUnifroms(widget);

        // set clip
        bool needClip = widget->isNeedClipedByParent();
        // set if back face need render
        const Vector2& rotate = widget->getScale();
        bool backRender = (rotate.x * rotate.y) < 0.0f;
        ID3D11RasterizerState *rasterizerState = nullptr;
        if (needClip || backRender) {
            // open scissor and cull mode
            mRasterizerDesc.ScissorEnable = needClip ? TRUE : FALSE;
            mRasterizerDesc.CullMode = backRender ? D3D11_CULL_FRONT : D3D11_CULL_BACK;
            mD3DDevice->CreateRasterizerState(&mRasterizerDesc, &rasterizerState);
            mDeviceContext->RSSetState(rasterizerState);

            if (needClip) {
                // set scissor rect
                const Rect& clipR = widget->getClipRect();
                const PlatformCreationParams& params = IPlatform::getSingleton().getCreationParams();
                D3D11_RECT srect;
                srect.left = int(clipR.pos.x);
                srect.bottom = params.height - int(clipR.pos.y);
                srect.right = int(clipR.size.x + clipR.pos.x + 0.5);
                srect.top = params.height - int(clipR.size.y + clipR.pos.y + 0.5);
                mDeviceContext->RSSetScissorRects(1, &srect);
            }
        }

        // draw triangles array
        mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mDeviceContext->DrawIndexedInstanced(6, count, 0, 0, 0);
        PD_ADD_DRAWCALL(1);
        PD_ADD_DRAWTRIAGNLE(2 * count);

        // disable clip after render, revert cull mode
        if (needClip || backRender) {
            // open scissor and cull mode
            mRasterizerDesc.ScissorEnable = FALSE;
            mRasterizerDesc.CullMode = D3D11_CULL_BACK;
            mD3DDevice->CreateRasterizerState(&mRasterizerDesc, &rasterizerState);
            mDeviceContext->RSSetState(rasterizerState);
        }
    }

    void WidgetManagerDX::renderAllWidget()
    {
        uint lastTexsHash = 0;
        bool lastForceDraw = false;
        uint renderCount = 0;
        Widget* renderWidget = nullptr;
        for (auto widget : mRenderList) {
            uint wHash = widget->getRenderTexturesHash();
            bool curForceDraw = widget->isUVScrollEnabled() || widget->isNeedClipedByParent() || !widget->isUsePresetProgram();
            if (lastTexsHash != wHash || curForceDraw || lastForceDraw) {
                if (renderCount > 0 && renderWidget) {
                    // draw widgets
                    drawBatchWidget(renderWidget, renderCount);
                    renderCount = 0;
                    lastTexsHash = 0;
                    lastForceDraw = false;
                }
                lastTexsHash = wHash;
            }

            if (widget->isNeedRender()) {
                // set is need draw next tern
                lastForceDraw = curForceDraw;
                // record rendering widget
                renderWidget = widget;

                // resize render buffer size
                int needCount = renderCount - mVertexMaxCount;
                if (needCount > 0) {
                    int formulaCount = min(mVertexMaxCount, (uint)VERTEX_COUNT_INCREASE_STEP);
                    createWidgetRenderBuffers(mVertexMaxCount + max(needCount, formulaCount));
                }
                // save render attrs
                const Color4& color = renderWidget->getColor();
                float colour[] = {color.r, color.g, color.b, renderWidget->getAlpha()};
                const Matrix4& matrix = renderWidget->formatTransformMatrix();
                const Rect& rect = renderWidget->getTextureRect();
                float vertexData[] = { -1.0f, 1.0f, rect.pos.x, rect.pos.y,
                    -1.0f, -1.0f, rect.pos.x, rect.pos.y + rect.size.y,
                    1.0f, 1.0f, rect.pos.x + rect.size.x, rect.pos.y,
                    1.0f, -1.0f, rect.pos.x + rect.size.x, rect.pos.y + rect.size.y };
                memcpy(mVertexData + renderCount * 16, vertexData, 16 * sizeof(float));
                memcpy(mInstanceData + renderCount * 20, &colour, 4 * sizeof(float));
                memcpy(mInstanceData + renderCount * 20 + 4, matrix.getTranspose().mat, 16 * sizeof(float));

                renderCount++;
            }
        }
        // draw last widgets
        if (renderCount > 0 && renderWidget) {
            drawBatchWidget(renderWidget, renderCount);
        }
    }
}