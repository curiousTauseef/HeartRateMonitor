LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := heartratemonitor
LOCAL_SRC_FILES := HeartRateMonitorPreview.cpp

include $(BUILD_SHARED_LIBRARY)
