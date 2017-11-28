
set(target tseytintransform)

set(sources
    main.cpp
)

set(includes
)

set(libraries
    circuits
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
