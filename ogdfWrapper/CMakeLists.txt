set(target
    ogdfWrapper
)

set(sources
    ogdfWrapper.cpp
)

set(includes
    types.hpp
    ogdfWrapper.hpp
)

set(libraries
    OGDF
)

create_library(${project} ${target} "${sources}" "${includes}" "${libraries}")

