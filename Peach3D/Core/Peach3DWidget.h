//
//  Peach3DWidget.h
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_WIDGET_H
#define PEACH3D_WIDGET_H

#include "Peach3DCompile.h"
#include "Peach3DRect.h"
#include "Peach3DNode.h"
#include "Peach3DColor4.h"
#include "Peach3DVector3.h"
#include "Peach3DITexture.h"

namespace Peach3D
{
    // bigger than this zorder widget will not be clear when SceneManager::reset.
#define NOT_CLEAR_WIDGET_ZORDER (std::numeric_limits<int>::max() - 100)
    
    // inherit enabled widget attribute type
    namespace InheritAttriType
    {
        const uint Alpha = 0x0001;      // pass alpha to children
        const uint AutoScale = 0x0002;  // pass auto scale to children
    }
    
    enum class PEACH3D_DLL AutoScaleType
    {
        eFix,   // no scale always
        eWidth,	// width scale
        eHeight,// height scale
        eMin,   // min(width, height) scale
        eMax,   // max(width, height) scale
    };
    
    class PEACH3D_DLL Widget : public Node
    {
    public:
        /** Create widget, this used to be UI Node or color Node. */
        static Widget* create(const Vector2& pos=Vector2Zero, const Vector2& size=Vector2Zero);
        
    public:
        /** Set children need sort when update, render not need sort again. */
        virtual void addChild(Node* childNode) {mChildNeedSort = true; Node::addChild(childNode);}
        
        /** Set position, this will auto set children need update. */
        void setPosition(const Vector2& pos);
        /** 
         * Get position, default pos relative to parent. 
         * World position is under anchor position.
         */
        const Vector2& getPosition(TranslateRelative type = TranslateRelative::eLocal);
        
        /** Set rotation by radians, also set children need update. */
        void setRotation(float rotate);
        /** Get rotation, default pos relative to parent. */
        float getRotation(TranslateRelative type = TranslateRelative::eLocal);
        
        /** Set scaling, also set children need update. */
        virtual void setScale(const Vector2& scale);
        /** Set x and y scaling at the same time. */
        void setScale(float scale) {setScale(Vector2(scale, scale));}
        /** Get scaling, default pos relative to parent. */
        const Vector2& getScale(TranslateRelative type = TranslateRelative::eLocal);
        
        /** Set content size, the last size need multiply auto scale. */
        virtual void setContentSize(const Vector2& size);
        /** Get content size, design size or world size. */
        const Vector2& getContentSize(TranslateRelative type = TranslateRelative::eLocal);
        
        /** Set anchor point value must be (0-1). */
        void setAnchorPoint(const Vector2& anchor);
        /** Get anchor point. */
        const Vector2& getAnchorPoint()const {return mAnchor;}
        
        /** Get is need rendering */
        virtual bool isNeedRender() {return Node::isNeedRender() && mRenderProgram && (mWorldAlpha > FLT_EPSILON);}
        /** Need update children alpha if inherit alpha type. */
        virtual void setAlpha(float alpha);
        /** Get render alpha, it maybe effect by parent. */
        float getRenderAlpha() { return mWorldAlpha; }
        /** Get render hash. */
        uint getRenderStateHash()const {return mRenderStateHash;}
        
        void useProgramForRender(const ProgramPtr& program);
        const ProgramPtr& getProgramForRender() {return mRenderProgram;}
        virtual void setColor(const Color3& color) { mDiffColor = color; }
        const Color3& getColor()const { return mDiffColor; }
        virtual void setMinSize(const Vector2& size) { mMinSize = size; }
        const Vector2& getMinSize()const { return mMinSize; }
        virtual void setMaxSize(const Vector2& size) { mMaxSize = size; }
        const Vector2& getMaxSize()const { return mMaxSize; }
        virtual void setBindCorner(const Vector2& corner) { mBindCorner = corner; }
        const Vector2& getBindCorner()const { return mBindCorner; }
        virtual void setPosScaleType(AutoScaleType typeX, AutoScaleType typeY) { mScaleTypeX = typeX; mScaleTypeY = typeY; }
        AutoScaleType getPosScaleTypeX() { return mScaleTypeX; }
        AutoScaleType getPosScaleTypeY() { return mScaleTypeY; }
        virtual void setSizeScaleType(AutoScaleType typeW, AutoScaleType typeH) { mScaleTypeWidth = typeW; mScaleTypeHeight = typeH; }
        AutoScaleType getPosScaleTypeWidth() { return mScaleTypeWidth; }
        AutoScaleType getPosScaleTypeHeight() { return mScaleTypeHeight; }
        
        void setClipEnabled(bool enable) {mClipEnabled = enable;}
        bool isClipEnabled() {return mClipEnabled;}
        void setInheritAttri(uint type) { mInheritAttri = type; mIsRenderDirty = true; }
        uint getInheritAttri() { return mInheritAttri; }
        void setLocalZOrder(int order);
        int getLocalZOrder() {return mLocalZOrder;}
        
        /** Is point in widget rect. */
        virtual bool isPointInZone(const Vector2& point);
        /** Convert global position to widget.
         * @return Vector2(-1, -1) will return if not inside.
         */
        Vector2 convertScreenToNode(const Vector2& point);
        /** Set widget global ZOrder, called before render. User should call setLocalZOrder. */
        void setGlobalZOrder(uint order) { mGlobalZOrder = order; }
        /** Get widget global ZOrder, called before Event trigger. User should call setLocalZOrder. */
        uint getGlobalZOrder() { return mGlobalZOrder; }
        
    public:
        /** Use Widget::create to create widget. */
        Widget();
        /** Use deleteFromParent or deleteChild to destroy widget . */
        virtual ~Widget() {}
        
        /** Update rendering attributes, about world rect/rotate/scale... */
        virtual void updateRenderingAttributes(float lastFrameTime);
        /** Update rendering state, preset program and calculate hash code. */
        virtual void updateRenderingState(const std::string& extState="");
        
        /** Convert scale type to value. */
        float getAutoScaleTypeValue(AutoScaleType type);
        
    protected:
        Vector2     mScale;     // scaling
        float       mRotate;    // rotation value, z axis
        Vector2     mAnchor;    // anchor point, it will effect position/rotation/scaling
        Color3      mDiffColor; // widget diffuse color, Node have alpha attribute
        bool        mClipEnabled;       // is widget will auto be cliped by parent
        uint        mInheritAttri;      // pass some attribute to children
        
        float       mWorldAlpha;        // cache world alpha
        Vector2     mWorldScale;        // cache world scaling
        float       mWorldRotate;       // cache world rotate
        Vector2     mWorldPos;          // cache world rendering rect, pos with anchor
        Vector2     mWorldSize;         // cache world rendering rect, pos with anchor
        
        Vector2     mDesignPos;         // user designed position
        Vector2     mBindCorner;        // bind parent corner for position, default is left-bottom
        AutoScaleType mScaleTypeX;      // widget position auto scale type
        AutoScaleType mScaleTypeY;      // widget position auto scale type
        
        Vector2     mDesignSize;        // user designed size
        Vector2     mMinSize;           // min size for widget auto scaling
        Vector2     mMaxSize;           // max size for widget auto scaling
        AutoScaleType mScaleTypeWidth;  // widget size auto scale type
        AutoScaleType mScaleTypeHeight; // widget size auto scale type
        
        ProgramPtr  mRenderProgram;     // render program
        uint        mRenderStateHash;   // render state hash, used for instancing render
        bool        mIsRenderHashDirty; // is render state hash need update
        
        bool        mChildNeedSort;     // children need sort when addChild()
        int         mLocalZOrder;       // widget local zorder
        uint        mGlobalZOrder;      // widget global zorder, used for EventDispather
    };
}

#endif // PEACH3D_WIDGET_H
