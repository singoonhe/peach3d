//
//  Peach3DProgressBar.cpp
//  Peach3DLib
//
//  Created by singoon.he on 21/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DProgressBar.hpp"
#include "Peach3DResourceManager.h"

namespace Peach3D
{
    ProgressBar* ProgressBar::create(const char* bgImage, const char* barImage, Vector2 barOffset)
    {
        ProgressBar* progress = new ProgressBar();
        if (bgImage) {
            auto bgTex = ResourceManager::getSingleton().addTexture(bgImage);
            progress->setTextureFrame(bgTex);
            if (barImage) {
                // create bar sprite
                auto barTex = ResourceManager::getSingleton().addTexture(bgImage);
                auto barSprite = Sprite::create(barTex);
                barSprite->setAnchorPoint(Vector2Zero);
                if (barOffset == Vector2Zero) {
                    barSprite->setPosition(Vector2((bgTex->getWidth() - barTex->getWidth()) / 2.f, (bgTex->getHeight() - barTex->getHeight()) / 2.f));
                }
                else {
                    barSprite->setPosition(barOffset);
                }
                progress->addChild(barSprite);
            }
        }
        return progress;
    }
    
    ProgressBar* ProgressBar::create()
    {
        ProgressBar* progress = new ProgressBar();
        return progress;
    }
}
