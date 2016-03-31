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

using namespace Windows::Graphics::Display;

namespace DisplayMetrics
{
    // High resolution displays can require a lot of GPU and battery power to render.
    // High resolution phones, for example, may suffer from poor battery life if
    // games attempt to render at 60 frames per second at full fidelity.
    // The decision to render at full fidelity across all platforms and form factors
    // should be deliberate.
    static const bool SupportHighResolutions = false;

    // The default thresholds that define a "high resolution" display. If the thresholds
    // are exceeded and SupportHighResolutions is false, the dimensions will be scaled
    // by 50%.
    static const float DpiThreshold = 192.0f;		// 200% of standard desktop display.
    static const float WidthThreshold = 1920.0f;	// 1080p width.
    static const float HeightThreshold = 1080.0f;	// 1080p height.
};
namespace Peach3D
{
    class PEACH3D_DLL PlatformWinUwp : public IPlatform
    {
    public:
        PlatformWinUwp();
        ~PlatformWinUwp();
        // init windows platform
        virtual bool initWithParams(const PlatformCreationParams &params);
        // set window handler, auto init render
        virtual void setStoreWindow(void* window);
        /** Reset window logical size. */
        void setLogicalSize(Windows::Foundation::Size logicalSize);
        /** Reset window orientation. */
        void setCurrentOrientation(DisplayOrientations currentOrientation);
        /** Reset window dpi. */
        void setDpi(float dpi);
        /** Resume animating */
        virtual void resumeAnimating();
        /** Display content invalid, auto exit. */
        void displayInvalidExit();
        /** Return is current app running. */
        bool isRunning() { return !mTerminating; }

        //! calc delta time, reduce the times for render frame
        virtual void renderOneFrame(float lastFrameTime);
        //! get label text texture
        TexturePtr getTextTexture(const std::vector<LabelStageTextInfo>& textList, const LabelTextDefined& defined,
            std::map<std::string, std::vector<Rect>>& clicksRect);

        /** Open url, "http://" is not necessary. */
        virtual void openUrl(const std::string& url);

    protected:
        /** Calc device current display size and return rotation based on NativeOrientation, then init RenderDX. */
        bool initRenderDX();
        /** Calc device current display size. */
        DXGI_MODE_ROTATION computeDisplayRotation();

    private:
        LARGE_INTEGER mLastRecordTime;     // last frame time record
        LARGE_INTEGER mFrequency;          // system frequency

        Windows::Foundation::Size   mLogicalSize;       // window logical size
        DisplayOrientations         mNativeOrientation; // windows last orientation
        DisplayOrientations         mCurrentOrientation;// windows current orientation
        float                       mDpi;               // device screen DPI
    };
}

#endif // PEACH3D_PLATFORMWINUWP_H