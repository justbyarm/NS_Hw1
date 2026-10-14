#include <gtest/gtest.h>
#include <utility>
#include <type_traits>

#include "UnqPtr.h"
#include "Instance_tracker.h"

// 1. СОЗДАНИЕ И ИНИЦИАЛИЗАЦИЯ

TEST(UnqPtrTest, DefaultAndValueInitialization) {
    UnqPtr<int> empty;
    EXPECT_EQ(empty.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(empty));

UnqPtr<int> ptr(new int(42));

EXPECT_NE(ptr.get(), nullptr);
EXPECT_TRUE(static_cast<bool>(ptr));
EXPECT_EQ(*ptr, 42);
}

// 2. ЗАПРЕТ КОПИРОВАНИЯ

TEST(UnqPtrTest, CopyOperationsAreDeleted) {
EXPECT_FALSE(std::is_copy_constructible_v<UnqPtr<int>>);
EXPECT_FALSE(std::is_copy_assignable_v<UnqPtr<int>>);
}

// 3. КОНСТРУКТОР ПЕРЕМЕЩЕНИЯ

TEST(UnqPtrTest, MoveConstructorTransfersOwnership) {
UnqPtr<int> first(new int(100));

int* originalAddress = first.get();

UnqPtr<int> second(std::move(first));

EXPECT_EQ(first.get(), nullptr);
EXPECT_FALSE(static_cast<bool>(first));

EXPECT_EQ(second.get(), originalAddress);
EXPECT_EQ(*second, 100);
}

// 4. ПЕРЕМЕЩАЮЩЕЕ ПРИСВАИВАНИЕ

TEST(UnqPtrTest, MoveAssignmentTransfersOwnership) {
UnqPtr<int> first(new int(10));
UnqPtr<int> second(new int(20));

int* originalAddress = first.get();

second = std::move(first);

EXPECT_EQ(first.get(), nullptr);
EXPECT_EQ(second.get(), originalAddress);
EXPECT_EQ(*second, 10);
}

// 5. МЕТОД RELEASE

TEST(UnqPtrTest, ReleaseTransfersRawPointer) {
UnqPtr<int> ptr(new int(55));

int* raw = ptr.release();

EXPECT_EQ(ptr.get(), nullptr);
EXPECT_FALSE(static_cast<bool>(ptr));

ASSERT_NE(raw, nullptr);
EXPECT_EQ(*raw, 55);

delete raw;
}

// 6. МЕТОД RESET

TEST(UnqPtrTest, ResetReplacesManagedObject) {
UnqPtr<int> ptr(new int(10));

ptr.reset(new int(20));

EXPECT_EQ(*ptr, 20);

ptr.reset();

EXPECT_EQ(ptr.get(), nullptr);
EXPECT_FALSE(static_cast<bool>(ptr));
}

// 7. РАБОТА С МАССИВАМИ

TEST(UnqPtrTest, ArrayElementsCanBeAccessed) {
UnqPtr<int[]> arr(new int[4]{10, 20, 30, 40});

EXPECT_EQ(arr[0], 10);
EXPECT_EQ(arr[1], 20);
EXPECT_EQ(arr[2], 30);
EXPECT_EQ(arr[3], 40);

arr[2] = 99;

EXPECT_EQ(arr[2], 99);
}

// 8. ПОДДЕРЖКА НАСЛЕДОВАНИЯ

class Base {
public:
    virtual ~Base() = default;

    virtual int getValue() const {
        return 1;
    }
};

class Derived : public Base {
public:
    int getValue() const override {
        return 2;
    }
};

TEST(UnqPtrTest, DerivedCanBeMovedToBase) {
UnqPtr<Derived> derived(new Derived());

UnqPtr<Base> base(std::move(derived));

EXPECT_EQ(derived.get(), nullptr);
ASSERT_NE(base.get(), nullptr);
EXPECT_EQ(base->getValue(), 2);
}

// 9. АВТОМАТИЧЕСКОЕ ОСВОБОЖДЕНИЕ ПАМЯТИ

TEST(UnqPtrTest, DestructorReleasesObject) {
ASSERT_EQ(InstanceTracker::active_instances, 0);

{
UnqPtr<InstanceTracker> ptr(new InstanceTracker(10));

EXPECT_EQ(InstanceTracker::active_instances, 1);
EXPECT_EQ(ptr->value, 10);
}

EXPECT_EQ(InstanceTracker::active_instances, 0);
}

// 10. КОРРЕКТНОЕ ОСВОБОЖДЕНИЕ МАССИВА

TEST(UnqPtrTest, ArrayDestructorReleasesAllObjects) {
ASSERT_EQ(InstanceTracker::active_instances, 0);

{
UnqPtr<InstanceTracker[]> arr(
        new InstanceTracker[5]
);

EXPECT_EQ(InstanceTracker::active_instances, 5);
}

EXPECT_EQ(InstanceTracker::active_instances, 0);
}

// 11. RESET НЕ ДОЛЖЕН ОСТАВЛЯТЬ СТАРЫЙ ОБЪЕКТ

TEST(UnqPtrTest, ResetDestroysPreviousObject) {
ASSERT_EQ(InstanceTracker::active_instances, 0);

{
UnqPtr<InstanceTracker> ptr(
        new InstanceTracker(10)
);

EXPECT_EQ(InstanceTracker::active_instances, 1);

ptr.reset(new InstanceTracker(20));

EXPECT_EQ(InstanceTracker::active_instances, 1);
EXPECT_EQ(ptr->value, 20);

ptr.reset();

EXPECT_EQ(InstanceTracker::active_instances, 0);
}

EXPECT_EQ(InstanceTracker::active_instances, 0);
}