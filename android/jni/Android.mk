LOCAL_PATH := $(call my-dir)
SOURCE_PATH := ../../src
INCLUDE_PATH := ../../include/

include $(CLEAR_VARS)

LOCAL_MODULE := et
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(INCLUDE_PATH)
LOCAL_CXXFLAGS := --std=c++11

LOCAL_SRC_FILES := $(SOURCE_PATH)/core/debug.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/core/plist.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/core/tools.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/core/transformable.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/platform-unix/criticalsection.unix.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/platform-unix/mutex.unix.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/platform-unix/referencecounter.unix.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/platform-unix/thread.unix.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/platform-unix/threading.unix.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/geometry/geometry.cpp
LOCAL_SRC_FILES += $(SOURCE_PATH)/geometry/rectplacer.cpp

include $(BUILD_SHARED_LIBRARY)
