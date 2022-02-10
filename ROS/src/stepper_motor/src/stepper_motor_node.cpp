#include "stepper_motor_node.hpp"

StepperMotorNode::StepperMotorNode(ros::NodeHandle const &node_handle) : m_node_handle(node_handle)
{
    m_set_stepper_motor_server = m_node_handle.advertiseService(ros::names::resolve("set_stepper_motor"), &StepperMotorNode::onSetStepperMotor, this);

    m_send_command_client = m_node_handle.serviceClient<serial_command_client::send_command>(ros::names::resolve("send_command"));
}

bool StepperMotorNode::onSetStepperMotor(stepper_motor::set_stepper_motor::Request &request, stepper_motor::set_stepper_motor::Response &response)
{
    static uint8_t const STEPPER_MOTOR_COMMAND = 1;

    serial_command_client::send_command send_command_srv;

    send_command_srv.request.command = STEPPER_MOTOR_COMMAND;

    send_command_srv.request.buffer.resize(sizeof(uint32_t) + sizeof(uint32_t) + sizeof(int32_t));

    std::memcpy(&send_command_srv.request.buffer[0], &request.stepper_motor_id, sizeof(uint32_t));
    std::memcpy(&send_command_srv.request.buffer[sizeof(uint32_t)], &request.speed, sizeof(uint32_t));
    std::memcpy(&send_command_srv.request.buffer[sizeof(uint32_t) + sizeof(uint32_t)], &request.step, sizeof(int32_t));

    if (m_send_command_client.call(send_command_srv)) {
        if (send_command_srv.response.status == serial_command_client::send_command::Response::SUCCESS) {
            return true;
        }
    }

    return false;
}

int main(int argc, char **argv)
{
    try {
        ros::init(argc, argv, "stepper_motor");

        ros::NodeHandle node_handle("~");

        StepperMotorNode stepper_motor_node(node_handle);
        
        while (ros::ok()) {
            ros::spinOnce();
        }

        return 0;
    }
    catch (std::exception const &exception) {
        ROS_ERROR("%s", exception.what());

        return 1;
    }
}
