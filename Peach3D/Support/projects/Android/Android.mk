LOCAL_PATH := $(call my-dir)/../..
include $(CLEAR_VARS)
LOCAL_MODULE := Peach3DImage

LOCAL_SRC_FILES := libpng/png.c libpng/pngerror.c libpng/pngget.c \
    libpng/pngmem.c libpng/pngpread.c libpng/pngread.c libpng/pngrio.c \
    libpng/pngrtran.c libpng/pngrutil.c libpng/pngset.c libpng/pngtrans.c \
    libpng/pngwio.c libpng/pngwrite.c libpng/pngwtran.c libpng/pngwutil.c \
    libjpeg/jaricom.c libjpeg/jcapimin.c libjpeg/jcapistd.c libjpeg/jcarith.c \
    libjpeg/jccoefct.c libjpeg/jccolor.c libjpeg/jcdctmgr.c libjpeg/jchuff.c \
    libjpeg/jcinit.c libjpeg/jcmainct.c libjpeg/jcmarker.c libjpeg/jcmaster.c \
    libjpeg/jcomapi.c libjpeg/jcparam.c libjpeg/jcprepct.c libjpeg/jcsample.c \
    libjpeg/jctrans.c libjpeg/jdapimin.c libjpeg/jdapistd.c libjpeg/jdarith.c \
    libjpeg/jdatadst.c libjpeg/jdatasrc.c libjpeg/jdcoefct.c libjpeg/jdcolor.c \
    libjpeg/jddctmgr.c libjpeg/jdhuff.c libjpeg/jdinput.c libjpeg/jdmainct.c \
    libjpeg/jdmarker.c libjpeg/jdmaster.c libjpeg/jdmerge.c libjpeg/jdpostct.c \
    libjpeg/jdsample.c libjpeg/jdtrans.c libjpeg/jerror.c libjpeg/jfdctflt.c \
    libjpeg/jfdctfst.c libjpeg/jfdctint.c libjpeg/jidctflt.c libjpeg/jidctfst.c \
    libjpeg/jidctint.c libjpeg/jquant1.c libjpeg/jquant2.c libjpeg/jutils.c \
    libjpeg/jmemmgr.c libjpeg/jmemansi.c

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/libpng \
                    $(LOCAL_PATH)/libjpeg

LOCAL_C_INCLUDES := $(LOCAL_PATH)/libpng \
                    $(LOCAL_PATH)/libjpeg

include $(BUILD_STATIC_LIBRARY)