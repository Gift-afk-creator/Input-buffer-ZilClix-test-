cmake_minimum_required(VERSION 3.21)
project(ZilClixInputBuffer VERSION 0.1.0 LANGUAGES CXX)

if (NOT DEFINED GEODE_SDK)
    message(FATAL_ERROR "GEODE_SDK is not set. Build this project with the Geode CLI or Geode SDK toolchain.")
endif()

add_subdirectory(${GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
add_library(${PROJECT_NAME} SHARED src/main.cpp)
setup_geode_mod(${PROJECT_NAME})
