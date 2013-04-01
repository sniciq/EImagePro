LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := EImagePro
LOCAL_SRC_FILES := EImagePro.cpp
LOCAL_LDFLAGS +=-ljnigraphics
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog

include $(BUILD_SHARED_LIBRARY)
