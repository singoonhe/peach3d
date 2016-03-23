LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Peach3DLib

LOCAL_MODULE_FILENAME := libpeach3d

LOCAL_SRC_FILES := \
Core/Peach3DAction.cpp \
Core/Peach3DActionImplement.cpp \
Core/Peach3DCamera.cpp \
Core/Peach3DCursorManager.cpp \
Core/Peach3DEventDispatcher.cpp \
Core/Peach3DIObject.cpp \
Core/Peach3DIPlatform.cpp \
Core/Peach3DIRender.cpp \
Core/Peach3DIScene.cpp \
Core/Peach3DITexture.cpp \
Core/Peach3DLayoutManager.cpp \
Core/Peach3DLight.cpp \
Core/Peach3DLogPrinter.cpp \
Core/Peach3DMatrix4.cpp \
Core/Peach3DMesh.cpp \
Core/Peach3DNode.cpp \
Core/Peach3DOBB.cpp \
Core/Peach3DObjLoader.cpp \
Core/Peach3DPmbLoader.cpp \
Core/Peach3DPmtLoader.cpp \
Core/Peach3DQuaternion.cpp \
Core/Peach3DRenderNode.cpp \
Core/Peach3DResourceManager.cpp \
Core/Peach3DSceneManager.cpp \
Core/Peach3DSceneNode.cpp \
Core/Peach3DShaderCode.cpp \
Core/Peach3DUtils.cpp \
Core/Peach3DVector.cpp \
Core/Peach3DWidget.cpp \
RenderGL/Peach3DObjectGL.cpp \
RenderGL/Peach3DProgramGL.cpp \
RenderGL/Peach3DRenderGL.cpp \
RenderGL/Peach3DShaderCodeGL.cpp \
RenderGL/Peach3DTextureGL.cpp \
RenderGL/Peach3DImageParse.cpp \
GUI/Peach3DSprite.cpp \
GUI/Peach3DButton.cpp \
GUI/Peach3DLabel.cpp \
Platform/Android/Peach3DPlatformAndroid.cpp \
Support/xxhash/xxhash.cpp \
Support/tinyxml2/tinyxml2.cpp \
Support/gl3stub/gl3stub.c \
Support/gl3stub/glandroidext2.c

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Core \
                    $(LOCAL_PATH)/GUI \
                    $(LOCAL_PATH)/Platform \
                    $(LOCAL_PATH)/Support \
                    $(LOCAL_PATH)/RenderGL

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/Core \
                    $(LOCAL_PATH)/GUI \
                    $(LOCAL_PATH)/Platform \
                    $(LOCAL_PATH)/Support \
                    $(LOCAL_PATH)/RenderGL

LOCAL_CPPFLAGS += -frtti
LOCAL_CFLAGS += -DANDROID_DYNAMIC_ES3
LOCAL_EXPORT_CFLAGS += -DANDROID_DYNAMIC_ES3
LOCAL_EXPORT_LDLIBS := -lEGL -lGLESv2 -llog -lz -ljnigraphics
LOCAL_STATIC_LIBRARIES := Peach3DImage

include $(BUILD_STATIC_LIBRARY)

$(call import-add-path, $(LOCAL_PATH)/Support)
$(call import-module, projects/Android)
