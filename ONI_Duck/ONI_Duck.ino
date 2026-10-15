// ===========================================================================
//  ONI-DUCK  -  WiFi Rubber Ducky fuer ESP32-S2 DevKitC-1
//
//  Funktionen angelehnt an SpacehuhnTech/WiFiDuck und ThingPulse/SuperWiFiDuck
//  (inkl. Community-Wuensche aus deren Issues/PRs):
//   - USB-HID Tastatur + Maus + Media-Tasten
//   - Ducky Script: STRING, DELAY, LOCALE, KEYCODE, REPEAT, LED, MOUSE, ...
//   - eigener WLAN-AP (WPA2) ODER Verbindung zu vorhandenem WLAN (STA)
//   - passwortgeschuetzte Weboberflaeche (Standard ONI / oni)
//   - Skripte speichern/loeschen/importieren, Start per Knopf + Autostart
//   - Start per physischem BOOT-Button
//   - RGB-Status-LED (GPIO18 des DevKitC-1)
//   - optionale eigene USB-Kennung (VID/PID/Hersteller/Produkt)
//
//  >>> Nur auf eigenen Geraeten bzw. mit ausdruecklicher Erlaubnis nutzen. <<<
//
//  Arduino-IDE (Tools):  Board "ESP32S2 Dev Module", USB CDC On Boot: Enabled,
//  USB Mode: USB-OTG (TinyUSB), Partition "Default 4MB with spiffs".
//  Keine externen Bibliotheken noetig (alles im ESP32-Arduino-Core).
// ===========================================================================

#include "USB.h"
#include "USBHIDKeyboard.h"
#include "USBHIDMouse.h"
#include "USBHIDConsumerControl.h"
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <esp_random.h>

#include "Config.h"
#include "Layouts.h"
#include "DuckParser.h"
#include "WebUI.h"

USBHIDKeyboard        Keyboard;
USBHIDMouse           Mouse;
USBHIDConsumerControl Consumer;
OniConfig             g_cfg;
hid_locale_t*         g_locale = &locale_de;   // aktuelles Tastaturlayout
int                   g_charDelay = 4;         // Tippverzoegerung pro Zeichen
int                   g_jitter = 0;            // zufaellige Zusatzverzoegerung
WebServer             server(80);

// --- Ausfuehrungs-Task (Stop / Live-Konsole / Dry-Run) ---
bool                  g_dryRun = false;
volatile bool         g_running = false;
volatile bool         g_stop = false;
volatile int          g_curLine = 0;
volatile int          g_totalLines = 0;
String                g_jobScript, g_jobName, g_console;
TaskHandle_t          g_duckTask = nullptr;
SemaphoreHandle_t     g_consoleMux = nullptr;

void consoleLog(const String &s) {
  if (!g_consoleMux) return;
  xSemaphoreTake(g_consoleMux, portMAX_DELAY);
  g_console += s; g_console += "\n";
  if (g_console.length() > 4000) g_console = g_console.substring(g_console.length() - 3500);
  xSemaphoreGive(g_consoleMux);
}

String g_session = "";
String g_ipInfo  = "";                 // fuer Serial/Anzeige
const char* SCRIPT_DIR   = "/scripts";
const char* AUTORUN_FILE = "/autorun.lst";
const int   BUTTON_PIN   = 0;          // BOOT-Taster am DevKitC-1

// --------------------------------------------------------------------------- LED
void oniLed(uint8_t r, uint8_t g, uint8_t b) {
  if (!g_cfg.ledEnabled) return;
  neopixelWrite(g_cfg.ledPin, r, g, b);
}

// --------------------------------------------------------------------------- Helfer
String sanitizeName(String n) {
  n.trim(); String out = "";
  for (unsigned i = 0; i < n.length() && out.length() < 32; i++) {
    char c = n[i];
    if ((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c==' '||c=='_'||c=='-') out += c;
  }
  out.trim(); return out;
}
String scriptPath(const String &name) { return String(SCRIPT_DIR) + "/" + name + ".txt"; }
String jsonEscape(const String &s) {
  String o = "";
  for (unsigned i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"' || c == '\\') { o += '\\'; o += c; }
    else if (c == '\n') { o += "\\n"; }
    else if (c == '\r') { o += "\\r"; }
    else if (c == '\t') { o += "\\t"; }
    else o += c;
  }
  return o;
}

// --------------------------------------------------------------------------- Autostart-Liste
bool isAutorun(const String &name) {
  File f = LittleFS.open(AUTORUN_FILE, "r"); if (!f) return false;
  bool found = false;
  while (f.available()) { String l = f.readStringUntil('\n'); l.trim(); if (l == name) { found = true; break; } }
  f.close(); return found;
}
void setAutorun(const String &name, bool on) {
  String lines[64]; int cnt = 0;
  File f = LittleFS.open(AUTORUN_FILE, "r");
  if (f) { while (f.available() && cnt < 64) { String l=f.readStringUntil('\n'); l.trim(); if (l.length() && l!=name) lines[cnt++]=l; } f.close(); }
  if (on && cnt < 64) lines[cnt++] = name;
  File w = LittleFS.open(AUTORUN_FILE, "w");
  if (w) { for (int i=0;i<cnt;i++) w.println(lines[i]); w.close(); }
}

void runScriptByName(const String &name) {
  File sf = LittleFS.open(scriptPath(name), "r");
  if (sf) { String c = sf.readString(); sf.close(); duckRunScript(c); }
}

// --------------------------------------------------------------------------- Standard-Skript
// Oeffnet den Windows-Editor und stellt das Projekt als Text vor.
static const char DEFAULT_SCRIPT[] = R"DUCK(REM ONI-Duck - Projektvorstellung (Windows, Editor)
DEFAULTDELAY 35
DELAY 800
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1500
STRING ONI-Duck - WiFi Rubber Ducky auf ESP32-S2
ENTER
STRING ========================================
ENTER
ENTER
STRING Ein ESP32-S2 meldet sich am PC als USB-Tastatur und Maus an
ENTER
STRING und tippt gespeicherte Ducky-Skripte. Gesteuert wird alles
ENTER
STRING ueber eine passwortgeschuetzte Weboberflaeche per WLAN.
ENTER
ENTER
STRING Funktionen:
ENTER
STRING - Weboberflaeche: Theme Schwarz/Rot/Weiss, Dark/Light, Sprache waehlbar
ENTER
STRING - Skripte speichern, bearbeiten, importieren, per Knopf starten
ENTER
STRING - Autostart beim Einstecken und Start ueber den BOOT-Taster
ENTER
STRING - Ducky Script: STRING, DELAY, LOCALE, KEYCODE, MOUSE, Media-Tasten
ENTER
STRING - 18 Tastaturlayouts (DE, US, GB, FR, RU und mehr)
ENTER
STRING - Eigener WLAN-Hotspot oder Verbindung zu vorhandenem WLAN
ENTER
STRING - RGB-Status-LED und optionale eigene USB-Kennung
ENTER
ENTER
STRING Hinweis: nur auf eigenen Geraeten oder mit Erlaubnis verwenden.
ENTER
)DUCK";

bool anyScriptExists() {
  File dir = LittleFS.open(SCRIPT_DIR);
  if (!dir || !dir.isDirectory()) return false;
  File fe = dir.openNextFile();
  while (fe) { if (String(fe.name()).endsWith(".txt")) return true; fe = dir.openNextFile(); }
  return false;
}
// Legt das Standard-Skript an, wenn noch keine Skripte vorhanden sind.
void seedDefaultScript() {
  if (anyScriptExists()) return;
  File f = LittleFS.open(scriptPath("ONI-Vorstellung"), "w");
  if (f) { f.print(DEFAULT_SCRIPT); f.close(); }
}

// Hintergrund-Task: wartet auf einen Job und fuehrt ihn aus
void duckTaskFn(void *) {
  for (;;) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);     // auf Job warten
    duckRunScript(g_jobScript);                  // schreibt Log, prueft g_stop
    g_dryRun = false;
    g_running = false;
  }
}
// Startet ein Skript asynchron im Task. false = laeuft bereits.
bool startJob(const String &name, const String &content, bool dry) {
  if (g_running) return false;
  g_jobName = name; g_jobScript = content; g_dryRun = dry;
  if (g_consoleMux) { xSemaphoreTake(g_consoleMux, portMAX_DELAY); g_console = ""; xSemaphoreGive(g_consoleMux); }
  g_stop = false; g_curLine = 0; g_totalLines = 0; g_running = true;
  xTaskNotifyGive(g_duckTask);
  return true;
}

// --------------------------------------------------------------------------- Auth
String getCookieToken() {
  if (!server.hasHeader("Cookie")) return "";
  String c = server.header("Cookie"); int i = c.indexOf("oni="); if (i < 0) return "";
  i += 4; int e = c.indexOf(';', i); if (e < 0) e = c.length(); return c.substring(i, e);
}
bool authed() { String t = getCookieToken(); return g_session.length() && t == g_session; }
bool requireAuth() { if (authed()) return true; server.send(401, "text/plain", "unauthorized"); return false; }
String genToken() { const char* h="0123456789abcdef"; char b[33]; for(int i=0;i<32;i++) b[i]=h[esp_random()&0xF]; b[32]=0; return String(b); }

// --------------------------------------------------------------------------- Routen
String LOGIN_HTML_err(const String &err) { String s = FPSTR(LOGIN_HTML); s.replace("__ERR__", err); return s; }
void handleRoot() { if (!authed()) { server.send(200,"text/html",LOGIN_HTML_err("")); return; } server.send_P(200,"text/html",APP_HTML); }

void handleLogin() {
  String u = server.arg("u"), p = server.arg("p");
  if (u == g_cfg.loginUser && p == g_cfg.loginPass) {
    g_session = genToken();
    server.sendHeader("Set-Cookie", "oni=" + g_session + "; Path=/; HttpOnly; SameSite=Lax");
    server.sendHeader("Location", "/"); server.send(302, "text/plain", "");
  } else server.send(200, "text/html", LOGIN_HTML_err("Falscher Benutzer oder Passwort"));
}
void handleLogout() {
  g_session = "";
  server.sendHeader("Set-Cookie", "oni=; Path=/; Max-Age=0");
  server.sendHeader("Location", "/"); server.send(302, "text/plain", "");
}

void handleScripts() {
  if (!requireAuth()) return;
  String json = "["; bool first = true;
  File dir = LittleFS.open(SCRIPT_DIR);
  if (dir && dir.isDirectory()) {
    File fe = dir.openNextFile();
    while (fe) {
      String fn = String(fe.name()); int slash = fn.lastIndexOf('/'); if (slash>=0) fn = fn.substring(slash+1);
      if (fn.endsWith(".txt")) {
        String name = fn.substring(0, fn.length()-4);
        json += (first?"":","); json += "{\"name\":\"" + jsonEscape(name) + "\",\"autorun\":" + (isAutorun(name)?"true":"false") + "}";
        first = false;
      }
      fe = dir.openNextFile();
    }
  }
  json += "]"; server.send(200, "application/json", json);
}
void handleGetScript() {
  if (!requireAuth()) return;
  String name = sanitizeName(server.arg("name"));
  File f = LittleFS.open(scriptPath(name), "r"); if (!f) { server.send(404,"text/plain","not found"); return; }
  server.streamFile(f, "text/plain"); f.close();
}
void handleSave() {
  if (!requireAuth()) return;
  String name = sanitizeName(server.arg("name"));
  if (!name.length()) { server.send(400,"text/plain","ungueltiger Name"); return; }
  File f = LittleFS.open(scriptPath(name), "w"); if (!f) { server.send(500,"text/plain","FS-Fehler"); return; }
  f.print(server.arg("content")); f.close(); server.send(200,"text/plain","ok");
}
void handleDelete() {
  if (!requireAuth()) return;
  String name = sanitizeName(server.arg("name"));
  LittleFS.remove(scriptPath(name)); setAutorun(name, false); server.send(200,"text/plain","ok");
}
void handleAutorun() {
  if (!requireAuth()) return;
  setAutorun(sanitizeName(server.arg("name")), server.arg("enabled") == "1"); server.send(200,"text/plain","ok");
}
void handleRun() {
  if (!requireAuth()) return;
  String name = sanitizeName(server.arg("name"));
  if (!LittleFS.exists(scriptPath(name))) { server.send(404,"text/plain","not found"); return; }
  if (g_running) { server.send(200,"text/plain","busy"); return; }
  File f = LittleFS.open(scriptPath(name), "r"); String c = f.readString(); f.close();
  startJob(name, c, server.arg("dry") == "1");
  server.send(200, "text/plain", "started");
}
void handleStop() {
  if (!requireAuth()) return;
  g_stop = true;
  server.send(200, "text/plain", "ok");
}
void handleProgress() {
  if (!requireAuth()) return;
  String log = "";
  if (g_consoleMux) { xSemaphoreTake(g_consoleMux, portMAX_DELAY); log = g_console; xSemaphoreGive(g_consoleMux); }
  String j = "{";
  j += "\"running\":" + String(g_running ? "true" : "false") + ",";
  j += "\"dry\":"     + String(g_dryRun ? "true" : "false") + ",";
  j += "\"line\":"    + String(g_curLine) + ",";
  j += "\"total\":"   + String(g_totalLines) + ",";
  j += "\"name\":\""  + jsonEscape(g_jobName) + "\",";
  j += "\"log\":\""   + jsonEscape(log) + "\"}";
  server.send(200, "application/json", j);
}
void handleGetSettings() {
  if (!requireAuth()) return;
  String j = "{";
  j += "\"user\":\""     + jsonEscape(g_cfg.loginUser) + "\",";
  j += "\"ssid\":\""     + jsonEscape(g_cfg.apSsid)    + "\",";
  j += "\"wifiMode\":\"" + jsonEscape(g_cfg.wifiMode)  + "\",";
  j += "\"staSsid\":\""  + jsonEscape(g_cfg.staSsid)   + "\",";
  j += "\"theme\":\""    + jsonEscape(g_cfg.theme)     + "\",";
  j += "\"layout\":\""   + jsonEscape(g_cfg.layout)    + "\",";
  j += "\"led\":"        + String(g_cfg.ledEnabled?"true":"false") + ",";
  j += "\"btn\":\""      + jsonEscape(g_cfg.buttonScript) + "\",";
  j += "\"usbManu\":\""  + jsonEscape(g_cfg.usbManufacturer) + "\",";
  j += "\"usbProd\":\""  + jsonEscape(g_cfg.usbProduct) + "\",";
  j += "\"usbVid\":\""   + jsonEscape(g_cfg.usbVid) + "\",";
  j += "\"usbPid\":\""   + jsonEscape(g_cfg.usbPid) + "\",";
  j += "\"host\":\""     + jsonEscape(g_cfg.hostname) + "\",";
  j += "\"tspeed\":\""   + jsonEscape(g_cfg.typeSpeed) + "\",";
  j += "\"jiggler\":"    + String(g_cfg.jiggler?"true":"false") + ",";
  j += "\"jigsec\":"     + String(g_cfg.jigglerSec) + ",";
  j += "\"version\":\""  + String(ONI_VERSION) + "\",";
  j += "\"ip\":\""       + jsonEscape(g_ipInfo) + "\"}";
  server.send(200, "application/json", j);
}
void handleSetSettings() {
  if (!requireAuth()) return;
  if (server.hasArg("user")   && server.arg("user").length())   g_cfg.loginUser = server.arg("user");
  if (server.hasArg("pass")   && server.arg("pass").length())   g_cfg.loginPass = server.arg("pass");
  if (server.hasArg("ssid")   && server.arg("ssid").length())   g_cfg.apSsid    = server.arg("ssid");
  if (server.hasArg("appass") && server.arg("appass").length() >= 8) g_cfg.apPass = server.arg("appass");
  if (server.hasArg("wifiMode")) g_cfg.wifiMode = server.arg("wifiMode");
  if (server.hasArg("staSsid"))  g_cfg.staSsid  = server.arg("staSsid");
  if (server.hasArg("staPass") && server.arg("staPass").length()) g_cfg.staPass = server.arg("staPass");
  if (server.hasArg("theme"))    g_cfg.theme    = server.arg("theme");
  if (server.hasArg("layout"))   g_cfg.layout   = server.arg("layout");
  if (server.hasArg("led"))      g_cfg.ledEnabled = (server.arg("led") == "1");
  if (server.hasArg("btn"))      g_cfg.buttonScript = sanitizeName(server.arg("btn"));
  if (server.hasArg("usbManu"))  g_cfg.usbManufacturer = server.arg("usbManu");
  if (server.hasArg("usbProd"))  g_cfg.usbProduct = server.arg("usbProd");
  if (server.hasArg("usbVid"))   g_cfg.usbVid = server.arg("usbVid");
  if (server.hasArg("usbPid"))   g_cfg.usbPid = server.arg("usbPid");
  if (server.hasArg("host") && server.arg("host").length()) g_cfg.hostname = server.arg("host");
  if (server.hasArg("tspeed"))   g_cfg.typeSpeed = server.arg("tspeed");
  if (server.hasArg("jiggler"))  g_cfg.jiggler = (server.arg("jiggler") == "1");
  if (server.hasArg("jigsec"))   g_cfg.jigglerSec = server.arg("jigsec").toInt();
  saveConfig();
  setLocaleByCode(g_cfg.layout);
  server.send(200, "text/plain", "ok");
}

// --------------------------------------------------------------------------- Backup / Restore
// Einfaches Textformat: Kopf, [SET]key=value-Zeilen, [DUCK]name + Inhaltszeilen.
void handleBackup() {
  if (!requireAuth()) return;
  String o = "ONIBACKUP1\n";
  o += "[SET]user="   + g_cfg.loginUser + "\n";
  o += "[SET]pass="   + g_cfg.loginPass + "\n";
  o += "[SET]ssid="   + g_cfg.apSsid + "\n";
  o += "[SET]appass=" + g_cfg.apPass + "\n";
  o += "[SET]wifimode=" + g_cfg.wifiMode + "\n";
  o += "[SET]stassid=" + g_cfg.staSsid + "\n";
  o += "[SET]stapass=" + g_cfg.staPass + "\n";
  o += "[SET]theme="  + g_cfg.theme + "\n";
  o += "[SET]layout=" + g_cfg.layout + "\n";
  o += "[SET]host="   + g_cfg.hostname + "\n";
  o += "[SET]tspeed=" + g_cfg.typeSpeed + "\n";
  o += "[SET]led="    + String(g_cfg.ledEnabled ? "1" : "0") + "\n";
  o += "[SET]jiggler=" + String(g_cfg.jiggler ? "1" : "0") + "\n";
  o += "[SET]jigsec=" + String(g_cfg.jigglerSec) + "\n";
  o += "[SET]btnscript=" + g_cfg.buttonScript + "\n";
  o += "[SET]usbmanu=" + g_cfg.usbManufacturer + "\n";
  o += "[SET]usbprod=" + g_cfg.usbProduct + "\n";
  o += "[SET]usbvid=" + g_cfg.usbVid + "\n";
  o += "[SET]usbpid=" + g_cfg.usbPid + "\n";
  File dir = LittleFS.open(SCRIPT_DIR);
  if (dir && dir.isDirectory()) {
    File fe = dir.openNextFile();
    while (fe) {
      String fn = String(fe.name()); int sl = fn.lastIndexOf('/'); if (sl >= 0) fn = fn.substring(sl + 1);
      if (fn.endsWith(".txt")) {
        String name = fn.substring(0, fn.length() - 4);
        o += "[DUCK]" + name + (isAutorun(name) ? "|1\n" : "|0\n");
        o += fe.readString(); if (!o.endsWith("\n")) o += "\n";
      }
      fe = dir.openNextFile();
    }
  }
  server.send(200, "text/plain", o);
}

void applyBackupSetting(const String &key, const String &val) {
  if (key == "user") g_cfg.loginUser = val; else if (key == "pass") g_cfg.loginPass = val;
  else if (key == "ssid") g_cfg.apSsid = val; else if (key == "appass") g_cfg.apPass = val;
  else if (key == "wifimode") g_cfg.wifiMode = val; else if (key == "stassid") g_cfg.staSsid = val;
  else if (key == "stapass") g_cfg.staPass = val; else if (key == "theme") g_cfg.theme = val;
  else if (key == "layout") g_cfg.layout = val; else if (key == "host") g_cfg.hostname = val;
  else if (key == "tspeed") g_cfg.typeSpeed = val; else if (key == "led") g_cfg.ledEnabled = (val == "1");
  else if (key == "jiggler") g_cfg.jiggler = (val == "1"); else if (key == "jigsec") g_cfg.jigglerSec = val.toInt();
  else if (key == "btnscript") g_cfg.buttonScript = val; else if (key == "usbmanu") g_cfg.usbManufacturer = val;
  else if (key == "usbprod") g_cfg.usbProduct = val; else if (key == "usbvid") g_cfg.usbVid = val;
  else if (key == "usbpid") g_cfg.usbPid = val;
}

void handleRestore() {
  if (!requireAuth()) return;
  String data = server.arg("plain");
  if (!data.startsWith("ONIBACKUP")) { server.send(400, "text/plain", "ungueltiges Backup"); return; }
  String curName = ""; File cur;
  int start = 0, n = data.length();
  while (start <= n) {
    int nl = data.indexOf('\n', start);
    String line = (nl < 0) ? data.substring(start) : data.substring(start, nl);
    if (line.endsWith("\r")) line.remove(line.length() - 1);
    if (line.startsWith("ONIBACKUP")) {
      // Kopf -> alle bisherigen Skripte loeschen (sauberer Restore)
      File d = LittleFS.open(SCRIPT_DIR);
      if (d && d.isDirectory()) { File fe = d.openNextFile(); while (fe) { String p = String(fe.name()); fe = d.openNextFile(); if (p.endsWith(".txt")) LittleFS.remove(p.startsWith("/") ? p : (String(SCRIPT_DIR) + "/" + p)); } }
      LittleFS.remove(AUTORUN_FILE);
    } else if (line.startsWith("[SET]")) {
      if (cur) { cur.close(); }
      String kv = line.substring(5); int eq = kv.indexOf('=');
      if (eq > 0) applyBackupSetting(kv.substring(0, eq), kv.substring(eq + 1));
    } else if (line.startsWith("[DUCK]")) {
      if (cur) { cur.close(); }
      String rest = line.substring(6); bool au = false;
      int bar = rest.lastIndexOf('|');
      if (bar >= 0) { au = (rest.substring(bar + 1) == "1"); rest = rest.substring(0, bar); }
      curName = sanitizeName(rest);
      cur = LittleFS.open(scriptPath(curName), "w");
      if (au) setAutorun(curName, true);
    } else if (cur) {
      cur.println(line);
    }
    if (nl < 0) break; start = nl + 1;
  }
  if (cur) cur.close();
  saveConfig();
  setLocaleByCode(g_cfg.layout);
  server.send(200, "text/plain", "ok");
}

// --------------------------------------------------------------------------- OTA (Firmware ueber WLAN)
static bool otaAuth = false;
void handleUpdatePage() {
  if (!authed()) { server.sendHeader("Location", "/"); server.send(302, "text/plain", ""); return; }
  server.send(200, "text/html",
    "<!doctype html><meta charset=utf-8><meta name=viewport content='width=device-width,initial-scale=1'>"
    "<title>ONI-Duck OTA</title>"
    "<style>body{background:#0a0a0a;color:#f5f5f5;font-family:system-ui;display:flex;min-height:100vh;align-items:center;justify-content:center}"
    ".b{background:#141414;border:1px solid #262626;border-top:3px solid #e01020;border-radius:14px;padding:30px;width:340px}"
    "h1{margin:0 0 14px;letter-spacing:2px}h1 span{color:#e01020}input[type=file]{width:100%;margin:10px 0;color:#9a9a9a}"
    "button{width:100%;padding:12px;border:0;border-radius:8px;background:#e01020;color:#fff;font-weight:600;cursor:pointer}"
    "a{color:#9a9a9a;font-size:13px}</style>"
    "<form class=b method=POST action='/update' enctype='multipart/form-data'>"
    "<h1>ONI<span>-</span>DUCK OTA</h1>"
    "<p style='color:#9a9a9a;font-size:13px'>Firmware (.bin) hochladen. Gerät startet danach neu.</p>"
    "<input type=file name=f accept='.bin'><button type=submit>Update</button>"
    "<p><a href='/'>&larr; zurück</a></p></form>");
}
void handleUpdateDone() {
  bool ok = otaAuth && !Update.hasError();
  server.send(200, "text/html", String("<!doctype html><meta charset=utf-8><body style='background:#0a0a0a;color:#f5f5f5;font-family:system-ui;text-align:center;padding-top:60px'>")
    + (ok ? "<h2>Update OK &ndash; Neustart&hellip;</h2>" : "<h2>Update fehlgeschlagen</h2><a style='color:#e01020' href='/update'>zur&uuml;ck</a>"));
  if (ok) { delay(900); ESP.restart(); }
}
void handleUpdateUpload() {
  HTTPUpload& up = server.upload();
  if (up.status == UPLOAD_FILE_START) {
    otaAuth = authed();
    if (otaAuth) Update.begin(UPDATE_SIZE_UNKNOWN);
  } else if (up.status == UPLOAD_FILE_WRITE) {
    if (otaAuth) Update.write(up.buf, up.currentSize);
  } else if (up.status == UPLOAD_FILE_END) {
    if (otaAuth) Update.end(true);
  }
}

// --------------------------------------------------------------------------- Autostart beim Einstecken
void runAutostart() {
  File f = LittleFS.open(AUTORUN_FILE, "r"); if (!f) return;
  while (f.available()) { String name = f.readStringUntil('\n'); name.trim(); if (name.length()) runScriptByName(name); }
  f.close();
}

// --------------------------------------------------------------------------- WLAN
void startAP() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(g_cfg.apSsid.c_str(), g_cfg.apPass.c_str());
  g_ipInfo = "AP " + g_cfg.apSsid + " / " + WiFi.softAPIP().toString();
  oniLed(0, 0, 30);                        // blau = AP bereit
}
void startWiFi() {
  if (g_cfg.wifiMode == "STA" && g_cfg.staSsid.length()) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(g_cfg.staSsid.c_str(), g_cfg.staPass.c_str());
    uint32_t t0 = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t0 < 9000) { delay(200); }
    if (WiFi.status() == WL_CONNECTED) {
      g_ipInfo = "STA " + g_cfg.staSsid + " / " + WiFi.localIP().toString();
      oniLed(0, 0, 30);
      return;
    }
    // Fehlschlag -> AP-Fallback
  }
  startAP();
}

// --------------------------------------------------------------------------- Button
void pollButton() {
  static bool last = HIGH; static uint32_t t = 0;
  bool b = digitalRead(BUTTON_PIN);
  if (b != last && millis() - t > 60) {
    t = millis(); last = b;
    if (b == LOW && !g_running && g_cfg.buttonScript.length() && LittleFS.exists(scriptPath(g_cfg.buttonScript))) {
      File f = LittleFS.open(scriptPath(g_cfg.buttonScript), "r"); String c = f.readString(); f.close();
      startJob(g_cfg.buttonScript, c, false);
    }
  }
}

// --------------------------------------------------------------------------- Setup / Loop
void setup() {
  loadConfig();
  setLocaleByCode(g_cfg.layout);
  randomSeed(esp_random());
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Optionale eigene USB-Kennung VOR USB.begin() setzen
  if (g_cfg.usbManufacturer.length()) USB.manufacturerName(g_cfg.usbManufacturer.c_str());
  if (g_cfg.usbProduct.length())      USB.productName(g_cfg.usbProduct.c_str());
  if (g_cfg.usbVid.length())          USB.VID((uint16_t)strtol(g_cfg.usbVid.c_str(), nullptr, 0));
  if (g_cfg.usbPid.length())          USB.PID((uint16_t)strtol(g_cfg.usbPid.c_str(), nullptr, 0));

  // HID-Geraete + USB starten
  Keyboard.begin();
  Mouse.begin();
  Consumer.begin();
  USB.begin();

  if (!LittleFS.begin(true)) { /* bei Bedarf formatieren */ }
  if (!LittleFS.exists(SCRIPT_DIR)) LittleFS.mkdir(SCRIPT_DIR);
  seedDefaultScript();

  g_consoleMux = xSemaphoreCreateMutex();
  // ESP32-S2 ist Single-Core -> keine feste Core-Bindung (tskNO_AFFINITY),
  // sonst stuerzt der Start ab. Auf S3 (Dual-Core) funktioniert das ebenso.
  xTaskCreatePinnedToCore(duckTaskFn, "duck", 8192, nullptr, 1, &g_duckTask, tskNO_AFFINITY);

  startWiFi();

  const char* headers[] = {"Cookie"};
  server.collectHeaders(headers, 1);
  server.on("/",             HTTP_GET,  handleRoot);
  server.on("/login",        HTTP_POST, handleLogin);
  server.on("/logout",       HTTP_GET,  handleLogout);
  server.on("/api/scripts",  HTTP_GET,  handleScripts);
  server.on("/api/script",   HTTP_GET,  handleGetScript);
  server.on("/api/save",     HTTP_POST, handleSave);
  server.on("/api/delete",   HTTP_POST, handleDelete);
  server.on("/api/autorun",  HTTP_POST, handleAutorun);
  server.on("/api/run",      HTTP_POST, handleRun);
  server.on("/api/stop",     HTTP_POST, handleStop);
  server.on("/api/progress", HTTP_GET,  handleProgress);
  server.on("/api/settings", HTTP_GET,  handleGetSettings);
  server.on("/api/settings", HTTP_POST, handleSetSettings);
  server.on("/api/backup",   HTTP_GET,  handleBackup);
  server.on("/api/restore",  HTTP_POST, handleRestore);
  server.on("/update",       HTTP_GET,  handleUpdatePage);
  server.on("/update",       HTTP_POST, handleUpdateDone, handleUpdateUpload);
  server.onNotFound([]() { server.sendHeader("Location","/"); server.send(302,"text/plain",""); });
  server.begin();

  // mDNS: erreichbar unter http://<hostname>.local
  if (MDNS.begin(g_cfg.hostname.c_str())) MDNS.addService("http", "tcp", 80);

  delay(2500);                 // warten, bis der Host die Tastatur erkennt
  oniLed(0, 12, 0);            // gruen = bereit
  runAutostart();
}

// Maus-Jiggler: bewegt die Maus periodisch minimal (netto 0), haelt den PC wach
void pollJiggler() {
  if (!g_cfg.jiggler) return;
  static uint32_t t = 0;
  if (millis() - t > (uint32_t)g_cfg.jigglerSec * 1000) {
    t = millis();
    Mouse.move(1, 0); delay(30); Mouse.move(-1, 0);
  }
}

void loop() {
  server.handleClient();
  pollButton();
  pollJiggler();
}
