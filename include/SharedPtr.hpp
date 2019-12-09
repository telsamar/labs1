// Copyright 2019 <telsamar>
#pragma once

#include <iostream>
#include <atomic>
#include <cstddef>
#include <utility>

template<class T>
class SharedPtr
{
public:
    SharedPtr()
        : totalUse(new std::atomic<size_t>{1})
    {}

    explicit SharedPtr(T *ptr)
        : pointer(ptr), totalUse(new std::atomic<size_t>{1})
    {}

    SharedPtr(const SharedPtr &shared)
        : totalUse(nullptr)
    {
        *this = shared;
    }

    SharedPtr(SharedPtr &&shared) noexcept
        : totalUse(nullptr)
    {
        *this = std::move(shared);
    }

    SharedPtr &operator=(const SharedPtr &shared)
    {
        if (&shared == this) {
            return *this;
        }

        this->~SharedPtr();

        pointer = shared.pointer;
        totalUse = shared.totalUse;
        (*totalUse)++;

        return *this;
    }

    SharedPtr &operator=(SharedPtr &&shared) noexcept
    {
        if (&shared == this) {
            return *this;
        }

        this->~SharedPtr();

        pointer = shared.pointer;
        totalUse = shared.totalUse;
        shared.pointer = nullptr;
        shared.totalUse = nullptr;

        return *this;
    }

    // проверяет, указывает ли указатель на объект
    explicit operator bool() const
    {
        return pointer != nullptr;
    }

    T &operator*() const
    {
        return *pointer;
    }

    T *operator->() const
    {
        return pointer;
    }

    T *get()
    {
        return pointer;
    }

    void reset()
    {
        *this = SharedPtr{};
    }

    void reset(T *ptr)
    {
        *this = SharedPtr{ptr};
    }

	std::swap;
    void swap(SharedPtr &shared)
    {
        if (&shared == this) {
            return;
        }
        swap(pointer, shared.pointer);
        swap(totalUse, shared.totalUse);
    }
    // возвращает количество объектов SharedPtr,
    // которые ссылаются на тот же управляемый объект
    [[nodiscard]] size_t use_count() const
    {
        return *totalUse;
    }

    ~SharedPtr()
    {
        if (!totalUse) {
            return;
        }

        if (--(*totalUse) == 0) {
            delete pointer;
            delete totalUse;
        }
    }

private:
    T *pointer = nullptr;
    std::atomic<size_t> *totalUse;
};
