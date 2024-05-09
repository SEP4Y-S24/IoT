#include "ccp_protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi.h"
#include "logger.h"

const char *LINE_TERMINATOR = "\r\n";
void ccp_at_to_string(CCP_ACTION_TYPE at, char *action_type);
const CCP_STATUS_CODE status_code_from_string(char *code);
const char *status_code_to_string(CCP_STATUS_CODE code);

response ccp_parse_response(char *raw_response)
{
    response response = {CCP_AT_UNKNOWN, CCP_STATUS_BAD_REQUEST, {0}};
    if (raw_response == NULL)
    {
        return response;
    }

    char *response_parts[4]; // Array to store message parts (actionType, Response Code, Body length, Body)
    int num_parts = 0;
    char *token = strtok(raw_response, LINE_TERMINATOR);

    while (token != NULL)
    {
        response_parts[num_parts] = token;
        log_info(response_parts[num_parts]);
        log_info("Next one.");
        num_parts++;
        token = strtok(NULL, LINE_TERMINATOR);
    }

    int body_length = atoi(response_parts[2]);

    if (body_length < 0 || body_length > CCP_MAX_BODY_LENGTH)
    {
        char error_response[35];
        ccp_create_response(error_response, ccp_at_from_str(response_parts[0]), CCP_STATUS_BAD_REQUEST, "Max Body Length is: 96.");
        uint8_t response_data[35];
        memcpy(response_data, error_response, strlen(error_response));
        wifi_command_TCP_transmit(response_data, 35);
        return response;
    }

    response.action_type = ccp_at_from_str(response_parts[0]);
    response.status_code = status_code_from_string(response_parts[1]);

    strncpy(response.body, response_parts[3], body_length);
    response.body[body_length] = '\0';

    return response;
}

void ccp_create_request(CCP_ACTION_TYPE at, char *body, char *request_buffer)
{
    int length_of_message = strlen(body);
    char at_str[3];
    ccp_at_to_string(at, at_str);
    char length_string[10];
    sprintf(length_string, "%d", length_of_message);
    strcpy(request_buffer, at_str);
    strcat(request_buffer, LINE_TERMINATOR);
    strcat(request_buffer, length_string);
    strcat(request_buffer, LINE_TERMINATOR);
    strcat(request_buffer, body);
    strcat(request_buffer, LINE_TERMINATOR);
}

void ccp_create_response(char *response_buffer, CCP_ACTION_TYPE at, CCP_STATUS_CODE code, char *body)
{
    char at_str[9];
    ccp_at_to_string(at, at_str);
    int length_of_message = strlen(body);
    char length_string[10];
    sprintf(length_string, "%d", length_of_message);
    strcpy(response_buffer, at_str);
    strcat(response_buffer, LINE_TERMINATOR);
    strcat(response_buffer, status_code_to_string(code));
    strcat(response_buffer, LINE_TERMINATOR);
    strcat(response_buffer, length_string);
    strcat(response_buffer, LINE_TERMINATOR);
    strcat(response_buffer, body);
    strcat(response_buffer, LINE_TERMINATOR);
}

const char *status_code_strings[] = {
    [CCP_STATUS_BAD_REQUEST] = "0",
    [CCP_STATUS_OK] = "1",
    [CCP_STATUS_SERVER_ERROR] = "2",
    [CCP_STATUS_UNAUTHENTICATED] = "3",
    [CCP_STATUS_NUM_STATUS_CODES] = "Unknown" // Default string for unknown status codes
};

const char *status_code_to_string(CCP_STATUS_CODE code)
{
    if (code >= 0 && code < CCP_STATUS_NUM_STATUS_CODES)
        return status_code_strings[code];
    else
        return status_code_strings[CCP_STATUS_NUM_STATUS_CODES]; // Return the default string
}

const CCP_STATUS_CODE status_code_from_string(char *code)
{
    for (int i = 0; i < CCP_STATUS_NUM_STATUS_CODES; ++i)
    {
        if (strcmp(code, status_code_strings[i]) == 0)
            return (CCP_STATUS_CODE)i;
    }
    return CCP_STATUS_NUM_STATUS_CODES; // Return the default status code for unknown strings
}

CCP_ACTION_TYPE ccp_at_from_str(char *message)
{
    if (strncmp(message, "TM", 2) == 0)
    {
        return CCP_AT_TM;
    }
    else if (strncmp(message, "MS", 2) == 0)
    {
        return CCP_AT_MS;
    }
    else
    {
        return CCP_AT_UNKNOWN;
    }
}

void ccp_at_to_string(CCP_ACTION_TYPE at, char *action_type)
{
    switch (at)
    {
    case CCP_AT_TM:
        strcpy(action_type, "TM");
        break;
    case CCP_AT_MS:
        strcpy(action_type, "MS");
        break;
    default:
        strcpy(action_type, "Unknown");
        break;
    }
}