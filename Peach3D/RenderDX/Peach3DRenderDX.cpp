
#include "d3dx12.h"
#include "Peach3DRenderDX.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DProgramDX.h"
#include "Peach3DObjectDX.h"
#include "Peach3DTextureDX.h"
#include "Peach3DIPlatform.h"
#include "Peach3DResourceManager.h"
#include "WinUwp/Peach3DPlatformWinUwp.h"

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
        // choose best adapter for desk, support DX12 hardware first
        UINT i = 0;
        WCHAR* adapterName = L"";
        IDXGIAdapter1 *pAdapter = nullptr, *choosedAdapter = nullptr;
        while (mDXFactory->EnumAdapters1(i++, &pAdapter) != DXGI_ERROR_NOT_FOUND) {
            DXGI_ADAPTER_DESC1 pDesc;
            pAdapter->GetDesc1(&pDesc);
            // Don't select the Basic Render Driver adapter.
            if (pDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue;
            }
            bool dx12Supported = SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
            if (dx12Supported) {
                choosedAdapter = pAdapter;
                adapterName = pDesc.Description;
                break;
            }
        }
        // Create the Direct3D 12 API device object
        HRESULT hr = D3D12CreateDevice(
            choosedAdapter,					// Using choosed adapter.
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

		for (UINT n = 0; n < gDXFrameCount; n++) {
			mD3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocators[n]));
		}
		// Create synchronization objects.
		mD3DDevice->CreateFence(mFenceValues[mCurrentFrame], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence));
		mFenceValues[mCurrentFrame]++;
		mFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

        // Create a root signature with a single constant buffer slot.
        {
            CD3DX12_DESCRIPTOR_RANGE range;
            CD3DX12_ROOT_PARAMETER parameter;
            range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
            parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
            CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
            descRootSignature.Init(1, &parameter, 0, nullptr, rootSignatureFlags);

            ComPtr<ID3DBlob> pSignature;
            ComPtr<ID3DBlob> pError;
            D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
            mD3DDevice->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&mRootSignature));
        }

        // Create a command list.
        mD3DDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocators[mCurrentFrame].Get(), nullptr, IID_PPV_ARGS(&mCommandList));
        mCommandList->Close();
        return true;
    }

    bool RenderDX::initRender(const Vector2& size, DXGI_MODE_ROTATION rotation)
    {
        // call base init
        IRender::initRender(size);
        // Wait until all previous GPU work is complete.
        waitForGpu();

        // Clear the previous window size specific content.
        for (UINT n = 0; n < gDXFrameCount; n++) {
            mRenderTargets[n] = nullptr;
        }
        mRtvHeap = nullptr;

        if (mSwapChain != nullptr) {
            // If the swap chain already exists, resize it.
            HRESULT hr = mSwapChain->ResizeBuffers(gDXFrameCount, lround(size.x), lround(size.y),
                DXGI_FORMAT_B8G8R8A8_UNORM, 0);

            if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
                // If the device was removed for any reason, a new device and swap chain will need to be created.
                static_cast<PlatformWinUwp*>(IPlatform::getSingletonPtr())->displayInvalidExit();
                // Do not continue execution of this method. DeviceResources will be destroyed and re-created.
                return false;
            }
            else if (FAILED(hr)) {
                // some error occur
            }
        }
        else {
            // Otherwise, create a new one using the same adapter as the existing Direct3D device.
            DXGI_SCALING scaling = DisplayMetrics::SupportHighResolutions ? DXGI_SCALING_NONE : DXGI_SCALING_STRETCH;
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
            swapChainDesc.Width = lround(size.x);	// Match the size of the window.
            swapChainDesc.Height = lround(size.y);
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;			// This is the most common swap chain format.
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;							// Don't use multi-sampling.
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            swapChainDesc.BufferCount = gDXFrameCount;					// Use triple-buffering to minimize latency.
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	// All Windows Universal apps must use _FLIP_ SwapEffects
            swapChainDesc.Flags = 0;
            swapChainDesc.Scaling = scaling;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

            ComPtr<IDXGISwapChain1> swapChain;
            auto uwpWindow = IPlatform::getSingleton().getCreationParams().window;
            mDXFactory->CreateSwapChainForCoreWindow(mCommandQueue.Get(), reinterpret_cast<IUnknown*>(uwpWindow),
                &swapChainDesc, nullptr, &swapChain);
            swapChain.As(&mSwapChain);
        }

        // Set the proper orientation for the swap chain, and generate
        // 3D matrix transformations for rendering to the rotated swap chain.
        // The 3D matrix is specified explicitly to avoid rounding errors.
        if (rotation == DXGI_MODE_ROTATION_ROTATE180) {
            mOrientationMatrix = Matrix4::createRotationZ(DEGREE_TO_RADIANS(180.0f));
        }
        else {
            mOrientationMatrix = Matrix4();
        }
        mSwapChain->SetRotation(rotation);

        // Create a render target view of the swap chain back buffer.
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.NumDescriptors = gDXFrameCount;
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            mD3DDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&mRtvHeap));
            mRtvHeap->SetName(L"Render Target View Descriptor Heap");

            mCurrentFrame = 0;
            CD3DX12_CPU_DESCRIPTOR_HANDLE rtvDescriptor(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
            mRtvDescriptorSize = mD3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            for (UINT n = 0; n < gDXFrameCount; n++) {
                mSwapChain->GetBuffer(n, IID_PPV_ARGS(&mRenderTargets[n]));
                mD3DDevice->CreateRenderTargetView(mRenderTargets[n].Get(), nullptr, rtvDescriptor);
                rtvDescriptor.Offset(mRtvDescriptorSize);

                WCHAR name[25];
                swprintf_s(name, L"Render Target %d", n);
                mRenderTargets[n]->SetName(name);
            }
        }

        // Create a depth stencil view.
        {
            D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
            dsvHeapDesc.NumDescriptors = 1;
            dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            mD3DDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&mDsvHeap));

            D3D12_HEAP_PROPERTIES depthHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            D3D12_RESOURCE_DESC depthResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
                static_cast<UINT>(size.x), static_cast<UINT>(size.y), 1, 0, 1, 0,
                D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

            D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
            depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
            depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
            depthOptimizedClearValue.DepthStencil.Stencil = 0;

            mD3DDevice->CreateCommittedResource(&depthHeapProperties, D3D12_HEAP_FLAG_NONE, &depthResourceDesc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&mDepthStencil));

            D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
            dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
            dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
            dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

            mD3DDevice->CreateDepthStencilView(mDepthStencil.Get(), &dsvDesc, mDsvHeap->GetCPUDescriptorHandleForHeapStart());
        }

        // All pending GPU work was already finished. Update the tracked fence values
        // to the last value signaled.
        for (UINT n = 0; n < gDXFrameCount; n++) {
            mFenceValues[n] = mFenceValues[mCurrentFrame];
        }

        Peach3DLog(LogLevel::eInfo, "Render window's width %.0f, height %.0f", size.x, size.y);
        // Render initialize success ^-^
        mIsRenderValid = true;
        return true;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE RenderDX::GetRenderTargetView() const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE resHandle = mRtvHeap->GetCPUDescriptorHandleForHeapStart();
        resHandle.ptr += mCurrentFrame * mRtvDescriptorSize;
        return resHandle;
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
        mCommandAllocators[mCurrentFrame]->Reset();
        // The command list can be reset anytime after ExecuteCommandList() is called.
        mCommandList->Reset(mCommandAllocators[mCurrentFrame].Get(), nullptr);

        // Set the graphics root signature to be used by this frame.
        mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
        // Set the viewport and scissor rectangle.
        auto winSize = IPlatform::getSingleton().getCreationParams().winSize;
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, winSize.x, winSize.y, 0.0f, 1.0f };
        mCommandList->RSSetViewports(1, &viewport);
        D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(winSize.x), static_cast<LONG>(winSize.y) };
        mCommandList->RSSetScissorRects(1, &scissorRect);

        // Indicate this resource will be in use as a render target.
        CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
            CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mCurrentFrame].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mCommandList->ResourceBarrier(1, &renderTargetResourceBarrier);

        // clear view
        FLOAT clearColor[] = { mRenderClearColor.r, mRenderClearColor.g, mRenderClearColor.b, mRenderClearColor.a };
        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentFrame, mRtvDescriptorSize);
        D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDsvHeap->GetCPUDescriptorHandleForHeapStart());
        mCommandList->ClearRenderTargetView(renderTargetView, clearColor, 0, nullptr);
        mCommandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
        mCommandList->OMSetRenderTargets(1, &renderTargetView, false, &depthStencilView);
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
        // Indicate that the render target will now be used to present when the command list is done executing.
        CD3DX12_RESOURCE_BARRIER presentResourceBarrier =
            CD3DX12_RESOURCE_BARRIER::Transition(mRenderTargets[mCurrentFrame].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        mCommandList->ResourceBarrier(1, &presentResourceBarrier);
        mCommandList->Close();

        // Execute the command list.
        ID3D12CommandList* ppCommandLists[] = { mCommandList.Get() };
        mCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // The first argument instructs DXGI to block until VSync, putting the application
        // to sleep until the next VSync. This ensures we don't waste any cycles rendering
        // frames that will never be displayed to the screen.
        HRESULT hr = mSwapChain->Present(1, 0);

        // If the device was removed either by a disconnection or a driver upgrade, we
        // must recreate all device resources.
        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            static_cast<PlatformWinUwp*>(IPlatform::getSingletonPtr())->displayInvalidExit();
        }
        else if (SUCCEEDED(hr)) {
            MoveToNextFrame();
        }
    }

    TexturePtr RenderDX::createTexture(const char* name)
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

    void RenderDX::waitForGpu()
    {
        // Schedule a Signal command in the queue.
        mCommandQueue->Signal(mFence.Get(), mFenceValues[mCurrentFrame]);

        // Wait until the fence has been crossed.
        mFence->SetEventOnCompletion(mFenceValues[mCurrentFrame], mFenceEvent);
        WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);

        // Increment the fence value for the current frame.
        mFenceValues[mCurrentFrame]++;
    }

    void RenderDX::MoveToNextFrame()
    {
        // Schedule a Signal command in the queue.
        const UINT64 currentFenceValue = mFenceValues[mCurrentFrame];
        mCommandQueue->Signal(mFence.Get(), currentFenceValue);

        // Advance the frame index.
        mCurrentFrame = (mCurrentFrame + 1) % gDXFrameCount;

        // Check to see if the next frame is ready to start.
        if (mFence->GetCompletedValue() < mFenceValues[mCurrentFrame])
        {
            mFence->SetEventOnCompletion(mFenceValues[mCurrentFrame], mFenceEvent);
            WaitForSingleObjectEx(mFenceEvent, INFINITE, FALSE);
        }

        // Set the fence value for the next frame.
        mFenceValues[mCurrentFrame] = currentFenceValue + 1;
    }

    /*
    void RenderDX::getObjectPresetVSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms)
    {
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
        Peach3DLog(LogLevel::eInfo, "vertex code: \n%s", code->c_str());
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
    }*/
}
