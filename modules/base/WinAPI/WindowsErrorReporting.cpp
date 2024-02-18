// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "WindowsErrorReporting.h"

#include "ErrorCode.h"
#include "Registry.h"

namespace Win32
{

/**
 * \brief returns whether local dumps are enabled for a specific application
 * 
 * Local dumps are an opt-in functionnality of the Windows Error Reporting system,
 * so by default this function returns false.
 */
bool WindowsErrorReporting::IsEnabled(const std::string& exename)
{
  RegistryKey rk;
  ErrorCode err = rk.TryOpen(Registry::HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\" + exename,
    Registry::Read);
  return !err;
}

/**
* \brief disable local dumps for a specific application
*/
void WindowsErrorReporting::Disable(const std::string& exename)
{
  Registry::DeleteKey(Registry::HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\" + exename);
}

/**
 * \brief enable local dumps for a specific application
 */
void WindowsErrorReporting::Enable(const std::string& exename)
{
  RegistryKey rk = Registry::CreateKey(Registry::HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\" + exename,
    Registry::Write);
}

/**
 * \brief enable local dumps for a specific application
 * \param exename     the name of the executable (including the extension)
 * \param dumpFolder  folder in which the dumps should be written
 * \param dumpType    type of the dumps
 * \param dumpCount   maximum number of dumps that should be kept on disk
 */
void WindowsErrorReporting::Enable(const std::string& exename, const std::string& dumpFolder, DumpType dumpType, int dumpCount)
{
  RegistryKey rk = Registry::CreateKey(Registry::HKEY_LOCAL_MACHINE,
    "SOFTWARE\\Microsoft\\Windows\\Windows Error Reporting\\LocalDumps\\" + exename,
    Registry::Write);

  rk.SetValue("DumpFolder", dumpFolder);
  rk.SetValue("DumpType", static_cast<int>(dumpType));
  rk.SetValue("DumpCount", dumpCount);
}

} // namespace Win32