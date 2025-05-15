#include <Framework.h>
using namespace GameLib;

#include <cstring>
#include <fstream>

/**
 * @brief リトルエンディアンのバイト列から 32 ビット符号なし整数を読み取る。
 *
 * @param p 先頭ポインタ。
 * @return unsigned 32 bit 符号なし整数。
 */
unsigned getUnsigned(char const *const p)
{
    auto const up = reinterpret_cast<const unsigned char *>(p);
    return (up[3] << 24) | (up[2] << 16) | (up[1] << 8) | up[0];
}

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
int readFile(char const *const path, char *&buf)
{
    using namespace std;

    ifstream inputFile(path, ios_base::binary);

    inputFile.seekg(0, ios_base::end);
    auto const fileSize = static_cast<int>(inputFile.tellg());
    if (fileSize == -1) {
        return -errno;
    }

    buf = new char[fileSize];
    inputFile.seekg(0, ios_base::beg);
    inputFile.read(buf, fileSize);
    return fileSize;
}

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
bool readDds(char const *const buf, int const len, unsigned *&img, int &width, int &height)
{
    int constexpr HEADER_LEN = 124;

    // ヘッダーサイズあるかの確認
    if (len < HEADER_LEN) {
        cout << "DDS file length is too small to have its header " << endl;
        return false;
    }
    // マジックの検証
    if (buf[0] != 'D' || buf[1] != 'D' || buf[2] != 'S' || buf[3] != ' ') {
        cout << "DDS file has invalid magic" << endl;
        return false;
    }
    // ヘッダー長の確認
    if (getUnsigned(buf + 4 * 1) != HEADER_LEN) {
        cout << "DDS header length is invalid" << endl;
        return false;
    }

    height = getUnsigned(buf + 4 * 3);
    width = getUnsigned(buf + 4 * 4);

    // フォーマットの確認
    int constexpr DDPF_RGB = 0x40;
    if (getUnsigned(buf + 4 * 19) != 32            // 1 ピクセルあたりのビット数
        || !(getUnsigned(buf + 4 * 20) & DDPF_RGB) // RGB かどうか
    ) {
        cout << "DDS file has invalid format" << endl;
        return false;
    }

    // データ長の確認
    if (len - HEADER_LEN < width * height * 4) {
        cout << "DDS file length is too small to have actual image data" << endl;
        return false;
    }
    img = new unsigned[width * height];
    for (auto i = 0; i < width * height; i++) {
        img[i] = getUnsigned(buf + HEADER_LEN + i * 4);
    }
    return true;
}

void Framework::update()
{
    static bool isFirst = true;
    static unsigned *img = nullptr;
    static int imgWidth = 0;
    static int imgHeight = 0;
    static bool isErrorOccurred = false;

    if (isFirst) {
        int w;
        int h;

        isFirst = false;

        unsigned *vram;

        char *fileBuf = nullptr;
        auto fileSize = readFile("assets/img/bar.dds", fileBuf);
        if (fileSize < 0) {
            char buf[256];
            strerror_s(buf, -fileSize);
            cout << buf << endl;
            goto ERROR;
        }

        if (!readDds(fileBuf, fileSize, img, imgWidth, imgHeight)) {
            cout << "failed to read DDS" << endl;
            isErrorOccurred = true;
            goto ERROR;
        }
        delete[] fileBuf;
        fileBuf = nullptr;

        vram = videoMemory();
        w = imgWidth < width() ? imgWidth : width();
        h = imgHeight < height() ? imgHeight : height();
        for (auto x = 0; x < w; x++) {
            for (auto y = 0; y < h; y++) {
                vram[x + y * width()] = img[x + y * imgWidth];
            }
        }
        return;

    ERROR:
        isErrorOccurred = true;
        delete[] fileBuf;
        fileBuf = nullptr;
        return;
    }

    if (isErrorOccurred) {
        requestEnd();
    }
    if (isEndRequested()) {
        delete[] img;
        img = nullptr;
    }
}
