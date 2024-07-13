cmake_minimum_required(VERSION 3.21.2)

add_executable(avltree-test
    ./AVLTree_Test.cpp
)
target_compile_options(avltree-test PRIVATE -g)

add_executable(avlallocator-test
    ./AVLAllocator_Test.cpp
)
target_compile_options(avlallocator-test PRIVATE -g)

linkGTEST(avltree-test avlallocator-test )
