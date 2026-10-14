#pragma once

#include <cstddef>
#include <stdexcept>

template <typename T>
class MsPtr {
private:
    T* position;
    T* first;
    T* last;

public:
    MsPtr(T* ptr, T
    * begin, T* end)
            : position(ptr), first(begin), last(end) {

        if (position < first || position > last) {
            throw std::out_of_range("MsPtr out of range");
        }
    }

    MsPtr& operator++() {
        if (position >= last) {
            throw std::out_of_range("MsPtr out of upper bound");
        }

        ++position;
        return *this;
    }

    MsPtr operator++(int) {
        MsPtr temp = *this;
        ++(*this);
        return temp;
    }

    MsPtr& operator--() {
        if (position <= first) {
            throw std::out_of_range("MsPtr out of lower bound");
        }

        --position;
        return *this;
    }

    MsPtr operator--(int) {
        MsPtr temp = *this;
        --(*this);
        return temp;
    }

    MsPtr operator+(std::ptrdiff_t offset) const {
        if (offset < 0) {
            return *this - (-offset);
        }

        if (offset > last - position) {
            throw std::out_of_range("MsPtr out of range");
        }

        return MsPtr(position + offset, first, last);
    }

    MsPtr operator-(std::ptrdiff_t offset) const {
        if (offset < 0) {
            return *this + (-offset);
        }

        if (offset > position - first) {
            throw std::out_of_range("MsPtr out of range");
        }

        return MsPtr(position - offset, first, last);
    }

    T& operator*() const {
        if (position == last) {
            throw std::out_of_range("Cannot dereference end pointer");
        }

        return *position;
    }

    T* operator->() const {
        if (position == last) {
            throw std::out_of_range("Cannot dereference end pointer");
        }

        return position;
    }

    bool operator==(const MsPtr& other) const noexcept {
        return position == other.position;
    }

    bool operator!=(const MsPtr& other) const noexcept {
        return !(*this == other);
    }
};