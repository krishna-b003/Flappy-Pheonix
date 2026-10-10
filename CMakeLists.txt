
cmake_minimum_required(VERSION 3.20)

project(FlappyBird LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(FlappyBird main.cpp)

target_link_libraries(FlappyBird
    PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
