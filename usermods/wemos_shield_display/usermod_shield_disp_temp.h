// Credits to @mrVanboy, @gwaland and my dearest friend @westward
#pragma once

#include "wled.h"
#include <Arduino.h>
#include <U8x8lib.h> // from https://github.com/olikraus/u8g2/
#include <OneWire.h> // Dallas temperature sensor

//Dallas sensor quick reading. Credit to - Author: Peter Scargill, August 17th, 2013
int16_t Dallas(int x, byte start)
{
    OneWire DallasSensor(x);
    byte i;
    byte data[2];
    int16_t result;
    do
    {
      DallasSensor.reset();
      DallasSensor.write(0xCC);
      DallasSensor.write(0xBE);
      for ( i = 0; i < 2; i++) data[i] = DallasSensor.read();
      result=(data[1]<<8)|data[0];
      result>>=4; if (data[1]&128) result|=61440;
      if (data[0]&8) ++result;
      DallasSensor.reset();
      DallasSensor.write(0xCC);
      DallasSensor.write(0x44,1);
      if (start) delay(1000);
    } while (start--);
      return result;
}

#define USERMOD_ID_SHIELD_DISP_TEMP 98

#ifdef ARDUINO_ARCH_ESP32
    uint8_t SCL_PIN = 22;
    uint8_t SDA_PIN = 21;
    #ifdef USERMOD_TEMPERATURE
    uint8_t DALLAS_PIN =23;
    #endif
#else
  uint8_t SCL_PIN = 5;
  uint8_t SDA_PIN = 4;
    #ifdef USERMOD_TEMPERATURE
    uint8_t DALLAS_PIN =13;
    #endif
#endif

#define U8X8_PIN_SCL SCL_PIN
#define U8X8_PIN_SDA SDA_PIN
// How often we are redrawing screen
#define USER_LOOP_REFRESH_RATE_MS 5000
long lastMeasure = 0;
#define Celsius // Show temperature mesaurement in Celcius otherwise is in Fahrenheit 

// If display does not work or looks corrupted check the
// constructor reference:
// https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// or check the gallery:
// https://github.com/olikraus/u8g2/wiki/gallery
// --> First choise of cheap I2C OLED 128X32 0.91"
U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE, U8X8_PIN_SCL, U8X8_PIN_SDA); // Pins are Reset, SCL, SDA
// --> Second choise of cheap I2C OLED 128X64 0.96" or 1.3"
//U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE, U8X8_PIN_SCL, U8X8_PIN_SDA); // Pins are Reset, SCL, SDA

//class name. Use something descriptive and leave the ": public Usermod" part :)
class ShieldDispTempUsermod : public Usermod {
  private:
    //Private class members. You can declare variables and functions only accessible to your usermod here
    unsigned long lastTime = 0;

    bool displayTurnedOff = false;
    long lastRedraw = 0;
    // needRedraw marks if redraw is required to prevent often redrawing.
    bool needRedraw = true;
    // Next variables hold the previous known values to determine if redraw is required.
    String knownSsid = "";
    IPAddress knownIp;
    uint8_t knownBrightness = 0;
    uint8_t knownMode = 0;
    uint8_t knownPalette = 0;
    long lastUpdate = 0;

  public:
    //Functions called by WLED

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      Dallas (DALLAS_PIN,1);
      u8x8.begin();
      u8x8.setPowerSave(0);
      u8x8.setFlipMode(1);
      u8x8.setContrast(10); //Contrast setup will help to preserve OLED lifetime. In case OLED need to be brighter increase number up to 255
      u8x8.setFont(u8x8_font_chroma48medium8_r);
      u8x8.drawString(0, 0, "Loading...");
      //Serial.println("Hello from my usermod!");
    }

    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      //Serial.println("Connected to WiFi!");
    }

    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     *
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     *    Additionally, "if (WLED_MQTT_CONNECTED)" is available to check for a connection to an MQTT broker.
     *
     * 2. Try to avoid using the delay() function. NEVER use delays longer than 10 milliseconds.
     *    Instead, use a timer check as shown here.
     */
    void loop() {
      if (millis() - lastTime > 1000) {

        // Turn off display after 3 minutes with no change.
        if(!displayTurnedOff && millis() - lastRedraw > 3*60*1000) {
          u8x8.setPowerSave(1);
          displayTurnedOff = true;
        }

        // Check if values which are shown on display changed from the last time.
        if (((apActive) ? String(apSSID) : WiFi.SSID()) != knownSsid) {
          needRedraw = true;
        } else if (knownIp != (apActive ? IPAddress(4, 3, 2, 1) : WiFi.localIP())) {
          needRedraw = true;
        } else if (knownBrightness != bri) {
          needRedraw = true;
        } else if (knownMode != strip.getMode()) {
          needRedraw = true;
        } else if (knownPalette != strip.getSegment(0).palette) {
          needRedraw = true;
        }

        if (!needRedraw) {
          return;
        }
        needRedraw = false;

        if (displayTurnedOff) {
          u8x8.setPowerSave(0);
          displayTurnedOff = false;
        }
          lastRedraw = millis();

      // Update last known values.
      #ifdef ARDUINO_ARCH_ESP32
        knownSsid = WiFi.SSID();
      #else
        knownSsid = apActive ? WiFi.softAPSSID() : WiFi.SSID();
      #endif
      knownIp = apActive ? IPAddress(4, 3, 2, 1) : WiFi.localIP();
      knownBrightness = bri;
      knownMode = strip.getMode();
      knownPalette = strip.getSegment(0).palette;
      u8x8.clear();
      u8x8.setFont(u8x8_font_chroma48medium8_r);

      // First row with Wifi name
      u8x8.setCursor(1, 0);
      u8x8.print(knownSsid.substring(0, u8x8.getCols() > 1 ? u8x8.getCols() - 2 : 0));
      // Print `~` char to indicate that SSID is longer, than owr dicplay
      if (knownSsid.length() > u8x8.getCols())
        u8x8.print("~");

      // Second row with IP or Psssword
      u8x8.setCursor(1, 1);
      // Print password in AP mode and if led is OFF.
      if (apActive && bri == 0)
        u8x8.print(apPass);
      else
        u8x8.print(knownIp);

      // Third row with mode name
      u8x8.setCursor(2, 2);
      uint8_t qComma = 0;
      bool insideQuotes = false;
      uint8_t printedChars = 0;
      char singleJsonSymbol;

      // Find the mode name in JSON
      for (size_t i = 0; i < strlen_P(JSON_mode_names); i++) {
        singleJsonSymbol = pgm_read_byte_near(JSON_mode_names + i);
        switch (singleJsonSymbol) {
        case '"':
        insideQuotes = !insideQuotes;
      break;
        case '[':
        case ']':
      break;
        case ',':
        qComma++;
      default:
        if (!insideQuotes || (qComma != knownMode))
      break;
      u8x8.print(singleJsonSymbol);
      printedChars++;
    }
    if ((qComma > knownMode) || (printedChars > u8x8.getCols() - 2))
      break;
  }
    // Fourth row with palette name
    u8x8.setCursor(2, 3);
    qComma = 0;
    insideQuotes = false;
    printedChars = 0;
    // Looking for palette name in JSON.
    for (size_t i = 0; i < strlen_P(JSON_palette_names); i++) {
      singleJsonSymbol = pgm_read_byte_near(JSON_palette_names + i);
      switch (singleJsonSymbol) {
      case '"':
        insideQuotes = !insideQuotes;
        break;
      case '[':
      case ']':
        break;
      case ',':
        qComma++;
      default:
        if (!insideQuotes || (qComma != knownPalette))
          break;
        u8x8.print(singleJsonSymbol);
        printedChars++;
    }
      if ((qComma > knownMode) || (printedChars > u8x8.getCols() - 2))
        break;
  }

  u8x8.setFont(u8x8_font_open_iconic_embedded_1x1);
  u8x8.drawGlyph(0, 0, 80); // wifi icon
  u8x8.drawGlyph(0, 1, 68); // home icon
  u8x8.setFont(u8x8_font_open_iconic_weather_2x2);
  u8x8.drawGlyph(0, 2, 66 + (bri > 0 ? 3 : 0)); // sun/moon icon
        lastTime = millis();
      }
    }


    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    /*
    void addToJsonInfo(JsonObject& root)
    {
      int reading = 20;
      //this code adds "u":{"Light":[20," lux"]} to the info object
      JsonObject user = root["u"];
      if (user.isNull()) user = root.createNestedObject("u");

      JsonArray lightArr = user.createNestedArray("Light"); //name
      lightArr.add(reading); //value
      lightArr.add(" lux"); //unit
    }
    */


    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void addToJsonState(JsonObject& root)
    {
      //root["user0"] = userVar0;
    }


    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    void readFromJsonState(JsonObject& root)
    {
      userVar0 = root["user0"] | userVar0; //if "user0" key exists in JSON, update, else keep old value
      //if (root["bri"] == 255) Serial.println(F("Don't burn down your garage!"));
    }


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     *
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     *
     * addToConfig() will also not yet add your setting to one of the settings pages automatically.
     * To make that work you still have to add the setting to the HTML, xml.cpp and set.cpp manually.
     *
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("exampleUsermod");
      top["great"] = userVar0; //save this var persistently whenever settings are saved
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens once immediately after boot)
     *
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     */
    void readFromConfig(JsonObject& root)
    {
      JsonObject top = root["top"];
      userVar0 = top["great"] | 42; //The value right of the pipe "|" is the default value in case your setting was not present in cfg.json (e.g. first boot)
    }


    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId()
    {
      return USERMOD_ID_SHIELD_DISP_TEMP;
    }

   //More methods can be added in the future, this example will then be extended.
   //Your usermod will remain compatible as it does not need to implement all methods from the Usermod base class!
};