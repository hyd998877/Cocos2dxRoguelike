LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

CPP_FILES := $(shell find $(LOCAL_PATH)/../../Classes -name *.cpp)
LOCAL_SRC_FILES := roguelike/main.cpp 
LOCAL_SRC_FILES += $(CPP_FILES:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
                   $(LOCAL_PATH)/../../Classes/dao \
                   $(LOCAL_PATH)/../../Classes/data \
                   $(LOCAL_PATH)/../../Classes/util \
                   $(LOCAL_PATH)/../../Classes/account \
                   $(LOCAL_PATH)/../../Classes/battle \
                   $(LOCAL_PATH)/../../Classes/mypage \
                   $(LOCAL_PATH)/../../Classes/ui \
                   $(LOCAL_PATH)/../../Classes/quest \
                   $(LOCAL_PATH)/../../Classes/actor \
                   $(LOCAL_PATH)/../../Classes/logic \
                   $(LOCAL_PATH)/../../Classes/game_object \
                   $(LOCAL_PATH)/../../Classes/ui/table \
                   $(LOCAL_PATH)/../../Classes/map \
                   $(LOCAL_PATH)/../../Classes/novel \
                   $(LOCAL_PATH)/../../Classes/rogue


LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static

include $(BUILD_SHARED_LIBRARY)

$(call import-module,extensions)
$(call import-module,editor-support/cocosbuilder)
$(call import-module,editor-support/spine)
$(call import-module,2d)
$(call import-module,audio/android)

