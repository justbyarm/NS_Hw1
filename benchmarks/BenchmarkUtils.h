#pragma once

#include <chrono>
#include <cstddef>
#include <utility>
#include <vector>
#include <algorithm>

class BenchmarkUtils {
public:
    template <typename Function>
    static double measure(Function&& function, std::size_t repetitions = 7) {
        using Clock = std::chrono::steady_clock;

        std::vector<double> measurements;
        measurements.reserve(repetitions);

        for (std::size_t i = 0; i < repetitions; ++i) {
            auto start = Clock::now();

            function();

            auto end = Clock::now();

            double duration = std::chrono::duration<double, std::milli>(
                    end - start
            ).count();

            measurements.push_back(duration);
        }

        std::sort(measurements.begin(), measurements.end());

        return measurements[measurements.size() / 2];
    }

    template <typename T>
    static void preventOptimization(const T& value) {
#if defined(__clang__) || defined(__GNUC__)
        asm volatile("" : : "g"(&value) : "memory");
#else
        volatile const T* ptr = &value;
        (void)ptr;
#endif
    }
};