# SPDX-License-Identifier: BSD-3-Clause
# Copyright (C) 2013-2019 Intel Corporation
LOCAL_PATH := $(call my-dir)

## MEI Library
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS:= optional
LOCAL_SRC_FILES := mei.c
LOCAL_MODULE    := libmei
LOCAL_MODULE_OWNER := intel

LOCAL_COPY_HEADERS_TO := libmei
LOCAL_COPY_HEADERS := libmei.h
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SHARED_LIBRARIES := libcutils
LOCAL_PROPRIETARY_MODULE := true
include $(BUILD_SHARED_LIBRARY)


ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)

# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
    $(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS)

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS:= optional
LOCAL_SRC_FILES := mei.c
LOCAL_MODULE    := libmei
LOCAL_MODULE_OWNER := intel

LOCAL_COPY_HEADERS_TO := libmei
LOCAL_COPY_HEADERS := libmei.h
LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_STATIC_LIBRARIES := libcutils

include $(BUILD_STATIC_LIBRARY)

