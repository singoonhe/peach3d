//
//  Peach3DIProgram.h
//  TestPeach3D
//
//  Created by singoon he on 12-5-17.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_IPROGRAM_H
#define PEACH3D_IPROGRAM_H

#include "Peach3DCompile.h"
#include "Peach3DVector2.h"
#include "Peach3DRect.h"
#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // define the common shader vertex name, VertexType::Point2 or VertexType::Point3
    const char pdShaderVertexAttribName[]     = "pd_vertex";
    // define the common shader color name
    const char pdShaderColorAttribName[]      = "pd_color";
    // define the common shader normal name
    const char pdShaderNormalAttribName[]     = "pd_normal";
    // define the common shader point sprite name
    const char pdShaderPSpriteAttribName[]  = "pd_pSprite";
    // define the common shader texture coord name
    const char pdShaderUVAttribName[]         = "pd_uv";
    // define the common shader bone widget name
    const char pdShaderBoneWidgetAttribName[]   = "pd_bWidget";
    // define the common shader bone index name
    const char pdShaderBoneIndexAttribName[]   = "pd_bIndex";
    
    // enum attrs bind location for GL3
    enum class PEACH3D_DLL DefaultAttrLocation
    {
        eVertex,    // position attr in vertex, float2 or float3
        eColor,     // color attr in vertex, float4
        eNormal,    // normal attr in vertex, flaot3
        ePSprite,   // point sprite attr in vertex, float2(size+rotation)
        eUV,        // uv attr in vertex, float2
        eBWidget,   // bone widget attr in vertex, float4
        eBIndex,    // bone index attr in vertex, float4
    };
    
    class OBB;
    class Widget;
    class RenderNode;
    class PEACH3D_DLL IProgram
    {
    public:
        //! Create program by IRender, user can't call constructor function.
        IProgram(uint pId) :mProgramId(pId), mProgramValid(false), mVertexType(0), mLightsCount(0), mShadowCount(0), mBoneCount(0) {}
        virtual ~IProgram() {}
        
        /** Set vertex shader, program will valid when vs and ps all is set. */
        virtual bool setVertexShader(const char* data, int size, bool isCompiled=false) = 0;
        /** Set pixel shader, program will valid when vs and ps all is set. */
        virtual bool setPixelShader(const char* data, int size, bool isCompiled=false) = 0;
        /** Set vertex data type, used to bind attr or layout. */
        virtual void setVertexType(uint type) { mVertexType = type; }
        /** Set user uniforms info, GL3 could get offset, DX need fcount to calc offset. */
        virtual void setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList) {mProgramUniformList = uniformList;}
        /** Set program lights count, auto enable lighting. */
        virtual void setLightsCount(uint count) { mLightsCount = count; }
        int getLightsCount() { return mLightsCount; }
        /** Set program shadow count. */
        virtual void setShadowCount(uint count) { mShadowCount = count; }
        int getShadowCount() { return mShadowCount; }
        /** Set program bones count. */
        virtual void setBoneCount(uint count) { mBoneCount = count; }
        int getBoneCount() { return mBoneCount; }
        
        /** Update RenderNode uniforms for 3d GL2 object material. */
		virtual void updateRenderNodeUniforms(RenderNode* node) {}
        /** Update instanced RenderNodes uniforms depend on mProgramUniformList. */
        virtual void updateInstancedRenderNodeUniforms(const std::vector<RenderNode*>& renderList) = 0;
        
        /** Update widget uniforms for 2d GL2 object, include matrix/textures... */
		virtual void updateWidgetUniforms(Widget* widget) {}
        /** Update instanced widgets uniforms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUniforms(const std::vector<Widget*>& renderList) = 0;
        
        /** Update OBB uniforms for GL2. */
        virtual void updateOBBUniforms(OBB* obb) {};
        /** Update instanced OBB uniforms depend on mProgramUniformList. */
        virtual void updateInstancedOBBUniforms(const std::vector<OBB*>& renderList) = 0;
        
        /** Update 2d particle uniforms, include rect/textures... */
        virtual void updateParticle2DUniforms(const Rect& coord) = 0;
        /** Update 3d particle uniforms, include rect/textures... */
        virtual void updateParticle3DUniforms(const Rect& coord) = 0;
        
        /** Set current object use this program. */
        virtual bool useAsRenderProgram() = 0;
        //! Return this program unique id
        virtual uint getProgramId() { return mProgramId; }
        //! Return program is valid
        virtual bool isProgramValid() { return mProgramValid; }

    protected:
        uint        mProgramId;     // program unique id
        bool        mProgramValid;  // is program valid
        uint        mVertexType;    // vertex data type
        int         mLightsCount;   // lights count
        int         mShadowCount;   // lights shadow count
        int         mBoneCount;     // skeleton bone count
        
        std::vector<ProgramUniform>  mProgramUniformList; // all the program uniforms in shader
    };
    
    // make shared program simple
    using ProgramPtr = std::shared_ptr<IProgram>;
}


#endif // PEACH3D_IPROGRAM_H
