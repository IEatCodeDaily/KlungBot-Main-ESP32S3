#include <usb/usb_host.h>
#include "show_desc.hpp"
#include "usbhhelp.hpp"
#include "params.h"
#include "MidiBuffer.h"


bool isMIDI = false;
bool isMIDIReady = false;

const size_t MIDI_IN_BUFFERS = 8;
const size_t MIDI_OUT_BUFFERS = 8;
usb_transfer_t *MIDIOut = NULL;
usb_transfer_t *MIDIIn[MIDI_IN_BUFFERS] = {NULL};

const size_t bufferSize = 128;
MidiBuffer midiBuffer(bufferSize);


// USB MIDI Event Packet Format (always 4 bytes)
//
// Byte 0 |Byte 1 |Byte 2 |Byte 3
// -------|-------|-------|------
// CN+CIN |MIDI_0 |MIDI_1 |MIDI_2
//
// CN = Cable Number (0x0..0xf) specifies virtual MIDI jack/cable
// CIN = Code Index Number (0x0..0xf) classifies the 3 MIDI bytes.
// See Table 4-1 in the MIDI 1.0 spec at usb.org.
//

static void midi_transfer_cb(usb_transfer_t *transfer)
{
  ESP_LOGD(TAG_USB, "midi_transfer_cb context: %d", transfer->context);
  if (Device_Handle == transfer->device_handle) {
    int in_xfer = transfer->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK;
    if ((transfer->status == 0) && in_xfer) {
      uint8_t *const p = transfer->data_buffer;
      for (int i = 0; i < transfer->actual_num_bytes; i += 4) {
        if ((p[i] + p[i+1] + p[i+2] + p[i+3]) == 0) break;
        ESP_LOGD(TAG_USB, "midi: %02x %02x %02x %02x", p[i], p[i+1], p[i+2], p[i+3]);
        MIDIMessage msg = {p[i+1], p[i+2], p[i+3]};
        midiBuffer.push(msg);
      }
      esp_err_t err = usb_host_transfer_submit(transfer);
      if (err != ESP_OK) {
        ESP_LOGW(TAG_USB, "usb_host_transfer_submit In fail: %x", err);
      }
    }
    else {
      ESP_LOGD(TAG_USB, "transfer->status %d", transfer->status);
    }
  }
}


void check_interface_desc_MIDI(const void *p)
{
  const usb_intf_desc_t *intf = (const usb_intf_desc_t *)p;

  // USB MIDI
  if ((intf->bInterfaceClass == USB_CLASS_AUDIO) &&
      (intf->bInterfaceSubClass == 3) &&
      (intf->bInterfaceProtocol == 0))
  {
    isMIDI = true;
    ESP_LOGI(TAG_USB, "Claiming a MIDI device!");
    esp_err_t err = usb_host_interface_claim(Client_Handle, Device_Handle,
        intf->bInterfaceNumber, intf->bAlternateSetting);
    if (err != ESP_OK) ESP_LOGW(TAG_USB, "usb_host_interface_claim failed: %x", err);
  }
}

void prepare_endpoints(const void *p)
{
  const usb_ep_desc_t *endpoint = (const usb_ep_desc_t *)p;
  esp_err_t err;

  // must be bulk for MIDI
  if ((endpoint->bmAttributes & USB_BM_ATTRIBUTES_XFERTYPE_MASK) != USB_BM_ATTRIBUTES_XFER_BULK) {
    ESP_LOGW(TAG_USB, "Not bulk endpoint: 0x%02x", endpoint->bmAttributes);
    return;
  }
  if (endpoint->bEndpointAddress & USB_B_ENDPOINT_ADDRESS_EP_DIR_MASK) {
    for (int i = 0; i < MIDI_IN_BUFFERS; i++) {
      err = usb_host_transfer_alloc(endpoint->wMaxPacketSize, 0, &MIDIIn[i]);
      if (err != ESP_OK) {
        MIDIIn[i] = NULL;
        ESP_LOGW(TAG_USB, "usb_host_transfer_alloc In fail: %x", err);
      }
      else {
        MIDIIn[i]->device_handle = Device_Handle;
        MIDIIn[i]->bEndpointAddress = endpoint->bEndpointAddress;
        MIDIIn[i]->callback = midi_transfer_cb;
        MIDIIn[i]->context = (void *)i;
        MIDIIn[i]->num_bytes = endpoint->wMaxPacketSize;
        esp_err_t err = usb_host_transfer_submit(MIDIIn[i]);
        if (err != ESP_OK) {
          ESP_LOGW(TAG_USB, "usb_host_transfer_submit In fail: %x", err);
        }
      }
    }
  }
  else {
    err = usb_host_transfer_alloc(endpoint->wMaxPacketSize, 0, &MIDIOut);
    if (err != ESP_OK) {
      MIDIOut = NULL;
      ESP_LOGW(TAG_USB, "usb_host_transfer_alloc Out fail: %x", err);
      return;
    }
    ESP_LOGW(TAG_USB, "Out data_buffer_size: %d", MIDIOut->data_buffer_size);
    MIDIOut->device_handle = Device_Handle;
    MIDIOut->bEndpointAddress = endpoint->bEndpointAddress;
    MIDIOut->callback = midi_transfer_cb;
    MIDIOut->context = NULL;
//    MIDIOut->flags |= USB_TRANSFER_FLAG_ZERO_PACK;
  }
  isMIDIReady = ((MIDIOut != NULL) && (MIDIIn[0] != NULL));
}

void show_config_desc_full(const usb_config_desc_t *config_desc)
{
  // Full decode of config desc.
  const uint8_t *p = &config_desc->val[0];
  uint8_t bLength;
  for (int i = 0; i < config_desc->wTotalLength; i+=bLength, p+=bLength) {
    bLength = *p;
    if ((i + bLength) <= config_desc->wTotalLength) {
      const uint8_t bDescriptorType = *(p + 1);
      switch (bDescriptorType) {
        case USB_B_DESCRIPTOR_TYPE_DEVICE:
          ESP_LOGI(TAG_USB, "USB Device Descriptor should not appear in config");
          break;
        case USB_B_DESCRIPTOR_TYPE_CONFIGURATION:
          show_config_desc(p);
          break;
        case USB_B_DESCRIPTOR_TYPE_STRING:
          ESP_LOGI(TAG_USB, "USB string desc TBD");
          break;
        case USB_B_DESCRIPTOR_TYPE_INTERFACE:
          show_interface_desc(p);
          if (!isMIDI) check_interface_desc_MIDI(p);
          break;
        case USB_B_DESCRIPTOR_TYPE_ENDPOINT:
          show_endpoint_desc(p);
          if (isMIDI && !isMIDIReady) {
            prepare_endpoints(p);
          }
          break;
        case USB_B_DESCRIPTOR_TYPE_DEVICE_QUALIFIER:
          // Should not be in config?
          ESP_LOGI(TAG_USB, "USB device qual desc TBD");
          break;
        case USB_B_DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION:
          // Should not be in config?
          ESP_LOGI(TAG_USB, "USB Other Speed TBD");
          break;
        case USB_B_DESCRIPTOR_TYPE_INTERFACE_POWER:
          // Should not be in config?
          ESP_LOGI(TAG_USB, "USB Interface Power TBD");
          break;
        default:
          ESP_LOGI(TAG_USB, "Unknown USB Descriptor Type: 0x%x", *p);
          break;
      }
    }
    else {
      ESP_LOGI(TAG_USB, "USB Descriptor invalid");
      return;
    }
  }
}
