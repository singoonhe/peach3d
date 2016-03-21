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
        //! declare class RenderGL is friend class, so RenderGL can call constructor function.
        friend class RenderGL;
    public:
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
        
        /** Update RenderNode unifroms for 3d object material. */
        virtual void updateRenderNodeUnifroms(RenderNode* node);
        /** Update lights uniform buffer for 3d object. */
        void updateObjectLightsUniforms(const std::vector<Light*>& lights);
        /** Update instanced RenderNode unifroms depend on mProgramUniformList. */
        virtual void updateInstancedRenderNodeUnifroms(const std::vector<RenderNode*>& renderList);
        
        /** Update widget unifroms for 2d object, include matrix/textures... */
        virtual void updateWidgetUnifroms(Widget* widget);
        /** Update instanced widgets unifroms depend on mProgramUniformList. */
        virtual void updateInstancedWidgetUnifroms(const std::vector<Widget*>& renderList);
        
        /** Update OBB unifroms for GL2. */
        virtual void updateOBBUnifroms(OBB* obb);
        /** Update instanced OBB unifroms depend on mProgramUniformList. */
        virtual void updateInstancedOBBUnifroms(const std::vector<OBB*>& renderList);
        
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
        static void updateGlobalObjectUnifroms();
        /** Update widget global uniform buffer, only RenderGL call it for GL3. */
        static void updateGlobalWidgetUnifroms();
        /** Delete all global uniform buffer, only RenderGL call it for GL3. */
        static void deleteGlobalUBO();
        
        //! set uniform value in shader, used for GL2 and GL3
        void setUnifromLocationValue(const std::string& name, std::function<void(GLint)> valueFunc);
        
    private:
        ProgramGL(uint pId) : IProgram(pId), mVSShader(0), mPSShader(0), mProgram(0), mAttriBuffer(0), mLightsCount(0),
            mInstancedCount(0), mUniformsSize(0), mLightsUBOId(GL_INVALID_INDEX), mLightsUBOSize(0) {}
        virtual ~ProgramGL();
        
    protected:
        GLuint  mVSShader;
        GLuint  mPSShader;
        GLuint  mProgram;
        
        std::map<std::string, GLint>    mUniformLocateMap;  // uniform location map
        
        GLuint  mAttriBuffer;           // object render attributes buffer for GL3
        uint    mInstancedCount;        // current instance draw count for GL3
        uint    mUniformsSize;          // once instanced uniforms buffer size for GL3
        
        int     mLightsCount;           // lights count
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
    };
}

#endif // PEACH3D_PROGRAMGL_H
