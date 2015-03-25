# Desktop
_DESKTOP_LIBS = -lSDL2 -lGLEW -lGL
_DESKTOP_CFLAGS =
_DESKTOP_CXXFLAGS = 
_DESKTOP_SOURCES = 

# Android
_ANDROID_LIBS = -llog -landroid  -lEGL -lGLESv2
_ANDROID_CFLAGS =
_ANDROID_CXXFLAGS = 
_ANDROID_SOURCES = 

# Common
_CFLAGS = 
_CXXFLAGS = -std=c++11
_SOURCES = source/main.cpp source/graphics/graphics.c source/graphics/shader.c source/media/android/android.c source/media/android/assets.c source/media/android/graphics.c source/media/common/common.c source/media/desktop/assets.c source/media/desktop/desktop.c source/media/desktop/graphics.c
_HEADERS = source/tree.hpp source/4u/complex/complex.hpp source/4u/complex/quaternion.hpp source/4u/la/mat.hpp source/4u/la/vec.hpp source/4u/random/contrand.hpp source/4u/random/diskrand.hpp source/4u/random/rand.hpp source/4u/random/sphericrand.hpp source/4u/util/const.hpp source/4u/util/op.hpp source/graphics/graphics.h source/graphics/shader.h source/graphics/shader_source.h source/media/android/android.h source/media/android/platform.h source/media/common/common.h source/media/desktop/desktop.h source/media/desktop/platform.h source/media/assets.h source/media/event.h source/media/input.h source/media/log.h source/media/media.h
