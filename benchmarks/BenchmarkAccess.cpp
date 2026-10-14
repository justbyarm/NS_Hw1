#include <iostream>
#include <iomanip>
#include <vector>
#include <cstddef>

#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "BenchmarkUtils.h"
#include "BenchmarkSuite.h"

// Тестовый объект
struct TestObject {
    int value;

    explicit TestObject(int v) : value(v) {}
};

// 1. РАЗЫМЕНОВАНИЕ ОБЫЧНОГО УКАЗАТЕЛЯ

void benchmarkRawDereference(std::size_t count) {
    int value = 42;
    int* ptr = &value;

    std::size_t sum = 0;

    for (std::size_t i = 0; i < count; ++i) {
        sum += static_cast<std::size_t>(*ptr);
        BenchmarkUtils::preventOptimization(ptr);
    }

    BenchmarkUtils::preventOptimization(sum);
}

// 2. РАЗЫМЕНОВАНИЕ UNQPTR

void benchmarkUniqueDereference(std::size_t count) {
    UnqPtr<int> ptr(new int(42));

    std::size_t sum = 0;

    for (std::size_t i = 0; i < count; ++i) {
        sum += static_cast<std::size_t>(*ptr);
        BenchmarkUtils::preventOptimization(ptr);
    }

    BenchmarkUtils::preventOptimization(sum);
}

// 3. РАЗЫМЕНОВАНИЕ SHRDPTR

void benchmarkSharedDereference(std::size_t count) {
    ShrdPtr<int> ptr(
            UnqPtr<int>(new int(42))
    );

    std::size_t sum = 0;

    for (std::size_t i = 0; i < count; ++i) {
        sum += static_cast<std::size_t>(*ptr);
        BenchmarkUtils::preventOptimization(ptr);
    }

    BenchmarkUtils::preventOptimization(sum);
}

// 4. ДОСТУП К ПОЛЮ ЧЕРЕЗ ОБЫЧНЫЙ УКАЗАТЕЛЬ

void benchmarkRawArrow(std::size_t count) {
    TestObject object(100);
    TestObject* ptr = &object;

    std::size_t sum = 0;

    for (std::size_t i = 0; i < count; ++i) {
        sum += static_cast<std::size_t>(ptr->value);
        BenchmarkUtils::preventOptimization(ptr);
    }

    BenchmarkUtils::preventOptimization(sum);
}

// 5. ДОСТУП К ПОЛЮ ЧЕРЕЗ UNQPTR

void benchmarkUniqueArrow(std::size_t count) {
    UnqPtr<TestObject> ptr(new TestObject(100));

    std::size_t sum = 0;

    for (std::size_t i = 0; i < count; ++i) {
        sum += static_cast<std::size_t>(ptr->value);
        BenchmarkUtils::preventOptimization(ptr);
    }

    BenchmarkUtils::preventOptimization(sum);
}

// 6. ДОСТУП К ПОЛЮ ЧЕРЕЗ SHRDPTR

void benchmarkSharedArrow(std::size_t count) {
    ShrdPtr<TestObject> ptr(
            UnqPtr<TestObject>(new TestObject(100))
    );

    std::size_t sum = 0;

    for (std::size_t i = 0; i < count; ++i) {
        sum += static_cast<std::size_t>(ptr->value);
        BenchmarkUtils::preventOptimization(ptr);
    }

    BenchmarkUtils::preventOptimization(sum);
}

// 7. ЗАПУСК БЕНЧМАРКА

void runAccessBenchmark() {
    const std::vector<std::size_t> sizes = {
            1000,
            10000,
            100000,
            1000000,
            10000000
    };

    std::cout << "\n========== ACCESS BENCHMARK ==========\n\n";

    std::cout << std::fixed << std::setprecision(3);

    // Разыменование
    std::cout << "--- Dereference (*) ---\n";

    std::cout
            << std::setw(12) << "Operations"
            << std::setw(18) << "Raw (ms)"
            << std::setw(18) << "UnqPtr (ms)"
            << std::setw(18) << "ShrdPtr (ms)"
            << '\n';

    for (std::size_t count : sizes) {
        double rawTime = BenchmarkUtils::measure([&]() {
            benchmarkRawDereference(count);
        });

        double uniqueTime = BenchmarkUtils::measure([&]() {
            benchmarkUniqueDereference(count);
        });

        double sharedTime = BenchmarkUtils::measure([&]() {
            benchmarkSharedDereference(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawTime
                << std::setw(18) << uniqueTime
                << std::setw(18) << sharedTime
                << '\n';
    }

    // Доступ к полям
    std::cout << "\n--- Member Access (->) ---\n";

    std::cout
            << std::setw(12) << "Operations"
            << std::setw(18) << "Raw (ms)"
            << std::setw(18) << "UnqPtr (ms)"
            << std::setw(18) << "ShrdPtr (ms)"
            << '\n';

    for (std::size_t count : sizes) {
        double rawTime = BenchmarkUtils::measure([&]() {
            benchmarkRawArrow(count);
        });

        double uniqueTime = BenchmarkUtils::measure([&]() {
            benchmarkUniqueArrow(count);
        });

        double sharedTime = BenchmarkUtils::measure([&]() {
            benchmarkSharedArrow(count);
        });

        std::cout
                << std::setw(12) << count
                << std::setw(18) << rawTime
                << std::setw(18) << uniqueTime
                << std::setw(18) << sharedTime
                << '\n';
    }
}