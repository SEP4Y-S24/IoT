#include <stdbool.h>
#include <state_coordinator.h>
#include <key_verification_state.h>
#include "key_verification_callback.h"
#include "logger.h"
#include "message.h"
#include "string.h"
#include "wifi.h"

static bool waiting_for_key_verification;
static char buffer[120];

void key_verification_callback_wrapper()
{
    key_verification_callback(buffer);
}


State key_verification_state_switch(char *key)
{
    wifi_reassign_callback(key_verification_callback_wrapper, buffer);
    log_debug("Switching to key verification state");
    waiting_for_key_verification = false;

    state_coordinator_wait_for_event(&waiting_for_key_verification);

    char no_message[] = "No message received";
    message_set_message(no_message);
    return WORKING_STATE;
}

void key_verification_state_set_waiting_for_key_verification(bool waiting)
{
    waiting_for_key_verification = waiting;
}

void key_verification_state_set_key(char *new_key)
{
    message_set_message(new_key);
}