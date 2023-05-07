#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <Arduino.h>

#define MAX_PEERS 20

struct Peer {
  int id;
  uint8_t macAddr[6];
};

String macToString(const uint8_t* macAddr);
void macStringToByteArray(const String &macAddr, uint8_t macArray[6]);

class PeerManager {
public:
  PeerManager();
  ~PeerManager();

  bool addPeer(const Peer &peer);
  bool removePeer(int id);
  int getPeerCount();
  const Peer *getPeerById(int id) const;
  Peer getPeerByIndex(int index);
  void listPeers() const;
  void clear();
  int getIdByMacAddress(const String &macAddr);
  String getMacAddressById(int id);
  
private:
  Peer peers[MAX_PEERS];
  int numPeers;
};

#endif