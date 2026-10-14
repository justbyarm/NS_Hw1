#include <iostream>
#include <iomanip>
#include <vector>
#include <cstddef>

#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "BenchmarkUtils.h"
#include "BenchmarkSuite.h"

// 1. ОБЫЧНЫЕ УКАЗАТЕЛИ

void benchmarkRawCreation(std::size_t count) {
    std::vector<int*> pointers;
    pointers.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        pointers.push_back(new int(static_cast<int>(i)));
    }

    std::size_t sum = 0;

    for (int* ptr : pointers) {
        sum += static_cast<std::size_t>(*ptr);
    }

    BenchmarkUtils::preventOptimization(sum);

    for (int* ptr : pointers) {
        delete ptr;
    }
}

// 2. УНИКАЛЬНЫЕ УКАЗАТЕЛИ

void benchmarkUniqueCreation(std::size_t count) {
    std::vector<UnqPtr<int>> pointers;
    pointers.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        pointers.emplace_back(new int(static_cast<int>(i)));
    }

    std::size_t sum = 0;

    for (const auto& ptr : pointers) {
        sum += static_cast<std::size_t>(*ptr);
    }

    BenchmarkUtils::preventOptimization(sum);

    // Объекты уничтожаются автоматически
}

// 3. РАЗДЕЛЯЕМЫЕ УКАЗАТЕЛИ

void benchmarkSharedCreation(std::size_t count) {
    std::vector<ShrdPtr<int>> pointers;
    pointers.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        pointers.emplace_back(
                UnqPtr<int>(new int(static_cast<int>(i)))
        );
    }

    std::size_t sum = 0;

    for (const auto& ptr : pointers) {
        sum += static_cast<std::size_t>(*ptr);
    }

    BenchmarkUtils::preventOptimization(sum);

    // Объекты уничтожаются автоматически
}

// 4. ЗАПУСК БЕНЧМАРКА

void runCreationBenchmark() {
    const std::vector<std::size_t> sizes = {
            10,
            100,
            1000,
            10000,
            100000,
            1000000
    };

    std::cout << "\n========== CREATION BENCHMARK ==========\n\n";

    std::cout << std::fixed << std::setprecision(3);

    std::cout
            << std::setw(12) << "Objects"
            << std::setw(18) << "Raw (ms)"
            << std::setw(18) << "UnqPtr (ms)"
            << std::setw(18) << "ShrdPtr (ms)"
            << '\n';

    for (std::size_t count : sizes) {
        double rawTime = BenchmarkUtils::measure([&]() {
            benchmarkRawCreation(count);
        });

        double uniqueTime = BenchmarkUtils::measure([&]() {
            benchmarkUniqueCreation(count);
        });

        double sharedTime = BenchmarkUtils::measure([&]() {
            benchmarkSharedCreation(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawTime
                << std::setw(18) << uniqueTime
                << std::setw(18) << sharedTime
                << '\n';
    }
}