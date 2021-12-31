#include "../include/Utilities/Logger.hpp"
#include "../include/Utilities/Timer.hpp"
#include "tester_assert.hpp"

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("Test", 0, 1, 0);
#endif

void test_all() {
    SC_TEST_ASSERT(true);
    using namespace Stardust_Celeste;
    Utilities::Logger::init();
    Utilities::Timer::init();

    auto appTimer = Utilities::Timer::get_app_timer();

    SC_APP_INFO("HELLO FROM APPLICATION!");

    auto dt = appTimer->get_delta_time();
    SC_APP_INFO("TIME TAKEN {}", dt);
    SC_CORE_INFO("HELLO FROM CORE!");

    Utilities::Timer::cleanup();
    Utilities::Logger::cleanup();
}

int main(int, char **) {
    test_all();

    return 0;
}