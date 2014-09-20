LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := rsmath
LOCAL_CFLAGS += -std=c99

LOCAL_SRC_FILES :=  $(addprefix rsMath/, $(notdir $(wildcard $(LOCAL_PATH)/rsMath/*.c) $(wildcard $(LOCAL_PATH)/rsMath/*.cpp)))

LOCAL_C_INCLUDES := $(LOCAL_PATH)/rsMath

include $(BUILD_STATIC_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := gl1

LOCAL_SRC_FILES := \
    driver.c \
    gl1.cpp \
    jwzgles.c \
    rgbhsl.cpp \
    rsMath.c \
    hufo_tunnel.cpp \
    hufo_tunnel_textures.cpp \
    lattice.c \
    lattice_textures.c \
    sundancer2.c

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH) $(LOCAL_PATH)/../bzip2/include $(LOCAL_PATH)/../optlist
LOCAL_STATIC_LIBRARIES := rsmath bzip2 optlist

include $(BUILD_SHARED_LIBRARY)
