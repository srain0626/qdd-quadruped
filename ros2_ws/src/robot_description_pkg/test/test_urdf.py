"""
test_urdf.py – robot_description_pkg URDF 검증 테스트

xml.etree.ElementTree 로 URDF를 파싱하여
링크/관절 수 및 필수 요소를 확인한다.
"""

import os
import xml.etree.ElementTree as ET
import pytest
from ament_index_python.packages import get_package_share_directory


@pytest.fixture(scope="module")
def urdf_tree():
    pkg = get_package_share_directory("robot_description_pkg")
    urdf_path = os.path.join(pkg, "urdf", "qdd_robot.urdf")
    return ET.parse(urdf_path).getroot()


def test_robot_name(urdf_tree):
    assert urdf_tree.attrib["name"] == "qdd_quadruped"


def test_link_count(urdf_tree):
    links = urdf_tree.findall("link")
    # base_link + 4 * (hip + thigh + shin + foot) = 17
    assert len(links) == 17, f"Expected 17 links, got {len(links)}"


def test_joint_count(urdf_tree):
    joints = urdf_tree.findall("joint")
    # 4 HAA + 4 HFE + 4 KFE + 4 foot_fixed = 16
    assert len(joints) == 16, f"Expected 16 joints, got {len(joints)}"


def test_joint_names(urdf_tree):
    expected = {
        "FL_HAA", "FL_HFE", "FL_KFE",
        "FR_HAA", "FR_HFE", "FR_KFE",
        "RL_HAA", "RL_HFE", "RL_KFE",
        "RR_HAA", "RR_HFE", "RR_KFE",
    }
    joint_names = {j.attrib["name"] for j in urdf_tree.findall("joint")}
    assert expected.issubset(joint_names), \
        f"Missing joints: {expected - joint_names}"


def test_revolute_limits(urdf_tree):
    for joint in urdf_tree.findall("joint[@type='revolute']"):
        limit = joint.find("limit")
        assert limit is not None, f"{joint.attrib['name']} has no <limit>"
        assert float(limit.attrib["effort"]) > 0
        assert float(limit.attrib["velocity"]) > 0
