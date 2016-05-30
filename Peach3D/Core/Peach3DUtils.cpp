//
//  Peach3DUtils.cpp
//  Peach3DLib
//
//  Created by singoon.he on 1/12/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include <sstream>
#include "Peach3DUtils.h"
#if (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX)
#include <stdarg.h>
#endif

namespace Peach3D
{
    std::string Utils::trimLeft(const std::string& str)
    {
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (!isspace((unsigned char)str[i]))
            {
                return str.substr(i, str.size());
            }
        }
        return str;
    }
    
    std::string Utils::trimRight(const std::string& str)
    {
        for (int i = (int)str.size() - 1; i >= 0; --i)
        {
            if (!isspace((unsigned char)str[i]))
            {
                return str.substr(0, i+1);
            }
        }
        return str;
    }
    
    std::string Utils::trim(const std::string& str)
    {
        return trimLeft(trimRight(str));
    }
    
    std::vector<std::string> Utils::split(const std::string& str, char seg)
    {
        std::stringstream strData(str);
        std::string line;
        std::vector<std::string> lineList;
        while (std::getline(strData, line, seg))
        {
            lineList.push_back(line);
        }
        return lineList;
    }
    
    std::string Utils::formatString(const char* format, ...)
    {
        char fatStr[512];
        va_list arg_ptr;
        va_start(arg_ptr, format);
        vsprintf(fatStr, format, arg_ptr);
        va_end(arg_ptr);
        return fatStr;
    }
    
    float Utils::rand(float min, float max)
    {
        int range = int(max-min+1);
        return (::rand()%(range * 100)) / 100.f + min;
    }
}