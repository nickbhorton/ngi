add_library(ngi_ttf)

target_sources(
    ngi_ttf 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ttf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ttf_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ttf_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/bezier.h
    ${CMAKE_CURRENT_SOURCE_DIR}/bezier.cpp
)

target_include_directories(
    ngi_ttf
    PRIVATE
    ${CMAKE_SOURCE_DIR}/arrayalgebra
)
