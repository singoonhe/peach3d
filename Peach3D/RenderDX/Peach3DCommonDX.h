#pragma once

#include <wrl.h>
#include <wrl/client.h>
#include <d3d12.h>
#include <d3d11_3.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include "Peach3DCompile.h"

// utf-8 convert to unicode, need manual free data
wchar_t* convertUTF8ToUnicode(const std::string& srcData);

// unicode convert to utf-8, need manual free data
char* convertUnicodeToUTF8(wchar_t* srcData);