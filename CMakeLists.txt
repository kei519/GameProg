cmake_minimum_required(VERSION 3.26)
project(GameProg CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    # 入力ファイルを UTF-8 で受け取る
    add_compile_options(/utf-8)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(src)
add_subdirectory(lib)
