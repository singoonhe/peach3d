//
//  IRender.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DCompile.h"
#include "Peach3DIRender.h"
#include "Peach3DLogPrinter.h"
#include "Peach3DLayoutManager.h"

namespace Peach3D
{
    IMPLEMENT_SINGLETON_STATIC(IRender);
    
    bool IRender::initRender(const Vector2& size)
    {
        // also set layout window size
        LayoutManager::getSingleton().setScreenSize(size);
        // default set render valid, DX or OpenGL can modify it
        mIsRenderValid = true;
        return true;
    }
}