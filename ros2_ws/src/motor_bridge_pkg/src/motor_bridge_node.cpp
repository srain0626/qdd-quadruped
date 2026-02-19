/**
 * motor_bridge_node
 *
 * ROS2 ↔ ESP-NOW 브리지
 * /joint_commands 구독 → ESP-NOW 브로드캐스트
 * ESP-NOW 수신 → /joint_states 퍼블리시
 *
 * TODO: ESP-NOW 실제 통신 구현 (현재 골격 코드)
 */

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>

class MotorBridgeNode : public rclcpp::Node {
public:
    MotorBridgeNode() : Node("motor_bridge_node") {
        joint_state_pub_ = create_publisher<sensor_msgs::msg::JointState>(
            "/joint_states", 10);

        joint_cmd_sub_ = create_subscription<sensor_msgs::msg::JointState>(
            "/joint_commands", 10,
            [this](const sensor_msgs::msg::JointState::SharedPtr msg) {
                on_joint_command(msg);
            });

        RCLCPP_INFO(get_logger(), "motor_bridge_node started");
    }

private:
    void on_joint_command(const sensor_msgs::msg::JointState::SharedPtr msg) {
        // TODO: ESP-NOW 패킷으로 변환 후 브로드캐스트
        (void)msg;
    }

    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_pub_;
    rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_cmd_sub_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MotorBridgeNode>());
    rclcpp::shutdown();
    return 0;
}
