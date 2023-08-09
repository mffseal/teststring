cmake_minimum_required(VERSION 3.20)
project(teststring)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")

add_compile_options(-mavx2)
add_compile_options(-mpclmul)

add_executable(teststring main.cpp table_str.cc SwiftJson.cpp)

add_subdirectory(fmt)
target_link_libraries(teststring PRIVATE fmt::fmt)
target_include_directories(teststring PRIVATE ${PROJECT_SOURCE_DIR}/sonic-cpp/include)

