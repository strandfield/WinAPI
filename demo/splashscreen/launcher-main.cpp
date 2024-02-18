// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "resource.h"

#include <WinAPI/splashscreen.h>
#include <WinAPI/launcher.h>

#include <iostream>

int main(int argc, char* argv[])
{
  Win32::SplashScreen ss{ ID_SPLASHIMAGE };
  Win32::Launcher l{ "MyApp", &ss };
  l.SetExecutableName("winapi-demo-app");
  l.PreventMultipleInstances();
  l.Run();

  int r = l.GetApplicationExitCode();

  if (r != 0)
  {
    std::cout << "app exited abnormally" << std::endl;
  }

  std::cout << "goodbye" << std::endl;
}
