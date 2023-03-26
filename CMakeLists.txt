##########
## Init ##
##########
cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(euclid)
include(lib/cmake-utils/dependency-integrator.cmake)

option(debug "debug" OFF)


############
## source ##
############
set(
        SOURCE_FILES
        src/paradigm.h
        src/foundation.h

        src/main.cpp
        src/euclid.h

        src/foundation/interfaces.h
        src/foundation/time.h

        src/ingress/audio_input.h
        src/ingress/ring_buffer.h
        src/ingress/stft.h
        src/egress/opus.h src/foundation/primitive.h)
add_executable(euclid ${SOURCE_FILES})
target_include_directories(euclid PRIVATE src)

###########
### WASM ##
###########
if (EMSCRIPTEN)
    message("(~) building for wasm")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sALLOW_MEMORY_GROWTH")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sMAXIMUM_MEMORY=4GB")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sPTHREAD_POOL_SIZE=8")
    if (debug)
        message("(~) [x] emcc debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sASSERTIONS")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --memoryprofiler")
    else ()
        message("(~) [ ] emcc debug")
    endif ()
    add_compile_definitions(WASM EMSCRIPTEN)
else ()
    message("not building for wasm")
    find_package(SDL2 REQUIRED)
    include_directories(euclid ${SDL2_INCLUDE_DIRS})
    target_link_libraries(euclid PRIVATE ${SDL2_LIBRARIES})
endif ()

############
## Spdlog ##
############
integrate_cmake_dependency(
        NAME spdlog
        GIT_URL https://github.com/gabime/spdlog.git
        GIT_TAG v1.11.0
)
target_link_libraries(euclid PRIVATE spdlog::spdlog)


#########
## FFT ##
#########
SET(KISSFFT_TOOLS OFF)
SET(KISSFFT_TEST OFF)
integrate_cmake_dependency(
        NAME fft
        GIT_URL git@github.com:mborgerding/kissfft.git
        GIT_TAG 131.1.0
)
target_link_libraries(euclid PRIVATE kissfft)
