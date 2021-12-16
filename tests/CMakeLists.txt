
add_executable(test_basic test_basic.cpp)
add_test(basic test_basic)

add_executable(test_scope_ptr test_scope_ptr.cpp)
add_test(scope_ptr test_scope_ptr)

add_executable(test_ref_ptr test_ref_ptr.cpp)
add_test(ref_ptr test_ref_ptr)

if(PSP)
create_pbp_file(
    TARGET test_basic
    TITLE test_basic
)
create_pbp_file(
    TARGET test_scope_ptr
    TITLE test_scope_ptr
)
create_pbp_file(
    TARGET test_ref_ptr
    TITLE test_ref_ptr
)
endif()