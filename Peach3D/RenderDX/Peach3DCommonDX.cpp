
#include "Peach3DCommonDX.h"

wchar_t* convertUTF8ToUnicode(const std::string& srcData)
{
    int wcsLen = MultiByteToWideChar(CP_UTF8, NULL, srcData.c_str(), (int)srcData.size(), NULL, 0);
    wchar_t* wszString = new wchar_t[wcsLen + 1];
    MultiByteToWideChar(CP_UTF8, NULL, srcData.c_str(), (int)srcData.size(), wszString, wcsLen);
    wszString[wcsLen] = '\0';
    return wszString;
}

char* convertUnicodeToUTF8(wchar_t* srcData)
{
    int utf8Len = WideCharToMultiByte(CP_ACP, NULL, srcData, (int)wcslen(srcData), NULL, 0, " ", NULL);
    char* utf8String = new char[utf8Len + 1];
    WideCharToMultiByte(CP_ACP, NULL, srcData, (int)wcslen(srcData), utf8String, utf8Len, " ", NULL);
    utf8String[utf8Len] = '\0';
    return utf8String;
}