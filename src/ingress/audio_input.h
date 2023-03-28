#pragma once

#include "foundation.h"
#include "ring_buffer.h"

namespace PROJECT_NAMESPACE {

class SDLAudioInput : NonCopyable {
private:
    SDL_AudioSpec desired_spec{};
    SDL_AudioSpec actual_spec{};

public:
    RingBuffer ring_buffer;

    SDLAudioInput(int frame_size) :
            ring_buffer{frame_size} {
        SDL_zero(desired_spec);
        desired_spec.freq = 44100;
        desired_spec.format = AUDIO_F32SYS;
        desired_spec.channels = 1;
        if (frame_size >= 256) {
            desired_spec.samples = frame_size;
        } else {
            desired_spec.samples = 256;
        }
        desired_spec.callback = recording_callback;
        desired_spec.userdata = &ring_buffer;
        auto audio_device = SDL_OpenAudioDevice(nullptr, 1, &desired_spec, &actual_spec, 0);
        SDL_PauseAudioDevice(audio_device, SDL_FALSE);
    }

    static void recording_callback(void *user_data, Uint8 *raw_samples, int raw_length) {
        auto ringBuffer = static_cast<RingBuffer *>(user_data);
        auto samples = (float *) raw_samples;
        auto length = raw_length / 4;
        ringBuffer->addSamples(samples, length);
    }
};

}
