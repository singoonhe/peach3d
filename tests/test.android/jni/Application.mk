APP_ABI := armeabi-v7a
APP_STL := gnustl_static
APP_CPPFLAGS := -std=gnu++11
ifeq ($(NDK_DEBUG),1)
APP_CPPFLAGS += -DDEBUG=1
endif