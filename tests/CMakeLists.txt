
add_executable(test_basic test_basic.cpp)
add_test(basic test_basic)

if(PSP)
create_pbp_file(
    TARGET test_basic
    TITLE test_basic
)
endif()