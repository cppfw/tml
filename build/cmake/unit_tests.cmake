# if the library is compiled by vcpkg during the port build (i.e. during the package installation),
# then we don't need to build unit tests
if(IS_VCPKG_PORT_BUILD)
    return()
endif()

# no unit tests for ios
if(IOS)
    return()
endif()

set(tests_srcs)
myci_add_source_files(tests_srcs
    DIRECTORY
        ${CMAKE_CURRENT_LIST_DIR}/../../tests/unit/src
    RECURSIVE
)
myci_declare_application(${PROJECT_NAME}-tests
    SOURCES
        ${tests_srcs}
    RESOURCE_DIRECTORY
        ${CMAKE_CURRENT_LIST_DIR}/../../tests/unit/res
    DEPENDENCIES
        tml::tml
        tst::tst
)

myci_declare_test(
    RUN_TARGET
        run-${PROJECT_NAME}-tests
)
