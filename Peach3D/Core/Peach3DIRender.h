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
        IRender() : mIsRenderValid(false), mLineWidth(2.0f) {}
        virtual ~IRender() {}
        // init render, set view port
        virtual bool initRender(const Vector2& size);
        /** Return render state. */
        bool isRenderValid() { return mIsRenderValid; }
        /** Return render video card name. */
        const std::string& getVideoCard() { return mVideoCard; }
        
        /** Open depth test... */
        virtual void prepareForObjectRender() = 0;
        /** Close depth test... */
        virtual void prepareForWidgetRender() = 0;
        /** Clear old frame before render. */
        virtual void prepareForMainRender() = 0;
        /**
         * Present view after render for DX11.
         * OpenGL have do nothing after render, buffer will swap in PlatformXXX.
         */
        virtual void finishForMainRender() {}

        virtual void setRenderClearColor(const Color4& newColor) { mRenderClearColor = newColor;}
        Color4 getRenderClearColor()const { return mRenderClearColor; }
        void setRenderOBBColor(const Color4& newColor) { mOBBColor = newColor; }
        const Color4& getRenderOBBColor() {return mOBBColor;}
        virtual void setRenderLineWidth(float newWidth) { mLineWidth = newWidth; }
        float getRenderLineWidth() {return mLineWidth;}

        //! Create renderable object.
        /** \params the name of new renderable object.
            \return return the new object pointer, return nullptr if create failed. */
        virtual IObject* createObject(const char* objectName) = 0;
        //! delete object
        virtual void deleteObject(IObject* object) { delete object; }
        
        /** Create normal texture with name. */
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
        bool        mIsRenderValid;     // render is init?
        Color4      mRenderClearColor;  // default render clear color is Color4Gray
        Color4      mOBBColor;          // OBB lines render color
        float       mLineWidth;         // draw line width, include OBB
        std::string mVideoCard;         // current video card name, need assigned in RenderXX
        
        friend class        ResourceManager;
        friend class        SceneManager;
    };
}

#endif // PEACH3D_IRENDER_H
