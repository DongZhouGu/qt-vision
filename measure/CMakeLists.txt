# 设置项目名,项目名即为target名
SET(execName detect)
PROJECT(${execName})

# 设置最低版本的cmake要求
cmake_minimum_required(VERSION 3.1)

# 指定使用的编译器
set(CMAKE_C_COMPILER "/usr/bin/gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

# 设置 C语言标准
set(CMAKE_C_STANDARD 99)  # 90, 99, 11
# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)  # 98, 11, 14, 17, 20.

# 设置编译模式,debug会输出调试信息,Debug版会使用参数-g；Release版使用-O3 –DNDEBUG
#set(CMAKE_BUILD_TYPE "Debug")
#set( CMAKE_BUILD_TYPE "Release" )

# 添加头文件路径
include_directories(./include )

link_directories(${OpenCV_LIBRARY_DIRS})

find_package(OpenCV REQUIRED)
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV library not found")
else()
    include_directories(${OpenCV_INCLUDE_DIRS})  # opencv添加头文件
endif()



# 查找当前目录中的所有源文件，并将结果列表存放在变量SRC_FILES中
aux_source_directory(./src SRC_FILES)
aux_source_directory(. SRC_FILES)

# 主函数生成可执行文件
add_executable(${execName}  ${SRC_FILES} )

target_link_libraries(${execName} ${OpenCV_LIBS})

