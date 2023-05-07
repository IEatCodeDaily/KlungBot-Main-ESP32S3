#ifndef ESP_NOW_HANDLER_H
#define ESP_NOW_HANDLER_H

#include <WiFi.h>
#include <esp_now.h>
#include "ESPNowPacket.h"
#include "params.h"
#include "ConfigManager.h"
#include "PeerManager.h"


#define CHANNEL_DEFAULT 1
const uint8_t BROADCAST_MAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#ifndef PEER_MANAGER
#define PEER_MANAGER
//PeerManager peerManager;
#endif

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void espNowRecv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void espNow_init();

bool espNow_removePeer(const String &macAddrStr);
bool espNow_addPeer(const String &macAddr);
void espNow_peerInit();

#endif