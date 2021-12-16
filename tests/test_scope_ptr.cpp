#include "tester_assert.hpp"

#include "../include/Utilities/Types.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void test_all() {
    auto ptr = Stardust_Celeste::createScopePtr<int>(2);
    SC_TEST_ASSERT(ptr);

    printf("%d\n", *ptr);

}

int main(int, char**) {
    test_all();

    return 0;
}