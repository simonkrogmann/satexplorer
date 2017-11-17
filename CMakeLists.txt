cmake_minimum_required (VERSION 2.8)
set(project satexplorer)
project(${project})
set("${project}_VERSION" 0.1)

function(create_library project name sources includes libraries)
    set(files ${sources} ${includes})
    add_library(${name} ${files})
    target_compile_options(${name} PRIVATE ${DEFAULT_COMPILE_FLAGS})
    target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(${name} PUBLIC ${libraries})
    install (TARGETS ${name} EXPORT ${project}Targets DESTINATION lib)
    install (DIRECTORY ${CMAKE_CURRENT_LIST_DIR} DESTINATION include/${project} FILES_MATCHING REGEX ".*\\.h(pp)?")
endfunction()

include_directories(SYSTEM ${CMAKE_SOURCE_DIR} 
    graphdrawer
    satviewer)

function(create_executable project name sources includes libraries)
    set(files ${sources} ${includes})
    add_executable(${name} ${files})
    target_compile_options(${name} PRIVATE ${DEFAULT_COMPILE_FLAGS})
    target_link_libraries(${name} PUBLIC ${libraries})
    install (TARGETS ${name} EXPORT ${project}Targets DESTINATION bin)
endfunction()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wfloat-equal -Wold-style-cast -Wno-sign-compare")
if (CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
# endif()

add_subdirectory(graphdrawer)
add_subdirectory(satviewer)
