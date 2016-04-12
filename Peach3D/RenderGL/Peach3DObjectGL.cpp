//
//  Peach3DObjectGL.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DObjectGL.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DVector4.h"
#include "Peach3DProgramGL.h"
#include "Peach3DRenderGL.h"
#include "Peach3DTextureGL.h"
#include "Peach3DSprite.h"
#include "Peach3DUtils.h"
#include "Peach3DRenderNode.h"
#include "Peach3DSceneManager.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    /** Convert Node draw mode to OpenGL mode. */
    static GLenum convertDrawModeToGL(DrawMode mode)
    {
        GLenum resMode = GL_TRIANGLES;
        switch (mode) {
            case DrawMode::ePoint:
                resMode = GL_POINTS;
                break;
            case DrawMode::eLine:
                resMode = GL_LINE_STRIP;
                break;
            default:
                break;
        }
        return resMode;
    }
    
    /** Special program used for base rendering. */
    ProgramPtr ObjectGL::mBaseProgram = nullptr;
    
    ObjectGL::ObjectGL(const char* name):IObject(name),mVertexBuffer(0),mIndexBuffer(0)
    {
        // create base program if not exist for OBB and shadow
        if (!mBaseProgram) {
            mBaseProgram = ResourceManager::getSingleton().getPresetProgram(PresetProgramFeatures(true, false));
        }
    }
    
    void ObjectGL::generateProgramVertexArray(const ProgramPtr& program)
    {
        GLuint programId = program ? program->getProgramId() : 0;
        // check is new VAO for program need
        if (mVAOMap.find(programId) == mVAOMap.end()) {
            GLuint vaoId = 0;
            glGenVertexArrays(1, &vaoId);
            glBindVertexArray(vaoId);
            // bind vertex buffer and index buffer
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
            bindObjectVertexAttrib();
            // bind program buffer for GL3
            if (program && PD_RENDERLEVEL_GL3()) {
                static_cast<ProgramGL*>(program.get())->bindProgramVertexAttrib();
            }
            mVAOMap[programId] = vaoId;
        }
        else {
            // also bind vertex array
            glBindVertexArray(mVAOMap[programId]);
        }
    }
    
    void ObjectGL::bindObjectVertexAttrib()
    {
        int offset = 0;
        const std::vector<VertexAttrInfo>& infoList = ResourceManager::getVertexAttrInfoList();
        // bind data offset to vertex array
        for (auto info=infoList.begin(); info!=infoList.end(); ++info) {
            uint typeValue = info->type;
            if (mVertexDataType & typeValue) {
                glEnableVertexAttribArray(static_cast<GLuint>(info->locate));
                // strde must set here. The buffer will not tightly packed when vertex have much type
                glVertexAttribPointer(static_cast<GLuint>(info->locate), info->size/sizeof(float), GL_FLOAT, GL_FALSE, mVertexDataStride, PEACH3D_BUFFER_OFFSET(offset));
                offset += info->size;
            }
        }
    }

    bool ObjectGL::setVertexBuffer(const void* data, uint size, uint type)
    {
        // base object setVertexBuffer
        bool result = IObject::setVertexBuffer(data, size, type);
		// delete old vertex buffer
		cleanObjectVertexBuffer();
        
        // create vertex buffer
        if (data && size>0 && result) {
            glGenBuffers(1, &mVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        return result && mVertexBuffer;
    }
    
    void ObjectGL::setIndexBuffer(const void*data, uint size, IndexType type)
    {
        // base object setIndexBuffer
        IObject::setIndexBuffer(data, size, type);
        // delete old index buffer
        cleanObjectIndexBuffer();
        
        // create index buffer
        if (data && size>0) {
            glGenBuffers(1, &mIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    
    void ObjectGL::render(const std::vector<RenderNode*>& renderList)
    {
        size_t listSize = renderList.size();
        Peach3DAssert(listSize > 0, "Can't render empty scene node list.");
        do {
            IF_BREAK(listSize == 0, nullptr);
            
            RenderNode* firstNode = renderList[0];
            auto isRenderShadow = firstNode->isRenderShadow();
            ProgramPtr usedProgram = isRenderShadow ? mBaseProgram : firstNode->getProgramForRender();
            IF_BREAK(!usedProgram || !usedProgram->useAsRenderProgram(), nullptr);
            ProgramGL* usedProgramGL = (ProgramGL*)usedProgram.get();
            if (PD_RENDERLEVEL_GL3()) {
                // update instanced uniforms
                usedProgram->updateInstancedRenderNodeUniforms(renderList);
                // set lighting unfo when render color
                auto lights = firstNode->getRenderLights();
                if (!isRenderShadow && lights.size() > 0) {
                    usedProgramGL->updateObjectLightsUniforms(lights);
                    // set shadow if need
                    auto shadows = firstNode->getShadowLights();
                    if (shadows.size() > 0) {
                        usedProgramGL->updateObjectShadowsUniforms(shadows);
                    }
                }
            }
            
            // bind vertex and index
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                generateProgramVertexArray((PD_RENDERLEVEL_GL3()) ? usedProgram : nullptr);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                bindObjectVertexAttrib();
            }
            
            // enable render state, flip using shader
            bool isTextureUsed = false;
            if (!isRenderShadow) {
                auto firMat = firstNode->getMaterial();
                auto mTexCount = firMat.getTextureCount();
                for (auto i = 0; i < mTexCount; i++) {
                    auto texGL = static_cast<TextureGL*>(firMat.textureList[i].get());
                    if (texGL) {
                        usedProgramGL->activeTextures(texGL->getGLTextureId(), i);
                        isTextureUsed = true;
                    }
                }
                // active shadow texture if need
                auto shadows = firstNode->getShadowLights();
                for (auto i = 0; i < shadows.size(); i++) {
                    auto texGL = static_cast<TextureGL*>(shadows[i]->getShadowTexture().get());
                    if (texGL) {
                        usedProgramGL->activeTextures(texGL->getGLTextureId(), mTexCount + i, Utils::formatString("pd_shadowTexture[%d]", i));
                        isTextureUsed = true;
                    }
                }
            }
            
            GLenum indexType = (mIndexDataType == IndexType::eUShort) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
            GLsizei indexCount = (mIndexDataType == IndexType::eUShort) ? mIndexBufferSize/sizeof(ushort) : mIndexBufferSize/sizeof(uint);
            GLenum glDrawMode = convertDrawModeToGL(firstNode->getDrawMode());
            // rendering
            if (PD_RENDERLEVEL_GL3()) {
                // draw objects once
                glDrawElementsInstanced(glDrawMode, indexCount, indexType, 0, (GLsizei)listSize);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE((indexCount * (GLsizei)listSize) / 3);
            }
            else {
                for (size_t i = 0; i < listSize; ++i) {
                    // update current widget uniforms
                    usedProgram->updateRenderNodeUniforms(renderList[i]);
                    // draw one widget
                    glDrawElements(glDrawMode, indexCount, indexType, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(indexCount / 3);
                }
            }
            // disable render state
            if (!isRenderShadow && isTextureUsed) {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            
            // unbind vertex and textures
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } while(0);
    }
    
    void ObjectGL::render(const std::vector<Widget*>& renderList)
    {
        size_t listSize = renderList.size();
        Peach3DAssert(listSize > 0, "Can't render empty widget node list.");
        do {
            IF_BREAK(listSize == 0, nullptr);
            
            Widget* firstNode = renderList[0];
            ProgramPtr usedProgram = firstNode->getProgramForRender();
            IF_BREAK(!usedProgram || !usedProgram->useAsRenderProgram(), nullptr);
            if (PD_RENDERLEVEL_GL3()) {
                // update instanced uniforms
                usedProgram->updateInstancedWidgetUniforms(renderList);
            }
            
            // bind vertex and index
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                generateProgramVertexArray((PD_RENDERLEVEL_GL3()) ? usedProgram : nullptr);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                bindObjectVertexAttrib();
            }
            
            // enable render state, flip using shader
            Sprite* texSprite = dynamic_cast<Sprite*>(firstNode);
            if (texSprite) {
                auto stex = texSprite->getTextureFrame().tex;
                if (stex) {
                    GLuint glTextureId = static_cast<TextureGL*>(stex.get())->getGLTextureId();
                    static_cast<ProgramGL*>(usedProgram.get())->activeTextures(glTextureId, 0);
                }
            }
            
            // rendering
            if (PD_RENDERLEVEL_GL3()) {
                // draw widget once
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, (GLsizei)listSize);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE((GLsizei)listSize * 2);
            }
            else {
                for (size_t i = 0; i < listSize; ++i) {
                    // update current widget uniforms
                    usedProgram->updateWidgetUniforms(renderList[i]);
                    // draw one widget
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(2);
                }
            }
            // disable render state
            if (texSprite && texSprite->getTextureFrame().tex) {
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            
            // unbind vertex and textures
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } while(0);
    }
    
    void ObjectGL::render(const std::vector<OBB*>& renderList)
    {
        size_t listSize = renderList.size();
        Peach3DAssert(listSize > 0, "Can't render empty OBB node list.");
        do {
            IF_BREAK(listSize == 0, nullptr);
            
            IF_BREAK(!mBaseProgram || !mBaseProgram->useAsRenderProgram(), nullptr);
            if (PD_RENDERLEVEL_GL3()) {
                // update instanced uniforms
                mBaseProgram->updateInstancedOBBUniforms(renderList);
            }
            
            // bind vertex and index
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                generateProgramVertexArray((PD_RENDERLEVEL_GL3()) ? mBaseProgram : nullptr);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                bindObjectVertexAttrib();
            }
            
            // rendering
            if (PD_RENDERLEVEL_GL3()) {
                // draw OBB once
                glDrawElementsInstanced(GL_LINES, mIndexBufferSize/sizeof(ushort), GL_UNSIGNED_SHORT, 0, (GLsizei)listSize);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE((GLsizei)listSize * 2);
            }
            else {
                for (size_t i = 0; i < listSize; ++i) {
                    // update current OBB uniforms
                    mBaseProgram->updateOBBUniforms(renderList[i]);
                    // draw one OBB
                    glDrawElements(GL_LINES, mIndexBufferSize/sizeof(ushort), GL_UNSIGNED_SHORT, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(2);
                }
            }
            
            // unbind vertex and textures
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } while(0);
    }
    
    void ObjectGL::cleanObjectVertexBuffer()
    {
        if (mVertexBuffer)
        {
            // delete vertex object
            glDeleteBuffers(1, &mVertexBuffer);
            mVertexBuffer = 0;
            // set vertex size and stride to zero
            mVertexBufferSize = 0;
            mVertexDataStride = 0;
            
            // delete all vertex array
            if (PD_GLEXT_VERTEXARRAY_SUPPORT() && mVAOMap.size() > 0) {
                for (auto vao : mVAOMap) {
                    glDeleteVertexArrays(1, &vao.second);
                }
                mVAOMap.clear();
            }
        }
    }
    
    void ObjectGL::cleanObjectIndexBuffer()
    {
        if (mIndexBuffer) {
            // delete index object
            glDeleteBuffers(1, &mIndexBuffer);
            mIndexBuffer = 0;
            // set index size and stride to zero
            mIndexBufferSize = 0;
            
            // delete all vertex array
            if (PD_GLEXT_VERTEXARRAY_SUPPORT() && mVAOMap.size() > 0) {
                for (auto vao : mVAOMap) {
                    glDeleteVertexArrays(1, &vao.second);
                }
                mVAOMap.clear();
            }
        }
    }
    
    ObjectGL::~ObjectGL()
    {
        // delete gl object
        cleanObjectIndexBuffer();
        cleanObjectVertexBuffer();
    }
}