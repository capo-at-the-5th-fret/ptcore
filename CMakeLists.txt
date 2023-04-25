cmake_minimum_required(VERSION 3.25.0)

project(ptcore
    VERSION 0.1.0
    LANGUAGES CXX
)

add_subdirectory(include)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)

    option(PTCORE_BUILD_TESTS "Enable or disable the building of tests" ON)
    option(PTCORE_ENABLE_INSTALL "Enable or disable the install rule" ON)

    if (PTCORE_BUILD_TESTS)
        enable_testing()
        add_subdirectory(tests)
    endif()

    if (PTCORE_ENABLE_INSTALL)

        include(GNUInstallDirs)
        include(CMakePackageConfigHelpers)

        # NOTE 1: The XXX_PACKAGE_TARGETS variable is referenced inside of the
        # xxxConfig.cmake.in file, so if the name of this variable changes, it
        # must be updated inside of the xxxConfig.cmake.in file.
        #
        # NOTE 2: If value of this variable changes, the xxxConfig.cmake.in file
        # name must be renamed to match the new value.
        set(PTCORE_PACKAGE_NAME ${PROJECT_NAME})
        set(PTCORE_PACKAGE_TARGETS ${PTCORE_PACKAGE_NAME}Targets)

        set(PTCORE_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
        set(PTCORE_INSTALL_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PTCORE_PACKAGE_NAME})

        set(PTCORE_CONFIG_DIR ${PROJECT_BINARY_DIR}/cmake)
        set(PTCORE_CONFIG_IN cmake/${PTCORE_PACKAGE_NAME}Config.cmake.in)
        set(PTCORE_CONFIG ${PTCORE_CONFIG_DIR}/${PTCORE_PACKAGE_NAME}Config.cmake)
        set(PTCORE_CONFIG_VERSION ${PTCORE_CONFIG_DIR}/${PTCORE_PACKAGE_NAME}ConfigVersion.cmake)

        configure_package_config_file(
            ${PTCORE_CONFIG_IN}
            ${PTCORE_CONFIG}
            INSTALL_DESTINATION ${PTCORE_INSTALL_CMAKE_DIR}
        )

        # major.minor.patch1 is compatible with major.minor.patch2..n
        # if no VERSION is specified, the ${PROJECT_VERSION} variable will be
        # used. if the ${PROJECT_VERSION} is not set, CMake will output an
        # error
        write_basic_package_version_file(
            ${PTCORE_CONFIG_VERSION}
            COMPATIBILITY SameMinorVersion
        )

        install(
            FILES
                ${PTCORE_CONFIG}
                ${PTCORE_CONFIG_VERSION}
            DESTINATION ${PTCORE_INSTALL_CMAKE_DIR}
        )

        install(TARGETS ${PTCORE_PACKAGE_NAME}
            EXPORT ${PTCORE_PACKAGE_TARGETS}
            FILE_SET HEADERS
        )

        install(EXPORT ${PTCORE_PACKAGE_TARGETS}
            NAMESPACE ${PTCORE_PACKAGE_NAME}::
            DESTINATION ${PTCORE_INSTALL_CMAKE_DIR}
        )

    endif()

endif()
