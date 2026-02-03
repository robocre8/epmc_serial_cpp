cmake_minimum_required(VERSION 3.16)
project(epmc_cpp LANGUAGES CXX)

include_directories(include)

add_executable( two_motor_control examples/two_motor_control.cpp )
target_link_libraries( two_motor_control serial )

add_executable( four_motor_control examples/four_motor_control.cpp )
target_link_libraries( four_motor_control serial )

