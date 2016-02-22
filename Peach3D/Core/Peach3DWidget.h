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
        
        /** Set content size, this will not effect children. */
        virtual void setContentSize(const Vector2& size);
        /** Get content size. */
        const Vector2& getContentSize(TranslateRelative type = TranslateRelative::eLocal);
        
        /** Set anchor point value must be (0-1). */
        void setAnchorPoint(const Vector2& anchor);
        /** Get anchor point. */
        const Vector2& getAnchorPoint()const {return mAnchor;}
        
        /** get is need rendering */
        virtual bool isNeedRender() {return Node::isNeedRender() && mRenderProgram;}
        /** Get render hash. */
        uint getRenderStateHash()const {return mRenderStateHash;}
        
        void useProgramForRender(IProgram* program);
        IProgram* getProgramForRender() {return mRenderProgram;}
        virtual void setColor(const Color3& color) { mDiffColor = color; }
        const Color3& getColor()const { return mDiffColor; }
        
        /** Set clip enabled by parent. */
        void setClipEnabled(bool enable) {mClipEnabled = enable;}
        /** Get clip enabled by parent. */
        bool isClipEnabled() {return mClipEnabled;}
        /** Set local ZOrder, it's used for user. */
        void setLocalZOrder(int order);
        /** Get local ZOrder, it's used for user. */
        int getLocalZOrder() {return mLocalZOrder;}
        
        /** Is point in widget rect. */
        virtual bool isPointInZone(const Vector2& point);
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
        
    protected:
        Vector2     mScale;     // scaling
        float       mRotate;    // rotation value, z axis
        Rect        mRect;      // pos and size
        Vector2     mAnchor;    // anchor point, it will effect position/rotation/scaling
        Color3      mDiffColor; // widget diffuse color, Node have alpha attribute
        bool        mClipEnabled;       // is widget will auto be cliped by parent
        
        Vector2     mWorldScale;        // cache world scaling
        float       mWorldRotate;       // cache world rotate
        Rect        mWorldAnchorRect;   // cache world rendering rect, pos with anchor
        
        IProgram*   mRenderProgram;     // render program
        uint        mRenderStateHash;   // render state hash, used for instancing render
        bool        mIsRenderHashDirty; // is render state hash need update
        
        bool        mChildNeedSort;     // children need sort when addChild()
        int         mLocalZOrder;       // widget local zorder
        uint        mGlobalZOrder;      // widget global zorder, used for EventDispather
    };
}

#endif // PEACH3D_WIDGET_H
