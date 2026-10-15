#pragma once
#include <Arduino.h>
#include "USBHIDMouse.h"
#include "USBHIDConsumerControl.h"
#include "Config.h"
#include "Layouts.h"
#include "Keymap.h"

// ---------------------------------------------------------------------------
// Ducky-Script-Interpreter (orientiert an WiFiDuck / SuperWiFiDuck).
// Laeuft im Hintergrund-Task; unterstuetzt Abbruch (g_stop), Dry-Run
// (g_dryRun, tippt nichts) und ein Fortschritts-Log (consoleLog).
// ---------------------------------------------------------------------------

extern USBHIDMouse            Mouse;
extern USBHIDConsumerControl  Consumer;
extern void oniLed(uint8_t r, uint8_t g, uint8_t b);

extern bool          g_dryRun;      // tippt nicht, nur Log
extern volatile bool g_stop;        // Abbruchwunsch
extern volatile int  g_curLine;     // aktuelle Zeile (1-basiert)
extern volatile int  g_totalLines;  // Gesamtzahl Zeilen
extern void consoleLog(const String &s);

// Abbrechbares, dry-run-bewusstes Delay
inline void oniDelay(uint32_t ms) {
  if (g_dryRun) return;
  uint32_t t = millis();
  while (millis() - t < ms) { if (g_stop) return; delay(5); }
}

inline int parseInts(const String &s, long *out, int maxN) {
  int c = 0, i = 0, n = s.length();
  while (i < n && c < maxN) {
    while (i < n && s[i] == ' ') i++;
    int st = i; while (i < n && s[i] != ' ') i++;
    if (st >= i) break;
    out[c++] = strtol(s.substring(st, i).c_str(), nullptr, 0);
  }
  return c;
}

inline void moveMouse(long x, long y, long wheel = 0, long pan = 0) {
  while (x || y || wheel || pan) {
    int8_t dx = (int8_t)constrain(x, -120, 120);
    int8_t dy = (int8_t)constrain(y, -120, 120);
    int8_t dw = (int8_t)constrain(wheel, -120, 120);
    int8_t dp = (int8_t)constrain(pan, -120, 120);
    Mouse.move(dx, dy, dw, dp);
    x -= dx; y -= dy; wheel -= dw; pan -= dp;
    delay(2);
  }
}

inline void duckRunLine(String line, int &defDelay, String &prev) {
  while (line.length() && (line[line.length()-1] == '\r' || line[line.length()-1] == '\n'))
    line.remove(line.length() - 1);
  if (line.length() == 0) return;

  int sp = line.indexOf(' ');
  String cmd  = (sp < 0) ? line : line.substring(0, sp);
  String rest = (sp < 0) ? ""   : line.substring(sp + 1);
  String CMD = cmd; CMD.toUpperCase();

  if (CMD == "REM" || CMD.startsWith("//")) return;
  if (CMD == "DEFAULTDELAY" || CMD == "DEFAULT_DELAY") { defDelay = rest.toInt(); return; }
  if (CMD == "DELAY")    { oniDelay(rest.toInt()); return; }
  if (CMD == "STRING")   { typeStringLocale(rest); prev = line; return; }
  if (CMD == "STRINGLN") { typeStringLocale(rest); tapUsage(0x28); prev = line; return; }
  if (CMD == "LOCALE")   { setLocaleByCode(rest); return; }
  if (CMD == "REPEAT" || CMD == "REPLAY") {
    int n = rest.toInt();
    for (int i = 0; i < n && !g_stop; i++) { String p = prev; duckRunLine(p, defDelay, prev); if (defDelay) oniDelay(defDelay); }
    return;
  }
  if (CMD == "LED") {
    long v[3] = {0,0,0}; parseInts(rest, v, 3);
    if (!g_dryRun) oniLed((uint8_t)v[0], (uint8_t)v[1], (uint8_t)v[2]);
    return;
  }
  if (CMD == "KEYCODE") {
    long v[7]; int c = parseInts(rest, v, 7);
    if (c >= 1 && !g_dryRun) {
      uint8_t mod = (uint8_t)v[0];
      for (int b = 0; b < 8; b++) if (mod & (1 << b)) Keyboard.press(0x80 + b);
      for (int k = 1; k < c; k++) Keyboard.press(0x88 + (uint8_t)v[k]);
      delay(12); Keyboard.releaseAll();
    }
    prev = line; return;
  }
  // --- Maus ---
  if (CMD == "MOUSE")  { long v[2]={0,0}; parseInts(rest,v,2); if(!g_dryRun) moveMouse(v[0],v[1]); prev=line; return; }
  if (CMD == "SCROLL" || CMD == "MOUSE_SCROLL") { long v[2]={0,0}; parseInts(rest,v,2); if(!g_dryRun) moveMouse(0,0,v[1],v[0]); prev=line; return; }
  if (CMD == "CLICK"   || CMD == "MOUSE_CLICK")   { long v[1]={1}; parseInts(rest,v,1); if(!g_dryRun) Mouse.click((uint8_t)v[0]); prev=line; return; }
  if (CMD == "PRESS"   || CMD == "MOUSE_PRESS")   { long v[1]={1}; parseInts(rest,v,1); if(!g_dryRun) Mouse.press((uint8_t)v[0]); prev=line; return; }
  if (CMD == "RELEASE" || CMD == "MOUSE_RELEASE") { long v[1]={1}; parseInts(rest,v,1); if(!g_dryRun) Mouse.release((uint8_t)v[0]); prev=line; return; }

  // --- Media-/Consumer-Taste (einzelnes Token) ---
  if (sp < 0) {
    uint16_t cc = nameToConsumer(CMD);
    if (cc) { if (!g_dryRun) { Consumer.press(cc); delay(8); Consumer.release(); } prev = line; return; }
  }

  // --- Tastenkombination: alle Tasten der Zeile gleichzeitig -----------------
  if (g_dryRun) { prev = line; return; }
  String seq = line; seq.replace("-", " ");
  int uc = 0, idx = 0, len = seq.length();
  while (idx < len) {
    while (idx < len && seq[idx] == ' ') idx++;
    int st = idx; while (idx < len && seq[idx] != ' ') idx++;
    if (st >= idx) break;
    String tok = seq.substring(st, idx);
    String T = tok; T.toUpperCase();
    uint8_t m = modCode(T);
    if (m) { Keyboard.press(m); continue; }
    uint8_t u = nameToUsage(T);
    if (!u && tok.length() == 1) u = usageForChar(tok[0]);
    if (u && uc < 6) { Keyboard.press(0x88 + u); uc++; }
  }
  delay(12);
  Keyboard.releaseAll();
  delay(4);
  prev = line;
}

inline int countLines(const String &s) {
  int c = 1; for (int i = 0; i < (int)s.length(); i++) if (s[i] == '\n') c++;
  return c;
}

inline void duckRunScript(const String &s) {
  setLocaleByCode(g_cfg.layout);
  if      (g_cfg.typeSpeed == "fast")  { g_charDelay = 0;  g_jitter = 0;  }
  else if (g_cfg.typeSpeed == "human") { g_charDelay = 22; g_jitter = 55; }
  else                                 { g_charDelay = 4;  g_jitter = 0;  }

  int defDelay = 0; String prev = "";
  g_totalLines = countLines(s); g_curLine = 0;
  if (!g_dryRun && g_cfg.ledEnabled) oniLed(40, 0, 0);     // rot = laeuft

  int start = 0, n = s.length();
  while (start <= n) {
    if (g_stop) break;
    int nl = s.indexOf('\n', start);
    String line = (nl < 0) ? s.substring(start) : s.substring(start, nl);
    g_curLine++;
    String t = line; t.trim();
    if (t.length()) consoleLog(String(g_curLine) + ": " + line);
    duckRunLine(line, defDelay, prev);
    if (defDelay) oniDelay(defDelay);
    if (nl < 0) break; start = nl + 1;
  }
  if (!g_dryRun) Keyboard.releaseAll();
  if (!g_dryRun && g_cfg.ledEnabled) oniLed(0, 12, 0);     // gruen = bereit
  consoleLog(g_stop ? "[Gestoppt]" : (g_dryRun ? "[Dry-Run beendet]" : "[Fertig]"));
}
