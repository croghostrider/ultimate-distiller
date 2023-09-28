#include "gui.h"
#include "main.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <WiFi.h>

/**
 * @file completeExample.cpp
 * @author Ian Gray @iangray1000
 *
 * This is an example GUI to show off all of the features of ESPUI.
 * This can be built using the Arduino IDE, or PlatformIO.
 *
 * ---------------------------------------------------------------------------------------
 * If you just want to see examples of the ESPUI code, jump down to the
 * setUpUI() function
 * ---------------------------------------------------------------------------------------
 *
 * When this program boots, it will load an SSID and password from the EEPROM.
 * The SSID is a null-terminated C string stored at EEPROM addresses 0-31
 * The password is a null-terminated C string stored at EEPROM addresses 32-95.
 * If these credentials do not work for some reason, the ESP will create an
 * Access Point wifi with the SSID HOSTNAME (defined below). You can then
 * connect and use the controls on the "Wifi Credentials" tab to store
 * credentials into the EEPROM.
 *
 */

// UI handles
uint16_t wifi_ssid_text, wifi_pass_text;
uint16_t mainStart, mainselector, mainStaus, mainInfo, mainSotp, tempHeatingElement, tempBottom,
    tempTop;
uint16_t mainSlider, mainText, mainNumber, mainScrambleButton, mainTime;
uint16_t styleButton, styleLabel, styleSwitcher, styleSlider, styleButton2, styleLabel2,
    styleSlider2;
uint16_t graph;
volatile bool updates = false;
volatile bool runProc = false;

// This is the main function which builds our GUI
void setUpUI() {

  // Turn off verbose debugging
  ESPUI.setVerbosity(Verbosity::Quiet);

  // Make sliders continually report their position as they are being dragged.
  ESPUI.sliderContinuous = true;

  // This GUI is going to be a tabbed GUI, so we are adding most controls using
  // ESPUI.addControl which allows us to set a parent control. If we didn't need
  // tabs we could use the simpler add functions like:
  //     ESPUI.button()
  //     ESPUI.label()

  /*
   * Tab: Basic Controls
   * This tab contains all the basic ESPUI controls, and shows how to read and
   *update them at runtime.
   *-----------------------------------------------------------------------------------------------------------*/
  auto maintab = ESPUI.addControl(Tab, "", "Basic controls");

  // These are the values for the selector's options. (Note that they *must* be
  // declared static so that the storage is allocated in global memory and not
  // just on the stack of this function.)
  static String optionValues[]{"Ethanol", "IPA", "Water", "D8", "D9", "Warming up"};
  mainselector =
      ESPUI.addControl(Select, "Choose process", "Ethanol", Wetasphalt, maintab, generalCallback);
  for (auto const &v : optionValues) {
    ESPUI.addControl(Option, v.c_str(), v, None, mainselector);
  }
  mainStart = ESPUI.addControl(Switcher, "Start process", "", Sunflower, maintab, runProcCallback);
  tempTop = ESPUI.addControl(Label, "Temperature", "Top: 32 °C", Emerald, maintab);
  tempHeatingElement =
      ESPUI.addControl(Label, "", "Heating element: 40 °C", Emerald, tempTop, generalCallback);
  tempBottom = ESPUI.addControl(Label, "", "Bottom: 38 °C", Dark, tempTop);
  mainStaus = ESPUI.addControl(Label, "Status", "Mode: Ready", Emerald, maintab);
  mainInfo = ESPUI.addControl(Label, "", "Info: ", Emerald, mainStaus);
  mainSotp = ESPUI.addControl(Button, "", "Stop", Alizarin, mainStaus, generalCallback);
  ESPUI.updateVisibility(mainSotp, false);

  ESPUI.addControl(Separator, "Updates", "", None, maintab);

  // This button will update all the updatable controls on this tab to random
  // values
  mainScrambleButton = ESPUI.addControl(Button, "Scramble Values", "Scramble Values", Carrot,
                                        maintab, scrambleCallback);
  ESPUI.addControl(Switcher, "Constant updates", "0", Carrot, maintab, updateCallback);
  mainTime = ESPUI.addControl(Time, "", "", None, 0, generalCallback);
  ESPUI.addControl(Button, "Get Time", "Get Time", Carrot, maintab, getTimeCallback);

  /*
   * Tab: WiFi Credentials
   * You use this tab to enter the SSID and password of a wifi network to
   *autoconnect to.
   *-----------------------------------------------------------------------------------------------------------*/
  auto settingstab = ESPUI.addControl(Tab, "", "Settings");
  ESPUI.addControl(Separator, "WiFi Credentials", "", None, settingstab);
  wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, settingstab, textCallback);
  // Note that adding a "Max" control to a text control sets the max length
  ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
  wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, settingstab, textCallback);
  ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
  ESPUI.addControl(Button, "Save", "Save", Peterriver, settingstab, enterWifiDetailsCallback);

  // Finally, start up the UI.
  // This should only be called once we are connected to WiFi.
  ESPUI.begin(HOSTNAME);
}

// This callback updates the "values" of a bunch of controls
void scrambleCallback(Control *sender, int type) {
  static char rndString1[10];
  static char rndString2[20];
  static bool scText = false;

  if (type == B_UP) { // Button callbacks generate events for both UP and DOWN.
    // Generate some random text
    randomString(rndString1, 10);
    randomString(rndString2, 20);

    // Set the various controls to random value to show how controls can be
    // updated at runtime
    ESPUI.updateLabel(tempHeatingElement, String(rndString1));
    ESPUI.updateSwitcher(mainStart, ESPUI.getControl(mainStart)->value.toInt() ? false : true);
    ESPUI.updateSlider(mainSlider, random(10, 400));
    ESPUI.updateText(mainText, String(rndString2));
    ESPUI.updateNumber(mainNumber, random(100000));
    ESPUI.updateButton(mainScrambleButton, scText ? "Scrambled!" : "Scrambled.");
    scText = !scText;
  }
}

void runProcCallback(Control *sender, int type) {
  if (sender->value.toInt() > 0) {
    ESPUI.setEnabled(mainselector, false);
    fsm.trigger(start_proc_state);
    ESPUI.updateVisibility(mainSotp, true);
  } else {
    ESPUI.setEnabled(mainselector, true);
    ESPUI.updateVisibility(mainSotp, false);
  }
}

void updateCallback(Control *sender, int type) { updates = (sender->value.toInt() > 0); }

void getTimeCallback(Control *sender, int type) {
  if (type == B_UP) {
    ESPUI.updateTime(mainTime);
  }
}

void graphAddCallback(Control *sender, int type) {
  if (type == B_UP) {
    ESPUI.addGraphPoint(graph, random(1, 50));
  }
}

void graphClearCallback(Control *sender, int type) {
  if (type == B_UP) {
    ESPUI.clearGraph(graph);
  }
}

// Most elements in this test UI are assigned this generic callback which prints
// some basic information. Event types are defined in ESPUI.h
void generalCallback(Control *sender, int type) {
  Serial.print("CB: id(");
  Serial.print(sender->id);
  Serial.print(") Type(");
  Serial.print(type);
  Serial.print(") '");
  Serial.print(sender->label);
  Serial.print("' = ");
  Serial.println(sender->value);
}

// Most elements in this test UI are assigned this generic callback which prints
// some basic information. Event types are defined in ESPUI.h The extended param
// can be used to hold a pointer to additional information or for C++ it can be
// used to return a this pointer for quick access using a lambda function
void extendedCallback(Control *sender, int type, void *param) {
  Serial.print("CB: id(");
  Serial.print(sender->id);
  Serial.print(") Type(");
  Serial.print(type);
  Serial.print(") '");
  Serial.print(sender->label);
  Serial.print("' = ");
  Serial.println(sender->value);
  Serial.print("param = ");
  Serial.println((long)param);
}

void uiLoop() {
  static long unsigned lastTime = 0;

  // Send periodic updates if switcher is turned on
  if (updates && millis() > lastTime + 500) {
    static uint16_t sliderVal = 10;

    // Flick this switcher on and off
    ESPUI.updateSwitcher(mainStart, ESPUI.getControl(mainStart)->value.toInt() ? false : true);
    sliderVal += 10;
    if (sliderVal > 400)
      sliderVal = 10;

    // Sliders, numbers, and labels can all be updated at will
    ESPUI.updateSlider(mainSlider, sliderVal);
    ESPUI.updateNumber(mainNumber, random(100000));
    ESPUI.updateLabel(tempHeatingElement, String(sliderVal));
    lastTime = millis();
  }
}

// Utilities
//
// If you are here just to see examples of how to use ESPUI, you can ignore the
// following functions
//------------------------------------------------------------------------------------------------
void readStringFromEEPROM(String &buf, int baseaddress, int size) {
  buf.reserve(size);
  for (int i = baseaddress; i < baseaddress + size; i++) {
    char c = EEPROM.read(i);
    buf += c;
    if (!c)
      break;
  }
}

void connectWifi() {
  int connect_timeout;

  WiFi.setHostname(HOSTNAME);

  Serial.println("Begin wifi...");

  // Load credentials from EEPROM
  if (!(FORCE_USE_HOTSPOT)) {
    yield();
    EEPROM.begin(100);
    String stored_ssid, stored_pass;
    readStringFromEEPROM(stored_ssid, 0, 32);
    readStringFromEEPROM(stored_pass, 32, 96);
    EEPROM.end();

    // Try to connect with stored credentials, fire up an access point if they
    // don't work.

    WiFi.begin(stored_ssid.c_str(), stored_pass.c_str());

    connect_timeout = 28; // 7 seconds
    while (WiFi.status() != WL_CONNECTED && connect_timeout > 0) {
      delay(250);
      Serial.print(".");
      connect_timeout--;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(WiFi.localIP());
    Serial.println("Wifi started");

    if (!MDNS.begin(HOSTNAME)) {
      Serial.println("Error setting up MDNS responder!");
    }
  } else {
    Serial.println("\nCreating access point...");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(IPAddress(192, 168, 1, 1), IPAddress(192, 168, 1, 1),
                      IPAddress(255, 255, 255, 0));
    WiFi.softAP(HOSTNAME);

    connect_timeout = 20;
    do {
      delay(250);
      Serial.print(",");
      connect_timeout--;
    } while (connect_timeout);
  }
}

void enterWifiDetailsCallback(Control *sender, int type) {
  if (type == B_UP) {
    Serial.println("Saving credentials to EPROM...");
    Serial.println(ESPUI.getControl(wifi_ssid_text)->value);
    Serial.println(ESPUI.getControl(wifi_pass_text)->value);
    unsigned int i;
    EEPROM.begin(100);
    for (i = 0; i < ESPUI.getControl(wifi_ssid_text)->value.length(); i++) {
      EEPROM.write(i, ESPUI.getControl(wifi_ssid_text)->value.charAt(i));
      if (i == 30)
        break; // Even though we provided a max length, user input should never
               // be trusted
    }
    EEPROM.write(i, '\0');

    for (i = 0; i < ESPUI.getControl(wifi_pass_text)->value.length(); i++) {
      EEPROM.write(i + 32, ESPUI.getControl(wifi_pass_text)->value.charAt(i));
      if (i == 94)
        break; // Even though we provided a max length, user input should never
               // be trusted
    }
    EEPROM.write(i + 32, '\0');
    EEPROM.end();
  }
}

void textCallback(Control *sender, int type) {
  // This callback is needed to handle the changed values, even though it
  // doesn't do anything itself.
}

void randomString(char *buf, int len) {
  for (auto i = 0; i < len - 1; i++)
    buf[i] = random(0, 26) + 'A';
  buf[len - 1] = '\0';
}