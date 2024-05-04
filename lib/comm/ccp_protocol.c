#include "ccp_protocol.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *LINE_TERMINATOR = "\r\n";

// char *ccp_create_request(char *at, char *body)
// {
//     int length_of_message = strlen(body);
//     char request[length_of_message];
//     strcpy(at, request);
//     strcat(LINE_TERMINATOR, request);
//     strcat((char *)length_of_message, request);
//     strcat(LINE_TERMINATOR, request);
//     strcat(body, request);
//     strcat(LINE_TERMINATOR, request);
//     return request;
// }

// TODO: Check if struct is not better? Do TESTs
char *ccp_parse_response(char *response, char *status_buffer, int status_buffer_size)
{
    if (response == NULL)
        return NULL;

    char *response_parts[4]; // Array to store message parts (actionType, Response Code, Body length, Body)
    int num_parts = 0;
    char *token = strtok(response, LINE_TERMINATOR);

    while (token != NULL && num_parts < 4)
    {
        response_parts[num_parts++] = token;
        token = strtok(NULL, LINE_TERMINATOR);
    }

    char *actionType = response_parts[0];
    char *status = response_parts[1];
    int body_length = atoi(response_parts[2]);

    char *body = NULL;
    if (body_length > 0)
        body = response_parts[3];

    if (status_buffer != NULL && status_buffer_size > strlen(status))
    {
        strncpy(status_buffer, status, status_buffer_size - 1);
        status_buffer[status_buffer_size - 1] = '\0';
    }

    return body;
}

const char *status_code_to_string(CCP_STATUS_CODE code);

void ccp_create_request(char *at, char *body, char *request)
{
    int length_of_message = strlen(body);
    char length_string[10];
    sprintf(length_string, "%d", length_of_message);
    strcpy(request, at);
    strcat(request, LINE_TERMINATOR);
    strcat(request, length_string);
    strcat(request, LINE_TERMINATOR);
    strcat(request, body);
    strcat(request, LINE_TERMINATOR);
}

void ccp_create_response(char *response, char *at, CCP_STATUS_CODE code, char *body)
{
    int length_of_message = strlen(body);
    char length_string[10];
    sprintf(length_string, "%d", length_of_message);
    strcpy(response, at);
    strcat(response, LINE_TERMINATOR);
    strcat(response, status_code_to_string(code));
    strcat(response, LINE_TERMINATOR);
    strcat(response, length_string);
    strcat(response, LINE_TERMINATOR);
    strcat(response, body);
    strcat(response, LINE_TERMINATOR);
}

const char *status_code_strings[] = {
    [CCP_STATUS_OK] = "1",
    [CCP_STATUS_SERVER_ERROR] = "2",
    [CCP_STATUS_BAD_REQUEST] = "0",
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

CCP_ACTION_TYPE ccp_at_from_str(char *action_type)
{
    char at[3];
    snprintf(at, sizeof at, "%.3s", action_type);
    at[3] = '\0';
    if (strcmp(at, "TM") == 0)
    {
        return CCP_AT_TM;
    }
    else if (strcmp(at, "MS") == 0)
    {
        return CCP_AT_MS;
    }
    else{
        return CCP_AT_UNKNOWN;
    }
}