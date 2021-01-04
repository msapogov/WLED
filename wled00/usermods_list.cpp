#include "wled.h"
/*
 * Register your v2 usermods here!
 *   (for v1 usermods using just usermod.cpp, you can ignore this file)
 */

/*
 * Add/uncomment your usermod filename here (and once more below)
 * || || ||
 * \/ \/ \/
 */
//#include "usermod_v2_example.h"
#ifdef USERMOD_DALLASTEMPERATURE
#include "../usermods/Temperature/usermod_temperature.h"
#endif
//#include "usermod_v2_empty.h"
#ifdef USERMOD_BUZZER
#include "../usermods/buzzer/usermod_v2_buzzer.h"
#endif
#ifdef USERMOD_SENSORSTOMQTT
#include "usermod_v2_SensorsToMqtt.h"
#endif
#ifdef USERMOD_SHIELD_DISPLAY
#include "../usermods/wemos_shield_display/usermod_shield_display.h"
#endif
#ifdef USERMOD_SHIELD_DISP_TEMP
#include "../usermods/wemos_shield_display/usermod_shield_disp_temp.h"
#endif
void registerUsermods()
{
/*
   * Add your usermod class name here
   * || || ||
   * \/ \/ \/
   */
//usermods.add(new MyExampleUsermod());
#ifdef USERMOD_DALLASTEMPERATURE
  usermods.add(new UsermodTemperature());
#endif
//usermods.add(new UsermodRenameMe());
#ifdef USERMOD_BUZZER
  usermods.add(new BuzzerUsermod());
#endif
#ifdef USERMOD_SENSORSTOMQTT
  usermods.add(new UserMod_SensorsToMQTT());
#endif
#ifdef USERMOD_SHIELD_DISPLAY
  usermods.add(new ShieldDisplayUsermod());
#endif
#ifdef USERMOD_SHIELD_DISP_TEMP
  usermods.add(new ShieldDispTempUsermod());
#endif
}