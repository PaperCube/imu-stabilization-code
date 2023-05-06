#pragma once

#include <cmath>
#include <cassert>
#include <utility>

/**
 * @tparam T must be trivially copiable
 */
template<typename T>
class fixed_size_buffer {
private:
    T *mData = nullptr;

    size_t mBegin = 0, mSize = 0;
    size_t mMaxSize = 0;


public:

    explicit fixed_size_buffer(size_t aSize) : mMaxSize(aSize) {
        mData = new T[aSize];
    }

    fixed_size_buffer(const fixed_size_buffer &other) {
        mMaxSize = other.mMaxSize;
        mData = new T[mMaxSize];
        other.copy_range(mData, mMaxSize);
        mSize = other.mSize;
    }

    fixed_size_buffer &operator=(const fixed_size_buffer &other) {
        fixed_size_buffer tmp(other);
        swap(tmp);
        return *this;
    }

    void swap(fixed_size_buffer &other) {
        std::swap(mData, other.mData);
        std::swap(mBegin, other.mBegin);
        std::swap(mSize, other.mSize);
        std::swap(mMaxSize, other.mMaxSize);
    }

    ~fixed_size_buffer() {
        delete[] mData;
        mData = nullptr;
    }

    fixed_size_buffer(fixed_size_buffer &&other) noexcept {
        swap(other);
    }

    fixed_size_buffer &operator=(fixed_size_buffer other) noexcept {
        swap(other);
        return *this;
    }

    size_t size() const {
        return mSize;
    }

    bool empty() const {
        return mSize == 0;
    }

    T &operator[](size_t index) {
        return mData[(mBegin + index) % mMaxSize];
    }

    const T &operator[](size_t index) const {
        return mData[(mBegin + index) % mMaxSize];
    }

    bool push_range(const T *begin, const T *end) {
        unsigned long long length = end - begin;
        if (length + mSize > mMaxSize) {
            return false;
        }
        for (size_t i = 0; i < length; ++i) {
            mData[(mBegin + mSize + i) % mMaxSize] = begin[i];
        }
        mSize += length;
    }

    void shrink_left(size_t count) {
        count = std::min(count, mSize);
        mBegin = (mBegin + count) % mMaxSize;
        mSize -= count;
    }

    void shrink_right(size_t count) {
        count = std::min(count, mSize);
        mSize -= count;
    }

    void pop_range(T *out, size_t count) {
        count = std::min(count, mSize);
        for (size_t i = 0; i < count; ++i) {
            out[i] = mData[(mBegin + i) % mMaxSize];
        }
    }

    void copy_range(T *out, size_t count, size_t offset = 0) const {
        assert(offset + count <= mSize);
        for (size_t i = 0; i < count; ++i) {
            out[i] = (*this)[offset + i];
        }
    }

    bool push_back(const T &value) {
        assert(mSize <= mMaxSize);
        if (mSize >= mMaxSize) {
            return false;
        }
        (*this)[mSize] = value;
        ++mSize;
        return true;
    }

    T pop_front() {
        assert(mSize <= mMaxSize);
        if (mSize == 0) {
            return false;
        }
        T value = (*this)[0];
        mBegin = (mBegin + 1) % mMaxSize;
        --mSize;
        return value;
    }
};