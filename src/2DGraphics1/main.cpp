#include <Framework.h>

using namespace GameLib;

bool isFirst = true;

void Framework::update()
{
    if (isFirst) {
        isFirst = false;
        auto vram = videoMemory();
        for (auto x = 100; x < 200; x++) {
            for (auto y = 100; y < 200; y++) {
                vram[x + y * width()] = 0x4cb24c;
            }
        }
    }
}

[[noreturn]] int main()
{
    Framework framework;
    cout << "2DGraphics1" << endl;
    while (true) {
        framework.update();
    }
}
