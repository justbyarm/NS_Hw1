#include <iostream>
#include <iomanip>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "MemorySpan.h"
#include "BenchmarkUtils.h"
#include "BenchmarkSuite.h"

namespace {

    constexpr std::size_t ARRAY_SIZE = 1024;

// 1. СОЗДАНИЕ И УНИЧТОЖЕНИЕ МАССИВА

    void benchmarkRawCreation(std::size_t count) {
        for (std::size_t i = 0; i < count; ++i) {
            std::unique_ptr<int[]> array(new int[ARRAY_SIZE]);

            array[0] = static_cast<int>(i);
            BenchmarkUtils::preventOptimization(array[0]);
        }
    }

    void benchmarkSpanCreation(std::size_t count) {
        for (std::size_t i = 0; i < count; ++i) {
            MemorySpan<int> span(static_cast<int>(ARRAY_SIZE));

            span.set(0, static_cast<int>(i));
            BenchmarkUtils::preventOptimization(*span.locate(0));
        }
    }

// 2. ЗАПИСЬ ЭЛЕМЕНТОВ

    void benchmarkRawSet(std::size_t count) {
        std::vector<int> array(ARRAY_SIZE);

        for (std::size_t i = 0; i < count; ++i) {
            std::size_t index = i % ARRAY_SIZE;

            array[index] = static_cast<int>(i);
        }

        BenchmarkUtils::preventOptimization(array.data());
    }

    void benchmarkSpanSet(std::size_t count) {
        MemorySpan<int> span(static_cast<int>(ARRAY_SIZE));

        for (std::size_t i = 0; i < count; ++i) {
            int index = static_cast<int>(i % ARRAY_SIZE);

            span.set(index, static_cast<int>(i));
        }

        BenchmarkUtils::preventOptimization(*span.locate(0));
    }

// 3. ПОЛУЧЕНИЕ УНИКАЛЬНОЙ КОПИИ

    void benchmarkRawGet(std::size_t count) {
        std::vector<int> array(ARRAY_SIZE, 42);

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::size_t index = i % ARRAY_SIZE;

            std::unique_ptr<int> copy(new int(array[index]));

            sum += static_cast<std::uint64_t>(*copy);

            BenchmarkUtils::preventOptimization(copy.get());
        }

        BenchmarkUtils::preventOptimization(sum);
    }

    void benchmarkSpanGet(std::size_t count) {
        MemorySpan<int> span(static_cast<int>(ARRAY_SIZE));

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            span.set(static_cast<int>(i), 42);
        }

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            int index = static_cast<int>(i % ARRAY_SIZE);

            auto copy = span.get(index);

            sum += static_cast<std::uint64_t>(*copy);

            BenchmarkUtils::preventOptimization(copy.get());
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 4. ПОЛУЧЕНИЕ РАЗДЕЛЯЕМОЙ КОПИИ

    void benchmarkRawCopy(std::size_t count) {
        std::vector<int> array(ARRAY_SIZE, 42);

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::size_t index = i % ARRAY_SIZE;

            ShrdPtr<int> copy(
                    UnqPtr<int>(new int(array[index]))
            );

            sum += static_cast<std::uint64_t>(*copy);

            BenchmarkUtils::preventOptimization(copy.get());
        }

        BenchmarkUtils::preventOptimization(sum);
    }

    void benchmarkSpanCopy(std::size_t count) {
        MemorySpan<int> span(static_cast<int>(ARRAY_SIZE));

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            span.set(static_cast<int>(i), 42);
        }

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            int index = static_cast<int>(i % ARRAY_SIZE);

            auto copy = span.copy(index);

            sum += static_cast<std::uint64_t>(*copy);

            BenchmarkUtils::preventOptimization(copy.get());
        }

        BenchmarkUtils::preventOptimization(sum);
    }

// 5. ДОСТУП К ЭЛЕМЕНТАМ ЧЕРЕЗ LOCATE

    void benchmarkRawLocate(std::size_t count) {
        std::vector<int> array(ARRAY_SIZE);

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            array[i] = static_cast<int>(i + 1);
        }

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            std::size_t index = i % ARRAY_SIZE;

            int* ptr = array.data() + index;

            sum += static_cast<std::uint64_t>(*ptr);

            BenchmarkUtils::preventOptimization(ptr);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

    void benchmarkSpanLocate(std::size_t count) {
        MemorySpan<int> span(static_cast<int>(ARRAY_SIZE));

        for (std::size_t i = 0; i < ARRAY_SIZE; ++i) {
            span.set(static_cast<int>(i), static_cast<int>(i + 1));
        }

        std::uint64_t sum = 0;

        for (std::size_t i = 0; i < count; ++i) {
            int index = static_cast<int>(i % ARRAY_SIZE);

            auto ptr = span.locate(index);

            sum += static_cast<std::uint64_t>(*ptr);

            BenchmarkUtils::preventOptimization(ptr);
        }

        BenchmarkUtils::preventOptimization(sum);
    }

} // namespace

// 6. ЗАПУСК ВСЕХ ИЗМЕРЕНИЙ MEMORYSPAN

void runMemorySpanBenchmark() {
    const std::vector<std::size_t> sizes = {
            100,
            1000,
            10000,
            100000,
            1000000
    };

    std::cout
            << "\n========== MEMORYSPAN BENCHMARK ==========\n\n";

    std::cout << std::fixed << std::setprecision(3);

    auto runComparison = [&](const char* name, auto raw, auto span) {
        std::cout << "\n--- " << name << " ---\n";

        std::cout
                << std::setw(12) << "Operations"
                << std::setw(18) << "Raw (ms)"
                << std::setw(18) << "MemorySpan (ms)"
                << '\n';

        for (std::size_t count : sizes) {
            double rawTime = BenchmarkUtils::measure([&]() {
                raw(count);
            });

            double spanTime = BenchmarkUtils::measure([&]() {
                span(count);
            });

            std::cout
                    << std::setw(12) << count
                    << std::setw(18) << rawTime
                    << std::setw(18) << spanTime
                    << '\n';
        }
    };

    runComparison(
            "Array Creation",
            benchmarkRawCreation,
            benchmarkSpanCreation
    );

    runComparison(
            "Set Elements",
            benchmarkRawSet,
            benchmarkSpanSet
    );

    runComparison(
            "Get Independent Copy",
            benchmarkRawGet,
            benchmarkSpanGet
    );

    runComparison(
            "Copy Shared Object",
            benchmarkRawCopy,
            benchmarkSpanCopy
    );

    runComparison(
            "Locate Elements",
            benchmarkRawLocate,
            benchmarkSpanLocate
    );
}