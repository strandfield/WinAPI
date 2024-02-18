
file(GLOB LIB_HDR_FILES "WinAPI/*.h")
file(GLOB LIB_SRC_FILES "WinAPI/*.cpp")

add_library(win32launcher STATIC ${LIB_HDR_FILES} ${LIB_SRC_FILES})
target_include_directories(win32launcher PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(win32launcher win32base)
target_link_libraries(win32launcher windowscodecs)
