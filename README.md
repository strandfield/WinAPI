
# `WinAPI` - a C++17 wrapper around some functions of the Win32 API

The goal of this project is twofold:
- provide basic functionalities that are not directly available in the C++ standard (e.g., starting a process);
- wrap some Windows-specific functionalities (e.g., modifying the Windows Registry) in a friendly C++ interface.

A secondary objective is to be able to do all the above without having to include `<Windows.h>` most
of the time, as this header uses a lot of `#define` and does other things that are considered 
good (C++) practices.

## What's in there

### Modules

The `modules` directory contains the source code for several static libraries.

#### base

Wraps some functions of the Win32 API.

- Conversion between UTF-8 (`std::string`) and UTF-16 (`std::wstring`) are provided in `<WinAPI/String.h>`
- A function for getting an error message from an error code (as returned by `GetLastError()`) is provided in `<WinAPI/ErrorMessage.h>`
- Facilities to start a process are provided in `<WinAPI/Process.h>`
- Header `<WinAPI/Event.h>` provides a class for creating and manipulating events.
- Facilities for manipulating the Windows Registry are provided in `<WinAPI/Registry.h>`

### launcher

A library, built on top of the `base` module that provides the means to create an application 
launcher.

Supported features:
- prevent multiple instances of the application
- display a splashscreen (`.bmp` or `.png`)

### Apps

The `apps` directory will be used for standalone programs.

#### werctl

A command-line tool that can be used to easily activate or deactivate the 
generation of local dumps by the Windows Error Reporting system for an application.

## Building

This project uses the CMake build system. 

It requires a compiler with C++17 support but otherwise does not 
depend on any external libraries and only links to Windows system libraries.

## License

The project is release under the MIT license.
