
#include "Peach3DRenderDX.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DProgramDX.h"
#include "Peach3DObjectDX.h"
#include "Peach3DTextureDX.h"
#include "Peach3DIPlatform.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    RenderDX::~RenderDX()
    {
        // delete global AABB buffers
        ObjectDX::deleteAABBBuffers();
        // delete object global UBO
        ProgramDX::deleteGlobalUBO();
        // clear state and release
        //mDeviceContext->ClearState();
        mSwapChain = nullptr;
        mTargetView = nullptr;
        mDepthStencilView = nullptr;
        mD3DDevice = nullptr;
        //mDeviceContext = nullptr;
        if (mD2DFactory){
            mD2DFactory->Release();
        }
        if (mDWriteFactory){
            mDWriteFactory->Release();
        }
        if (mWICImageFactory){
            mWICImageFactory->Release();
        }
    }

    bool RenderDX::createRender()
    {
#if defined(_DEBUG)
		// If the project is in a debug build, enable debugging via SDK Layers.
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
#endif
        // create dxgi factory
        CreateDXGIFactory1(IID_PPV_ARGS(&mDXFactory));
        // choose best adapter for desk, such NVIDIA or ATI
        UINT i = 0;
        WCHAR* adapterName = L"";
        IDXGIAdapter1 *pAdapter = nullptr, *choosedAdapter = nullptr;
        while (mDXFactory->EnumAdapters1(i++, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC1 pDesc;
            pAdapter->GetDesc1(&pDesc);
            // choose NVIDIA and ATI first
            bool isBestAdapter = (pDesc.VendorId == 0x10DE || pDesc.VendorId == 0x1002);
            if (isBestAdapter || i == 0) {
                choosedAdapter = pAdapter;
                adapterName = pDesc.Description;
                if (isBestAdapter) {
                    break;
                }
            }
        }
        // Create the Direct3D 12 API device object
        HRESULT hr = D3D12CreateDevice(
            choosedAdapter,						// Using choosed adapter.
            D3D_FEATURE_LEVEL_11_0,			// Minimum feature level this app can support.
            IID_PPV_ARGS(&mD3DDevice)		// Returns the Direct3D device created.
            );
        if (FAILED(hr)) {
            // If the initialization fails, fall back to the WARP device.
            // For more information on WARP, see: 
            // http://go.microsoft.com/fwlink/?LinkId=286690

            ComPtr<IDXGIAdapter> warpAdapter;
            mDXFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter));
            D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3DDevice));
            DXGI_ADAPTER_DESC aDesc;
            warpAdapter->GetDesc(&aDesc);
            adapterName = aDesc.Description;
        }
        char* utf8Name = convertUnicodeToUTF8(adapterName);
        Peach3DLog(LogLevel::eInfo, "Adapter : %s", utf8Name);
        free(utf8Name);

		// Create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		mD3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue));

		ComPtr<ID3D12CommandAllocator>	commandAllocators[gDXFrameCount];
		for (UINT n = 0; n < gDXFrameCount; n++) {
			mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[n]));
		}
		// Create synchronization objects.
		mD3DDevice->CreateFence(mFenceValues[mCurrentFrame], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
		mFenceValues[mCurrentFrame]++;

		mFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        return true;
    }

    bool RenderDX::initRender(const Vector2& size, DXGI_MODE_ROTATION rotation)
    {
        // call base init
        IRender::initRender(size);
//        // init widget manager
//        mWidgetMgr = new WidgetManagerDX(mD3DDevice, mDeviceContext);
//
//        mIsRenderValid = false;
//#if (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
//        // Windows Phone not support resize swapchain
//        mSwapChain = nullptr;
//#endif
//        // reset window dependent handlers
//        ID3D11RenderTargetView* nullViews[] = {nullptr};
//        mDeviceContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
//        mTargetView = nullptr;
//        mDepthStencilView = nullptr;
//        mDeviceContext->Flush();
//
//        const PlatformCreationParams& params = IPlatform::getSingletonPtr()->getCreationParams();
//        if (mSwapChain != nullptr)
//        {
//            // resize chain if exist
//            HRESULT hr = mSwapChain->ResizeBuffers(2, width,
//                height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
//
//            if (FAILED(hr))
//            {
//                if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
//                {
//                    // device lost, need recreate gloabel device and window deivce
//                }
//                Peach3DLog(LogLevel::eError, "Create swap chain failed!");
//                return false;
//            }
//        }
//        else
//        {
//            // check support MSAA level
//            int MSAA = params.MSAA;
//            int MSAAQuality = checkMSAASupportQulityLevel(MSAA);
//            MSAA = (MSAAQuality > 0) ? MSAA : 0;
//
//            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
//            swapChainDesc.Width = width;
//            swapChainDesc.Height = height;
//            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//            swapChainDesc.Stereo = false;
//            swapChainDesc.SampleDesc.Count = 1 + MSAA;
//            swapChainDesc.SampleDesc.Quality = MSAAQuality;
//            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//            swapChainDesc.BufferCount = 2;
//            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
//            swapChainDesc.Flags = 0;
//            swapChainDesc.Scaling = DXGI_SCALING_NONE;
//            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
//
//            // get dxgi factory2
//            ComPtr<IDXGIDevice3> dxgiDevice;
//            mD3DDevice.As(&dxgiDevice);
//            ComPtr<IDXGIAdapter> dxgiAdapter;
//            dxgiDevice->GetAdapter(&dxgiAdapter);
//            ComPtr<IDXGIFactory2> dxgiFactory;
//            dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
//            // create new swap chain
//#if (PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_WINSTORE)
//			IUnknown* coreWindow = reinterpret_cast<IUnknown*>(params.window);
//            HRESULT hr = dxgiFactory->CreateSwapChainForCoreWindow(mD3DDevice.Get(), coreWindow, &swapChainDesc, nullptr, &mSwapChain);
//#else if (PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_WINDESK)
//			HWND window = (HWND)params.window;
//			HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(mD3DDevice.Get(), window, &swapChainDesc, NULL, NULL, &mSwapChain);
//#endif
//            if (FAILED(hr))
//            {
//                Peach3DLog(LogLevel::eError, "Create swap chain failed!");
//                return false;
//            }
//            // 确保 DXGI 不会一次对多个帧排队。这样既可以减小延迟，
//            // 又可以确保应用程序将只在每个 VSync 之后呈现，从而最大程度地降低功率消耗。
//            dxgiDevice->SetMaximumFrameLatency(1);
//        }
//
//        // create render target
//        ComPtr<ID3D11Texture2D> backBuffer;
//        mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
//        HRESULT hr = mD3DDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &mTargetView);
//        if (FAILED(hr))
//        {
//            Peach3DLog(LogLevel::eError, "Create render target failed!");
//            return false;
//        }
//
//        // get used depth stencil format
//        DXGI_FORMAT dsFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//        if (params.zBits == 16 && params.sBits == 0 )
//        {
//            dsFormat = DXGI_FORMAT_D16_UNORM;
//        }
//        // create texture2d for depth and stencil
//        CD3D11_TEXTURE2D_DESC depthStencilDesc(dsFormat,
//            width, height, 1, 1, D3D11_BIND_DEPTH_STENCIL);
//        ComPtr<ID3D11Texture2D> depthStencil;
//        mD3DDevice->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);
//        // create depth stencil view
//        CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
//        hr = mD3DDevice->CreateDepthStencilView(depthStencil.Get(), &depthStencilViewDesc, &mDepthStencilView);
//        if (FAILED(hr))
//        {
//            Peach3DLog(LogLevel::eError, "Create depth stencil failed!");
//            return false;
//        }
//
//        // set viewport
//        CD3D11_VIEWPORT viewport = CD3D11_VIEWPORT(0.0f, 0.0f, float(width), float(height));
//        mDeviceContext->RSSetViewports(1, &viewport);
//
//        mRasterizerDesc.FillMode = D3D11_FILL_SOLID;
//        mRasterizerDesc.CullMode = D3D11_CULL_BACK;
//        mRasterizerDesc.FrontCounterClockwise = TRUE;
//        mRasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
//        mRasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
//        mRasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
//        mRasterizerDesc.DepthClipEnable = TRUE;
//        mRasterizerDesc.ScissorEnable = FALSE;
//        mRasterizerDesc.MultisampleEnable = params.MSAA > 0;
//        mRasterizerDesc.AntialiasedLineEnable = FALSE;
//        // modify cull mode same as opengl es
//        ID3D11RasterizerState *rasterizerState = nullptr;
//        mD3DDevice->CreateRasterizerState(&mRasterizerDesc, &rasterizerState);
//        mDeviceContext->RSSetState(rasterizerState);
//
//        // set default depth and stencil desc
//        mDepthStencilDesc.DepthEnable = true;
//        mDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
//        mDepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
//        mDepthStencilDesc.StencilEnable = true;
//        mDepthStencilDesc.StencilReadMask = 0xFF;
//        mDepthStencilDesc.StencilWriteMask = 0xFF;
//        mDepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//        mDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
//        mDepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//        mDepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//        mDepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
//        mDepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
//        mDepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
//        mDepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
//        ID3D11DepthStencilState * pDSState = nullptr;
//        mD3DDevice->CreateDepthStencilState(&mDepthStencilDesc, &pDSState);
//        mDeviceContext->OMSetDepthStencilState(pDSState, 1);
//
//        // set blend state
//        D3D11_BLEND_DESC blendState;
//        blendState.AlphaToCoverageEnable = FALSE;
//        blendState.IndependentBlendEnable = FALSE;
//        blendState.RenderTarget[0].BlendEnable = TRUE;
//        blendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//        blendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//        blendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//        blendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//        blendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//        blendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//        blendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//        ID3D11BlendState *pBState = nullptr;
//        mD3DDevice->CreateBlendState(&blendState, &pBState);
//        mDeviceContext->OMSetBlendState(pBState, NULL, 0xffffffff);
//
//        // Create a D2D and WIC factory.
//        if (!createD2DWICFactory()) {
//            return false;
//        }
        Peach3DLog(LogLevel::eInfo, "Render window's width %.0f, height %.0f", size.x, size.y);
        // Render initialize success ^-^
        mIsRenderValid = true;
        return true;
    }

    bool RenderDX::createD2DWICFactory()
    {
        // Create a Direct2D factory.
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &mD2DFactory);
        if (FAILED(hr) || !mD2DFactory) {
            Peach3DLog(LogLevel::eError, "Create Direct2D factory failed!");
            return false;
        }
        // Create a DWirte factory.
        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&mDWriteFactory));
        if (FAILED(hr) || !mDWriteFactory) {
            Peach3DLog(LogLevel::eError, "Create DWirte factory failed!");
            return false;
        }
        // 
#if (PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_WINDESK)
        // COM initialize, use multi thread
        hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
        if (FAILED(hr)) {
            Peach3DLog(LogLevel::eError, "Call CoInitializeEx failed");
            return false;
        }
#endif
        hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
            CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory), (LPVOID*)&mWICImageFactory);
        if (FAILED(hr) || !mWICImageFactory) {
            Peach3DLog(LogLevel::eError, "Call CoCreateInstance failed");
            return false;
        }
        return true;
    }

    void RenderDX::prepareForRender()
    {
        // Specify the render target we created as the output target.
        //mDeviceContext->OMSetRenderTargets(1, mTargetView.GetAddressOf(), mDepthStencilView.Get());
        //// Clear the render target to a solid color.
        //mDeviceContext->ClearRenderTargetView(mTargetView.Get(), (float*)&mRenderClearColor);
        //// is need clean stencil
        //static bool isStencilEnabled = (IPlatform::getSingleton().getCreationParams().sBits > 0);
        //UINT clearBit = D3D11_CLEAR_DEPTH;
        //if (isStencilEnabled)
        //{
        //    clearBit = clearBit | D3D11_CLEAR_STENCIL;
        //}
        //mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), clearBit, 1.0f, 0);
    }

    void RenderDX::prepareForObjectRender()
    {
        // open depth
        //ID3D11DepthStencilState * pDSState = nullptr;
        //mDepthStencilDesc.DepthEnable = true;
        //mD3DDevice->CreateDepthStencilState(&mDepthStencilDesc, &pDSState);
        //mDeviceContext->OMSetDepthStencilState(pDSState, 1);
        //// disable scissor after widget render
    }

    void RenderDX::prepareForWidgetRender()
    {
        // close depth, pixel not rely on z-pos
        //ID3D11DepthStencilState * pDSState = nullptr;
        //mDepthStencilDesc.DepthEnable = false;
        //mD3DDevice->CreateDepthStencilState(&mDepthStencilDesc, &pDSState);
        //mDeviceContext->OMSetDepthStencilState(pDSState, 1);
        //// enable scissor in widget rendering
    }

    void RenderDX::finishForRender()
    {
        // the first param is svnc
        HRESULT hr = mSwapChain->Present(1, 0);
        // discard view content
        //mDeviceContext->DiscardView(mTargetView.Get());
        //mDeviceContext->DiscardView(mDepthStencilView.Get());

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // device lost, need recreate gloabel device and window deivce
        }
    }

    void RenderDX::updateGlobalUnifroms(RenderObjectAttr* attrs)
    {
        // call program to update global UBO
        ProgramDX::updateGlobalUnifroms(attrs);
    }

    ITexture* RenderDX::createTexture(const char* name)
    {
        Peach3DAssert(strlen(name)>0, "The texture name can't be null");
        TextureDX* texture = new TextureDX(mD3DDevice, name);
        return texture;
    }

    IProgram* RenderDX::createProgram(uint pId)
    {
        ProgramDX* program = new ProgramDX(mD3DDevice, pId);
        return program;
    }

    IObject* RenderDX::createObject(const char* objectName)
    {
        ObjectDX* object = new ObjectDX(mD3DDevice, objectName);
        return object;
    }

    int RenderDX::checkMSAASupportQulityLevel(int MSAA)
    {
        UINT qulityLevels = 0;
        // return support MSAA qulity level for MSAA count
        //mD3DDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, MSAA + 1, &qulityLevels);
        return 0;
    }

    void RenderDX::getObjectPresetVSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms)
    {/*
        uint vertexType = params[0];
        bool scrollEnabled = params[2] < params[1]; // enable uv scroll if scrolltexindex is valid
        std::vector<std::string> codeList;
        // add global uniform block
        codeList.push_back("cbuffer GlobalConstantBuffer : register(b0)\n"
            "{\n"
            "    matrix pd_projMatrix;\n"
            "    matrix pd_viewMatrix;\n"
            "    float4 pd_ambient;\n"
            "};\n");
        // add object uniform block
        codeList.push_back("cbuffer ObjectConstantBuffer : register(b1)\n"
            "{\n"
            "    matrix pd_modelMatrix;\n");
        uniforms->push_back(ProgramUniform(pdShaderModelMatrixUniformName, UniformType::eMatrix4));
        if ((vertexType & VertexTypeUV) && scrollEnabled)
        {
            codeList.push_back("    float2 pd_scrollUV;\n");
            uniforms->push_back(ProgramUniform(pdShaderScrollUVUniformName, UniformType::eVector2));
        }
        codeList.push_back("};\n");
        // add vertex attri
        codeList.push_back("struct VertexShaderInput\n"
            "{\n");
        codeList.push_back("    float3 pos : pd_vertex;\n");
        if (vertexType & VertexTypeColor4)
        {
            codeList.push_back("    float4 color : pd_color;\n");
        }
        if (vertexType & VertexTypeNormal)
        {
            codeList.push_back("    float3 normal : pd_normal;\n");
        }
        if (vertexType & VertexTypeUV)
        {
            codeList.push_back("    float2 uv : pd_uv;\n");
        }
        codeList.push_back("};\n");
        // add out put uniform
        codeList.push_back("struct PixelShaderInput\n"
            "{\n"
            "    float4 pos : SV_POSITION;\n"
            "    float4 color : COLOR0;\n");
        if (vertexType & VertexTypeUV)
        {
            codeList.push_back("    float2 uv : TEXCOORD0;\n");
            if (scrollEnabled)
            {
                codeList.push_back("    float2 uvOffset : UVSCROLL;\n");
            }
        }
        codeList.push_back("};\n");
        // add main func
        codeList.push_back("PixelShaderInput main(VertexShaderInput input)\n"
            "{\n"
            "    PixelShaderInput vertexShaderOutput;\n"
            "    float4 pos = float4(input.pos, 1.0f);\n"
            "    pos = mul(pos, pd_modelMatrix);\n"
            "    pos = mul(pos, pd_viewMatrix);\n"
            "    vertexShaderOutput.pos = mul(pos, pd_projMatrix);\n");
        if (vertexType & VertexTypeColor4)
        {
            codeList.push_back("    vertexShaderOutput.color = lerp(pd_ambient, input.color, input.color.a);\n");
        }
        else
        {
            codeList.push_back("    vertexShaderOutput.color = pd_ambient;\n");
        }
        if (vertexType & VertexTypeUV)
        {
            if (scrollEnabled)
            {
                codeList.push_back("    vertexShaderOutput.uvOffset = pd_scrollUV;\n");
            }
            codeList.push_back("    vertexShaderOutput.uv = input.uv;\n");
        }
        // add main end
        codeList.push_back("    return vertexShaderOutput;\n"
            "}\n");
        std::for_each(codeList.begin(), codeList.end(), [&](const std::string &piece){ *code += piece; });
        Peach3DLog(LogLevel::eInfo, "vertex code: \n%s", code->c_str());*/
    }

    void RenderDX::getObjectPresetPSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms)
    {
        uint vertexType = params[0];
        uint texCount = params[1];
        bool scrollEnabled = params[2] < params[1]; // enable uv scroll if scrolltexindex is valid
        std::vector<std::string> codeList;
        // add input struct
        codeList.push_back("struct PixelShaderInput\n"
            "{\n"
            "    float4 pos : SV_POSITION;\n"
            "    float4 color : COLOR0;\n");
        if (vertexType & VertexTypeUV)
        {
            codeList.push_back("    float2 uv : TEXCOORD0;\n");
            if (scrollEnabled)
            {
                codeList.push_back("    float2 uvOffset : UVSCROLL;\n");
            }
        }
        codeList.push_back("};\n");
        // add texture sampler
        const char* texNames[] = { pdShaderTexture0UniformName, pdShaderTexture1UniformName,
            pdShaderTexture2UniformName, pdShaderTexture3UniformName };
        for (uint i = 0; i<texCount; ++i)
        {
            char lineCode[100] = {};
            sprintf(lineCode, "Texture2D %s : register(t%d);\nSamplerState pd_sampler%d : register(s%d);\n",
                texNames[i], i, i, i);
            codeList.push_back(lineCode);
        }
        // add main func
        codeList.push_back("float4 main(PixelShaderInput input) : SV_TARGET\n"
            "{\n");
        if (texCount > 0 && (vertexType & VertexTypeUV))
        {
            if (scrollEnabled)
            {
                codeList.push_back("    float2 tex_uv = input.uv + input.uvOffset;\n");
                //SHADER_CLAMP_UNIFORM_0_1(tex_uv.x)
                //SHADER_CLAMP_UNIFORM_0_1(tex_uv.y)
            }

            codeList.push_back("    float4 out_color = input.color;\n");
            for (uint i = 0; i<texCount; ++i)
            {
                // use orign uv if current is not scrolled texture
                char* uvName = params[2] == i ? "tex_uv" : "input.uv";
                char lineCode[100] = {};
                sprintf(lineCode, "    float4 tex_color%d = pd_texture%d.Sample(pd_sampler%d, %s);\n", i, i, i, uvName);
                codeList.push_back(lineCode);
                sprintf(lineCode, "    out_color = lerp(out_color, tex_color%d, tex_color%d.a);\n", i, i);
                codeList.push_back(lineCode);
            }
            codeList.push_back("    return out_color;\n");
        }
        else {
            codeList.push_back("    return input.color;\n");
        }
        // add main end
        codeList.push_back("}\n");
        std::for_each(codeList.begin(), codeList.end(), [&](const std::string &piece){ *code += piece; });
        Peach3DLog(LogLevel::eInfo, "pixel code: \n%s", code->c_str());
    }
}
