#ifndef __INCLUDE_UTIL_H
#define __INCLUDE_UTIL_H

#include <fstream>

namespace Util
{

/**
 * @brief リトルエンディアンのバイト列から 32 ビット符号なし整数を読み取る。
 *
 * @param p 先頭ポインタ。
 * @return unsigned 32 bit 符号なし整数。
 */
unsigned getUnsigned(char const *const p);

/**
 * @brief ファイルの内容を読み込む。
 *
 * @note 使用し終わったあとは読み込んだ領域を `delete[]` すること。
 *
 * @param [in] path ファイルのパス。
 * @param [out] buf 読み込んだ内容の先頭を指すポインタ。
 * @retval >=0 読み取ったデータ長。
 * @retval <0 `-errno`。
 */
int readFile(char const *const path, char *&buf);

/**
 * @brief 与えられた DDS データが正当であれば、その画像データを ARGB として返す。
 *
 * DDS データが正当とは以下を満たすこと。
 * - ヘッダーの情報と実際の情報が一致している
 * - 画像フォーマットが 1 ピクセルあたり 32 bit の ARGB であること。
 *
 * @note 画像データを使用し終わったあとは `delete[]` すること。
 *
 * @param [in] buf DDS データ。
 * @param [in] len DDS データ長。
 * @param [out] img ARGB 画像データ。
 * @param [out] width 画像の幅。
 * @param [out] height 画像の高さ。
 * @retval true 与えられた DDS データが正当。
 * @retval false 与えられた DDS データが不正。
 */
bool readDds(char const *const buf, int const len, unsigned *&img, int &width, int &height);

} // namespace Util

#endif
