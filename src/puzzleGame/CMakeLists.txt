add_executable(puzzleGame main.cpp)

if (MSVC)
    add_dependencies(puzzleGame copy_assets)
else()
    file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
