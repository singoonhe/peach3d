//
//  Peach3DPlatformWinDesk.h
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014Äê singoon.he. All rights reserved.
//

#ifndef PEACH3D_PLATFORMWINDESK_H
#define PEACH3D_PLATFORMWINDESK_H

#include "Peach3DIPlatform.h"
#include "Peach3DCommonDX.h"

namespace Peach3D
{
#if (PEACH3D_CURRENT_RENDER == PEACH3D_RENDER_DX11)
    // fix dll export warning C4251 when use stl
    template class PEACH3D_DLL std::map<std::wstring, std::string>;
#endif

    class PEACH3D_DLL PlatformWinDesk : public IPlatform
    {
    public:
        PlatformWinDesk();
        ~PlatformWinDesk();
        // init windows platform
        virtual bool initWithParams(const PlatformCreationParams &params);
        //! resume animating
        virtual void resumeAnimating();
        //! is flag key down
        virtual bool isFlagKeyDown(KeyCode flagKey);

        //! is application terminate, it's needed to trigger exit in windesk
        virtual bool isTerminating() { return mTerminating; }
        //! calc delta time, reduce the times for render frame
        virtual void renderOneFrame(float lastFrameTime);
        //! get label text texture
        ITexture* getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
            std::map<std::string, std::vector<Rect>>& clicksRect);

        /** Open url, "http://" is not necessary. */
        virtual void openUrl(const std::string& url);
        
    private:
        LARGE_INTEGER mLastRecordTime;    // last frame time record
        LARGE_INTEGER mFrequency;         // system frequency
        std::map<std::wstring, std::string> mClickedStrMap;
    };
}

#endif // PEACH3D_PLATFORMWINDESK_H