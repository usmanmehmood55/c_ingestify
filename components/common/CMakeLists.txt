# Start of common CMakeLists.txt

set(CURRENT_DIR_NAME common)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_DIR_NAME}.c)
target_include_directories(${PROJECT_NAME} PRIVATE .)

# End of common CMakeLists.txt
