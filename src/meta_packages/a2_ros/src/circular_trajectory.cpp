#include <chrono>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"

using namespace std::chrono_literals;

class CircleTrajectory : public rclcpp::Node
{
public:
  CircleTrajectory() : Node("circle_trajectory"), count_(0)
  {
    // Publisher to send TwistStamped messages to the robot
    publisher_ = this->create_publisher<geometry_msgs::msg::TwistStamped>("/cmd_vel", 10);
    
    // Timer to run our control loop at 10 Hz
    timer_ = this->create_wall_timer(
      100ms, std::bind(&CircleTrajectory::timer_callback, this));
  }

private:
  void timer_callback()
  {
    auto message = geometry_msgs::msg::TwistStamped();
    message.header.stamp = this->get_clock()->now();
    message.header.frame_id = "";
    
    // Drive forward and rotate to follow a circular trajectory.
    message.twist.linear.x = 2.0;
    message.twist.angular.z = 2.0;
    
    publisher_->publish(message);
    
    // Stop the robot after 10 seconds (roughly 1.5 loops at this speed)
    if (count_ >= 200) {
      message.twist.linear.x = 0.0;
      message.twist.angular.z = 0.0;
      publisher_->publish(message);
      RCLCPP_INFO(this->get_logger(), "Trajectory complete. Stopping robot.");
      rclcpp::shutdown();
    }
    count_++;
  }

  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr publisher_;
  int count_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CircleTrajectory>());
  rclcpp::shutdown();
  return 0;
}
