if (MSVC)
    add_executable(DisplayImage WIN32 main.cpp)
else()
    add_executable(DisplayImage main.cpp)
endif()

target_link_libraries(DisplayImage GameLib)
