/**
 * @file dig_twin_mission.cpp
 *
 * @brief Demonstrates how to import mission items from QGroundControl plan,
 * and fly them using the MAVSDK.
 *
 * Steps to run this example:
 * 1. (a) Create a Mission in QGroundControl and save them to a file (.plan) (OR)
 *    (b) Use a pre-created sample mission plan in "plugins/mission/qgroundcontrol_sample.plan".
 *    Click
 * [here](https://user-images.githubusercontent.com/26615772/31763673-972c5bb6-b4dc-11e7-8ff0-f8b39b6b88c3.png)
 * to see what sample mission plan in QGroundControl looks like.
 * 2. Run the example by passing path of the QGC mission plan as argument (By default, sample
 * mission plan is imported).
 *
 * Example description:
 * 1. Imports QGC mission items from .plan file.
 * 2. Uploads mission items to vehicle.
 * 3. Starts mission from first mission item.
 * 4. Commands RTL once QGC Mission is accomplished.
 * 5. Execute mission in a separate thread
 * 5. Collect and log telemetry information in a separate thread
 *
 * @author Anton Hristozov, Software Engineering Institute
 * @date 2020-06-15
 */

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

#include "mavsdk_helper.h"

using namespace mavsdk;
using namespace std::chrono; // for seconds(), milliseconds()
using namespace std::this_thread; // for sleep_for()

static void telemetry_activate(void);
static void telemetry_task(void);
static void mission_task(void);

void usage(std::string bin_name)
{
    std::cout << std::endl << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name
              << " <connection_url> <path of QGC Mission plan>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
    std::cout << std::endl << "Typical usage:" <<  std::endl << NORMAL_CONSOLE_TEXT << "./dig_twin_mission udp://:14540 ./test_mission_plan" << std::endl;
}

std::string qgc_plan;

int main(int argc, char** argv)
{
    std::string connection_url;

    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    connection_url = argv[1];
    qgc_plan = argv[2];

    init_mavsdk(connection_url);

    // Create a thread for telemetry
    telemetry_activate();
    std::thread mon_task(&telemetry_task);

    // Create a thread for executing the mission
    std::thread msn_task(&mission_task);

    msn_task.join();
    std::cout << std::endl << GREEN_CONSOLE_TEXT << "mission_task finished" << NORMAL_CONSOLE_TEXT << std::endl;
    mon_task.join();
    std::cout << std::endl << GREEN_CONSOLE_TEXT << "telemetry_task finished" << NORMAL_CONSOLE_TEXT << std::endl;
    std::cout << std::endl << GREEN_CONSOLE_TEXT << "Exiting application " << argv[0]  << NORMAL_CONSOLE_TEXT << std::endl << std::endl;
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Telemetry task

std::shared_ptr<mavsdk::Telemetry> telemetry;

void telemetry_activate(void)
{

  // We want to listen to the altitude of the drone at 1 Hz.
   telemetry = std::make_shared<Telemetry>(dc.systems().at(0));

  // Wait for the IMU to be initialized
  while (!telemetry->health_all_ok()) {
      sleep_for(seconds(1));
  }

  std::cout << "Drone is ready" << std::endl;

  // Configure update rate of the various devices

  Telemetry::Result set_rate_result = telemetry->set_rate_position(0.5);
  set_rate_result = telemetry->set_rate_battery(0.5);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cout << ERROR_CONSOLE_TEXT << "telemetry_task::Setting rate failed:" << set_rate_result
    << NORMAL_CONSOLE_TEXT << std::endl;
    return;
  }

  set_rate_result = telemetry->set_rate_imu(0.5);
  if (set_rate_result != Telemetry::Result::Success) {
    std::cout << ERROR_CONSOLE_TEXT << "telemetry_task::Setting rate failed:" << set_rate_result
    << NORMAL_CONSOLE_TEXT << std::endl;
    return;
  }

}

void telemetry_task(void){

  // Wait until UAV starts flying
  while (!telemetry->in_air()) {
    sleep_for(seconds(1));
  }

  // While UAV is still flying
  while (telemetry->in_air()) {
    if (!telemetry->health_all_ok()) {
      std::cout << "telemetry_task::Health is NOT OK" << std::endl;
    }

    std::cout << "Position: " << telemetry->position() << std::endl;
    std::cout << "Home Position: " << telemetry->home() << std::endl;
    std::cout << "Attitude: " << telemetry->attitude_quaternion() << std::endl;
    std::cout << "Attitude: " << telemetry->attitude_euler() << std::endl;
    std::cout << "Angular velocity: " << telemetry->attitude_angular_velocity_body()
    << std::endl;
    std::cout << "Fixed wing metrics: " << telemetry->fixedwing_metrics() << std::endl;
    std::cout << "Ground Truth: " << telemetry->ground_truth() << std::endl;
    std::cout << "Velocity: " << telemetry->velocity_ned() << std::endl;
    std::cout << "GPS Info: " << telemetry->gps_info() << std::endl;
    std::cout << "Battery: " << telemetry->battery() << std::endl;
    std::cout << "Actuators: " << telemetry->actuator_control_target() << std::endl;
    std::cout << "Flight mode: " << telemetry->flight_mode() << std::endl;
    std::cout << "Landed state: " << telemetry->landed_state()
    << "(in air: " << telemetry->in_air() << ")" << std::endl;

    sleep_for(milliseconds(500));
  }
}

void mission_task(void){

  auto action = std::make_shared<Action>(dc.systems().at(0));
  auto mission = std::make_shared<Mission>(dc.systems().at(0));
  Mission::Result result;

  // Import Mission items from QGC plan
  std::cout << std::endl << "Importing mission from mission plan: " << qgc_plan << NORMAL_CONSOLE_TEXT << std::endl<< std:: endl;

  std::pair<Mission::Result, Mission::MissionPlan> import_res = mission->import_qgroundcontrol_mission(qgc_plan);
  handle_mission_err_exit(import_res.first, "Failed to import mission items: ");

  if (import_res.second.mission_items.size() == 0) {
    std::cerr << "No missions! Exiting..." << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << GREEN_CONSOLE_TEXT << "Found " << import_res.second.mission_items.size()
  << " mission items in the given QGC plan." << NORMAL_CONSOLE_TEXT << std::endl;

  std::cout << GREEN_CONSOLE_TEXT << "mission_task::Uploading mission..." << NORMAL_CONSOLE_TEXT <<  std::endl;
  result = mission->upload_mission(import_res.second);
  handle_mission_err_exit(result, "Mission upload failed: ");
  std::cout << GREEN_CONSOLE_TEXT << "mission_task::Mission uploaded." <<  NORMAL_CONSOLE_TEXT << std::endl;

  std::cout << GREEN_CONSOLE_TEXT << "mission_task::Arming..." << NORMAL_CONSOLE_TEXT << std::endl;
  const Action::Result arm_result = action->arm();
  handle_action_err_exit(arm_result, "mission_task::Arm failed: ");
  std::cout << GREEN_CONSOLE_TEXT <<  "mission_task::Armed." << NORMAL_CONSOLE_TEXT << std::endl;

  // Before starting the mission subscribe to the mission progress.
  mission->subscribe_mission_progress([](Mission::MissionProgress mission_progress) {
    std::cout << GREEN_CONSOLE_TEXT << "mission_task::Mission status update: " << mission_progress.current << " / "
    << mission_progress.total << NORMAL_CONSOLE_TEXT << std::endl;
  });

  std::cout << GREEN_CONSOLE_TEXT << "mission_task::Starting mission." << NORMAL_CONSOLE_TEXT << std::endl;
  result =  mission->start_mission();
  handle_mission_err_exit(result, "mission_task::Mission start failed: ");
  std::cout << GREEN_CONSOLE_TEXT << "mission_task::Started mission." << NORMAL_CONSOLE_TEXT << std::endl;

  while (!mission->is_mission_finished().second) {
    sleep_for(seconds(1));
  }
  std::cout << GREEN_CONSOLE_TEXT << "mission_task::Sleep for 5 seconds" << NORMAL_CONSOLE_TEXT << std::endl;
  sleep_for(seconds(5));

  {
    // Mission complete. Command RTL to go home.
    std::cout << GREEN_CONSOLE_TEXT << "mission_task::Commanding RTL..." << NORMAL_CONSOLE_TEXT << std::endl;
    const Action::Result result = action->return_to_launch();
    if (result != Action::Result::Success) {
      std::cout << "mission_task::Failed to command RTL (" << result << ")" << std::endl;
    } else {
      std::cout << GREEN_CONSOLE_TEXT << "mission_task::Commanded RTL." <<  NORMAL_CONSOLE_TEXT << std::endl;
    }
  }
  //Done with mission
  //No need for this thread
  //Exit thread
}
