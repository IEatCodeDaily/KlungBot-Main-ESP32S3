#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>


#define CONFIG_FILE "/config.json"
#define JSON_DOC_SIZE 1024

DynamicJsonDocument loadConfig();
bool saveConfig(const DynamicJsonDocument &doc);
void printConfig();

#endif // CONFIG_MANAGER_H