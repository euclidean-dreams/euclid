#pragma once

#include "foundation.h"
#include <complex>

namespace PROJECT_NAMESPACE {

class HarmonicTransform : public NonCopyable {
private:
    vec<vec<double>> harmonic_signal_snapshots;
    vec<vec<double>> low_phi_correlation_signals;
    double squish_factor = 2.0;
    int max_partials = 5;
    int consideration_offset = 3 * scast<int>(std::floor(squish_factor));
    int initial_phi = 3;
    int max_phi_divisor = 2;
    double harmonic_signal_magnitude_multiplier = 100;
    double harmonic_signal_peak_decay = 0.1;
    sp<Signal<float>> previous_signal;

    void initializeHarmonicSignalSnapshots() {
        harmonic_signal_snapshots.reserve(max_partials);
        for (int n = 1; n <= max_partials; n++) {
            harmonic_signal_snapshots.emplace_back();
            auto &result = harmonic_signal_snapshots[n - 1];
            result.reserve(2 * consideration_offset + 1);
            for (int x = 0; x < 2 * consideration_offset + 1; x++) {
                auto component = scast<double>(x - consideration_offset) / squish_factor;
                auto value = -harmonic_signal_magnitude_multiplier * component / std::pow(n, harmonic_signal_peak_decay)
                             * std::exp(-std::pow(component, 2));
                result.push_back(value);
            }
        }
    }

    void initializeLowPhiHarmonicSignals() {
        low_phi_correlation_signals.reserve(2 * consideration_offset);
        for (int i = 0; i < initial_phi; i++) {
            low_phi_correlation_signals.emplace_back();
        }
        for (int phi = initial_phi; phi <= 2 * consideration_offset; phi++) {
            low_phi_correlation_signals.emplace_back();
            auto &signal = low_phi_correlation_signals[phi];
            auto max_index = phi * max_partials + consideration_offset;
            signal.reserve(max_index + 1);
            for (int x = 0; x <= max_index; x++) {
                auto result = 0.0;
                for (int n = 1; n <= max_partials; n++) {
                    auto component = scast<double>(x - n * phi) / squish_factor;
                    result += -component / std::pow(n, harmonic_signal_peak_decay) * std::exp(-std::pow(component, 2));
                }
                signal.push_back(result * harmonic_signal_magnitude_multiplier);
            }
        }
    }

public:
    HarmonicTransform() :
            harmonic_signal_snapshots{},
            previous_signal{nullptr} {
        initializeHarmonicSignalSnapshots();
        initializeLowPhiHarmonicSignals();
    }

    up<Signal<float>> transform(sp<Signal<float>> input_signal) {
        if (previous_signal == nullptr) {
            // waiting for enough input signals to operate on...
            previous_signal = mv(input_signal);
            return nullptr;
        }

        // calculate signal derivative
        vec<double> signal_derivative;
        signal_derivative.reserve(input_signal->size());
        for (int index = 0; index < input_signal->size(); index++) {
            if (index == 0) {
                signal_derivative.push_back(0);
            } else {
                auto vertical_derivative = input_signal->get_sample(index) - input_signal->get_sample(index - 1);
                auto sample = vertical_derivative + (input_signal->get_sample(index)
                                                     - previous_signal->get_sample(index));
                signal_derivative.push_back(vertical_derivative);
            }
        }

        // create output packet
        auto max_phi = input_signal->size() / max_phi_divisor;
        auto output_signal = mkup<Signal<float>>();

        // populate packet with low phi skipped values
        for (int phi = 0; phi < initial_phi; phi++) {
            output_signal->push_back(0);
        }

        // calculate harmonic transform
        for (int phi = initial_phi; phi < max_phi; phi++) {
            float harmonic_integral = 0.0;
            if (phi <= 2 * consideration_offset) {
                // consideration windows overlap, cannot optimize
                auto &correlation_signal = low_phi_correlation_signals[phi];
                auto last_index = phi * max_partials + consideration_offset;
                for (int index = 0; index <= last_index; index++) {
                    harmonic_integral += correlation_signal[index] * signal_derivative[index];
                }
            } else {
                // no overlap in consideration windows, optimize
                auto last_allowed_peak_index = scast<double>(signal_derivative.size()) - consideration_offset - 1;
                for (int peak_index = phi; peak_index / phi <= max_partials && peak_index <= last_allowed_peak_index;
                     peak_index += phi) {
                    int first_index = peak_index - consideration_offset;
                    int last_index = peak_index + consideration_offset;
                    auto &correlation_signal = harmonic_signal_snapshots[peak_index / phi - 1];
                    for (int index = first_index; index <= last_index; index++) {
                        harmonic_integral += signal_derivative[index] * correlation_signal[index - first_index];
                    }
                }
            }
            auto harmonic_output = harmonic_integral / scast<float>(signal_derivative.size());
            output_signal->push_back(harmonic_output);
        }
        previous_signal = mv(input_signal);
        return output_signal;
    }
};

}
