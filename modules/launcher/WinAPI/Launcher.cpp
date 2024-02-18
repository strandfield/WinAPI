// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "Launcher.h"

#include "splashscreen_priv.h"

#include "WinAPI/Event.h"
#include "WinAPI/EventImpl.h"
#include "WinAPI/ProcessEnvironment.h"
#include "WinAPI/Process.h"
#include "WinAPI/processpriv.h"

#include <Windows.h>
#include <shlwapi.h>

#include <filesystem>
#include <numeric>
#include <optional>
#include <vector>

#include <iostream>

namespace Win32
{

namespace Impl
{

std::string compute_close_event_name(const std::string& appname, bool single_instance)
{
  std::string ev_name = appname + "CloseSplashScreenEvent";

  if (!single_instance) {
    ev_name += std::to_string(GetTickCount());
  }

  return ev_name;
}

struct LauncherPriv
{
  std::string appname;
  SplashScreen* ss = nullptr;
  std::string executable_name;
  std::string executable_path;
  bool single_instance = false;
  Event single_instance_event;
  int app_exit_code = 0;
};

} // namespace Impl

/**
 * \brief construct an application launcher
 * \param appname  name of the application
 * \param ss       an optional splashscreen
 * 
 * \note \a appname is a generic name for the application and can be different 
 * from the executable name that you set with SetExecutableName().
 * This application name is used to prevent multiple instances of the application 
 * when PreventMultipleInstances() is called.
 */
Launcher::Launcher(const std::string& appname, SplashScreen* ss)
  : d(std::make_unique<Impl::LauncherPriv>())
{
  d->appname = appname;
  d->ss = ss;
}

Launcher::~Launcher()
{
  
}

/**
 * \brief sets the name of the executable
 * \param exeName  the name of the executable (without the extension)
 * 
 * The executable must be in the same directory as the executable of the current process.
 * 
 * Alternatively, you may specify a full path using SetExecutablePath().
 */
void Launcher::SetExecutableName(std::string exeName)
{
  d->executable_name = std::move(exeName);
}

/**
 * \brief sets the full path of the executable
 * \param exePath absolute file path to the executable (with the extension)
 */
void Launcher::SetExecutablePath(std::string exePath)
{
  d->executable_path = std::move(exePath);
}

/**
 * \brief prevent multiple instances of the application
 */
void Launcher::PreventMultipleInstances()
{
  d->single_instance = true;
}

/**
 * \brief runs the application and wait for it to be finished
 * 
 * If a splashscreen was passed to the constructor, it is shown until the application
 * requests the splashscreen to be hidden.
 * 
 * If PreventMultipleInstances() was called, and an instance of the application 
 * is already running, this function does not start a new instance and returns immediately.
 */
void Launcher::Run()
{
  std::vector<HANDLE> handles;

  if (d->single_instance)
  {
    d->single_instance_event = Event{ d->appname + "InstanceRunning" };

    if (!d->single_instance_event.Created()) {
      std::cout << "app already running, exiting..." << std::endl;
      return;
    }
  }

  if (d->ss)
  {
    std::string closeEventName = Impl::compute_close_event_name(d->appname, d->single_instance);

    if (d->ss->CreateCloseEvent(closeEventName)) {
      d->ss->Show();
      handles.push_back(GetHANDLE(d->ss->GetCloseEvent()));
    }
    else {
      // event creation failed for some reasons...
      std::cout << "splashscreen close event creation failed, no splashscreen will be shown" << std::endl;
    }
  }

  std::string exe_path = d->executable_path;

  if (exe_path.empty()) {
    if (!d->executable_name.empty()) {
      exe_path = d->executable_name + ".exe";
    } else {
      exe_path = d->appname + ".exe";
    }

    auto myfolder = std::filesystem::path(Process::GetExecutablePath()).parent_path();
    exe_path = (myfolder / std::filesystem::path(exe_path)).u8string();
  }

  Process p;
  p.SetExecutablePath(exe_path);

  if (d->ss && !d->single_instance) {
    auto penv = ProcessEnvironment::GetSystemEnvironment();
    penv.Insert("CLOSE_SPLASHSCREEN_EVENT_NAME", d->ss->GetCloseEvent().GetName());
    p.SetProcessEnvironment(std::move(penv));
  }

  p.Start();

  if (p.GetImpl()->handle == nullptr) {
    return;
  }

  AllowSetForegroundWindow(GetProcessId(p.GetImpl()->handle));
  handles.push_back(p.GetImpl()->handle);

  for (;;)
  {
    constexpr DWORD timeout = INFINITE;
    constexpr bool wait_all = false;
    DWORD wait_result = ::MsgWaitForMultipleObjects(
      static_cast<DWORD>(handles.size()), 
      handles.data(), 
      wait_all, 
      timeout, 
      QS_ALLINPUT);

    if (wait_result == WAIT_FAILED)
    {
      std::cout << "wait failed " << GetLastError() << std::endl;
    }
    else
    {
      if (wait_result == WAIT_OBJECT_0 + handles.size())
      {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE)
        {
          if (msg.message == WM_QUIT)
          {
            PostQuitMessage((int)msg.wParam);
            return;
          }

          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
      else
      {
        size_t handle_index = wait_result - WAIT_OBJECT_0;
        
        if (handle_index < handles.size())
        {
          HANDLE h = handles.at(handle_index);

          if (h == p.GetImpl()->handle)
          {
            // process has exited, return
            d->app_exit_code = p.GetExitCode();
            return;
          }
          else if (d->ss && !d->ss->GetImpl()->close_event.IsNull())
          {
            // received request to close splash screen
            d->ss->Close();
            handles.erase(handles.begin() + handle_index);
          }
        }
      }
    }
  }
}

/**
 * \brief returns the exit code of the application
 */
int Launcher::GetApplicationExitCode() const
{
  return d->app_exit_code;
}

} // namespace Win32