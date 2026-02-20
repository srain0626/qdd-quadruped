"""
display.launch.py

robot_state_publisher 노드를 시작하고 URDF를 /robot_description 토픽으로 퍼블리시한다.
RViz2 선택적 실행 지원 (use_rviz:=true / false).

사용 예:
  ros2 launch robot_description_pkg display.launch.py
  ros2 launch robot_description_pkg display.launch.py use_rviz:=false
"""

import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    pkg_share = get_package_share_directory("robot_description_pkg")
    urdf_path = os.path.join(pkg_share, "urdf", "qdd_robot.urdf")

    with open(urdf_path, "r") as f:
        robot_description = f.read()

    use_rviz = LaunchConfiguration("use_rviz")

    return LaunchDescription([
        DeclareLaunchArgument(
            "use_rviz",
            default_value="true",
            description="RViz2 실행 여부",
        ),
        Node(
            package="robot_state_publisher",
            executable="robot_state_publisher",
            name="robot_state_publisher",
            output="screen",
            parameters=[{"robot_description": robot_description}],
        ),
        Node(
            package="joint_state_publisher_gui",
            executable="joint_state_publisher_gui",
            name="joint_state_publisher_gui",
            output="screen",
        ),
        Node(
            package="rviz2",
            executable="rviz2",
            name="rviz2",
            output="screen",
            condition=IfCondition(use_rviz),
        ),
    ])
