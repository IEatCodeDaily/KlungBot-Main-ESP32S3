#include "ESPNowPacket.h"

ESPNowPacket::ESPNowPacket(PacketType packetType) : packetType(packetType) {
}

ESPNowPacket::~ESPNowPacket() {
}

void ESPNowPacket::setMidiMessage(const MIDIMessage &message) {
  midiMessage = message;
}

MIDIMessage ESPNowPacket::getMidiMessage() const {
  return midiMessage;
}

void ESPNowPacket::setPairingMessage(const PairingMessage &message) {
  pairingMessage = message;
}

PairingMessage ESPNowPacket::getPairingMessage() const {
  return pairingMessage;
}

PacketType ESPNowPacket::getPacketType() const {
  return packetType;
}

void ESPNowPacket::toBytes(uint8_t *bytes) const {
  bytes[0] = static_cast<uint8_t>(packetType);
  switch (packetType) {
    case MIDI:
      bytes[1] = midiMessage.status;
      bytes[2] = midiMessage.data1;
      bytes[3] = midiMessage.data2;
      break;
    case PAIRING:
      bytes[1] = pairingMessage.id;
      memcpy(bytes + 2, pairingMessage.macAddr, 6);
      bytes[8] = pairingMessage.channel;
      break;

    // Handle more packet types here
    default:
      break;
  }
}

ESPNowPacket ESPNowPacket::fromBytes(const uint8_t *bytes) {
  PacketType packetType = static_cast<PacketType>(bytes[0]);
  ESPNowPacket packet(packetType);

  switch (packetType) {
    case MIDI:
      packet.setMidiMessage({bytes[1], bytes[2], bytes[3]});
      break;
    case PAIRING:
      PairingMessage pairingMessage;
      pairingMessage.id = bytes[1];
      memcpy(pairingMessage.macAddr, bytes + 2, 6);
      pairingMessage.channel = bytes[8];
      packet.setPairingMessage(pairingMessage);
      break;
    // Handle more packet types here
    default:
      break;
  }

  return packet;
}

uint8_t ESPNowPacket::getPacketSize() const {
  uint8_t baseSize = sizeof(PacketType);
  switch (packetType) {
    case MIDI:
      return baseSize + sizeof(MIDIMessage);
    case PING:
      return baseSize + sizeof(PingMessage);
    case PAIRING:
      return baseSize + sizeof(PairingMessage);
    case REPORT:
      return baseSize + sizeof(ReportMessage);
    case MANAGEMENT:
      return baseSize + sizeof(ManagementMessage);
    // Add more cases for other packet types here
    default:
      return 0; // Invalid packet type
  }
}