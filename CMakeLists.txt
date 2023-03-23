##########
## Init ##
##########
cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(euclid)
include(lib/cmake-utils/dependency-integrator.cmake)

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

        src/shaping/audio_input.h
        src/shaping/ring_buffer.h
        #        src/shaping/stft.h
)
add_executable(euclid ${SOURCE_FILES})
target_include_directories(euclid PRIVATE src)

###########
### WASM ##
###########
if (EMSCRIPTEN)
    message("building for wasm")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_SDL=2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sUSE_PTHREADS=1")
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
