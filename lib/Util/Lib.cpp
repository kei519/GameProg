#include <Util.hpp>

#include <Framework.h>
using namespace GameLib;

namespace Util
{

unsigned getUnsigned(char const *const p)
{
    auto const up = reinterpret_cast<const unsigned char *>(p);
    return (up[3] << 24) | (up[2] << 16) | (up[1] << 8) | up[0];
}

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

} // namespace Util
