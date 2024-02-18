// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef WINAPI_SPLASHSCREEN_H
#define WINAPI_SPLASHSCREEN_H

#include <memory>
#include <string>
#include <variant>

namespace Win32
{

class Event;

using resource_id = std::variant<int, std::string>;

namespace Impl
{
struct SplashScreenPriv;
} // namespace Impl

/**
 * \brief provides a splashscreen
 */
class SplashScreen
{
public:
  explicit SplashScreen(const resource_id& res_id);
  ~SplashScreen();

  bool CreateCloseEvent(const std::string& name);
  Event& GetCloseEvent() const;

  void Show();
  void Close();

  Impl::SplashScreenPriv* GetImpl() const;

private:
  std::unique_ptr<Impl::SplashScreenPriv> d;
};

void CloseSplashScreen(const std::string& appname);

} // namespace Win32

#endif // WINAPI_SPLASHSCREEN_H