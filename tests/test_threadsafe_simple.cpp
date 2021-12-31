#include "tester_assert.hpp"

#include "../include/Utilities/ThreadSafe.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

struct MyData {
    int x;
    int y;
    char a;
    float f;

    MyData(int _x, int _y, char _a, float _f) {
        x = _x;
        y = _y;
        a = _a;
        f = _f;
    }
};

void test_all() {
    auto ts = new ThreadSafe<MyData>(1, 2, 'b', 3.0f);

    SC_TEST_ASSERT(ts);

    printf("%d\n", (*ts)->x);

    delete ts;
}

int main(int, char **) {
    test_all();

    return 0;
}