add_executable(ttf_parse)

target_sources(
    ttf_parse 
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ttf_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bezier.h
)

target_include_directories(
    ttf_parse
    PRIVATE
    ${CMAKE_SOURCE_DIR}/arrayalgebra
)
