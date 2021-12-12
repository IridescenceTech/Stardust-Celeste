#include "tester_assert.hpp"

void test_all() {
    SC_TEST_ASSERT(true);
    SC_TEST_EQUAL(2 + 2, 4);
}

int main(int, char**) {
    test_all();

    return 0;
}