//
//  Peach3DWidget.cpp
//  Peach3DLib
//
//  Created by singoon.he on 9/4/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#include "Peach3DWidget.h"
#include "Peach3DIPlatform.h"
#include "Peach3DLayoutManager.h"
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
    
    Widget::Widget() : mScale(1.0f, 1.0f), mClipEnabled(false), mAnchor(0.5f, 0.5f), mGlobalZOrder(0), mRotate(0.0f), mLocalZOrder(0), mChildNeedSort(false), mRenderProgram(nullptr), mRenderStateHash(0), mIsRenderHashDirty(true), mBindCorner(Vector2LeftBottom)
    {
        // set default diffuse color, not show widget
        mDiffColor.r = mDiffColor.g = mDiffColor.b = 1.f;
        mAlpha = 0.f;
        // set default auto scale
        mScaleTypeX = mScaleTypeY = AutoScaleType::eFix;
        mScaleTypeWidth = mScaleTypeHeight = AutoScaleType::eFix;
    }
    
    void Widget::setPosition(const Vector2& pos)
    {
        if (pos != mDesignPos) {
            mDesignPos = pos;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Widget::getPosition(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldPos;
        }
        else {
            return mDesignPos;
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
        if (size != mDesignSize) {
            mDesignSize = size;
            // set children need update attributes
            setNeedUpdateRenderingAttributes();
        }
    }
    
    const Vector2& Widget::getContentSize(TranslateRelative type)
    {
        if (type == TranslateRelative::eWorld) {
            updateRenderingAttributes(0.0f);
            return mWorldSize;
        }
        else {
            return mDesignSize;
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
    
    void Widget::useProgramForRender(const ProgramPtr& program)
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
            Vector2 anchorSize(mAnchor.x * mWorldSize.x, mAnchor.y * mWorldSize.y);
            if (mWorldRotate > FLT_EPSILON) {
                // Calc point offset to widget anchor position.
                Vector2 offset(point.x - mWorldPos.x, point.y - mWorldPos.y);
                // convert nevgative world rotate
                float sinTheta = cosf(mWorldRotate), cosTheta = cosf(mWorldRotate);
                Vector2 rotaOffset(offset.x * cosTheta - offset.y * sinTheta, offset.x * sinTheta + offset.y * cosTheta);
                rotaOffset = rotaOffset + anchorSize;
                if (rotaOffset.x < 0 || rotaOffset.y < 0 || rotaOffset.x > mWorldSize.x ||
                    rotaOffset.y > mWorldSize.y) {
                    inZoom = false;
                }
            }
            else {
                Vector2 startPos(mWorldPos.x - mAnchor.x * mWorldSize.x,
                                 mWorldPos.y - mAnchor.y * mWorldSize.y);
                if (point.x < startPos.x || point.y < startPos.y || point.x > (startPos.x + mWorldSize.x) ||
                    point.y > (startPos.y  + mWorldSize.y)) {
                    inZoom = false;
                }
            }
        }
        return inZoom;
    }
    
    void Widget::updateRenderingState(const std::string& extState)
    {
        // calc render hash code
        // current program may not ready, so can't use isNeedRender()
        if (mIsRenderHashDirty && mNeedRender) {
            // set default program if user or subclass not set
            if (!mRenderProgram) {
                mRenderProgram = ResourceManager::getSingleton().getPresetProgram(PresetProgramFeatures(false, false));
            }
            
            std::string states = extState + Utils::formatString("Program:%u", mRenderProgram->getProgramId());
            mRenderStateHash = XXH32((void*)states.c_str(), (int)states.size(), 0);
            mIsRenderHashDirty = false;
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
        
        if (mIsRenderDirty) {
            // update world position, world scale and world rotation
            mWorldSize = mDesignSize;
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
                Vector2 autoScaleV(getAutoScaleTypeValue(mScaleTypeWidth), getAutoScaleTypeValue(mScaleTypeHeight));
                mWorldSize = mWorldSize * Vector2(fabsf(mWorldScale.x), fabsf(mWorldScale.y)) * autoScaleV;
                // clamp widget size
                if (mMinSize.x > FLT_EPSILON && mWorldSize.x < mMinSize.x) {
                    mWorldSize.x = mMinSize.x;
                }
                if (mMinSize.y > FLT_EPSILON && mWorldSize.y < mMinSize.y) {
                    mWorldSize.y = mMinSize.y;
                }
                if (mMaxSize.x > FLT_EPSILON && mWorldSize.x > mMaxSize.x) {
                    mWorldSize.x = mMaxSize.x;
                }
                if (mMaxSize.y > FLT_EPSILON && mWorldSize.y > mMaxSize.y) {
                    mWorldSize.y = mMaxSize.y;
                }
                
                // convert pos to parent left-bottom pos (0, 0)
                auto parentSize = parentWidget->getContentSize(TranslateRelative::eWorld);
                auto offsetPos = mDesignPos * Vector2(getAutoScaleTypeValue(mScaleTypeX), getAutoScaleTypeValue(mScaleTypeY));
                mWorldPos = parentSize * mBindCorner + offsetPos;
                
                // cumulative parent offset, convert to world coordinate
                const Vector2 worldAnchorPos = parentSize * parentWidget->getAnchorPoint();
                Vector2 rotateOffset = mWorldPos * parentWidget->getScale(TranslateRelative::eWorld) - worldAnchorPos;
                float worldRotate = parentWidget->getRotation(TranslateRelative::eWorld);
                if (worldRotate > FLT_EPSILON) {
                    float sinRot = sinf(worldRotate), cosRot = cosf(worldRotate);
                    rotateOffset = Vector2(cosRot * rotateOffset.x - sinRot * rotateOffset.y,
                                           sinRot * rotateOffset.x + cosRot * rotateOffset.y);
                }
                mWorldPos = parentWidget->getPosition(TranslateRelative::eWorld) + rotateOffset;
            }
            
            mIsRenderDirty = false;
        }
        
        // update render state
        updateRenderingState();
    }
    
    float Widget::getAutoScaleTypeValue(AutoScaleType type)
    {
        float retV = 1.f;
        if (type == AutoScaleType::eWidth) {
            retV = LayoutManager::getSingleton().getWidthScale();
        }
        else if (type == AutoScaleType::eHeight) {
            retV = LayoutManager::getSingleton().getHeightScale();
        }
        else if (type == AutoScaleType::eMin) {
            retV = LayoutManager::getSingleton().getMinScale();
        }
        else if (type == AutoScaleType::eMax) {
            retV = LayoutManager::getSingleton().getMaxScale();
        }
        return retV;
    }
}