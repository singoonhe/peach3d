//
//  Peach3DPlatformMAC.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/2/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include <Carbon/Carbon.h>
#include <sys/sysctl.h>
#include "Peach3DPlatformMAC.h"
#include "Peach3DRenderGL.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    PlatformMAC::PlatformMAC():mGLView(NULL)
    {
        // get local system language
        NSUserDefaults* defs = [NSUserDefaults standardUserDefaults];
        NSArray* languages = [defs objectForKey:@"AppleLanguages"];
        NSString* preferredLang = [languages objectAtIndex:0];
        if ([preferredLang hasPrefix:@"en"]) {
            mLocalLanguage = LanguageType::eEnglish;
        }
        else if ([preferredLang hasPrefix:@"zh-Hans"]) {
            mLocalLanguage = LanguageType::eChineseHans;
        }
        else if ([preferredLang hasPrefix:@"zh-Hant"]) {
            mLocalLanguage = LanguageType::eChineseHant;
        }
        else if ([preferredLang hasPrefix:@"fr"]) {
            mLocalLanguage = LanguageType::eFrench;
        }
        else if ([preferredLang hasPrefix:@"ru"]) {
            mLocalLanguage = LanguageType::eRussian;
        }
        
        // get OS version string
        NSString *versionString = [[NSProcessInfo processInfo] operatingSystemVersionString];
        mOSVerStr = [versionString UTF8String];
        
        // get device model
        size_t len = 0;
        sysctlbyname("hw.model", NULL, &len, NULL, 0);
        if (len) {
            char *model = (char*)malloc(len * sizeof(char));
            sysctlbyname("hw.model", model, &len, NULL, 0);
            mDeviceModel = model;
            free(model);
        }

        // get "application support" dir and add application bundleId
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES);
        NSMutableString* supportDir = [NSMutableString stringWithString:[paths objectAtIndex:0]];
        if (![supportDir hasSuffix:@"/"]) {
            [supportDir appendString:@"/"];
        }
        [supportDir appendString:[[NSBundle mainBundle] bundleIdentifier]];
        [supportDir appendString:@"/"];
        mWriteablePath = [supportDir UTF8String];
        // mkdir
        NSError *error = nil;
        [[NSFileManager defaultManager] createDirectoryAtPath:supportDir withIntermediateDirectories:YES attributes:nil error:&error];
    }
    
    PlatformMAC::~PlatformMAC()
    {
        // delete gl view, render will auto delete in IPlatform
        mGLView = NULL;
    }
    
    bool PlatformMAC::initWithParams(const PlatformCreationParams &params)
    {
        IPlatform::initWithParams(params);
        
        // create opengl view attribs
        GLuint attribs[20];
        int attrIndex = 0;
        attribs[attrIndex++] = NSOpenGLPFADoubleBuffer;
        attribs[attrIndex++] = NSOpenGLPFAClosestPolicy;
        attribs[attrIndex++] = NSOpenGLPFAColorSize;
        attribs[attrIndex++] = 24;
        attribs[attrIndex++] = NSOpenGLPFAAlphaSize;
        attribs[attrIndex++] = 8;
        attribs[attrIndex++] = NSOpenGLPFADepthSize;
        attribs[attrIndex++] = params.zBits;
        if (params.sBits > 0) {
            attribs[attrIndex++] = NSOpenGLPFAStencilSize;
            attribs[attrIndex++] = params.sBits;
        }
        if (params.MSAA > 0) {
            attribs[attrIndex++] = NSOpenGLPFASampleBuffers;
            attribs[attrIndex++] = params.MSAA;
            attribs[attrIndex++] = NSOpenGLPFASamples;
            attribs[attrIndex++] = 4;
        }
        attribs[attrIndex++] = NSOpenGLPFAOpenGLProfile;
        attribs[attrIndex++] = NSOpenGLProfileVersion3_2Core;
        attribs[attrIndex++] = 0;
        
        // new opengl view
        NSOpenGLPixelFormat* format = [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute*)attribs];
        NSRect winRect = NSMakeRect(0.f, 0.f, params.winSize.x, params.winSize.y);
        mGLView = [[EAGLViewMAC alloc] initWithFrame:winRect pixelFormat:format];
        
        // enable high resolution
        [mGLView setWantsBestResolutionOpenGLSurface:YES];
        
        // auto set sync
        GLint swapInt = 1;
        [[mGLView openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
        
        // set window
        IPlatform::setWindow((__bridge void*)mGLView);
        return true;
    }
    
    bool PlatformMAC::initRenderGL()
    {
        // init render gl
        mRender = new RenderGL();
        bool success = mRender->initRender(mCreationParams.winSize);
        // system version below 10.9 always use GL2, it's 3.x gl func not all ready
        std::string secVerStr = mOSVerStr.substr(mOSVerStr.find('.') + 1);
        if (atof(secVerStr.c_str()) < 9) {
            mFeatureLevel = RenderFeatureLevel::eGL2;
            Peach3DLog(LogLevel::eInfo, "Render feature level GL2 be used");
        }
        else {
            mFeatureLevel = RenderFeatureLevel::eGL3;
            Peach3DLog(LogLevel::eInfo, "Render feature level GL3 be used");
        }
        
        bool userSuccess = true;
        if (success) {
            // add default search file dir
            NSString * path = [[NSBundle mainBundle] resourcePath];
            ResourceManager::getSingleton().addSearchDirectory([path UTF8String]);
            
            // notify IAppDelegate launch finished
            userSuccess = mCreationParams.delegate->appDidFinishLaunching();
            if (userSuccess) {
                // start animating
                mAnimating = true;
            }
        }
        return success && userSuccess;
    }
    
    void PlatformMAC::pauseAnimating()
    {
        if (mAnimating) {
            IPlatform::pauseAnimating();
            // eagl pause
            [mGLView pause];
        }
    }
    
    void PlatformMAC::resumeAnimating()
    {
        if (!mAnimating) {
            IPlatform::resumeAnimating();
            // eagl resume
            [mGLView resume];
        }
    }
    
    void PlatformMAC::terminate()
    {
        IPlatform::terminate();
        // notify system terminate
        [NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0.0];
    }
    
    bool PlatformMAC::isFlagKeyDown(KeyCode flagKey)
    {
        // KeyCode is equal to MAC key
        ushort inKeyCode = (ushort)flagKey;
        unsigned char keyMap[16];
        GetKeys((BigEndianUInt32*) &keyMap);
        return (0 != ((keyMap[ inKeyCode >> 3] >> (inKeyCode & 7)) & 1));
    }
    
    Vector2 PlatformMAC::calcTextRenderInfoList(const std::vector<LabelStageTextInfo>& textList, NSMutableDictionary* attr,
                                                const LabelTextDefined& defined, std::vector<std::vector<tTextDrawInfo>>* drawInfoList)
    {
#define CHANGE_TEXT_NEW_LINE(height) \
        curHeight += height; \
        if (curWidth > textMaxWidth) { \
            textMaxWidth = curWidth; \
        } \
        curWidth = 0.0f; \
        lineMaxHeight = 0.0f; \
        if (lineInfoList.size() > 0) { \
            drawInfoList->push_back(lineInfoList); \
        } \
        lineInfoList.clear();
        
        mClickedStrMap.clear();
        // calc space render size
        CGSize spaceSize = [@" " sizeWithAttributes:attr];
        Vector2 rSize = defined.dim;
        if (FLOAT_EQUAL_0(defined.dim.x)) {
            rSize.x = (float)mCreationParams.winSize.x;
        }
        
        float curWidth=0.0f, curHeight=0.0f;
        float lineMaxHeight=0.0f, textMaxWidth=0.0f;
        std::vector<tTextDrawInfo> lineInfoList;
        for (auto info : textList) {
            float curDrawHeight = 0.0f;
            if (info.image.size() > 0) {
                ulong imageDataSize = 0;
                uchar* imageData = ResourceManager::getSingleton().getFileData(info.image.c_str(), &imageDataSize);
                NSImage *labelImage = NULL;
                if (imageDataSize > 0 && imageData) {
                    NSData* sysData = [NSData dataWithBytesNoCopy:imageData length:imageDataSize];
                    if (sysData) {
                        // NSImage will handle data, we needn't free it
                        imageData = nullptr;
                        labelImage = [[NSImage alloc] initWithData:sysData];
                    }
                }
                if (labelImage) {
                    float imageWidth = [labelImage size].width, imageHeight = [labelImage size].height;
                    if (curWidth > 0.0f && (rSize.x-curWidth) < imageWidth) {
                        // direct change to newline
                        CHANGE_TEXT_NEW_LINE(lineMaxHeight)
                    }
                    // add image to list
                    float drawPosY = curHeight;
                    if (imageHeight < lineMaxHeight) {
                        if (defined.imageVAlign == TextVAlignment::eCenter){
                            drawPosY += (lineMaxHeight - imageHeight)/2.0f;
                        }
                        else if (defined.imageVAlign == TextVAlignment::eBottom) {
                            drawPosY += lineMaxHeight - imageHeight;
                        }
                    }
                    lineInfoList.push_back(tTextDrawInfo(CGPointMake(curWidth, drawPosY), CGSizeMake(imageWidth, imageHeight), labelImage));
                    curWidth += imageWidth;
                    // save current draw height
                    curDrawHeight = imageHeight;
                }
            }
            else if (info.text.compare("\n")==0) {
                // direct change to newline
                CHANGE_TEXT_NEW_LINE(spaceSize.height);
            }
            else {
                NSString* textStr = [NSString stringWithUTF8String:info.text.c_str()];
                ulong lastLength = [textStr length];
                // calc if need change line
                while (lastLength && [textStr length]) {
                    ulong drawLength = lastLength;
                    NSString* drawStr = [textStr copy];
                    CGSize drawSize = [drawStr sizeWithAttributes:attr];
                    // calc last width could render string length
                    float lastDrawWidth = rSize.x-curWidth;
                    while (drawLength && drawSize.width >= lastDrawWidth) {
                        drawLength = drawLength - 1;
                        drawStr = [textStr substringToIndex:(drawLength)];
                        drawSize = [drawStr sizeWithAttributes:attr];
                    }
                    NSColor* infoColor = [NSColor colorWithCalibratedRed:info.color.r green:info.color.g blue:info.color.b alpha:info.color.a];
                    // calc text draw need pos y
                    float drawPosY = curHeight;
                    if (drawSize.height < lineMaxHeight) {
                        if (defined.imageVAlign == TextVAlignment::eCenter){
                            drawPosY += (lineMaxHeight - drawSize.height)/2.0f;
                        }
                        else if (defined.imageVAlign == TextVAlignment::eBottom) {
                            drawPosY += lineMaxHeight - drawSize.height;
                        }
                    }
                    lineInfoList.push_back(tTextDrawInfo(CGPointMake(curWidth, drawPosY), drawSize, infoColor, drawStr));
                    curWidth += drawSize.width;
                    // store click enabled string
                    if (info.clickEnabled) {
                        mClickedStrMap[drawStr] = info.text;
                    }
                    if (FLOAT_EQUAL_0(lineMaxHeight)){
                        lineMaxHeight = drawSize.height;
                    }
                    if (drawLength < lastLength) {
                        // direct change to newline
                        CHANGE_TEXT_NEW_LINE(lineMaxHeight);
                        textStr = [textStr substringFromIndex:drawLength];
                    }
                    lastLength -= drawLength;
                    // save current draw height
                    curDrawHeight = drawSize.height;
                }
            }
            if (curDrawHeight > lineMaxHeight) {
                // modify all previous tTextDrawInfo
                std::for_each(lineInfoList.begin(), lineInfoList.end(), [&](tTextDrawInfo& info){
                    if (curDrawHeight > info.size.height){
                        if (defined.imageVAlign == TextVAlignment::eCenter){
                            info.start.y += (curDrawHeight - lineMaxHeight)/2.0f;
                        }
                        else if (defined.imageVAlign == TextVAlignment::eBottom) {
                            info.start.y += curDrawHeight - lineMaxHeight;
                        }
                    }
                });
                lineMaxHeight = curDrawHeight;
            }
        }
        // add last render content
        if (lineInfoList.size() > 0) {
            CHANGE_TEXT_NEW_LINE(lineMaxHeight)
        }
        else {
            // when last word is '\n', lineInfoList will be empty
            CHANGE_TEXT_NEW_LINE(spaceSize.height);
        }
        
        // save text need render size
        if (FLOAT_EQUAL_0(defined.dim.x)) {
            rSize.x = textMaxWidth;
        }
        rSize.y = curHeight;
        return rSize;
    }
    
    ITexture* PlatformMAC::getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
                                          std::map<std::string, std::vector<Rect>>& clicksRect)
    {
        // create font
        NSFont* font = [[NSFontManager sharedFontManager] fontWithFamily:[NSString stringWithUTF8String:defined.font.c_str()] traits:NSUnboldFontMask | NSUnitalicFontMask weight:0 size:defined.fontSize];
        if (!font) {
            // default is Arial font, system font is obliqued
            font = [[NSFontManager sharedFontManager] fontWithFamily:@"Arial" traits:NSUnboldFontMask | NSUnitalicFontMask weight:0 size:defined.fontSize];
        }
        NSMutableDictionary* attr = [NSMutableDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
        [attr setObject:[NSColor blackColor] forKey:NSForegroundColorAttributeName];
        // calc text need size
        std::vector<std::vector<tTextDrawInfo>> drawInfoList;
        Vector2 renderSize = calcTextRenderInfoList(textList, attr, defined, &drawInfoList);
        if (renderSize.x < FLT_EPSILON || renderSize.y < FLT_EPSILON) {
            return nullptr;
        }
        
        // apply horizontal alignment
        if (defined.hAlign != TextHAlignment::eLeft) {
            std::for_each(drawInfoList.begin(), drawInfoList.end(), [&](std::vector<tTextDrawInfo>& lineList){
                tTextDrawInfo& lastInfo = lineList[lineList.size() - 1];
                // default for horizontal right
                float offsetX = renderSize.x - (lastInfo.start.x + lastInfo.size.width);
                if (defined.hAlign == TextHAlignment::eCenter) {
                    // set horizontal center
                    offsetX = offsetX / 2.0f;
                }
                std::for_each(lineList.begin(), lineList.end(), [&](tTextDrawInfo& info) {
                    info.start.x += offsetX;
                });
            });
        }
        float startPosY = 0.0f;
        // apply vertical alignment
        if (defined.dim.x > 0 && defined.dim.y > 0) {
            if (defined.vAlign == TextVAlignment::eBottom) {
                startPosY = defined.dim.y - renderSize.y;
            }
            else if (defined.vAlign == TextVAlignment::eCenter) {
                startPosY = (defined.dim.y - renderSize.y)/2.0f;
            }
            renderSize.y = defined.dim.y;
        }
        
        uint imageWidth = (uint)renderSize.x, imageHeight = (uint)renderSize.y;
        NSImage *image = [[NSImage alloc] initWithSize:NSMakeSize(imageWidth, imageHeight)];
        [image lockFocus];
        // just for test texture created rect
        //[[NSColor whiteColor] drawSwatchInRect:NSMakeRect (0.0f, 0.0f, renderSize.x, renderSize.y)];
        
        // enable text antialias
        [[NSGraphicsContext currentContext] setShouldAntialias:YES];
        // patch for mac retina display and lableTTF
        [[NSAffineTransform transform] set];
        for (auto lineList : drawInfoList) {
            for (auto info : lineList) {
                info.start.y = renderSize.y-info.start.y-info.size.height - startPosY;
                if (info.start.y >= (-info.size.height) && info.start.y <= renderSize.y) {
                    if (info.image) {
                        [info.image drawInRect:NSMakeRect(info.start.x, info.start.y, [info.image size].width, [info.image size].height)];
                    }
                    else if ([info.str length] > 0) {
                        [attr setObject:info.color forKey:NSForegroundColorAttributeName];
                        [info.str drawAtPoint:info.start withAttributes:attr];
                        
                        // add click enabled rect
                        auto clickIter = mClickedStrMap.find(info.str);
                        if (clickIter != mClickedStrMap.end()) {
                            clicksRect[clickIter->second].push_back(Rect(info.start.x, info.start.y, info.size.width, info.size.height));
                        }
                    }
                }
            }
        }
        // get bitmap data
        NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc] initWithFocusedViewRect:NSMakeRect (0.0f, 0.0f, imageWidth, imageHeight)];
        [image unlockFocus];
        void* data = (void*)[bitmap bitmapData];
        uint textureSize = imageWidth*imageHeight*4;
        
        // create texture
        ITexture* textTex = ResourceManager::getSingleton().createTexture(data, textureSize, imageWidth, imageHeight, TextureFormat::eRGBA8);
        return textTex;
    }
    
    void PlatformMAC::openUrl(const std::string& url)
    {
        if (url.size()==0) return;
        
        // auto add "http://" header, maybe "itms-apps:"
        std::string finalUrl = url;
        if (finalUrl.find("://")==std::string::npos) {
            finalUrl = "http://" + finalUrl;
        }
        NSURL* nsurl = [NSURL URLWithString: [NSString stringWithUTF8String:finalUrl.c_str()]];
        [[NSWorkspace sharedWorkspace] openURL:nsurl];
    }
}