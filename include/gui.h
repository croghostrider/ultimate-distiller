#include <ESPUI.h>

// Settings
#define SLOW_BOOT 0
#define HOSTNAME "ESPUITest"
#define FORCE_USE_HOTSPOT 0

// Function Prototypes
void connectWifi();
void setUpUI();
void uiLoop();
void enterWifiDetailsCallback(Control *sender, int type);
void textCallback(Control *sender, int type);
void generalCallback(Control *sender, int type);
void scrambleCallback(Control *sender, int type);
void runProcCallback(Control *sender, int type);
void updateCallback(Control *sender, int type);
void getTimeCallback(Control *sender, int type);
void graphAddCallback(Control *sender, int type);
void graphClearCallback(Control *sender, int type);
void randomString(char *buf, int len);
void extendedCallback(Control *sender, int type, void *param);