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
#include "Peach3DParticle.h"

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
    ProgramPtr ObjectGL::mParticle2DProgram = nullptr;
    ProgramPtr ObjectGL::mParticle3DProgram = nullptr;
    
    ObjectGL::ObjectGL(const char* name, const char* meshName):IObject(name, meshName),mVertexBuffer(0),mIndexBuffer(0)
    {
        // create base program if not exist for OBB and shadow
        if (!mBaseProgram) {
            mBaseProgram = ResourceManager::getSingleton().getPresetProgram({{PROGRAM_FEATURE_POINT3, 1}, {PROGRAM_FEATURE_UV, 0}});
        }
        // create 2D particle program, point2|texture|particle
        if (!mParticle2DProgram) {
            mParticle2DProgram = ResourceManager::getSingleton().getPresetProgram({{PROGRAM_FEATURE_POINT3, 0}, {PROGRAM_FEATURE_UV, 1}, {PROGRAM_FEATURE_PARTICLE, 1}});
        }
        // create 3D particle program, point3|texture|particle
        if (!mParticle3DProgram) {
            mParticle3DProgram = ResourceManager::getSingleton().getPresetProgram({{PROGRAM_FEATURE_POINT3, 1}, {PROGRAM_FEATURE_UV, 1}, {PROGRAM_FEATURE_PARTICLE, 1}});
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
            // index buffer may not valid, such as Particle
            if (mIndexBuffer) {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
            }
            bindObjectVertexAttrib();
            // bind program buffer for GL3, Particle not use draw instance
            if (program) {
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

    bool ObjectGL::setVertexBuffer(const void* data, uint size, uint type, bool isDynamic)
    {
        // base object setVertexBuffer
        bool result = IObject::setVertexBuffer(data, size, type, isDynamic);
		// delete old vertex buffer
		cleanObjectVertexBuffer();
        
        // create vertex buffer
        if (data && size>0 && result) {
            glGenBuffers(1, &mVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, size, data, isDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
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
            auto isRenderShadow = firstNode->isRenderForShadow();
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
            
            // update bind skeleton info
            auto isSkeleton = firstNode->getAnimateName().length() > 0;
            if (isSkeleton) {
                firstNode->updateSkeletonAnimate();
                // update bone UBO
                if (PD_RENDERLEVEL_GL3()) {
                    auto& boneNames = firstNode->getObject()->getUsedBones();
                    usedProgramGL->updateObjectBoneUniforms(firstNode->getBindSkeleton(), boneNames);
                }
            }
            
            // enable render state, flip using shader
            bool isTextureUsed = false;
            if (!isRenderShadow) {
                int usedTexCount = 0;
                // active UV texture if need
                auto firMat = firstNode->getMaterial();
                for (auto i = 0; i < firMat.getTextureCount(); i++) {
                    auto texGL = static_cast<TextureGL*>(firMat.textureList[i].get());
                    if (texGL) {
                        usedProgramGL->activeTextures(texGL->getGLTextureId(), usedTexCount++);
                    }
                }
                // active shadow texture if need
                auto shadows = firstNode->getShadowLights();
                for (auto i = 0; i < shadows.size(); i++) {
                    auto texGL = static_cast<TextureGL*>(shadows[i]->getShadowTexture().get());
                    if (texGL) {
                        usedProgramGL->activeTextures(texGL->getGLTextureId(), usedTexCount++, Utils::formatString("pd_shadowTexture[%d]", i));
                    }
                }
                // signed texture used
                isTextureUsed = usedTexCount > 0;
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
    
    void ObjectGL::render(Particle2D* particle)
    {
        auto& emitters = particle->getEmitters();
        do {
            IF_BREAK(emitters.size() == 0, nullptr);
            // use particle2d program
            IF_BREAK(!mParticle2DProgram || !mParticle2DProgram->useAsRenderProgram(), nullptr);
            
            // must use VAO on MAC OpenGL core version
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                // do not use draw instance
                generateProgramVertexArray(nullptr);
                // bind vertex buffer, emitter data need update
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                bindObjectVertexAttrib();
            }
            
            for (auto& emit : emitters) {
                // active texture and update program uniforms
                auto& texFrame = emit.texFrame;
                if (texFrame.tex) {
                    GLuint glTextureId = static_cast<TextureGL*>(texFrame.tex.get())->getGLTextureId();
                    static_cast<ProgramGL*>(mParticle2DProgram.get())->activeTextures(glTextureId, 0);
                    // support texture plist, size must be power of 2
                    mParticle2DProgram->updateParticle2DUniforms(texFrame.rc);
                    // bind vertex and draw points
                    auto bufferSize = emit.getRenderBufferSize();
                    glBufferData(GL_ARRAY_BUFFER, bufferSize, emit.getRenderBuffer(), GL_DYNAMIC_DRAW);
                    glDrawArrays(GL_POINTS, 0, bufferSize / mVertexDataStride);
                    PD_ADD_DRAWCALL(1);
                }
            }
            // unbind vertex and textures
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        } while(0);
    }
    
    void ObjectGL::render(Particle3D* particle)
    {
        auto& emitters = particle->getEmitters();
        do {
            IF_BREAK(emitters.size() == 0, nullptr);
            // use particle3d program
            IF_BREAK(!mParticle3DProgram || !mParticle3DProgram->useAsRenderProgram(), nullptr);
            
            // must use VAO on MAC OpenGL core version
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                // do not use draw instance
                generateProgramVertexArray(nullptr);
                // bind vertex buffer, emitter data need update
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                bindObjectVertexAttrib();
            }
            
            for (auto& emit : emitters) {
                // active texture and update program uniforms
                auto& texFrame = emit.texFrame;
                if (texFrame.tex) {
                    GLuint glTextureId = static_cast<TextureGL*>(texFrame.tex.get())->getGLTextureId();
                    static_cast<ProgramGL*>(mParticle3DProgram.get())->activeTextures(glTextureId, 0);
                    // support texture plist, size must be power of 2
                    mParticle3DProgram->updateParticle3DUniforms(texFrame.rc);
                    // bind vertex and draw points
                    auto bufferSize = emit.getRenderBufferSize();
                    glBufferData(GL_ARRAY_BUFFER, bufferSize, emit.getRenderBuffer(), GL_DYNAMIC_DRAW);
                    glDrawArrays(GL_POINTS, 0, bufferSize / mVertexDataStride);
                    PD_ADD_DRAWCALL(1);
                }
            }
            // unbind vertex and textures
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        } while(0);
    }
    
    void ObjectGL::render(Terrain* terr)
    {
        do {
            ProgramPtr usedProgram = terr->getProgramForRender();
            IF_BREAK(!usedProgram || !usedProgram->useAsRenderProgram(), nullptr);
            // bind vertex and index, not use draw instance
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                generateProgramVertexArray(nullptr);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                bindObjectVertexAttrib();
            }
            
            ProgramGL* usedProgramGL = (ProgramGL*)usedProgram.get();
            int usedTexCount = 0;
            // active UV texture
            auto brushes = terr->getBrushes();
            for (auto i = 0; i < brushes.size(); i++) {
                auto texGL = static_cast<TextureGL*>(brushes[i].get());
                if (texGL) {
                    usedProgramGL->activeTextures(texGL->getGLTextureId(), usedTexCount++, Utils::formatString("pd_texture[%d]", i));
                }
            }
            // active alpha map texture
            auto mapTes = terr->getAlphaMaps();
            for (auto i = 0; i < mapTes.size(); i++) {
                auto texGL = static_cast<TextureGL*>(mapTes[i].get());
                if (texGL) {
                    usedProgramGL->activeTextures(texGL->getGLTextureId(), usedTexCount++, Utils::formatString("pd_alphaMap%d", i));
                }
            }
            // active shadow texture if need
            auto shadows = terr->getShadowLights();
            for (auto i = 0; i < shadows.size(); i++) {
                auto texGL = static_cast<TextureGL*>(shadows[i]->getShadowTexture().get());
                if (texGL) {
                    usedProgramGL->activeTextures(texGL->getGLTextureId(), usedTexCount++, Utils::formatString("pd_shadowTexture[%d]", i));
                }
            }
            
            GLenum indexType = (mIndexDataType == IndexType::eUShort) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
            GLsizei indexCount = (mIndexDataType == IndexType::eUShort) ? mIndexBufferSize/sizeof(ushort) : mIndexBufferSize/sizeof(uint);
            GLenum glDrawMode = convertDrawModeToGL(terr->getDrawMode());
            // update current widget uniforms
            usedProgram->updateTerrainUniforms(terr);
            // draw one widget
            glDrawElements(glDrawMode, indexCount, indexType, 0);
            PD_ADD_DRAWCALL(1);
            PD_ADD_DRAWTRIAGNLE(indexCount / 3);
            // disable render state
            glBindTexture(GL_TEXTURE_2D, 0);
            
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
        if (mVertexBuffer) {
            // delete vertex object
            glDeleteBuffers(1, &mVertexBuffer);
            mVertexBuffer = 0;
            // set vertex size and stride to zero
            mVertexBufferSize = 0;
            mVertexDataStride = 0;
            
            // delete all vertex array
            if (PD_GLEXT_VERTEXARRAY_SUPPORT() && mVAOMap.size() > 0) {
                for (auto& vao : mVAOMap) {
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
                for (auto& vao : mVAOMap) {
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
