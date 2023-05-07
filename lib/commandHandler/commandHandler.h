#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include "ESPNowHandler.hpp"
#include "params.h"

#define MAX_ARGS 5

#ifndef PEER_MANAGER
#define PEER_MANAGER
PeerManager peerManager;
#endif
extern PeerManager peerManager;


// Command handler
void handleCommand(const String &message);
void splitArgs(const String &argsStr, String args[], int &argsCount);

//Get MAC
String getMacAddress();
bool isValidMacAddress(const String &macAddr);

// Peers Command
void cmdPeer(const String args[5]);

void listPeers();
void clearPeers();

void savePeer();
void loadPeer();

void addPeer(int id, const String &macAddr);

void removePeerByMac(const String &macAddr);
void removePeerById(int id);
void removePeer(const String &macOrID);


// Log Command
void set_log(String args[], int argsCount);
void print_tags();
bool isValidTag(const String &tag);

#endif