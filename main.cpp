/*
 * mbed Microcontroller Library
 * Copyright (c) 2006-2016 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file fopen.cpp Test cases to POSIX file fopen() interface.
 *
 * Please consult the documentation under the test-case functions for
 * a description of the individual test case.
 */

#include "mbed.h"

//#include "utest/utest.h"
//#include "unity/unity.h"
//#include "greentea-client/test_env.h"

#include "mbed-trace/mbed_trace.h"
#define TRACE_GROUP  "main"

#include <string>

#include "SeggerRTT.h"
#include "WINC1500Interface.h"

#define MBED_CONF_APP_PROTAGONIST_DOWNLOAD                                    "alice.h"                                                                                        // set by application[*]

//using namespace utest::v1;

#include MBED_CONF_APP_PROTAGONIST_DOWNLOAD
#include "certificate_aws_s3.h"


#define OVERRIDE_CONSOLE_STDIO_UART_TX 0
#define OVERRIDE_CONSOLE_SEGGER_RTT 1


static volatile bool event_fired = false;

//NetworkInterface* interface = NULL;
WiFiInterface* interface = NULL;

#define MAX_RETRIES 3

const char part1[] = "GET /firmware/";
const char filename[] = MBED_CONF_APP_PROTAGONIST_DOWNLOAD;
const char part2[] = "txt HTTP/1.1\nHost: lootbox.s3.dualstack.us-west-2.amazonaws.com\n\n";

static void socket_event(void)
{
    event_fired = true;
}

void download(size_t size)
{
    int result = -1;
    printf("!!### Start of download routine, size: %d\r\n", size);

    /* setup TLS socket */
    TLSSocket* socket = new TLSSocket();

    result = socket->open(interface);
    if (result != 0) {
         printf("!!### Socket open error: %d\r\n", result);
         return;
    }

   // TEST_ASSERT_NOT_NULL_MESSAGE(socket, "failed to instantiate socket");
    printf("!!### Socket is opened\r\n");

    result = socket->set_root_ca_cert(SSL_CA_PEM);
   // TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "failed to set root CA");
    printf("!!### Sertificate is set, res: %d\r\n", result);

    for (int tries = 0; tries < MAX_RETRIES; tries++) {
        result = socket->connect("lootbox.s3.dualstack.us-west-2.amazonaws.com", 443);
        printf("!!### Connect to server, res: %d\r\n", result);
        if (result == 0) {
            break;
        }
        printf("connection failed. retry %d of %d\r\n", tries, MAX_RETRIES);
    }
   // TEST_ASSERT_EQUAL_INT_MESSAGE(0, result, "failed to connect");
    printf("!!### Socket connection, res: %d\r\n", result);

    socket->set_blocking(false);
    printf("non-blocking mode set\r\n");

    socket->sigio(socket_event);
    printf("registered callback function\r\n");

    /* setup request */
    /* -1 to remove h from .h in header file name */
    size_t request_size = strlen(part1) + strlen(filename) - 1 + strlen(part2) + 1;
    char *request = new char[request_size]();

    /* construct request */
    memcpy(&request[0], part1, strlen(part1));
    memcpy(&request[strlen(part1)], filename, strlen(filename) - 1);
    memcpy(&request[strlen(part1) + strlen(filename) - 1], part2, strlen(part2));

    printf("request: %s[end]\r\n", request);

    /* send request to server */
    result = socket->send(request, request_size);
   // TEST_ASSERT_EQUAL_INT_MESSAGE(request_size, result, "failed to send HTTP request");

    /* read response */
    char* receive_buffer = new char[size];
   // TEST_ASSERT_NOT_NULL_MESSAGE(receive_buffer, "failed to allocate receive buffer");

    size_t expected_bytes = size;
//    size_t expected_bytes = sizeof(story);
    size_t received_bytes = 0;
    uint32_t body_index = 0;

    /* loop until all expected bytes have been received */
    while (received_bytes < expected_bytes)
    {
        /* wait for async event */
        while(!event_fired);
        event_fired = false;

        /* loop until all data has been read from socket */
        do
        {
            result = socket->recv(receive_buffer, size);
           // TEST_ASSERT_MESSAGE((result == NSAPI_ERROR_WOULD_BLOCK) || (result >= 0), "failed to read socket");

//            printf("result: %d\r\n", result);

            if (result > 0)
            {
                /* skip HTTP header */
                if (body_index == 0)
                {
                    std::string header(receive_buffer, result);
                    body_index = header.find("\r\n\r\n");
                   // TEST_ASSERT_MESSAGE(body_index != std::string::npos, "failed to find body");

                    /* remove header before comparison */
                    memmove(receive_buffer, &receive_buffer[body_index + 4], result - body_index - 4);

                   // TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(story,
                    //                                                        receive_buffer,
                    //                                   result - body_index - 4,
                    //                                   "character mismatch");

                    received_bytes += (result - body_index - 4);
                }
                else
                {
                   // TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE(&story[received_bytes],
//                                                         receive_buffer,
                    //                                                         result,
                    //                                   "character mismatch");

                    received_bytes += result;
                }

//                receive_buffer[result] = '\0';
//                printf("%s", receive_buffer);
                printf("received_bytes: %u\r\n", received_bytes);
            }
        }
        while ((result > 0) && (received_bytes < expected_bytes));
    }

    delete request;
    delete socket;
    delete[] receive_buffer;

    printf("done\r\n");
}

static int setup_network(const size_t call_count)
{
    printf("!!### setup_network start! call_count: %d\r\n", call_count);
    interface = &WINC1500Interface::getInstance();
//    interface = NetworkInterface::get_default_instance();
   // TEST_ASSERT_NOT_NULL_MESSAGE(interface, "failed to initialize network");

    nsapi_error_t err = -1;

    for (int tries = 0; tries < MAX_RETRIES; tries++) {
        err = interface->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2, M2M_WIFI_CH_ALL);

        if (err == NSAPI_ERROR_OK) {
            break;
        } else {

            printf("Error connecting to network. Retrying %d of %d\r\n", tries, MAX_RETRIES);
        }
    }

   // TEST_ASSERT_EQUAL(NSAPI_ERROR_OK, err);
    printf("IP address is '%s'\r\n", interface->get_ip_address());
    printf("MAC address is '%s'\r\n", interface->get_mac_address());

    return 0;
}

static int download_1k(const size_t call_count)
{
    download(1024);

    return 0;
}
#if 0
static control_t download_2k(const size_t call_count)
{
    download(2*1024);

    return 0;
}

static control_t download_4k(const size_t call_count)
{
    download(4*1024);

    return CaseNext;
}

static control_t download_8k(const size_t call_count)
{
    download(8*1024);

    return CaseNext;
}

static control_t download_16k(const size_t call_count)
{
    download(16*1024);

    return CaseNext;
}

static control_t download_32k(const size_t call_count)
{
    download(32*1024);

    return CaseNext;
}

utest::v1::status_t greentea_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(30*60, "default_auto");
    return greentea_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("Setup network", setup_network),
    Case("Download  1k", download_1k),
    Case("Download  2k", download_2k),
    Case("Download  4k", download_4k),
    Case("Download  8k", download_8k),
    Case("Download 16k", download_16k),
    Case("Download 32k", download_32k),
};

Specification specification(greentea_setup, cases);
#endif

#if (OVERRIDE_CONSOLE_STDIO_UART_TX || OVERRIDE_CONSOLE_SEGGER_RTT)
FileHandle* mbed::mbed_override_console(int fd) {
#if OVERRIDE_CONSOLE_STDIO_UART_TX
    static UARTSerial console(STDIO_UART_TX, STDIO_UART_RX, MBED_CONF_PLATFORM_STDIO_BAUD_RATE);
    return &console;
#elif OVERRIDE_CONSOLE_SEGGER_RTT
    static SeggerRTT rtt;
    return &rtt;
#endif
}
#endif  // (OVERRIDE_CONSOLE_STDIO_UART_TX || OVERRIDE_CONSOLE_SEGGER_RTT)

#if 0
WiFiInterface *WiFiInterface::get_default_instance()
{
    static WINC1500Interface &winc = WINC1500Interface::getInstance();
    return &winc;
}

NetworkInterface *NetworkInterface::get_default_instance()
{
    static WINC1500Interface &winc = WINC1500Interface::getInstance();
    return &winc;
}
#endif

extern "C" void printf_all(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int main()
{
    mbed_trace_init();       // initialize the trace library
    tr_debug("Hello world!");  //-> "[DBG ][main]: Hello world!"

    printf("\r\n!#\r\n!#\r\----Mbed initialized\r\n");
    wait(1);
    printf("!!### Before harness start!\r\n");
    int res1 = setup_network(2);
    printf("!!### setup_network PASSED!\r\n");
    wait(3);
    res1 = download_1k(3);
//    return !Harness::run(specification);
//    bool res = Harness::run(specification);

//    printf("!!### End of program, res: %s!\r\n", res ? "True" : "False");
    printf("!!### End of program, r!\r\n");
    for (;;) ;
    return 0;
}
