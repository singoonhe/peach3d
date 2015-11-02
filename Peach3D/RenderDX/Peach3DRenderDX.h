#pragma once

#include "Peach3DCommonDX.h"
#include "Peach3DIRender.h"

using namespace Microsoft::WRL;
namespace Peach3D
{
    class RenderDX : public IRender
    {
    public:
        RenderDX() : mD2DFactory(nullptr), mDWriteFactory(nullptr), mWICImageFactory(nullptr) {}
        virtual ~RenderDX();
        //! init not window dependent d3d handlers
        bool createGlobelRender();
        // init render, will create window dependent handlers
        virtual bool initRender(uint width, uint height);
        // clear old frame before render
        virtual void prepareForRender();
        // open depth test...
        virtual void prepareForObjectRender();
        // close depth test...
        virtual void prepareForWidgetRender();
        // present, discard target and stencil view
        virtual void finishForRender();

        //! Set current oritation, this must called after initRender
        void setContentOritation(DXGI_MODE_ROTATION rotation);
        //! call program update global Unifroms
        void updateGlobalUnifroms(RenderObjectAttr* attrs);

        //! Create renderable object.
        /** \params the name of new renderable object.
            \retuan retuan the new object pointer, return nullptr if create failed. */
        virtual IObject* createObject(const char* objectName);

        //! Create texture with data
        virtual ITexture* createTexture(const char* name);

        //! Create render program with name. Program include vertex and pixel shader for render.
        /** \params the name of new OpenGL ES program.
        \return pointer of new program, return nullptr if create failed. */
        virtual IProgram* createProgram(uint pId);

        //! generate object vertex shader source code
        virtual void getObjectPresetVSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms);
        //! generate object pixel shader source code
        virtual void getObjectPresetPSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms);
        //! generate widget vertex shader source code
        virtual void getWidgetPresetVSSource(const WidgetProgramImpact& impact, std::string* code, std::vector<ProgramUniform>* uniforms);
        //! generate widget pixel shader source code
        virtual void getWidgetPresetPSSource(const WidgetProgramImpact& impact, std::string* code, std::vector<ProgramUniform>* uniforms);
        
        //! get Direct2D factory
        ID2D1Factory1* getD2DFactory() { return mD2DFactory; }
        //! get DWrite factory
        IDWriteFactory* getDWriteFactory() { return mDWriteFactory; }
        //! get WICImage factory
        IWICImagingFactory* getWICImageFactory() { return mWICImageFactory; }

    protected:
        //! Check support MSAA qulity level
        int checkMSAASupportQulityLevel(int MSAA);
        //! create D2D and WIC factorys
        bool createD2DWICFactory();

    private:
        // dx device handlers, ComPtr can't set nullptr, it not a pointer
        ComPtr<ID3D11Device2>           mD3DDevice;
        ComPtr<ID3D11DeviceContext2>    mDeviceContext;
        D3D11_DEPTH_STENCIL_DESC        mDepthStencilDesc;
        D3D11_RASTERIZER_DESC           mRasterizerDesc;

        ComPtr<IDXGISwapChain1>         mSwapChain;
        ComPtr<ID3D11RenderTargetView>  mTargetView;
        ComPtr<ID3D11DepthStencilView>  mDepthStencilView;

        ID2D1Factory1*                  mD2DFactory;
        IDWriteFactory*                 mDWriteFactory;
        IWICImagingFactory*             mWICImageFactory;
    };
}

