LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

CPP_FILES := $(shell find $(LOCAL_PATH)/../../Classes -name *.cpp)
LOCAL_SRC_FILES := roguelike/main.cpp 
LOCAL_SRC_FILES += $(CPP_FILES:$(LOCAL_PATH)/%=%)

ifeq ($(HOST_OS),windows)
  LOCAL_C_INCLUDES := $(shell dir $(LOCAL_PATH)/../../Classes /b/ad/s)                   
else
  LOCAL_C_INCLUDES := $(shell find $(LOCAL_PATH)/../../Classes -type d)                   
endif

ifeq ($(APP_OPTIM),debug)
  LOCAL_CFLAGS := -w -g
endif

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,extensions)
$(call import-module,editor-support/cocosbuilder)
$(call import-module,editor-support/spine)
$(call import-module,editor-support/cocostudio)
$(call import-module,audio/android)
$(call import-module,network)

