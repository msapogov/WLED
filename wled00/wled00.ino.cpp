# 1 "/var/folders/0s/29fgdnsx1pd9p3wcjqt_3rq00000gn/T/tmpk8s_yq7c"
#include <Arduino.h>
# 1 "/Users/sk/Documents/GitHub/WLED/wled00/wled00.ino"
# 13 "/Users/sk/Documents/GitHub/WLED/wled00/wled00.ino"
#include "wled.h"
void setup();
void loop();
#line 15 "/Users/sk/Documents/GitHub/WLED/wled00/wled00.ino"
void setup() {
  WLED::instance().setup();
}

void loop() {
  WLED::instance().loop();
}