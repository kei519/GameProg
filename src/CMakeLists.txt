if (MSVC)
    # 入力ファイルを UTF-8 で受け取る
    add_compile_options(/utf-8)
endif()

add_subdirectory(puzzleGame)
add_subdirectory(2DGraphics1)
