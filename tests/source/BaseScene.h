//
//  BaseScene.h
//  test
//
//  Created by singoon.he on 8/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef BASE_SCENE_H
#define BASE_SCENE_H

#include "Peach3DIScene.h"
#include "BaseSample.h"

class BaseScene : public Peach3D::IScene
{
public:
    BaseScene() : mSampleIndex(0), mCurSample(nullptr) {}
    //! add a cube scene node for test
    virtual bool init();
    //! update cube rotation
    virtual void update(float lastFrameTime);
    
protected:
    /** Change to next index sample. */
    void changeToSample(int index);
    
protected:
    int     mSampleIndex;
    Widget* mSampleWidget;  // sample root widget
    Label*  mTitleLabel;
    Label*  mDescLabel;
    
    BaseSample* mCurSample;
    std::vector<std::function<BaseSample*()>>   mSampleList;
};

#endif /* defined(BASE_SCENE_H) */
