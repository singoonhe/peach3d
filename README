A game engine using c++ for multi platform. Using OpenGL for MAC, OpenGL ES for IOS and ANDROID, DX12 for Windows UWP. Engine don't include audio module.

In Android, you need modify "process_input" function in android_native_app_glue.c to avoid system error like "Failed to receive dispatch signal status=-11" or "keyDispatchingTimedOut". It maybe a bug of NDK.
static void process_input(struct android_app* app, struct android_poll_source* source) {
    AInputEvent* event = NULL;
    while (AInputQueue_hasEvents( app->inputQueue ) && AInputQueue_getEvent(app->inputQueue, &event) >= 0) {
        LOGV("New input event: type=%d\n", AInputEvent_getType(event));
        if (AInputQueue_preDispatchEvent(app->inputQueue, event)) {
            continue;
        }
        int32_t handled = 0;
        if (app->onInputEvent != NULL) handled = app->onInputEvent(app, event);
        AInputQueue_finishEvent(app->inputQueue, event, handled);
    }
}