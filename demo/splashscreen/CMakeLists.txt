
add_executable(winapi-demo-launcher "launcher-main.cpp" "resource.rc")
target_link_libraries(winapi-demo-launcher win32launcher)

add_executable(winapi-demo-app "app-main.cpp" "resource.rc")
target_link_libraries(winapi-demo-app win32launcher)
target_link_options(winapi-demo-app PRIVATE "/SUBSYSTEM:WINDOWS")
target_link_options(winapi-demo-app PRIVATE "/ENTRY:mainCRTStartup")