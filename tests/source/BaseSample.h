//
//  BaseSample.h
//  test
//
//  Created by singoon.he on 7/31/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef BASE_SAMPLE_H
#define BASE_SAMPLE_H

#include <string>
#include "Peach3D.h"

using namespace Peach3D;
class BaseSample
{
public:
    virtual ~BaseSample() {}
    virtual void init(Widget* parentWidget) = 0;
    virtual void update(float lastFrameTime) {}
    
    const std::string& getSampleTitle() {return mTitle;}
    const std::string& getSampleDesc() {return mDesc;}
    
protected:
    std::string mTitle;
    std::string mDesc;
};

#endif /* defined(BASE_SAMPLE_H) */
