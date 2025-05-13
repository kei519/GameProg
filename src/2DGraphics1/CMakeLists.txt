if (MSVC)
    add_executable(2DGraphics1 WIN32 main.cpp)
    add_dependencies(puzzleGame copy_assets)
else()
    add_executable(2DGraphics1 main.cpp)
    file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

target_link_libraries(2DGraphics1 GameLib)
