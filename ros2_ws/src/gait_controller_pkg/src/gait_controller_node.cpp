/**
 * gait_controller_node
 *
 * /joint_states 구독 → 패턴 기반 트로트 보행 계산 → /joint_commands 퍼블리시
 * 타이머 주기: 10 ms (100 Hz)
 */

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include "gait_controller_pkg/gait_controller.hpp"

using namespace gait_controller_pkg;

static const std::array<std::string, 12> kJointNames = {
    "FL_HAA", "FL_HFE", "FL_KFE",
    "FR_HAA", "FR_HFE", "FR_KFE",
    "RL_HAA", "RL_HFE", "RL_KFE",
    "RR_HAA", "RR_HFE", "RR_KFE",
};

class GaitControllerNode : public rclcpp::Node {
public:
    GaitControllerNode() : Node("gait_controller_node"), controller_() {
        cmd_pub_ = create_publisher<sensor_msgs::msg::JointState>(
            "/joint_commands", 10);

        timer_ = create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&GaitControllerNode::on_timer, this));

        RCLCPP_INFO(get_logger(), "gait_controller_node started (trot pattern)");
    }

private:
    void on_timer() {
        controller_.advance(0.01);  // dt = 10 ms

        sensor_msgs::msg::JointState msg;
        msg.header.stamp = now();
        msg.name = {kJointNames.begin(), kJointNames.end()};
        msg.position.resize(12, 0.0);

        for (int leg = 0; leg < 4; ++leg) {
            auto fp = controller_.footTarget(leg, controller_.phase());
            LegAngles angles;
            if (!controller_.inverseKinematics(fp.x, fp.z, angles)) {
                RCLCPP_WARN_ONCE(get_logger(), "IK out of workspace for leg %d", leg);
            }
            msg.position[leg * 3 + 0] = angles.haa;
            msg.position[leg * 3 + 1] = angles.hfe;
            msg.position[leg * 3 + 2] = angles.kfe;
        }

        cmd_pub_->publish(msg);
    }

    GaitController controller_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr cmd_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<GaitControllerNode>());
    rclcpp::shutdown();
    return 0;
}
