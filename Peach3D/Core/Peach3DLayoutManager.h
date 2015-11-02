//
//  Peach3DLayoutManager.h
//  Peach3DLib
//
//  Created by singoon.he on 5/17/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_LAYOUTMANAGER_H
#define PEACH3D_LAYOUTMANAGER_H

#include "Peach3DCompile.h"
#include "Peach3DSingleton.h"
#include "Peach3DWidget.h"
#include "tinyxml2/tinyxml2.h"

using namespace::tinyxml2;
namespace Peach3D
{
    // define enum type to case attribute name
    enum class LayoutAttrType {
        eWidget,
        eLabel,
        eButton,
        eLayout,
        eNormalTexture,     // <NormalTexture>xxx.png</NormalTexture>       button normal texture file name
        eDownTexture,       // <DownTexture>xxx.png</DownTexture>           ...
        eHighlightTexture,  // <HighlightTexture>xxx.png</HighlightTexture> ...
        eDisableTexture,    // <DisableTexture>xxx.png</DisableTexture>     ...
        eNormalCoord,       // <NormalCoord>0, 0, 1, 1</NormalCoord>        button nomal texture rect, default is "0, 0, 1, 1"
        eDownCoord,         // <DownCoord>0, 0, 1, 1</DownCoord>            ...
        eHighlightCoord,    // <HighlightCoord>0, 0, 1, 1</HighlightCoord>  ...
        eDisableCoord,      // <DisableCoord>0, 0, 1, 1</DisableCoord>      ...
        eAnchor,            // <Anchor>0, 0</Anchor>
        eBindPosition,      // <BindPosition>Center</BindPosition>          widget bind pos type, 9 fixed position, default is (0, 0).
        eAutoPosition,      // <AutoPosition>Fix, Min</AutoPosition>        widget position will convert with this scale, default is fixed.
        ePosition,          // <Position>10, 30</Position>                  widget position, relative to BindPosition
        eAutoScale,         // <AutoScale>Min, Min</AutoScale>              widget auto scale type, default is fixed.
        eScale,             // <Scale>1.0, 1.0</Scale>
        eText,              // <Text>HonoraryPresent</Text>                 button or label text, use "setTextLocalizedFunc" can localized it.
        eFontSize,          // <FontSize>30</FontSize>                      button or label font size, default is 20.0f.
        eAutoFontSize,      // <AutoFontSize>Min</AutoFontSize>             button or label auto font size scale.
        eHAlignment,        // <HAlignment>Center</HAlignment>              label HAlignment, default is left.
        eVAlignment,        // <VAlignment>Center</VAlignment>              label VAlignment, default is bottom.
        eTitleOffset,       // <TitleOffset>0.5, 0.5</TitleOffset>          button title offset, default is (0.5, 0.5).
        eSwallowEvents,     // <SwallowEvents>true</SwallowEvents>          set widget swallow events when text is true.
        eFillColor,         // <FillColor>1, 1, 1, 1</FillColor>            label fill color
        eAlpha,             // <Alpha>1.0</Alpha>
        eColor,             // <Color>1, 1, 1, 1</Color>                    widget color
        eTexture,           // <Texture>xxx.png</Texture>                   widget texture 0
        eCoord,             // <Coord>0, 0, 1, 1</Coord>                    widget sprite 0
        eVisible,           // <Visible>true</Visible>
        eCount,
    };

    // redefine layout load binding and over call func, it's too long
    typedef std::function<void(const std::string&, Widget*)>    LayoutVarBindFunction;
    typedef std::function<void(const std::string&)>             LayoutLoadOverFunction;
    typedef std::function<std::string(const std::string&)>      LayoutTextLocalizedFunction;
    
#define LAYOUT_BIND_NAME_VARIATE(memname, memvar, loadname, loadnode) \
    if (loadname.compare(memname) == 0) { \
        memvar = (decltype(memvar))(loadnode); \
    }
#define LAYOUT_INNER_POSITION_COUNT 9   // LeftBottom, LeftCenter, LeftTop, CenterBottom, Center, CenterTop, RightBottom, RightCenter, RightTop
#define LAYOUT_SCALE_TYPE_COUNT     4   // Fixed, Width, Height, Min
    
    class PEACH3D_DLL LayoutManager : public Singleton < LayoutManager >
    {
    public:
        /** 
         * Load layout from xml file. 
         * @params parent Which widget layout will add to, nullptr for root widget.
         * @params bindFunc Use LAYOUT_BIND_NAME_VARIATE to bind var, it will called much times.
         * @params overFunc Called when layout widgets create over.
         */
        bool loadLayout(const std::string& file, Widget* parent = nullptr, const LayoutVarBindFunction& bindFunc = nullptr,
                        const LayoutLoadOverFunction& overFunc = nullptr);
        /** Load layout from xml file data, see above loadLayout for more. */
        bool loadLayout(const uchar* content, ulong len, Widget* parent = nullptr, const LayoutVarBindFunction& bindFunc = nullptr,
                        const LayoutLoadOverFunction& overFunc = nullptr);
        /** Set design size, it will calc scale, auto landscape. */
        void setDesignSize(float width, float height);
        
        /** Set text localized func, return localized text from key. */
        void setTextLocalizedFunc(const LayoutTextLocalizedFunction& func) { mLocalizedFunc = func; }
        
        const Vector2& getDesignSize() { return mDesignScreenSize; }
        const Vector2& getScreenSize() { return mScreenSize; }
        bool isLandscape() { return mIsLandscape; }
        float getWidthScale() { return mWidthScale; }
        float getHeightScale() { return mHeightScale; }
        float getMinScale() { return mMinScale; }
        
    protected:
        LayoutManager();
        ~LayoutManager() {}
        /** Set screen size. */
        void setScreenSize(uint width, uint height);
        /** Load widget data and children. */
        void loadWidgetData(Widget* newNode, const XMLElement* nodeEle, Widget* parentNode, const LayoutVarBindFunction& bindFunc);
        /** Create new widget for type name. */
        Widget* createTypeNameWidget(const char* typeName, const char* nodeName, Widget* parentNode, const LayoutVarBindFunction& bindFunc);
        
    private:
        bool        mIsLandscape;       // is show landscape
        Vector2     mScreenSize;        // screen orign size
        Vector2     mDesignScreenSize;  // design screen size, default is screen size
        float       mWidthScale;        // screen-width / design-width
        float       mHeightScale;       // screen-height / design-height
        float       mMinScale;          // min(mWidthScale, mHeightScale)
        Vector2     mInnerPoss[LAYOUT_INNER_POSITION_COUNT];    // inner positions
        LayoutTextLocalizedFunction mLocalizedFunc; // localize layout text
        std::map<std::string, LayoutAttrType>   mLoadAttrMap;   // layout load attribure map
        
        friend class IPlatform; // create and delete by IPlatform
        friend class IRender;   // set screen size by IRender
    };
}

#endif /* defined(PEACH3D_LAYOUTMANAGER_H) */
