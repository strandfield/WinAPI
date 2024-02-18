// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_LAUNCHER_H
#define WINAPI_LAUNCHER_H

#include <memory>
#include <string>

namespace Win32
{

class SplashScreen;

namespace Impl
{
struct LauncherPriv;
} // namespace Impl

/**
 * \brief provides an application launcher
 * 
 * The launcher supports displaying a splashscreen and preventing multiple 
 * instances of the application.
 */
class Launcher
{
public:
  explicit Launcher(const std::string& appname, SplashScreen* ss = nullptr);
  ~Launcher();

  void SetExecutableName(std::string exe_name);
  void SetExecutablePath(std::string exe_path);

  void PreventMultipleInstances();

  void Run();

  int GetApplicationExitCode() const;

private:
  std::unique_ptr<Impl::LauncherPriv> d;
};

} // namespace Win32

#endif // WINAPI_LAUNCHER_H