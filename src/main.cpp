
#include "params.h"
#include <esp_log.h>
#include <Arduino.h>
#include <FreeRTOS.h>
#include "commandHandler.h"

//==========Dependencies=========
//esp32-usb-host-demos by TouchGadget
// https://github.com/touchgadget/esp32-usb-host-demos
#include "usbhostmidi.hpp"


// Task handles
TaskHandle_t TaskHandleMIDI;
TaskHandle_t TaskSerial;



//===============RTOS TASKS=================
void Task_MIDI(void *parameter) {
  // Initialize USB
  usbh_setup(show_config_desc_full);
  while (1) {
    
    // Listen for USB MIDI events
    usbh_task();
    while(!midiBuffer.isEmpty()){
      MIDIMessage msg;
      msg = midiBuffer.getMsg();

      //Create MIDI packet
      ESPNowPacket packet(PacketType::MIDI);
      packet.setMidiMessage(msg);
      uint8_t packetSize = packet.getPacketSize();
      uint8_t buffer[packetSize];
      packet.toBytes(buffer);
      //packet Type - status - data1 - data 2
      
      // Send the MIDI message to all connected peers
      if(esp_now_send(0, buffer, packetSize)){
        ESP_LOGD(TAG_MIDI, "Sent MIDI message to all peers");
        //ESP_LOGD(TAG_MIDI,"Message sent: %s %s %s", msg.status, msg.data1, msg.data2);
      }
      else{
        ESP_LOGE(TAG_MIDI, "Failed to send MIDI message to all peers");
      }
    }
    

    // Yield the task, allowing other tasks to run
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void Task_Serial(void *parameter) {
  String command;
  while (1) {
    if (Serial.available()) {
      command = Serial.readStringUntil('\n');
      command.trim();
      
      // Process command
      handleCommand(command);
    }
    // Yield the task, allowing other tasks to run
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true); //Allow ESP_LOG debugging
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    ESP_LOGE(TAG_SYSTEM,"Failed to mount SPIFFS");
    return;
  }
  ESP_LOGI(TAG_SYSTEM,"SPIFFS mounted successfully");
  
  // Initialize ESP-NOW
  espNow_init();
  

  // Set Default log level - USELESS
  esp_log_level_set("*", ESP_LOG_INFO);
  
  // Create tasks
  xTaskCreatePinnedToCore(Task_MIDI, "TaskHandleMIDI", 4096, NULL, 3, &TaskHandleMIDI, 1);
  xTaskCreatePinnedToCore(Task_Serial, "TaskSerial", 4096, NULL, 1, &TaskSerial, 1);
}

void loop() {
  // Intentionally left empty, as tasks are handled by RTOS.
}
