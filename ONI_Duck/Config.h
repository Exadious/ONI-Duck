#pragma once
#include <Arduino.h>
#include <Preferences.h>

// Firmware-Version des ONI-Duck (entspricht dem CLI-Befehl "version" der Vorbilder)
#define ONI_VERSION "1.0.0"

// ---------------------------------------------------------------------------
// Zentrale Konfiguration des ONI-Duck. Gespeichert im NVS (Preferences).
// ---------------------------------------------------------------------------

struct OniConfig {
  // Login der Weboberflaeche
  String loginUser = "ONI";
  String loginPass = "oni";

  // Eigener Access-Point (WPA2)
  String apSsid    = "ONI-Duck";
  String apPass    = "oni-duck-1234";   // >= 8 Zeichen!

  // WLAN-Modus: "AP" (eigener Hotspot) oder "STA" (an vorhandenes WLAN)   [Issue #47 / PR #140]
  String wifiMode  = "AP";
  String staSsid   = "";
  String staPass   = "";

  // Darstellung + Tastaturlayout
  String theme     = "dark";            // "dark" | "light"
  String layout    = "DE";              // "DE" | "US"

  // mDNS-Name -> http://<hostname>.local
  String hostname  = "oni-duck";
  // Tippgeschwindigkeit: "fast" | "normal" | "human"
  String typeSpeed = "normal";
  // Maus-Jiggler: haelt den PC wach
  bool   jiggler    = false;
  int    jigglerSec = 30;

  // Status-/Befehls-LED (WS2812 an GPIO18 auf dem S2 DevKitC-1)           [Issue #69 / PR #127]
  bool   ledEnabled = true;
  int    ledPin     = 18;

  // Physischer Knopf: Name des Skripts, das der BOOT-Button startet       [Issue #131]
  String buttonScript = "ONI-Vorstellung";

  // USB-Kennung (leer = Standard Espressif). Zum Nachbilden einer         [SuperWiFiDuck]
  // bestimmten Tastaturmarke / fuer Kompatibilitaet.
  String usbVid = "";                   // z. B. "0x05ac"
  String usbPid = "";                   // z. B. "0x0267"
  String usbManufacturer = "";
  String usbProduct = "";
};

extern OniConfig g_cfg;

inline void loadConfig() {
  Preferences p; p.begin("oni", true);
  g_cfg.loginUser     = p.getString("user",   "ONI");
  g_cfg.loginPass     = p.getString("pass",   "oni");
  g_cfg.apSsid        = p.getString("ssid",   "ONI-Duck");
  g_cfg.apPass        = p.getString("appass", "oni-duck-1234");
  g_cfg.wifiMode      = p.getString("wifimode", "AP");
  g_cfg.staSsid       = p.getString("stassid", "");
  g_cfg.staPass       = p.getString("stapass", "");
  g_cfg.theme         = p.getString("theme",  "dark");
  g_cfg.layout        = p.getString("layout", "DE");
  g_cfg.hostname      = p.getString("host",   "oni-duck");
  g_cfg.typeSpeed     = p.getString("tspeed", "normal");
  g_cfg.jiggler       = p.getBool  ("jiggler", false);
  g_cfg.jigglerSec    = p.getInt   ("jigsec", 30);
  g_cfg.ledEnabled    = p.getBool  ("led",    true);
  g_cfg.ledPin        = p.getInt   ("ledpin", 18);
  g_cfg.buttonScript  = p.getString("btnscript", "ONI-Vorstellung");
  g_cfg.usbVid          = p.getString("usbvid", "");
  g_cfg.usbPid          = p.getString("usbpid", "");
  g_cfg.usbManufacturer = p.getString("usbmanu", "");
  g_cfg.usbProduct      = p.getString("usbprod", "");
  p.end();
}

inline void saveConfig() {
  Preferences p; p.begin("oni", false);
  p.putString("user",   g_cfg.loginUser);
  p.putString("pass",   g_cfg.loginPass);
  p.putString("ssid",   g_cfg.apSsid);
  p.putString("appass", g_cfg.apPass);
  p.putString("wifimode", g_cfg.wifiMode);
  p.putString("stassid",  g_cfg.staSsid);
  p.putString("stapass",  g_cfg.staPass);
  p.putString("theme",  g_cfg.theme);
  p.putString("layout", g_cfg.layout);
  p.putString("host",   g_cfg.hostname);
  p.putString("tspeed", g_cfg.typeSpeed);
  p.putBool  ("jiggler", g_cfg.jiggler);
  p.putInt   ("jigsec", g_cfg.jigglerSec);
  p.putBool  ("led",    g_cfg.ledEnabled);
  p.putInt   ("ledpin", g_cfg.ledPin);
  p.putString("btnscript", g_cfg.buttonScript);
  p.putString("usbvid", g_cfg.usbVid);
  p.putString("usbpid", g_cfg.usbPid);
  p.putString("usbmanu", g_cfg.usbManufacturer);
  p.putString("usbprod", g_cfg.usbProduct);
  p.end();
}
