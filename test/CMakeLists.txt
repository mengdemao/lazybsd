cmake_minimum_required(VERSION 3.24)

project(test C CXX ASM)

enable_testing()

include(GoogleTest)
find_package(GTest)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_TEST)
add_executable(test ${SRC_TEST})

target_link_libraries(test gtest::gtest fmt::fmt Threads::Threads lazybsd)

gtest_discover_tests(test)

if(BUILD_COVERAGE)
  include(CodeCoverage)
  append_coverage_compiler_flags()

  setup_target_for_coverage_gcovr_xml(
      NAME TestXml
      EXECUTABLE test
      DEPENDENCIES test
  )

  setup_target_for_coverage_gcovr_html(
      NAME TestHtml
      EXECUTABLE test
      DEPENDENCIES test
  )

#   setup_target_for_coverage_lcov(
#       NAME TestLcov
#       EXECUTABLE test
#       DEPENDENCIES test
#   )

endif()

