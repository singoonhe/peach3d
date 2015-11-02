//
//  Peach3DIScene.h
//  Peach3DLib
//
//  Created by singoon.he on 9/22/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_ISCENE_H
#define PEACH3D_ISCENE_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    class PEACH3D_DLL IScene
    {
    public:
        //! virtual destructor function, delete keyboard event listener
        virtual ~IScene();
        
        //! scene inited
        virtual bool init() = 0;
        //! scene will exit
        virtual void exit() {}
        //! scene update before render
        virtual void update(float lastFrameTime) {}
    };
}

#endif // PEACH3D_ISCENE_H
