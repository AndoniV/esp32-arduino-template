/*******************************************************************************
 @author    A Villarreal
 @date      15/02/23
 @file      main.cpp
 @brief     Main application "sketch"
 ******************************************************************************/

#include <Arduino.h>
#include <InputDebounce.h>
#include <arduino-timer.h>
#include <ArduinoLog.h>
#include <WiFi.h>
#include "version.h"
#include "board.h"
#include "app.h"

// DEFINITIONS ****************************************************************/

/** Timeout in milliseconds for app state machine timer */
#define APP_TIMER_TIMEOUT_MS    (5)
/** Timout in milliseconds for the APPS_INIT state */
#define APP_INIT_TIMEOUT_MS     (15000)

// DECLARATIONS ***************************************************************/

/** Whether its first state entry */
static bool s_app_first_state_entry = true;
/** State machine flags */
static AppFlags s_app_flags = {0};
/** State machine time-keeping counters */
static AppTimerCounters s_app_timer_counters = {0};
/** State machine counters */
static AppCounters s_app_counters = {0};
/** State machine current state */
static AppState s_app_state = APPS_NULL;
/** Monitor timer for app state machine */
auto s_app_timer = timer_create_default();
/** Wifi SSID and password */
static const char* app_wifi_ssid = "<SSID>";
static const char* app_wifi_pswd = "<PWD>";

/**
 * Servie the app state machine
 */
static void app_service(void);
/**
 * Set the state machine state
 * @param state State to set
 */
static void app_set_state(AppState state);
/**
 * Gets the state name
 * @param state The state name
 * @return The state name
 */
static const char* app_get_state_name(AppState state);
/**
 * Monitor timer callback
 * @param arg Timer argument
 */
static bool app_timer_callback(void *arg);

// IMPLEMENTATIONS ************************************************************/

void setup(void)
{
    // Initialise serial port and Logging module
    Serial.begin(115200);
    while (!Serial && !Serial.available()) { /* Do nothing */ }
    Log.begin(LOG_LEVEL_VERBOSE, &Serial);
    Log.notice("********************************************************************************\n");
    Log.notice("Firmware v%d.%d\n", FW_VERSION_MAJ, FW_VERSION_MIN);
    Log.notice("Built %s %s\n", __DATE__, __TIME__);
    Log.notice("********************************************************************************\n");

    // Configure state machine timer
    s_app_timer.every(APP_TIMER_TIMEOUT_MS, app_timer_callback);
    // Set WiFi mode to station mode
    WiFi.mode(WIFI_STA);
    // Set initial app state
    app_set_state(APPS_INIT);
}

void loop(void)
{
    // Service any other modules here
    // NOTE: Service app state machine
    app_service();
}

/**
 * Servie the app state machine
 */
static void app_service(void)
{
    // Service the application timer
    s_app_timer.tick();
    // NOTE: Service any inputs here

    switch (s_app_state)
    {
    // ========================================================================/
    case APPS_IDLE:
        if (s_app_first_state_entry)
        {
            s_app_first_state_entry = false;
        } // if: first state entry
    // ========================================================================/
    case APPS_INIT:
        if (s_app_first_state_entry)
        {
            s_app_first_state_entry = false;
            s_app_timer_counters.init_count = 0;
            Log.notice("[APP] Connecting to SSID: %s...\n", app_wifi_ssid);
            WiFi.begin(app_wifi_ssid, app_wifi_pswd);
        } // if: first state entry
        if (WiFi.isConnected() || (s_app_timer_counters.init_count >= APP_INIT_TIMEOUT_MS))
        {
            Log.notice("[APP] %s to: %s %s %s\n", WiFi.isConnected() ? "Connected" : "Connection failed",  //
                                                    app_wifi_ssid,                                         //
                                                    WiFi.isConnected() ? "with IP: " : "",                 //
                                                    WiFi.isConnected() ? WiFi.localIP().toString() : "");
            app_set_state(APPS_IDLE);
        }
        break;
    // ========================================================================/
    case APPS_NULL:
    default:
        break;
    } // switch: state
}

/**
 * Set the state machine state
 */
static void app_set_state(AppState state)
{
    s_app_first_state_entry = true;
    s_app_state = state;
    const char* name = app_get_state_name(s_app_state);
    Log.info("[APP] State set: %s\n", name);
}

/**
 * Gets the state name
 */
static const char* app_get_state_name(AppState state)
{
    switch (state)
    {
    case APPS_IDLE:         return "APPS_IDLE";
    case APPS_INIT:         return "APPS_INIT";
    case APPS_NULL:
    default:
                            return "APPS_NULL";
    } // switch: state
}

/**
 * Monitor timer callback
 */
static bool app_timer_callback(void *arg)
{
    switch (s_app_state)
    {
    // ========================================================================/
    case APPS_INIT:
        s_app_timer_counters.init_count += APP_TIMER_TIMEOUT_MS;
        break;
    // ========================================================================/
    case APPS_IDLE:
    case APPS_NULL:
    default:
        break;
    } // switch: state
    return true;
}
