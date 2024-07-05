#pragma once

#include <portaudio.h>
#include <pa_linux_alsa.h>
#include "paradigm.h"
#include "perception/ring_buffer.h"

namespace euclid {

auto SAMPLE_RATE = 44100;

class PortaudioInput : public Name {
private:
    PaStream *input_stream;
    PaStreamParameters input_parameters;

    static void throw_on_error(PaError return_code) {
        if (return_code != paNoError) {
            spdlog::error(Pa_GetErrorText(return_code));
            throw std::runtime_error(Pa_GetErrorText(return_code));
        }
    }

public:
    RingBuffer ring_buffer;

    PortaudioInput(int frame_size) :
            ring_buffer{frame_size},
            input_stream{},
            input_parameters{} {
        throw_on_error(Pa_Initialize());
        int device_count = Pa_GetDeviceCount();
        spdlog::info("-- enumerating {} audio device(s) --", device_count);
        for (int device_index = 0; device_index < device_count; device_index++) {
            auto device_info = Pa_GetDeviceInfo(device_index);
            auto device_name = std::string{device_info->name};
            spdlog::info("    name: {}, index: {}", device_name, device_index);
        }
        auto input_device = Pa_GetDefaultInputDevice();
        spdlog::info("using default audio device: {}", Pa_GetDeviceInfo(input_device)->name);
        input_parameters.device = input_device;
        input_parameters.channelCount = 1;
        input_parameters.sampleFormat = paFloat32;
        input_parameters.suggestedLatency = Pa_GetDeviceInfo(input_device)->defaultLowInputLatency;
        input_parameters.hostApiSpecificStreamInfo = nullptr;
        throw_on_error(Pa_OpenStream(
                &input_stream,
                &input_parameters,
                nullptr,
                SAMPLE_RATE,
                paFramesPerBufferUnspecified,
                paNoFlag,
                stream_callback,
                &ring_buffer
        ));
        PaAlsa_EnableRealtimeScheduling(input_stream, true);
        throw_on_error(Pa_StartStream(input_stream));
    }

    ~PortaudioInput() {
        throw_on_error(Pa_Terminate());
    }

    static int stream_callback(const void *input, void *output, unsigned long frameCount,
                               const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
                               void *userData) {
        auto samples = static_cast<const float *>(input);
        auto ringBuffer = static_cast<RingBuffer *>(userData);
        ringBuffer->addSamples(samples, frameCount);
        return paContinue;
    }

};

}
