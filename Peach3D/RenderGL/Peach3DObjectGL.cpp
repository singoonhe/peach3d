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
#include "Peach3DRenderNode.h"
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
    
    GLuint ObjectGL::mAABBVertexArrayId = 0;
    GLuint ObjectGL::mAABBVertexBuffer = 0;
    GLuint ObjectGL::mAABBIndexBuffer = 0;
    IProgram* ObjectGL::mAABBProgram = 0;
    
    ObjectGL::ObjectGL(const char* name):IObject(name),mVertexBuffer(0),mIndexBuffer(0),mVertexArrayId(0)
    {
    }
    
    void ObjectGL::generateObjectVertexArray()
    {
        if (PD_GLEXT_VERTEXARRAY_SUPPORT() && !mVertexArrayId) {
            glGenVertexArrays(1, &mVertexArrayId);
        }
        // also bind vertex array
        glBindVertexArray(mVertexArrayId);
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
            generateObjectVertexArray();
            
            glGenBuffers(1, &mVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

            // unbind vertex array
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                bindObjectVertexAttrib();
                glBindVertexArray(0);
            }
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
            generateObjectVertexArray();
            
            glGenBuffers(1, &mIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
            
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }
    
    void ObjectGL::render(const std::vector<RenderNode*>& renderList)
    {
        size_t listSize = renderList.size();
        Peach3DAssert(listSize > 0, "Can't render empty node list.");
        do {
            IF_BREAK(listSize == 0, nullptr);
            
            RenderNode* firstNode = renderList[0];
            IProgram* usedProgram = firstNode->getProgramForRender();
            IF_BREAK(!usedProgram || !usedProgram->useAsRenderProgram(), nullptr);
            
            // bind vertex and index
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(mVertexArrayId);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                bindObjectVertexAttrib();
            }
            
            // enable render state, flip using shader
            auto firMat = firstNode->getMaterial();
            for (auto i = 0; i < firMat.getTextureCount(); i++) {
                GLuint glTextureId = static_cast<TextureGL*>(firMat.textureList[i])->getGLTextureId();
                static_cast<ProgramGL*>(usedProgram)->activeTextures(glTextureId, i);
            }
            
            GLenum indexType = (mIndexDataType == IndexType::eUShort) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
            GLsizei indexCount = (mIndexDataType == IndexType::eUShort) ? mIndexBufferSize/sizeof(ushort) : mIndexBufferSize/sizeof(uint);
            GLenum glDrawMode = convertDrawModeToGL(firstNode->getDrawMode());
            // rendering
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
                // update instanced uniforms
                usedProgram->updateInstancedRenderNodeUnifroms(renderList);
                // draw objects once
                glDrawElementsInstanced(glDrawMode, indexCount, indexType, 0, (GLsizei)listSize);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE((indexCount * (GLsizei)listSize) / 3);
            }
            else {
                for (size_t i = 0; i < listSize; ++i) {
                    // update current widget uniforms
                    usedProgram->updateRenderNodeUnifroms(renderList[i]);
                    // draw one widget
                    glDrawElements(glDrawMode, indexCount, indexType, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(indexCount / 3);
                }
            }
            // disable render state
            if (firMat.getTextureCount() > 0) {
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
    /*
    void ObjectGL::render(RenderObjectAttr* attrs, Material* mtl, float lastFrameTime)
    {
        // check is need choose preset program
        IObject::render(attrs, mtl, lastFrameTime);
        
        // use as current render program
        if (mVertexBuffer && mRenderProgram && mRenderProgram->useAsRenderProgram()) {
            // bind vertex buffer and index buffer, set program uniforms
            this->bindBaseAttrBuffer(attrs, mtl, lastFrameTime);
            
            // enable depth bias
            float biasFactor = attrs->depthBias;
            if (biasFactor > FLT_EPSILON || biasFactor < -FLT_EPSILON) {
                glEnable(GL_POLYGON_OFFSET_FILL);
                float units = (biasFactor > FLT_EPSILON) ? 1.0f : -1.0f;
                glPolygonOffset(1.0f * biasFactor, units);
            }
            
            if (mIndexBuffer) {
                // draw as triangles when indexs exist
                if (mIndexDataType == IndexType::eUShort) {
                    glDrawElements(GL_TRIANGLES, mIndexBufferSize/sizeof(ushort), GL_UNSIGNED_SHORT, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(mIndexBufferSize/(sizeof(ushort)*3));
                }
                else if (mIndexDataType == IndexType::eUInt) {
                    glDrawElements(GL_TRIANGLES, mIndexBufferSize/sizeof(uint), GL_UNSIGNED_INT, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(mIndexBufferSize/(sizeof(uint)*3));
                }
            }
            else {
                // draw as trangles or trianglestrip if index not exist
                glDrawArrays(GL_TRIANGLES, 0, mVertexBufferSize/mVertexDataStride);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE(mVertexBufferSize/(mVertexDataStride*3));
            }
            // disable depth bias after rendering
            if (biasFactor > FLT_EPSILON || biasFactor < -FLT_EPSILON) {
                glDisable(GL_POLYGON_OFFSET_FILL);
            }
            
            // unbind vertex and textures
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            // check AABB need show
            if (attrs->showAABB) {
                renderAABB(attrs);
            }
        }
    }
    */
    void ObjectGL::render(const std::vector<Widget*>& renderList)
    {
        size_t listSize = renderList.size();
        Peach3DAssert(listSize > 0, "Can't render empty node list.");
        do {
            IF_BREAK(listSize == 0, nullptr);
            
            Widget* firstNode = renderList[0];
            IProgram* usedProgram = firstNode->getProgramForRender();
            IF_BREAK(!usedProgram || !usedProgram->useAsRenderProgram(), nullptr);
            
            // bind vertex and index
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(mVertexArrayId);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
                bindObjectVertexAttrib();
            }
            
            // enable render state, flip using shader
            Sprite* texSprite = dynamic_cast<Sprite*>(firstNode);
            if (texSprite && texSprite->getTexture()) {
                GLuint glTextureId = static_cast<TextureGL*>(texSprite->getTexture())->getGLTextureId();
                static_cast<ProgramGL*>(usedProgram)->activeTextures(glTextureId, 0);
            }
            
            GLenum glDrawMode = convertDrawModeToGL(firstNode->getDrawMode());
            // rendering
            if (PD_RENDERLEVEL() == RenderFeatureLevel::eGL3) {
                // update instanced uniforms
                usedProgram->updateInstancedWidgetUnifroms(renderList);
                // draw widget once
                glDrawElementsInstanced(glDrawMode, 6, GL_UNSIGNED_SHORT, 0, (GLsizei)listSize);
                PD_ADD_DRAWCALL(1);
                PD_ADD_DRAWTRIAGNLE((GLsizei)listSize * 2);
            }
            else {
                for (size_t i = 0; i < listSize; ++i) {
                    // update current widget uniforms
                    usedProgram->updateWidgetUnifroms(renderList[i]);
                    // draw one widget
                    glDrawElements(glDrawMode, 6, GL_UNSIGNED_SHORT, 0);
                    PD_ADD_DRAWCALL(1);
                    PD_ADD_DRAWTRIAGNLE(2);
                }
            }
            // disable render state
            if (texSprite && texSprite->getTexture()) {
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
    
    void ObjectGL::generateAABBBuffers()
    {
        /*
        // generate vertex buffer for AABB rendering
        if (!mAABBVertexBuffer && !mAABBIndexBuffer) {
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                // general vertex array
                glGenVertexArrays(1, &mAABBVertexArrayId);
                glBindVertexArray(mAABBVertexArrayId);
            }
            
            glGenBuffers(1, &mAABBVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, mAABBVertexBuffer);
            GLfloat globalVertex[] = {-0.5, -0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5, -0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5,  0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                -0.5,  0.5, -0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                -0.5, -0.5,  0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5, -0.5,  0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                0.5,  0.5,  0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a,
                -0.5,  0.5,  0.5, mAABBColor.r, mAABBColor.g, mAABBColor.b, mAABBColor.a};
            glBufferData(GL_ARRAY_BUFFER, sizeof(globalVertex), globalVertex, GL_STATIC_DRAW);
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                const GLsizei vStride = 7*sizeof(float);
                glEnableVertexAttribArray(static_cast<GLuint>(DefaultAttrLocation::eVertex));
                glVertexAttribPointer(static_cast<GLuint>(DefaultAttrLocation::eVertex), 3, GL_FLOAT, GL_FALSE, vStride, 0);
                glEnableVertexAttribArray(static_cast<GLuint>(DefaultAttrLocation::eColor));
                glVertexAttribPointer(static_cast<GLuint>(DefaultAttrLocation::eColor), 4, GL_FLOAT, GL_FALSE, vStride, PEACH3D_BUFFER_OFFSET(3 * sizeof(float)));
            }
            GLushort globalIndex[] = {0, 1, 1, 2, 2, 3, 0, 3,  4, 5, 5, 6, 6, 7, 4, 7,  0, 4, 1, 5, 2, 6, 3, 7};
            glGenBuffers(1, &mAABBIndexBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mAABBIndexBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(globalIndex), globalIndex, GL_STATIC_DRAW);
            
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            
            // set line width once
            glLineWidth(2.0f);
            // generate program
            Material AABBMtl;
//            mAABBProgram = ResourceManager::getSingleton().getObjectPresetProgram(VertexType::Point3|VertexTypeColor4, AABBMtl);
        }*/
    }
    
    /*
    void ObjectGL::renderAABB(RenderObjectAttr* attrs)
    {
        // generate AABB buffers if needed
        generateAABBBuffers();
        AABB nodeAABB = getAABB(*attrs->modelMatrix);
        if (nodeAABB.isValid()) {
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(mAABBVertexArrayId);
            }
            else {
                glBindBuffer(GL_ARRAY_BUFFER, mAABBVertexBuffer);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mAABBIndexBuffer);
                // attrib need always set if vertexarray disable
                const GLsizei vStride = 7*sizeof(float);
                glEnableVertexAttribArray(static_cast<GLuint>(DefaultAttrLocation::eVertex));
                glVertexAttribPointer(static_cast<GLuint>(DefaultAttrLocation::eVertex), 3, GL_FLOAT, GL_FALSE, vStride, 0);
                glEnableVertexAttribArray(static_cast<GLuint>(DefaultAttrLocation::eColor));
                glVertexAttribPointer(static_cast<GLuint>(DefaultAttrLocation::eColor), 4, GL_FLOAT, GL_FALSE, vStride, PEACH3D_BUFFER_OFFSET(3 * sizeof(float)));
            }
            
            // update AABB model matrix
            Vector3 AABBSize = Vector3(nodeAABB.max.x - nodeAABB.min.x,
                                       nodeAABB.max.y - nodeAABB.min.y,
                                       nodeAABB.max.z - nodeAABB.min.z);
            Vector3 AABBCenter = Vector3((nodeAABB.max.x + nodeAABB.min.x) / 2.0f,
                                         (nodeAABB.max.y + nodeAABB.min.y) / 2.0f,
                                         (nodeAABB.max.z + nodeAABB.min.z) / 2.0f);
            Matrix4 AABBTranslateMat = Matrix4::createTranslation(AABBCenter.x, AABBCenter.y, AABBCenter.z);
            Matrix4 AABBScaleMat = Matrix4::createScaling(AABBSize.x, AABBSize.y, AABBSize.z);
            Matrix4 AABBMat = AABBTranslateMat * AABBScaleMat;
            attrs->modelMatrix = &AABBMat;
            // set program
            mAABBProgram->useAsRenderProgram();
            mAABBProgram->updateObjectUnifroms(attrs, nullptr, 0.0f);
            // modelMatrix not enabled, it point to a local Matrix4.
            attrs->modelMatrix = nullptr;
            // draw AABB, DepthBias not supported when rendering LINE.
            glDrawElements(GL_LINES, 24, GL_UNSIGNED_SHORT, 0);
            PD_ADD_DRAWCALL(1);
            
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                glBindVertexArray(0);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }*/
    
    void ObjectGL::deleteAABBBuffers()
    {
        if (mAABBIndexBuffer)
        {
            // delete vertex object
            glDeleteBuffers(1, &mAABBIndexBuffer);
            mAABBIndexBuffer = 0;
            if (PD_GLEXT_VERTEXARRAY_SUPPORT())
            {
                // delete vertex array
                glDeleteVertexArrays(1, &mAABBVertexArrayId);
                mAABBVertexArrayId = 0;
            }
        }
        if (mAABBIndexBuffer)
        {
            // delete index object
            glDeleteBuffers(1, &mAABBIndexBuffer);
            mAABBIndexBuffer = 0;
        }
    }
    
    void ObjectGL::cleanObjectVertexBuffer()
    {
        if (mVertexBuffer)
        {
            // delete vertex object
            glDeleteBuffers(1, &mVertexBuffer);
            mVertexBuffer = 0;
            if (PD_GLEXT_VERTEXARRAY_SUPPORT()) {
                // delete vertex array
                glDeleteVertexArrays(1, &mVertexArrayId);
                mVertexArrayId = 0;
            }
            // set vertex size and stride to zero
            mVertexBufferSize = 0;
            mVertexDataStride = 0;
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
        }
    }
    
    ObjectGL::~ObjectGL()
    {
        // delete gl object
        cleanObjectIndexBuffer();
        cleanObjectVertexBuffer();
    }
}