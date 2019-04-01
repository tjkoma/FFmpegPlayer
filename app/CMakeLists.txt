# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/native-lib.cpp)

add_library( # Sets the name of the library.
             multi

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             src/main/cpp/multi.cpp)

add_library(division
            SHARED
            src/main/jni/division.cpp)

#导入头文件
include_directories(src/main/jniLibs/include)

set(distribution_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi)

add_library(avcodec57
            SHARED
            IMPORTED)

set_target_properties(
    avcodec57
    PROPERTIES IMPORTED_LOCATION
   ${distribution_DIR}/libavcodec57.so
)

add_library(avdevice57
            SHARED
            IMPORTED)

set_target_properties(
    avdevice57
    PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libavdevice57.so
)

add_library(avfilter6
            SHARED
            IMPORTED)

set_target_properties(
    avfilter6
    PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libavfilter6.so
)

add_library(avformat57
            SHARED
            IMPORTED)

set_target_properties(
    avformat57
    PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libavformat57.so
)

add_library(avutil55
            SHARED
            IMPORTED)

set_target_properties(
    avutil55
    PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libavutil55.so
)

add_library(swresample2
            SHARED
            IMPORTED)

set_target_properties(
    swresample2
    PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libswresample2.so
)

add_library(swscale4
            SHARED
            IMPORTED)

set_target_properties(
    swscale4
    PROPERTIES IMPORTED_LOCATION
    ${distribution_DIR}/libswscale4.so
)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )

target_link_libraries( # Specifies the target library.
                       multi

                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib} )
target_link_libraries( division
                       avcodec57
                       avdevice57
                       avfilter6
                       avformat57
                       avutil55
                       swresample2
                       swscale4
                       ${log-lib} )
