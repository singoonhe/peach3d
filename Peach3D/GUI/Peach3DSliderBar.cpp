//
//  Peach3DSliderBar.cpp
//  Peach3DLib
//
//  Created by singoon.he on 23/11/2016.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#include "Peach3DSliderBar.hpp"

namespace Peach3D
{
    SliderBar* SliderBar::create(const char* bgImage, const char* barImage, const char* markImage, ProgressBarType type, Vector2 barOffset)
    {
        SliderBar* slider = new SliderBar();
        return slider;
    }
}
