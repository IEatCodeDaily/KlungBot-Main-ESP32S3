#include "ESPNowHandler.hpp"

PeerManager peerManager;

void espNowSend_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    ESP_LOGD(TAG_NETWORK,"Message Sent to: %x", mac_addr);
  // Handle the status of sent messages
  // ...
}

void espNowRecv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len){
    // Handle the status of received messages
    // ...
}

void espNow_init() {
  // Set up WiFi in STA mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  //Initialize all the peers
  espNow_peerInit();
  // Set up ESP-NOW callback
  esp_now_register_send_cb(espNowSend_cb);
  esp_now_register_recv_cb(espNowRecv_cb);
}

bool espNow_addPeer(const String &macAddrStr) {
  uint8_t macAddr[6];

  // Convert MAC address string to byte array
  sscanf(macAddrStr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &macAddr[0], &macAddr[1], &macAddr[2], &macAddr[3], &macAddr[4], &macAddr[5]);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, macAddr, 6);
  peerInfo.channel = 0; // Use the default Wi-Fi channel
  peerInfo.encrypt = false; // Disable encryption

  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus == ESP_OK) {
    return true;
  } else {
    return false;
  }
}

bool espNow_removePeer(const String &macAddrStr) {
  uint8_t macAddr[6];
   // Convert MAC address string to byte array
  sscanf(macAddrStr.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &macAddr[0], &macAddr[1], &macAddr[2], &macAddr[3], &macAddr[4], &macAddr[5]);

  
  esp_err_t result = esp_now_del_peer(macAddr);
  if (result == ESP_OK) {
    Serial.println("Peer removed successfully.");
    return true;
  } else {
    Serial.print("Failed to remove peer. Error: ");
    Serial.println(esp_err_to_name(result));
    return false;
  }
}

void espNow_peerInit() {
  DynamicJsonDocument doc = loadConfig();
  JsonArray peers = doc["peers"];

  if (peers.isNull()) {
    ESP_LOGW(TAG_NETWORK,"No peers in config file");
    return;
  }

  peerManager.clear();
  for (JsonObject peer : peers) {
    Peer p;
    p.id = peer["id"];
    String macAddr = peer["macAddr"].as<String>();
    macStringToByteArray(macAddr, p.macAddr);
    peerManager.addPeer(p);
    
    if(espNow_addPeer(macAddr)){
      ESP_LOGI(TAG_NETWORK,"Added Peer ID:%d MAC:%s", p.id, macAddr);
    }
    else{
      ESP_LOGE(TAG_NETWORK,"Failed to add Peer ID:%d MAC:%s", p.id, macAddr);
    }
  }


  ESP_LOGI(TAG_NETWORK,"ESP-NOW Peers initialized");
}
