//
//  Peach3DProgramGL.h
//  TestPeach3D
//
//  Created by singoon he on 12-5-17.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PROGRAMGL_H
#define PEACH3D_PROGRAMGL_H

#include "Peach3DCommonGL.h"
#include "Peach3DIProgram.h"
#include "Peach3DLight.h"

namespace Peach3D
{
    class ProgramGL : public IProgram
    {
    public:
        //! Create program by IRender, user can't call constructor function.
        ProgramGL(uint pId) : IProgram(pId), mVSShader(0), mPSShader(0), mProgram(0), mAttriBuffer(0),
            mInstancedCount(0), mUniformsSize(0), mLightsUBOId(GL_INVALID_INDEX), mLightsUBOSize(0) {}
        virtual ~ProgramGL();
        
        // set vertex shader, program will valid when vs and ps all is set
        virtual bool setVertexShader(const char* data, int size, bool isCompiled=false);
        // set pixel shader, program will valid when vs and ps all is set
        virtual bool setPixelShader(const char* data, int size, bool isCompiled=false);
        // set vertex data type, used to bind attr
        virtual void setVertexType(uint type);
        //! set user uniforms info, GL3 could get offset, DX need fcount to calc offset
        virtual void setProgramUniformsDesc(const std::vector<ProgramUniform>& uniformList);
        /** Set program lights count, auto enable lighting. */
        virtual void setLightsCount(uint count);
        
        /** Bind instance vertex attrib, used for GL3. */
        void bindProgramVertexAttrib();
        //! Set current object use this program.
        virtual bool useAsRenderProgram();
        /** Active textures, index from 0. */
        void activeTextures(GLuint texId, uint index);
        
        /** Update RenderNode uniforms for 3d object material. */
        virtual void updateRenderNodeUniforms(RenderNode* node);
        /** Update lights uniform buffer for 3d object. */
        void updateObjectLightsUniforms(const std::vector<LightPtr>& lights);
        /** Update instanced RenderNode uniforms depend on mProgramUniformList. */
        virtual void updateInstancedRenderNodeUniforms(const std::vector<RenderNode*>& renderList);
        
        /** Update widget uniforms for 2d object, include matrix/textures... */
        virtual void updateWidgetUniforms(Widget* widget);
        /** Update instanced widgets uniforms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUniforms(const std::vector<Widget*>& renderList);
        
        /** Update OBB uniforms for GL2. */
        virtual void updateOBBUniforms(OBB* obb);
        /** Update instanced OBB uniforms depend on mProgramUniformList. */
        virtual void updateInstancedOBBUniforms(const std::vector<OBB*>& renderList);
        
    protected:
        /** Compile program, this will be called automatically. */
        void compileProgram();
        /** Bind system attrs, live position\color\normal and so on... */
        void bindProgramAttrs();
        /** Create vertex shader or pixel from memeory. */
        GLuint loadShaderFromMemory(GLenum type, const char* data, int size);
        
        /** Begin map instance buffer to write for GL3. */
        float* beginMapInstanceUniformBuffer(uint count);
        /** End map instance buffer for GL3. */
        void endMapInstanceUniformBuffer();
        /** Bind uniforms buffer, used for GL3. */
        void bindUniformsBuffer(const char* uName, GLuint* UBOId, GLint* UBOSize, std::vector<ProgramUniform>* uniforms, GLint index);
        
        /** Update object global uniform buffer, only RenderGL call it for GL3. */
        static void updateGlobalObjectUniforms();
        /** Update widget global uniform buffer, only RenderGL call it for GL3. */
        static void updateGlobalWidgetUniforms();
        /** Delete all global uniform buffer, only RenderGL call it for GL3. */
        static void deleteGlobalUBO();
        
        //! set uniform value in shader, used for GL2 and GL3
        void setUniformLocationValue(const std::string& name, std::function<void(GLint)> valueFunc);
        
    protected:
        GLuint  mVSShader;
        GLuint  mPSShader;
        GLuint  mProgram;
        
        std::map<std::string, GLint>    mUniformLocateMap;  // uniform location map
        
        GLuint  mAttriBuffer;           // object render attributes buffer for GL3
        uint    mInstancedCount;        // current instance draw count for GL3
        uint    mUniformsSize;          // once instanced uniforms buffer size for GL3
        
        GLuint  mLightsUBOId;           // object lights uniform buffer id for GL3
        GLint   mLightsUBOSize;         // object lights uniform buffer size for GL3
        std::vector<ProgramUniform>     mLightsUBOUniforms;

        
        static GLuint   mWidgetUBOId;   // widget global uniform buffer id for GL3
        static GLint    mWidgetUBOSize; // widget global uniform buffer size for GL3
        static std::vector<ProgramUniform>  mWidgetUBOUniforms;
        static GLuint   mObjectUBOId;   // object global uniform buffer id for GL3
        static GLint    mObjectUBOSize; // object global uniform buffer size for GL3
        static std::vector<ProgramUniform>  mObjectUBOUniforms;
        
        std::function<uint(const std::string&, std::vector<Widget*>, float*, uint)> mGL3ExtendFunc;
        std::function<uint(const std::string&, Widget*)> mGL2ExtendFunc;
        friend class RenderGL;          // Declare class RenderGL is friend class
    };
}

#endif // PEACH3D_PROGRAMGL_H
