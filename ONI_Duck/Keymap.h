#pragma once
#include <Arduino.h>
#include <pgmspace.h>
#include "USBHIDKeyboard.h"

// ---------------------------------------------------------------------------
// Tastaturlayout-System.
//
// Die Layout-Tabellen (locale_*.h) und usb_hid_keys.h stammen aus dem
// WiFiDuck-Projekt (MIT-Lizenz, https://github.com/spacehuhntech/WiFiDuck).
// Die Tipp-Logik ist aus deren atmega_duck/keyboard.cpp portiert und auf die
// ESP32-S2-USB-HID-API umgesetzt (Keyboard.press(0x88 + usage)).
//
// Tabellenformat:
//   ascii[b]        -> 2 Byte: {Modifier-Maske, Keycode}      (b = 0..127)
//   utf8[]          -> 6 Byte: {4 Byte UTF-8, Modifier, Keycode}
//   combinations[]  -> 8 Byte: {4 Byte UTF-8, mod1,key1 (Tottaste), mod2,key2}
// ---------------------------------------------------------------------------

#include "usb_hid_keys.h"
#include "locale_types.h"
#include "locale_us.h"
#include "locale_de.h"
#include "locale_gb.h"
#include "locale_es.h"
#include "locale_dk.h"
#include "locale_ru.h"
#include "locale_fr.h"
#include "locale_be.h"
#include "locale_pt.h"
#include "locale_it.h"
#include "locale_sk.h"
#include "locale_cz.h"
#include "locale_si.h"
#include "locale_bg.h"
#include "locale_cafr.h"
#include "locale_chde.h"
#include "locale_chfr.h"
#include "locale_hu.h"

extern USBHIDKeyboard Keyboard;
extern hid_locale_t*  g_locale;        // in ONI_Duck.ino definiert
extern int g_charDelay;                // ms zusaetzlich pro Zeichen
extern int g_jitter;                   // max. zufaellige Zusatzverzoegerung (ms)

// Code (z. B. "DE", "CA-FR") -> Layout-Zeiger
inline void setLocaleByCode(String code) {
  code.trim(); code.toUpperCase(); code.replace("-", ""); code.replace("_", "");
  if      (code == "US")   g_locale = &locale_us;
  else if (code == "DE")   g_locale = &locale_de;
  else if (code == "GB")   g_locale = &locale_gb;
  else if (code == "ES")   g_locale = &locale_es;
  else if (code == "DK")   g_locale = &locale_dk;
  else if (code == "RU")   g_locale = &locale_ru;
  else if (code == "FR")   g_locale = &locale_fr;
  else if (code == "BE")   g_locale = &locale_be;
  else if (code == "PT")   g_locale = &locale_pt;
  else if (code == "IT")   g_locale = &locale_it;
  else if (code == "SK")   g_locale = &locale_sk;
  else if (code == "CZ")   g_locale = &locale_cz;
  else if (code == "SI")   g_locale = &locale_si;
  else if (code == "BG")   g_locale = &locale_bg;
  else if (code == "CAFR") g_locale = &locale_cafr;
  else if (code == "CHDE") g_locale = &locale_chde;
  else if (code == "CHFR") g_locale = &locale_chfr;
  else if (code == "HU")   g_locale = &locale_hu;
  else                     g_locale = &locale_us;
}

// Modifier-Maske (KEY_MOD_*) in einzelne Press-Aufrufe uebersetzen
inline void kmPressModMask(uint8_t m) {
  for (uint8_t b = 0; b < 8; b++) if (m & (1 << b)) Keyboard.press(0x80 + b);
}
inline void kmPressKey(uint8_t key, uint8_t mods) {
  if (g_dryRun) return;
  kmPressModMask(mods);
  if (key) Keyboard.press(0x88 + key);
}
inline void kmRelease() { if (g_dryRun) return; Keyboard.releaseAll(); delay(3); }

// Ein Zeichen (bis zu 4 Byte ab b, hoechstens 'avail' verfuegbar) tippen.
// Rueckgabe: Anzahl zusaetzlich verbrauchter Bytes (Mehrbyte-UTF-8).
inline uint8_t kmPress(const uint8_t* b, size_t avail) {
  // 1) Kombinationen (Tottasten) zuerst pruefen
  for (size_t i = 0; i < g_locale->combinations_len; i++) {
    uint8_t res = 0;
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t kc = pgm_read_byte(g_locale->combinations + (i*8) + j);
      uint8_t sb = (j < avail) ? b[j] : 0;
      if (kc == 0) break;
      if (kc == sb) res++; else { res = 0; break; }
    }
    if (res > 0) {
      uint8_t cmod = pgm_read_byte(g_locale->combinations + (i*8) + 4);
      uint8_t ckey = pgm_read_byte(g_locale->combinations + (i*8) + 5);
      uint8_t mod  = pgm_read_byte(g_locale->combinations + (i*8) + 6);
      uint8_t key  = pgm_read_byte(g_locale->combinations + (i*8) + 7);
      kmPressKey(ckey, cmod); kmRelease();
      kmPressKey(key,  mod);  kmRelease();
      return res - 1;
    }
  }
  // 2) ASCII
  if (b[0] < g_locale->ascii_len) {
    uint8_t mod = pgm_read_byte(g_locale->ascii + (b[0]*2) + 0);
    uint8_t key = pgm_read_byte(g_locale->ascii + (b[0]*2) + 1);
    kmPressKey(key, mod);
    return 0;
  }
  // 3) UTF-8 (Mehrbyte, z. B. Umlaute, €, Kyrillisch)
  for (size_t i = 0; i < g_locale->utf8_len; i++) {
    uint8_t res = 0;
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t kc = pgm_read_byte(g_locale->utf8 + (i*6) + j);
      uint8_t sb = (j < avail) ? b[j] : 0;
      if (kc == 0) break;
      if (kc == sb) res++; else { res = 0; break; }
    }
    if (res > 0) {
      uint8_t mod = pgm_read_byte(g_locale->utf8 + (i*6) + 4);
      uint8_t key = pgm_read_byte(g_locale->utf8 + (i*6) + 5);
      kmPressKey(key, mod);
      return res - 1;
    }
  }
  return 0;
}
inline uint8_t kmWrite(const uint8_t* b, size_t avail) {
  uint8_t r = kmPress(b, avail); kmRelease();
  if (!g_dryRun) { int d = g_charDelay + (g_jitter > 0 ? (int)random(g_jitter) : 0); if (d > 0) delay(d); }
  return r;
}

// Kompletten String im aktuellen Layout tippen
inline void typeStringLocale(const String &s) {
  const uint8_t* b = (const uint8_t*)s.c_str();
  size_t len = s.length(), i = 0;
  while (i < len) { uint8_t extra = kmWrite(b + i, len - i); i += 1 + extra; }
}

// Physischen Keycode fuer ein Zeichen im aktuellen Layout (fuer Tastenkuerzel)
inline uint8_t usageForChar(char c) {
  uint8_t b = (uint8_t)c;
  if (g_locale && b < g_locale->ascii_len) {
    uint8_t key = pgm_read_byte(g_locale->ascii + (b*2) + 1);
    if (key) return key;
  }
  if (c >= 'A' && c <= 'Z') c += 32;
  if (c >= 'a' && c <= 'z') { if (c=='y') return 0x1C; if (c=='z') return 0x1D; return 0x04 + (c - 'a'); }
  if (c >= '1' && c <= '9') return 0x1E + (c - '1');
  if (c == '0') return 0x27;
  return 0;
}
