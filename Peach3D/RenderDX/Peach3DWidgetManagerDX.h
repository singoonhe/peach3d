//
//  Peach3DWidgetManagerDX.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_WidgetManagerDX_H
#define PEACH3D_WidgetManagerDX_H

#include "Peach3DCommonDX.h"
#include "Peach3DIWidgetManager.h"

namespace Peach3D
{
    using namespace Microsoft::WRL;
    class WidgetManagerDX : public IWidgetManager
    {
    public:
        WidgetManagerDX(ComPtr<ID3D11Device2> device, ComPtr<ID3D11DeviceContext2> context);
        ~WidgetManagerDX();
        //! render all widget
        virtual void renderAllWidget();

    protected:
        //! create size widget render vertex buffers
        void createWidgetRenderBuffers(uint size);
        //! draw a batch widget
        void drawBatchWidget(Widget* widget, uint count);

    private:
        ComPtr<ID3D11Device2>          mD3DDevice;
        ComPtr<ID3D11DeviceContext2>   mDeviceContext;
        ID3D11Buffer*                  mVertexBuffer;
        ID3D11Buffer*                  mInstanceBuffer;
        ID3D11Buffer*                  mIndexBuffer;

        D3D11_RASTERIZER_DESC          mRasterizerDesc;
        float* mInstanceData;       // color/matrix float data
        float* mVertexData;         // vertex and 
        uint   mVertexMaxCount;     // vertex max count, default is VERTEX_COUNT_PER_STEP
    };
}

#endif // PEACH3D_WidgetManagerDX_H
