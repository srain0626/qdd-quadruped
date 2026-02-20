cmake_minimum_required(VERSION 3.8)
project(robot_description_pkg)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

install(DIRECTORY urdf launch
    DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
    find_package(ament_cmake_pytest REQUIRED)
    ament_add_pytest_test(test_urdf test/test_urdf.py)
endif()

ament_package()
