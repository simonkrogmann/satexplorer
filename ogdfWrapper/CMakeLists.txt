set(libname
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

add_library(${libname} "${sources}" "${includes}")
target_link_libraries(${libname} PUBLIC "${libraries}")
target_include_directories (${libname} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
