LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := stob

SRC_BASE_DIR :=

LOCAL_SRC_FILES := $(SRC_BASE_DIR)stob/parser.cpp
LOCAL_SRC_FILES += $(SRC_BASE_DIR)stob/dom.cpp

LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ting

LOCAL_LDLIBS := -llog #-landroid

LOCAL_STATIC_LIBRARIES := ting

#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)

