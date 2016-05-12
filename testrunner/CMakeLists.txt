set(target runtests)

set(sources
    main.cpp
)

set(includes
    language.h
)

set(resources)

set(libraries
    utilgpu::cpp utilgpu::qt
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
