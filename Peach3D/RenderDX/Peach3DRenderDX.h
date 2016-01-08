#pragma once

#include "Peach3DCommonDX.h"
#include "Peach3DIRender.h"

using namespace Microsoft::WRL;
namespace Peach3D
{
	// Use triple buffering.
	static const UINT gDXFrameCount = 3;

    class RenderDX : public IRender
    {
    public:
        RenderDX() : mCurrentFrame(0), mD2DFactory(nullptr), mDWriteFactory(nullptr), mWICImageFactory(nullptr) {
			ZeroMemory(mFenceValues, sizeof(mFenceValues));
		}
        virtual ~RenderDX();
        //! init not window dependent d3d handlers
        bool createRender();
        // init render, will create window dependent handlers
        bool initRender(const Vector2& size, DXGI_MODE_ROTATION rotation);
        // clear old frame before render
        virtual void prepareForRender();
        // open depth test...
        virtual void prepareForObjectRender();
        // close depth test...
        virtual void prepareForWidgetRender();
        // present, discard target and stencil view
        virtual void finishForRender();

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
        
        //! get Direct2D factory
        ID2D1Factory1* getD2DFactory() { return mD2DFactory; }
        //! get DWrite factory
        IDWriteFactory* getDWriteFactory() { return mDWriteFactory; }
        //! get WICImage factory
        IWICImagingFactory* getWICImageFactory() { return mWICImageFactory; }
        D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;

    protected:
        //! Check support MSAA qulity level
        int checkMSAASupportQulityLevel(int MSAA);
        //! create D2D and WIC factorys
        bool createD2DWICFactory();
        /** Wait for pending GPU work to complete. */
        void waitForGpu();
        /** Prepare to render the next frame.*/
        void MoveToNextFrame();

    private:
        // dx device handlers, ComPtr can't set nullptr, it not a pointer
		ComPtr<IDXGIFactory4>				mDXFactory;
		ComPtr<ID3D12Device>				mD3DDevice;
        ComPtr<ID3D12CommandQueue>			mCommandQueue;
        ComPtr<ID3D12CommandAllocator>	    mCommandAllocators[gDXFrameCount];
        ComPtr<ID3D12Fence>                 mFence;
        HANDLE                              mFenceEvent;
        UINT                                mCurrentFrame;
        UINT64                              mFenceValues[gDXFrameCount];

        ComPtr<ID3D12Resource>              mRenderTargets[gDXFrameCount];
        ComPtr<ID3D12DescriptorHeap>        mRtvHeap;
        UINT                                mRtvDescriptorSize;
        ComPtr<ID3D12DescriptorHeap>        mDsvHeap;
        ComPtr<ID3D12Resource>              mDepthStencil;
        ComPtr<ID3D12RootSignature>			mRootSignature;
        ComPtr<ID3D12GraphicsCommandList>   mCommandList;
        D3D12_GRAPHICS_PIPELINE_STATE_DESC  mPipelineDesc;
        ComPtr<IDXGISwapChain3>             mSwapChain;
        Matrix4                             mOrientationMatrix;
        ComPtr<ID3D11RenderTargetView>  mTargetView;
        ComPtr<ID3D11DepthStencilView>  mDepthStencilView;

        ID2D1Factory1*                  mD2DFactory;
        IDWriteFactory*                 mDWriteFactory;
        IWICImagingFactory*             mWICImageFactory;
    };
}

