//
//  MeshScene.h
//  test
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef EVENT_SCENE_H
#define EVENT_SCENE_H

#include "Peach3DIScene.h"
#include "Peach3D.h"

using namespace Peach3D;
class MeshScene : public Peach3D::IScene
{
public:
    MeshScene() : mMeshNode(nullptr), mScaleLabel(nullptr), mRotationLabel(nullptr), mNodeScale(1.0f, 1.0f, 1.0f) {}
    //! add a cube scene node for test
    bool init();
    //! scene update
    void update(float lastFrameTime);
    
private:
    //! reset node rotation
    void reset();
    // set scene node scale and show text
    void setNodeScale(const Vector3& scale);
    // set scene node rotation and show text
    void setNodeRotation(const Vector3& rotate);
    
private:
    Button*     mOpenFile;
    Label*      mNoticeLabel;
    SceneNode*  mMeshNode;
    Label*      mScaleLabel;
    Label*      mRotationLabel;
    
    std::string mChoosedFile;
    Vector3     mNodeRotation;  // scene node rotation
    Vector3     mNodeScale;     // scene node scale
};

#endif // EVENT_SCENE_H
