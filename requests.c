#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: Write the method name, URL, request params (if any), and protocol type
    if (query_params != NULL) {
        snprintf(line, LINELEN, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        snprintf(line, LINELEN, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // Step 2: Add the host
    snprintf(line, LINELEN, "Host: %s", host);
    compute_message(message, line);

    // Step 3: Add JWT token if present
    if (jwt != NULL) {
        snprintf(line, LINELEN, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // Step 4: Add cookies if present
    if (cookies != NULL) {
        snprintf(line, LINELEN, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Step 5: Add final new line
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                           int body_data_fields_count, char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(BUFLEN, sizeof(char));

    // Step 1: Write the method name, URL, and protocol type
    snprintf(line, LINELEN, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Step 2: Add the host
    snprintf(line, LINELEN, "Host: %s", host);
    compute_message(message, line);

    // Step 3: Add necessary headers (Content-Type and Content-Length are mandatory)
    int body_data_size = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
        if (i < body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }
    body_data_size = strlen(body_data_buffer);
    snprintf(line, LINELEN, "Content-Type: %s\r\nContent-Length: %d", content_type, body_data_size);
    compute_message(message, line);

    // Step 4: Add JWT token if present
    if (jwt != NULL) {
        snprintf(line, LINELEN, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // Step 5: Add cookies if present
    if (cookies != NULL) {
        snprintf(line, LINELEN, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Step 6: Add new line at end of header
    compute_message(message, "");

    // Step 7: Add body data
    strcat(message, body_data_buffer);

    free(body_data_buffer);
    free(line);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                             char **cookies, int cookies_count, char *jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: Write the method name, URL, request params (if any), and protocol type
    if (query_params != NULL) {
        snprintf(line, LINELEN, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        snprintf(line, LINELEN, "DELETE %s HTTP/1.1", url);
    }
    compute_message(message, line);

    // Step 2: Add the host
    snprintf(line, LINELEN, "Host: %s", host);
    compute_message(message, line);

    // Step 3: Add JWT token if present
    if (jwt != NULL) {
        snprintf(line, LINELEN, "Authorization: Bearer %s", jwt);
        compute_message(message, line);
    }

    // Step 4: Add cookies if present
    if (cookies != NULL) {
        snprintf(line, LINELEN, "Cookie: ");
        for (int i = 0; i < cookies_count; i++) {
            strcat(line, cookies[i]);
            if (i < cookies_count - 1) {
                strcat(line, "; ");
            }
        }
        compute_message(message, line);
    }

    // Step 5: Add final new line
    compute_message(message, "");

    free(line);
    return message;
}
