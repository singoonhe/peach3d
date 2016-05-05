//
//  PlatformDelegate.h
//  test
//
//  Created by singoon.he on 9/5/14.
//  Copyright (c) 2014 singoon.he. All rights reserved.
//

#ifndef PLATFORM_DELEGATE_H
#define PLATFORM_DELEGATE_H

#include "Peach3DIAppDelegate.h"

class PlatformDelegate : public Peach3D::IAppDelegate
{
public:
    //! be called after render initialized
    virtual bool appDidFinishLaunching();
    
    //! be called when app enter background
    virtual void appDidEnterBackground();
    
    //! be called when app enter foreground
    virtual void appWillEnterForeground();
    
    //! be called after window size changed(not include oritation change)
    virtual void appWindowSizeChanged(const Peach3D::Vector2& size);
};

#endif // PLATFORM_DELEGATE_H
