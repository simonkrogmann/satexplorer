find_package(Boost REQUIRED COMPONENTS program_options)

set(target graphdrawer-bin)

set(sources
    main.cpp
    cfg.cpp
)

set(includes
    cfg.hpp
)

set(libraries
    ogdfWrapper
    Boost::program_options
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
