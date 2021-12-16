#include "../include/Utilities/Assertion.hpp"
#include "tester_assert.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void test_all() {
    SC_TEST_ASSERT(true);
    
    using namespace Stardust_Celeste;
    Utilities::Logger::init();

    SC_CORE_ASSERT(true, "No worky\n");

    Utilities::Logger::cleanup();
}

int main(int, char**) {
    test_all();

    return 0;
}