// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_PROCESS_H
#define WINAPI_PROCESS_H

#include <memory>
#include <string>

namespace Win32
{

class ProcessEnvironment;

namespace Impl
{
struct ProcessPriv;
} // namespace Impl

/**
 * \brief represents a process
 */
class Process
{
public:
  Process();
  Process(const Process&) = delete;
  Process(Process&&) = default;
  ~Process();

  explicit Process(std::unique_ptr<Impl::ProcessPriv> pd);

  void SetExecutablePath(std::string exe_path);
  void SetProcessEnvironment(ProcessEnvironment penv);

  void Start();

  void WaitForFinished();

  int GetExitCode() const;

  static std::string GetExecutablePath();

  Impl::ProcessPriv* GetImpl() const;

private:
  std::unique_ptr<Impl::ProcessPriv> d;
};

Process LaunchProcess(const std::string& executable_path);

} // namespace Win32

#endif // WINAPI_PROCESS_H