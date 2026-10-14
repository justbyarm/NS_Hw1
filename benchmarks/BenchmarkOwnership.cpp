#include <iostream>
#include <iomanip>
#include <vector>
#include <cstddef>
#include <utility>

#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "BenchmarkUtils.h"
#include "BenchmarkSuite.h"

// 1. КОПИРОВАНИЕ ОБЫЧНОГО УКАЗАТЕЛЯ

void benchmarkRawCopy(std::size_t count) {
    int value = 42;
    int* source = &value;

    std::vector<int*> copies;
    copies.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        copies.push_back(source);
    }

    BenchmarkUtils::preventOptimization(copies.data());
}

// 2. КОПИРОВАНИЕ SHRDPTR

void benchmarkSharedCopy(std::size_t count) {
    ShrdPtr<int> source(
            UnqPtr<int>(new int(42))
    );

    std::vector<ShrdPtr<int>> copies;
    copies.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        copies.push_back(source);
    }

    BenchmarkUtils::preventOptimization(copies.data());
}

// 3. ПЕРЕМЕЩЕНИЕ UNQPTR

void benchmarkUniqueMove(std::size_t count) {
    UnqPtr<int> first(new int(42));
    UnqPtr<int> second;

    for (std::size_t i = 0; i < count; ++i) {
        second = std::move(first);
        first = std::move(second);

        BenchmarkUtils::preventOptimization(first.get());
    }
}

// 4. ПЕРЕМЕЩЕНИЕ SHRDPTR

void benchmarkSharedMove(std::size_t count) {
    ShrdPtr<int> first(
            UnqPtr<int>(new int(42))
    );

    ShrdPtr<int> second;

    for (std::size_t i = 0; i < count; ++i) {
        second = std::move(first);
        first = std::move(second);

        BenchmarkUtils::preventOptimization(first.get());
    }
}

// 5. ЗАПУСК БЕНЧМАРКА

void runOwnershipBenchmark() {
    const std::vector<std::size_t> sizes = {
            10,
            100,
            1000,
            10000,
            100000,
            1000000
    };

    std::cout << "\n========== OWNERSHIP BENCHMARK ==========\n\n";

    std::cout << std::fixed << std::setprecision(3);

    std::cout
            << std::setw(12) << "Operations"
            << std::setw(18) << "Raw Copy (ms)"
            << std::setw(18) << "Shrd Copy (ms)"
            << std::setw(18) << "Unq Move (ms)"
            << std::setw(18) << "Shrd Move (ms)"
            << '\n';

    for (std::size_t count : sizes) {
        double rawCopy = BenchmarkUtils::measure([&]() {
            benchmarkRawCopy(count);
        });

        double sharedCopy = BenchmarkUtils::measure([&]() {
            benchmarkSharedCopy(count);
        });

        double uniqueMove = BenchmarkUtils::measure([&]() {
            benchmarkUniqueMove(count);
        });

        double sharedMove = BenchmarkUtils::measure([&]() {
            benchmarkSharedMove(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawCopy
                << std::setw(18) << sharedCopy
                << std::setw(18) << uniqueMove
                << std::setw(18) << sharedMove
                << '\n';
    }
}