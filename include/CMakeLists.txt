include(FetchContent)

# Tags: https://github.com/capo-at-the-5th-fret/tags
# Linkage: target_link_libraries(easy::easy)
set(NEEDED_easy_VERSION "1.0.0")

find_package(easy ${NEEDED_easy_VERSION} QUIET)

if (NOT easy_FOUND)

    FetchContent_Declare(
        easy
        GIT_REPOSITORY http://github.com/capo-at-the-5th-fret/easy.git
        GIT_TAG        origin/main
        GIT_SHALLOW    TRUE
    )

    FetchContent_MakeAvailable(easy)

endif()

add_library(ptcore INTERFACE)
add_library(ptcore::ptcore ALIAS ptcore)

target_sources(ptcore
    INTERFACE
        FILE_SET HEADERS
        FILES
            ptcore/parser.h
            ptcore/ratio.h
            ptcore/text_literals.h
)

target_compile_features(ptcore INTERFACE cxx_std_23)
target_link_libraries(ptcore INTERFACE $<BUILD_INTERFACE:easy::easy>)
