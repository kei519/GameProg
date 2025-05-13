# ライブラリは提供されているのもなので、警告は抑制する
# つまり実行ファイルのみ警告を考慮する
if (MSVC)
    add_compile_options(/W4 /WX /w15262)

    # assets ディレクトリをコピー
    add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"$<CONFIG>"/assets
    )
else()
    add_compile_options(-Wall -Wextra -Werror)
endif()

add_subdirectory(NimotsuKun)
add_subdirectory(DisplayImage)
