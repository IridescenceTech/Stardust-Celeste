
add_executable(test_basic test_basic.cpp)
add_test(basic test_basic)

add_executable(test_scope_ptr test_scope_ptr.cpp)
add_test(scope_ptr test_scope_ptr)

add_executable(test_ref_ptr test_ref_ptr.cpp)
add_test(ref_ptr test_ref_ptr)

add_executable(test_threadsafe_simple test_threadsafe_simple.cpp)
add_test(threadsafe_simple test_threadsafe_simple)

add_executable(test_fmt test_fmt.cpp)
add_test(fmt test_fmt)
target_link_libraries(test_fmt Stardust-Celeste)


add_executable(test_log test_log.cpp)
add_test(log test_log)
target_link_libraries(test_log Stardust-Celeste)

add_executable(test_assert test_assert.cpp)
add_test(log test_assert)
target_link_libraries(test_assert Stardust-Celeste)

if(PSP)
create_pbp_file(
    TARGET test_basic
    TITLE test_basic
    BUILD_PRX
)
create_pbp_file(
    TARGET test_scope_ptr
    TITLE test_scope_ptr
    BUILD_PRX
)
create_pbp_file(
    TARGET test_ref_ptr
    TITLE test_ref_ptr
    BUILD_PRX
)
create_pbp_file(
    TARGET test_threadsafe_simple
    TITLE test_threadsafe_simple
    BUILD_PRX
)
create_pbp_file(
    TARGET test_fmt
    TITLE test_fmt
    BUILD_PRX
)
create_pbp_file(
    TARGET test_log
    TITLE test_log
    BUILD_PRX
)
create_pbp_file(
    TARGET test_assert
    TITLE test_assert
    BUILD_PRX
)
endif()