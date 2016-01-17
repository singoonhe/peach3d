
#include "Peach3DProgramDX.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DTextureDX.h"
#include "Peach3DWidget.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    ID3D11Buffer* ProgramDX::mGUniformBuffer = nullptr;
    ComPtr<ID3D11DeviceContext2> ProgramDX::mGlobalUpdateContext = NULL;
    uint ProgramDX::mGlobalUboSize = 0;
    std::map<std::string, int>   ProgramDX::mGUniformOffsetMap;

    ProgramDX::ProgramDX(ComPtr<ID3D12Device> device, uint pId)
        : IProgram(pId), mD3DDevice(device), mOUniformBuffer(nullptr),
        mPixelShader(nullptr), mVertexShader(nullptr), mInputLayout(nullptr), mVertexShaderData(nullptr)
    {
        // set global uniform offset, only init once
        if (mGUniformOffsetMap.size() == 0)
        {
            //mGlobalUpdateContext = context;
            //mGUniformOffsetMap[pdShaderProjMatrixUniformName] = 0;
            //mGUniformOffsetMap[pdShaderViewMatrixUniformName] = 16;
            //mGUniformOffsetMap[pdShaderAmbientUniformName] = 32;
            //mGlobalUboSize = (mGUniformOffsetMap[pdShaderAmbientUniformName] + 4) * sizeof(float);
        }
    }

    bool ProgramDX::setVertexShader(const char* data, int size, bool isCompiled)
    {/*
        if (mVertexShader)
        {
            // delete vertex shader when it's exist
            mVertexShader->Release();
            mVertexShader = nullptr;
            mProgramValid = false;
        }

        HRESULT hr = S_OK;
        if (isCompiled)
        {
            // direct create vertex shader from compiled data
            hr = mD3DDevice->CreateVertexShader(data, size, NULL, &mVertexShader);
            // save vertex compiled shader data
            mVertexShaderData = malloc(size);
            memcpy(mVertexShaderData, data, size);
            mVertexShaderDataSize = size;
        }
        else
        {
            ID3DBlob* shaderBlob = compileShader(data, size, "main", "vs_4_0_level_9_3");
            if (shaderBlob == nullptr)
            {
                return false;
            }
            // save vertex compiled shader data
            mVertexShaderData = malloc(shaderBlob->GetBufferSize());
            memcpy(mVertexShaderData, shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
            mVertexShaderDataSize = (ulong)shaderBlob->GetBufferSize();
            // create vertex shader if compile success
            hr = mD3DDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &mVertexShader);
            shaderBlob->Release();
        }
        if (FAILED(hr))
        {
            Peach3DLog(LogLevel::eError, "Create vertex shader for program %u failed", mProgramId);
            return false;
        }

        if (mPixelShader && mVertexType)
        {
            // pixel shader and mVertexType all are valid, create layout
            createInputLayout();
        }*/
        return true;
    }

    bool ProgramDX::setPixelShader(const char* data, int size, bool isCompiled)
    {/*
        if (mPixelShader)
        {
            // delete pixel shader when it's exist
            mPixelShader->Release();
            mPixelShader = nullptr;
            mProgramValid = false;
        }

        HRESULT hr = S_OK;
        if (isCompiled)
        {
            // direct create pixel shader from compiled data
            hr = mD3DDevice->CreatePixelShader(data, size, NULL, &mPixelShader);
        }
        else
        {
            ID3DBlob* shaderBlob = compileShader(data, size, "main", "ps_4_0_level_9_3");
            if (shaderBlob == nullptr)
            {
                return false;
            }
            // create pixel shader if compile success
            hr = mD3DDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), NULL, &mPixelShader);
            shaderBlob->Release();
        }
        if (FAILED(hr))
        {
            Peach3DLog(LogLevel::eError, "Create pixel shader for program %u failed", mProgramId);
            return false;
        }

        if (mVertexShader && mVertexType)
        {
            // vertex shader and mVertexType all are valid, create layout
            createInputLayout();
        }*/
        return true;
    }

    void ProgramDX::setVertexType(uint type, uint instancedType)
    {
        //IProgram::setVertexType(type, instancedType);
        //if (mVertexShader && mPixelShader)
        //{
        //    // vertex shader and pixel shader are valid, create layout
        //    createInputLayout();
        //}
    }

    void ProgramDX::setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList)
    {/*
        IProgram::setProgramUniformsDesc(uniformList);
        mOUniformOffsetMap.clear();

        // uniform type and size list
        static std::map<UniformType, int> uniformSizeMap;
        if (uniformSizeMap.size() == 0)
        {
            uniformSizeMap[UniformType::eFloat] = 1;
            uniformSizeMap[UniformType::eVector2] = 2;
            uniformSizeMap[UniformType::eVector3] = 3;
            uniformSizeMap[UniformType::eVector4] = 4;
            uniformSizeMap[UniformType::eMatrix4] = 16;
        }

        int offset = 0;
        // calc uniform offset and need buffer size
        // the rule of offset here : http://msdn.microsoft.com/en-us/library/windows/desktop/bb509632%28v=vs.85%29.aspx 
        int lastSize = 16;
        for (auto iter = mProgramUniformList.begin(); iter != mProgramUniformList.end(); ++iter)
        {
            // current uniform need size
            int typeSize = uniformSizeMap[iter->type] * 4;
            // if last size not enough, move offset
            if (lastSize < typeSize && lastSize < 16)
            {
                offset += 4 - (offset % 4);
                lastSize = 16;
            }
            mOUniformOffsetMap[iter->name] = offset;
            offset += uniformSizeMap[iter->type];
            // calc the last size for next uniform
            lastSize = lastSize - (typeSize % 16);
        }
        mObjectUboSize = offset * sizeof(float);
        // object constant buffer size must be a multiple of 16
        if (mObjectUboSize%16 != 0)
        {
            mObjectUboSize += 16 - (mObjectUboSize % 16);
        }
        // bind object uniforms when program have valid
        if (mProgramValid && uniformList.size() > 0)
        {
            bindProgramUniformsForObject();
        }*/
    }

    /*
    void ProgramDX::updateObjectUnifroms(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime)
    {
        if (mOUniformBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE subData;
            mDeviceContext->Map(mOUniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
            float* dataBuffer = (float*)subData.pData;
            for (auto iter = mProgramUniformList.begin(); iter != mProgramUniformList.end(); ++iter)
            {
                if (iter->name.compare(pdShaderModelMatrixUniformName) == 0)
                {
                    //memcpy(dataBuffer + mOUniformOffsetMap[iter->name], attrs->modelMatrix->getTranspose().mat, 16 * sizeof(float));
                }
                else if (iter->name.compare(pdShaderScrollUVUniformName) == 0 && mtl)
                {
                    // update texture uv coordinate
                    Vector2 curOffset = mtl->getUpdateUVScrollOffset(lastFrameTime);
                    memcpy(dataBuffer + mOUniformOffsetMap[iter->name], &curOffset, sizeof(float) * 2);
                }
            }
            mDeviceContext->Unmap(mOUniformBuffer, 0);

            // active textures
            if (mtl) {
                mtl->tranverseTextures([&](int index, ITexture* texture){
                    TextureDX* dxTexture = static_cast<TextureDX*>(texture);
                    if (dxTexture)
                    {
                        // bind one texture
                        ID3D11ShaderResourceView* dxTexRV = dxTexture->getTextureRV();
                        ID3D11SamplerState* dxTexState = dxTexture->getTextureState();
                        mDeviceContext->PSSetShaderResources(index, 1, &dxTexRV);
                        mDeviceContext->PSSetSamplers(index, 1, &dxTexState);
                    }
                });
            }
        }
    }*/

    /*
    void ProgramDX::updateWidgetUnifroms(Widget* widget)
    {
        if (mOUniformBuffer)
        {
            D3D11_MAPPED_SUBRESOURCE subData;
            mDeviceContext->Map(mOUniformBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subData);
            float* dataBuffer = (float*)subData.pData;
            for (auto iter = mProgramUniformList.begin(); iter != mProgramUniformList.end(); ++iter)
            {
                if (iter->name.compare(pdShaderScrollUVUniformName) == 0)
                {
                    Vector2 curOffset = widget->getCurUVScrollOffset();
                    memcpy(dataBuffer + mOUniformOffsetMap[iter->name], &curOffset, 2 * sizeof(float));
                }
            }
            mDeviceContext->Unmap(mOUniformBuffer, 0);
        }

        // active textures
        const std::vector<ITexture*>& texList = widget->getTextureList();
        for (size_t i = 0; i<texList.size(); ++i)
        {
            TextureDX* dxTexture = static_cast<TextureDX*>(texList[i]);
            if (dxTexture)
            {
                // bind one texture
                ID3D11ShaderResourceView* dxTexRV = dxTexture->getTextureRV();
                ID3D11SamplerState* dxTexState = dxTexture->getTextureState();
                mDeviceContext->PSSetShaderResources(UINT(i), 1, &dxTexRV);
                mDeviceContext->PSSetSamplers(UINT(i), 1, &dxTexState);
            }
        }
    }*/

    void ProgramDX::updateGlobalObjectUnifroms()
    {/*
        // create global uniform buffer
        if (mGUniformBuffer == nullptr)
        {
            CD3D11_BUFFER_DESC constantBufferDesc(mGlobalUboSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
            mD3DDevice->CreateBuffer(&constantBufferDesc, nullptr, &mGUniformBuffer);
            Peach3DLog(LogLevel::eInfo, "Create global UBO success for DX11");
        }*/
    }

    void ProgramDX::updateGlobalWidgetUnifroms()
    {
        // release object uniform buffer if has created
        //if (mOUniformBuffer)
        //{
        //    mOUniformBuffer->Release();
        //    mOUniformBuffer = nullptr;
        //}
        // create object uniform buffer
        //CD3D11_BUFFER_DESC constantBufferDesc(mObjectUboSize, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
        //mD3DDevice->CreateBuffer(&constantBufferDesc, nullptr, &mOUniformBuffer);
    }

	void ProgramDX::updateInstancedRenderNodeUnifroms(const std::vector<RenderNode*>& renderList)
	{
	}

	void ProgramDX::updateInstancedWidgetUnifroms(const std::vector<Widget*>& renderList)
	{
	}

    void ProgramDX::updateInstancedOBBUnifroms(const std::vector<OBB*>& renderList)
    {
    }

    void ProgramDX::deleteGlobalUBO()
    {
        if (mGUniformBuffer)
        {
            // delete global uniform
            mGUniformBuffer->Release();
            mGUniformBuffer = nullptr;
            mGlobalUboSize = 0;
        }
    }

    bool ProgramDX::useAsRenderProgram()
    {
        if (mProgramValid)
        {
            // set input layout
            mDeviceContext->IASetInputLayout(mInputLayout);
            if (mVertexType & VertexType::Point3)
            {
                // set constant buffer
                mDeviceContext->VSSetConstantBuffers(0, 1, &mGUniformBuffer);
                mDeviceContext->VSSetConstantBuffers(1, 1, &mOUniformBuffer);
            }
            else if (mOUniformBuffer)
            {
                mDeviceContext->VSSetConstantBuffers(0, 1, &mOUniformBuffer);
            }
            // Set the vertex and pixel shader stage state
            mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
            mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
        }
        return mProgramValid;
    }

    void ProgramDX::createInputLayout()
    {/*
        // check compiled vertex data
        if (!mVertexShaderData)
        {
            return ;
        }
        if (mInputLayout)
        {
            // delete vertex input layout
            mInputLayout->Release();
            mInputLayout = nullptr;
        }

        uint layoutOffset = 0;
        std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc;
        // set program vertex info
        const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
        uint perVertexType = mVertexType ^ mInstancedVertexType;
        // bind per vertex data offset to vertex array
        DXGI_FORMAT eleementFormat[] = { DXGI_FORMAT_R32_FLOAT, DXGI_FORMAT_R32G32_FLOAT, DXGI_FORMAT_R32G32B32_FLOAT, DXGI_FORMAT_R32G32B32A32_FLOAT };
        for (auto info = infoList.begin(); info != infoList.end(); ++info)
        {
            uint typeValue = (*info).type;
            if (perVertexType & typeValue)
            {
                int floatCount = info->size / sizeof(float);
                D3D11_INPUT_ELEMENT_DESC pointDesc = { info->name.c_str(), 0, eleementFormat[floatCount-1], 0, layoutOffset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
                layoutOffset += info->size;
                layoutDesc.push_back(pointDesc);
            }
        }
        layoutOffset = 0;
        // bind instance data offset to vertex array
        for (auto info = infoList.begin(); info != infoList.end(); ++info)
        {
            uint typeValue = (*info).type;
            if (mInstancedVertexType & typeValue)
            {
                int floatCount = info->size / sizeof(float);
                int perCount = (floatCount < 4) ? floatCount : 4;
                for (size_t i = 0; i < size_t(floatCount / 4); i++)
                {
                    D3D11_INPUT_ELEMENT_DESC pointDesc = { info->name.c_str(), (UINT)i, eleementFormat[perCount - 1], 1, layoutOffset, D3D11_INPUT_PER_INSTANCE_DATA, 1 };
                    layoutOffset += perCount * sizeof(float);
                    layoutDesc.push_back(pointDesc);
                }
            }
        }
        // create vertex layout
        HRESULT hr = mD3DDevice->CreateInputLayout(layoutDesc.data(), (UINT)layoutDesc.size(), mVertexShaderData, mVertexShaderDataSize, &mInputLayout);
        if (SUCCEEDED(hr))
        {
            mProgramValid = true;
            if (mVertexType & VertexTypePosition3)
            {
                // bind global uniform only for object
                bindGlobalUniformsForObject();
            }
            // bind program uniform if uniform desc exist
            if (mProgramUniformList.size() > 0)
            {
                bindProgramUniformsForObject();
            }
        }

        // free compiled shader data
        free(mVertexShaderData);
        mVertexShaderData = nullptr;
        mVertexShaderDataSize = 0;*/
    }

    ID3DBlob* ProgramDX::compileShader(const char* data, int size, const char* entryName, const char* targetName)
    {
        Peach3DAssert(strlen(data) > 0 && size > 0, "Can't compile invalid shader source");
        ID3DBlob* shaderBlob = nullptr;/*
        do
        {
            if (strlen(data) == 0 || size == 0)
            {
                Peach3DLog(LogLevel::eError, "Can't compile null shader source for program %u", mProgramId);
                break ;
            }

            UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if PEACH3D_DEBUG == 1
            flags |= D3DCOMPILE_DEBUG;
#endif
            ID3DBlob* errorBlob = nullptr;
            HRESULT hr = D3DCompile(data, size, NULL, NULL, NULL, entryName, targetName, flags, NULL, &shaderBlob, &errorBlob);
            // check compile result
            if (FAILED(hr))
            {
                Peach3DLog(LogLevel::eError, "Compile shader for program %u error : %s", mProgramId, errorBlob->GetBufferPointer());
                if (shaderBlob)
                {
                    shaderBlob->Release();
                    shaderBlob = nullptr;
                }
                errorBlob->Release();
            }
        } while (0);*/
        return shaderBlob;
    }

    void ProgramDX::cleanProgram()
    {
        if (mOUniformBuffer)
        {
            // delete object uniform buffer
            mOUniformBuffer->Release();
            mOUniformBuffer = nullptr;
        }
        if (mVertexShader)
        {
            // delete vertex shader when it's exist
            mVertexShader->Release();
            mVertexShader = nullptr;
        }
        if (mPixelShader)
        {
            // delete pixel shader when it's exist
            mPixelShader->Release();
            mPixelShader = nullptr;
        }
    }

    ProgramDX::~ProgramDX()
    {
        // delete gl program
        cleanProgram();
    }
}