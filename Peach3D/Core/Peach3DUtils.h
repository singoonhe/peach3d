//
//  Peach3DUtils.h
//  Peach3DLib
//
//  Created by singoon.he on 1/12/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_UTILS_H
#define PEACH3D_UTILS_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    class PEACH3D_DLL Utils
    {
    public:
        /** String utils. */
        static std::string trimLeft(const std::string& str);
        static std::string trimRight(const std::string& str);
        static std::string trim(const std::string& str);
        /** Split string. */
        static std::vector<std::string> split(const std::string& str, char seg);
        /** Return format string, default buffer size 512. */
        static std::string formatString(const char* format, ...);
        
        /** Get rand float, clamp in (min, max). */
        static float rand(float min, float max);
        
        /** Unzip memory data, out data base size default 256K.
         * @params in Input memory data.
         * @params inLength Input memory data size.
         * @params out Output data, need manual free it .
         * @return Output data size if success, or return 0.
         */
        static uint unzipMemoryData(uchar *in, uint inLength, uchar **out);
    };
}

#endif /* defined(PEACH3D_UTILS_H) */
