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
#include "Peach3DEventDispatcher.h"

using namespace Peach3D;
/**
 * Process the next input event.
 */
static int32_t system_handle_input(struct android_app* app, AInputEvent* event) 
{
    PlatformAndroid* platform = (struct PlatformAndroid*)app->userData;
    EventDispatcher* dispatcher = EventDispatcher::getSingletonPtr();
    int32_t eventType = AInputEvent_getType(event);
    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
        // deal with click event
        size_t clickCount = AMotionEvent_getPointerCount(event);
        int32_t clickAction = AMotionEvent_getAction(event);
        int pointerIndex = -1;
        // click event type
        ClickEvent cEvent = ClickEvent::eScrollWheel;   // init with invalid event in andorid
        switch( clickAction & AMOTION_EVENT_ACTION_MASK ) {
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                pointerIndex = (clickAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            case AMOTION_EVENT_ACTION_DOWN:
                cEvent = ClickEvent::eDown;
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                cEvent = ClickEvent::eDrag;
                break;
            case AMOTION_EVENT_ACTION_POINTER_UP:
                pointerIndex = (clickAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            case AMOTION_EVENT_ACTION_UP:
                cEvent = ClickEvent::eUp;
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
            case AMOTION_EVENT_ACTION_OUTSIDE:
                cEvent = ClickEvent::eCancel;
                break;
        }
        std::vector<uint> clickIds;
        std::vector<Vector2> poss;
        const float screenHeight = IPlatform::getSingleton().getCreationParams().winSize.y;
        if (pointerIndex >= 0 && (cEvent==ClickEvent::eDown || cEvent==ClickEvent::eUp)) {
            int32_t clickId = AMotionEvent_getPointerId(event, pointerIndex);
            float posx = AMotionEvent_getX(event, pointerIndex);
            float posy = AMotionEvent_getY(event, pointerIndex);
            clickIds.push_back(clickId+1);
            poss.push_back(Vector2(posx, screenHeight - posy));
            dispatcher->triggerClickEvent(cEvent, clickIds, poss);
        }
        else {
            for (int i = 0; i < clickCount; ++i) {
                int32_t clickId = AMotionEvent_getPointerId(event, i);
                float posx = AMotionEvent_getX(event, i);
                float posy = AMotionEvent_getY(event, i);
                clickIds.push_back(clickId+1);
                poss.push_back(Vector2(posx, screenHeight - posy));
                dispatcher->triggerClickEvent(cEvent, clickIds, poss);
            }
        }
        return 1;
    }
    else if (eventType == AINPUT_EVENT_TYPE_KEY)
    {
        // deal with key event
        int32_t eventKey = AKeyEvent_getKeyCode(event);
        // AKEY_STATE_UP or AKEY_STATE_DOWN
        int32_t eventState = AKeyEvent_getAction(event);
        KeyboardEvent keyEvent;
        if (eventState == AKEY_EVENT_ACTION_DOWN) {
            keyEvent = KeyboardEvent::eKeyDown;
        }
        else if (eventState == AKEY_EVENT_ACTION_UP) {
            keyEvent = KeyboardEvent::eKeyUp;
        }
        KeyCode key;
        if (eventKey == AKEYCODE_BACK) {
            key = KeyCode::eBack;
        }
        else if (eventKey == AKEYCODE_MENU) {
            key = KeyCode::eEnum;
        }
        dispatcher->triggerKeyboardEvent(keyEvent, key);
        return 1;
    }
    return 0;
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
    // Make sure glue isn't stripped.
    app_dummy();

    // new platform and init
    PlatformAndroid platform;
    PlatformDelegate delegate;
    PlatformCreationParams params;
    // params.MSAA = 4;
    // params.maxFramsePerSecond = 30.0f;
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
