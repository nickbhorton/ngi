# Nick's Graphics Interface
- OpenGL loading library of choice is GLAD
- GL context library of choice is GLFW

# Building
Requires CMake and glfw to be installed
```bash
mkdir build && cd build
cmake .. # or cmake .. -DNGI_LOG=1 for debug mode
make
./assignment1 # paths are relative from build directory
```

# Debugging
If cmake is called with '-DNGI_LOG=1' set then there will be some OpenGL debugging
hints as well as constructor and destructor notifications.

# How to use
WASD + mouse for looking around
o - orthogonal projection
p - perspective projection
q - exit
