// Copyright 2018 Your Name <your_email>

#ifndef INCLUDE_HEADER_HPP_
#define INCLUDE_HEADER_HPP_

#include <iostream>
#include <atomic>
#include <map>
#include <stdint.h>
#include <utility>

using std::int64_t;
using std::atomic_size_t;
using std::map;

using std::cout;
template <typename T>
class SharedPtr {
public:
    T* _adress = nullptr;
    SharedPtr();
explicit  SharedPtr(T* ptr)
    {
        _adress = ptr;
        if (_adresses.find(reinterpret_cast<int64_t>(ptr)) != _adresses.end())
        {++_adresses[reinterpret_cast<int64_t>(ptr)];}
        else
        {_adresses.insert({reinterpret_cast<int64_t>(ptr), 1});}
    };
explicit  SharedPtr(const SharedPtr& r)
{_adress = r._adress; ++_adresses[reinterpret_cast<int64_t>(_adress)];}
    SharedPtr(SharedPtr&& r)
    {
        _adress = r._adress; ++_adresses[reinterpret_cast<int64_t>(_adress)];
    }

    ~SharedPtr()
    {
        --_adresses[reinterpret_cast<int64_t>(_adress)];
        if (_adresses[reinterpret_cast<int64_t>(_adress)] == 0)
        {
            cout << "delete object";
            _adresses.erase(reinterpret_cast<int64_t>(_adress));
            delete(_adress);
        }
    }

   auto operator=(SharedPtr& r) -> SharedPtr&
   {
       if (_adress)
       {reset(r._adress);}
       return *this;
   }
   // auto opeartor=(SharedPtr&& r) -> SharedPtr&;

    // проверяет, указывает ли указатель на объект
    operator bool() const
    {
        if (!_adress){return false;}
        return true;
    }
    auto operator*() const -> T&
    {
        return *_adress;
    }
    auto operator->() const -> T*
    {
        return _adress;
    }

    auto get() -> T*
    {
        return _adress;
    }
    void reset()
    {
        --_adresses[reinterpret_cast<int64_t>(_adress)];
        _adress = nullptr;
    }
    void reset(T* ptr)
    {
        --_adresses[reinterpret_cast<int64_t>(_adress)];
        _adress = ptr;
        if (_adresses.find(reinterpret_cast<int64_t>(ptr)) != _adresses.end())
        {++_adresses[reinterpret_cast<int64_t>(ptr)];}
        else
        {_adresses.insert({reinterpret_cast<int64_t>(ptr), 1});}
    }
    void swap(SharedPtr& r)
    {
        T*tmp;
        tmp = r._adress;
        r._adress = _adress;
        _adress = tmp;
    }
    auto use_count() const -> size_t
    {
        return _adresses[reinterpret_cast<int64_t>(_adress)];
    }
    static map <int64_t , size_t > _adresses;
};
template <typename T>
map<int64_t, size_t> SharedPtr<T>::_adresses{};

#endif // INCLUDE_HEADER_HPP_
