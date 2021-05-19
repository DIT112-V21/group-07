# Dummy test
add_executable(car_test
        car_test.cpp
)
target_include_directories(car_test PUBLIC ../arduino/wrappers/)
configure_test(car_test)
