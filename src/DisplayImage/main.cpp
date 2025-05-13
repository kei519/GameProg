#include <Framework.h>
using namespace GameLib;

void Framework::update()
{
    static bool isFirst = true;

    if (isFirst) {
        isFirst = false;
        cout << "update!" << endl;
    }
}
