# Start of root CMakeLists.txt

cmake_minimum_required(VERSION 3.10)

# Setting the project name based on the root folder name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Common build flags
set(CMAKE_C_FLAGS         "-Wall -Wextra -std=c11")

# Individual build type flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -O2     -Wall -Wextra")
set(CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS} -O0 -g3 -Wall -Wextra")
set(CMAKE_C_FLAGS_TEST    "${CMAKE_C_FLAGS} -O0 -g3 -Wall -Wextra -D__test_build__ --coverage")

# List of components
set(COMPONENTS 
  
  ingestify
  ignore
  common)

# Component build options


if(CMAKE_BUILD_TYPE MATCHES Test)
    add_executable(${PROJECT_NAME} main_test.c)
else()
    add_executable(${PROJECT_NAME} main.c)
endif()

# Add component subdirectories using loop
foreach(COMPONENT ${COMPONENTS})
    add_subdirectory(components/${COMPONENT})
endforeach()

# Linking to coverage report tool in case of test build
if(CMAKE_BUILD_TYPE MATCHES Test)
    add_subdirectory(components/c_asserts)
    target_link_libraries(${PROJECT_NAME} gcov c_asserts_obj)
endif()

# Printing the size of build after building
add_custom_command(TARGET ${PROJECT_NAME} 
    POST_BUILD COMMAND size $<TARGET_FILE:${PROJECT_NAME}>)

# End of root CMakeLists.txt
