//
//  TestScene.h
//  test
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "Peach3DIScene.h"
#include "Peach3D.h"

using namespace Peach3D;
class TestScene : public Peach3D::IScene
{
public:
    TestScene() : mMsgNode(nullptr) {}
    //! add a cube scene node for test
    bool init();
    //! update cube rotation
    void update(float lastFrameTime);
    
private:
    //! keyboard event function
    void keyboardEventCallback(KeyboardEvent event, KeyCode code);
    //! message box
    void showMessageBox(const std::string& text, const ControlListenerFunction& okCallBack);
    //! delete message box
    void deleteMsgWidgetAction(const Vector2&);
    
private:
    Widget* mMsgNode;
};

#endif // TEST_SCENE_H
