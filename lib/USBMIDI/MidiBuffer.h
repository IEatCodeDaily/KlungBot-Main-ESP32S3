#ifndef MIDI_BUFFER_H
#define MIDI_BUFFER_H

#include <Arduino.h>

#ifndef MIDI_MESSAGE_STRUCT
#define MIDI_MESSAGE_STRUCT
struct MIDIMessage{
    uint8_t status;
    uint8_t data1;
    uint8_t data2;
};
#endif

class MidiBuffer {
public:
  MidiBuffer(size_t size);
  ~MidiBuffer();

  bool push(const MIDIMessage &message);
  bool isEmpty() const;
  MIDIMessage getMsg();

private:
  size_t bufferSize;
  MIDIMessage *buffer;
  size_t head;
  size_t tail;
  size_t count;

  bool isFull() const;
};

#endif // MIDI_BUFFER_H