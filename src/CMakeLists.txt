if (MSVC)
    # 入力ファイルを UTF-8 で受け取る
    add_compile_options(/utf-8)

    # assets ディレクトリをコピー
    add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"$<CONFIG>"/assets
    )
endif()

add_subdirectory(puzzleGame)
add_subdirectory(2DGraphics1)
