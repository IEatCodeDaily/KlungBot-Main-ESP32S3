#ifndef ESP_NOW_PACKET_H
#define ESP_NOW_PACKET_H

#include <Arduino.h>
#include "MidiBuffer.h"

enum PacketType : uint8_t {
  MIDI = 0,
  PING = 1,
  PAIRING = 2,
  REPORT = 3,
  MANAGEMENT = 4

  // Add more packet types here
};

#ifndef MIDI_MESSAGE_STRUCT
#define MIDI_MESSAGE_STRUCT
struct MIDIMessage {
  uint8_t status;
  uint8_t data1;
  uint8_t data2;
};
#endif

struct PingMessage {
  uint8_t id;
  char* name[24];
};

struct PairingMessage {
  uint8_t id;
  uint8_t macAddr[6];
  uint8_t channel;
};

struct ReportMessage {
  uint8_t id;
  uint8_t status;
  uint8_t reserved[32];
};

struct ManagementMessage {
  uint8_t id;
  uint8_t command;
  uint8_t args[4];
};




class ESPNowPacket {
public:
  ESPNowPacket(PacketType packetType);
  ~ESPNowPacket();

  void setMidiMessage(const MIDIMessage &message);
  MIDIMessage getMidiMessage() const;

  void setPairingMessage(const PairingMessage &message);
  PairingMessage getPairingMessage() const;

  PacketType getPacketType() const;
  void toBytes(uint8_t *bytes) const;
  static ESPNowPacket fromBytes(const uint8_t *bytes);
  uint8_t getPacketSize() const;

private:
  PacketType packetType;
  union {
    MIDIMessage midiMessage;
    PingMessage pingMessage;
    PairingMessage pairingMessage;
    ReportMessage reportMessage;
    ManagementMessage managementMessage;
    // Add more data structures for other packet types here
  };
};

#endif // ESP_NOW_PACKET_H