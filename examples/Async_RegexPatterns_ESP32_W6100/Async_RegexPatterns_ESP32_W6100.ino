/****************************************************************************************************************************
  Async_RegexPatterns_ESP32_W6100.ino

  For W6100 LwIP Ethernet in ESP32_SC_W6100 (ESP32_S2/3, ESP32_C3 + W6100)

  AsyncWebServer_ESP32_SC_W6100 is a library for the LwIP Ethernet W6100 in ESP32_S2/3, ESP32_C3 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_ESP32_SC_W6100
  Licensed under GPLv3 license
 *****************************************************************************************************************************/
//
// A simple server implementation with regex routes:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//
// Add buildflag ASYNCWEBSERVER_REGEX to enable the regex support
//
// For platformio: platformio.ini:
//  build_flags =
//      -DASYNCWEBSERVER_REGEX
//
// For arduino IDE: create/update platform.local.txt
// Windows: C:\Users\(username)\AppData\Local\Arduino15\packages\espxxxx\hardware\espxxxx\{version}\platform.local.txt
// Linux: ~/.arduino15/packages/espxxxx/hardware/espxxxx/{version}/platform.local.txt
//
// compiler.cpp.extra_flags=-DASYNCWEBSERVER_REGEX=1

#define ASYNCWEBSERVER_REGEX      true

#if !( defined(ESP32) )
  #error This code is designed for (ESP32_S2/3, ESP32_C3 + W6100) to run on ESP32 platform! Please check your Tools->Board setting.
#endif

#include <Arduino.h>

#define _ASYNC_WEBSERVER_LOGLEVEL_       2

// Enter a MAC address and IP address for your controller below.
#define NUMBER_OF_MAC      20

byte mac[][NUMBER_OF_MAC] =
{
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x02 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x03 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x04 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x05 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x06 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x07 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x08 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x09 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0A },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0B },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0C },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0D },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x0E },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x0F },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x10 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x11 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x12 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x13 },
  { 0xDE, 0xAD, 0xBE, 0xEF, 0xBE, 0x14 },
};

// Select the IP address according to your local network
IPAddress myIP(192, 168, 2, 232);
IPAddress myGW(192, 168, 2, 1);
IPAddress mySN(255, 255, 255, 0);

// Google DNS Server IP
IPAddress myDNS(8, 8, 8, 8);

//////////////////////////////////////////////////////////

// For ESP32-S3
// Optional values to override default settings
// Don't change unless you know what you're doing
//#define ETH_SPI_HOST        SPI3_HOST
//#define SPI_CLOCK_MHZ       25

// Must connect INT to GPIOxx or not working
//#define INT_GPIO            4

//#define MISO_GPIO           13
//#define MOSI_GPIO           11
//#define SCK_GPIO            12
//#define CS_GPIO             10

// For ESP32_C3
// Optional values to override default settings
// Don't change unless you know what you're doing
//#define ETH_SPI_HOST        SPI2_HOST
//#define SPI_CLOCK_MHZ       25

// Must connect INT to GPIOxx or not working
//#define INT_GPIO            10

//#define MISO_GPIO           5
//#define MOSI_GPIO           6
//#define SCK_GPIO            4
//#define CS_GPIO             7

//////////////////////////////////////////////////////////

#include <AsyncTCP.h>

#include <AsyncWebServer_ESP32_SC_W6100.h>

AsyncWebServer    server(80);

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  delay(500);

  Serial.print(F("\nStart Async_RegexPatterns_ESP32_W6100 on "));
  Serial.print(ARDUINO_BOARD);
  Serial.print(F(" with "));
  Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_WEBSERVER_ESP32_SC_W6100_VERSION);

  AWS_LOGWARN(F("Default SPI pinout:"));
  AWS_LOGWARN1(F("SPI_HOST:"), ETH_SPI_HOST);
  AWS_LOGWARN1(F("MOSI:"), MOSI_GPIO);
  AWS_LOGWARN1(F("MISO:"), MISO_GPIO);
  AWS_LOGWARN1(F("SCK:"),  SCK_GPIO);
  AWS_LOGWARN1(F("CS:"),   CS_GPIO);
  AWS_LOGWARN1(F("INT:"),  INT_GPIO);
  AWS_LOGWARN1(F("SPI Clock (MHz):"), SPI_CLOCK_MHZ);
  AWS_LOGWARN(F("========================="));

  ///////////////////////////////////

  // To be called before ETH.begin()
  ESP32_W6100_onEvent();

  // start the ethernet connection and the server:
  // Use DHCP dynamic IP and random mac
  //bool begin(int MISO_GPIO, int MOSI_GPIO, int SCLK_GPIO, int CS_GPIO, int INT_GPIO, int SPI_CLOCK_MHZ,
  //           int SPI_HOST, uint8_t *W6100_Mac = W6100_Default_Mac);
  ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST );
  //ETH.begin( MISO_GPIO, MOSI_GPIO, SCK_GPIO, CS_GPIO, INT_GPIO, SPI_CLOCK_MHZ, ETH_SPI_HOST, mac[millis() % NUMBER_OF_MAC] );

  // Static IP, leave without this line to get IP via DHCP
  //bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = 0, IPAddress dns2 = 0);
  //ETH.config(myIP, myGW, mySN, myDNS);

  ESP32_W6100_waitForConnect();

  ///////////////////////////////////

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(200, "text/plain", "Hello, world from Async_RegexPatterns_ESP32_W6100 on " + String(ARDUINO_BOARD));
  });

  // Send a GET request to <IP>/sensor/<number>
  server.on("^\\/sensor\\/([0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request)
  {
    String sensorNumber = request->pathArg(0);
    request->send(200, "text/plain", "Hello, sensor: " + sensorNumber);
  });

  // Send a GET request to <IP>/sensor/<number>/action/<action>
  server.on("^\\/sensor\\/([0-9]+)\\/action\\/([a-zA-Z0-9]+)$", HTTP_GET, [] (AsyncWebServerRequest * request)
  {
    String sensorNumber = request->pathArg(0);
    String action = request->pathArg(1);
    request->send(200, "text/plain", "Hello, sensor: " + sensorNumber + ", with action: " + action);
  });

  server.onNotFound(notFound);

  server.begin();

  Serial.print("Server started @ ");
  Serial.println(ETH.localIP());
}

void loop()
{
}
