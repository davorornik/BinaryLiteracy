cmake_minimum_required(VERSION 3.16)
project(BinaryLiteracy)
set(CMAKE_CXX_STANDARD 20)

Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(BinaryLiteracy main.cpp)
target_link_libraries(BinaryLiteracy Catch2::Catch2WithMain)