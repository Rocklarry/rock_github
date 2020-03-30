LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := rest_server
LOCAL_SRC_FILES := \
    server/server.c
LOCAL_C_INCLUDES := \
  external/jsmn \
  external/mongoose \
  $(LOCAL_PATH)/external/jsmn \
  $(LOCAL_PATH)/network \
  $(LOCAL_PATH)/ssdp
LOCAL_SHARED_LIBRARIES := libbtop libntop libssdpop libmongoose
LOCAL_MODULE_TAGS := optional
include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)
LOCAL_MODULE := libntop
LOCAL_SRC_FILES := \
    network/network.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := libssdpop
LOCAL_SRC_FILES := \
    ssdp/ssdp.c
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
