# Installation (Kubuntu 26.04 LTS, Wayland)

Only for **Kubuntu 26.04 LTS with Plasma 6.6.6** (check with `apt-cache policy libplasma7`).
No support; see the warning in [README.md](README.md). Make a **Timeshift snapshot** first.

Commands starting with `sudo` ask for your password. Run them one at a time.

---

## Part 1: the theme

### 1. Bring the system up to date

```bash
sudo apt update && sudo apt full-upgrade
```

Reboot afterwards.

### 2. Build packages and Kvantum

The packages needed to build all parts (worked out from the theme's own build files):

```bash
sudo apt install build-essential cmake ninja-build pkgconf extra-cmake-modules qt6-base-dev qt6-declarative-dev qt6-svg-dev qt6-tools-dev qt6-5compat-dev qt6-wayland-dev libkf6archive-dev libkf6attica-dev libkf6auth-dev libkf6codecs-dev libkf6colorscheme-dev libkf6config-dev libkf6configwidgets-dev libkf6coreaddons-dev libkf6crash-dev libkf6dbusaddons-dev libkf6globalaccel-dev libkf6guiaddons-dev libkf6i18n-dev libkf6iconthemes-dev libkf6itemmodels-dev libkf6kcmutils-dev libkf6kio-dev libkf6newstuff-dev libkf6notifications-dev libkf6notifyconfig-dev libkf6package-dev libkf6qqc2desktopstyle-dev libkf6runner-dev libkf6service-dev libkf6sonnet-dev libkf6statusnotifieritem-dev libkf6style-dev libkf6svg-dev libkf6widgetsaddons-dev libkf6windowsystem-dev libkf6xmlgui-dev libkirigami-dev libplasma-dev libplasma5support-dev libplasmaactivities-dev libplasmaactivitiesstats-dev plasma-wayland-protocols plasma-workspace-dev kwin-dev libkdecorations3-dev libksysguard-dev libpolkit-qt6-1-dev libwayland-dev wayland-protocols libepoxy-dev libdrm-dev qml6-module-org-kde-kirigamiaddons-sounds
```

**Kvantum must be installed before the theme.** The theme sets the application style to
`kvantum`; if Kvantum is missing at that moment, Plasma quietly saves Breeze instead and windows
such as the password (UAC) dialog look wrong:

```bash
sudo apt install qt6-style-kvantum qt5-style-kvantum
```

### 3. Download this repository

```bash
cd ~
git clone https://github.com/Basic-Capsuleer/aerothemeplasma-kubuntu-lts-26.04.git
```

### 4. Run the install script

It builds and installs all parts from the `repos/` folder (nothing is downloaded). It takes a
while and asks for your password itself (no `sudo` in front):

```bash
cd ~/aerothemeplasma-kubuntu-lts-26.04
LIBEXEC_DIR=lib/x86_64-linux-gnu/libexec UAC_LIBEXEC_DIR=lib/x86_64-linux-gnu/libexec bash install.sh --skip-x11
```

Wait until it prints `Done.`

### 5. Protect the Aero libplasma

Ubuntu updates (including automatic security updates and Discover) would otherwise replace the
theme's libplasma with Ubuntu's own. The hold script puts it (and Aero Dolphin's packages, part 2)
on hold:

```bash
sh ~/aerothemeplasma-kubuntu-lts-26.04/tools/atp-hold.sh
```

Then install the login check: completely silent when all is well; it only shows a popup when the
Aero libplasma was replaced, Ubuntu offers a newer one, or the hold is missing. Its "Show the
steps" button explains what to do. No sudo needed:

```bash
SRC=~/aerothemeplasma-kubuntu-lts-26.04/tools/atp-libplasma-check
D=~/.local/share/atp-libplasma-check
mkdir -p "$D" ~/.config/autostart
install -m 755 "$SRC/atp-libplasma-check" "$D/"
install -m 644 "$SRC/update-steps.txt" "$D/"
sed "s|@HOME@|$HOME|" "$SRC/atp-libplasma-check.desktop" > ~/.config/autostart/atp-libplasma-check.desktop
"$D/atp-libplasma-check" --record
```

`--record` remembers the installed libplasma as "the Aero version"; only run it right after
(re)building libplasma. `"$D/atp-libplasma-check" --status` shows the results in the terminal.

### 6. Log in to the AeroThemePlasma session and run the welcome wizard

The install script adds its own login session. The theme only works fully in that session: it
loads the theme's own settings, desktop shell and UAC password window.

1. **Log out.** On the login screen, open the **session menu** (bottom left) and choose
   **AeroThemePlasma (Wayland)** instead of "Plasma (Wayland)". Then log in. The login screen
   remembers this choice.
2. A few seconds after logging in, the **AeroThemePlasma first time setup wizard** opens
   ("AeroThemePlasma OOTB Experience"). Follow it to the end and click **Finish**. It:
   - applies the global theme (`authui7`), the Kvantum theme `Windows7Aero` and the `aero-drop`
     cursor (size 32);
   - switches on the Aero desktop effects and switches off KDE's own versions of them;
   - sets the fonts to Segoe UI 9 (small 8) (see the font note in step 7);
   - themes the **login screen** (SDDM) and copies your settings to it. It asks for your admin
     password for that.

   The wizard starts at every login until it has been finished once. To run it again later:
   `atpootb` in a terminal (after setting `wizardRun=false` under `[OOTB]` in
   `~/.config/aerothemeplasmarc`).
3. Check System Settings → Colours & Themes → Application Style is **kvantum** (if Kvantum was
   installed before step 4 it is).
4. Switch on the two KWin scripts the show desktop button needs. The wizard tries to, but it uses
   the effects list for them, so these two scripts stay off:
   ```bash
   kwriteconfig6 --file kwinrc --group Plugins --key minimizeallEnabled true
   kwriteconfig6 --file kwinrc --group Plugins --key smodpeekscriptEnabled true
   qdbus6 org.kde.KWin /KWin reconfigure
   ```
5. **Log out and back in** (into the AeroThemePlasma (Wayland) session).

**After Ubuntu updates KWin or Plasma** (for example a security update), the theme's KWin effects
can stop working until they are rebuilt: run step 4 again. libplasma itself is protected by the
hold; the login check tells you when something needs attention.

### 7. Bigger but sharp (optional; this is what the fixes are made for)

Instead of 125% display scaling (which makes the theme's artwork blurry on Wayland):
- Display scaling **100%** on all screens (System Settings → Display Configuration).
- System Settings → Text & Fonts → Fonts → *Adjust All Fonts…* → **11** or **12**; *Small* → **10**.
  **Font note:** the theme uses **Segoe UI**, a Microsoft font that isn't included (or in Ubuntu).
  Without it, the system quietly uses **Noto Sans** instead; my screenshots show Noto Sans. To get
  Segoe UI you need it from your own Windows 7 installation disc (ISO); the original project has a
  script for Arch Linux (see INSTALL.upstream.md, "Fonts").
- Taskbar height **50** px (right-click the taskbar → Show Panel Configuration → Panel Height).

The taskbar, tray, start menu, menus, previews, the clock's calendar and the UAC window then grow
with the font. **Exception: the clock on the taskbar** has its own font setting and doesn't follow
the system font; set it by hand (right-click the clock → Configure → its font/size).

### 8. The exact look of my setup (optional)

What makes the look in my screenshots, on top of the steps above:

| What | Setting |
|---|---|
| Screen | 2560×1440 @ 120 Hz at **100%** scaling |
| Fonts | System Settings → Text & Fonts → Fonts: **Segoe UI 12** (general, menu, toolbar, window title), small font **Segoe UI 11**; Segoe UI isn't installed, so it shows as Noto Sans (see step 7) |
| Taskbar | Height **50** px, not floating, on every screen (bottom) |
| Taskbar clock | Its own font size **12** (right-click the clock → Configure → Appearance); it does not follow the system font |
| Start orb | Custom image, see below |
| Global theme | AeroThemePlasma (`authui7`); colours **Aero**, Plasma style **Seven-Black**, icons **Windows 7 Aero**, cursor **aero-drop** (size 32), splash **authui7** |
| Application style | **kvantum** with Kvantum theme **Windows7Aero** |
| Window decoration | **SMOD** |
| Aero glass colour | Desktop Effects → Aero Glass Blur → colour **group 7, colour 0** (hue 214, saturation 100, brightness 66, intensity 168) |
| Desktop effects on | Aero Glass Blur, Aero Glide, SMOD Glow, SMOD Snap, SMOD Peek (effect and script), Dim Screen (Aero), MinimizeAll |
| Wallpaper | The default Windows 7 wallpaper (Windows logo on blue), a 3840×2160 version; not included |

**Start orb.** The orb is `WindowsStartButtonLarge (Thin taskbar at bottom).png` from
`Windows7 different sizes.zip` in this Classic Shell / Open-Shell forum topic:
https://classicshell.net/forum/viewtopic.php?t=3207
(not included here; it's someone else's work). Unzip it somewhere permanent (e.g. `~/Pictures`),
then right-click the start orb → Configure → custom button image → choose that file. It is
81×81 px per state, so it sticks out above a 50 px taskbar like in Windows 7 (this fork's orb
fixes keep it in place).

**In Dolphin:** toolbar icon size 22 (right-click the toolbar → Icon Size).

---

## Part 2: Aero Dolphin (optional)

The file manager in the Windows 7 Explorer look. It **replaces Ubuntu's Dolphin**.

Note: it is KDE's Dolphin development version of 23 May 2026 (later Dolphin 26.08), labelled
25.12.3, plus the Aero changes and four fixes from this fork. It works with Ubuntu 26.04's KDE
Frameworks 6.24.

### 1. Extra build packages

```bash
sudo apt install libkf6parts-dev libkf6textwidgets-dev libkf6baloo-dev libbaloowidgets-dev libkf6filemetadata-dev qt6-multimedia-dev
```

### 2. Build and install libAeroQt (first)

It also adds an "Aero" page to System Settings and an extra application style; neither changes
anything by itself.

```bash
cd ~/aerothemeplasma-kubuntu-lts-26.04/apps/aero-dolphin/libaero-qt
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build .
cmake --build build -j$(nproc)
sudo cmake --install build
```

### 3. Build and install Aero Dolphin

```bash
cd ~/aerothemeplasma-kubuntu-lts-26.04/apps/aero-dolphin/dolphin-aero
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build .
cmake --build build -j$(nproc)
sudo cmake --install build
```

Then run the hold script again (it also holds `dolphin`, `dolphin-data`, `dolphin-doc` and
`libdolphinvcs6`, so Ubuntu updates don't overwrite it):

```bash
sh ~/aerothemeplasma-kubuntu-lts-26.04/tools/atp-hold.sh
```

To go back to Ubuntu's Dolphin:
`sudo apt-mark unhold dolphin dolphin-data dolphin-doc libdolphinvcs6` and
`sudo apt install --reinstall dolphin dolphin-data dolphin-doc libdolphinvcs6`.

### 4. Settings

- **One seamless glass header** (no line under the title bar): System Settings → Colours & Themes
  → Window Decorations → SMOD → edit → window-specific overrides → add: type **window class**,
  pattern **`dolphin`**, tick **Hide inner border** → Apply.
- **Bigger toolbar icons:** right-click an empty spot in Dolphin's toolbar → **Icon Size** → **22**.
  (The same works in Gwenview and other KDE programs.)

---

## Part 3: LibreWolf in the Windows 7 / old Chrome look (optional)

Uses two add-ons recommended by AeroThemePlasma: [Geckium](https://github.com/Neo-Kitchenfox/Geckium)
(old Chrome look) and [Aero UserChrome](https://gitgud.io/souris/aero-userchrome) (Windows 7 glass
frame). The files from this fork are in `extras/librewolf/`.

**Why not Geckium's own instructions:** they replace the browser's startup config file, but
LibreWolf uses that same mechanism for **all its privacy settings**. Either Geckium never starts,
or LibreWolf silently loses its privacy settings. Here Geckium's loader goes into LibreWolf's own
user file instead, and only the startup sandbox is switched off.

> [!CAUTION]
> With the sandbox off, Geckium's scripts run with full browser rights. Only use Geckium from its
> official page.

### 1. Install LibreWolf

```bash
sudo apt update && sudo apt install extrepo -y
sudo extrepo enable librewolf && sudo extrepo update librewolf
sudo apt update && sudo apt install librewolf -y
```

Start LibreWolf once and close it again (it creates its profile folder).

### 2. Download Geckium and Aero UserChrome

Tested with Geckium 1.19.7 and Aero UserChrome 2025.04.12:

```bash
mkdir -p ~/Downloads/atp-librewolf/geckium ~/Downloads/atp-librewolf/aero-userchrome
cd ~/Downloads/atp-librewolf
curl -L -o geckium/Geckium_Public_Beta_1.19.7.zip https://github.com/Neo-Kitchenfox/Geckium/releases/download/b0.20.21.2/Geckium_Public_Beta_1.19.7.zip
curl -L -o aero-userchrome/Release-2025.04.12.zip https://gitgud.io/souris/aero-userchrome/-/package_files/17324/download
unzip -q geckium/Geckium_Public_Beta_1.19.7.zip -d geckium/x
unzip -q aero-userchrome/Release-2025.04.12.zip -d aero-userchrome/x
```

### 3. Copy everything into the profile

Find the profile folder in LibreWolf: `about:support` → *Profile Folder*. Close LibreWolf, then
(replace `XXXXXXXX` with your profile's name):

```bash
P=~/.config/librewolf/librewolf/XXXXXXXX.default-release
E=~/aerothemeplasma-kubuntu-lts-26.04/extras/librewolf
cd ~/Downloads/atp-librewolf
cp -a "$P" ~/Downloads/atp-librewolf/profile-backup
cp -r geckium/x/"Profile Folder"/chrome geckium/x/"Profile Folder"/chrThemes "$P/"
cp -r aero-userchrome/x/Aero aero-userchrome/x/CSS aero-userchrome/x/GTK "$P/chrome/"
cp aero-userchrome/x/JS/aero_button_glow.uc.js "$P/chrome/JS/"
printf '\n/* Aero UserChrome (Windows 7 glass window frame) */\n@import url("Aero/Geckium.css");\n' >> "$P/chrome/userChrome.css"
cat "$E/librewolf-aero-buttons-100pct.css" "$E/librewolf-urlbar-dark.css" >> "$P/chrome/userChrome.css"
cp "$E/librewolf.overrides.cfg" ~/.config/librewolf/librewolf/librewolf.overrides.cfg
sudo cp "$E/geckium-sandbox.js" /usr/share/librewolf/defaults/pref/geckium-sandbox.js
```

What the two CSS files do: `librewolf-aero-buttons-100pct.css` keeps the caption buttons at 100%
(matching the window buttons of other programs) when LibreWolf runs at 125%;
`librewolf-urlbar-dark.css` gives the address bar a dark grey in both states with Geckium's dark
mode, and hides the suggestion list under it (its background stayed white; autofill still works).

### 4. First start and checks

Start LibreWolf; Geckium's setup window appears (it may restart the browser once or twice). In it,
pick a Chrome version; my screenshots use **Chrome 58 (2017)**. Then
check in `about:config` that LibreWolf's own settings still load:

| Setting | Must show |
|---|---|
| `librewolf.cfg.version` | a version number (only exists if LibreWolf's settings loaded) |
| `general.config.filename` | `librewolf.cfg` |
| `general.config.sandbox_enabled` | `false` |
| `privacy.resistFingerprinting` | `true` |

### 5. Settings in `about:config`

Create a setting if it doesn't exist (type the name, choose the type, click +):

| Setting | Value | Why |
|---|---|---|
| `widget.non-native-theme.enabled` | reset (default) | Geckium changes it; default = like every other LibreWolf user |
| `widget.gtk.overlay-scrollbars.enabled` | reset (default) | same; otherwise web pages can measure the scrollbar width |
| `widget.gtk.rounded-bottom-corners.enabled` | `true` | asked for by Aero UserChrome |
| `Geckium.appearance.titlebarStyle` | `win` (String) | Aero UserChrome only works with Windows caption buttons… |
| `Geckium.appearance.titlebarNative` | `1` (Number) | …and "native" (glass) titlebar mode |
| `Geckium.people.style` | `off` (String) | hides the profile button next to the caption buttons |
| `aero-userchrome.force-css-glow` | `true` (Boolean) | button glow inside the window (the pop-up glow lands in the wrong place on Wayland) |
| `layout.css.devPixelsPerPx` | `1.25` | whole browser at 125%, without zoom (optional) |

Keep **Title bar** off in Customize Toolbar.

### 6. Let the Aero Glass Blur effect blur LibreWolf

System Settings → Desktop Effects → Aero Glass Blur → configure → *"Classes of Firefox windows to
apply custom blur:"* → add a line `librewolf` under `firefox` → Apply.

### Optional: let websites see dark mode

LibreWolf's anti-fingerprinting tells every website "light mode". `optional-dark-websites.cfg`
has the recipe from LibreWolf's FAQ to let websites see dark mode while keeping all other
protections; add its lines to `~/.config/librewolf/librewolf/librewolf.overrides.cfg`.
LibreWolf advises against this: it makes you one of a smaller group.

### Undo

Close LibreWolf; `sudo rm /usr/share/librewolf/defaults/pref/geckium-sandbox.js`; delete
`~/.config/librewolf/librewolf/librewolf.overrides.cfg`; put the profile backup back.
