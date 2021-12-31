#include "../include/Utilities/Logger.hpp"
#include "../include/Utilities/Profiler.hpp"
#include "tester_assert.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void function_test() {
    SC_APP_INFO("HELLO FROM APPLICATION!");

    int x = 0;
    SC_APP_INFO("TIME TAKEN {}", x);
    SC_CORE_INFO("HELLO FROM CORE!");
}

void test_all() {
    SC_TEST_ASSERT(true);
    using namespace Stardust_Celeste;
    Utilities::Logger::init();

    SC_PROFILE_BEGIN_SESSION("Main", "Test.json");

    SC_PROFILE_FUNCTION(function_test, __LINE__, __FILE__)

    SC_PROFILE_END_SESSION();
    Utilities::Logger::cleanup();
}

int main(int, char **) {
    test_all();

    return 0;
}