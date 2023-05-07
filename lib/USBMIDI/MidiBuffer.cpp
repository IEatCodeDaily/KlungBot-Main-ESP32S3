#include "MidiBuffer.h"

MidiBuffer::MidiBuffer(size_t size)
  : bufferSize(size), buffer(new MIDIMessage[size]), head(0), tail(0), count(0) {
}

MidiBuffer::~MidiBuffer() {
  delete[] buffer;
}

bool MidiBuffer::push(const MIDIMessage &message) {
  if (isFull()) {
    return false;
  }

  buffer[head] = message;
  head = (head + 1) % bufferSize;
  count++;
  return true;
}


MIDIMessage MidiBuffer::getMsg() {
  if (isEmpty()) {
    return MIDIMessage{0, 0, 0};
  }

  MIDIMessage message = buffer[tail];
  tail = (tail + 1) % bufferSize;
  count--;
  return message;
}

bool MidiBuffer::isFull() const {
  return count == bufferSize;
}

bool MidiBuffer::isEmpty() const {
  return count == 0;
}