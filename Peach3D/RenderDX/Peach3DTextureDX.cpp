//
//  Peach3DTextureDX.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DTextureDX.h"
#include "Peach3DTextureLoader.h"
#include "Peach3DResourceManager.h"
#include "Peach3DIPlatform.h"
#include "Peach3DRenderDX.h"

namespace Peach3D
{
    // defined text clicked list
    std::map<std::wstring, std::string> TextureDX::mClickedStrMap;

    bool TextureDX::setTextureData(void* data, uint size, TextureDataStatus status)
    {
        if (!ITexture::setTextureData(data, size, status)) {
            return false;
        }

        // is mipmap enabled
        bool isMipMapEnabled = ResourceManager::getSingleton().isTextureMipMapEnabled();
        // load texture
        HRESULT hr = -1;
        if (status == TextureDataStatus::eDecoded) {
            DXGI_FORMAT format;
            uint rowPitch = 0;
            if (mTexFormat == TextureFormat::eRGBA8) {
                format = DXGI_FORMAT_R8G8B8A8_UNORM;
                rowPitch = sizeof(uchar) * mWidth * 4;
            }
            else if (mTexFormat == TextureFormat::eI8) {
                format = DXGI_FORMAT_A8_UNORM;
                rowPitch = sizeof(uchar) * mWidth;
            }
            // create texture with decoded data
            if (rowPitch > 0) {
                hr = CreateWICTextureFromDecodedData(mD3DDevice.Get(), mDeviceContext.Get(), (uchar*)data, size,
                    mWidth, rowPitch, mHeight, format, isMipMapEnabled, &mTextureRV);
            }
        }
        else {
            hr = CreateWICTextureFromMemory(mD3DDevice.Get(), mDeviceContext.Get(), (const uchar*)data, size,
                isMipMapEnabled, &mTextureRV, &mWidth, &mHeight, &mTexFormat);
        }
        if (FAILED(hr))
            return false;

        // Create the sample state
        if (!recreatedSamplerState())
            return false;
        return true;
    }

    bool TextureDX::setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status)
    {
        // Create the sample state
        if (!recreatedSamplerState())
            return false;
        return true;
    }

    void TextureDX::setFilter(TextureFilter filter)
    {
        if (filter != mTexFilter)
        {
            ITexture::setFilter(filter);
            recreatedSamplerState();
        }
    }

    void TextureDX::setWrap(TextureWrap wrap)
    {
        if (wrap != mTexWrap)
        {
            ITexture::setWrap(wrap);
            recreatedSamplerState();
        }
    }

    bool TextureDX::recreatedSamplerState()
    {
        D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        if (mTexFilter == TextureFilter::eLinear) {
            filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        }
        else if (mTexFilter == TextureFilter::eAnisotropic) {
            filter = D3D11_FILTER_ANISOTROPIC;
        }
        D3D11_TEXTURE_ADDRESS_MODE wrap = D3D11_TEXTURE_ADDRESS_CLAMP;
        if (mTexWrap == TextureWrap::eRepeat) {
            wrap = D3D11_TEXTURE_ADDRESS_WRAP;
        }
        else if (mTexWrap == TextureWrap::eMirrored) {
            wrap = D3D11_TEXTURE_ADDRESS_MIRROR;
        }
        // create sampler state
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter = filter;
        sampDesc.AddressU = wrap;
        sampDesc.AddressV = wrap;
        sampDesc.AddressW = wrap;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        ID3D11SamplerState* tempState = nullptr;
        HRESULT hr = mD3DDevice->CreateSamplerState(&sampDesc, &tempState);
        if (FAILED(hr))
            return false;

        // release old state if create success
        if (mTexState)
        {
            mTexState->Release();
            mTexState = nullptr;
        }
        mTexState = tempState;
        return true;
    }

    TextureDX::~TextureDX()
    {
        if (mTextureRV)
        {
            mTextureRV->Release();
            mTextureRV = nullptr;
        }
        if (mTexState)
        {
            mTexState->Release();
            mTexState = nullptr;
        }
    }

    D2D_SIZE_F TextureDX::getTextRenderSize(const std::wstring& text, IDWriteTextFormat* textFormat, D2D_SIZE_F maxSize)
    {
        RenderDX* render = static_cast<RenderDX*>(IRender::getSingletonPtr());
        IDWriteTextLayout* textLayout = nullptr;
        render->getDWriteFactory()->CreateTextLayout(text.c_str(), (UINT32)text.size(), textFormat, maxSize.width, maxSize.height, &textLayout);
        DWRITE_TEXT_METRICS textMetrics;
        textLayout->GetMetrics(&textMetrics);
        textLayout->Release();
        return D2D1::SizeF(textMetrics.width, textMetrics.height);
    }

    Vector2 TextureDX::calcTextRenderInfoList(const std::vector<LabelStageTextInfo>& textList, IDWriteTextFormat* textFormat,
        const LabelTextDefined& defined, std::vector<std::vector<tTextDrawInfo>>* drawInfoList)
    {
#define CHANGE_TEXT_NEW_LINE(height) \
        curHeight += height; \
        if (curWidth > textMaxWidth) { \
            textMaxWidth = curWidth; \
                        } \
        curWidth = 0.0f; \
        lineMaxHeight = 0.0f; \
        if (lineInfoList.size() > 0) { \
            drawInfoList->push_back(lineInfoList); \
                        } \
        lineInfoList.clear();

        mClickedStrMap.clear();
        const PlatformCreationParams& params = IPlatform::getSingleton().getCreationParams();
        // calc space render size
        D2D_SIZE_F spaceSize = getTextRenderSize(L" ", textFormat, D2D1::SizeF((float)params.width, (float)params.height));
        Vector2 rSize = defined.dim;
        if (FLOAT_EQUAL_0(defined.dim.x)) {
            rSize.x = (float)params.width;
        }

        float curWidth = 0.0f, curHeight = 0.0f;
        float lineMaxHeight = 0.0f, textMaxWidth = 0.0f;
        std::vector<tTextDrawInfo> lineInfoList;
        for (auto info : textList) {
            float curDrawHeight = 0.0f;
            if (info.image.size() > 0) {
                ulong texLength = 0;
                uchar *texData = ResourceManager::getSingleton().getFileData(info.image.c_str(), &texLength);
                if (texLength > 0 && texData){
                    UINT iWidth = 0, iHeight = 0;
                    TextureFormat format;
                    ID3D11Texture2D* texture = nullptr;
                    HRESULT hr = CreateD3DTextureFromMemory(mD3DDevice.Get(), mDeviceContext.Get(),
                        texData, texLength, false, &texture, &iWidth, &iHeight, &format);
                    if (SUCCEEDED(hr) && texture && iWidth > 0 && iHeight > 0) {
                        float imageWidth = (float)iWidth, imageHeight = (float)iHeight;
                        if (curWidth > 0.0f && (rSize.x - curWidth) < imageWidth) {
                            // direct change to newline
                            CHANGE_TEXT_NEW_LINE(lineMaxHeight)
                        }
                        // add image to list
                        float drawPosY = curHeight;
                        if (imageHeight < lineMaxHeight) {
                            if (defined.imageVAlign == TextVAlignment::eCenter){
                                drawPosY += (lineMaxHeight - imageHeight) / 2.0f;
                            }
                            else if (defined.imageVAlign == TextVAlignment::eBottom) {
                                drawPosY += lineMaxHeight - imageHeight;
                            }
                        }
                        lineInfoList.push_back(tTextDrawInfo(D2D1::Point2F(curWidth, drawPosY), D2D1::SizeF(imageWidth, imageHeight), texture));
                        curWidth += imageWidth;
                        // save current draw height
                        curDrawHeight = imageHeight;
                    }
                }
            }
            else if (info.text.compare("\n") == 0) {
                // direct change to newline
                CHANGE_TEXT_NEW_LINE(spaceSize.height);
            }
            else {
                wchar_t* cTextStr = convertUTF8ToUnicode(info.text);
                std::wstring textStr = cTextStr;
                free(cTextStr);
                ulong lastLength = (ulong)textStr.size();
                // calc if need change line
                while (lastLength && textStr.size()) {
                    ulong drawLength = lastLength;
                    std::wstring drawStr = textStr;
                    D2D_SIZE_F drawSize = getTextRenderSize(drawStr, textFormat, D2D1::SizeF((float)params.width, (float)params.height));
                    // calc last width could render string length
                    float lastDrawWidth = rSize.x - curWidth;
                    while (drawLength && drawSize.width >= lastDrawWidth) {
                        drawLength = drawLength - 1;
                        drawStr = textStr.substr(0, drawLength);
                        drawSize = getTextRenderSize(drawStr, textFormat, D2D1::SizeF((float)params.width, (float)params.height));
                    }
                    DXGI_RGBA infoColor = D2D1::ColorF(info.color.r, info.color.g, info.color.b, info.color.a);
                    // calc text draw need pos y
                    float drawPosY = curHeight;
                    if (drawSize.height < lineMaxHeight) {
                        if (defined.imageVAlign == TextVAlignment::eCenter){
                            drawPosY += (lineMaxHeight - drawSize.height) / 2.0f;
                        }
                        else if (defined.imageVAlign == TextVAlignment::eBottom) {
                            drawPosY += lineMaxHeight - drawSize.height;
                        }
                    }
                    lineInfoList.push_back(tTextDrawInfo(D2D1::Point2F(curWidth, drawPosY), drawSize, infoColor, drawStr));
                    curWidth += drawSize.width;
                    // store click enabled string
                    if (info.clickEnabled) {
                        mClickedStrMap[drawStr] = info.text;
                    }
                    if (FLOAT_EQUAL_0(lineMaxHeight)){
                        lineMaxHeight = drawSize.height;
                    }
                    if (drawLength < lastLength) {
                        // direct change to newline
                        CHANGE_TEXT_NEW_LINE(lineMaxHeight);
                        textStr = textStr.substr(drawLength);
                    }
                    lastLength -= drawLength;
                    // save current draw height
                    curDrawHeight = drawSize.height;
                }
            }
            if (curDrawHeight > lineMaxHeight) {
                // modify all previous tTextDrawInfo
                std::for_each(lineInfoList.begin(), lineInfoList.end(), [&](tTextDrawInfo& info){
                    if (curDrawHeight > info.size.height){
                        if (defined.imageVAlign == TextVAlignment::eCenter){
                            info.start.y += (curDrawHeight - lineMaxHeight) / 2.0f;
                        }
                        else if (defined.imageVAlign == TextVAlignment::eBottom) {
                            info.start.y += curDrawHeight - lineMaxHeight;
                        }
                    }
                });
                lineMaxHeight = curDrawHeight;
            }
        }
        // add last render content
        if (lineInfoList.size() > 0) {
            CHANGE_TEXT_NEW_LINE(lineMaxHeight)
        }
        else {
            // when last word is '\n', lineInfoList will be empty
            CHANGE_TEXT_NEW_LINE(spaceSize.height);
        }

        // save text need render size
        if (FLOAT_EQUAL_0(defined.dim.x)) {
            rSize.x = textMaxWidth;
        }
        rSize.y = curHeight;
        return rSize;
    }

    bool TextureDX::createTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
        std::map<std::string, std::vector<Rect>>& clicksRect)
    {
        RenderDX* render = static_cast<RenderDX*>(IRender::getSingletonPtr());
        // create text format
        IDWriteTextFormat* pWriteFormat = nullptr;
        wchar_t* fontName = convertUTF8ToUnicode(defined.font);
        HRESULT hr = render->getDWriteFactory()->CreateTextFormat(fontName, NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, defined.fontSize, L"", &pWriteFormat);
        free(fontName);
        if (FAILED(hr)) {
            hr = render->getDWriteFactory()->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, defined.fontSize, L"", &pWriteFormat);
            if (FAILED(hr)) {
                Peach3DErrorLog("Create font Arial text format failed");
                return false;
            }
        }
        // calc text need size
        std::vector<std::vector<tTextDrawInfo>> drawInfoList;
        Vector2 renderSize = calcTextRenderInfoList(textList, pWriteFormat, defined, &drawInfoList);
        if (renderSize.x < FLT_EPSILON || renderSize.y < FLT_EPSILON) {
            pWriteFormat->Release();
            return false;
        }

        // apply horizontal alignment
        if (defined.hAlign != TextHAlignment::eLeft) {
            std::for_each(drawInfoList.begin(), drawInfoList.end(), [&](std::vector<tTextDrawInfo>& lineList){
                tTextDrawInfo& lastInfo = lineList[lineList.size() - 1];
                // default for horizontal right
                float offsetX = renderSize.x - (lastInfo.start.x + lastInfo.size.width);
                if (defined.hAlign == TextHAlignment::eCenter) {
                    // set horizontal center
                    offsetX = offsetX / 2.0f;
                }
                std::for_each(lineList.begin(), lineList.end(), [&](tTextDrawInfo& info) {
                    info.start.x += offsetX;
                });
            });
        }
        float startPosY = 0.0f;
        // apply vertical alignment
        if (defined.dim.x > 0 && defined.dim.y > 0) {
            if (defined.vAlign == TextVAlignment::eBottom) {
                startPosY = defined.dim.y - renderSize.y;
            }
            else if (defined.vAlign == TextVAlignment::eCenter) {
                startPosY = (defined.dim.y - renderSize.y) / 2.0f;
            }
            renderSize.y = defined.dim.y;
        }

        FLOAT dpiX = 0.0f; FLOAT dpiY = 0.0f;
        render->getD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
        // Create texture
        ID3D11Texture2D* m_pOffscreenTexture = nullptr;
        ID2D1RenderTarget* renderTarget = nullptr;
        ID2D1BitmapRenderTarget* bmpRenderTarget = nullptr;
        do {
            uint mipLevels = 1;
            D3D11_TEXTURE2D_DESC desc;
            desc.Width = (int)renderSize.x;
            desc.Height = (int)renderSize.y;
            desc.MipLevels = mipLevels;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;
            if (FAILED(mD3DDevice->CreateTexture2D(&desc, NULL, &m_pOffscreenTexture))){
                Peach3DErrorLog("Create text ID3D11Texture2D %f, %f failed", renderSize.x, renderSize.y);
                break;
            }
            // create render target
            ComPtr<IDXGISurface2> surface;
            m_pOffscreenTexture->QueryInterface(__uuidof(IDXGISurface2), &surface);
            D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
                dpiX, dpiY);
            if (FAILED(render->getD2DFactory()->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, &renderTarget))) {
                Peach3DErrorLog("Create text render target failed");
                break;
            }
            // enable antialias
            renderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
            // create color brush
            ID2D1SolidColorBrush* colorBrush = nullptr;
            renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &colorBrush);
            renderTarget->BeginDraw();
            // just for test
            //renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Red));
            renderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));
            // draw text
            for (auto lineList : drawInfoList) {
                for (auto info : lineList) {
                    info.start.y = info.start.y + startPosY;
                    if (info.start.y >= (-info.size.height) && info.start.y <= renderSize.y) {
                        if (info.str.size() > 0) {
                            D2D_RECT_F drawRect = D2D1::RectF(info.start.x, info.start.y, info.size.width + info.start.x, info.size.height + info.start.y);
                            colorBrush->SetColor(info.color);
                            renderTarget->DrawText(info.str.c_str(), (UINT32)info.str.size(), pWriteFormat, drawRect, colorBrush);

                            // add click enabled rect
                            auto clickIter = mClickedStrMap.find(info.str);
                            if (clickIter != mClickedStrMap.end()) {
                                clicksRect[clickIter->second].push_back(Rect(info.start.x, info.start.y, info.size.width, info.size.height));
                            }
                        }
                    }
                }
            }
            renderTarget->EndDraw();
            colorBrush->Release();

            // draw image
            for (auto lineList : drawInfoList) {
                for (auto info : lineList) {
                    info.start.y = info.start.y + startPosY;
                    if (info.start.y >= (-info.size.height) && info.start.y <= renderSize.y) {
                        if (info.image) {
                            D3D11_BOX sourceRegion;
                            sourceRegion.left = 0; sourceRegion.top = 0;
                            sourceRegion.right = (uint)(info.size.width + 0.5f);
                            sourceRegion.bottom = (uint)(info.size.height + 0.5f);
                            sourceRegion.front = 0; sourceRegion.back = 1;
                            mDeviceContext->CopySubresourceRegion(m_pOffscreenTexture, 0, (uint)(info.start.x + 0.5f),
                                (uint)(info.start.y + 0.5f), 0, info.image, 0, &sourceRegion);
                        }
                    }
                }
            }

            // create texture rv
            D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
            memset(&SRVDesc, 0, sizeof(SRVDesc));
            SRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Texture2D.MipLevels = mipLevels;
            if (FAILED(mD3DDevice->CreateShaderResourceView(m_pOffscreenTexture, &SRVDesc, &mTextureRV))) {
                Peach3DErrorLog("Create shader resource view failed from text texture");
                mTextureRV = nullptr;
                break;
            }

            mWidth = desc.Width;
            mHeight = desc.Height;
            mTexFormat = TextureFormat::eRGBA8;
            // Create the sample state
            recreatedSamplerState();
        } while (0);

        // release handlers
        pWriteFormat->Release();
        if (m_pOffscreenTexture) {
            m_pOffscreenTexture->Release();
        }
        if (renderTarget) {
            renderTarget->Release();
        }
        for (auto lineList : drawInfoList) {
            for (auto info : lineList) {
                if (info.image) {
                    info.image->Release();
                }
            }
        }

        return mTextureRV!=nullptr;
    }
}