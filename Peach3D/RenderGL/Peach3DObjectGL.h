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
    public:
        // constructor and destructor must be public, because shared_ptr need call them
        ObjectGL(const char* name);
        virtual ~ObjectGL();
        
        /**
         * See IObject::setVertexBuffer. 
         */
        virtual bool setVertexBuffer(const void* data, uint size, uint type, bool isDynamic = false);
        /**
         * @brief Reset vertex buffer data for render object.
         */
        virtual bool resetVertexBuffer(const void* data, uint size);
        /**
        * See IObject::setIndexBuffer.
        */
        virtual void setIndexBuffer(const void*data, uint size, IndexType type = IndexType::eUShort);
        
        /**
         * @brief Render widget list, called by SceneManager.
         */
        virtual void render(const std::vector<Widget*>& renderList);
        /**
         * @brief Render scene node list, called by SceneManager.
         */
        virtual void render(const std::vector<RenderNode*>& renderList);
        /**
         * @brief Render OBB list, called by SceneManager.
         */
        virtual void render(const std::vector<OBB*>& renderList);
        /**
         * @brief Render Particle2D, called by SceneManager.
         */
        virtual void render(Particle2D* particle);
        /**
         * @brief Render Particle3D, called by SceneManager.
         */
        virtual void render(Particle3D* particle);
        
    protected:
        /**
         * Generate or bind VAO for program, each program need a new VAO for DrawInstance.
         * Every program will generate a VBO for DrawInstance.
         */
        void generateProgramVertexArray(const ProgramPtr& program);
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
        
        
    private:
        GLuint mVertexBuffer;   // object own vertex buffer
        GLuint mIndexBuffer;    // object own index buffer
        
        std::map<GLuint, GLuint> mVAOMap;   // each program need different VAO (program will create VBO, bind to VAO)
        static ProgramPtr mBaseProgram;     // base rendering program, no texture and lights
        static ProgramPtr mParticle2DProgram;   // particle program, render 2D point sprite
        static ProgramPtr mParticle3DProgram;   // particle program, render 3D point sprite
    };
}

#endif // PEACH3D_OBJECTGL_H
