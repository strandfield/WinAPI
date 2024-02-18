// Copyright (C) 2024 Vincent Chambrin
// This file is part of the WinAPI project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include <WinAPI/WindowsErrorReporting.h>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::string> get_args(int argc, char** argv)
{
  std::vector<std::string> r;
  r.reserve(argc);

  while (--argc >= 0)
  {
    r.push_back(std::string(*(argv++)));
  }

  return r;
}

void help()
{
  std::cout << "Enable or disable Windows Error Reporting local dumpts for an executable." << std::endl;
  std::cout << std::endl;
  std::cout << "Check the status:" << std::endl;
  std::cout << "    werctl status <executable>" << std::endl;
  std::cout << "Enable local dumps:" << std::endl;
  std::cout << "    werctl enable <executable>" << std::endl;
  std::cout << "Disable local dumps:" << std::endl;
  std::cout << "    werctl status <executable>" << std::endl;
}

void status(const std::vector<std::string>& args)
{
  if (args.empty())
  {
    std::cout << "missing program name after 'status'" << std::endl;
    std::exit(1);
  }

  std::string exename = args.front();
  bool on = Win32::WindowsErrorReporting::IsEnabled(exename);

  std::cout << "Windows Error Reporting local dumps are "
    << (on ? "enabled" : "NOT activated") << " for " << exename << std::endl;
}

void enable(std::vector<std::string> args)
{
  if (args.empty())
  {
    std::cout << "missing program name after 'enable'" << std::endl;
    std::exit(1);
  }

  std::string exename = args.front();

  try
  {
    Win32::WindowsErrorReporting::Enable(exename);
  }
  catch (const std::exception& ex)
  {
    std::cerr << ex.what() << std::endl;
    std::exit(1);
  }
}

void disable(const std::vector<std::string>& args)
{
  if (args.empty())
  {
    std::cout << "missing program name after 'disable'" << std::endl;
    std::exit(1);
  }
  else
  {
    std::string exename = args.front();

    try
    {
      Win32::WindowsErrorReporting::Disable(exename);
    }
    catch (const std::exception& ex)
    {
      std::cerr << ex.what() << std::endl;
      std::exit(1);
    }
  }
}

int main(int argc, char* argv[])
{
  std::vector<std::string> args = get_args(argc, argv);

  if (args.size() < 2 || args[1] == "--help" || args[1] == "-h")
  {
    help();
    return 0;
  }

  if (args[1] == "status")
  {
    status(std::vector<std::string>(args.begin() + 2, args.end()));
  }
  else if (args[1] == "enable")
  {
    enable(std::vector<std::string>(args.begin() + 2, args.end()));
  }
  else if (args[1] == "disable")
  {
    disable(std::vector<std::string>(args.begin() + 2, args.end()));
  }
  else
  {
    std::cout << "unrecognized command: " << args[1] << std::endl;
    return 1;
  }

  return 0;
}
