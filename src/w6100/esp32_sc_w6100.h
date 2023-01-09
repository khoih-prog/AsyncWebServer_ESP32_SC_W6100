/****************************************************************************************************************************
  esp32_sc_w6100.h

  For W6100 LwIP Ethernet in ESP32_SC_W6100 (ESP32_S2/S3/C3 + W6100)

  AsyncWebServer_ESP32_SC_W6100 is a library for the LwIP Ethernet W6100 in ESP32_S2/S3/C3 to run AsyncWebServer

  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncWebServer_ESP32_SC_W6100
  Licensed under GPLv3 license

  Version: 1.8.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.8.1   K Hoang      08/01/2023 Initial porting for W6100 + ESP32_S2/S3/C3. Sync with AsyncWebServer_ESP32_W5500 v1.8.1
 *****************************************************************************************************************************/

#pragma once

#ifndef _ESP32_SC_W6100_H_
#define _ESP32_SC_W6100_H_

////////////////////////////////////////

#include "WiFi.h"
#include "esp_system.h"
#include "esp_eth.h"

#include <hal/spi_types.h>

////////////////////////////////////////

#if ESP_IDF_VERSION_MAJOR < 4 || ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4,4,0)
  #error "This version of Arduino is too old"
#endif

////////////////////////////////////////

static uint8_t W6100_Default_Mac[] = { 0xFE, 0xED, 0xDE, 0xAD, 0xBE, 0xEF };

////////////////////////////////////////

class ESP32_W6100
{
  private:
    bool initialized;
    bool staticIP;

    uint8_t mac_eth[6] = { 0xFE, 0xED, 0xDE, 0xAD, 0xBE, 0xEF };

#if ESP_IDF_VERSION_MAJOR > 3
    esp_eth_handle_t eth_handle;

  protected:
    bool started;
    eth_link_t eth_link;
    static void eth_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
#else
    bool started;
    eth_config_t eth_config;
#endif

  public:
    ESP32_W6100();
    ~ESP32_W6100();

    bool begin(int MISO, int MOSI, int SCLK, int CS, int INT, int SPICLOCK_MHZ = 25, int SPIHOST = SPI3_HOST,
               uint8_t *W6100_Mac = W6100_Default_Mac);

    bool config(IPAddress local_ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0x00000000,
                IPAddress dns2 = (uint32_t)0x00000000);

    const char * getHostname();
    bool setHostname(const char * hostname);

    bool fullDuplex();
    bool linkUp();
    uint8_t linkSpeed();

    bool enableIpV6();
    IPv6Address localIPv6();

    IPAddress localIP();
    IPAddress subnetMask();
    IPAddress gatewayIP();
    IPAddress dnsIP(uint8_t dns_no = 0);

    IPAddress broadcastIP();
    IPAddress networkID();
    uint8_t subnetCIDR();

    uint8_t * macAddress(uint8_t* mac);
    String macAddress();

    friend class WiFiClient;
    friend class WiFiServer;
};

////////////////////////////////////////

extern ESP32_W6100 ETH;

////////////////////////////////////////

#endif /* _ESP32_SC_W6100_H_ */
