# ONI-Duck

**WiFi Rubber Ducky für den ESP32-S2** – eine per WLAN steuerbare USB-Tastatur (BadUSB) mit passwortgeschützter Weboberfläche.

> ⚠️ **Nur für eigene Geräte oder mit ausdrücklicher Erlaubnis.** Dieses Werkzeug ist zum Lernen, Testen und für Automatisierung gedacht. Der Einsatz an fremden Rechnern ohne Zustimmung kann strafbar sein. Du bist für die Nutzung selbst verantwortlich.

---

## Was ist das?

Ein ESP32-S2 meldet sich am PC als USB-Tastatur (und Maus) an und tippt gespeicherte **Ducky-Script**-Skripte. Verwaltet wird alles bequem über eine Weboberfläche per WLAN – kein Treiber, keine App, kein Neukompilieren nötig.

Angelehnt an [WiFiDuck](https://github.com/SpacehuhnTech/WiFiDuck) und [SuperWiFiDuck](https://github.com/ThingPulse/SuperWiFiDuck), neu aufgebaut für das native USB des ESP32-S2.

## Hardware

- **ESP32-S2** (entwickelt/getestet mit dem **ESP32-S2-DevKitC-1**)
- USB-Datenkabel
- Optional: die On-Board-RGB-LED (WS2812 an GPIO18) dient als Statusanzeige

## Funktionen

- USB-HID: **Tastatur, Maus und Media-/Consumer-Tasten**
- **Weboberfläche** (Theme Schwarz/Rot/Weiß, Dark-/Light-Umschalter, Oberflächensprache umschaltbar)
- Skripte **anlegen, bearbeiten, importieren, herunterladen, per Knopf starten**
- **Autostart** beim Einstecken und Start über den **BOOT-Taster**
- **Live-Konsole**, **Stop** (Abbruch während der Ausführung) und **Dry-Run** (Testlauf ohne echtes Tippen)
- **Ducky Script**: `STRING`, `STRINGLN`, `DELAY`, `DEFAULTDELAY`, `REPEAT`, `LOCALE`, `KEYCODE`, `MOUSE`, `CLICK`/`PRESS`/`RELEASE`, `SCROLL`, `LED`, Media-Tasten u. v. m.
- **18 Tastaturlayouts** (DE, US, GB, ES, DK, RU, FR, BE, PT, IT, SK, CZ, SI, BG, CA-FR, CH-DE, CH-FR, HU)
- Eigener **WLAN-Hotspot (AP)** oder Verbindung zu vorhandenem WLAN (**STA**)
- **mDNS** (`http://oni-duck.local`), **OTA-Update** über die Weboberfläche
- **Backup/Restore** aller Skripte und Einstellungen
- Einstellbare **Tippgeschwindigkeit** (inkl. „menschlichem" Jitter) und **Maus-Jiggler**
- Optionale eigene **USB-Kennung** (VID/PID/Hersteller/Produkt)

---

## Installation / Flashen (Arduino IDE)

### 1. Voraussetzungen

1. [Arduino IDE](https://www.arduino.cc/en/software) installieren.
2. ESP32-Boardpaket einrichten: *Datei → Einstellungen → Zusätzliche Boardverwalter-URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. *Werkzeuge → Board → Boardverwalter* öffnen, nach **esp32** suchen und das Paket von Espressif installieren.
4. Falls der COM-Port fehlt: USB-UART-Treiber installieren (CP2102 oder CH340).

### 2. Projekt öffnen

Den Ordner `ONI_Duck` öffnen und darin die `ONI_Duck.ino` starten. **Alle Dateien müssen im selben Ordner liegen** (die `.ino` plus alle `.h`-Dateien) – oben in der IDE sollten mehrere Tabs erscheinen.

### 3. Tools-Einstellungen

| Einstellung           | Wert                          |
| --------------------- | ----------------------------- |
| Board                 | **ESP32S2 Dev Module**        |
| USB CDC On Boot       | **Enabled**                   |
| USB DFU On Boot       | Disabled                      |
| Upload Mode           | Internal USB *(oder UART0 / Hardware CDC)* |
| USB Mode              | **USB-OTG (TinyUSB)**         |
| Partition Scheme      | **Default 4MB with spiffs** *(OTA-fähig)* |
| Port                  | COM-Port des ESP              |

### 4. Hochladen

1. Zum Flashen das Kabel in die **USB**-Buchse (nativer S2-USB) stecken.
2. **Upload** klicken.
3. Startet der Upload nicht (bleibt bei „Connecting…") oder fehlt der Port:
   **BOOT gedrückt halten → kurz RESET antippen → BOOT loslassen → Upload** erneut klicken.
4. Nach „Hash of data verified" / „Done uploading" einmal **RESET** drücken.

### Häufige Stolpersteine

- *„Config.h: No such file or directory"* → nicht alle Dateien im selben Ordner.
- *„Invalid head of packet" / „port is busy"* → Board nicht im Download-Modus (BOOT/RESET-Trick), seriellen Monitor schließen, Datenkabel (kein reines Ladekabel) verwenden, Upload Speed auf 115200 senken.
- Der COM-Port kann sich im Download-Modus ändern → Port neu auswählen.

---

## Erste Schritte

1. Nach dem Flashen das Kabel in die **USB**-Buchse stecken.
2. Mit dem WLAN **`ONI-Duck`** verbinden (Standard-Passwort: `oni-duck-1234`).
3. Im Browser **`http://192.168.4.1`** oder **`http://oni-duck.local`** öffnen.
4. Anmelden – Standard: Benutzer **`ONI`**, Passwort **`oni`**.
5. **SSID, Passwörter und Login in den Einstellungen ändern!**

Statusanzeige der RGB-LED (falls vorhanden): blau = WLAN bereit, grün = bereit, rot = Skript läuft.

## Weboberfläche

- **Skripte:** erstellen/bearbeiten, importieren, herunterladen, „beim Einstecken" (Autostart), ▶ Start (echt) und Test (Dry-Run).
- **Konsole:** zeigt live die laufende Zeile und Ausgaben; **Stop** bricht ab.
- **Einstellungen:** Login, WLAN-Modus (AP/STA), Tastaturlayout (mit Flaggen), Gerätename (mDNS), Tippgeschwindigkeit, Status-LED, BOOT-Knopf-Skript, Backup/Restore, OTA, USB-Kennung.

## Ducky Script (Kurzreferenz)

```
REM Kommentar
DEFAULTDELAY 200      Pause nach jeder Zeile (ms)
DELAY 1000            einmalige Pause (ms)
STRING Text           Text tippen
STRINGLN Text         Text tippen + Enter
REPEAT 3              vorige Zeile 3x wiederholen
LOCALE DE             Tastaturlayout umschalten
ENTER TAB ESC F1..F12 benannte Tasten
GUI r                 Tasten gleichzeitig (Win+R)
CTRL ALT DELETE       Kombination
KEYCODE 0x02 0x04     rohe HID-Codes
MOUSE 40 -30          Maus relativ bewegen
CLICK 1               Mausklick (L1 R2 M4 Back8 Fwd16)
SCROLL 0 10           Mausrad
VOLUME_UP PLAYPAUSE   Media-/Consumer-Tasten
LED 40 0 0            RGB-LED setzen (0-255)
```

**Beispiel** (`examples/ONI-Vorstellung.txt`): öffnet den Windows-Editor und stellt das Projekt vor.

## OTA-Update

Firmware ohne Kabel aktualisieren: in der Arduino IDE *Sketch → Kompilierte Binärdatei exportieren*, dann in der Weboberfläche auf **Firmware-Update (OTA)** (bzw. `http://192.168.4.1/update`) die `.bin` hochladen.

---

## Haftungsausschluss

Dieses Projekt dient ausschließlich zu Test-, Lern- und Schulungszwecken. Setze es niemals ein, um Schaden anzurichten oder in fremde Systeme ohne Erlaubnis einzugreifen. Die Verantwortung für die Nutzung liegt allein bei dir.

## Credits

- [WiFiDuck](https://github.com/SpacehuhnTech/WiFiDuck) von Spacehuhn Technologies (MIT) – Ducky-Script-Format und die Tastaturlayout-Tabellen (`locale_*.h`).
- [SuperWiFiDuck](https://github.com/ThingPulse/SuperWiFiDuck) von ThingPulse (MIT) – native USB-Umsetzung für ESP32-S2/S3.
- `usb_hid_keys.h` von [MightyPork](https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2) (Public Domain).

## Lizenz

Veröffentlicht unter der **MIT-Lizenz** – siehe [LICENSE](LICENSE). Die übernommenen `locale_*.h`- und `usb_hid_keys.h`-Dateien behalten ihre ursprünglichen Lizenzvermerke.
