#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>

#include "MemorySpan.h"
#include "MsPtr.h"
#include "UnqPtr.h"
#include "ShrdPtr.h"
#include "Instance_tracker.h"

// 1. СОЗДАНИЕ КОНТЕЙНЕРА

TEST(MemorySpanTest, ConstructorInitializesSizeAndCapacity) {
    MemorySpan<int> span(5);

    EXPECT_EQ(span.get_size(), 5);
    EXPECT_EQ(span.get_capacity(), 5);
}

// 2. СОЗДАНИЕ ПУСТОГО КОНТЕЙНЕРА

TEST(MemorySpanTest, DefaultConstructorCreatesEmptySpan) {
    MemorySpan<int> span;

    EXPECT_EQ(span.get_size(), 0);
    EXPECT_EQ(span.get_capacity(), 0);

    EXPECT_THROW(span.get(0), std::out_of_range);
    EXPECT_THROW(span.copy(0), std::out_of_range);
    EXPECT_THROW(span.locate(0), std::out_of_range);
}

// 3. ПРОВЕРКА ОТРИЦАТЕЛЬНОГО РАЗМЕРА

TEST(MemorySpanTest, NegativeSizeThrowsException) {
    EXPECT_THROW(MemorySpan<int>(-1), std::invalid_argument);
}

// 4. ЗАПИСЬ И ЧТЕНИЕ ЭЛЕМЕНТОВ

TEST(MemorySpanTest, SetAndGetWorkCorrectly) {
    MemorySpan<int> span(3);

    span.set(0, 10);
    span.set(1, 20);
    span.set(2, 30);

    EXPECT_EQ(*span.get(0), 10);
    EXPECT_EQ(*span.get(1), 20);
    EXPECT_EQ(*span.get(2), 30);
}

// 5. ПРОВЕРКА ГРАНИЦ МАССИВА

TEST(MemorySpanTest, OutOfBoundsThrowsException) {
    MemorySpan<int> span(3);

    EXPECT_THROW(span.set(3, 100), std::out_of_range);
    EXPECT_THROW(span.set(-1, 100), std::out_of_range);

    EXPECT_THROW(span.get(3), std::out_of_range);
    EXPECT_THROW(span.get(-1), std::out_of_range);

    EXPECT_THROW(span.copy(3), std::out_of_range);
    EXPECT_THROW(span.copy(-1), std::out_of_range);

    EXPECT_THROW(span.locate(3), std::out_of_range);
    EXPECT_THROW(span.locate(-1), std::out_of_range);
}

// 6. МЕТОД GET СОЗДАЁТ НЕЗАВИСИМУЮ КОПИЮ

TEST(MemorySpanTest, GetReturnsIndependentCopy) {
    MemorySpan<int> span(2);

    span.set(0, 42);

    UnqPtr<int> ptr = span.get(0);

    EXPECT_EQ(*ptr, 42);

    *ptr = 100;

    EXPECT_EQ(*ptr, 100);
    EXPECT_EQ(*span.locate(0), 42);
}

// 7. МЕТОД COPY СОЗДАЁТ РАЗДЕЛЯЕМУЮ КОПИЮ

TEST(MemorySpanTest, CopyReturnsSharedCopy) {
    MemorySpan<int> span(2);

    span.set(0, 50);

    ShrdPtr<int> first = span.copy(0);
    ShrdPtr<int> second = first;

    EXPECT_EQ(first.use_count(), 2);
    EXPECT_EQ(second.use_count(), 2);
    EXPECT_EQ(first.get(), second.get());

    *first = 99;

    EXPECT_EQ(*second, 99);

    // Исходный элемент не изменяется
    EXPECT_EQ(*span.locate(0), 50);
}

// 8. LOCATE ПРЕДОСТАВЛЯЕТ ДОСТУП К ИСХОДНОМУ МАССИВУ

TEST(MemorySpanTest, LocateProvidesDirectAccess) {
    MemorySpan<int> span(3);

    span.set(0, 10);
    span.set(1, 20);
    span.set(2, 30);

    MsPtr<int> ptr = span.locate(1);

    EXPECT_EQ(*ptr, 20);

    // Изменяем элемент через MsPtr
    *ptr = 100;

    EXPECT_EQ(*span.locate(1), 100);

    // Переходим к следующему элементу
    ++ptr;

    EXPECT_EQ(*ptr, 30);
}

// 9. БЕЗОПАСНАЯ АРИФМЕТИКА УКАЗАТЕЛЕЙ

TEST(MemorySpanTest, LocateRespectsArrayBounds) {
    MemorySpan<int> span(3);

    span.set(0, 10);
    span.set(1, 20);
    span.set(2, 30);

    auto ptr = span.locate(0);

    EXPECT_EQ(*(ptr + 1), 20);
    EXPECT_EQ(*(ptr + 2), 30);

    // Попытка выйти за границы
    EXPECT_THROW(ptr + 4, std::out_of_range);
    EXPECT_THROW(ptr - 1, std::out_of_range);

    // Указатель на конец массива допустим,
    // но его нельзя разыменовывать
    auto end = ptr + 3;

    EXPECT_THROW(*end, std::out_of_range);
}

// 10. КОНСТРУКТОР ПЕРЕМЕЩЕНИЯ

TEST(MemorySpanTest, MoveConstructorTransfersOwnership) {
    MemorySpan<int> first(3);

    first.set(0, 10);
    first.set(1, 20);
    first.set(2, 30);

    MemorySpan<int> second(std::move(first));

    EXPECT_EQ(first.get_size(), 0);
    EXPECT_EQ(first.get_capacity(), 0);

    EXPECT_EQ(second.get_size(), 3);
    EXPECT_EQ(second.get_capacity(), 3);

    EXPECT_EQ(*second.locate(0), 10);
    EXPECT_EQ(*second.locate(1), 20);
    EXPECT_EQ(*second.locate(2), 30);
}

// 11. ПЕРЕМЕЩАЮЩЕЕ ПРИСВАИВАНИЕ

TEST(MemorySpanTest, MoveAssignmentTransfersOwnership) {
    MemorySpan<int> first(2);

    first.set(0, 100);
    first.set(1, 200);

    MemorySpan<int> second(3);

    second = std::move(first);

    EXPECT_EQ(first.get_size(), 0);
    EXPECT_EQ(first.get_capacity(), 0);

    EXPECT_EQ(second.get_size(), 2);
    EXPECT_EQ(second.get_capacity(), 2);

    EXPECT_EQ(*second.locate(0), 100);
    EXPECT_EQ(*second.locate(1), 200);
}

// 12. ПРОВЕРКА АВТОМАТИЧЕСКОГО ОСВОБОЖДЕНИЯ ПАМЯТИ

TEST(MemorySpanTest, DestructorReleasesAllObjects) {
    ASSERT_EQ(InstanceTracker::active_instances, 0);

    {
        MemorySpan<InstanceTracker> span(4);

        EXPECT_EQ(InstanceTracker::active_instances, 4);

        {
            UnqPtr<InstanceTracker> unique = span.get(0);

            EXPECT_EQ(InstanceTracker::active_instances, 5);
        }

        EXPECT_EQ(InstanceTracker::active_instances, 4);

        {
            ShrdPtr<InstanceTracker> shared = span.copy(1);

            EXPECT_EQ(InstanceTracker::active_instances, 5);

            ShrdPtr<InstanceTracker> second = shared;

            EXPECT_EQ(shared.use_count(), 2);
            EXPECT_EQ(InstanceTracker::active_instances, 5);
        }

        EXPECT_EQ(InstanceTracker::active_instances, 4);
    }

    EXPECT_EQ(InstanceTracker::active_instances, 0);
}