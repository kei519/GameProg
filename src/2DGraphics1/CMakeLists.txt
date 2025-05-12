if (MSVC)
    add_executable(2DGraphics1 WIN32 main.cpp)
else()
    add_executable(2DGraphics1 main.cpp)
endif()

target_link_libraries(2DGraphics1 GameLib)
