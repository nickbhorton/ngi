add_library(ngi_glad)

# adding the glad source from online resource
# https://gen.glad.sh/#generator=c&api=gl%3D4.5&profile=gl%3Dcompatibility%2Cgles1%3Dcommon
target_sources(ngi_glad PRIVATE glad/gl.c) 

target_include_directories(ngi_glad PRIVATE SYSTEM ${CMAKE_SOURCE_DIR}/deps)

add_library(ngi_imgui)

# adding the glad source from online resource
# https://gen.glad.sh/#generator=c&api=gl%3D4.5&profile=gl%3Dcompatibility%2Cgles1%3Dcommon
target_sources(ngi_imgui PRIVATE
    imgui/imgui.h
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_internal.h
    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3.cpp
) 

target_include_directories(ngi_imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/deps/imgui
)
