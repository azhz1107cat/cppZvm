cmake_minimum_required(VERSION 3.12)
project(cppZvm)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Python 3.12
find_package(Python3 3.12 EXACT COMPONENTS Interpreter Development REQUIRED)

# Python配置信息
message(STATUS "Python3 executable: ${Python3_EXECUTABLE}")
message(STATUS "Python3 include dirs: ${Python3_INCLUDE_DIRS}")
message(STATUS "Python3 libraries: ${Python3_LIBRARIES}")

# 手动指定Python头文件路径
set(PYTHON312_INCLUDE_DIR "C:/Program Files/Python312/include")
message(STATUS "Manual Python include dir: ${PYTHON312_INCLUDE_DIR}")

if(NOT EXISTS ${PYTHON312_INCLUDE_DIR}/Python.h)
    message(FATAL_ERROR "Python.h not found at ${PYTHON312_INCLUDE_DIR}")
endif()

# 配置pybind11
set(PYBIND11_DIR ${CMAKE_SOURCE_DIR}/include/pybind11)
message(STATUS "pybind11 directory: ${PYBIND11_DIR}")

if(NOT EXISTS ${PYBIND11_DIR}/pybind11.h)
    message(FATAL_ERROR "pybind11 not found at ${PYBIND11_DIR}")
endif()

add_library(pybind11 INTERFACE)
target_include_directories(pybind11 INTERFACE ${PYBIND11_DIR})

# 生成Python模块（.pyd文件）
add_library(cppZvm MODULE main.cpp)
target_include_directories(cppZvm PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${PYTHON312_INCLUDE_DIR}
)
target_link_libraries(cppZvm PRIVATE
        Python3::Python
        pybind11
)

# 设置模块属性
if(WIN32)
    set_target_properties(cppZvm PROPERTIES
            PREFIX ""
            SUFFIX ".pyd"
            LINK_FLAGS "-shared"
    )

    # 关键：复制MinGW运行时库到输出目录
    # 替换为你的MinGW bin目录（通常在CLion的安装目录下）
    set(MINGW_BIN_DIR "C:/Program Files/JetBrains/CLion 2025.1.3/bin/mingw/bin")
    message(STATUS "MinGW bin directory: ${MINGW_BIN_DIR}")

    # 复制缺失的libgcc和libstdc++库
    add_custom_command(TARGET cppZvm POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            $<TARGET_FILE_DIR:cppZvm>  # .pyd文件所在目录
    )
endif()