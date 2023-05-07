#define PROJECT_NAME "KlungBot_Main"
#define VERSION "v2.0"
// KlungBot Wireless v2.0
// by IEatCodeDaily - Raisal P (13319072)

#ifndef KLUNBGBOT_PARAMS
#define KLUNBGBOT_PARAMS

#include <ctype.h>
#include <esp32-hal-log.h>


// Available log tags
static const char* TAG_SYSTEM = "SYSTEM";
static const char* TAG_USB = "USB";
static const char* TAG_NETWORK = "NETWORK";
static const char* TAG_MIDI = "MIDI";

// Log tags enumeration
static const char* LOG_TAGS[] = {
    TAG_SYSTEM,
    TAG_USB,
    TAG_NETWORK,
    TAG_MIDI
};

static const size_t NUM_LOG_TAGS = sizeof(LOG_TAGS) / sizeof(LOG_TAGS[0]);

#endif // KLUNBGBOT_PARAMS
