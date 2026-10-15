#pragma once
#include <Arduino.h>

// ---------------------------------------------------------------------------
// Weboberflaeche. Theme Schwarz/Rot/Weiss (Dark+Light), umschaltbare Sprache
// mit echten Inline-SVG-Flaggen (Emoji-Flaggen werden unter Windows nicht
// angezeigt), Tastaturlayout-Auswahl ebenfalls mit Flaggen.
// ---------------------------------------------------------------------------

static const char LOGIN_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="de"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ONI-Duck &middot; Login</title>
<style>
:root{--bg:#0a0a0a;--card:#141414;--fg:#f5f5f5;--muted:#9a9a9a;--red:#e01020;--border:#262626}
*{box-sizing:border-box}
body{margin:0;min-height:100vh;display:flex;align-items:center;justify-content:center;
background:var(--bg);color:var(--fg);font-family:system-ui,Segoe UI,Roboto,sans-serif}
.box{background:var(--card);border:1px solid var(--border);border-top:3px solid var(--red);
border-radius:14px;padding:32px;width:320px;box-shadow:0 20px 60px rgba(0,0,0,.5)}
h1{margin:0 0 4px;font-size:26px;letter-spacing:2px}h1 span{color:var(--red)}
p{margin:0 0 22px;color:var(--muted);font-size:13px}
label{display:block;font-size:12px;color:var(--muted);margin:14px 0 6px;text-transform:uppercase;letter-spacing:1px}
input{width:100%;padding:11px 12px;border-radius:8px;border:1px solid var(--border);background:#0e0e0e;color:var(--fg);font-size:15px}
input:focus{outline:none;border-color:var(--red)}
button{margin-top:22px;width:100%;padding:12px;border:none;border-radius:8px;background:var(--red);color:#fff;font-size:15px;font-weight:600;cursor:pointer;letter-spacing:1px}
button:hover{filter:brightness(1.1)}
.err{color:var(--red);font-size:13px;margin-top:14px;min-height:16px;text-align:center}
</style></head><body>
<form class="box" method="POST" action="/login">
<h1>ONI<span>-</span>DUCK</h1><p>Zugriff geschützt</p>
<label>Benutzer</label><input name="u" autocomplete="username" autofocus>
<label>Passwort</label><input name="p" type="password" autocomplete="current-password">
<button type="submit">Anmelden</button>
<div class="err">__ERR__</div>
</form></body></html>)HTML";


static const char APP_HTML[] PROGMEM = R"HTML(<!doctype html>
<html lang="de" data-theme="dark"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>ONI-Duck</title>
<style>
:root{--bg:#0a0a0a;--card:#141414;--card2:#0e0e0e;--fg:#f5f5f5;--muted:#9a9a9a;--red:#e01020;--border:#262626}
[data-theme=light]{--bg:#f4f4f5;--card:#fff;--card2:#fafafa;--fg:#141414;--muted:#6b6b6b;--red:#d00616;--border:#e2e2e2}
*{box-sizing:border-box}
body{margin:0;background:var(--bg);color:var(--fg);font-family:system-ui,Segoe UI,Roboto,sans-serif}
header{display:flex;align-items:center;gap:12px;padding:16px 22px;border-bottom:1px solid var(--border);position:sticky;top:0;background:var(--bg);z-index:5}
header h1{font-size:22px;margin:0;letter-spacing:2px}header h1 span{color:var(--red)}
.spacer{flex:1}
.btn{border:1px solid var(--border);background:var(--card);color:var(--fg);padding:8px 14px;border-radius:8px;cursor:pointer;font-size:14px}
.btn:hover{border-color:var(--red)}
.btn.red{background:var(--red);border-color:var(--red);color:#fff}
.btn.sm{padding:6px 10px;font-size:13px}
main{max-width:860px;margin:0 auto;padding:22px}
.card{background:var(--card);border:1px solid var(--border);border-radius:12px;padding:18px;margin-bottom:20px}
.card h2{margin:0 0 14px;font-size:15px;text-transform:uppercase;letter-spacing:1px;color:var(--muted)}
.card h2 b{color:var(--red)}
label{display:block;font-size:12px;color:var(--muted);margin:10px 0 5px;text-transform:uppercase;letter-spacing:1px}
input[type=text],input[type=password],textarea,select{width:100%;padding:10px 12px;border-radius:8px;border:1px solid var(--border);background:var(--card2);color:var(--fg);font-size:14px;font-family:inherit}
textarea{min-height:190px;resize:vertical;font-family:ui-monospace,Consolas,monospace;line-height:1.5}
input:focus,textarea:focus,select:focus{outline:none;border-color:var(--red)}
.row{display:flex;gap:10px;flex-wrap:wrap}.row>*{flex:1;min-width:140px}
.item{display:flex;align-items:center;gap:10px;padding:11px 0;border-bottom:1px solid var(--border);flex-wrap:wrap}
.item:last-child{border-bottom:none}.item .name{flex:1 1 140px;min-width:120px;font-weight:600;overflow-wrap:anywhere}
.item .ctl{display:flex;align-items:center;gap:8px;flex-wrap:wrap;justify-content:flex-end}
.chk{display:flex;align-items:center;gap:6px;font-size:12px;color:var(--muted);cursor:pointer;white-space:nowrap}
.chk input{width:16px;height:16px;accent-color:var(--red)}
.hint{font-size:12px;color:var(--muted);margin-top:8px;line-height:1.7}
.hint code{background:var(--card2);padding:1px 6px;border-radius:4px;border:1px solid var(--border)}
.toast{position:fixed;bottom:22px;left:50%;transform:translateX(-50%);background:var(--card);border:1px solid var(--red);padding:12px 20px;border-radius:10px;opacity:0;transition:.3s;pointer-events:none;z-index:50}
.toast.show{opacity:1}
details summary{cursor:pointer;color:var(--muted);font-size:13px}
.ip{font-size:12px;color:var(--muted);margin-top:6px}
/* Flaggen-Dropdown */
.dd{position:relative}
.dd-btn{display:flex;align-items:center;gap:7px;border:1px solid var(--border);background:var(--card);color:var(--fg);padding:7px 10px;border-radius:8px;cursor:pointer;font-size:14px}
.dd-btn:hover{border-color:var(--red)}
.flag{width:20px;height:14px;display:inline-block;flex:none;border-radius:2px;overflow:hidden;box-shadow:0 0 0 1px rgba(0,0,0,.18)}
.flag svg{width:20px;height:14px;display:block}
.dd-car{color:var(--muted);font-size:11px}
.dd-menu{display:none;position:absolute;right:0;top:calc(100% + 6px);background:var(--card);border:1px solid var(--border);border-radius:10px;padding:6px;min-width:210px;max-height:320px;overflow:auto;z-index:40;box-shadow:0 14px 40px rgba(0,0,0,.45)}
.dd.open .dd-menu{display:block}
.dd-opt{display:flex;align-items:center;gap:9px;padding:8px 10px;border-radius:7px;cursor:pointer;font-size:14px;white-space:nowrap}
.dd-opt:hover{background:var(--card2);color:var(--red)}
.dd-full{display:block}
.dd-full .dd-btn{width:100%;justify-content:flex-start}
.dd-full .dd-menu{left:0;right:auto;width:100%}
#console{background:var(--card2);border:1px solid var(--border);border-radius:8px;padding:10px;min-height:54px;max-height:260px;overflow:auto;font-family:ui-monospace,Consolas,monospace;font-size:12.5px;line-height:1.5;white-space:pre-wrap;word-break:break-word;margin:0 0 12px}
#runState{font-weight:400;text-transform:none;letter-spacing:0;color:var(--muted);margin-left:8px}
</style></head><body>
<header>
  <h1>ONI<span>-</span>DUCK</h1><div class="spacer"></div>
  <div id="langDD"></div>
  <button class="btn sm" onclick="toggleTheme()">◐ <span data-i18n="theme">Theme</span></button>
  <button class="btn sm" onclick="location.href='/logout'" data-i18n="logout">Logout</button>
</header>
<main>
  <div class="card">
    <h2><b>&rsaquo;</b> <span data-i18n="editTitle"></span></h2>
    <label data-i18n="name"></label>
    <input type="text" id="sName" placeholder="sperren" maxlength="32">
    <label data-i18n="script"></label>
    <textarea id="sBody" placeholder="REM PC sperren&#10;GUI l"></textarea>
    <div class="row" style="margin-top:14px">
      <button class="btn red" onclick="save()" data-i18n="save"></button>
      <button class="btn" onclick="clearEditor()" data-i18n="clear"></button>
      <button class="btn" onclick="document.getElementById('imp').click()" data-i18n="imp"></button>
      <input type="file" id="imp" accept=".txt,.duck,.script,text/plain" style="display:none" onchange="importFile(event)">
    </div>
    <div class="hint">
      <code>STRING</code> <code>STRINGLN</code> <code>ENTER</code> <code>DELAY ms</code>
      <code>GUI r</code> <code>CTRL ALT DELETE</code> <code>ALT F4</code>
      <code>REPEAT n</code> <code>LOCALE DE</code> <code>MOUSE 40 -30</code>
      <code>VOLUME_UP</code> <code>LED 40 0 0</code> <code>REM</code>
    </div>
  </div>

  <div class="card">
    <h2><b>&rsaquo;</b> <span data-i18n="savedTitle"></span></h2>
    <div id="list"></div>
  </div>

  <div class="card">
    <h2><b>&rsaquo;</b> <span data-i18n="console"></span><span id="runState"></span></h2>
    <pre id="console"></pre>
    <button class="btn" id="stopBtn" onclick="stopRun()" data-i18n="stop" style="display:none"></button>
  </div>

  <div class="card">
    <h2><b>&rsaquo;</b> <span data-i18n="settingsTitle"></span></h2>
    <details><summary data-i18n="expand"></summary>
      <div style="margin-top:14px">
        <div class="row">
          <div><label data-i18n="loginUser"></label><input type="text" id="cUser"></div>
          <div><label data-i18n="loginPass"></label><input type="password" id="cPass" placeholder="••••••"></div>
        </div>
        <div class="row">
          <div><label data-i18n="wifiMode"></label>
            <select id="cWifiMode"><option value="AP" data-i18n="modeAP"></option><option value="STA" data-i18n="modeSTA"></option></select>
          </div>
          <div><label data-i18n="layout"></label><div id="layoutDD"></div></div>
        </div>
        <div class="row">
          <div><label data-i18n="apName"></label><input type="text" id="cSsid"></div>
          <div><label data-i18n="apPass"></label><input type="password" id="cAppass" placeholder="••••••"></div>
        </div>
        <div class="row">
          <div><label data-i18n="staSsid"></label><input type="text" id="cStaSsid"></div>
          <div><label data-i18n="staPass"></label><input type="password" id="cStaPass" placeholder="••••••"></div>
        </div>
        <div class="row">
          <div><label data-i18n="btnScript"></label><input type="text" id="cBtn" placeholder="sperren"></div>
          <div><label data-i18n="statusLed"></label>
            <select id="cLed"><option value="1" data-i18n="on"></option><option value="0" data-i18n="off"></option></select>
          </div>
        </div>
        <div class="row">
          <div><label data-i18n="hostname"></label><input type="text" id="cHost" placeholder="oni-duck"></div>
          <div><label data-i18n="typeSpeed"></label>
            <select id="cSpeed"><option value="fast" data-i18n="spFast"></option><option value="normal" data-i18n="spNormal"></option><option value="human" data-i18n="spHuman"></option></select>
          </div>
        </div>
        <div class="row">
          <div><label data-i18n="jiggler"></label>
            <select id="cJig"><option value="0" data-i18n="off"></option><option value="1" data-i18n="on"></option></select>
          </div>
          <div><label data-i18n="jigSec"></label><input type="text" id="cJigSec" placeholder="30"></div>
        </div>
        <div class="ip" id="ipInfo"></div>
        <button class="btn red" style="margin-top:16px" onclick="saveSettings()" data-i18n="saveSettings"></button>
        <div class="row" style="margin-top:12px">
          <button class="btn" onclick="downloadBackup()" data-i18n="backupDl"></button>
          <button class="btn" onclick="document.getElementById('bkup').click()" data-i18n="backupUp"></button>
          <input type="file" id="bkup" accept=".txt" style="display:none" onchange="restoreBackup(event)">
          <a class="btn" id="otaLink" href="/update" style="text-align:center;text-decoration:none;line-height:1.6" data-i18n="ota"></a>
        </div>
        <details style="margin-top:16px"><summary data-i18n="advUsb"></summary>
          <div class="row" style="margin-top:12px">
            <div><label data-i18n="manufacturer"></label><input type="text" id="uManu" placeholder="Logitech"></div>
            <div><label data-i18n="product"></label><input type="text" id="uProd" placeholder="USB Keyboard"></div>
          </div>
          <div class="row">
            <div><label>VID (hex)</label><input type="text" id="uVid" placeholder="0x046d"></div>
            <div><label>PID (hex)</label><input type="text" id="uPid" placeholder="0xc31c"></div>
          </div>
        </details>
        <div class="hint" data-i18n="note"></div>
      </div>
    </details>
  </div>
</main>
<div class="toast" id="toast"></div>
<script>
const $=s=>document.querySelector(s);

/* ---------- Inline-SVG-Flaggen (funktionieren auch unter Windows) ---------- */
const R=(x,y,w,h,c)=>`<rect x="${x}" y="${y}" width="${w}" height="${h}" fill="${c}"/>`;
const HB=(a,b,c)=>`<svg viewBox="0 0 9 6">${R(0,0,9,2,a)}${R(0,2,9,2,b)}${R(0,4,9,2,c)}</svg>`;
const VB=(a,b,c)=>`<svg viewBox="0 0 9 6">${R(0,0,3,6,a)}${R(3,0,3,6,b)}${R(6,0,3,6,c)}</svg>`;
function usFlag(){let s='<svg viewBox="0 0 19 10">';for(let i=0;i<13;i++)s+=R(0,i*10/13,19,10/13,i%2?'#fff':'#B22234');s+=R(0,0,7.6,5.38,'#3C3B6E');for(let r=0;r<4;r++)for(let c=0;c<6;c++)s+=`<circle cx="${0.7+c*1.15}" cy="${0.7+r*1.2}" r="0.2" fill="#fff"/>`;return s+'</svg>';}
const FLAG={
 DE:HB('#000','#DD0000','#FFCE00'), RU:HB('#fff','#0039A6','#D52B1E'),
 BG:HB('#fff','#00966E','#D62612'), HU:HB('#CE2939','#fff','#477050'),
 FR:VB('#0055A4','#fff','#EF4135'), IT:VB('#009246','#fff','#CE2B37'),
 BE:VB('#000','#FAE042','#ED2939'),
 ES:`<svg viewBox="0 0 9 6">${R(0,0,9,6,'#AA151B')}${R(0,1.5,9,3,'#F1BF00')}</svg>`,
 PT:`<svg viewBox="0 0 9 6">${R(0,0,9,6,'#FF0000')}${R(0,0,3.6,6,'#006600')}<circle cx="3.6" cy="3" r="1.1" fill="#FFE900"/><circle cx="3.6" cy="3" r="0.5" fill="#AA1515"/></svg>`,
 DK:`<svg viewBox="0 0 12 9">${R(0,0,12,9,'#C8102E')}${R(4,0,1.5,9,'#fff')}${R(0,3.75,12,1.5,'#fff')}</svg>`,
 CH:`<svg viewBox="0 0 6 6">${R(0,0,6,6,'#D52B1E')}${R(2.4,1,1.2,4,'#fff')}${R(1,2.4,4,1.2,'#fff')}</svg>`,
 CZ:`<svg viewBox="0 0 9 6">${R(0,0,9,3,'#fff')}${R(0,3,9,3,'#D7141A')}<polygon points="0,0 4.5,3 0,6" fill="#11457E"/></svg>`,
 SK:`<svg viewBox="0 0 9 6">${R(0,0,9,2,'#fff')}${R(0,2,9,2,'#0B4EA2')}${R(0,4,9,2,'#EE1C25')}${R(1.6,1.2,.6,3,'#fff')}${R(1,2.1,1.8,.6,'#fff')}</svg>`,
 SI:`<svg viewBox="0 0 9 6">${R(0,0,9,2,'#fff')}${R(0,2,9,2,'#005DA4')}${R(0,4,9,2,'#ED1C24')}${R(1,0.7,1.7,1.8,'#005DA4')}${R(1.2,0.9,1.3,1.4,'#fff')}</svg>`,
 GB:`<svg viewBox="0 0 12 8">${R(0,0,12,8,'#012169')}<path d="M0,0 L12,8 M12,0 L0,8" stroke="#fff" stroke-width="2"/><path d="M0,0 L12,8 M12,0 L0,8" stroke="#C8102E" stroke-width="0.9"/>${R(5,0,2,8,'#fff')}${R(0,3,12,2,'#fff')}${R(5.5,0,1,8,'#C8102E')}${R(0,3.5,12,1,'#C8102E')}</svg>`,
 US:usFlag(),
 CA:`<svg viewBox="0 0 12 6">${R(0,0,3,6,'#FF0000')}${R(9,0,3,6,'#FF0000')}<polygon points="6,1 6.35,2.1 7.5,1.9 6.75,2.9 7.1,3.3 6.25,3.2 6,4.6 5.75,3.2 4.9,3.3 5.25,2.9 4.5,1.9 5.65,2.1" fill="#FF0000"/></svg>`
};

/* ---------- Oberflaechen-Uebersetzungen ---------- */
const T={
 en:{theme:"Theme",logout:"Logout",editTitle:"Create / edit script",name:"Name",script:"Ducky Script",save:"Save",clear:"New / clear",imp:"Import file",savedTitle:"Saved scripts",loading:"Loading…",empty:"No scripts yet.",onPlug:"on plug-in",start:"Start",edit:"Edit",settingsTitle:"Settings",expand:"Expand",loginUser:"Login user",loginPass:"Login password (blank = unchanged)",wifiMode:"Wi-Fi mode",modeAP:"Own hotspot (AP)",modeSTA:"Join existing Wi-Fi (STA)",layout:"Keyboard layout",apName:"AP name (SSID)",apPass:"AP password (min. 8, blank = unchanged)",staSsid:"Wi-Fi SSID (STA)",staPass:"Wi-Fi password (STA, blank = unchanged)",btnScript:"Script for BOOT button",statusLed:"Status LED",on:"On",off:"Off",saveSettings:"Save settings",advUsb:"Advanced: USB identity",manufacturer:"Manufacturer",product:"Product",note:"Wi-Fi and USB changes apply after a restart.",saved:"Saved",nameMissing:"Name missing",imported:"Imported – now save",deleted:"Deleted",running:"Running…",done:"Done",confirmDel:"Delete \"{n}\"?",hostname:"Device name (mDNS)",typeSpeed:"Typing speed",spFast:"Fast",spNormal:"Normal",spHuman:"Human-like",jiggler:"Mouse jiggler (keep awake)",jigSec:"Interval (s)",backupDl:"Download backup",backupUp:"Restore backup",ota:"Firmware update (OTA)",console:"Console",stop:"Stop",test:"Test",idle:"Idle",busy:"Already running"},
 de:{theme:"Theme",logout:"Logout",editTitle:"Skript erstellen / bearbeiten",name:"Name",script:"Ducky Script",save:"Speichern",clear:"Neu / Leeren",imp:"Datei importieren",savedTitle:"Gespeicherte Skripte",loading:"Lade…",empty:"Noch keine Skripte.",onPlug:"beim Einstecken",start:"Start",edit:"Bearb.",settingsTitle:"Einstellungen",expand:"Ausklappen",loginUser:"Login-Benutzer",loginPass:"Login-Passwort (leer = unverändert)",wifiMode:"WLAN-Modus",modeAP:"Eigener Hotspot (AP)",modeSTA:"An vorhandenes WLAN (STA)",layout:"Tastatur-Layout",apName:"AP-Name (SSID)",apPass:"AP-Passwort (min. 8, leer = unverändert)",staSsid:"WLAN-SSID (STA)",staPass:"WLAN-Passwort (STA, leer = unverändert)",btnScript:"Skript für BOOT-Knopf",statusLed:"Status-LED",on:"An",off:"Aus",saveSettings:"Einstellungen speichern",advUsb:"Erweitert: USB-Kennung",manufacturer:"Hersteller",product:"Produkt",note:"WLAN- und USB-Änderungen gelten nach Neustart.",saved:"Gespeichert",nameMissing:"Name fehlt",imported:"Importiert – jetzt speichern",deleted:"Gelöscht",running:"Führe aus…",done:"Fertig",confirmDel:"„{n}“ löschen?",hostname:"Gerätename (mDNS)",typeSpeed:"Tippgeschwindigkeit",spFast:"Schnell",spNormal:"Normal",spHuman:"Menschlich",jiggler:"Maus-Jiggler (wach halten)",jigSec:"Intervall (s)",backupDl:"Backup herunterladen",backupUp:"Backup einspielen",ota:"Firmware-Update (OTA)",console:"Konsole",stop:"Stop",test:"Test",idle:"Bereit",busy:"Läuft bereits"},
 es:{theme:"Tema",logout:"Salir",editTitle:"Crear / editar script",name:"Nombre",script:"Ducky Script",save:"Guardar",clear:"Nuevo / limpiar",imp:"Importar archivo",savedTitle:"Scripts guardados",loading:"Cargando…",empty:"Aún no hay scripts.",onPlug:"al conectar",start:"Iniciar",edit:"Editar",settingsTitle:"Ajustes",expand:"Mostrar",loginUser:"Usuario de acceso",loginPass:"Contraseña (vacío = sin cambios)",wifiMode:"Modo Wi-Fi",modeAP:"Punto de acceso propio (AP)",modeSTA:"Unirse a Wi-Fi existente (STA)",layout:"Distribución del teclado",apName:"Nombre del AP (SSID)",apPass:"Contraseña del AP (mín. 8, vacío = sin cambios)",staSsid:"SSID Wi-Fi (STA)",staPass:"Contraseña Wi-Fi (STA, vacío = sin cambios)",btnScript:"Script para el botón BOOT",statusLed:"LED de estado",on:"Encendido",off:"Apagado",saveSettings:"Guardar ajustes",advUsb:"Avanzado: identidad USB",manufacturer:"Fabricante",product:"Producto",note:"Los cambios de Wi-Fi y USB se aplican tras reiniciar.",saved:"Guardado",nameMissing:"Falta el nombre",imported:"Importado – ahora guarda",deleted:"Eliminado",running:"Ejecutando…",done:"Listo",confirmDel:"¿Eliminar \"{n}\"?"},
 fr:{theme:"Thème",logout:"Déconnexion",editTitle:"Créer / modifier un script",name:"Nom",script:"Ducky Script",save:"Enregistrer",clear:"Nouveau / vider",imp:"Importer un fichier",savedTitle:"Scripts enregistrés",loading:"Chargement…",empty:"Aucun script pour l'instant.",onPlug:"au branchement",start:"Lancer",edit:"Éditer",settingsTitle:"Paramètres",expand:"Afficher",loginUser:"Utilisateur",loginPass:"Mot de passe (vide = inchangé)",wifiMode:"Mode Wi-Fi",modeAP:"Point d'accès (AP)",modeSTA:"Rejoindre un Wi-Fi (STA)",layout:"Disposition du clavier",apName:"Nom du point d'accès (SSID)",apPass:"Mot de passe AP (min. 8, vide = inchangé)",staSsid:"SSID Wi-Fi (STA)",staPass:"Mot de passe Wi-Fi (STA, vide = inchangé)",btnScript:"Script pour le bouton BOOT",statusLed:"LED d'état",on:"Activé",off:"Désactivé",saveSettings:"Enregistrer les paramètres",advUsb:"Avancé : identité USB",manufacturer:"Fabricant",product:"Produit",note:"Les changements Wi-Fi et USB prennent effet après redémarrage.",saved:"Enregistré",nameMissing:"Nom manquant",imported:"Importé – enregistrez",deleted:"Supprimé",running:"Exécution…",done:"Terminé",confirmDel:"Supprimer « {n} » ?"},
 it:{theme:"Tema",logout:"Esci",editTitle:"Crea / modifica script",name:"Nome",script:"Ducky Script",save:"Salva",clear:"Nuovo / svuota",imp:"Importa file",savedTitle:"Script salvati",loading:"Caricamento…",empty:"Ancora nessuno script.",onPlug:"all'inserimento",start:"Avvia",edit:"Modifica",settingsTitle:"Impostazioni",expand:"Espandi",loginUser:"Utente di accesso",loginPass:"Password (vuoto = invariato)",wifiMode:"Modalità Wi-Fi",modeAP:"Hotspot proprio (AP)",modeSTA:"Connetti a Wi-Fi esistente (STA)",layout:"Layout tastiera",apName:"Nome AP (SSID)",apPass:"Password AP (min. 8, vuoto = invariato)",staSsid:"SSID Wi-Fi (STA)",staPass:"Password Wi-Fi (STA, vuoto = invariato)",btnScript:"Script per il tasto BOOT",statusLed:"LED di stato",on:"Acceso",off:"Spento",saveSettings:"Salva impostazioni",advUsb:"Avanzate: identità USB",manufacturer:"Produttore",product:"Prodotto",note:"Le modifiche Wi-Fi e USB valgono dopo il riavvio.",saved:"Salvato",nameMissing:"Nome mancante",imported:"Importato – ora salva",deleted:"Eliminato",running:"Esecuzione…",done:"Fatto",confirmDel:"Eliminare \"{n}\"?"},
 pt:{theme:"Tema",logout:"Sair",editTitle:"Criar / editar script",name:"Nome",script:"Ducky Script",save:"Guardar",clear:"Novo / limpar",imp:"Importar ficheiro",savedTitle:"Scripts guardados",loading:"A carregar…",empty:"Ainda sem scripts.",onPlug:"ao ligar",start:"Iniciar",edit:"Editar",settingsTitle:"Definições",expand:"Mostrar",loginUser:"Utilizador",loginPass:"Palavra-passe (vazio = inalterado)",wifiMode:"Modo Wi-Fi",modeAP:"Hotspot próprio (AP)",modeSTA:"Ligar a Wi-Fi existente (STA)",layout:"Esquema do teclado",apName:"Nome do AP (SSID)",apPass:"Palavra-passe do AP (mín. 8, vazio = inalterado)",staSsid:"SSID Wi-Fi (STA)",staPass:"Palavra-passe Wi-Fi (STA, vazio = inalterado)",btnScript:"Script para o botão BOOT",statusLed:"LED de estado",on:"Ligado",off:"Desligado",saveSettings:"Guardar definições",advUsb:"Avançado: identidade USB",manufacturer:"Fabricante",product:"Produto",note:"As alterações de Wi-Fi e USB aplicam-se após reiniciar.",saved:"Guardado",nameMissing:"Falta o nome",imported:"Importado – agora guarde",deleted:"Eliminado",running:"A executar…",done:"Concluído",confirmDel:"Eliminar \"{n}\"?"},
 ru:{theme:"Тема",logout:"Выход",editTitle:"Создать / изменить скрипт",name:"Имя",script:"Ducky Script",save:"Сохранить",clear:"Новый / очистить",imp:"Импорт файла",savedTitle:"Сохранённые скрипты",loading:"Загрузка…",empty:"Пока нет скриптов.",onPlug:"при подключении",start:"Запуск",edit:"Правка",settingsTitle:"Настройки",expand:"Развернуть",loginUser:"Пользователь",loginPass:"Пароль (пусто = без изменений)",wifiMode:"Режим Wi-Fi",modeAP:"Своя точка доступа (AP)",modeSTA:"Подключиться к Wi-Fi (STA)",layout:"Раскладка клавиатуры",apName:"Имя точки доступа (SSID)",apPass:"Пароль точки (мин. 8, пусто = без изм.)",staSsid:"SSID Wi-Fi (STA)",staPass:"Пароль Wi-Fi (STA, пусто = без изм.)",btnScript:"Скрипт для кнопки BOOT",statusLed:"Индикатор",on:"Вкл",off:"Выкл",saveSettings:"Сохранить настройки",advUsb:"Дополнительно: USB-идентификатор",manufacturer:"Производитель",product:"Продукт",note:"Изменения Wi-Fi и USB вступают в силу после перезагрузки.",saved:"Сохранено",nameMissing:"Нет имени",imported:"Импортировано – сохраните",deleted:"Удалено",running:"Выполнение…",done:"Готово",confirmDel:"Удалить \"{n}\"?"},
 da:{theme:"Tema",logout:"Log ud",editTitle:"Opret / rediger script",name:"Navn",script:"Ducky Script",save:"Gem",clear:"Ny / ryd",imp:"Importér fil",savedTitle:"Gemte scripts",loading:"Indlæser…",empty:"Ingen scripts endnu.",onPlug:"ved tilslutning",start:"Start",edit:"Rediger",settingsTitle:"Indstillinger",expand:"Vis",loginUser:"Loginbruger",loginPass:"Adgangskode (tom = uændret)",wifiMode:"Wi-Fi-tilstand",modeAP:"Eget hotspot (AP)",modeSTA:"Tilslut eksisterende Wi-Fi (STA)",layout:"Tastaturlayout",apName:"AP-navn (SSID)",apPass:"AP-adgangskode (min. 8, tom = uændret)",staSsid:"Wi-Fi-SSID (STA)",staPass:"Wi-Fi-adgangskode (STA, tom = uændret)",btnScript:"Script til BOOT-knap",statusLed:"Status-LED",on:"Til",off:"Fra",saveSettings:"Gem indstillinger",advUsb:"Avanceret: USB-identitet",manufacturer:"Producent",product:"Produkt",note:"Wi-Fi- og USB-ændringer gælder efter genstart.",saved:"Gemt",nameMissing:"Navn mangler",imported:"Importeret – gem nu",deleted:"Slettet",running:"Kører…",done:"Færdig",confirmDel:"Slet \"{n}\"?"},
 sk:{theme:"Téma",logout:"Odhlásiť",editTitle:"Vytvoriť / upraviť skript",name:"Názov",script:"Ducky Script",save:"Uložiť",clear:"Nový / vymazať",imp:"Importovať súbor",savedTitle:"Uložené skripty",loading:"Načítava sa…",empty:"Zatiaľ žiadne skripty.",onPlug:"pri zapojení",start:"Spustiť",edit:"Upraviť",settingsTitle:"Nastavenia",expand:"Zobraziť",loginUser:"Používateľ",loginPass:"Heslo (prázdne = bez zmeny)",wifiMode:"Režim Wi-Fi",modeAP:"Vlastný hotspot (AP)",modeSTA:"Pripojiť k Wi-Fi (STA)",layout:"Rozloženie klávesnice",apName:"Názov AP (SSID)",apPass:"Heslo AP (min. 8, prázdne = bez zmeny)",staSsid:"Wi-Fi SSID (STA)",staPass:"Heslo Wi-Fi (STA, prázdne = bez zmeny)",btnScript:"Skript pre tlačidlo BOOT",statusLed:"Stavová LED",on:"Zap.",off:"Vyp.",saveSettings:"Uložiť nastavenia",advUsb:"Pokročilé: USB identita",manufacturer:"Výrobca",product:"Produkt",note:"Zmeny Wi-Fi a USB sa prejavia po reštarte.",saved:"Uložené",nameMissing:"Chýba názov",imported:"Importované – teraz uložte",deleted:"Odstránené",running:"Spúšťa sa…",done:"Hotovo",confirmDel:"Odstrániť \"{n}\"?"},
 cs:{theme:"Motiv",logout:"Odhlásit",editTitle:"Vytvořit / upravit skript",name:"Název",script:"Ducky Script",save:"Uložit",clear:"Nový / vymazat",imp:"Importovat soubor",savedTitle:"Uložené skripty",loading:"Načítání…",empty:"Zatím žádné skripty.",onPlug:"při zapojení",start:"Spustit",edit:"Upravit",settingsTitle:"Nastavení",expand:"Zobrazit",loginUser:"Uživatel",loginPass:"Heslo (prázdné = beze změny)",wifiMode:"Režim Wi-Fi",modeAP:"Vlastní hotspot (AP)",modeSTA:"Připojit k Wi-Fi (STA)",layout:"Rozložení klávesnice",apName:"Název AP (SSID)",apPass:"Heslo AP (min. 8, prázdné = beze změny)",staSsid:"Wi-Fi SSID (STA)",staPass:"Heslo Wi-Fi (STA, prázdné = beze změny)",btnScript:"Skript pro tlačítko BOOT",statusLed:"Stavová LED",on:"Zap.",off:"Vyp.",saveSettings:"Uložit nastavení",advUsb:"Pokročilé: USB identita",manufacturer:"Výrobce",product:"Produkt",note:"Změny Wi-Fi a USB se projeví po restartu.",saved:"Uloženo",nameMissing:"Chybí název",imported:"Importováno – nyní uložte",deleted:"Smazáno",running:"Spouštění…",done:"Hotovo",confirmDel:"Smazat \"{n}\"?"},
 sl:{theme:"Tema",logout:"Odjava",editTitle:"Ustvari / uredi skript",name:"Ime",script:"Ducky Script",save:"Shrani",clear:"Novo / počisti",imp:"Uvozi datoteko",savedTitle:"Shranjeni skripti",loading:"Nalaganje…",empty:"Zaenkrat ni skriptov.",onPlug:"ob priklopu",start:"Zaženi",edit:"Uredi",settingsTitle:"Nastavitve",expand:"Prikaži",loginUser:"Uporabnik",loginPass:"Geslo (prazno = nespremenjeno)",wifiMode:"Način Wi-Fi",modeAP:"Lastna vroča točka (AP)",modeSTA:"Poveži z obstoječim Wi-Fi (STA)",layout:"Razporeditev tipkovnice",apName:"Ime AP (SSID)",apPass:"Geslo AP (najmanj 8, prazno = nespremenjeno)",staSsid:"Wi-Fi SSID (STA)",staPass:"Geslo Wi-Fi (STA, prazno = nespremenjeno)",btnScript:"Skript za gumb BOOT",statusLed:"Statusna LED",on:"Vklop",off:"Izklop",saveSettings:"Shrani nastavitve",advUsb:"Napredno: USB identiteta",manufacturer:"Proizvajalec",product:"Izdelek",note:"Spremembe Wi-Fi in USB začnejo veljati po ponovnem zagonu.",saved:"Shranjeno",nameMissing:"Manjka ime",imported:"Uvoženo – zdaj shranite",deleted:"Izbrisano",running:"Izvajanje…",done:"Končano",confirmDel:"Izbrišem \"{n}\"?"},
 bg:{theme:"Тема",logout:"Изход",editTitle:"Създаване / редакция на скрипт",name:"Име",script:"Ducky Script",save:"Запази",clear:"Нов / изчисти",imp:"Импортиране на файл",savedTitle:"Запазени скриптове",loading:"Зареждане…",empty:"Все още няма скриптове.",onPlug:"при включване",start:"Старт",edit:"Редакция",settingsTitle:"Настройки",expand:"Покажи",loginUser:"Потребител",loginPass:"Парола (празно = без промяна)",wifiMode:"Wi-Fi режим",modeAP:"Собствена точка (AP)",modeSTA:"Към съществуващ Wi-Fi (STA)",layout:"Клавиатурна подредба",apName:"Име на AP (SSID)",apPass:"Парола за AP (мин. 8, празно = без промяна)",staSsid:"Wi-Fi SSID (STA)",staPass:"Wi-Fi парола (STA, празно = без промяна)",btnScript:"Скрипт за бутон BOOT",statusLed:"Статус LED",on:"Вкл.",off:"Изкл.",saveSettings:"Запази настройките",advUsb:"Разширено: USB идентичност",manufacturer:"Производител",product:"Продукт",note:"Промените по Wi-Fi и USB важат след рестарт.",saved:"Запазено",nameMissing:"Липсва име",imported:"Импортирано – запазете",deleted:"Изтрито",running:"Изпълнение…",done:"Готово",confirmDel:"Изтриване на \"{n}\"?"},
 hu:{theme:"Téma",logout:"Kilépés",editTitle:"Szkript létrehozása / szerkesztése",name:"Név",script:"Ducky Script",save:"Mentés",clear:"Új / törlés",imp:"Fájl importálása",savedTitle:"Mentett szkriptek",loading:"Betöltés…",empty:"Még nincs szkript.",onPlug:"csatlakoztatáskor",start:"Indítás",edit:"Szerk.",settingsTitle:"Beállítások",expand:"Megnyitás",loginUser:"Belépési név",loginPass:"Jelszó (üres = változatlan)",wifiMode:"Wi-Fi mód",modeAP:"Saját hotspot (AP)",modeSTA:"Csatlakozás meglévő Wi-Fi-hez (STA)",layout:"Billentyűzetkiosztás",apName:"AP neve (SSID)",apPass:"AP jelszó (min. 8, üres = változatlan)",staSsid:"Wi-Fi SSID (STA)",staPass:"Wi-Fi jelszó (STA, üres = változatlan)",btnScript:"Szkript a BOOT gombhoz",statusLed:"Állapot-LED",on:"Be",off:"Ki",saveSettings:"Beállítások mentése",advUsb:"Haladó: USB-azonosító",manufacturer:"Gyártó",product:"Termék",note:"A Wi-Fi és USB módosítások újraindítás után lépnek életbe.",saved:"Mentve",nameMissing:"Hiányzó név",imported:"Importálva – most mentsen",deleted:"Törölve",running:"Futtatás…",done:"Kész",confirmDel:"Törli: \"{n}\"?"}
};

const UILANGS=[
 {val:'en',flag:FLAG.GB,label:'English'},{val:'de',flag:FLAG.DE,label:'Deutsch'},
 {val:'es',flag:FLAG.ES,label:'Español'},{val:'fr',flag:FLAG.FR,label:'Français'},
 {val:'it',flag:FLAG.IT,label:'Italiano'},{val:'pt',flag:FLAG.PT,label:'Português'},
 {val:'ru',flag:FLAG.RU,label:'Русский'},{val:'da',flag:FLAG.DK,label:'Dansk'},
 {val:'sk',flag:FLAG.SK,label:'Slovenčina'},{val:'cs',flag:FLAG.CZ,label:'Čeština'},
 {val:'sl',flag:FLAG.SI,label:'Slovenščina'},{val:'bg',flag:FLAG.BG,label:'Български'},
 {val:'hu',flag:FLAG.HU,label:'Magyar'}
];
const LAYOUTS=[
 {val:'DE',flag:FLAG.DE,label:'Deutsch'},{val:'US',flag:FLAG.US,label:'English (US)'},
 {val:'GB',flag:FLAG.GB,label:'English (UK)'},{val:'ES',flag:FLAG.ES,label:'Español'},
 {val:'DK',flag:FLAG.DK,label:'Dansk'},{val:'RU',flag:FLAG.RU,label:'Русский'},
 {val:'FR',flag:FLAG.FR,label:'Français'},{val:'BE',flag:FLAG.BE,label:'Belgisch'},
 {val:'PT',flag:FLAG.PT,label:'Português'},{val:'IT',flag:FLAG.IT,label:'Italiano'},
 {val:'SK',flag:FLAG.SK,label:'Slovenčina'},{val:'CZ',flag:FLAG.CZ,label:'Čeština'},
 {val:'SI',flag:FLAG.SI,label:'Slovenščina'},{val:'BG',flag:FLAG.BG,label:'Български'},
 {val:'CAFR',flag:FLAG.CA,label:'Canada (FR)'},{val:'CHDE',flag:FLAG.CH,label:'Schweiz (DE)'},
 {val:'CHFR',flag:FLAG.CH,label:'Suisse (FR)'},{val:'HU',flag:FLAG.HU,label:'Magyar'}
];

/* ---------- Flaggen-Dropdown-Komponente ---------- */
function dropdown(host,items,val,onChange,full){
  if(full)host.classList.add('dd-full');
  host.classList.add('dd');
  host.innerHTML=`<button type="button" class="dd-btn"><span class="flag"></span><span class="dd-lbl"></span><span class="dd-car">▾</span></button><div class="dd-menu"></div>`;
  const btn=host.querySelector('.dd-btn'),menu=host.querySelector('.dd-menu');
  const bf=host.querySelector('.flag'),bl=host.querySelector('.dd-lbl');
  function render(){
    const it=items.find(x=>x.val===val)||items[0];
    bf.innerHTML=it.flag;bl.textContent=it.label;
    menu.innerHTML=items.map(x=>`<div class="dd-opt" data-v="${x.val}"><span class="flag">${x.flag}</span>${x.label}</div>`).join('');
  }
  btn.onclick=e=>{e.stopPropagation();document.querySelectorAll('.dd.open').forEach(d=>{if(d!==host)d.classList.remove('open');});host.classList.toggle('open');};
  menu.onclick=e=>{const o=e.target.closest('.dd-opt');if(!o)return;val=o.dataset.v;host.classList.remove('open');render();if(onChange)onChange(val);};
  render();
  return {set:v=>{val=v;render();},get:()=>val};
}
document.addEventListener('click',()=>document.querySelectorAll('.dd.open').forEach(d=>d.classList.remove('open')));

/* ---------- Sprache / Theme ---------- */
let lang=(function(){try{return localStorage.getItem('oni_lang')}catch(e){return null}})()||'de';
const tr=k=>((T[lang]||T.en)[k]!=null?(T[lang]||T.en)[k]:k);
function applyLang(code){
  lang=code;try{localStorage.setItem('oni_lang',code)}catch(e){}
  document.documentElement.lang=code;
  const t=T[code]||T.en;
  document.querySelectorAll('[data-i18n]').forEach(el=>{const k=el.dataset.i18n;const v=t[k]!=null?t[k]:T.en[k];if(v!=null)el.textContent=v;});
  renderList();
}
function applyTheme(t){document.documentElement.dataset.theme=t;try{localStorage.setItem('oni_theme',t)}catch(e){}}
function toggleTheme(){applyTheme(document.documentElement.dataset.theme==='dark'?'light':'dark');}
function toast(m){const t=$('#toast');t.textContent=m;t.classList.add('show');setTimeout(()=>t.classList.remove('show'),2200);}

const langApi=dropdown($('#langDD'),UILANGS,lang,applyLang);
const layoutApi=dropdown($('#layoutDD'),LAYOUTS,'DE',null,true);

/* ---------- Backend ---------- */
async function api(u,o){const r=await fetch(u,o);if(r.status===401){location.href='/';throw 0;}return r;}
async function loadSettings(){
  const s=await (await api('/api/settings')).json();
  $('#cUser').value=s.user;$('#cSsid').value=s.ssid;layoutApi.set(s.layout||'DE');
  $('#cWifiMode').value=s.wifiMode||'AP';$('#cStaSsid').value=s.staSsid||'';
  $('#cBtn').value=s.btn||'';$('#cLed').value=s.led?'1':'0';
  $('#cHost').value=s.host||'oni-duck';$('#cSpeed').value=s.tspeed||'normal';
  $('#cJig').value=s.jiggler?'1':'0';$('#cJigSec').value=s.jigsec||30;
  $('#uManu').value=s.usbManu||'';$('#uProd').value=s.usbProd||'';$('#uVid').value=s.usbVid||'';$('#uPid').value=s.usbPid||'';
  $('#ipInfo').textContent=(s.version?('ONI-Duck v'+s.version):'')+(s.ip?('  ·  '+s.ip):'');
  applyTheme((function(){try{return localStorage.getItem('oni_theme')}catch(e){return null}})()||s.theme||'dark');
}
async function saveSettings(){
  const b=new URLSearchParams();
  b.set('user',$('#cUser').value);b.set('ssid',$('#cSsid').value);b.set('layout',layoutApi.get());
  b.set('wifiMode',$('#cWifiMode').value);b.set('staSsid',$('#cStaSsid').value);
  b.set('btn',$('#cBtn').value);b.set('led',$('#cLed').value);
  b.set('host',$('#cHost').value);b.set('tspeed',$('#cSpeed').value);
  b.set('jiggler',$('#cJig').value);b.set('jigsec',$('#cJigSec').value);
  b.set('usbManu',$('#uManu').value);b.set('usbProd',$('#uProd').value);b.set('usbVid',$('#uVid').value);b.set('usbPid',$('#uPid').value);
  b.set('theme',document.documentElement.dataset.theme);
  if($('#cPass').value)b.set('pass',$('#cPass').value);
  if($('#cAppass').value)b.set('appass',$('#cAppass').value);
  if($('#cStaPass').value)b.set('staPass',$('#cStaPass').value);
  await api('/api/settings',{method:'POST',body:b});
  $('#cPass').value='';$('#cAppass').value='';$('#cStaPass').value='';toast(tr('saved'));
}

let curScripts=[];
function renderList(){
  const el=$('#list');
  if(!curScripts.length){el.innerHTML=`<div class="hint">${tr('empty')}</div>`;return;}
  el.innerHTML='';
  for(const s of curScripts){
    const row=document.createElement('div');row.className='item';
    row.innerHTML=`<div class="name"></div><div class="ctl">
      <label class="chk"><input type="checkbox" ${s.autorun?'checked':''}> <span></span></label>
      <button class="btn sm red b-run"></button><button class="btn sm b-test"></button>
      <button class="btn sm b-edit"></button><button class="btn sm b-dl">⬇</button>
      <button class="btn sm b-del">✕</button></div>`;
    row.querySelector('.name').textContent=s.name;
    const chk=row.querySelector('input');
    row.querySelector('.chk span').textContent=tr('onPlug');
    const run=row.querySelector('.b-run'),test=row.querySelector('.b-test'),edit=row.querySelector('.b-edit');
    run.textContent='▶ '+tr('start');test.textContent=tr('test');edit.textContent=tr('edit');
    chk.onchange=()=>setAutorun(s.name,chk.checked);
    run.onclick=()=>startRun(s.name,false);
    test.onclick=()=>startRun(s.name,true);
    edit.onclick=()=>editScript(s.name);
    row.querySelector('.b-dl').onclick=()=>downloadScript(s.name);
    row.querySelector('.b-del').onclick=()=>delScript(s.name);
    el.appendChild(row);
  }
}
async function loadList(){curScripts=await (await api('/api/scripts')).json();renderList();}
async function save(){
  const name=$('#sName').value.trim();if(!name){toast(tr('nameMissing'));return;}
  const b=new URLSearchParams();b.set('name',name);b.set('content',$('#sBody').value);
  const r=await api('/api/save',{method:'POST',body:b});
  if(r.ok){toast(tr('saved'));loadList();}else{toast('Fehler: '+(await r.text()));}
}
function clearEditor(){$('#sName').value='';$('#sBody').value='';}
async function editScript(name){
  const t=await (await api('/api/script?name='+encodeURIComponent(name))).text();
  $('#sName').value=name;$('#sBody').value=t;window.scrollTo({top:0,behavior:'smooth'});
}
async function downloadScript(name){
  const t=await (await api('/api/script?name='+encodeURIComponent(name))).text();
  const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([t],{type:'text/plain'}));
  a.download=name+'.txt';a.click();URL.revokeObjectURL(a.href);
}
function importFile(e){
  const f=e.target.files[0];if(!f)return;const r=new FileReader();
  r.onload=()=>{$('#sBody').value=r.result;if(!$('#sName').value)$('#sName').value=f.name.replace(/\.[^.]+$/,'').slice(0,32);toast(tr('imported'));};
  r.readAsText(f);e.target.value='';
}
async function delScript(name){
  if(!confirm(tr('confirmDel').replace('{n}',name)))return;
  const b=new URLSearchParams();b.set('name',name);
  await api('/api/delete',{method:'POST',body:b});toast(tr('deleted'));loadList();
}
async function setAutorun(name,on){
  const b=new URLSearchParams();b.set('name',name);b.set('enabled',on?'1':'0');
  await api('/api/autorun',{method:'POST',body:b});
}
let polling=null;
async function startRun(name,dry){
  const b=new URLSearchParams();b.set('name',name);b.set('dry',dry?'1':'0');
  const t=await (await api('/api/run',{method:'POST',body:b})).text();
  if(t==='busy'){toast(tr('busy'));return;}
  $('#console').textContent='';beginPolling();
}
function beginPolling(){if(polling)return;pollProgress();polling=setInterval(pollProgress,600);}
async function pollProgress(){
  let p;try{p=await (await api('/api/progress')).json();}catch(e){return;}
  const con=$('#console');con.textContent=p.log||'';con.scrollTop=con.scrollHeight;
  $('#stopBtn').style.display=p.running?'':'none';
  $('#runState').textContent=p.running?(tr('running')+' '+p.line+'/'+p.total+(p.dry?' · Dry-Run':'')):tr('idle');
  if(!p.running&&polling){clearInterval(polling);polling=null;}
}
async function stopRun(){await api('/api/stop',{method:'POST'});}
async function downloadBackup(){
  const t=await (await api('/api/backup')).text();
  const a=document.createElement('a');a.href=URL.createObjectURL(new Blob([t],{type:'text/plain'}));
  a.download='oni-backup.txt';a.click();URL.revokeObjectURL(a.href);
}
function restoreBackup(e){
  const f=e.target.files[0];if(!f)return;const r=new FileReader();
  r.onload=async()=>{await api('/api/restore',{method:'POST',headers:{'Content-Type':'text/plain'},body:r.result});toast(tr('saved'));loadSettings();loadList();};
  r.readAsText(f);e.target.value='';
}

applyTheme((function(){try{return localStorage.getItem('oni_theme')}catch(e){return null}})()||'dark');
applyLang(lang);
loadSettings();loadList();pollProgress();
</script></body></html>)HTML";
