
file(GLOB LIB_HDR_FILES "WinAPI/*.h")
file(GLOB LIB_SRC_FILES "WinAPI/*.cpp")

add_library(win32base STATIC ${LIB_HDR_FILES} ${LIB_SRC_FILES})
target_include_directories(win32base PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(win32base Shlwapi)
