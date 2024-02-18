
#set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
#set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /MANIFEST:NO")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /MANIFEST:NO")

add_executable(werctl "werctl.cpp" "resources.rc" "werctl.exe.manifest")
target_link_libraries(werctl win32base)
target_link_options(werctl PRIVATE "/MANIFEST:NO")
