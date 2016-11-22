//
//  UIScene.h
//  test
//
//  Created by singoon.he on 8/1/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef UI_SCENE_H
#define UI_SCENE_H

#include "BaseScene.h"

using namespace Peach3D;
class UIScene : public BaseScene
{
public:
    //! add test samples for test
    bool init();
};

class WidgetSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class SpriteSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class LabelSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class ButtonSample : public BaseSample
{
    void init(Widget* parentWidget);
private:
    Label*  mEventLabel;
    Button* mTitleButton;
    Widget* mParentWidget;
};

class ProgressBarSample : public BaseSample
{
    void init(Widget* parentWidget);
};

class LayoutSample : public BaseSample
{
    void init(Widget* parentWidget);
private:
    Button* mChangeButton;
    Label*  mNoticeLabel;
    Sprite* mLogoSprite;
};

#endif /* defined(UI_SCENE_H) */
