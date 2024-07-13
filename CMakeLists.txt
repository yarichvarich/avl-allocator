cmake_minimum_required(VERSION 3.21.2)

project(yaro-messenger)

include(cmake/linkGTEST.cmake)

set(CXX_STANDARD 17)
set(LINKER_LANGUAGE CXX)

option(BUILD_TESTS "" ON)

set(EXT_PROJ_DIRS ${PROJECT_SOURCE_DIR}/third-party)

add_subdirectory(${EXT_PROJ_DIRS}/gtest)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_dependencies(avltree-test googletest)
    add_dependencies(avlallocator-test googletest)
endif()

set(LIB_SRC
    ./include/AVLAllocator.hpp
    ./include/AVLTree.hpp 
)

add_library(
	AVLAllocator INTERFACE
    ${LIB_SRC}
)
