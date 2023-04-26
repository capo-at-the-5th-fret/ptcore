include(FetchContent)

# Tags: https://github.com/doctest/doctest/tags
set(NEEDED_DOCTEST_VERSION "2.4.11")

find_package(doctest ${NEEDED_DOCTEST_VERSION} QUIET)

if (NOT doctest_FOUND)

    FetchContent_Declare(
        doctest
        GIT_REPOSITORY "https://github.com/doctest/doctest"
        GIT_TAG        v${NEEDED_DOCTEST_VERSION}
        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(doctest)

    # include the doctest module that contains doctest_discover_tests
    list(APPEND CMAKE_MODULE_PATH ${doctest_SOURCE_DIR}/scripts/cmake)

endif()

# this needs to be called for the doctest_discover_tests function to work
include(doctest)

add_executable(testrunner

    main.cpp

    parser_tests.cpp
    text_literals_tests.cpp

)

target_include_directories(testrunner PRIVATE "${CMAKE_SOURCE_DIR}/")
target_link_libraries(testrunner
    PRIVATE
        doctest::doctest
        ptcore::ptcore
)

doctest_discover_tests(testrunner)
