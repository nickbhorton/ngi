add_library(ngi_glad)

# adding the glad source from online resource
# https://gen.glad.sh/#generator=c&api=gl%3D4.5&profile=gl%3Dcompatibility%2Cgles1%3Dcommon
target_sources(ngi_glad PRIVATE glad/gl.c) 

target_include_directories(ngi_glad PRIVATE SYSTEM ${CMAKE_SOURCE_DIR}/deps)
