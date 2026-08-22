# File I/O, memory-map, and byte-swap tests. Currently Unix-like only.
if (NOT WIN32)
    set(SSC_TEST_SOURCES TestFileRead TestFileSeek TestFileAppendTruncate TestMemMapInit TestMemMapResize TestMemMapSecret TestSwap)
    foreach(test ${SSC_TEST_SOURCES})
        add_executable(${test} ${test}.c)
        target_include_directories(${test} PRIVATE ${CMAKE_SOURCE_DIR})
        target_link_libraries(${test} PRIVATE SSC)
        target_compile_options(${test} PRIVATE -Wall -Wextra)
        add_test(NAME ${test} COMMAND ${test})
    endforeach()

    # Force the raw bit-manipulation swap implementation; its definitions come from Impl/Swap.c.
    add_executable(TestSwapRaw TestSwap.c ../Impl/Swap.c)
    target_include_directories(TestSwapRaw PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(TestSwapRaw PRIVATE SSC)
    target_compile_options(TestSwapRaw PRIVATE -Wall -Wextra -DSSC_SWAP_IMPL=SSC_SWAP_IMPL_RAW)
    add_test(NAME TestSwapRaw COMMAND TestSwapRaw)
endif()
