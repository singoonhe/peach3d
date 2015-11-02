//
//  Peach3DTextureDX.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_TEXTUREDX_H
#define PEACH3D_TEXTUREDX_H

#include "Peach3DCommonDX.h"
#include "Peach3DITexture.h"
#include "Peach3DTypes.h"
#include "Peach3DRect.h"
#include <wincodec.h>

using namespace Microsoft::WRL;
namespace Peach3D
{
    // defined text draw base info
    typedef struct _tTextDrawInfo
    {
        D2D_POINT_2F    start;  // draw start pos
        D2D_SIZE_F      size;   // draw text size
        D2D_COLOR_F     color;  // draw color
        std::wstring    str;    // draw text
        ID3D11Texture2D* image;  // draw image

        _tTextDrawInfo(D2D_POINT_2F _start, D2D_SIZE_F _size, ID3D11Texture2D* _image) : start(_start), size(_size), image(_image) {}
        _tTextDrawInfo(D2D_POINT_2F _start, D2D_SIZE_F _size, DXGI_RGBA _color, std::wstring _str) : start(_start), size(_size), color(_color), str(_str) { image = nullptr; }
    }tTextDrawInfo;

    class TextureDX : public ITexture
    {
    public:
        /** Set texture data and init texture, format and size must set before. */
        virtual bool setTextureData(void* data, uint size, TextureDataStatus status = TextureDataStatus::eDecoded);
        /** Set texture data and init texture for cube texture, format and size must set before. */
        virtual bool setTextureData(void* dataList[6], uint sizeList[6], TextureDataStatus status = TextureDataStatus::eDecoded);
        //! set texture filter
        virtual void setFilter(TextureFilter filter);
        //! set texture wrap
        virtual void setWrap(TextureWrap wrap);

        //! return texture resource
        ID3D11ShaderResourceView* getTextureRV() { return mTextureRV; }
        //! return texture sample state
        ID3D11SamplerState* getTextureState() { return mTexState; }

        //! create texture from text
        bool createTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
            std::map<std::string, std::vector<Rect>>& clicksRect);

    protected:
        TextureDX(ComPtr<ID3D11Device2> device, ComPtr<ID3D11DeviceContext2> context, const char* name) : ITexture(name),
            mD3DDevice(device), mDeviceContext(context), mTextureRV(nullptr), mTexState(nullptr) {}
        virtual ~TextureDX();

        // recreated sampler state
        bool recreatedSamplerState();
        // get text render size
        D2D_SIZE_F getTextRenderSize(const std::wstring& text, IDWriteTextFormat* textFormat, D2D_SIZE_F maxSize);
        // calc text render info list
        Vector2 calcTextRenderInfoList(const std::vector<LabelStageTextInfo>& textList, IDWriteTextFormat* textFormat,
            const LabelTextDefined& defined, std::vector<std::vector<tTextDrawInfo>>* drawInfoList);

    private:
        ComPtr<ID3D11Device2>          mD3DDevice;            // D3D11 device 
        ComPtr<ID3D11DeviceContext2>   mDeviceContext;        // D3D11 device context
        ID3D11ShaderResourceView*      mTextureRV;            // texture handler
        ID3D11SamplerState*            mTexState;             // texture render state

        static std::map<std::wstring, std::string> mClickedStrMap;  // clicked string list, used for text texture
        friend class RenderDX;
    };
}

#endif // PEACH3D_TEXTUREDX_H
