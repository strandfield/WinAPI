// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_EVENTIMPL_H
#define WINAPI_EVENTIMPL_H

#include <Windows.h>

namespace Win32
{

namespace Impl
{

struct EventPriv
{
  std::string name;
  HANDLE handle = nullptr;
  bool created = false;
};

} // namespace Impl

class Event;

HANDLE GetHANDLE(const Event& e);

} // namespace Win32

#endif // WINAPI_EVENTIMPL_H