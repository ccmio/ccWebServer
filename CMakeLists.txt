<<<<<<< HEAD
cmake_minimum_required(VERSION 3.0.0)
project(webserver VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(webserver main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
=======
cmake_minimum_required(VERSION 3.15)
project(ccwebserver)

set(CMAKE_CXX_STANDARD 11)

add_executable(ccwebserver main.cpp)
>>>>>>> 2d11fe049e129b778f30fb6c2c6cc13e172fd2e2
