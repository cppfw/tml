LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#TARGET_BUILD_TYPE := debug

LOCAL_MODULE := stob

LOCAL_SRC_FILES := stob/parser.cpp
LOCAL_SRC_FILES += stob/dom.cpp

LOCAL_CFLAGS := -Wno-div-by-zero #disable integer division by zero warning as it is sometimes useful when working with templates
LOCAL_CFLAGS += -DDEBUG

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../ting

LOCAL_LDLIBS := -llog -landroid

LOCAL_SHARED_LIBRARIES := ting

include $(BUILD_SHARED_LIBRARY)
