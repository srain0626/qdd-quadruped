cmake_minimum_required(VERSION 3.8)
project(gait_controller_pkg)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

include_directories(include)

add_executable(gait_controller_node src/gait_controller_node.cpp)
ament_target_dependencies(gait_controller_node rclcpp sensor_msgs)
target_include_directories(gait_controller_node PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

install(TARGETS gait_controller_node
    DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY include/
    DESTINATION include)

if(BUILD_TESTING)
    find_package(ament_cmake_gtest REQUIRED)
    ament_add_gtest(test_gait_controller test/test_gait_controller.cpp)
    target_include_directories(test_gait_controller PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
endif()

ament_package()
