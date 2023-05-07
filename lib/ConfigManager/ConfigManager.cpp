#include "ConfigManager.h"


DynamicJsonDocument loadConfig() {
  DynamicJsonDocument doc(JSON_DOC_SIZE);

  if (!SPIFFS.exists(CONFIG_FILE)) {
    Serial.println("Config file does not exist. Creating a default one.");
    doc.createNestedArray("peers");
    saveConfig(doc);
  } else {
    File configFile = SPIFFS.open(CONFIG_FILE, "r");
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error) {
      Serial.println("Failed to parse config file. Creating a default one.");
      doc["peers"] = JsonArray();
      saveConfig(doc);
    }
  }


  return doc;
}

bool saveConfig(const DynamicJsonDocument &doc) {
  File configFile = SPIFFS.open(CONFIG_FILE, "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing.");
    return false;
  }


  Serial.println("Saving config:");
  serializeJson(doc, Serial);
  Serial.println();

  size_t bytesWritten = serializeJson(doc, configFile);
  configFile.close();

  if (bytesWritten == 0) {
    Serial.println("Failed to write JSON to config file.");
    return false;
  }

  Serial.println("Config file saved successfully.");
  return true;
}

void printConfig() {
  DynamicJsonDocument doc = loadConfig();
  Serial.println("Config content:");
  serializeJsonPretty(doc, Serial);
  Serial.println();
}