cmake_minimum_required (VERSION 3.0)
set(project competition-test-runner)
project(${project})

set(use_utilgpu_compilation_options ON)
find_package(utilgpu REQUIRED)
find_package(Qt5Core REQUIRED)

set(target test-runner)

set(sources
    test-runner.cpp
)

set(includes
    language.h
)

set(resources)

set(libraries
    utilgpu::cpp utilgpu::qt
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
