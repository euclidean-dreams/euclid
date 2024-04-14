#pragma once

#include "axioms.h"
#include "kiss_fftr.h"

namespace euclid {

enum class WindowFunction {
    hamming = 0,
    hann = 1
};

class FourierTransform : public Name {
private:
    WindowFunction window_function;
    int hop_size = 1;
    int window_size = 32;
    int fft_size;
    int stft_size;
    vec<float> fft_input{};
    vec<kiss_fft_cpx> fft_output{};
    kiss_fftr_cfg fft_plan;
    lst<uptr<Signal<float>>> input_signals{};

    float hamming_window(int sample_number) const {
        return static_cast<float>(0.53836 - 0.46164 * std::cos((2 * M_PI * sample_number) / (fft_size - 1)));
    }

    float hann_window(int sample_number) const {
        return static_cast<float>(0.5 * (1 - std::cos((2 * M_PI * sample_number) / (fft_size - 1))));
    }

public:
    FourierTransform(int frame_size, WindowFunction window_function)
            : window_function{window_function},
              fft_size{window_size * frame_size},
              stft_size{fft_size / 2 + 1},
              fft_plan(kiss_fftr_alloc(fft_size, 0, nullptr, nullptr)) {
        fft_input.resize(fft_size);
        fft_output.resize(fft_size);
    }

    ~FourierTransform() {
        kiss_fft_free(fft_plan);
    }

    uptr<Signal<cmplx>> stft(uptr<Signal<float>> signal) {
        if (input_signals.size() < window_size / hop_size) {
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
        for (int i = 0; i < stft_size; i++) {
            result->push_back({fft_output[i].r, fft_output[i].i});
        }
        return result;
    }

};

}
