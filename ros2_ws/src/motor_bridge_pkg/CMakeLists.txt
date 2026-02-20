cmake_minimum_required(VERSION 3.8)
project(motor_bridge_pkg)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(motor_bridge_node src/motor_bridge_node.cpp)
ament_target_dependencies(motor_bridge_node
    rclcpp sensor_msgs std_msgs)

install(TARGETS motor_bridge_node
    DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)
    ament_add_gtest(test_motor_bridge test/test_motor_bridge.cpp)
    ament_target_dependencies(test_motor_bridge rclcpp sensor_msgs)
endif()

ament_package()
