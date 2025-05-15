if (MSVC)
    add_executable(DisplayImage WIN32 main.cpp)
    add_dependencies(DisplayImage copy_assets)
else()
    add_executable(DisplayImage main.cpp)
    file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

target_link_libraries(DisplayImage GameLib)
