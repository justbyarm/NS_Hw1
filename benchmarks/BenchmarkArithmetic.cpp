#include <iostream>
#include <iomanip>
#include <vector>
#include <cstddef>
#include <cstdint>

#include "MsPtr.h"
#include "BenchmarkUtils.h"
#include "BenchmarkSuite.h"

namespace {

    constexpr std::size_t ARRAY_SIZE = 1024;

// 1. ИНКРЕМЕНТ И ДЕКРЕМЕНТ ОБЫЧНОГО УКАЗАТЕЛЯ

    void benchmarkRawIncrement(std::size_t count) {
        std::vector<int> data(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i + 1);
        }

        int* ptr = data.data();
        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            ++ptr;
            sum += static_cast<std::uint64_t>(*ptr);
            --ptr;

            BenchmarkUtils::preventOptimization(ptr);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 2. ИНКРЕМЕНТ И ДЕКРЕМЕНТ MSPTR

    void benchmarkMsPtrIncrement(std::size_t count) {
        std::vector<int> data(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i + 1);
        }

        MsPtr<int> ptr(
                data.data(),
                data.data(),
                data.data() + data.size()
        );

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            ++ptr;
            sum += static_cast<std::uint64_t>(*ptr);
            --ptr;

            BenchmarkUtils::preventOptimization(ptr);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 3. СЛОЖЕНИЕ ОБЫЧНОГО УКАЗАТЕЛЯ

    void benchmarkRawAddition(std::size_t count) {
        std::vector<int> data(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i + 1);
        }

        int* ptr = data.data();
        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::ptrdiff_t offset =
                    static_cast<std::ptrdiff_t>(i % ARRAY_SIZE);

            int* current = ptr + offset;

            sum += static_cast<std::uint64_t>(*current);

            BenchmarkUtils::preventOptimization(current);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 4. СЛОЖЕНИЕ MSPTR

    void benchmarkMsPtrAddition(std::size_t count) {
        std::vector<int> data(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i + 1);
        }

        MsPtr<int> ptr(
                data.data(),
                data.data(),
                data.data() + data.size()
        );

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::ptrdiff_t offset =
                    static_cast<std::ptrdiff_t>(i % ARRAY_SIZE);

            MsPtr<int> current = ptr + offset;

            sum += static_cast<std::uint64_t>(*current);

            BenchmarkUtils::preventOptimization(current);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 5. ВЫЧИТАНИЕ ОБЫЧНОГО УКАЗАТЕЛЯ

    void benchmarkRawSubtraction(std::size_t count) {
        std::vector<int> data(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i + 1);
        }

        int* ptr = data.data() + ARRAY_SIZE - 1;
        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::ptrdiff_t offset =
                    static_cast<std::ptrdiff_t>(i % ARRAY_SIZE);

            int* current = ptr - offset;

            sum += static_cast<std::uint64_t>(*current);

            BenchmarkUtils::preventOptimization(current);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 6. ВЫЧИТАНИЕ MSPTR

    void benchmarkMsPtrSubtraction(std::size_t count) {
        std::vector<int> data(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            data[i] = static_cast<int>(i + 1);
        }

        MsPtr<int> ptr(
                data.data() + ARRAY_SIZE - 1,
                data.data(),
                data.data() + data.size()
        );

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::ptrdiff_t offset =
                    static_cast<std::ptrdiff_t>(i % ARRAY_SIZE);

            MsPtr<int> current = ptr - offset;

            sum += static_cast<std::uint64_t>(*current);

            BenchmarkUtils::preventOptimization(current);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

} // namespace

// 7. ЗАПУСК БЕНЧМАРКА

void runArithmeticBenchmark() {
    const std::vector<std::size_t> sizes = {
            1000,
            10000,
            100000,
            1000000,
            10000000
    };

    std::cout << "\n========== ARITHMETIC BENCHMARK ==========\n\n";

    std::cout << std::fixed << std::setprecision(3);

    auto printHeader = []() {
        std::cout
                << std::setw(12) << "Operations"
                << std::setw(18) << "Raw (ms)"
                << std::setw(18) << "MsPtr (ms)"
                << '\n';
    };

    // Инкремент и декремент
    std::cout << "--- Increment / Decrement ---\n";
    printHeader();

    for (std::size_t count : sizes) {
        double rawTime = BenchmarkUtils::measure([&]() {
            benchmarkRawIncrement(count);
        });

        double msPtrTime = BenchmarkUtils::measure([&]() {
            benchmarkMsPtrIncrement(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawTime
                << std::setw(18) << msPtrTime
                << '\n';
    }

    // Сложение
    std::cout << "\n--- Addition (+) ---\n";
    printHeader();

    for (std::size_t count : sizes) {
        double rawTime = BenchmarkUtils::measure([&]() {
            benchmarkRawAddition(count);
        });

        double msPtrTime = BenchmarkUtils::measure([&]() {
            benchmarkMsPtrAddition(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawTime
                << std::setw(18) << msPtrTime
                << '\n';
    }

    // Вычитание
    std::cout << "\n--- Subtraction (-) ---\n";
    printHeader();

    for (std::size_t count : sizes) {
        double rawTime = BenchmarkUtils::measure([&]() {
            benchmarkRawSubtraction(count);
        });

        double msPtrTime = BenchmarkUtils::measure([&]() {
            benchmarkMsPtrSubtraction(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawTime
                << std::setw(18) << msPtrTime
                << '\n';
    }
}