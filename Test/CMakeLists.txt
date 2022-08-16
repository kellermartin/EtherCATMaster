add_executable(ecm_test main.cpp)

target_link_libraries(ecm_test ethercatmaster Threads::Threads)
