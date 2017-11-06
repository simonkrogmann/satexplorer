find_package(Boost REQUIRED COMPONENTS program_options)

set(target graphdrawer-bin)

set(sources
    main.cpp
    cfg.cpp
    ogdfWrapper.cpp
)

set(includes
    types.hpp
    cfg.hpp
    ogdfWrapper.hpp
)

set(libraries
    OGDF
    Boost::program_options
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
