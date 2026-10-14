#pragma once

#include <string>
#include <cstddef>

// Создание и уничтожение объектов
void runCreationBenchmark();

// Копирование и перемещение указателей
void runOwnershipBenchmark();

// Доступ к объектам
void runAccessBenchmark();

// Арифметика указателей
void runArithmeticBenchmark();

// Операции MemorySpan
void runMemorySpanBenchmark();

// Измерение памяти
void runMemoryUsageBenchmark(
        const std::string& type,
        std::size_t count
);