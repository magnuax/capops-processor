cmake_minimum_required(VERSION 3.31)

project(backend VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# -----------------------------
# Backend sources
# -----------------------------
file(GLOB_RECURSE BACKEND_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(backend
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${BACKEND_SOURCES}
)

target_include_directories(backend PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# -----------------------------
# Testing (Catch2)
# -----------------------------
include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
)

add_executable(backend_tests ${TEST_SOURCES})
target_link_libraries(backend_tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(backend_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME backend_tests COMMAND backend_tests)




