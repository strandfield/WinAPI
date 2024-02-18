// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Process.h"
#include "processpriv.h"

#include "String.h"

#include <shlwapi.h>

#include <iostream>
#include <numeric>
#include <optional>
#include <vector>

namespace Win32
{

Process::Process()
  : d(std::make_unique<Impl::ProcessPriv>())
{

}

Process::Process(std::unique_ptr<Impl::ProcessPriv> pd)
  : d(std::move(pd))
{

}

Process::~Process()
{

}

/**
 * \brief sets the executable path
 * \param exePath  path to the executable
 */
void Process::SetExecutablePath(std::string exePath)
{
  d->executable_path = std::move(exePath);
}

/**
 * \brief sets the environment variables for the process
 * \param penv  the environment variables
 * 
 * Setting a ProcessEnvironment is optional. 
 * If none is set, the new process will use the environment of its parent.
 */
void Process::SetProcessEnvironment(ProcessEnvironment penv)
{
  d->environment = std::move(penv);
}

/**
 * \brief stats the process
 */
void Process::Start()
{
  if (d->executable_path.empty())
    return;

  TCHAR szCurrentFolder[MAX_PATH] = { 0 };
  GetModuleFileName(NULL, szCurrentFolder, MAX_PATH);
  PathRemoveFileSpec(szCurrentFolder);

  // start the application
  STARTUPINFO si = { 0 };
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi = { 0 };
  constexpr bool inherit_handles = false;
  constexpr DWORD creation_flags = 0;

  std::vector<char> envdata;
  LPVOID environment = nullptr;
  if (d->environment.has_value())
  {
    const ProcessEnvironment& penv = d->environment.value();
    std::vector<std::string> variables = penv.ToStringList();
    size_t nbchars = std::accumulate(variables.begin(), variables.end(), size_t(0), [](size_t n, const std::string& str) -> size_t {
      return n + str.size() + 1;
      });
    envdata.reserve(nbchars + 1);
    for (const std::string& str : variables) {
      envdata.insert(envdata.end(), str.begin(), str.end());
      envdata.push_back('\0');
    }
    envdata.push_back('\0');

    environment = envdata.data();
  }
  
  CreateProcess(d->executable_path.c_str(), NULL, NULL, NULL, inherit_handles, creation_flags, environment, szCurrentFolder, &si, &pi);

  d->handle = pi.hProcess;
}

/**
 * \brief waits for the process to be finished
 */
void Process::WaitForFinished()
{
  WaitForSingleObject(d->handle, INFINITE);
}

/**
 * \brief returns the exit code of the process
 */
int Process::GetExitCode() const
{
  DWORD val = 0;
  GetExitCodeProcess(d->handle, &val);
  return val;
}

/**
 * \brief returns the path of the executable of the current process
 */
std::string Process::GetExecutablePath()
{
  auto wpath = std::wstring(1024, wchar_t(0));
  DWORD charsWritten = ::GetModuleFileNameW(
    NULL,
    wpath.data(),
    static_cast<DWORD>(wpath.size()));
  wpath.resize(charsWritten);
  return ToUtf8(wpath);
}

Impl::ProcessPriv* Process::GetImpl() const
{
  return d.get();
}

Process LaunchProcess(const std::string& executable_path)
{
  TCHAR szCurrentFolder[MAX_PATH] = { 0 };
  GetModuleFileName(NULL, szCurrentFolder, MAX_PATH);
  PathRemoveFileSpec(szCurrentFolder);

  // add the application name to the path
  //TCHAR szApplicationPath[MAX_PATH];
  //PathCombine(szApplicationPath, szCurrentFolder, executable_path.c_str());

  // start the application
  STARTUPINFO si = { 0 };
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi = { 0 };
  //CreateProcess(szApplicationPath, NULL, NULL, NULL, FALSE, 0, NULL, szCurrentFolder, &si, &pi);
  constexpr bool inherit_handles = false;
  constexpr DWORD creation_flags = 0;
  LPVOID environment = nullptr;
  CreateProcess(executable_path.c_str(), NULL, NULL, NULL, inherit_handles, creation_flags, environment, szCurrentFolder, &si, &pi);

  Impl::ProcessPriv pd;
  pd.handle = pi.hProcess;
  pd.executable_path = executable_path;
  return Process(std::make_unique<Impl::ProcessPriv>(pd));
}

} // namespace Win32