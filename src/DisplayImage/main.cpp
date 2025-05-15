#include <Framework.h>
using namespace GameLib;

#include <Util.hpp>
using namespace Util;

#include <cstring>
#include <fstream>

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
        auto fileSize = readFile("assets/img/nimotsuKunImage.dds", fileBuf);
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
