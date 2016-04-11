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
#include "Peach3DShaderCode.h"

namespace Peach3D
{
    // define the common shader vertex name, VertexType::Point2 or VertexType::Point3
    const char pdShaderVertexAttribName[]     = "pd_vertex";
    // define the common shader normal name
    const char pdShaderNormalAttribName[]     = "pd_normal";
    // define the common shader pSize name
    const char pdShaderPSizeAttribName[]      = "pd_pSize";
    // define the common shader texture coord name
    const char pdShaderUVAttribName[]         = "pd_uv";
    
    // enum attrs bind location for GL3
    enum class PEACH3D_DLL DefaultAttrLocation
    {
        eVertex,    // position attr in vertex
        eNormal,    // normal attr in vertex
        ePSize,     // PSize attr in vertex
        eUV,        // uv attr in vertex
    };
    
    class OBB;
    class Widget;
    class RenderNode;
    class PEACH3D_DLL IProgram
    {
    public:
        //! Create program by IRender, user can't call constructor function.
        IProgram(uint pId) :mProgramId(pId), mProgramValid(false), mVertexType(0), mLightsCount(0), mShadowCount(0) {}
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
        
        /** Update RenderNode unifroms for 3d GL2 object material. */
		virtual void updateRenderNodeUnifroms(RenderNode* node) {}
        /** Update instanced RenderNodes unifroms depend on mProgramUniformList. */
        virtual void updateInstancedRenderNodeUnifroms(const std::vector<RenderNode*>& renderList) = 0;
        
        /** Update widget unifroms for 2d GL2 object, include matrix/textures... */
		virtual void updateWidgetUnifroms(Widget* widget) {}
        /** Update instanced widgets unifroms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUnifroms(const std::vector<Widget*>& renderList) = 0;
        
        /** Update OBB unifroms for GL2. */
        virtual void updateOBBUnifroms(OBB* obb) {};
        /** Update instanced OBB unifroms depend on mProgramUniformList. */
        virtual void updateInstancedOBBUnifroms(const std::vector<OBB*>& renderList) = 0;
        
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
        
        std::vector<ProgramUniform>  mProgramUniformList; // all the program uniforms in shader
    };
    
    // make shared program simple
    using ProgramPtr = std::shared_ptr<IProgram>;
}


#endif // PEACH3D_IPROGRAM_H
