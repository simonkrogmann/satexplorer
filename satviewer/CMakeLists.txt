find_package(Qt5 COMPONENTS Core Widgets Svg Gui)

set(target satviewer-bin)

set(sources
    main.cpp
    SatWindow.cpp
    Stepper.cpp
    SelectionSvgWidget.cpp
)

set(includes
    SatWindow.hpp
    Stepper.hpp
    SelectionSvgWidget.hpp
)

set(libraries
    ogdfWrapper
    Qt5::Widgets
    Qt5::Svg
    Qt5::Core
    Qt5::Gui
)

create_executable(${project} ${target} "${sources}" "${includes}" "${libraries}")
