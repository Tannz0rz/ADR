#pragma once

#include <stdexcept>
#include <memory>
#include <vector>

#include "ros/ros.h"
#include "ros/console.h"

#include "actionlib/client/simple_action_client.h"
#include "move_base_msgs/MoveBaseAction.h"

#include "std_srvs/Empty.h"
#include "uvc_light/set_uvc_light.h"
#include "coverage_path_planner/make_plan.h"

#include "ros_launch_manager.hpp"

#include "hfsm2/machine.hpp"

struct Context 
{
    Context(ros::NodeHandle const &node_handle);

    ros::NodeHandle m_node_handle;
};

struct Idle;
struct Manual;
struct Automatic;
struct Delay;
struct Discover;
struct Observe;
struct Explore;
struct Disinfect;

using Machine = hfsm2::MachineT<hfsm2::Config::ContextT<Context>>;
using FiniteStateMachine = Machine::PeerRoot<
                Idle,
                Manual,
                Machine::Composite<Automatic,
                    Delay,
                    Machine::Composite<Discover,
                        Observe,
                        Explore
                    >,
                    Disinfect
                >
            >;

struct Idle : public FiniteStateMachine::State 
{
	void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Manual : public FiniteStateMachine::State
{
	void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Automatic : public FiniteStateMachine::State
{
    ROSLaunchManager m_ros_launch_manager;

    pid_t m_realsense_pid;
    pid_t m_rtabmap_pid;

	void entryGuard(GuardControl &control) noexcept;
    void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exitGuard(GuardControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Delay : public FiniteStateMachine::State
{
    ros::Time m_start;
    ros::Duration m_delay;

	void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Discover : public FiniteStateMachine::State
{
    ros::ServiceClient m_set_mode_mapping_client;

	void entryGuard(GuardControl &control) noexcept;
    void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Observe : public FiniteStateMachine::State
{
	void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Explore : public FiniteStateMachine::State
{
	void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exit(Control &control) noexcept;
};

struct Disinfect : public FiniteStateMachine::State
{
    ros::ServiceClient m_set_mode_localization_client;
    ros::ServiceClient m_set_uvc_light_client;
    ros::ServiceClient m_make_plan_client;

    std::unique_ptr<actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction>> m_move_base_client;

    std::vector<geometry_msgs::Pose> m_plan;

    std::size_t m_plan_index;

	void entryGuard(GuardControl &control) noexcept;
    void enter(Control &control) noexcept;
	void update(FullControl &control) noexcept;
    void exitGuard(GuardControl &control) noexcept;
    void exit(Control &control) noexcept;
};
