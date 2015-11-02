//
//  Peach3DPlatformIOS.h
//  Peach3DLib
//
//  Created by singoon.he on 9/15/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PLATFORMIOS_H
#define PEACH3D_PLATFORMIOS_H

#include "Peach3DIPlatform.h"
#include "Peach3DEAGLViewController.h"

namespace Peach3D
{
    // defined text draw base info
    typedef struct _tTextDrawInfo
    {
        CGPoint     start;  // draw start pos
        CGSize      size;   // draw text size
        UIColor*    color;  // draw color
        NSString*   str;    // draw text
        UIImage*    image;  // draw image
        
        _tTextDrawInfo(CGPoint _start, CGSize _size, UIImage* _image) : start(_start), size(_size), image(_image) {}
        _tTextDrawInfo(CGPoint _start, CGSize _size, UIColor* _color, NSString* _str) : start(_start), size(_size), color(_color), str(_str) {}
    }tTextDrawInfo;
    
    class PlatformIOS : public IPlatform
    {
    public:
        PlatformIOS();
        ~PlatformIOS();
        
        // init ios platform
        virtual bool initWithParams(const PlatformCreationParams &params);
        //! pause animating
        virtual void pauseAnimating();
        //! resume animating
        virtual void resumeAnimating();
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
        EAGLViewController* mGLViewController;
        std::map<NSString*, std::string>  mClickedStrMap;
    };
}

#endif // PEACH3D_PLATFORMIOS_H
