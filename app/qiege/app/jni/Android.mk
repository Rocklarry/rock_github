LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CERTIFICATE :=platform
LOCAL_MODULE    := serial_port
LOCAL_SRC_FILES := SerialPort.c
LOCAL_LDLIBS := \
    -llog \
    -lz \
    -lm \

include $()BUILD_SHARED_LIBRARY