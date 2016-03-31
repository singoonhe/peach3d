//
//  Peach3DPlatformIOS.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/15/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include <sys/utsname.h>
#include "Peach3DPlatformIOS.h"
#include "Peach3DRenderGL.h"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    PlatformIOS::PlatformIOS() : mGLViewController(NULL)
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
        NSString *versionString = [[UIDevice currentDevice] systemVersion];
        mOSVerStr = [versionString UTF8String];
        
        // get device model
        struct utsname systemInfo;
        uname(&systemInfo);
        mDeviceModel = systemInfo.machine;
        
        // get writeable path
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        mWriteablePath = [[paths objectAtIndex:0] UTF8String];
        if (mWriteablePath[mWriteablePath.size() - 1] != '/') {
            mWriteablePath = mWriteablePath + "/";
        }
    }
    
    PlatformIOS::~PlatformIOS()
    {
        // delete gl view controller, render will auto delete in IPlatform
        mGLViewController = NULL;
    }
    
    bool PlatformIOS::initWithParams(const PlatformCreationParams &params)
    {
        IPlatform::initWithParams(params);
        
        // get screen size
        CGRect screenRect = [UIScreen mainScreen].bounds;
        mCreationParams.winSize = Vector2(screenRect.size.width, screenRect.size.height);
        
        mGLViewController = [[EAGLViewController alloc] init];
        float screenScale = [[UIScreen mainScreen] scale];
        if (screenScale != 1.0f) {
            // enable retina
            mCreationParams.winSize.x  *= screenScale;
            mCreationParams.winSize.y *= screenScale;
            [mGLViewController.view setContentScaleFactor:screenScale];
        }
        // save render feature level
        mFeatureLevel = [mGLViewController getOpenGLESVersion];
        // save view controller
        mCreationParams.window = (__bridge void*)mGLViewController;
        
        // init render gl es
        mRender = new RenderGL();
        bool success = mRender->initRender(mCreationParams.winSize);
        
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
    
    void PlatformIOS::pauseAnimating()
    {
        if (mAnimating) {
            IPlatform::pauseAnimating();
            // eagl pause
            [mGLViewController pause];
        }
    }
    
    void PlatformIOS::resumeAnimating()
    {
        if (!mAnimating) {
            IPlatform::resumeAnimating();
            // eagl resume
            [mGLViewController resume];
        }
    }
    
    Vector2 PlatformIOS::calcTextRenderInfoList(const std::vector<LabelStageTextInfo>& textList, NSMutableDictionary* attr,
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
                UIImage *labelImage = NULL;
                if (imageDataSize > 0 && imageData) {
                    NSData* sysData = [NSData dataWithBytesNoCopy:imageData length:imageDataSize];
                    if (sysData) {
                        // UIImage will handle data, we needn't free it
                        imageData = nullptr;
                        labelImage = [[UIImage alloc] initWithData:sysData];
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
                    UIColor* infoColor = [UIColor colorWithRed:info.color.r green:info.color.g blue:info.color.b alpha:info.color.a];
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
    
    TexturePtr PlatformIOS::getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
                                          std::map<std::string, std::vector<Rect>>& clicksRect)
    {
        // create font
        UIFont* font = [UIFont fontWithName:[NSString stringWithUTF8String:defined.font.c_str()] size:defined.fontSize];
        if (!font) {
            // default is system font
            font = [UIFont systemFontOfSize:defined.fontSize];
        }
        NSMutableDictionary* attr = [NSMutableDictionary dictionaryWithObject:font forKey:NSFontAttributeName];
        [attr setObject:[UIColor blackColor] forKey:NSForegroundColorAttributeName];
        // create bitmap
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
        
        uint imageDataSize = (int)(renderSize.x * renderSize.y * 4);
        unsigned char* imageData = new unsigned char[imageDataSize];
        memset(imageData, 0, imageDataSize);
        // reate canvas bitmap
        CGColorSpaceRef colorSpace  = CGColorSpaceCreateDeviceRGB();
        CGContextRef context        = CGBitmapContextCreate(imageData,
                                                            renderSize.x,
                                                            renderSize.y,
                                                            8,
                                                            (int)(renderSize.x) * 4,
                                                            colorSpace,
                                                            kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
        
        CGColorSpaceRelease(colorSpace);
        // move Y rendering to the bottom of the image
        CGContextTranslateCTM(context, 0.0f, 0.0f );
        CGContextScaleCTM(context, 1.0f, 1.0f);
        
        // store the current context
        UIGraphicsPushContext(context);
        // just for test texture created rect
        //CGContextSetRGBFillColor(context, 1, 1, 1, 1);
        //CGContextFillRect(context, CGRectMake(0.0f, 0.0f, renderSize.x, renderSize.y));
        for (auto lineList : drawInfoList) {
            for (auto info : lineList) {
                info.start.y += startPosY;
                if (info.start.y >= (-info.size.height) && info.start.y <= renderSize.y) {
                    if (info.image) {
                        [info.image drawAtPoint:CGPointMake(info.start.x, info.start.y)];
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
        // pop the context
        UIGraphicsPopContext();
        // release the context
        CGContextRelease(context);
        
        // create texture
        TexturePtr textTex = ResourceManager::getSingleton().createTexture(imageData, imageDataSize, renderSize.x, renderSize.y, TextureFormat::eRGBA8);
        return textTex;
    }
    
    void PlatformIOS::openUrl(const std::string& url)
    {
        if (url.size()==0) return;
        
        // auto add "http://" header
        std::string finalUrl = url;
        if (finalUrl.find("://")==std::string::npos) {
            finalUrl = "http://" + finalUrl;
        }
        NSURL *nsurl = [NSURL URLWithString:[NSString stringWithUTF8String:finalUrl.c_str()]];
        [[UIApplication sharedApplication] openURL:nsurl];
    }
}