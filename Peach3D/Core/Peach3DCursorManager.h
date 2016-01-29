//
//  Peach3DCursorManager.h
//  Peach3DLib
//
//  Created by singoon.he on 1/29/16.
//  Copyright © 2016 singoon.he. All rights reserved.
//

#ifndef PEACH3D_CURSORMANAGER_H
#define PEACH3D_CURSORMANAGER_H

#include "Peach3DCompile.h"
#include "Peach3DSingleton.h"
#include "Peach3DSprite.h"

namespace Peach3D
{
    // define cursor state, use combined
    namespace CursorState
    {
        const uint Normal = 0x0001;
        const uint Down = 0x0002;       // mouse down
        const uint Count  = 2;          // cursor status count
    }
    
    class PEACH3D_DLL CursorManager : public Singleton < CursorManager >
    {
    public:
        CursorManager();
        /** Return is cursor current enabled. */
        bool isCursorEnabled() { return mIsCursorEnabled; }
        
        /** Set multi status texture use combination sign "|", default cursor will show before texture set. */
        void setTextureForStatus(ITexture* tex, uint status);
        /** Return status's texture. */
        ITexture* getTextureForStatus(uint status) { return mStatusTexs[getStatusIndex(status)]; }
        /** Set multi status rect use combination sign "|". */
        void setTextureRectForStatus(Rect rect, uint status);
        /** Return status's texture rect. */
        Rect getTextureRectForStatus(uint status) { return mStatusTexRects[getStatusIndex(status)]; }
        
        /** Update cursor animate and state, create or delete cursor sprite. */
        void prepareForRender();
        /** Update current cursor position. */
        void updateCursorPosition(const Vector2& pos);
        
    private:
        /** This need be called when device mouse plug-in or plug-out. */
        void setCursorEnabled(bool enabled) { mIsCursorEnabled = enabled; }
        // set current button show status
        void setButtonShowStatus(uint status);
        /** Get texture and rect index for show status. */
        uint getStatusIndex(uint status);
        
    protected:
        bool        mIsCursorEnabled;   // is current cursor enabled
        Sprite*     mCursorSprite;      // cursor sprite, highest zorder
        
        uint        mCurStatus;
        ITexture*   mStatusTexs[CursorState::Count];
        Rect        mStatusTexRects[CursorState::Count];
        
        friend class        IPlatform;
    };
}

#endif /* PEACH3D_CURSORMANAGER_H */
