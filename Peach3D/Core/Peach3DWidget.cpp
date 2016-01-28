//
//  Peach3DWidget.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DWidget.h"
#include "Peach3DIPlatform.h"
#include "Peach3DSceneManager.h"
#include "Peach3DResourceManager.h"
#include "Peach3DUtils.h"
#include "xxhash/xxhash.h"

namespace Peach3D
{
    Widget* Widget::create(const Vector2& pos, const Vector2& size)
    {
        Widget* widget = new Widget();
        widget->setPosition(pos);
        widget->setContentSize(size);
        if (size==Vector2Zero) {
            // default to set win size
            const Vector2& winSize = IPlatform::getSingleton().getCreationParams().winSize;
            widget->setContentSize(winSize);
        }
        return widget;
    }
    
    Widget::Widget() : mScale(1.0f, 1.0f), mClipEnabled(false), mAnchor(0.5f, 0.5f), mGlobalZOrder(0), mRotate(0.0f), mLocalZOrder(0), mChildNeedSort(false), mRenderProgram(nullptr), mRenderStateHash(0), mIsRenderHashDirty(true)
    {
        // set default diffuse color, not show widget
        mDiffColor = Color4(1.0f, 1.0f, 1.0f, 0.0f);
    }
    
    void Widget::setPosition(const Vector2& pos)
    {
        if (pos != mRect.pos) {
            mRect.pos = pos;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Widget::getPosition(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldAnchorRect.pos;
        }
        else {
            return mRect.pos;
        }
    }
    
    void Widget::setScale(const Vector2& scale)
    {
        if (scale != mScale) {
            mScale = scale;
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Widget::getScale(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldScale;
        }
        else {
            return mScale;
        }
    }
    
    void Widget::setRotation(float rotate)
    {
        if (!FLOAT_EQUAL(rotate, mRotate)) {
            mRotate = rotate;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    float Widget::getRotation(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldRotate;
        }
        else {
            return mRotate;
        }
    }
    
    void Widget::setContentSize(const Vector2& size)
    {
        if (size != mRect.size) {
            mRect.size = size;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Widget::getContentSize(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldAnchorRect.size;
        }
        else {
            return mRect.size;
        }
    }
    
    void Widget::setAnchorPoint(const Vector2& anchor)
    {
        if (anchor != mAnchor) {
            mAnchor = anchor;
            CLAMP(mAnchor.x, 0.0f, 1.0f);
            CLAMP(mAnchor.y, 0.0f, 1.0f);
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    void Widget::useProgramForRender(IProgram* program)
    {
        Peach3DAssert(program, "Object could not use a null program!");
        if (program) {
            // use new program
            mRenderProgram = program;
            mIsRenderHashDirty = true;
        }
    }
    
    void Widget::setLocalZOrder(int order)
    {
        mLocalZOrder = order;
        // set parent widget need sort children, when children zorder changed
        if (mParentNode && dynamic_cast<Widget*>(mParentNode)) {
            dynamic_cast<Widget*>(mParentNode)->mChildNeedSort = true;
        }
    }
    
    bool Widget::isPointInZone(const Vector2& point)
    {
        bool inZoom = Node::isPointInZone(point);
        if (inZoom) {
            Vector2 anchorSize(mAnchor.x * mWorldAnchorRect.size.x, mAnchor.y * mWorldAnchorRect.size.y);
            if (mWorldRotate > FLT_EPSILON) {
                // Calc point offset to widget anchor position.
                Vector2 offset(point.x - mWorldAnchorRect.pos.x, point.y - mWorldAnchorRect.pos.y);
                // convert nevgative world rotate
                float sinTheta = cosf(mWorldRotate), cosTheta = cosf(mWorldRotate);
                Vector2 rotaOffset(offset.x * cosTheta - offset.y * sinTheta, offset.x * sinTheta + offset.y * cosTheta);
                rotaOffset = rotaOffset + anchorSize;
                if (rotaOffset.x < 0 || rotaOffset.y < 0 || rotaOffset.x > mWorldAnchorRect.size.x ||
                    rotaOffset.y > mWorldAnchorRect.size.y) {
                    inZoom = false;
                }
            }
            else {
                Vector2 startPos(mWorldAnchorRect.pos.x - mAnchor.x * mWorldAnchorRect.size.x,
                                 mWorldAnchorRect.pos.y - mAnchor.y * mWorldAnchorRect.size.y);
                if (point.x < startPos.x || point.y < startPos.y || point.x > (startPos.x + mWorldAnchorRect.size.x) ||
                    point.y > (startPos.y  + mWorldAnchorRect.size.y)) {
                    inZoom = false;
                }
            }
        }
        return inZoom;
    }
    
    std::string Widget::getRenderStateString()
    {
        return Utils::formatString("Program:%u", mRenderProgram->getProgramId());
    }
    
    void Widget::setPresetProgram()
    {
        // set default program
        if (!mRenderProgram) {
            mRenderProgram = ResourceManager::getSingleton().getPresetProgram(VertexType::Point2, "PosColorVerShader2D", "PosColorFragShader2D");
        }
    }
    
    void Widget::updateRenderingAttributes(float lastFrameTime)
    {
        if (mChildNeedSort) {
            // sort all children by local zorder. std::sort will cause unpredictable order when equal.
            std::stable_sort(mChildNodeList.begin(), mChildNodeList.end(), [](Node* a, Node* b) {
                Widget* wa = dynamic_cast<Widget*>(a);
                Widget* wb = dynamic_cast<Widget*>(b);
                return wa->getLocalZOrder() < wb->getLocalZOrder();
            });
            mChildNeedSort = false;
        }
        
        // calc render hash code
        if (mIsRenderHashDirty && mNeedRender) {
            // set program first
            setPresetProgram();
            
            std::string states = getRenderStateString();
            mRenderStateHash = XXH32((void*)states.c_str(), (int)states.size(), 0);
            mIsRenderHashDirty = false;
        }
        
        if (mIsRenderDirty) {
            // update world position, world scale and world rotation
            mWorldAnchorRect = mRect;
            mWorldRotate = mRotate;
            mWorldScale = mScale;
            Widget* parentWidget = static_cast<Widget*>(mParentNode);
            if (parentWidget) {
                Widget* rootWidget = SceneManager::getSingletonPtr()->getRootWidget();
                if (parentWidget != rootWidget) {
                    // cumulative parent scale
                    mWorldScale = mWorldScale * parentWidget->getScale(TranslateRelative::eWorld);
                    // cumulative parent rotation
                    mWorldRotate = mWorldRotate + parentWidget->getRotation(TranslateRelative::eWorld);
                }
                // update world size
                mWorldAnchorRect.size = mWorldAnchorRect.size * Vector2(fabsf(mWorldScale.x), fabsf(mWorldScale.y));
                
                // cumulative parent offset
                const Vector2 worldAnchorPos = parentWidget->getContentSize(TranslateRelative::eWorld) * parentWidget->getAnchorPoint();
                Vector2 rotateOffset = mWorldAnchorRect.pos * parentWidget->getScale(TranslateRelative::eWorld) - worldAnchorPos;
                float worldRotate = parentWidget->getRotation(TranslateRelative::eWorld);
                if (worldRotate > FLT_EPSILON) {
                    float sinRot = sinf(worldRotate), cosRot = cosf(worldRotate);
                    rotateOffset = Vector2(cosRot * rotateOffset.x - sinRot * rotateOffset.y,
                                           sinRot * rotateOffset.x + cosRot * rotateOffset.y);
                }
                mWorldAnchorRect.pos = parentWidget->getPosition(TranslateRelative::eWorld) + rotateOffset;
            }
            
            mIsRenderDirty = false;
        }
    }
}