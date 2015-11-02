//
//  Peach3DActionImplement.h
//  Peach3DLib
//
//  Created by singoon.he on 5/7/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_ACTION_IMPLEMENT_H
#define PEACH3D_ACTION_IMPLEMENT_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    class IAction;
    class PEACH3D_DLL ActionImplement
    {
    public:
        /** Update actions. */
        virtual void prepareForRender(float lastFrameTime);
        
        /** Add a action to list, action will auto deleted. */
        void runAction(IAction* addAction);
        /**
         * Add multiple actions to list, action will auto deleted.
         * Important: params must end with "NULL".
         */
        void runActionList(IAction* addAction, ...);
        /** Stop all actions. */
        void stopActions();
        
    protected:
        virtual ~ActionImplement();
        
    protected:
        std::list<IAction*>     mActionList;                // all actions list, play by sequence
    };
}

#endif /* defined(PEACH3D_ACTION_IMPLEMENT_H) */
