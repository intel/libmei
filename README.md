# libmei

Libmei provides a simple programming interface for accessing
Intel HECI interfaces on devices found in BigCore and Atom based products.

## CMake Build

libmei library uses CMake build system

1. Run `cmake -S <src> -B <builddir>` to configure the build
2. Run `cmake --build <builddir> --target package` to build .deb and .rpm packages and .tgz archive
