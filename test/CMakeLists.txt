# Dummy test
add_executable(smartcar_test
    smartcar_test.cpp
)
target_include_directories(smartcar_test PUBLIC ../arduino/smartcar/)
configure_test(smartcar_test)
