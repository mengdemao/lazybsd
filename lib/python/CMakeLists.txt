project(pylazybsd)

find_package(Python COMPONENTS Interpreter Development)
find_package(pybind11 CONFIG)

include_directories(${pybind11_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})
pybind11_add_module(pyncnn src/main.cpp)
set_target_properties(pyncnn PROPERTIES OUTPUT_NAME "lazybsd")
target_link_libraries(pyncnn PUBLIC lazybsd)
set_target_properties(pyncnn PROPERTIES PREFIX "" LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lazybsd")
set_property(TARGET pyncnn PROPERTY FOLDER "python")
if("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" STREQUAL "")
    add_custom_command(TARGET pyncnn POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lazybsd/lazybsd${PYTHON_MODULE_PREFIX}${PYTHON_MODULE_EXTENSION} 
        ${PROJECT_SOURCE_DIR}/lazybsd/lazybsd${PYTHON_MODULE_PREFIX}${PYTHON_MODULE_EXTENSION})
endif("${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" STREQUAL "")

configure_file(setup.py.i ${PROJECT_SOURCE_DIR}/setup.py)