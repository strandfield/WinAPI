// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "processenvironment.h"

#include <Windows.h>

namespace Win32
{

/**
 * \brief reads the environment variables for the current process
 */
ProcessEnvironment ProcessEnvironment::GetSystemEnvironment()
{
  ProcessEnvironment r;

  // format for 'data' is "Var1=Value1\0Var2=Value2\0VarN=ValueN\0\0"
  char* data = GetEnvironmentStrings();

  const char* begin = data;

  while (*begin != '\0')
  {
    const char* ptr = begin;

    while (*(++ptr) != '\0');

    const char* eq = std::find(begin, ptr, '=');

    auto name = std::string(begin, eq);
    auto val = std::string(eq + 1, ptr);
    r.Insert(name, val);

    begin = ptr + 1;
  }

  FreeEnvironmentStrings(data);

  return r;
}

} // namespace Win32