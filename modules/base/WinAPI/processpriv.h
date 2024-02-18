// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_PROCESSPRIV_H
#define WINAPI_PROCESSPRIV_H

#include "processenvironment.h"

#include <Windows.h>

#include <optional>
#include <string>

namespace Win32
{

namespace Impl
{
struct ProcessPriv
{
  std::string executable_path;
  std::optional<ProcessEnvironment> environment;
  HANDLE handle = {};
};
} // namespace Impl

} // namespace Win32

#endif // WINAPI_PROCESSPRIV_H