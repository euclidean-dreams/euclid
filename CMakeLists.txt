##########
## Init ##
##########
cmake_minimum_required(VERSION 3.18)
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
option(mac "build for a mac" OFF)
option(quetzal "build for a quetzal" OFF)
option(kfr "accelerate fft calculations with kfr" OFF)

message("mac: ${mac}, wasm: ${wasm}, quetzal: ${quetzal}, kfr: ${kfr}")

# render width and height
if (wasm)
    # set automatically by the browser
    add_compile_definitions(COMPILED_RENDER_WIDTH=-1)
    add_compile_definitions(COMPILED_RENDER_HEIGHT=-1)
elseif (mac)
    add_compile_definitions(COMPILED_RENDER_WIDTH=1470)
    add_compile_definitions(COMPILED_RENDER_HEIGHT=956)
elseif (quetzal)
    set(RENDER_WIDTH 64 CACHE STRING "width of the output frame")
    set(RENDER_HEIGHT 64 CACHE STRING "height of the output frame")
    add_compile_definitions(COMPILED_RENDER_WIDTH=${RENDER_WIDTH})
    add_compile_definitions(COMPILED_RENDER_HEIGHT=${RENDER_HEIGHT})
endif ()

############
## source ##
############
set(
        SOURCE_FILES
        src/main.cpp
        src/euclid.h
        src/paradigm.h
)
if (wasm)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/opus.cpp src/optics/opus.h
            src/acoustics/sdl_input.h
    )
endif ()
if (mac)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/opus.cpp src/optics/opus.h
            src/interaction/fascia.cpp src/interaction/fascia.h
            src/acoustics/sdl_input.h
            src/optics/lodepng.cpp src/optics/lodepng.h
            src/optics/save_png.h
    )
endif ()
if (quetzal)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/quetzal.cpp src/optics/quetzal.h
            src/acoustics/portaudio_input.h
    )
endif ()
if (kfr)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/acoustics/kfr_fft.h
    )
else ()
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/acoustics/kiss_fft.h
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --embed-file assets")
    if (debug)
        message("(~) [x] emcc debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -sASSERTIONS")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --memoryprofiler")
    else ()
        message("(~) [ ] emcc debug")
    endif ()
    add_compile_definitions(WASM EMSCRIPTEN OPUS SDL_AUDIO)
endif ()


##########
### mac ##
##########
if (mac)
    message("(~) building for your mac")
    add_compile_definitions(MAC OPUS SDL_AUDIO)
    set(CMAKE_PREFIX_PATH /opt/homebrew/lib)
    find_package(SDL2 REQUIRED)
    include_directories(euclid ${SDL2_INCLUDE_DIRS})
    target_link_libraries(euclid PRIVATE ${SDL2_LIBRARIES})
    include_directories(euclid /opt/homebrew/Cellar/sdl2_ttf/2.22.0/include)
    target_link_libraries(euclid PUBLIC /opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/libSDL2_ttf.dylib)
    include_directories(euclid /opt/homebrew/Cellar/sdl2_image/2.8.2_1/include)
    target_link_libraries(euclid PUBLIC /opt/homebrew/Cellar/sdl2_image/2.8.2_1/lib/libSDL2_image.dylib)
    target_link_libraries(euclid PUBLIC /opt/homebrew/Cellar/fmt/10.2.1_1/lib/libfmt.dylib)
endif ()


##############
### quetzal ##
##############
if (quetzal)
    message("(~) building for quetzal")
    add_compile_definitions(QUETZAL PORT_AUDIO)
    target_link_libraries(euclid PRIVATE pigpio)
    target_link_libraries(euclid PRIVATE portaudio)
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
if (kfr)
    find_package(KFR CONFIG REQUIRED)
    target_link_libraries(euclid PRIVATE kfr kfr_dft)
    add_compile_definitions(KFR_FFT)
elseif (wasm)
    SET(KISSFFT_TOOLS OFF)
    SET(KISSFFT_TEST OFF)
    integrate_cmake_dependency(
            NAME fft
            GIT_URL git@github.com:mborgerding/kissfft.git
            GIT_TAG 131.1.0
    )
    target_link_libraries(euclid PRIVATE kissfft)
    add_compile_definitions(KISS_FFT)
else ()
    find_package(kissfft CONFIG REQUIRED)
    target_link_libraries(euclid PRIVATE kissfft-float)
    add_compile_definitions(KISS_FFT)
endif ()
