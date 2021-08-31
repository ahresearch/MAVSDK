#include "mavsdk_helper.h"
#include <iostream>
#include <functional>
#include <future> //
#include <iostream>
#include <memory>
#include <chrono>
using namespace std::this_thread;
using namespace std::chrono;

Mavsdk dc;

void handle_action_err_exit(Action::Result result, const std::string& message)
{
    if (result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

void handle_mission_err_exit(Mission::Result result, const std::string& message)
{
    if (result != Mission::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles connection result
void handle_connection_err_exit(ConnectionResult result, const std::string& message)
{
    if (result != ConnectionResult::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}



void init_mavsdk(std::string connection_url)
{
    auto prom = std::make_shared<std::promise<void>>();
    auto future_result = prom->get_future();
    ConnectionResult connection_result;
    bool discovered_system = false;

    std::cout << "Connecting to URL: " << connection_url << std::endl;

    connection_result = dc.add_any_connection(connection_url);
    handle_connection_err_exit(connection_result, "Connection failed: ");

    std::cout << GREEN_CONSOLE_TEXT << "Waiting to discover  drone..." << NORMAL_CONSOLE_TEXT << std::endl;
    dc.subscribe_on_new_system([&discovered_system]() {
        const auto system = dc.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            discovered_system = true;
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2
    // seconds.
    sleep_for(seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        exit (1);
    }
}
