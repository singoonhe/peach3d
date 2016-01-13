//
//  Peach3DObjectGL.h
//  TestPeach3D
//
//  Created by singoon he on 12-6-7.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_OBJECTGL_H
#define PEACH3D_OBJECTGL_H

#include "Peach3DCommonGL.h"
#include "Peach3DIObject.h"
#include "Peach3DIPlatform.h"

namespace Peach3D
{
    class ObjectGL : public IObject
    {
        //! declare class RenderGL is friend class, so RenderGL can call constructor function.
        friend class RenderGL;
    public:
        ObjectGL(const char* name);
        virtual ~ObjectGL();
        
        /**
         * See IObject::setVertexBuffer. 
         */
        virtual bool setVertexBuffer(const void* data, uint size, uint type);
        /**
        * See IObject::setIndexBuffer.
        */
        virtual void setIndexBuffer(const void*data, uint size, IndexType type = IndexType::eUShort);
        
        /**
         * @brief Render widget list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<Widget*>& renderList);
        /**
         * @brief Render scene node list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<RenderNode*>& renderList);
        /**
         * @brief Render OBB list, only for GL3 and DX, called by SceneManager.
         */
        virtual void render(const std::vector<OBB*>& renderList);
        
    protected:
        /**
         * Generate or bind VAO for programId, each program need a new VAO for DrawInstance.
         * Every program will generate a VBO for DrawInstance.
         */
        void generateProgramVertexArray(GLuint programId);
        /**
         * Bind object vertex attrib, once called if VertexArray supported.
         */
        void bindObjectVertexAttrib();
        /**
         * Delete gl object vertex buffer.
         */
        void cleanObjectVertexBuffer();
        /**
         * Delete gl object index buffer.
         */
        void cleanObjectIndexBuffer();
        /**
         * Generate AABB vertex buffers if SceneNode needed.
         */
        static void generateAABBBuffers();
        /**
         * @brief Render a AABB using object matrix, object AABB data use to scale global AABB.
         * @params attrs Object rendering attr, include matrixes.
         */
        // void renderAABB(RenderObjectAttr* attrs);
        /**
         * Delete AABB vertex buffers, it could be called by RenderGL.
         */
        static void deleteAABBBuffers();
        
        
    private:
        GLuint mVertexBuffer;   // object own vertex buffer
        GLuint mIndexBuffer;    // object own index buffer
        std::map<GLuint, GLuint> mVAOMap;   // each program need different VAO (program will create VBO, bind to VAO)
        
        static GLuint mAABBVertexArrayId;   // vertex array id for AABB rendering
        static GLuint mAABBVertexBuffer;    // vertex buffer for AABB rendering
        static GLuint mAABBIndexBuffer;     // vertex buffer for AABB rendering
        static IProgram* mAABBProgram;      // AABB rendering program
        static IProgram* mOBBProgram;       // OBB rendering program
    };
}

#endif // PEACH3D_OBJECTGL_H
