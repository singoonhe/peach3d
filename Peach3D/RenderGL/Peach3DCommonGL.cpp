//
//  Peach3DCommonGL.cpp
//  Peach3DLib
//
//  Created by singoon.he on 6/4/16.
//  Copyright (c) 2016 singoon.he. All rights reserved.
//

#include "Peach3DCommonGL.h"

void checkGlError(const char* op)
{
    GLint error = glGetError();
    if (error) {
        Peach3D::LogPrinter::getSingleton().print(Peach3D::LogLevel::eError, "After %s, gl error 0x%x", op, error);
    }
}