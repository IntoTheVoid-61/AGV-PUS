/**
 * @file agv_velocity_controller.cpp
 * @brief Controls a mecanum wheeled robot to move in with specified velocity, that can be modified in cmd
 *
 * This program creates a ROS 2 node that publishes velocity commands to make a
 * mecanum wheeled robot move with desired velocity. It takes advantage of the
 * omnidirectional capabilities of mecanum wheels.
 * 
 * This is a basic controller node, used for testing and dynamically adjusting robots inverse kinematics i.e. (velocity)
 *
 * Publishing Topics:
 *     /mecanum_drive_controller/cmd_vel (geometry_msgs/TwistStamped):
 *         Velocity commands for the robot's motion
 *
 * @author Ziga Breznikar
 * @date September 09, 2025
 */

#include <chrono>
#include <functional>
#include <memory>
#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist_stamped.hpp"

using namespace std::chrono_literals;

class VelocityController : public rclcpp::Node
{
public:
    VelocityController() : Node("velocity_controller") // Constructor
    {
        // Creates a publisher, that publishes on topic /mecanum_drive_controller/cmd_vel. With the msg type geometry_msgs/TwistStamped
        publisher_ = this->create_publisher<geometry_msgs::msg::TwistStamped>("/mecanum_drive_controller/cmd_vel",10); 

        // Timer that calls the update function every 100ms
        timer_ = this->create_wall_timer(200ms, std::bind(&VelocityController::timer_callback,this));

        // Default velocities   => Rotation with radius 1
        vx_ = 1.0;
        vy_ = 0.0;
        wz_ = 1.0;
    }

    // Destructor, makes sure that when class (Node) is destroyed it stops the robot
    ~VelocityController()
    {
        stop_robot();
    }

    void set_velocity(double vx, double vy, double wz)
    {
        vx_ = vx;
        vy_ = vy;
        wz_ = wz;
    }

private:
    // this is the function that the timer calls every 100ms
    void timer_callback()
    {
        auto msg = geometry_msgs::msg::TwistStamped(); // msg is of type ...
        msg.header.stamp = this->now();

        msg.twist.linear.x = vx_;
        msg.twist.linear.y = vy_;
        msg.twist.angular.z = wz_;

        //publish
        publisher_->publish(msg);
    }

    void stop_robot()
    {
        auto msg = geometry_msgs::msg::TwistStamped();
        msg.header.stamp = this->now();

        msg.twist.linear.x = 0.0;
        msg.twist.linear.y = 0.0;
        msg.twist.angular.z = 0.0;

        publisher_->publish(msg); // defaults to zero velocities
    }

    // ROS 2 members
    rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;

    // Velocities
    double vx_;
    double vy_; 
    double wz_;



};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<VelocityController>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}