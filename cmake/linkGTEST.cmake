macro(linkGTEST)
    foreach(TARGET ${ARGN})
        target_link_libraries(${TARGET}
            ${GTEST_LIBS_DIR}/libgtest.a
            ${GTEST_LIBS_DIR}/libgtest_main.a
        )
        target_include_directories(${TARGET} PRIVATE
            ${GTEST_INCLUDE_DIRS}
        )
    endforeach()
endmacro()