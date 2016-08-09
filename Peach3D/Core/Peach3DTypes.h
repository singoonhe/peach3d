//
//  Peach3DTypes.h
//  Peach3DLib
//
//  Created by singoon.he on 1/12/15.
//  Copyright (c) 2015 singoon.he. All rights reserved.
//

#ifndef PEACH3D_TYPES_H
#define PEACH3D_TYPES_H

#include "Peach3DKeyCode.h"
#include "Peach3DVector3.h"
#include "Peach3DColor4.h"

namespace Peach3D
{
    /////////////////////RenderFeatureLevel////////////////////
    enum class PEACH3D_DLL RenderFeatureLevel
    {
        eUnkonw,// unknow feature level
        eDX,	// feature level for dx12
        eGL3,   // feature level for OpenGL 3.2 or OpenGL ES 3.0
        eGL2    // feature level for OpenGL ES 2.0 or OpenGL 2.x
    };
    
    /////////////////////RenderFeatureLevel////////////////////
    struct PEACH3D_DLL CameraState
    {
        Vector3 pos;        // camera position
        Vector3 rotation;   // camera rotation
        Vector3 lockPos;    // locked position for locked camera
        Vector3 up;         // camera up
        Vector3 forward;    // forward for free camera
        bool    isLocked;   // is camera locked to position(free camera or locked camera)
        CameraState &operator=(const CameraState& other) { pos = other.pos; rotation = other.rotation; lockPos = other.lockPos; up = other.up; forward = other.forward; isLocked = other.isLocked; return *this; }
    };
    
    /////////////////////input event////////////////////
    // define mouse and touch event
    enum class PEACH3D_DLL ClickEvent
    {
        eMoved,         // mouse moved
        eDown,          // mouse left down or touch begin
        eDrag,          // mouse left drag or touch drag
        eUp,            // mouse left up or touch end
        eCancel,        // touch cancel
        eRightDown,     // mouse right down
        eRightDrag,     // mouse right drag
        eRightUp,       // mouse right up
        eMoveIn,        // move in node
        eMoveOut,       // move out node
        eMoveHold,      // move hold for a moment
        eDragIn,        // mouse or touch drag in node
        eDragOut,       // mouse or touch drag out node, it's also equal to touch cancel
        eDragHold,      // mouse or touch drag hold for a moment
        eClicked,       // mouse left or touch clicked
        eDClicked,      // mouse left or touch double clicked
        eRightClicked,  // mouse right clicked
        eRightDClicked, // mouse right double clicked
        eScrollWheel,   // mouse middle scroll wheel
        ePinch,         // touch gesture event, only for root widget(first pos x is two figures changed distance)
        eRotation,      // touch gesture event, only for root widget(not complete)
    };
    // define keyboard event, include android key and wp8 key
    enum class PEACH3D_DLL KeyboardEvent
    {
        eKeyDown,
        eKeyUp,
    };
    
    /////////////////////label enum////////////////////
    // define text vertical alignment
    enum class PEACH3D_DLL TextVAlignment
    {
        eTop,
        eCenter,
        eBottom
    };
    // define text horizontal alignment
    enum class PEACH3D_DLL TextHAlignment
    {
        eLeft,
        eCenter,
        eRight
    };
    // define label text per stage info
    struct PEACH3D_DLL LabelStageTextInfo
    {
        LabelStageTextInfo():clickEnabled(false) {}
        std::string text;   // draw text
        std::string image;  // draw image, text will ignore
        Color4      color;  // text color, image not effected
        bool        clickEnabled;   // is text click enabled
    };
    // define label init defined
    struct PEACH3D_DLL LabelTextDefined
    {
        std::string     font;       // label font
        float           fontSize;   // label font size
        TextHAlignment  hAlign;
        TextVAlignment  vAlign;
        Vector2         dim;        // label max content size
        TextVAlignment  imageVAlign; // image vertical alignment in label
    };
    
    // define resource loading data info
    struct PEACH3D_DLL ResourceLoaderInput {
        ResourceLoaderInput(void* _data, ulong _length, void* handler = nullptr, const char* _dir = nullptr) :
        data(_data), length(_length), dir(_dir), handler(handler) {}
        
        void*   data;       // resource data
        ulong   length;     // resource data size
        const char* dir;    // resource local directory
        void*   handler;    // used data handler
    };
}

#endif
