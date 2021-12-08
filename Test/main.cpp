#include <Stardust-Celeste.hpp>
#include <iostream>

int main() {
    std::cout << STARDUST_CELESTE_VERSION_MAJOR << " " << STARDUST_CELESTE_VERSION_MINOR << std::endl;
    Stardust_Celeste::exit();

    return 0;
}