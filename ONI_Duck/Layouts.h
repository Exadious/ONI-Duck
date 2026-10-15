#pragma once
#include <Arduino.h>
#include "USBHIDKeyboard.h"

// ---------------------------------------------------------------------------
// Benannte Tasten, Numpad, Consumer-Control und Modifier.
// Das Tippen von Text/Zeichen uebernimmt Keymap.h (layoutabhaengig).
//   Keyboard.press(0x88 + usage) = rohe HID-Usage-ID
//   Keyboard.press(0x80..0x87)   = Modifier
// ---------------------------------------------------------------------------

extern USBHIDKeyboard Keyboard;
extern bool g_dryRun;                 // true = nicht wirklich tippen (Testlauf)

static const uint8_t MOD_CTRL  = 0x80;
static const uint8_t MOD_SHIFT = 0x81;
static const uint8_t MOD_ALT   = 0x82;
static const uint8_t MOD_GUI   = 0x83;
static const uint8_t MOD_ALTGR = 0x86;

inline void tapUsage(uint8_t usage, bool shift = false, bool altgr = false) {
  if (g_dryRun) return;
  if (shift) Keyboard.press(MOD_SHIFT);
  if (altgr) Keyboard.press(MOD_ALTGR);
  Keyboard.press(0x88 + usage);
  delay(4);
  Keyboard.releaseAll();
  delay(2);
}

// --- Benannte Tasten -> Usage (inkl. Numpad) -------------------------------
inline uint8_t nameToUsage(const String &up) {
  if (up == "ENTER" || up == "RETURN")            return 0x28;
  if (up == "ESC"   || up == "ESCAPE")            return 0x29;
  if (up == "BACKSPACE")                          return 0x2A;
  if (up == "TAB")                                return 0x2B;
  if (up == "SPACE")                              return 0x2C;
  if (up == "CAPSLOCK")                           return 0x39;
  if (up == "PRINTSCREEN")                        return 0x46;
  if (up == "SCROLLLOCK")                         return 0x47;
  if (up == "PAUSE" || up == "BREAK")             return 0x48;
  if (up == "INSERT")                             return 0x49;
  if (up == "HOME")                               return 0x4A;
  if (up == "PAGEUP")                             return 0x4B;
  if (up == "DELETE" || up == "DEL")              return 0x4C;
  if (up == "END")                                return 0x4D;
  if (up == "PAGEDOWN")                           return 0x4E;
  if (up == "RIGHT" || up == "RIGHTARROW")        return 0x4F;
  if (up == "LEFT"  || up == "LEFTARROW")         return 0x50;
  if (up == "DOWN"  || up == "DOWNARROW")         return 0x51;
  if (up == "UP"    || up == "UPARROW")           return 0x52;
  if (up == "NUMLOCK")                            return 0x53;
  if (up == "MENU"  || up == "APP")               return 0x65;
  if (up == "F1")  return 0x3A; if (up == "F2")  return 0x3B;
  if (up == "F3")  return 0x3C; if (up == "F4")  return 0x3D;
  if (up == "F5")  return 0x3E; if (up == "F6")  return 0x3F;
  if (up == "F7")  return 0x40; if (up == "F8")  return 0x41;
  if (up == "F9")  return 0x42; if (up == "F10") return 0x43;
  if (up == "F11") return 0x44; if (up == "F12") return 0x45;
  if (up == "NUM_0") return 0x62; if (up == "NUM_1") return 0x59;
  if (up == "NUM_2") return 0x5A; if (up == "NUM_3") return 0x5B;
  if (up == "NUM_4") return 0x5C; if (up == "NUM_5") return 0x5D;
  if (up == "NUM_6") return 0x5E; if (up == "NUM_7") return 0x5F;
  if (up == "NUM_8") return 0x60; if (up == "NUM_9") return 0x61;
  if (up == "NUM_ENTER")   return 0x58;
  if (up == "NUM_PLUS")    return 0x57;
  if (up == "NUM_MINUS")   return 0x56;
  if (up == "NUM_ASTERIX") return 0x55;
  if (up == "NUM_SLASH")   return 0x54;
  if (up == "NUM_DOT")     return 0x63;
  return 0;
}

// --- Consumer-Control (Media/Lautstaerke) -> rohe Usage-ID -----------------
inline uint16_t nameToConsumer(const String &up) {
  if (up == "PLAYPAUSE")        return 0xCD;
  if (up == "STOP")             return 0xB7;
  if (up == "NEXTTRACK")        return 0xB5;
  if (up == "PREVTRACK")        return 0xB6;
  if (up == "VOLUME_MUTE" || up == "MUTE")  return 0xE2;
  if (up == "VOLUME_UP")        return 0xE9;
  if (up == "VOLUME_DOWN")      return 0xEA;
  if (up == "BRIGHTNESS_UP")    return 0x6F;
  if (up == "BRIGHTNESS_DOWN")  return 0x70;
  if (up == "POWER")            return 0x30;
  if (up == "SLEEP")            return 0x32;
  if (up == "CALCULATOR")       return 0x192;
  if (up == "SEARCH")           return 0x221;
  if (up == "WWW_BACK" || up == "BACK")       return 0x224;
  if (up == "WWW_FORWARD" || up == "FORWARD") return 0x225;
  if (up == "REFRESH")          return 0x227;
  if (up == "WWW_HOME")         return 0x223;
  return 0;
}

// --- Modifier-Token -> Press-Code ------------------------------------------
inline uint8_t modCode(const String &up) {
  if (up == "GUI" || up == "WINDOWS" || up == "WIN" || up == "COMMAND") return MOD_GUI;
  if (up == "CTRL" || up == "CONTROL")                                  return MOD_CTRL;
  if (up == "ALT"  || up == "OPTION")                                   return MOD_ALT;
  if (up == "SHIFT")                                                    return MOD_SHIFT;
  if (up == "ALTGR")                                                    return MOD_ALTGR;
  return 0;
}
