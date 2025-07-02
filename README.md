# Knot-DI

**Knot-DI** is a lightweight, header-only Dependency Injection (DI) library for C++03.  
It is designed for embedded, legacy, or resource-constrained environments where modern C++ features are unavailable.

## Features

- **Header-only:** No linking required, just include the headers.
- **C++03 compatible:** Works on old toolchains.
- **Singleton and Transient lifetimes**
- **Custom memory pool support**
- **Macro-based service registration for multiple constructor arities**

## Getting Started

### Requirements

- C++03-compatible compiler (e.g., GCC, Clang)
- CMake ≥ 3.10
- [Optional] lcov/genhtml for coverage, clang-format for formatting, Doxygen for docs

### Build & Test

```sh
# Clone and enter the repo
git clone <repo-url>
cd knot-di

# Build and run tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
cd build
ctest --output-on-failure
```

### Code Coverage

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCOVERAGE=ON
cmake --build build
cd build
ctest
make coverage  # Generates coverage-report/ with HTML
```

### Formatting

```sh
cmake --build build --target format
```

### Documentation

```sh
cmake --build build --target doc
```

## Usage

Include the main header in your project:

```cpp
#include <knot-di/Container.hpp>
```

Register and resolve services:

```cpp
Knot::Container container;
container.registerService<MyType>(SINGLETON);
MyType* instance = container.resolve<MyType>();
```

See `tests/ContainerTests.cpp` for more usage examples.

## Development Environment (Nix)

If you use [Nix](https://nixos.org):

```sh
nix develop
```

This provides all dependencies for building, testing, formatting, and coverage.
