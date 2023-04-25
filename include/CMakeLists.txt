add_library(ptcore INTERFACE)
add_library(ptcore::ptcore ALIAS ptcore)

target_sources(ptcore
    INTERFACE
        FILE_SET HEADERS
        FILES
            ptcore/text_literals.h
)

target_compile_features(ptcore INTERFACE cxx_std_23)
