add_library(ngi_aux)

target_sources(ngi_aux PRIVATE 
    glfw_wrapper.cpp
    camera.h camera.cpp
) 

target_include_directories(ngi_aux PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/arrayalgebra
)

target_link_libraries(ngi_aux PRIVATE ngi_glad)

if (NGI_LOG)
    target_link_libraries(ngi_aux PRIVATE ngi_log)
endif()
