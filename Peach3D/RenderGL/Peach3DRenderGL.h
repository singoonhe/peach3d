//
//  RenderGL.h
//  RenderGL
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_RENDERGL_H
#define PEACH3D_RENDERGL_H

#include <algorithm>
#include "Peach3DCommonGL.h"
#include "Peach3DIRender.h"

namespace Peach3D
{
    // define OpenGL suppourt extensions
    enum class GLExtensionType
    {
        eAndroidGL3,    // OpenGL ES 3.0 support just for android
        eVertexArray,   // glGenVertexArray in OpenGL ES 2.0 or OpenGL 2.x
        eMapBuffer,     // glMapBuffer in OpenGL ES 2.0 or OpenGL 2.x
    };

    class RenderGL : public IRender
    {
    public:
        ~RenderGL();
        // init render, set view port
        virtual bool initRender(const Vector2& size);
        /** Open depth test... */
        virtual void prepareForObjectRender();
        /** Close depth test... */
        virtual void prepareForWidgetRender();
        /** Clear old frame before render. */
        virtual void prepareForMainRender();
        /** Update render view size if window changed. */
        virtual void setRenderSize(const Peach3D::Vector2& size);
        //! return is support type gl extersion
        bool isTypeExtersionSupport(GLExtensionType type)
        {
            return std::find(mExtensionList.begin(), mExtensionList.end(), type)!=mExtensionList.end();
        }
        //! delete gl extersion
        //! extersion may be not valid on android if adress get NULL
        void deleteExtersionSupport(GLExtensionType type)
        {
            auto typeIter = std::find(mExtensionList.begin(), mExtensionList.end(), type);
            if (typeIter != mExtensionList.end()) {
                mExtensionList.erase(typeIter);
            }
        }
        
        /** Set render clear color, call GL once. */
        virtual void setRenderClearColor(const Color4& color);
        /** Set render lines width, call GL once. */
        virtual void setRenderLineWidth(float newWidth);
        
        //! Create renderable object.
        /** \params the name of new renderable object.
         \return return the new object pointer, return nullptr if create failed. */
        virtual ObjectPtr createObject(const char* objectName);
        
        /** Create texture with name. */
        virtual TexturePtr createTexture(const char* name);
        
        //! Create render program with name. Program include vertex and pixel shader for render.
        /** \params the name of new OpenGL ES program.
         \return pointer of new program, return nullptr if create failed. */
        virtual ProgramPtr createProgram(uint pId);
        
    protected:
        //! filter which extension gpu supported
        void filterGLExtensions(const char* version, const char* extension);

    private:
        std::vector<GLExtensionType> mExtensionList;
    };
}

#endif // PEACH3D_RENDERES_H
