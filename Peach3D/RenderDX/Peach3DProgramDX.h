#pragma once

#include "Peach3DCommonDX.h"
#include "Peach3DIProgram.h"

using namespace Microsoft::WRL;
namespace Peach3D
{
    class ProgramDX : public IProgram
    {
        //! Declare class RenderDX is friend class, so RenderDX can call constructor function.
        friend class RenderDX;
    public:
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
        virtual void updateInstancedRenderNodeUnifroms(const std::vector<RenderNode*>& renderList);
        /** Update instanced widgets unifroms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUnifroms(const std::vector<Widget*>& renderList);
        /** Update instanced OBB unifroms depend on mProgramUniformList. */
        virtual void updateInstancedOBBUnifroms(const std::vector<OBB*>& renderList);

    protected:
        ProgramDX(ComPtr<ID3D12Device> device, uint pId);
        virtual ~ProgramDX();

        //! compile shader and check result
        ID3DBlob* compileShader(const char* data, int size, const char* entryName, const char* targetName);
        //! Set vertex data stride.
        virtual void createInputLayout();
        //! Delete dx program.
        void cleanProgram();
        /** Update object global uniform buffer. */
        static void updateGlobalObjectUnifroms();
        /** Update widget global uniform buffer. */
        static void updateGlobalWidgetUnifroms();
        //! delete global uniform buffer
        static void deleteGlobalUBO();

    protected:
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
