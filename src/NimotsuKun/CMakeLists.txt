add_executable(NimotsuKun main.cpp)

if (MSVC)
    add_dependencies(NimotsuKun copy_assets)
else()
    file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()
