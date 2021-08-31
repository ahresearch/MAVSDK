#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define GREEN_CONSOLE_TEXT "\033[32m" // Turn text on console green
#define TELEMETRY_CONSOLE_TEXT "\033[33m" // Turn text on console yellow
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

using namespace mavsdk;

extern Mavsdk dc;

void init_mavsdk(std::string connection_url);

// Handles Action's result
void handle_action_err_exit(Action::Result result, const std::string& message);

// Handles Mission's result
void handle_mission_err_exit(Mission::Result result, const std::string& message);

// Handles Connection result
void handle_connection_err_exit(ConnectionResult result, const std::string& message);
