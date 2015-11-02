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

// define the common shader vertex name, VertexTypePosition2 or VertexTypePosition3
const char pdShaderVertexAttribName[]     = "pd_vertex";
// define the common shader color name
const char pdShaderColorAttribName[]      = "pd_color";
// define the common shader normal name
const char pdShaderNormalAttribName[]     = "pd_normal";
// define the common shader pSize name
const char pdShaderPSizeAttribName[]      = "pd_pSize";
// define the common shader texture coord name
const char pdShaderUVAttribName[]         = "pd_uv";
// define global uniforms
const char pdShaderProjMatrixUniformName[]    = "pd_projMatrix";
const char pdShaderViewMatrixUniformName[]    = "pd_viewMatrix";
const char pdShaderAmbientUniformName[]       = "pd_ambient";
// define object or widget unifroms
const char pdShaderModelMatrixUniformName[]   = "pd_modelMatrix";
// define object texture scroll UV name
const char pdShaderScrollUVUniformName[]      = "pd_scrollUV";
// define object textures
const char pdShaderTexture0UniformName[]      = "pd_texture0";
const char pdShaderTexture1UniformName[]      = "pd_texture1";
const char pdShaderTexture2UniformName[]      = "pd_texture2";
const char pdShaderTexture3UniformName[]      = "pd_texture3";

namespace Peach3D
{
    // enum attrs bind location for GL3
    enum class PEACH3D_DLL DefaultAttrLocation
    {
        eVertex,    // position attr in vertex
        eColor,     // color attr in vertex
        eNormal,    // normal attr in vertex
        ePSize,     // PSize attr in vertex
        eUV,        // uv attr in vertex
        eMatrix,    // matrix attr in vertex, this will talk 4 size
    };
    
    class Material;
    class Widget;
    class SceneNode;
    struct RenderObjectAttr;
    class PEACH3D_DLL IProgram
    {
    public:
        // set vertex shader, program will valid when vs and ps all is set
        virtual bool setVertexShader(const char* data, int size, bool isCompiled=false) = 0;
        // set pixel shader, program will valid when vs and ps all is set
        virtual bool setPixelShader(const char* data, int size, bool isCompiled=false) = 0;
        // set vertex data type, used to bind attr or layout
        virtual void setVertexType(uint type) { mVertexType = type; }
        //! set user uniforms info, GL3 could get offset, DX need fcount to calc offset
        virtual void setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList) {mProgramUniformList = uniformList;}

        /**
         * @brief Update object uniforms for 3d object.
         * @params attrs Object rendering attrs, include much matrixs(projective/camera/model).
         * @params mtl Object material, include light params and textures. If it's nullptr, only update attrs uniforms.
         * @params lastFrameTime Auto update uniforms param.
         */
        virtual void updateObjectUnifroms(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime) = 0;
        
        /** Update SceneNode unifroms for 3d object material. */
        virtual void updateSceneNodeUnifroms(SceneNode* node) = 0;
        /** Update instanced SceneNodes unifroms depend on mProgramUniformList. */
        virtual void updateInstancedSceneNodeUnifroms(std::vector<SceneNode*> renderList) = 0;
        
        /** Update widget unifroms for 2d object, include matrix/textures... */
        virtual void updateWidgetUnifroms(Widget* widget) = 0;
        /** Update instanced widgets unifroms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUnifroms(std::vector<Widget*> renderList) = 0;
        
        /** Set current object use this program. */
        virtual bool useAsRenderProgram() = 0;
        //! Return this program unique id
        virtual uint getProgramId() { return mProgramId; }
        //! Return program is valid
        virtual bool isProgramValid() { return mProgramValid; }

    protected:
        //! Create program by IRender, user can't call constructor function.
        IProgram(uint pId) :mProgramId(pId), mProgramValid(false), mVertexType(0) {}
        //! Delete program by IRender, user can't call destructor function.
        virtual ~IProgram() {}
        
    protected:
        uint        mProgramId;     // program unique id
        bool        mProgramValid;  // is program valid
        uint        mVertexType;    // vertex data type
        
        std::vector<ProgramUniform>  mProgramUniformList; // all the program uniforms in shader
        friend class IRender;       //! Declare class IRender is friend class
    };
}


#endif // PEACH3D_IPROGRAM_H
