//
//  Peach3DWorldManager.h
//  Peach3DLib
//
//  Created by singoon.he on 5/17/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_WORLDMANAGER_H
#define PEACH3D_WORLDMANAGER_H

#include "Peach3DCompile.h"
#include "Peach3DSingleton.h"
#include "Peach3DSceneNode.h"

namespace Peach3D
{
    // redefine world load binding and over call func, it's too long
    typedef std::function<void(const std::string&, SceneNode*)> WorldVarBindFunction;
    typedef std::function<void(const std::string&)>             WorldLoadOverFunction;
    
    class PEACH3D_DLL WorldManager : public Singleton < WorldManager >
    {
    public:
        WorldManager() {}
        ~WorldManager() {}
    };
}

#endif /* defined(PEACH3D_WORLDMANAGER_H) */
