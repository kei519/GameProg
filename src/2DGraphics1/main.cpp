#include <Framework.h>

using namespace GameLib;

bool isFirst = true;

void Framework::update()
{
    if (isFirst) {
        cout << "update() called!" << endl;
        isFirst = false;
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
