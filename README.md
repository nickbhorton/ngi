# Nick's Graphics Interface
- OpenGL loading library of choice is GLAD
- GL context library of choice is GLFW

# Building
Requires CMake and glfw to be installed
```bash
mkdir build && cd build
cmake .. # or cmake .. -DNGI_LOG=1 for debug mode
make
./assignment* # paths are relative from build directory
```

# Debugging
If cmake is called with '-DNGI_LOG=1' set then there will be some OpenGL debugging
hints as well as constructor and destructor notifications.

# Assignment 6
This shows off an image processing technique called the difference of gaussians
or (dog). It is the first part of [this](https://www.youtube.com/watch?v=5EuYKEvugLU) 
Youtube video from Acerola. The video uses [this](https://users.cs.northwestern.edu/~sco590/winnemoeller-cag2012.pdf) 
paper for the dog implementations. 

The dog technique is a edge detection techniques. I have implemented two dog
modifications, thresholding and tanh thresholding. Which are ways to stylize
the image based on the dog.

