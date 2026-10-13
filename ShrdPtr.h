#pragma once

#include "UnqPtr.h"

#include <cstddef>
#include <type_traits>
#include <utility>

template <typename T>
class ShrdPtr {
public:
    using ElementType = std::remove_extent_t<T>;

private:
    UnqPtr<T>* owner_ = nullptr;
    std::size_t* refCount_ = nullptr;

    void releaseReference() noexcept {
        if (refCount_ == nullptr) {
            return;
        }

        --(*refCount_);

        if (*refCount_ == 0) {
            delete owner_;
            delete refCount_;
        }

        owner_ = nullptr;
        refCount_ = nullptr;
    }

public:
    ShrdPtr() noexcept = default;

    explicit ShrdPtr(UnqPtr<T>&& uniqueOwner) {
        if (uniqueOwner) {
            owner_ = new UnqPtr<T>(std::move(uniqueOwner));
            refCount_ = new std::size_t(1);
        }
    }

    ~ShrdPtr() noexcept {
        releaseReference();
    }

    // Копирование
    ShrdPtr(const ShrdPtr& source) noexcept
            : owner_(source.owner_),
              refCount_(source.refCount_) {

        if (refCount_ != nullptr) {
            ++(*refCount_);
        }
    }

    ShrdPtr& operator=(const ShrdPtr& source) noexcept {
        if (this != &source) {
            releaseReference();

            owner_ = source.owner_;
            refCount_ = source.refCount_;

            if (refCount_ != nullptr) {
                ++(*refCount_);
            }
        }

        return *this;
    }

    // Перемещение
    ShrdPtr(ShrdPtr&& source) noexcept
            : owner_(source.owner_),
              refCount_(source.refCount_) {

        source.owner_ = nullptr;
        source.refCount_ = nullptr;
    }

    ShrdPtr& operator=(ShrdPtr&& source) noexcept {
        if (this != &source) {
            releaseReference();

            owner_ = source.owner_;
            refCount_ = source.refCount_;

            source.owner_ = nullptr;
            source.refCount_ = nullptr;
        }

        return *this;
    }

    // Разыменование
    ElementType& operator*() const noexcept {
        return **owner_;
    }

    // Доступ к членам объекта
    ElementType* operator->() const noexcept {
        return owner_->get();
    }

    // Доступ к элементам массива
    ElementType& operator[](std::size_t index) const noexcept {
        return (*owner_)[index];
    }

    // Получение обычного указателя
    ElementType* get() const noexcept {
        if (owner_ == nullptr) {
            return nullptr;
        }

        return owner_->get();
    }

    // Проверка наличия объекта
    explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    // Количество ShrdPtr, разделяющих объект
    std::size_t use_count() const noexcept {
        if (refCount_ == nullptr) {
            return 0;
        }

        return *refCount_;
    }

    // Отказ от текущего объекта
    void reset() noexcept {
        releaseReference();
    }
};