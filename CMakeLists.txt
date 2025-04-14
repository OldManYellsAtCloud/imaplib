cmake_minimum_required(VERSION 3.16)

project(imaplib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)
find_package(loglib REQUIRED)
find_package(OpenSSL REQUIRED)

set(HEADERS
    authenticate.h
    base64.h
    curlrequest.h
    curlresponse.h
    capability.h
    inboxlabel.h
    imapcommand.h
    imapconnection.h
    logout.h)

set(SOURCES
    authenticate.cpp
    base64.cpp
    curlrequest.cpp
    curlresponse.cpp
    capability.cpp
    inboxlabel.cpp
    imapcommand.cpp
    imapconnection.cpp
    logout.cpp)


add_library(imaplib SHARED
    ${HEADERS}
    ${SOURCES}
    )

include(GNUInstallDirs)
install(TARGETS imaplib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_link_libraries(imaplib PRIVATE curl loglib OpenSSL::SSL)

option(enable_test "Compile and run tests" OFF)

if (enable_test)
    find_package(GTest REQUIRED)
    enable_testing()

    include(GoogleTest)


    set(TEST_SOURCES tests/dev_tests.cpp)

    add_executable(imaplib_tests ${HEADERS} ${SOURCES} ${TEST_SOURCES})

    target_include_directories(imaplib_tests PRIVATE include)
    target_include_directories(imaplib_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

    target_link_directories(imaplib_tests PRIVATE $ENV{CMAKE_SYSROOT}/usr/lib)
    target_link_libraries(imaplib_tests PRIVATE GTest::gtest_main GTest::gmock_main SettingsLib curl loglib OpenSSL::SSL)

    install(TARGETS imaplib_tests
            BUNDLE DESTINATION .
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
