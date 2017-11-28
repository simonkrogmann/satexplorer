find_package(Qt5Widgets)
find_package(Qt5Svg)
find_package(Qt5Core)
find_package(Qt5Gui)

set(target satviewer-bin)

set(sources
    main.cpp
    SatWindow.cpp
    SatWindow.hpp
    Stepper.cpp
    Stepper.hpp
)

set(includes
)

set(libraries
    ogdfWrapper
    Qt5::Widgets
    Qt5::Svg
    Qt5::Core
    Qt5::Gui
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
