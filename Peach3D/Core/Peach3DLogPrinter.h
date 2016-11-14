//
//  Peach3DLogPrinter.h
//  TestPeach3D
//
//  Created by singoon he on 12-5-5.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_LOGPRINTER_H
#define PEACH3D_LOGPRINTER_H

#include "stdio.h"
#include <string>
#include "Peach3DSingleton.h"
#include "Peach3DCompile.h"

namespace Peach3D
{
    // define system time struct
    struct PEACH3D_DLL SystemTimeval
    {
        int year;    // sytem year
        int month;   // sytem month
        int day;     // sytem day
        int hour;    // sytem hour
        int minute;  // sytem minute
        int second;  // sytem second
    };
    
    // Log level
    enum class PEACH3D_DLL LogLevel
    {
        eInfo = 0,   // info level
        eWarn = 1,   // warn level
        eError = 2   // error level
    };

    class PEACH3D_DLL LogPrinter : public Singleton < LogPrinter >
    {
    public:
        LogPrinter(const std::string& filePath = "");
        ~LogPrinter();
        //! Get current system timeval.
        /** \params timeval return current system timeval, include year,month,day,hour,minute,second,millisecond. */
        void getCurrentSystemTimeval(SystemTimeval *timeval);
        //! print log info
        void print(LogLevel level, const char* format, ...);

    private:
        FILE* mLogFp;
    };
    
    /** log print macro definition */
#define Peach3DLog(type, desc, ...)   LogPrinter::getSingleton().print(type, desc, ##__VA_ARGS__)
#define Peach3DInfoLog(desc, ...)     LogPrinter::getSingleton().print(Peach3D::LogLevel::eInfo, desc, ##__VA_ARGS__)
#define Peach3DWarnLog(desc, ...)     LogPrinter::getSingleton().print(Peach3D::LogLevel::eWarn, desc, ##__VA_ARGS__)
#define Peach3DErrorLog(desc, ...)    LogPrinter::getSingleton().print(Peach3D::LogLevel::eError, desc, ##__VA_ARGS__)
}

#endif // PEACH3D_LOGPRINTER_H
