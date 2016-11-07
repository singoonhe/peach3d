//
//  LightScene.h
//  test
//
//  Created by singoon.he on 3/8/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef LIGHT_SCENE_H
#define LIGHT_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class LightScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
    void exit();
    
private:
    Matrix4   mSysPMat;
};

class DirLightSample : public BaseSample
{
    void init(Widget* parentWidget);
    void update(float lastFrameTime);
    virtual ~DirLightSample();
    
private:
    LightPtr   mDirLight;
};

class DotLightSample : public BaseSample
{
    void init(Widget* parentWidget);
    void update(float lastFrameTime);
    virtual ~DotLightSample();
    
private:
    LightPtr    mDotLight;
    SceneNode*  mDotNode;
};

class SpotLightSample : public BaseSample
{
    void init(Widget* parentWidget);
    void update(float lastFrameTime);
    virtual ~SpotLightSample();
    
private:
    LightPtr mSpotLight;
    Vector3  mLockPos;
    Vector3  mSpotPos;
};

class LightTextureSample : public BaseSample
{
    void init(Widget* parentWidget);
    virtual ~LightTextureSample();
};

class MultiLightsSample : public BaseSample
{
    void init(Widget* parentWidget);
    void update(float lastFrameTime);
    virtual ~MultiLightsSample();
    
private:
    LightPtr mDotLight;
    SceneNode*  mDotNode;
    LightPtr mSpotLight;
    Vector3  mLockPos;
    Vector3  mSpotPos;
};

class LightControlSample : public BaseSample
{
    void init(Widget* parentWidget);
    virtual ~LightControlSample();
    
private:
    LightPtr    mDotLight;
    SceneNode*  mControlNode;
};

#endif /* LIGHT_SCENE_H */
