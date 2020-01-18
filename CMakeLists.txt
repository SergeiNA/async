cmake_minimum_required(VERSION 3.9)

if(DEFINED ENV{TRAVIS_BUILD_NUMBER})
    project(async VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})
else()
    project(async VERSION 7.7.7)
endif()

project(async)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set (LIB_FILES 
                async.h             async.cpp 
                async_adaptor.h     async_adaptor.cpp 
                command_handler.h   command_handler.cpp
                observer.h          observer.cpp 
                Math.h              Math.cpp 
                metrics.h           metrics.cpp 
                threadManager.h     threadManager.cpp 
                )

set(EXE_FILES examples.cpp)

set(TEST_FILES 
                test_asyncAdaptor.cpp
                test_cmdhandler_nested.cpp
                test_cmdhandler_regular.cpp)

add_library(async SHARED ${LIB_FILES})

add_executable(async_cli ${EXE_FILES})

set(TESTS test_async_adaptor 
          test_async_cmdQ_nested
          test_async_cmdQ_regular)

list(LENGTH TESTS len_TESTS_r)
math(EXPR len_TESTS "${len_TESTS_r} - 1")

foreach(i RANGE ${len_TESTS})
    list(GET TESTS ${i} test_name)
    list(GET TEST_FILES ${i} test_file)

    add_executable(
        ${test_name}
        ${test_file}
    ) 

endforeach(i)

set(CMP_OPTIONS "-pthread;-Wpedantic;-Wall;-Wextra")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    list(APPEND CMP_OPTIONS "-g")
    list(APPEND CMP_OPTIONS "-fsanitize=thread")
    message("debug")
endif()


set_target_properties(async_cli  ${TESTS} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    COMPILE_OPTIONS "${CMP_OPTIONS}"
    LINK_FLAGS "-fsanitize=thread"
)

set_target_properties(${TESTS} PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

target_link_libraries(async_cli
    Threads::Threads
)

foreach(i RANGE ${len_TESTS})
    list(GET TESTS ${i} element)
    message(${element})

    target_link_libraries(
        ${element}
        async
        Threads::Threads
        ${Boost_LIBRARIES}
    )   

endforeach(i)

target_link_libraries(async_cli async)

install(TARGETS async LIBRARY DESTINATION lib)

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT nikseas@gmail.com)

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)

enable_testing()
add_test(${TESTS} ${TESTS})
