//
//  Peach3DTextureLoader.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_TEXTURELOADER_H
#define PEACH3D_TEXTURELOADER_H

#include "Peach3DCommonDX.h"
#include "Peach3DITexture.h"
#include <dxgiformat.h>
#include <assert.h>
#include <wincodec.h>
#include <memory>

namespace Peach3D
{
    // create ID3D11ShaderResourceView from orign texture file
    HRESULT CreateWICTextureFromMemory(_In_ ID3D11Device* d3dDevice,
        _In_opt_ ID3D11DeviceContext* d3dContext,
        _In_bytecount_(wicDataSize) const uchar* wicData,
        _In_ size_t wicDataSize,
        _In_ bool mipMapEnabled,
        _Out_opt_ ID3D11ShaderResourceView** textureView,
        _Out_opt_ uint* tWidth,
        _Out_opt_ uint* tHeight,
        _Out_opt_ TextureFormat* tFormat
        );
    // create ID3D11Texture2D from orign texture file
    HRESULT CreateD3DTextureFromMemory(_In_ ID3D11Device* d3dDevice,
        _In_opt_ ID3D11DeviceContext* d3dContext,
        _In_bytecount_(wicDataSize) const uchar* wicData,
        _In_ size_t wicDataSize,
        _In_ bool mipMapEnabled,
        _Out_opt_ ID3D11Texture2D** texture,
        _Out_opt_ uint* tWidth,
        _Out_opt_ uint* tHeight,
        _Out_opt_ TextureFormat* tFormat
        );
    // create ID3D11ShaderResourceView from pixels content
    HRESULT CreateWICTextureFromDecodedData(_In_ ID3D11Device* d3dDevice,
        _In_opt_ ID3D11DeviceContext* d3dContext,
        _In_bytecount_(wicDataSize) const uchar* dataBuf,
        _In_ size_t dataSize,
        _In_ uint twidth,
        _In_ uint rowPitch,
        _In_ uint theight,
        _In_ DXGI_FORMAT format,
        _In_ bool mipMapEnabled,
        _Out_opt_ ID3D11ShaderResourceView** textureView
        );
    // create ID3D11Texture2D from pixels content
    HRESULT CreateD3DTextureFromDecodedData(_In_ ID3D11Device* d3dDevice,
        _In_opt_ ID3D11DeviceContext* d3dContext,
        _In_bytecount_(wicDataSize) const uchar* dataBuf,
        _In_ size_t dataSize,
        _In_ uint twidth,
        _In_ uint rowPitch,
        _In_ uint theight,
        _In_ DXGI_FORMAT format,
        _In_ bool mipMapEnabled,
        _Out_opt_ ID3D11Texture2D** texture
        );
}

#endif // PEACH3D_TEXTURELOADER_H
