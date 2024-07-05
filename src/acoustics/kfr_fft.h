#pragma once

#include "paradigm.h"
#include <kfr/dft.hpp>
#include <kfr/dsp/window.hpp>

namespace euclid {
#ifdef QUETZAL
#define WINDOW_SIZE 1
#else
#define WINDOW_SIZE 8
#endif
#define FFT_SIZE scast<size_t>(WINDOW_SIZE * FRAME_SIZE)
#define STFT_SIZE (FFT_SIZE / 2 + 1)


class FourierTransform : public Name {
private:
    kfr::dft_plan_real<double> fft_plan;
    kfr::univector<double, FFT_SIZE> fft_input;
    kfr::univector<kfr::complex<double>, FFT_SIZE> fft_output;
    uptr<kfr::univector < kfr::u8>> temp_buffer;
    kfr::univector<kfr::fbase, FFT_SIZE> window;
    lst<uptr<Signal<float>>> input_signals{};


public:
    FourierTransform() : fft_plan{FFT_SIZE}, temp_buffer{} {
        temp_buffer = mkuptr<kfr::univector<kfr::u8>>(fft_plan.temp_size);
        window = kfr::window_hamming(FFT_SIZE);
    }

    ~FourierTransform() {
        kfr::dft_cache::instance().clear();
    }

    uptr<Signal<cmplx>> stft(uptr<Signal<float>> signal) {
        if (input_signals.size() < WINDOW_SIZE) {
            // waiting for enough input signals to operate on...
            input_signals.push_back(mv(signal));
            return nullptr;
        } else {
            // we have enough input signals
            input_signals.pop_front();
            input_signals.push_back(mv(signal));
        }

        // perform windowing
        auto fft_input_index = 0;
        for (auto &input_signal: input_signals) {
            for (auto sample: *input_signal) {
                fft_input[fft_input_index] = sample * window[fft_input_index];
                fft_input_index++;
            }
        }

        fft_plan.execute(fft_output, fft_input, *temp_buffer);
        auto result = mkuptr<Signal<cmplx>>();
        for (int i = 0; i < STFT_SIZE; i++) {
            result->push_back({fft_output[i].real(), fft_output[i].imag()});
        }
        return result;
    }
};

}
