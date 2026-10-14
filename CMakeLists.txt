cmake_minimum_required(VERSION 3.16)

project(SmartPointers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключение Google Test
find_package(GTest REQUIRED)

enable_testing()

# Основная программа
add_executable(SmartPointers
        main.cpp
)

# Функциональные тесты
add_executable(smart_pointer_tests
        tests/Test_UnqPtr.cpp
        tests/Test_ShrdPtr.cpp
        tests/Test_MsPtr.cpp
        tests/Test_MemorySpan.cpp

)
# Нагрузочное тестирование
add_executable(smart_pointer_benchmarks
        benchmarks/BenchmarkMain.cpp

        benchmarks/BenchmarkUtils.h
        benchmarks/BenchmarkSuite.h

        benchmarks/BenchmarkCreation.cpp
        benchmarks/BenchmarkOwnership.cpp
        benchmarks/BenchmarkAccess.cpp
        benchmarks/BenchmarkArithmetic.cpp
        benchmarks/BenchmarkMemorySpan.cpp
        benchmarks/BenchmarkMemoryUsage.cpp
)

# Оптимизация для измерения производительности
target_compile_options(smart_pointer_benchmarks
        PRIVATE
        -O2
        -Wall
        -Wextra
)
# Подключение библиотеки Google Test
target_link_libraries(smart_pointer_tests
        PRIVATE
        GTest::gtest_main
)

# Пути к заголовочным файлам
target_include_directories(smart_pointer_tests
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
)
target_include_directories(smart_pointer_benchmarks
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks
)

# Регистрация тестов
include(GoogleTest)

gtest_discover_tests(smart_pointer_tests)