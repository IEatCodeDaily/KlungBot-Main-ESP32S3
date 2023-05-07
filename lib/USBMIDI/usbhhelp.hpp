#include "params.h"
const TickType_t HOST_EVENT_TIMEOUT = 1;
const TickType_t CLIENT_EVENT_TIMEOUT = 1;

usb_host_client_handle_t Client_Handle;
usb_device_handle_t Device_Handle;
typedef void (*usb_host_enum_cb_t)(const usb_config_desc_t *config_desc);
static usb_host_enum_cb_t _USB_host_enumerate;

void _client_event_callback(const usb_host_client_event_msg_t *event_msg, void *arg)
{
  esp_err_t err;
  switch (event_msg->event)
  {
    /**< A new device has been enumerated and added to the USB Host Library */
    case USB_HOST_CLIENT_EVENT_NEW_DEV:
      ESP_LOGD(TAG_USB, "New device address: %d", event_msg->new_dev.address);
      err = usb_host_device_open(Client_Handle, event_msg->new_dev.address, &Device_Handle);
      if (err != ESP_OK) ESP_LOGD(TAG_USB, "usb_host_device_open: %x", err);

      usb_device_info_t dev_info;
      err = usb_host_device_info(Device_Handle, &dev_info);
      if (err != ESP_OK) ESP_LOGD(TAG_USB, "usb_host_device_info: %x", err);
      ESP_LOGD(TAG_USB, "speed: %d dev_addr %d vMaxPacketSize0 %d bConfigurationValue %d",
          dev_info.speed, dev_info.dev_addr, dev_info.bMaxPacketSize0,
          dev_info.bConfigurationValue);

      const usb_device_desc_t *dev_desc;
      err = usb_host_get_device_descriptor(Device_Handle, &dev_desc);
      if (err != ESP_OK) ESP_LOGD(TAG_USB, "usb_host_get_device_desc: %x", err);
      show_dev_desc(dev_desc);

      const usb_config_desc_t *config_desc;
      err = usb_host_get_active_config_descriptor(Device_Handle, &config_desc);
      if (err != ESP_OK) ESP_LOGD(TAG_USB, "usb_host_get_config_desc: %x", err);
      (*_USB_host_enumerate)(config_desc);
      break;
    /**< A device opened by the client is now gone */
    case USB_HOST_CLIENT_EVENT_DEV_GONE:
      ESP_LOGI(TAG_USB, "Device Gone handle: %x", event_msg->dev_gone.dev_hdl);
      break;
    default:
      ESP_LOGI(TAG_USB, "Unknown value %d", event_msg->event);
      break;
  }
}

// Reference: esp-idf/examples/peripherals/usb/host/usb_host_lib/main/usb_host_lib_main.c

void usbh_setup(usb_host_enum_cb_t enumeration_cb)
{
  const usb_host_config_t config = {
    .intr_flags = ESP_INTR_FLAG_LEVEL1,
  };
  esp_err_t err = usb_host_install(&config);
  ESP_LOGD(TAG_USB, "usb_host_install: %x", err);

  const usb_host_client_config_t client_config = {
    .is_synchronous = false,
    .max_num_event_msg = 5,
    .async = {
        .client_event_callback = _client_event_callback,
        .callback_arg = Client_Handle
    }
  };
  err = usb_host_client_register(&client_config, &Client_Handle);
  ESP_LOGD(TAG_USB, "usb_host_client_register: %x", err);

  _USB_host_enumerate = enumeration_cb;
}

void usbh_task(void)
{
  uint32_t event_flags;
  static bool all_clients_gone = false;
  static bool all_dev_free = false;

  esp_err_t err = usb_host_lib_handle_events(HOST_EVENT_TIMEOUT, &event_flags);
  if (err == ESP_OK) {
    if (event_flags & USB_HOST_LIB_EVENT_FLAGS_NO_CLIENTS) {
      ESP_LOGI(TAG_USB, "No more clients");
      all_clients_gone = true;
    }
    if (event_flags & USB_HOST_LIB_EVENT_FLAGS_ALL_FREE) {
      ESP_LOGI(TAG_USB, "No more devices");
      all_dev_free = true;
    }
  }
  else {
    if (err != ESP_ERR_TIMEOUT) {
      ESP_LOGI(TAG_USB, "usb_host_lib_handle_events: %x flags: %x", err, event_flags);
    }
  }

  err = usb_host_client_handle_events(Client_Handle, CLIENT_EVENT_TIMEOUT);
  if ((err != ESP_OK) && (err != ESP_ERR_TIMEOUT)) {
    ESP_LOGI(TAG_USB, "usb_host_client_handle_events: %x", err);
  }
}