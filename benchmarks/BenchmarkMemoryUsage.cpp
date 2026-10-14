#include <iostream>
#include <iomanip>
#include <vector>
#include <cstddef>
#include <cstdint>
#include <string>
#include <stdexcept>
#include <limits>

#include <mach/mach.h>

#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "MemorySpan.h"
#include "BenchmarkSuite.h"

namespace {

    constexpr double MB = 1024.0 * 1024.0;

// Получение текущего объёма резидентной памяти процесса
    std::uint64_t getMemoryUsage() {
        mach_task_basic_info_data_t info{};
        mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;

        kern_return_t result = task_info(
                mach_task_self(),
                MACH_TASK_BASIC_INFO,
                reinterpret_cast<task_info_t>(&info),
                &count
        );

        if (result != KERN_SUCCESS) {
            throw std::runtime_error("Failed to read process memory");
        }

        return static_cast<std::uint64_t>(info.resident_size);
    }

// Вывод результата измерения
    void printMemory(std::uint64_t before, std::uint64_t after) {
        double difference =
                static_cast<double>(after) - static_cast<double>(before);

        std::cout << std::fixed << std::setprecision(3);

        std::cout << "Before: " << before / MB << " MB\n";
        std::cout << "After:  " << after / MB << " MB\n";
        std::cout << "Change: " << difference / MB << " MB\n";
    }

// Обычные указатели
    void measureRawMemory(std::size_t count) {
        std::vector<int*> pointers;
        pointers.reserve(count);

        std::uint64_t before = getMemoryUsage();

        for (std::size_t i = 0; i < count; ++i) {
            pointers.push_back(new int(static_cast<int>(i)));
        }

        std::uint64_t after = getMemoryUsage();

        printMemory(before, after);

        for (int* ptr : pointers) {
            delete ptr;
        }
    }

// UnqPtr
    void measureUniqueMemory(std::size_t count) {
        std::vector<UnqPtr<int>> pointers;
        pointers.reserve(count);

        std::uint64_t before = getMemoryUsage();

        for (std::size_t i = 0; i < count; ++i) {
            pointers.emplace_back(new int(static_cast<int>(i)));
        }

        std::uint64_t after = getMemoryUsage();

        printMemory(before, after);
    }

// ShrdPtr
    void measureSharedMemory(std::size_t count) {
        std::vector<ShrdPtr<int>> pointers;
        pointers.reserve(count);

        std::uint64_t before = getMemoryUsage();

        for (std::size_t i = 0; i < count; ++i) {
            pointers.emplace_back(
                    UnqPtr<int>(new int(static_cast<int>(i)))
            );
        }

        std::uint64_t after = getMemoryUsage();

        printMemory(before, after);
    }

// MemorySpan
    void measureSpanMemory(std::size_t count) {
        if (count > static_cast<std::size_t>(
                std::numeric_limits<int>::max())) {
            throw std::invalid_argument("Array size is too large");
        }

        std::uint64_t before = getMemoryUsage();

        MemorySpan<int> span(static_cast<int>(count));

        for (std::size_t i = 0; i < count; ++i) {
            span.set(static_cast<int>(i), static_cast<int>(i));
        }

        std::uint64_t after = getMemoryUsage();

        printMemory(before, after);
    }

} // namespace

// Запуск выбранного измерения
void runMemoryUsageBenchmark(const std::string& type,
                             std::size_t count) {
    std::cout << "\n========== MEMORY USAGE BENCHMARK ==========\n\n";

    std::cout << "--- Object Sizes ---\n";
    std::cout << "Raw Pointer: " << sizeof(int*) << " bytes\n";
    std::cout << "UnqPtr<int>: " << sizeof(UnqPtr<int>) << " bytes\n";
    std::cout << "ShrdPtr<int>: " << sizeof(ShrdPtr<int>) << " bytes\n";
    std::cout << "MemorySpan<int>: " << sizeof(MemorySpan<int>) << " bytes\n";

    std::cout << "\nType: " << type << '\n';
    std::cout << "Objects: " << count << "\n\n";

    if (type == "raw") {
        measureRawMemory(count);
    }
    else if (type == "unique") {
        measureUniqueMemory(count);
    }
    else if (type == "shared") {
        measureSharedMemory(count);
    }
    else if (type == "span") {
        measureSpanMemory(count);
    }
    else {
        throw std::invalid_argument("Unknown memory benchmark type");
    }
}