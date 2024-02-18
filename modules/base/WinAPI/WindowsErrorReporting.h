// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_WER_H
#define WINAPI_WER_H

#include <string>

namespace Win32
{

/**
 * \brief provides functions related to Windows Error Reporting local dumps system
 * 
 * For more information, please consult: https://learn.microsoft.com/en-us/windows/win32/wer/collecting-user-mode-dumps
 */
class WindowsErrorReporting
{
public:
  enum DumpType {
    MiniDump = 1,
    FullDump = 2,
  };

  static bool IsEnabled(const std::string& exename);
  static void Disable(const std::string& exename);
  static void Enable(const std::string& exename);
  static void Enable(const std::string& exename, const std::string& dumpFolder, DumpType dumpType, int dumpCount);

};

} // namespace Win32

#endif // WINAPI_WER_H