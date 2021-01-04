# Wemos shield display and temperature usermod
## Enabling usernod
 <code>-D USERMOD_SHIELD_DISPLAY #For display only<br>
 -D USERMOD_SHIELD_DISP_TEMP #For display and temperature sensor
 </code>

## Project repository
-   [Original repository](https://github.com/srg74/WLED-wemos-shield) - WLED Wemos shield repository
-   [Wemos shield project Wiki](https://github.com/srg74/WLED-wemos-shield/wiki)
-   [Precompiled WLED firmware](https://github.com/srg74/WLED-wemos-shield/tree/master/resources/Firmware)

## Features
-   SSD1306 128x32 or 128x64 I2C OLED display
-   On screen IP address, SSID and controller status (e.g. ON or OFF, recent effect)
-   Auto display shutoff for saving display lifetime
-   Dallas temperature sensor
-   Reporting temperature to MQTT broker

## Hardware
![Shield](https://github.com/srg74/WLED-wemos-shield/blob/master/resources/Images/Assembly_8.jpg)

## Functionality checked with
-   Wemos D1 mini original v3.1 and clones
-   Wemos32 mini
-   VSCode + PlatformIO
-   SSD1306 128x32, 128x32 I2C OLED displays
-   DS18B20 (temperature sensor)

### Platformio requirements

For Dallas sensor uncomment `U8g2@~2.27.2`,`DallasTemperature@~3.8.0`,`OneWire@~2.3.5 under` `[common]` section in `platformio.ini`:
```ini
# platformio.ini
...
[platformio]
...
; default_envs = esp07
; default_envs = d1_mini
...
[common]
...
lib_deps_external =
  ...
  #For use SSD1306 OLED display uncomment following
  U8g2@~2.27.3
  #For Dallas sensor uncomment following 2 lines
  DallasTemperature@~3.8.0
  OneWire@~2.3.5
...
```
