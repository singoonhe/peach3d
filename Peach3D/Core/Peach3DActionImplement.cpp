//
//  Peach3DActionImplement.cpp
//  Peach3DLib
//
//  Created by singoon.he on 5/7/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DActionImplement.h"
#include "Peach3DAction.h"
#if (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX)
#include <stdarg.h>
#endif

namespace Peach3D
{
    void ActionImplement::prepareForRender(float lastFrameTime)
    {
        // update action list
        if (mActionList.size() > 0) {
            IAction* firstAction = mActionList.front();
            firstAction->update(this, lastFrameTime);
            
            // delete first action if action finished
            if (firstAction->isActionFinished()) {
                delete firstAction;
                mActionList.pop_front();
            }
        }
    }
    
    void ActionImplement::runAction(IAction* addAction)
    {
        Peach3DAssert(addAction && !addAction->isAdded(), "One Action can't added twice or not end list with NULL!");
        
        if (addAction && !addAction->isAdded()) {
            mActionList.push_back(addAction);
            // set Action can't added any more
            addAction->setAdded();
        }
    }
    
    void ActionImplement::runActionList(IAction* addAction, ...)
    {
        // add first action
        runAction(addAction);
        
        va_list arg_ptr;
        va_start(arg_ptr, addAction);
        IAction* newAction = va_arg(arg_ptr, IAction*);
        while (newAction) {
            runAction(newAction);
            newAction = va_arg(arg_ptr, IAction*);
        }
        va_end(arg_ptr);
    }
    
    void ActionImplement::stopActions()
    {
        for (auto inAction : mActionList) {
            delete inAction;
        }
        mActionList.clear();
    }
    
    ActionImplement::~ActionImplement()
    {
        // delete all actions
        stopActions();
    }
}