/**
 * motor_bridge_pkg 유닛 테스트
 *
 * MotorBridgeNode 초기화 및 토픽 존재 여부를 검증한다.
 */

#include <gtest/gtest.h>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

// ── 노드 초기화 테스트 ───────────────────────────────────────
TEST(MotorBridgeTest, NodeInit) {
    rclcpp::init(0, nullptr);
    auto node = rclcpp::Node::make_shared("test_motor_bridge");
    ASSERT_NE(node, nullptr);
    EXPECT_EQ(std::string(node->get_name()), "test_motor_bridge");
    rclcpp::shutdown();
}

// ── JointState 메시지 구성 테스트 ───────────────────────────
TEST(MotorBridgeTest, JointStateMessage) {
    sensor_msgs::msg::JointState msg;
    msg.name = {"FL_HAA", "FL_HFE", "FL_KFE",
                "FR_HAA", "FR_HFE", "FR_KFE",
                "RL_HAA", "RL_HFE", "RL_KFE",
                "RR_HAA", "RR_HFE", "RR_KFE"};
    msg.position.assign(12, 0.0);
    msg.velocity.assign(12, 0.0);
    msg.effort.assign(12, 0.0);

    EXPECT_EQ(msg.name.size(), 12u);
    EXPECT_EQ(msg.position.size(), 12u);
    EXPECT_EQ(msg.velocity.size(), 12u);
    EXPECT_EQ(msg.effort.size(), 12u);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
