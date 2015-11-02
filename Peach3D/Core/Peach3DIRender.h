//
//  IRender.h
//  TestPeach3D
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_IRENDER_H
#define PEACH3D_IRENDER_H

#include "Peach3DCompile.h"
#include "Peach3DSingleton.h"
#include "Peach3DIProgram.h"
#include "Peach3DITexture.h"
#include "Peach3DIObject.h"
#include "Peach3DColor4.h"

namespace Peach3D
{
    class PEACH3D_DLL IRender : public Singleton < IRender >
    {
    public:
        IRender() : mIsRenderValid(false) {}
        virtual ~IRender() {}
        // init render, set view port
        virtual bool initRender(uint width, uint height);
        //! return render state
        bool isRenderValid() { return mIsRenderValid; }
        // clear old frame before render
        virtual void prepareForRender() = 0;
        // open depth test...
        virtual void prepareForObjectRender() = 0;
        // close depth test...
        virtual void prepareForWidgetRender() = 0;
        /**
         * Present view after render for DX11.
         * OpenGL have do nothing after render, buffer will swap in PlatformXXX.
         */
        virtual void finishForRender() {}

        virtual void setRenderClearColor(const Color4& color) { mRenderClearColor = color;}
        Color4 getRenderClearColor()const { return mRenderClearColor; }

        //! Create renderable object.
        /** \params the name of new renderable object.
            \return return the new object pointer, return nullptr if create failed. */
        virtual IObject* createObject(const char* objectName) = 0;
        //! delete object
        virtual void deleteObject(IObject* object) { delete object; }
        
        //! create texture with data
        virtual ITexture* createTexture(const char* name) = 0;
        //! delete texture
        virtual void deleteTexture(ITexture* tex) { delete tex; }
        
        //! Create render program with name. Program include vertex and pixel shader for render.
        /** \params the name of new program.
         \return pointer of new program, return nullptr if create failed. */
        virtual IProgram* createProgram(uint pId) = 0;
        //! delete program
        virtual void deleteProgram(IProgram* program) { delete program; }

    protected:
        //! generate object vertex shader source code
        virtual void getObjectPresetVSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms) = 0;
        //! generate object pixel shader source code
        virtual void getObjectPresetPSSource(uint* params, std::string* code, std::vector<ProgramUniform>* uniforms) = 0;
        
    protected:
        bool      mIsRenderValid;               // render is init?
        uint      mRenderWidth;                 // render window width
        uint      mRenderHeight;                // render window height
        Color4    mRenderClearColor;            // default render clear color is Color4Gray
        
        friend class        ResourceManager;
        friend class        SceneManager;
    };
}

#endif // PEACH3D_IRENDER_H
