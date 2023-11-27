##########
## Init ##
##########
cmake_minimum_required(VERSION 3.23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
project(euclid)
include(lib/cmake-utils/dependency-integrator.cmake)


#############
## options ##
#############
option(debug "debug" OFF)

# pick one!
option(wasm "build for the web" OFF)
option(local "build for your screen" OFF)
option(sjofn "build for sjofn" OFF)


############
## source ##
############
set(
        SOURCE_FILES
        src/main.cpp
        src/euclid.h
        src/paradigm.h

        src/acoustics/audio_input.h
        src/acoustics/equalizer.h
        src/acoustics/ring_buffer.h
        src/acoustics/stft.h
)
if (wasm)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/opus.h
    )
endif ()
if (local)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/opus.h
    )
endif ()
if (sjofn)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/wavelet.h src/optics/wavelet.cpp
    )
endif ()
add_executable(euclid ${SOURCE_FILES})
target_include_directories(euclid PRIVATE src)


###########
### WASM ##
###########
if (wasm)
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
    add_compile_definitions(WASM EMSCRIPTEN OPUS)
endif ()


############
### local ##
############
if (local)
    message("(~) building for your screen")
    add_compile_definitions(LOCAL OPUS)
    find_package(SDL2 REQUIRED)
    include_directories(euclid ${SDL2_INCLUDE_DIRS})
    target_link_libraries(euclid PRIVATE ${SDL2_LIBRARIES})
endif ()


############
### sjofn ##
############
if (sjofn)
    message("(~) building for sjofn")
    add_compile_definitions(SJOFN WAVELET)
    find_package(SDL2 REQUIRED)
    include_directories(euclid ${SDL2_INCLUDE_DIRS})
    target_link_libraries(euclid PRIVATE ${SDL2_LIBRARIES})
    integrate_cmake_dependency(
            NAME pigpio
            GIT_URL git@github.com:joan2937/pigpio.git
            GIT_TAG c33738a320a3e28824af7807edafda440952c05d
    )
    target_include_directories(euclid PRIVATE ${CMAKE_BINARY_DIR}/pigpio-src)
    target_link_libraries(euclid PRIVATE pigpio)
endif ()


###############
## cosmology ##
###############
add_subdirectory(lib/cosmology)
target_link_libraries(euclid PRIVATE cosmology)


############
## Spdlog ##
############
if (wasm)
    integrate_cmake_dependency(
            NAME spdlog
            GIT_URL https://github.com/gabime/spdlog.git
            GIT_TAG v1.11.0
    )
    target_link_libraries(euclid PRIVATE spdlog::spdlog)
else ()
    find_package(spdlog REQUIRED)
    target_link_libraries(euclid PRIVATE spdlog::spdlog_header_only)
endif ()


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
