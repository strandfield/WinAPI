// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_COMPTR_H
#define WINAPI_COMPTR_H

// A modern C++ implementation of atlbase.h' CComPtr.

#include <Unknwnbase.h>

#include <type_traits>

namespace Win32
{

template<typename T>
class com_ptr
{
private:
  T* p;

public:
  static_assert(std::is_base_of<IUnknown, T>::value, "T must be derived from IUnknown");

public:
  com_ptr() : p(nullptr) { }

  com_ptr(const com_ptr<T>& other) : p(other.p) {
    if (p) p->AddRef();
  }

  com_ptr(com_ptr<T>&& other) : p(other.p) {
    other.p = nullptr;
  }

  ~com_ptr() {
    if (p) p->Release();
  }

  template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
  com_ptr(const com_ptr<U>& other) : p(other.p) {
    if (p) p->AddRef();
  }

  template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
  explicit com_ptr(U* ptr) : p(ptr) { }

  com_ptr<T>& operator=(const com_ptr<T>& other) {
    if (p == other.p) return *this;
    if (p) p->Release();
    p = other.p;
    if (p) p->AddRef();
    return *this;
  }

  com_ptr<T>& operator=(com_ptr<T>&& other) {
    if (p == other.p) return *this;
    if (p) p->Release();
    p = other.p;
    other.p = nullptr;
    return *this;
  }

  T* operator->() const {
    return p;
  }

  T& operator*() const {
    return *p;
  }

  bool operator!() const {
    return !p;
  }

  operator bool() const {
    return p;
  }

  T* get() const {
    return p;
  }
};

} // namespace Win32

#endif // WINAPI_COMPTR_H