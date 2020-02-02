/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>

#include <android_native_app_glue.h>
#include "Android/Peach3DPlatformAndroid.h"
#include "PlatformDelegate.h"

using namespace Peach3D;
/**
 * Process the next input event.
 */
static int32_t system_handle_input(struct android_app* app, AInputEvent* event)
{
    PlatformAndroid* platform = (struct PlatformAndroid*)app->userData;
    return platform->onInputEvent(event);
}

/**
 * Process the next main command.
 */
static void system_handle_cmd(struct android_app* app, int32_t cmd)
{
    PlatformAndroid* platform = (struct PlatformAndroid*)app->userData;
    IAppDelegate* delegate = platform->getCreationParams().delegate;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.
            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (app->window != NULL)  {
                // create egl and init opengl or only recreate egl surface
                platform->setAndroidWindow(app->window);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, do nothing
            break;
        case APP_CMD_GAINED_FOCUS:
            // When our app gains focus, resume animating
            delegate->appWillEnterForeground();
            break;
        case APP_CMD_LOST_FOCUS:
            // When our app loses focus, stop animating
            delegate->appDidEnterBackground();
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state)
{
    // new platform and init
    PlatformAndroid platform;
    PlatformDelegate delegate;
    PlatformCreationParams params;
    // params.MSAA = 4;
    // params.maxFPS = 30.0f;
    params.delegate = &delegate;
    params.logPath = "pdtest.log";
    platform.initWithParams(params, state->activity, state->config);

    // set event call back
    state->userData = &platform;
    state->onAppCmd = system_handle_cmd;
    state->onInputEvent = system_handle_input;

    // loop waiting for stuff to do.
    while (1)  {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(platform.isAnimating() ? 0 : -1, NULL, &events,
                (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL)  {
                source->process(state, source);
            }

            if (state->destroyRequested != 0) {
                return;
            }
        }

        // render frames, platform decide if need really render
        platform.renderOneFrame(0);
    }
}
//END_INCLUDE(all)
