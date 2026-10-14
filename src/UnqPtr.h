#pragma once

#include <type_traits>
#include <cstddef>

template <typename T>
class UnqPtr {
public:
    using ValueType = std::remove_extent_t<T>;

private:
    ValueType* ptr_ = nullptr;

    void destroy(ValueType* ptr) noexcept {
        if constexpr (std::is_array_v<T>) {
            delete[] ptr;
        } else {
            delete ptr;
        }
    }

public:
    // Пустой указатель
    UnqPtr() noexcept = default;

    // Получение владения объектом
    explicit UnqPtr(ValueType* ptr) noexcept
            : ptr_(ptr) {}

    // Освобождение памяти
    ~UnqPtr() noexcept {
        destroy(ptr_);
    }

    // Копирование запрещено
    UnqPtr(const UnqPtr&) = delete;
    UnqPtr& operator=(const UnqPtr&) = delete;

    // Конструктор перемещения
    UnqPtr(UnqPtr&& other) noexcept
            : ptr_(other.release()) {}

    // Присваивание перемещением
    UnqPtr& operator=(UnqPtr&& other) noexcept {
        if (this != &other) {
            reset(other.release());
        }

        return *this;
    }

    // Подтипизация: UnqPtr<Derived> -> UnqPtr<Base>
    template <typename U>
    UnqPtr(UnqPtr<U>&& other) noexcept
            : ptr_(static_cast<ValueType*>(other.release())) {}

    template <typename U>
    UnqPtr& operator=(UnqPtr<U>&& other) noexcept {
        reset(static_cast<ValueType*>(other.release()));
        return *this;
    }

    // Разыменование
    ValueType& operator*() const noexcept {
        return *ptr_;
    }

    // Доступ к членам объекта
    ValueType* operator->() const noexcept {
        return ptr_;
    }

    // Доступ к элементам массива
    ValueType& operator[](std::size_t index) const noexcept {
        return ptr_[index];
    }

    // Получить обычный указатель
    ValueType* get() const noexcept {
        return ptr_;
    }

    // Проверка на nullptr
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    // Отдать владение
    ValueType* release() noexcept {
        ValueType* result = ptr_;
        ptr_ = nullptr;

        return result;
    }

    // Заменить управляемый объект
    void reset(ValueType* new_ptr = nullptr) noexcept {
        if (ptr_ == new_ptr) {
            return;
        }

        destroy(ptr_);
        ptr_ = new_ptr;
    }
};