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
option(quetzal_keyhole "build for a quetzal keyhole" OFF)
option(quetzal_dmx "build for a quetzal dmx rig" OFF)
option(kfr "accelerate fft calculations with kfr" OFF)

message("mac: ${mac}, wasm: ${wasm}, quetzal_keyhole: ${quetzal_keyhole}, quetzal_dmx: ${quetzal_dmx}, kfr: ${kfr}")

# render width and height
if (wasm)
    # set automatically by the browser
    add_compile_definitions(COMPILED_RENDER_WIDTH=-1)
    add_compile_definitions(COMPILED_RENDER_HEIGHT=-1)
elseif (mac)
    set(RENDER_WIDTH 64 CACHE STRING "width of the output frame")
    set(RENDER_HEIGHT 64 CACHE STRING "height of the output frame")
    add_compile_definitions(COMPILED_RENDER_WIDTH=${RENDER_WIDTH})
    add_compile_definitions(COMPILED_RENDER_HEIGHT=${RENDER_HEIGHT})
elseif (quetzal_dmx OR quetzal_keyhole)
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
if (quetzal_dmx OR quetzal_keyhole)
    set(
            SOURCE_FILES
            ${SOURCE_FILES}
            src/optics/quetzal.cpp src/optics/quetzal.h
            src/optics/dmx/rig.h
            src/acoustics/portaudio_input.h
            src/optics/dmx/vivid.h src/optics/dmx/vivid.cpp
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
    include_directories(euclid /opt/homebrew/include)
    target_link_libraries(euclid PUBLIC /opt/homebrew/lib/libSDL2_ttf.dylib)
    target_link_libraries(euclid PUBLIC /opt/homebrew/lib/libSDL2_image.dylib)
    target_link_libraries(euclid PUBLIC /opt/homebrew/lib/libfmt.dylib)
endif ()


##############
### quetzal ##
##############
if (quetzal_dmx OR quetzal_keyhole)
    message("(~) building for quetzal")

    add_compile_definitions(QUETZAL PORT_AUDIO)
    if (quetzal_dmx)
        add_compile_definitions(QUETZAL_DMX)
    elseif (quetzal_keyhole)
        add_compile_definitions(QUETZAL_KEYHOLE)
    endif ()
    target_link_libraries(euclid PRIVATE pigpio)
    target_link_libraries(euclid PRIVATE portaudio)
endif ()


###############
## cosmology ##
###############
add_subdirectory(lib/cosmology)
target_link_libraries(euclid PRIVATE cosmology)
target_link_libraries(euclid PRIVATE cosmology_selections)


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
    add_compile_definitions(KFR_FFT)
else ()
    add_compile_definitions(KISS_FFT)
endif ()
