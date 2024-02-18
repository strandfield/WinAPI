// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <WinAPI/splashscreen.h>

#include <WinAPI/String.h>

#include <Windows.h>

#include <chrono>
#include <iostream>
#include <thread>

int main(int argc, char* argv[])
{
  std::cout << "hello" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  Win32::CloseSplashScreen("MyApp");
  //std::this_thread::sleep_for(std::chrono::seconds(2));
  
  ::MessageBoxW(nullptr, L"Press OK", L"Hi", MB_SETFOREGROUND);

  //system("pause");
  std::cout << "goodbye" << std::endl;
}
