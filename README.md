# Dynamically Open Game Engine (doge)

## Prerequisites

* GCC 7
* CMake 3.9
* Ninja or Make
* cmcstl2 tip-of-trunk
* Boost 1.65.1
* GLFW 3
* GLM
* Git

## Install

```bash
mkdir -p doge/build
cd doge
git clone https://github.com/cjdb/doge.git
cd build
cmake -G'Ninja' -DDOGE_GLFW_PATH=<path-to-glfw> -DDOGE_GLM_PATH=<path-to-glm> -DDOGE_GLI_PATH=<path-to-gli> -DDOGE_CMCSTL2_PATH=<path-to-cmcstl2> ../doge
ninja
ninja test
ninja install # administrator privliges may be necessary
```

Note that `DOGE_*_PATH` variables are optional, and are not necessary if the library is installed in
the compiler's default path.

### Officially supported platforms

* Microsoft Windows 10 (using MinGW-w64)

## Reporting bugs and issues

Please use the issue tracker at https://github.com/cjdb/doge to report any issues that you
encounter.

## Submitting patches

Patches can be submitted by making pull requests from your local fork. All patches are subject to
review. Please make sure that your pull request attempts to merge from a branch other than master.
Patches that attempt to merge from master will be closed.
