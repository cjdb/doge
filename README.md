# Dynamically Open Game Engine (doge)

# Prerequisites

* GCC 7
* CMake 3.9
* Ninja or Make
* cmcstl2 tip-of-trunk
* Boost 1.65.1
* GLFW 3
* GLM
* Git

# Install

```bash
mkdir -p doge/build
cd doge
git clone https://github.com/cjdb/doge.git
cd build
cmake -G'Ninja' -DDOGE_GLFW_PATH=<path-to-glfw> -DDOGE_GLM_PATH=<path-to-glm> -DDOGE_CMCSTL2_PATH=<path-to-cmcstl2> ../doge
ninja
```

Note that `DOGE_*_PATH` variables are optional, and are not necessary if the library is installed in the compiler's default path.
