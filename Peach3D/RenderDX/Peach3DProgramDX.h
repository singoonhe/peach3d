#pragma once

#include "Peach3DCommonDX.h"
#include "Peach3DIProgram.h"

using namespace Microsoft::WRL;
namespace Peach3D
{
    class ProgramDX : public IProgram
    {
    public:
        ProgramDX(ComPtr<ID3D12Device> device, uint pId);
        virtual ~ProgramDX();
        // set vertex shader, program will valid when vs and ps all is set
        virtual bool setVertexShader(const char* data, int size, bool isCompiled = false);
        // set pixel shader, program will valid when vs and ps all is set
        virtual bool setPixelShader(const char* data, int size, bool isCompiled = false);
        // set vertex data type and instanced vertex type, used to create layout
        virtual void setVertexType(uint type, uint instancedType = 0);
        //! set user uniforms info, DX need fcount to calc offset
        virtual void setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList);

        //! Set current object use this program.
        virtual bool useAsRenderProgram();

        /** Update instanced RenderNode unifroms depend on mProgramUniformList. */
        virtual void updateInstancedRenderNodeUniforms(const std::vector<RenderNode*>& renderList);
        /** Update instanced widgets unifroms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUniforms(const std::vector<Widget*>& renderList);
        /** Update instanced OBB unifroms depend on mProgramUniformList. */
        virtual void updateInstancedOBBUniforms(const std::vector<OBB*>& renderList);

        //! delete global uniform buffer
        static void deleteGlobalUBO();

    protected:
        /** Compile vertex shader or pixel shader. */
        ID3DBlob* compileShader(const char* data, int size, const char* entryName, const char* targetName, bool isCompiled);
        //! Set vertex data stride.
        virtual void createInputLayout();
        //! Delete dx program.
        void cleanProgram();
        /** Update object global uniform buffer. */
        static void updateGlobalObjectUnifroms();
        /** Update widget global uniform buffer. */
        static void updateGlobalWidgetUnifroms();

    protected:
        ID3DBlob*   mVertexBlob;    // vertex compiled shader
        ID3DBlob*   mPixelBlob;     // pixel compiled shader
        ComPtr<ID3D12Device>          mD3DDevice;            // D3D11 device 
        ComPtr<ID3D11DeviceContext2>   mDeviceContext;        // D3D11 device context
        void*                          mVertexShaderData;     // vertex compiled shader data
        ulong                          mVertexShaderDataSize; // vertex compiled shader data length
        ID3D11PixelShader*             mPixelShader;          // D3D11 pixel shader
        ID3D11VertexShader*            mVertexShader;         // D3D11 vertex shader
        ID3D11InputLayout*             mInputLayout;          // D3D11 vertex input layout

        ID3D11Buffer*                  mOUniformBuffer;       // object own uniform buffer
        uint                           mObjectUboSize;        // object own uniform buffer size
        std::map<std::string, int>     mOUniformOffsetMap;    // object uniforms offset map
        static ID3D11Buffer*           mGUniformBuffer;       // global uniform buffer
        static uint                    mGlobalUboSize;         // object global uniform buffer size
        static std::map<std::string, int>   mGUniformOffsetMap;// global uniforms offset map
        static ComPtr<ID3D11DeviceContext2> mGlobalUpdateContext;// D3D11 device context for global UBO update
    };
}
