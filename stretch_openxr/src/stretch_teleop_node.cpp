/*
 * =====================================================================================
 *
 * Filename:  stretch_teleop_node.cpp
 *
 * Description:  A ROS2 node that receives UDP joystick data from a Meta Quest
 * and publishes it as a Twist command for a Stretch robot.
 *
 * =====================================================================================
 */

#include "rclcpp/rclcpp.h"
#include "geometry_msgs/msg/twist.h"
#include "quest_udp_receiver.hpp" // Assumes the receiver library is in the include path

#include <memory>
#include <chrono>

using namespace std::chrono_literals;

class StretchTeleopNode : public rclcpp::Node
{
public:
    StretchTeleopNode() : Node("stretch_teleop_node")
    {
        // --- Parameters ---
        // Declare parameters for IP, port, and scaling factors for movement
        this->declare_parameter<int>("udp_port", 12345);
        this->declare_parameter<double>("linear_scale", 0.5); // m/s
        this->declare_parameter<double>("angular_scale", 0.5); // rad/s

        int port = this->get_parameter("udp_port").as_int();
        linear_scale_ = this->get_parameter("linear_scale").as_double();
        angular_scale_ = this->get_parameter("angular_scale").as_double();

        // --- Network Receiver ---
        // Create an instance of our UDP receiver
        receiver_ = std::make_unique<QuestUdpReceiver>(port);

        // --- ROS2 Publisher ---
        // Create a publisher for the Twist command
        publisher_ = this->create_publisher<geometry_msgs::msg::Twist>("/stretch/cmd_vel", 10);

        // --- ROS2 Timer ---
        // Create a timer to periodically check for new UDP data
        timer_ = this->create_wall_timer(
            30ms, std::bind(&StretchTeleopNode::timer_callback, this));
        
        RCLCPP_INFO(this->get_logger(), "Stretch Teleop Node has started.");
        RCLCPP_INFO(this->get_logger(), "Listening on UDP port %d", port);
    }

private:
    void timer_callback()
    {
        JoystickData received_data;

        // Attempt to receive data from the network
        if (receiver_->receiveData(received_data))
        {
            // Create a new Twist message
            auto twist_msg = geometry_msgs::msg::Twist();

            // --- Map Joystick Input to Robot Motion ---
            // Left stick Y-axis controls forward/backward movement (linear.x)
            // Left stick X-axis controls strafing movement (linear.y)
            // Right stick X-axis controls rotation (angular.z)
            twist_msg.linear.x = received_data.left_y * linear_scale_;
            // twist_msg.linear.y = received_data.left_x * linear_scale_;
            twist_msg.angular.z = received_data.right_x * angular_scale_;

            // Publish the message
            publisher_->publish(twist_msg);
        }
    }

    // Member Variables
    std::unique_ptr<QuestUdpReceiver> receiver_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
    double linear_scale_;
    double angular_scale_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<StretchTeleopNode>());
    rclcpp::shutdown();
    return 0;
}
