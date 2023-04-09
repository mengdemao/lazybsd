cmake_minimum_required(VERSION 3.24)

project(test C CXX ASM)

include(GoogleTest)
enable_testing()

macro(make_tests)

set(file_name ${ARGV0})

add_executable(${file_name} ${file_name}.cc)
target_link_libraries(${file_name} gtest gtest_main Threads::Threads bsd bypass wrapper lazybsd fmt)
gtest_add_tests(TARGET      ${file_name}
                TEST_SUFFIX .noArgs
                TEST_LIST   ${file_name}
)

target_compile_definitions(${file_name} PRIVATE TEST_NAME=${file_name})
set_tests_properties(${${file_name}} PROPERTIES TIMEOUT 10)
endmacro(make_tests)

file(GLOB TEST_FILE_SOURCES "*.cc")
foreach(TEST_FILE ${TEST_FILE_SOURCES})
    string(REGEX MATCH "test/.*"    TEST_FILE ${TEST_FILE})
    string(REGEX REPLACE "test/" "" TEST_FILE ${TEST_FILE})
    string(REGEX REPLACE ".cc"   "" TEST_FILE ${TEST_FILE})

    make_tests(${TEST_FILE})
    add_test(NAME ${TEST_FILE}  COMMAND test/${TEST_FILE} RETURN_VALUE 0)
endforeach()


