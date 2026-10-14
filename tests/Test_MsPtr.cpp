#include <gtest/gtest.h>

#include <stdexcept>

#include "MsPtr.h"

// 1. СОЗДАНИЕ УКАЗАТЕЛЯ

TEST(MsPtrTest, ConstructorPointsToCorrectElement) {

    int arr[3] = {10, 20, 30};

    MsPtr<int> ptr(arr + 1, arr, arr + 3);

    EXPECT_EQ(*ptr, 20);

}

// 2. ПРЕФИКСНЫЙ ИНКРЕМЕНТ

TEST(MsPtrTest, PrefixIncrementMovesForward) {

int arr[3] = {10, 20, 30};

MsPtr<int> ptr(arr, arr, arr + 3);

++ptr;

EXPECT_EQ(*ptr, 20);

}

// 3. ПОСТФИКСНЫЙ ИНКРЕМЕНТ

TEST(MsPtrTest, PostfixIncrementReturnsPreviousPosition) {

int arr[3] = {10, 20, 30};

MsPtr<int> ptr(arr, arr, arr + 3);

MsPtr<int> old = ptr++;

EXPECT_EQ(*old, 10);

EXPECT_EQ(*ptr, 20);

}

// 4. ПРЕФИКСНЫЙ ДЕКРЕМЕНТ

TEST(MsPtrTest, PrefixDecrementMovesBackward) {

int arr[3] = {10, 20, 30};

MsPtr<int> ptr(arr + 2, arr, arr + 3);

--ptr;

EXPECT_EQ(*ptr, 20);

}

// 5. ПОСТФИКСНЫЙ ДЕКРЕМЕНТ

TEST(MsPtrTest, PostfixDecrementReturnsPreviousPosition) {

int arr[3] = {10, 20, 30};

MsPtr<int> ptr(arr + 2, arr, arr + 3);

MsPtr<int> old = ptr--;

EXPECT_EQ(*old, 30);

EXPECT_EQ(*ptr, 20);

}

// 6. СЛОЖЕНИЕ СО СМЕЩЕНИЕМ

TEST(MsPtrTest, AdditionMovesPointerForward) {

int arr[5] = {10, 20, 30, 40, 50};

MsPtr<int> ptr(arr, arr, arr + 5);

auto result = ptr + 3;

EXPECT_EQ(*result, 40);

EXPECT_EQ(*ptr, 10);

}

// 7. ВЫЧИТАНИЕ СМЕЩЕНИЯ

TEST(MsPtrTest, SubtractionMovesPointerBackward) {

int arr[5] = {10, 20, 30, 40, 50};

MsPtr<int> ptr(arr + 4, arr, arr + 5);

auto result = ptr - 2;

EXPECT_EQ(*result, 30);

EXPECT_EQ(*ptr, 50);

}

// 8. ВЫХОД ЗА ВЕРХНЮЮ ГРАНИЦУ

TEST(MsPtrTest, UpperBoundThrowsException) {

int arr[3] = {1, 2, 3};

MsPtr<int> ptr(arr + 2, arr, arr + 3);

EXPECT_THROW(ptr + 2, std::out_of_range);

}

// 9. ВЫХОД ЗА НИЖНЮЮ ГРАНИЦУ

TEST(MsPtrTest, LowerBoundThrowsException) {

int arr[3] = {1, 2, 3};

MsPtr<int> ptr(arr, arr, arr + 3);

EXPECT_THROW(ptr - 1, std::out_of_range);

}

// 10. ЗАПРЕТ РАЗЫМЕНОВАНИЯ END

TEST(MsPtrTest, DereferencingEndThrowsException) {

int arr[3] = {1, 2, 3};

MsPtr<int> ptr(arr + 3, arr, arr + 3);

EXPECT_THROW(*ptr, std::out_of_range);

EXPECT_THROW(ptr.operator->(), std::out_of_range);

}

// 11. СРАВНЕНИЕ УКАЗАТЕЛЕЙ

TEST(MsPtrTest, EqualityOperatorsWorkCorrectly) {

int arr[3] = {10, 20, 30};

MsPtr<int> first(arr, arr, arr + 3);

MsPtr<int> second(arr, arr, arr + 3);

MsPtr<int> third(arr + 1, arr, arr + 3);

EXPECT_TRUE(first == second);

EXPECT_TRUE(first != third);

}

// 12. НЕЛЬЗЯ ПЕРЕЙТИ ДАЛЬШЕ END

TEST(MsPtrTest, IncrementBeyondEndThrowsException) {

int arr[2] = {1, 2};

MsPtr<int> ptr(arr + 2, arr, arr + 2);

EXPECT_THROW(++ptr, std::out_of_range);

}