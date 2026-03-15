cmake_minimum_required(VERSION 3.28.0)

# Name of the project
project(gdiff)

include(.cuark/TheBasics.cmake)

# Find Conan2 generated libraries
include(.cuark/ConanLibImports.cmake)

include(.cuark/Extras.cmake)

# The actual C++ project lives in the cpp/ subdirectory
add_subdirectory(src)
