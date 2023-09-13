#include "gui.h"
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
uint16_t mainLabel, mainSwitcher, mainSlider, mainText, mainNumber, mainScrambleButton, mainTime;
uint16_t styleButton, styleLabel, styleSwitcher, styleSlider, styleButton2, styleLabel2,
    styleSlider2;
uint16_t graph;
volatile bool updates = false;

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

  ESPUI.addControl(Separator, "General controls", "", None, maintab);

  // These are the values for the selector's options. (Note that they *must* be
  // declared static so that the storage is allocated in global memory and not
  // just on the stack of this function.)
  static String optionValues[]{"Ethanol", "IPA", "Water", "D8", "D9", "Warming up"};
  auto mainselector =
      ESPUI.addControl(Select, "Choose process", "selector", Wetasphalt, maintab, generalCallback);
  for (auto const &v : optionValues) {
    ESPUI.addControl(Option, v.c_str(), v, None, mainselector);
  }
  mainSwitcher = ESPUI.addControl(Switcher, "Switcher", "", Sunflower, maintab, generalCallback);
  ESPUI.addControl(Button, "Button", "Button 1", Alizarin, maintab, extendedCallback, (void *)19);
  mainLabel = ESPUI.addControl(Label, "Label", "Label text", Emerald, maintab, generalCallback);

  // Sliders default to being 0 to 100, but if you want different limits you can
  // add a Min and Max control
  mainSlider = ESPUI.addControl(Slider, "Slider", "200", Turquoise, maintab, generalCallback);
  ESPUI.addControl(Min, "", "10", None, mainSlider);
  ESPUI.addControl(Max, "", "400", None, mainSlider);

  mainText =
      ESPUI.addControl(Text, "Text Input", "Initial value", Alizarin, maintab, generalCallback);

  // Number inputs also accept Min and Max components, but you should still
  // validate the values.
  mainNumber = ESPUI.addControl(Number, "Number Input", "42", Emerald, maintab, generalCallback);
  ESPUI.addControl(Min, "", "10", None, mainNumber);
  ESPUI.addControl(Max, "", "50", None, mainNumber);

  ESPUI.addControl(Separator, "Updates", "", None, maintab);

  // This button will update all the updatable controls on this tab to random
  // values
  mainScrambleButton = ESPUI.addControl(Button, "Scramble Values", "Scramble Values", Carrot,
                                        maintab, scrambleCallback);
  ESPUI.addControl(Switcher, "Constant updates", "0", Carrot, maintab, updateCallback);
  mainTime = ESPUI.addControl(Time, "", "", None, 0, generalCallback);
  ESPUI.addControl(Button, "Get Time", "Get Time", Carrot, maintab, getTimeCallback);

  ESPUI.addControl(Separator, "Control Pads", "", None, maintab);
  ESPUI.addControl(Pad, "Normal", "", Peterriver, maintab, generalCallback);
  ESPUI.addControl(PadWithCenter, "With center", "", Peterriver, maintab, generalCallback);

  /*
   * Tab: Grouped controls
   * This tab shows how multiple control can be grouped into the same panel
   *through the use of the parentControl value. This also shows how to add
   *labels to grouped controls, and how to use vertical controls.
   *-----------------------------------------------------------------------------------------------------------*/
  auto grouptab = ESPUI.addControl(Tab, "", "Grouped controls");

  // The parent of this button is a tab, so it will create a new panel with one
  // control.
  auto groupbutton =
      ESPUI.addControl(Button, "Button Group", "Button A", Dark, grouptab, generalCallback);
  // However the parent of this button is another control, so therefore no new
  // panel is created and the button is added to the existing panel.
  ESPUI.addControl(Button, "", "Button B", Alizarin, groupbutton, generalCallback);
  ESPUI.addControl(Button, "", "Button C", Alizarin, groupbutton, generalCallback);

  // Sliders can be grouped as well
  // To label each slider in the group, we are going add additional labels and
  // give them custom CSS styles We need this CSS style rule, which will remove
  // the label's background and ensure that it takes up the entire width of the
  // panel
  String clearLabelStyle = "background-color: unset; width: 100%;";
  // First we add the main slider to create a panel
  auto groupsliders =
      ESPUI.addControl(Slider, "Slider Group", "10", Dark, grouptab, generalCallback);
  // Then we add a label and set its style to the clearLabelStyle. Here we've
  // just given it the name "A"
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, groupsliders), clearLabelStyle);
  // We can now continue to add additional sliders and labels
  ESPUI.addControl(Slider, "", "20", None, groupsliders, generalCallback);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, groupsliders), clearLabelStyle);
  ESPUI.addControl(Slider, "", "30", None, groupsliders, generalCallback);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, groupsliders), clearLabelStyle);

  // We can also usefully group switchers.
  auto groupswitcher =
      ESPUI.addControl(Switcher, "Switcher Group", "0", Dark, grouptab, generalCallback);
  ESPUI.addControl(Switcher, "", "1", Sunflower, groupswitcher, generalCallback);
  ESPUI.addControl(Switcher, "", "0", Sunflower, groupswitcher, generalCallback);
  ESPUI.addControl(Switcher, "", "1", Sunflower, groupswitcher, generalCallback);
  // To label these switchers we need to first go onto a "new line" below the
  // line of switchers To do this we add an empty label set to be clear and full
  // width (with our clearLabelStyle)
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, groupswitcher), clearLabelStyle);
  // We will now need another label style. This one sets its width to the same
  // as a switcher (and turns off the background)
  String switcherLabelStyle = "width: 60px; margin-left: .3rem; margin-right: "
                              ".3rem; background-color: unset;";
  // We can now just add the styled labels.
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, groupswitcher), switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, groupswitcher), switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, groupswitcher), switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "D", None, groupswitcher), switcherLabelStyle);

  // You can mix and match different control types, but the results might
  // sometimes need additional styling to lay out nicely.
  auto grouplabel = ESPUI.addControl(Label, "Mixed Group", "Main label", Dark, grouptab);
  auto grouplabel2 = ESPUI.addControl(Label, "", "Secondary label", Emerald, grouplabel);
  ESPUI.addControl(Button, "", "Button D", Alizarin, grouplabel, generalCallback);
  ESPUI.addControl(Switcher, "", "1", Sunflower, grouplabel, generalCallback);
  ESPUI.setElementStyle(grouplabel2, "font-size: x-large; font-family: serif;");

  // Some controls can even support vertical orientation, currently Switchers
  // and Sliders
  ESPUI.addControl(Separator, "Vertical controls", "", None, grouptab);
  auto vertgroupswitcher =
      ESPUI.addControl(Switcher, "Vertical Switcher Group", "0", Dark, grouptab, generalCallback);
  ESPUI.setVertical(vertgroupswitcher);
  // On the following lines we wrap the value returned from addControl and send
  // it straight to setVertical
  ESPUI.setVertical(ESPUI.addControl(Switcher, "", "0", None, vertgroupswitcher, generalCallback));
  ESPUI.setVertical(ESPUI.addControl(Switcher, "", "0", None, vertgroupswitcher, generalCallback));
  ESPUI.setVertical(ESPUI.addControl(Switcher, "", "0", None, vertgroupswitcher, generalCallback));
  // The mechanism for labelling vertical switchers is the same as we used above
  // for horizontal ones
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, vertgroupswitcher), clearLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, vertgroupswitcher),
                        switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, vertgroupswitcher),
                        switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, vertgroupswitcher),
                        switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "D", None, vertgroupswitcher),
                        switcherLabelStyle);

  auto vertgroupslider =
      ESPUI.addControl(Slider, "Vertical Slider Group", "15", Dark, grouptab, generalCallback);
  ESPUI.setVertical(vertgroupslider);
  ESPUI.setVertical(ESPUI.addControl(Slider, "", "25", None, vertgroupslider, generalCallback));
  ESPUI.setVertical(ESPUI.addControl(Slider, "", "35", None, vertgroupslider, generalCallback));
  ESPUI.setVertical(ESPUI.addControl(Slider, "", "45", None, vertgroupslider, generalCallback));
  // The mechanism for labelling vertical sliders is the same as we used above
  // for switchers
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "", None, vertgroupslider), clearLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "A", None, vertgroupslider),
                        switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "B", None, vertgroupslider),
                        switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "C", None, vertgroupslider),
                        switcherLabelStyle);
  ESPUI.setElementStyle(ESPUI.addControl(Label, "", "D", None, vertgroupslider),
                        switcherLabelStyle);

  // Note that combining vertical and horizontal sliders is going to result in
  // very messy layout!

  /*
   * Tab: Example UI
   * An example UI for the documentation
   *-----------------------------------------------------------------------------------------------------------*/
  auto exampletab = ESPUI.addControl(Tab, "Example", "Example");
  ESPUI.addControl(Separator, "Control and Status", "", None, exampletab);
  ESPUI.addControl(Switcher, "Power", "1", Alizarin, exampletab, generalCallback);
  ESPUI.addControl(Label, "Status", "System status: OK", Wetasphalt, exampletab, generalCallback);

  ESPUI.addControl(Separator, "Settings", "", None, exampletab);
  ESPUI.addControl(PadWithCenter, "Attitude Control", "", Dark, exampletab, generalCallback);
  auto examplegroup1 = ESPUI.addControl(Button, "Activate Features", "Feature A", Carrot,
                                        exampletab, generalCallback);
  ESPUI.addControl(Button, "Activate Features", "Feature B", Carrot, examplegroup1,
                   generalCallback);
  ESPUI.addControl(Button, "Activate Features", "Feature C", Carrot, examplegroup1,
                   generalCallback);
  ESPUI.addControl(Slider, "Value control", "45", Peterriver, exampletab, generalCallback);

  /*
   * Tab: WiFi Credentials
   * You use this tab to enter the SSID and password of a wifi network to
   *autoconnect to.
   *-----------------------------------------------------------------------------------------------------------*/
  auto wifitab = ESPUI.addControl(Tab, "", "WiFi Credentials");
  wifi_ssid_text = ESPUI.addControl(Text, "SSID", "", Alizarin, wifitab, textCallback);
  // Note that adding a "Max" control to a text control sets the max length
  ESPUI.addControl(Max, "", "32", None, wifi_ssid_text);
  wifi_pass_text = ESPUI.addControl(Text, "Password", "", Alizarin, wifitab, textCallback);
  ESPUI.addControl(Max, "", "64", None, wifi_pass_text);
  ESPUI.addControl(Button, "Save", "Save", Peterriver, wifitab, enterWifiDetailsCallback);

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
    ESPUI.updateLabel(mainLabel, String(rndString1));
    ESPUI.updateSwitcher(mainSwitcher,
                         ESPUI.getControl(mainSwitcher)->value.toInt() ? false : true);
    ESPUI.updateSlider(mainSlider, random(10, 400));
    ESPUI.updateText(mainText, String(rndString2));
    ESPUI.updateNumber(mainNumber, random(100000));
    ESPUI.updateButton(mainScrambleButton, scText ? "Scrambled!" : "Scrambled.");
    scText = !scText;
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
    ESPUI.updateSwitcher(mainSwitcher,
                         ESPUI.getControl(mainSwitcher)->value.toInt() ? false : true);
    sliderVal += 10;
    if (sliderVal > 400)
      sliderVal = 10;

    // Sliders, numbers, and labels can all be updated at will
    ESPUI.updateSlider(mainSlider, sliderVal);
    ESPUI.updateNumber(mainNumber, random(100000));
    ESPUI.updateLabel(mainLabel, String(sliderVal));
    lastTime = millis();
  }

  // Simple debug UART interface
  if (Serial.available()) {
    switch (Serial.read()) {
    case 'w': // Print IP details
      Serial.println(WiFi.localIP());
      break;
    case 'W': // Reconnect wifi
      connectWifi();
      break;
    case 'C': // Force a crash (for testing exception decoder)

      break;
    default:
      Serial.print('#');
      break;
    }
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