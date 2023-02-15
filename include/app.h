/*******************************************************************************
 @author    A Villarreal
 @date      15/02/23
 @file      app.h
 @brief     Provides definitions of app's data structures
 ******************************************************************************/

#ifndef APP_H
#define APP_H

// DEFINITIONS ****************************************************************/

// DECLARATIONS ***************************************************************/

/**
 * Enummeration for state machine states
 */
typedef enum
{
    APPS_IDLE,  /// Idle state
    APPS_INIT,  /// Initialising state
    APPS_NULL   /// No state
} AppState;

/**
 * Encapsulation for state machine flags
 */
typedef union
{
    struct
    {
        /** Reserved bits */
        uint32_t reserved : 32;
    };
    uint32_t flags;
} AppFlags;

/**
 * Encapsulation for state machine time keeping counters
 */
typedef struct
{
    /** Counter for APPS_INIT state */
    uint16_t init_count;
} AppTimerCounters;

/**
 * Encapsulation for state machine counters
 */
typedef struct
{
    /** Test cycle counter */
    uint16_t placeholder;
} AppCounters;

#endif // APP_H