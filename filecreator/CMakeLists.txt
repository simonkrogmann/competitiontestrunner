set(target createfiles)

set(sources
    main.cpp
)

set(includes)

set(resources
    template/template.cpp
    template/template.py
)

set(libraries
    utilgpu::cpp utilgpu::qt
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
generate_resource_header(${project} ${target} "${resources}")
