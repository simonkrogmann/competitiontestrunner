cmake_minimum_required (VERSION 3.0)
set(project competitiontestrunner)
project(${project})

set(use_utilgpu_compilation_options ON)
find_package(utilgpu REQUIRED)
find_package(Qt5Core REQUIRED)

add_subdirectory(testrunner)
add_subdirectory(filecreator)
