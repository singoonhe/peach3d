//
//  Peach3DPlatformWinUwp.h
//  Peach3DLib
//
//  Created by singoon he on 14-9-1.
//  Copyright (c) 2014Äê singoon.he. All rights reserved.
//

#ifndef PEACH3D_PLATFORMWINUWP_H
#define PEACH3D_PLATFORMWINUWP_H

#include "Peach3DIPlatform.h"
#include "Peach3DCommonDX.h"

namespace Peach3D
{
    class PEACH3D_DLL PlatformWinUwp : public IPlatform
    {
    public:
        PlatformWinUwp();
        ~PlatformWinUwp();
        // init windows platform
        virtual bool initWithParams(const PlatformCreationParams &params);
        // set window handler, init renderdx and set rotate degree
        virtual void setStoreWindow(void* window, int width, int height, DXGI_MODE_ROTATION rotation);
        /** Resume animating */
        virtual void resumeAnimating();
        /** Display content invalid, auto exit. */
        void displayInvalidExit();
        /** Return is current app running. */
        bool isRunning() { return !mTerminating; }

        //! calc delta time, reduce the times for render frame
        virtual void renderOneFrame(float lastFrameTime);
        //! get label text texture
        ITexture* getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
            std::map<std::string, std::vector<Rect>>& clicksRect);
        
        /** Open url, "http://" is not necessary. */
        virtual void openUrl(const std::string& url);

    private:
        LARGE_INTEGER mLastRecordTime;     // last frame time record
        LARGE_INTEGER mFrequency;          // system frequency
    };
}

#endif // PEACH3D_PLATFORMWINUWP_H