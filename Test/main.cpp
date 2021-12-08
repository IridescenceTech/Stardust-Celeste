#include <Stardust-Celeste.hpp>
#include <iostream>

#ifdef PSP
#include <pspkernel.h>
PSP_MODULE_INFO("TEST", 0, 1, 0);
#endif

int main() {
    std::cout << STARDUST_CELESTE_VERSION_MAJOR << " " << STARDUST_CELESTE_VERSION_MINOR << std::endl;
    Stardust_Celeste::exit();

    return 0;
}