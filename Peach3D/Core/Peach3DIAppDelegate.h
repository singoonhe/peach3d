//
//  Peach3DIAppDelegate.h
//  Peach3DLib
//
//  Created by singoon.he on 9/5/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PEACH3D_IAPPDELEGAET_H
#define PEACH3D_IAPPDELEGAET_H

#include "Peach3DVector2.h"

namespace Peach3D
{
    class IAppDelegate
    {
    public:
        //! be called after render initialized
        virtual bool appDidFinishLaunching() = 0;
        
        //! be called when app enter background
        virtual void appDidEnterBackground() = 0;
        
        //! be called when app enter foreground
        virtual void appWillEnterForeground() = 0;
        
        //! be called after window size changed(not include oritation change)
        virtual void appWindowSizeChanged(const Peach3D::Vector2& size) {};
        
        //! user need delete delegate himself
    };
}

#endif // PEACH3D_IAPPDELEGAET_H
