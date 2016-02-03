//
//  Peach3DPlatformMAC.h
//  Peach3DLib
//
//  Created by singoon.he on 9/2/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PLATFORMMAC_H
#define PEACH3D_PLATFORMMAC_H

#include "Peach3DIPlatform.h"
#include "Peach3DEAGLViewMAC.h"

namespace Peach3D
{
    // defined text draw base info
    typedef struct _tTextDrawInfo
    {
        CGPoint     start;  // draw start pos
        CGSize      size;   // draw text size
        NSColor*    color;  // draw color
        NSString*   str;    // draw text
        NSImage*    image;  // draw image
        
        _tTextDrawInfo(CGPoint _start, CGSize _size, NSImage* _image) : start(_start), size(_size), image(_image) {}
        _tTextDrawInfo(CGPoint _start, CGSize _size, NSColor* _color, NSString* _str) : start(_start), size(_size), color(_color), str(_str) {}
    }tTextDrawInfo;
    
    class PlatformMAC : public IPlatform
    {
    public:
        PlatformMAC();
        ~PlatformMAC();
        
        // init mac platform
        virtual bool initWithParams(const PlatformCreationParams &params);
        // init render (mac only, not IPlatform func)
        bool initRenderGL();
        //! pause animating
        virtual void pauseAnimating();
        //! resume animating
        virtual void resumeAnimating();
        /** Notify system terminate, exit engine in PlatformMAC::exitEngine(). */
        virtual void terminate();
        /** Real exit game engine. */
        void exitEngine();
        
        //! is flag key down
        bool isFlagKeyDown(KeyCode flagKey);
        //! get label text texture
        ITexture* getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
                                 std::map<std::string, std::vector<Rect>>& clicksRect);
        
        /** Open url, "http://" is not necessary. */
        virtual void openUrl(const std::string& url);
        
    private:
        // calc text render info list
        Vector2 calcTextRenderInfoList(const std::vector<LabelStageTextInfo>& textList, NSMutableDictionary* attr,
                                       const LabelTextDefined& defined, std::vector<std::vector<tTextDrawInfo>>* drawInfoList);
        
    private:
        EAGLViewMAC*   mGLView;
        std::map<NSString*, std::string>  mClickedStrMap;
    };
}

#endif // PEACH3D_PLATFORMMAC_H
