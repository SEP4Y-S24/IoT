#include "working_state.h"
#include "state_coordinator.h"
#include "periodic_request.h"
#include "../drivers/wifi.h"
#include "../drivers/periodic_task.h"
#include "../comm/ccp_message_handler.h"
#include "../comm/ccp_message_sender.h"
#include "../utils/logger.h"
#include "../utils/native.h"

static char message_buffer[CCP_MAX_MESSAGE_LENGTH];

static bool error;

static void tcp_callback()
{
    ccp_message_handler_handle(message_buffer);
}

State working_state_switch()
{
    error = false;
    wifi_reassign_callback(tcp_callback, message_buffer);

    log_info("Entered working state");

    ccp_message_sender_send_request(CCP_AT_TM, "");
    native_delay_ms(2000);

    periodic_request_10_minutes_init();

    state_coordinator_wait_for_event(&error);

    periodic_request_10_minutes_stop();
    return WORKING_STATE; // Does not matter cause it only return in case of error
}
