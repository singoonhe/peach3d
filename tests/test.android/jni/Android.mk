# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := test
LOCAL_SRC_FILES := main.cpp \
../../source/EventScene.cpp \
../../source/TestScene.cpp \
../../source/WidgetScene.cpp \
../../source/PlatformDelegate.cpp

LOCAL_LDLIBS    := -llog -landroid
LOCAL_WHOLE_STATIC_LIBRARIES := android_native_app_glue Peach3DLib

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../source

include $(BUILD_SHARED_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/../../..)
$(call import-module, Peach3D)
$(call import-module,android/native_app_glue)
