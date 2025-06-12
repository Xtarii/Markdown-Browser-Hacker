cmake_minimum_required(VERSION 3.30)
project(hack)

set(CMAKE_CXX_STANDARD 20)

add_executable(injector main.cpp
        injector/Injector.cpp
        injector/Injector.h)
target_link_libraries(injector shlwapi)
target_link_libraries(injector -static)



# Hack Module
add_library(hack SHARED
        module/hack.cpp
        module/base/pch.cpp
        module/base/pch.h
        module/base/framework.h
        module/main.cpp
        module/main.h
        module/browser/DocumentManager.h
        module/browser/node/Node.h
        module/browser/DocumentManager.cpp
)
