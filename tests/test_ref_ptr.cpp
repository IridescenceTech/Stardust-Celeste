#include "tester_assert.hpp"

#include "../include/Utilities/Types.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void print_ref_ptr(Stardust_Celeste::RefPtr<int> ptr) {
    printf("%d\n", *ptr);
}

void test_all() {
    auto ptr = Stardust_Celeste::createRefPtr<int>(2);
    print_ref_ptr(ptr);
}

int main(int, char**) {
    test_all();

    return 0;
}