# Fork notes: decisions and history

Design decisions and history of this Kubuntu 26.04 LTS fork. The fixes themselves are listed in
the README; each commit message explains its change.

## Decisions

**Plasma/6.6 instead of upstream's newest code (2026-10-19).** Kubuntu 26.04 LTS ships Plasma
6.6.6 for its whole lifetime; upstream's default branch targets Plasma 6.8. aeroshell-sddm-kcm has
no 6.6 branch, so it's pinned to `4a65ae2`, its last 6.6.5 version (newer commits need the 6.7
workspace).

**libplasma merged with KDE v6.6.6 (2026-10-19).** The theme's libplasma was labelled 6.6.5; with
Ubuntu's own 6.6.6 installed, `ldconfig` could switch the theme's library off. Merging KDE's
official 6.6.6 (no conflicts) keeps the version equal, and an apt hold plus a login check
(`tools/`) protect it from updates.

**Version checks lowered (2026-10-19).** aeroshell-workspace asked for PlasmaActivities 6.6.5
(Ubuntu: 6.6.4, identical code) and aeroshell-kwin-components for wayland-protocols 1.48 (Ubuntu:
1.47; the code doesn't use it, KWin needs 1.45).

**Bigger font instead of fractional scaling (2026-10-20).** Wayland's 125% scaling blurs the
theme's pixel artwork. Instead: 100% scaling with a bigger system font, and the theme's fixed
pixel sizes made to follow the font, snapping icons to sizes the icon theme has as real images.

**Everything bundled and frozen (2026-10-24).** Upstream's install script downloads each
component's newest version. For a fork that won't follow upstream, the working versions are
included in `repos/` and the script builds from there.

**Release builds (2026-10-23).** Without a build type the components were built as Debug
(unoptimised, e.g. libPlasma 9.4 MB instead of ~0.45 MB). Release matters most for the KWin
effects, which run every frame.

**Aero Dolphin as is (2026-10-23).** The fork is KDE's Dolphin development branch of 2026-05-23
(labelled 25.12.3) plus the Aero changes; used as is rather than porting the changes onto Ubuntu's
25.12.3. Works with KDE Frameworks 6.24.

**LibreWolf via its overrides file (2026-10-22).** Geckium's instructions replace the browser's
autoconfig file, which LibreWolf uses for its privacy settings. The loader goes into LibreWolf's
user file `librewolf.overrides.cfg` instead; only the startup sandbox is switched off.

## History

- 2026-10-19: fresh Kubuntu 26.04 install, theme built on the Plasma/6.6 branches.
- 2026-10-20 to 2026-10-24: multi-screen, scaling and Wayland fixes (see README and commits).
- 2026-10-24: published as a fork of the GitHub mirror.
- 2026-10-26: CLAUDE.md and these notes added.
