LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libccjni

LOCAL_STATIC_LIBRARIES := libzip

# Make all warnings into errors
LOCAL_CPPFLAGS  := -Werror

# Reduce compilation time and make debugging produce the expected results.
# This is the default. So no need to turn on
#LOCAL_CPPFLAGS  += -O0

# CCRelease: Makes macros debugable
LOCAL_CPPFLAGS  += -g3

# CCRelease: We don't need accurate floating point maths, do we?
#LOCAL_CPPFLAGS  += -ffast-math
#LOCAL_CPPFLAGS  += -O3 -funroll-loops -DFPM_ARM

LOCAL_C_INCLUDES := \
				$(LOCAL_PATH)/ \
				$(LOCAL_PATH)/Rendering \
				$(LOCAL_PATH)/Tools \
\
				$(LOCAL_PATH)/../libzip \
\
				$(LOCAL_PATH)/../../../../Engine/Source \
				$(LOCAL_PATH)/../../../../Engine/Source/AI \
				$(LOCAL_PATH)/../../../../Engine/Source/Objects \
				$(LOCAL_PATH)/../../../../Engine/Source/Rendering \
				$(LOCAL_PATH)/../../../../Engine/Source/Scenes \
				$(LOCAL_PATH)/../../../../Engine/Source/Tools \
				$(LOCAL_PATH)/../../../../Engine/Source/UI \
\
				$(LOCAL_PATH)/../../../../External/3dsloader \
				$(LOCAL_PATH)/../../../../External/jansson-2.1/src \
				$(LOCAL_PATH)/../../../../External/ObjLoader3 \
\
				$(LOCAL_PATH)/../../../../App/Source/ \

# Use the wildcard operator so we only need to update the
# makefile if a new folder is added/one is removed or renamed
MY_FILES := \
		    $(wildcard $(LOCAL_PATH)/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/Rendering/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/Tools/*.cpp) \
\
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/AI/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/Objects/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/Rendering/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/Scenes/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/Tools/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../Engine/Source/UI/*.cpp) \
\
		    $(wildcard $(LOCAL_PATH)/../../../../External/3dsloader/*.cpp) \
		    $(wildcard $(LOCAL_PATH)/../../../../External/jansson-2.1/src/*.c) \
		    $(wildcard $(LOCAL_PATH)/../../../../External/ObjLoader3/*.cpp) \
\
		    $(wildcard $(LOCAL_PATH)/../../../../App/Source/*.cpp)

LOCAL_SRC_FILES := $(MY_FILES:$(LOCAL_PATH)/%=%)

LOCAL_LDLIBS  := -llog -lGLESv2
LOCAL_LDLIBS  += -L$(SYSROOT)/usr/lib -ldl
LOCAL_LDLIBS  += -L$(SYSROOT)/usr/lib -lz

include $(BUILD_SHARED_LIBRARY)
