﻿project(kys)
cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 OLD)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_COMPILER $ENV{CXX})
set(CMAKE_CXX_FLAGS "-O2 -std=c++23 -pthread -DNDEBUG -DUSE_SDL_MIXER_AUDIO")

include_directories(../include . ../mlcc ../others ../local/include /usr/include)
link_directories(. ../lib)

file(GLOB SRC_LIST
    ./*.cpp
    ../mlcc/strfunc.cpp
    ../mlcc/filefunc.cpp
    ../mlcc/PotConv.cpp
    ../others/Hanz2Piny.cpp    
    )

set(LINK_LIBRARIES SDL3 SDL3_ttf SDL3_image SDL3_mixer lua5.4 opencc sqlite3 yaml-cpp zip)
if (APPLE)
    set(LINK_LIBRARIES ${LINK_LIBRARIES} iconv)
endif ()

add_executable(kys ${SRC_LIST})
target_link_libraries(kys ${LINK_LIBRARIES})
