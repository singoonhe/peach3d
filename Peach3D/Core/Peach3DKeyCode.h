//
//  Peach3DKeyCode.h
//  Peach3DLib
//
//  Created by singoon he on 12-4-15.
//  Copyright (c) 2012年 singoon.he. All rights reserved.
//

#ifndef PEACH3D_KEYCODE_H
#define PEACH3D_KEYCODE_H

#include "Peach3DCompile.h"

namespace Peach3D
{
    // define key code
    enum class KeyCode
    {
        eUnknow = 0,
#if PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_WINDESK || PEACH3D_CURRENT_PLATFORM == PEACH3D_PLATFORM_WINSTORE
        // for windows
        eBackSpace = 8,
        eTab = 9,
        eClear = 12,
        eEnter = 13,
        eShift= 16,
        eControl = 17,
        eAlt = 18,
        eCapeLock = 20,
        eESC = 27,
        eSpacebar = 32,
        ePageUp = 33,
        ePageDown,
        eEnd,
        eHome,
        eLeftArrow,
        eUpArrow,
        eRightArrow,
        eDownArrow,
        eSelect,
        ePrint,
        eExecute,
        eSnapShot,
        eInsert,
        eDelete,
        eHelp,
        eNum0 = 48,
        eNum1,
        eNum2,
        eNum3,
        eNum4,
        eNum5,
        eNum6,
        eNum7,
        eNum8,
        eNum9,
        eKeyA = 65,
        eKeyB,
        eKeyC,
        eKeyD,
        eKeyE,
        eKeyF,
        eKeyG,
        eKeyH,
        eKeyI,
        eKeyJ,
        eKeyK,
        eKeyL,
        eKeyM,
        eKeyN,
        eKeyO,
        eKeyP,
        eKeyQ,
        eKeyR,
        eKeyS,
        eKeyT,
        eKeyU,
        eKeyV,
        eKeyW,
        eKeyX,
        eKeyY,
        eKeyZ,
        eLeftWindows,   // left windows key
        eRightWindows,  // right windows key
        ekeypad0 = 96,  // "keypad num 0"
        ekeypad1,
        ekeypad2,
        ekeypad3,
        ekeypad4,
        ekeypad5,
        ekeypad6,
        ekeypad7,
        ekeypad8,
        ekeypad9,
        ekeypadMul,     // "keypad *"
        ekeypadAdd,     // "keypad +"
        ekeypadEnter,   // "keypad enter"
        ekeypadMinus,   // "keypad -"
        ekeypadDec,     // "keypad ."
        ekeypadDiv,     // "keypad /"
        eF1,
        eF2,
        eF3,
        eF4,
        eF5,
        eF6,
        eF7,
        eF8,
        eF9,
        eF10,
        eF11,
        eF12,
        eNumLock = 144,
        eScrollLock = 145,
        eLeftShift = 160,
        eRightShift,
        eLeftControl,
        eRightControl,
        eLeftAlt,
        eRightAlt,
        eSemiColon = 186,       // ";:"
        eEqual = 187,           // "=+"
        eComma =188,            // ",<"
        eMinus = 189,           // "-_"
        ePeriod =190,           // ".>"
        eSlash = 191,           // "/?"
        eGrave = 192,           // "`~"
        eLeftBracket = 219,     // "[{"
        eBackslash = 220,       // "\|"
        eRightBracket = 221,    // "]}"
        eQuote = 222,           // "'""
        // windows not used key
        eCommand = 301,
#else
        // for MAC
        eTab = 0x30,
        eClear = 0x47,
        eEnter = 0x24,
        eShift= 0x38,
        eControl = 0x3B,
        eAlt = 0x3A,
        eCommand = 0x37,
        eCapeLock = 0x39,
        eESC = 0x35,
        eSpacebar = 0x31,
        ePageUp = 0x74,
        ePageDown = 0x79,
        eEnd = 0x77,
        eHome = 0x73,
        eLeftArrow = 0x7B,
        eUpArrow = 0x7E,
        eRightArrow = 0x7C,
        eDownArrow = 0x7D,
        eInsert = 0x75,
        eDelete = 0x33,
        eHelp = 0x72,
        eNum0 = 0x1D,
        eNum1 = 0x12,
        eNum2 = 0x13,
        eNum3 = 0x14,
        eNum4 = 0x15,
        eNum5 = 0x17,
        eNum6 = 0x16,
        eNum7 = 0x1A,
        eNum8 = 0x1C,
        eNum9 = 0x19,
        eKeyA = 0x00,
        eKeyB = 0x0B,
        eKeyC = 0x08,
        eKeyD = 0x02,
        eKeyE = 0x0E,
        eKeyF = 0x03,
        eKeyG = 0x05,
        eKeyH = 0x04,
        eKeyI = 0x22,
        eKeyJ = 0x26,
        eKeyK = 0x28,
        eKeyL = 0x25,
        eKeyM = 0x2E,
        eKeyN = 0x2D,
        eKeyO = 0x1F,
        eKeyP = 0x23,
        eKeyQ = 0x0C,
        eKeyR = 0x0F,
        eKeyS = 0x01,
        eKeyT = 0x11,
        eKeyU = 0x20,
        eKeyV = 0x09,
        eKeyW = 0x0D,
        eKeyX = 0x07,
        eKeyY = 0x10,
        eKeyZ = 0x06,
        ekeypad0 = 0x52,    // "keypad num 0"
        ekeypad1 = 0x53,
        ekeypad2 = 0x54,
        ekeypad3 = 0x55,
        ekeypad4 = 0x56,
        ekeypad5 = 0x57,
        ekeypad6 = 0x58,
        ekeypad7 = 0x59,
        ekeypad8 = 0x5B,
        ekeypad9 = 0x5C,
        ekeypadMul = 0x43,     // "keypad *"
        ekeypadAdd = 0x45,     // "keypad +"
        ekeypadEnter = 0x4C,   // "keypad enter"
        ekeypadMinus = 0x4E,   // "keypad -"
        ekeypadDec = 0x41,     // "keypad ."
        ekeypadDiv = 0x4B,     // "keypad /"
        eF1 = 0x7A,
        eF2 = 0x78,
        eF3 = 0x63,
        eF4 = 0x76,
        eF5 = 0x60,
        eF6 = 0x61,
        eF7 = 0x62,
        eF8 = 0x64,
        eF9 = 0x65,
        eF10 = 0x6D,
        eF11 = 0x67,
        eF12 = 0x6F,
        eLeftShift = 0x38,
        eRightShift = 0x3C,
        eLeftControl = 0x3B,
        eRightControl = 0x3E,
        eLeftAlt = 0x3A,
        eRightAlt = 0x3D,
        eSemiColon = 0x29,      // ";:"
        eEqual = 0x18,          // "=+"
        eComma =0x2B,           // ",<"
        eMinus = 0x1B,          // "-_"
        ePeriod =0x2F,          // ".>"
        eSlash = 0x2C,          // "/?"
        eGrave = 0x32,          // "`~"
        eLeftBracket = 0x21,    // "[{"
        eBackslash = 0x2A,      // "\|"
        eRightBracket = 0x1E,   // "]}"
        eQuote = 0x27,          // "'""
        // MAC not used key
        eBackSpace = 301,
        eSelect,
        ePrint,
        eExecute,
        eSnapShot,
        eLeftWindows,
        eRightWindows,
        eNumLock,
        eScrollLock,
#endif
        eBack = 401,            // android or wp8 back
        eEnum = 402,            // android enum
    };
}

#endif // PEACH3D_KEYCODE_H
