#include <gtest/gtest.h>
#include <utility>

#include "ShrdPtr.h"
#include "UnqPtr.h"
#include "instance_tracker.h"

// 1. СОЗДАНИЕ ПУСТОГО УКАЗАТЕЛЯ

TEST(ShrdPtrTest, DefaultConstructorCreatesEmptyPointer) {
    ShrdPtr<int> ptr;
    EXPECT_EQ(ptr.get(), nullptr);
    EXPECT_EQ(ptr.use_count(), 0);
    EXPECT_FALSE(static_cast<bool>(ptr));
}

// 2. СОЗДАНИЕ ИЗ UNQPTR

TEST(ShrdPtrTest, ConstructionFromUnqPtrTransfersOwnership) {
UnqPtr<int> unique(new int(42));

ShrdPtr<int> shared(std::move(unique));

EXPECT_EQ(unique.get(), nullptr);

ASSERT_NE(shared.get(), nullptr);
EXPECT_EQ(*shared, 42);
EXPECT_EQ(shared.use_count(), 1);
}

// 3. КОПИРОВАНИЕ УВЕЛИЧИВАЕТ СЧЁТЧИК

TEST(ShrdPtrTest, CopyConstructorIncreasesReferenceCount) {
ShrdPtr<int> first(
        UnqPtr<int>(new int(100))
);

ShrdPtr<int> second(first);

EXPECT_EQ(first.use_count(), 2);
EXPECT_EQ(second.use_count(), 2);

EXPECT_EQ(first.get(), second.get());
}

// 4. КОПИРУЮЩЕЕ ПРИСВАИВАНИЕ

TEST(ShrdPtrTest, CopyAssignmentSharesOwnership) {
ShrdPtr<int> first(
        UnqPtr<int>(new int(10))
);

ShrdPtr<int> second(
        UnqPtr<int>(new int(20))
);

second = first;

EXPECT_EQ(first.use_count(), 2);
EXPECT_EQ(second.use_count(), 2);
EXPECT_EQ(first.get(), second.get());
EXPECT_EQ(*second, 10);
}

// 5. КОНСТРУКТОР ПЕРЕМЕЩЕНИЯ

TEST(ShrdPtrTest, MoveConstructorTransfersOwnership) {
ShrdPtr<int> first(
        UnqPtr<int>(new int(50))
);

ShrdPtr<int> second(std::move(first));

EXPECT_EQ(first.get(), nullptr);
EXPECT_EQ(first.use_count(), 0);

EXPECT_EQ(*second, 50);
EXPECT_EQ(second.use_count(), 1);
}

// 6. ПЕРЕМЕЩАЮЩЕЕ ПРИСВАИВАНИЕ

TEST(ShrdPtrTest, MoveAssignmentTransfersOwnership) {
ShrdPtr<int> first(
        UnqPtr<int>(new int(10))
);

ShrdPtr<int> second(
        UnqPtr<int>(new int(20))
);

second = std::move(first);

EXPECT_EQ(first.get(), nullptr);
EXPECT_EQ(first.use_count(), 0);

EXPECT_EQ(*second, 10);
EXPECT_EQ(second.use_count(), 1);
}

// 7. RESET УМЕНЬШАЕТ СЧЁТЧИК

TEST(ShrdPtrTest, ResetDecreasesReferenceCount) {
ShrdPtr<int> first(
        UnqPtr<int>(new int(30))
);

ShrdPtr<int> second(first);

EXPECT_EQ(first.use_count(), 2);

first.reset();

EXPECT_EQ(first.use_count(), 0);
EXPECT_EQ(second.use_count(), 1);
EXPECT_EQ(*second, 30);
}

// 8. НЕСКОЛЬКО УКАЗАТЕЛЕЙ НА ОДИН ОБЪЕКТ

TEST(ShrdPtrTest, MultiplePointersShareSameObject) {
ShrdPtr<int> first(
        UnqPtr<int>(new int(10))
);

ShrdPtr<int> second(first);
ShrdPtr<int> third(second);

EXPECT_EQ(first.use_count(), 3);

*second = 99;

EXPECT_EQ(*first, 99);
EXPECT_EQ(*third, 99);
}

// 9. ПОДДЕРЖКА МАССИВОВ

TEST(ShrdPtrTest, ArrayElementsCanBeAccessed) {
ShrdPtr<int[]> ptr(
        UnqPtr<int[]>(new int[3]{1, 2, 3})
);

EXPECT_EQ(ptr[0], 1);
EXPECT_EQ(ptr[1], 2);
EXPECT_EQ(ptr[2], 3);

ptr[1] = 100;

EXPECT_EQ(ptr[1], 100);
}

// 10. ПОСЛЕДНИЙ ВЛАДЕЛЕЦ УДАЛЯЕТ ОБЪЕКТ

TEST(ShrdPtrTest, LastOwnerDestroysObject) {
ASSERT_EQ(InstanceTracker::active_instances, 0);

{
ShrdPtr<InstanceTracker> first(
        UnqPtr<InstanceTracker>(
                new InstanceTracker(10)
        )
);

EXPECT_EQ(InstanceTracker::active_instances, 1);

{
ShrdPtr<InstanceTracker> second(first);

EXPECT_EQ(first.use_count(), 2);
EXPECT_EQ(InstanceTracker::active_instances, 1);
}

EXPECT_EQ(first.use_count(), 1);
EXPECT_EQ(InstanceTracker::active_instances, 1);
}

EXPECT_EQ(InstanceTracker::active_instances, 0);
}

// 11. ПРИСВАИВАНИЕ УДАЛЯЕТ ПРЕДЫДУЩИЙ ОБЪЕКТ

TEST(ShrdPtrTest, AssignmentReleasesPreviousObject) {
ASSERT_EQ(InstanceTracker::active_instances, 0);

{
ShrdPtr<InstanceTracker> first(
        UnqPtr<InstanceTracker>(
                new InstanceTracker(10)
        )
);

ShrdPtr<InstanceTracker> second(
        UnqPtr<InstanceTracker>(
                new InstanceTracker(20)
        )
);

EXPECT_EQ(InstanceTracker::active_instances, 2);

second = first;

EXPECT_EQ(InstanceTracker::active_instances, 1);
EXPECT_EQ(first.use_count(), 2);
}

EXPECT_EQ(InstanceTracker::active_instances, 0);
}

// 12. RESET ПОСЛЕДНЕГО ВЛАДЕЛЬЦА

TEST(ShrdPtrTest, ResetLastOwnerDestroysObject) {
ASSERT_EQ(InstanceTracker::active_instances, 0);

{
ShrdPtr<InstanceTracker> ptr(
        UnqPtr<InstanceTracker>(
                new InstanceTracker(100)
        )
);

EXPECT_EQ(InstanceTracker::active_instances, 1);

ptr.reset();

EXPECT_EQ(InstanceTracker::active_instances, 0);
EXPECT_EQ(ptr.use_count(), 0);
}

EXPECT_EQ(InstanceTracker::active_instances, 0);
}