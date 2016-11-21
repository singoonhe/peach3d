//
//  Peach3DProgressBar.hpp
//  Peach3DLib
//
//  Created by singoon.he on 21/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_PROGRESSBAR_H
#define PEACH3D_PROGRESSBAR_H

#include "Peach3DCompile.h"
#include "Peach3DSprite.h"

namespace Peach3D
{
    class PEACH3D_DLL ProgressBar : public Sprite
    {
    public:
        /** Create progress bar, static method.
         * @params
         */
        static ProgressBar* create(const char* bgImage, const char* barImage, Vector2 barOffset = Vector2Zero);
        static ProgressBar* create();
        
        /** Return bar sprite above on backgruond. */
        Sprite* getBarSprite() { return mBarSprite; };
        /** Set bar position relative of bg sprite, bar sprite anchor is (0, 0). */
        void setBarOffset(const Vector2& offset) { mBarSprite->setPosition(offset); }
        const Vector2& getBarOffset() { return mBarSprite ? mBarSprite->getPosition(): Vector2Zero; }
        
    public:
        ProgressBar() : mBarSprite(nullptr) {}
        virtual ~ProgressBar() {}
        
    private:
        Sprite* mBarSprite;
    };
}

#endif /* defined(PEACH3D_PROGRESSBAR_H) */
