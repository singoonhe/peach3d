//
//  Peach3DUtils.cpp
//  Peach3DLib
//
//  Created by singoon.he on 1/12/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#include "Peach3DUtils.h"
#include "Peach3DLogPrinter.h"
#include <sstream>
#include <zlib.h>
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
    
    uint Utils::unzipMemoryData(uchar *in, uint inLength, uchar **out)
    {
        uint outLength = 0;
        ssize_t bufferSize = 256 * 1024;
        *out = (unsigned char*)malloc(bufferSize);
        
        z_stream d_stream; /* decompression stream */
        d_stream.zalloc = (alloc_func)0;
        d_stream.zfree = (free_func)0;
        d_stream.opaque = (voidpf)0;
        
        d_stream.next_in  = in;
        d_stream.avail_in = static_cast<unsigned int>(inLength);
        d_stream.next_out = *out;
        d_stream.avail_out = static_cast<unsigned int>(bufferSize);
        
        int err = Z_OK;
        /* window size to hold 256k */
        if( (err = inflateInit2(&d_stream, 15 + 32)) != Z_OK )
            return 0;
        
        const float BUFFER_INC_FACTOR = 1.5;
        for (;;) {
            err = inflate(&d_stream, Z_NO_FLUSH);
            if (err == Z_STREAM_END) {
                break;
            }
            
            switch (err) {
                case Z_NEED_DICT:
                    err = Z_DATA_ERROR;
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    inflateEnd(&d_stream);
                    return 0;
            }
            
            // not enough memory ?
            if (err != Z_STREAM_END) {
                *out = (unsigned char*)realloc(*out, bufferSize * BUFFER_INC_FACTOR);
                
                /* not enough memory, ouch */
                if (! *out ) {
                    Peach3DErrorLog("unzip: realloc failed");
                    inflateEnd(&d_stream);
                    return 0;
                }
                
                d_stream.next_out = *out + bufferSize;
                d_stream.avail_out = static_cast<unsigned int>(bufferSize);
                bufferSize *= BUFFER_INC_FACTOR;
            }
        }
        outLength = bufferSize - d_stream.avail_out;
        err = inflateEnd(&d_stream);
        
        // check result
        if (err != Z_OK || *out == nullptr) {
            Peach3DErrorLog("unzip failed");
            if(*out) {
                free(*out);
                *out = nullptr;
            }
            outLength = 0;
        }
        return outLength;
    }
}
