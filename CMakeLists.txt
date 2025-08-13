cmake_minimum_required(VERSION 3.12)
project(cppZvm)

# 基础配置：强制64位和C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_GENERATOR "MinGW Makefiles")
set(CMAKE_GENERATOR_PLATFORM x64)

# 编译器路径（绝对路径，无多余空格）
set(MINGW_BIN_DIR "C:/Program Files/JetBrains/CLion 2025.1.3/bin/mingw/bin")
set(CMAKE_C_COMPILER "${MINGW_BIN_DIR}/gcc.exe")
set(CMAKE_CXX_COMPILER "${MINGW_BIN_DIR}/g++.exe")
set(CMAKE_MAKE_PROGRAM "${MINGW_BIN_DIR}/mingw32-make.exe")

# 验证编译器存在
if(NOT EXISTS "${CMAKE_C_COMPILER}")
    message(FATAL_ERROR "C compiler missing: ${CMAKE_C_COMPILER}")
endif()
if(NOT EXISTS "${CMAKE_CXX_COMPILER}")
    message(FATAL_ERROR "C++ compiler missing: ${CMAKE_CXX_COMPILER}")
endif()

# 编译选项（单行，无注释，无多余空格）
set(CMAKE_CXX_FLAGS "-m64")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -DNDEBUG")

# Python配置
find_package(Python3 3.12 EXACT COMPONENTS Interpreter Development REQUIRED)
set(PYTHON312_INCLUDE_DIR "C:/Program Files/Python312/include")
if(NOT EXISTS "${PYTHON312_INCLUDE_DIR}/Python.h")
    message(FATAL_ERROR "Python.h not found: ${PYTHON312_INCLUDE_DIR}")
endif()

# pybind11配置
set(PYBIND11_DIR "${CMAKE_SOURCE_DIR}/include/pybind11")
if(NOT EXISTS "${PYBIND11_DIR}/pybind11.h")
    message(FATAL_ERROR "pybind11 missing: ${PYBIND11_DIR}")
endif()
add_library(pybind11 INTERFACE)
target_include_directories(pybind11 INTERFACE "${PYBIND11_DIR}")

# 目标：生成Python模块
add_library(cppZvm MODULE
        main.cpp
        include/builtins/builtins_functions.hpp
        include/models/Errors.hpp
        include/utils/Utils.hpp
        include/utils/SLL_loader.hpp
        include/vm_deps/VmModels.hpp
        include/builtins/builtins_type.hpp
        include/vm_deps/vm_ctor.hpp
        include/vm_deps/CallFrame.hpp
)

# 目标属性（无多余空格和换行）
target_include_directories(cppZvm PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
        "${PYTHON312_INCLUDE_DIR}"
        "${Python3_INCLUDE_DIRS}"
)
target_link_libraries(cppZvm PRIVATE Python3::Python pybind11)

# Windows特定配置（严格单行或规范换行）
if(WIN32)
    set_target_properties(cppZvm PROPERTIES
            PREFIX ""
            SUFFIX ".pyd"
            LINK_FLAGS "-shared"
    )
    if(EXISTS "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll")
        add_custom_command(TARGET cppZvm POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
                "${MINGW_BIN_DIR}/libstdc++-6.dll"
                "$<TARGET_FILE_DIR:cppZvm>"
        )
    endif()
endif()

# 调试信息
message(STATUS "C++ Debug Flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Output Dir: ${CMAKE_CURRENT_BINARY_DIR}")