# Start of ignore CMakeLists.txt

set(CURRENT_DIR_NAME ignore)
target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/${CURRENT_DIR_NAME}.c)
target_include_directories(${PROJECT_NAME} PRIVATE .)

# End of ignore CMakeLists.txt
