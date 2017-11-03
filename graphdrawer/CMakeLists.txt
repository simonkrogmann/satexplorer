find_package(Boost REQUIRED COMPONENTS program_options)

set(target graphdrawer-bin)

set(sources
    main.cpp
)

set(includes
    
)

set(libraries
    OGDF
    Boost::program_options
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
