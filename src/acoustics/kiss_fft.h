#pragma once

#include "axioms.h"
#include "kissfft/kiss_fftr.h"

namespace euclid {

#define WINDOW_SIZE 8
#define FFT_SIZE scast<size_t>(WINDOW_SIZE * FRAME_SIZE)
#define STFT_SIZE (FFT_SIZE / 2 + 1)

enum class WindowFunction {
    hamming = 0,
    hann = 1
};

class FourierTransform : public Name {
private:
    WindowFunction window_function;
    vect<float> fft_input{};
    vect<kiss_fft_cpx> fft_output{};
    kiss_fftr_cfg fft_plan;
    lst<uptr<Signal<float>>> input_signals{};

    float hamming_window(int sample_number) const {
        return static_cast<float>(0.53836 - 0.46164 * std::cos((2 * M_PI * sample_number) / (FFT_SIZE - 1)));
    }

    float hann_window(int sample_number) const {
        return static_cast<float>(0.5 * (1 - std::cos((2 * M_PI * sample_number) / (FFT_SIZE - 1))));
    }

public:
    FourierTransform()
            : window_function{WindowFunction::hamming},
              fft_plan(kiss_fftr_alloc(FFT_SIZE, 0, nullptr, nullptr)) {
        fft_input.resize(FFT_SIZE);
        fft_output.resize(FFT_SIZE);
    }

    ~FourierTransform() {
        kiss_fft_free(fft_plan);
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
                if (window_function == WindowFunction::hann) {
                    fft_input[fft_input_index] = sample * hann_window(fft_input_index);
                } else if (window_function == WindowFunction::hamming) {
                    fft_input[fft_input_index] = sample * hamming_window(fft_input_index);
                } else {
                    throw std::logic_error{"invalid window function selected"};
                }
                fft_input_index++;
            }
        }

        // perform the actual fft
        kiss_fftr(fft_plan, fft_input.data(), fft_output.data());
        auto result = mkuptr<Signal<cmplx>>();
        for (int i = 0; i < STFT_SIZE; i++) {
            result->push_back({fft_output[i].r, fft_output[i].i});
        }
        return result;
    }

};

}
