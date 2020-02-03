//
//  Peach3DLogPrinter.cpp
//  TestPeach3D
//
//  Created by singoon he on 12-5-5.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#include "Peach3DLogPrinter.h"
#include "Peach3DCompile.h"
#include "Peach3DIPlatform.h"
#if (PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_ANDROID)
#include <android/log.h>
#elif (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX)
#include <stdarg.h>
#include <windows.h>
#endif

namespace Peach3D
{    
    LogPrinter::LogPrinter(const std::string& filePath) : mLogFp(nullptr)
    {
        // there can't print log if open file failed,
        // because IPlatform did not initalized.
        if (filePath.size() > 0) {
            if(filePath[0] != '/' && filePath.find(':') == std::string::npos) {
                // Notice: auto set file under writeable dir.
                mLogFp = fopen((IPlatform::getSingleton().getWriteablePath() + filePath).c_str(), "w");
            }
            else {
                mLogFp = fopen(filePath.c_str(), "w");
            }
        }
    }
    
    void LogPrinter::getCurrentSystemTimeval(SystemTimeval *timeval)
    {
        if (timeval)
        {
            // get current time
            time_t current = time(NULL);
            struct tm * time = localtime(&current);
            // return system time
            timeval->year = time->tm_year + 1900;
            timeval->month = time->tm_mon + 1;
            timeval->day = time->tm_mday;
            timeval->hour = time->tm_hour;
            timeval->minute = time->tm_min;
            timeval->second = time->tm_sec;
        }
    }

    void LogPrinter::print(LogLevel level, const char* format, ...)
    {
        // do nothing when release and no log file
#if PEACH3D_DEBUG == 0
        if (mLogFp == nullptr) {
            return ;
        }
#endif
        
        // get fromat system time
        char stime[100] = { 0 };
#if (PEACH3D_CURRENT_PLATFORM != PEACH3D_PLATFORM_ANDROID)
        SystemTimeval timeval;
        getCurrentSystemTimeval(&timeval);
        sprintf(stime, "%d/%02d/%02d %02d:%02d:%02d", timeval.year, timeval.month, timeval.day, timeval.hour, timeval.minute, timeval.second);
#endif

        // get unfold log
        char logInfo[10240], outLog[10240];
        va_list arg_ptr;
        va_start(arg_ptr, format);
        vsprintf(logInfo, format, arg_ptr);
        va_end(arg_ptr);

        // whole log info
        std::string levelName[] = { "Info", "Warn", "Error" };
        sprintf(outLog, "%s (%s) %s\n", stime, levelName[(int)level].c_str(), logInfo);

        if (mLogFp) {
            // write to file
            fwrite(outLog, 1, strlen(outLog), mLogFp);
            fflush(mLogFp);
        }
        
#if PEACH3D_DEBUG == 1
#if (PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_ANDROID)
        int logLevel = ANDROID_LOG_INFO;
        if (level == LogLevel::eWarn)
        {
            logLevel = ANDROID_LOG_WARN;
        }
        else if (level == LogLevel::eError)
        {
            logLevel = ANDROID_LOG_ERROR;
        }
        __android_log_print(logLevel, "Peach3D", "%s", outLog);
#elif (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX)
        OutputDebugStringA(outLog);
#else
        printf("%s", outLog);
#endif
#endif
    }

    LogPrinter::~LogPrinter()
    {
        if (mLogFp)
        {
            fclose(mLogFp);
            mLogFp = nullptr;
        }
    }
}
