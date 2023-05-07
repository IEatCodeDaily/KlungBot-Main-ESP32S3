#include "PeerManager.h"

PeerManager::PeerManager() : numPeers(0) {
}

PeerManager::~PeerManager() {
  clear();
}

bool PeerManager::addPeer(const Peer &peer) {
  if (numPeers >= MAX_PEERS) {
    return false;
  }
  
  peers[numPeers++] = peer;
  return true;
}

bool PeerManager::removePeer(int id) {
  for (int i = 0; i < numPeers; ++i) {
    if (peers[i].id == id) {
      for (int j = i + 1; j < numPeers; ++j) {
        peers[j - 1] = peers[j];
      }
      --numPeers;
      return true;
    }
  }
  return false;
}

int PeerManager::getPeerCount(){
  return numPeers;
}

const Peer *PeerManager::getPeerById(int id) const {
  for (int i = 0; i < numPeers; ++i) {
    if (peers[i].id == id) {
      return &peers[i];
    }
  }
  return nullptr;
}

Peer PeerManager::getPeerByIndex(int index) {
  if (index >= 0 && index < numPeers) {
    return peers[index];
  }
  // Return a dummy Peer object if the index is out of range
  Peer dummy;
  dummy.id = -1;
  memset(dummy.macAddr, 0, sizeof(dummy.macAddr));
  return dummy;
}

void PeerManager::listPeers() const {
  for (int i = 0; i < numPeers; ++i) {
    Serial.print("ID: ");
    Serial.print(peers[i].id);
    Serial.print(", MAC Address: ");
    for (int j = 0; j < 6; ++j) {
      if (peers[i].macAddr[j] < 16) {
        Serial.print("0");
      }
      Serial.print(peers[i].macAddr[j], HEX);
      if (j < 5) {
        Serial.print(":");
      }
    }
    Serial.println();
  }
  if (numPeers == 0){
    Serial.println("No peers found.");
  }
}

void PeerManager::clear() {
  numPeers = 0;
}

int PeerManager::getIdByMacAddress(const String &macAddr) {
  for (const auto &peer : peers) {
    if (macToString(peer.macAddr) == macAddr) {
      return peer.id;
    }
  }
  return -1;
}

String PeerManager::getMacAddressById(int id) {
  for (const auto &peer : peers) {
    if (peer.id == id) {
      return macToString(peer.macAddr);
    }
  }
  return "";
}

String macToString(const uint8_t* macAddr) {
  char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X",
           macAddr[0], macAddr[1], macAddr[2],
           macAddr[3], macAddr[4], macAddr[5]);
  return String(buf);
}

void macStringToByteArray(const String &macAddr, uint8_t macArray[6]) {
  for (int i = 0; i < 6; i++) {
    macArray[i] = strtol(macAddr.substring(i * 3, i * 3 + 2).c_str(), 0, 16);
  }
}