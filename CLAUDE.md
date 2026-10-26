# CLAUDE.md

Frozen personal fork of AeroThemePlasma for **Kubuntu 26.04 LTS, Plasma 6.6.6, Wayland only**,
with a bigger system font (Segoe UI 11–12 at 100% scaling) and multiple screens. Public on GitHub
as a fork of the read-only mirror `aeroshell-desktop/aerothemeplasma`; the real upstream is
https://gitgud.io/wackyideas/aerothemeplasma. No support, not actively maintained.

## Rules
- Only branch: `main`. Never use GitHub's "Sync fork" and never merge upstream: upstream targets
  newer Plasma versions. The `main` branch has a ruleset (no deletion, no force push, pull
  requests required; repository admin may bypass).
- All components are bundled and frozen in `repos/` (plus Aero Dolphin in `apps/aero-dolphin/`);
  `install.sh` builds from these folders and downloads nothing. Keep it that way.
- Changes must keep the standard font size unchanged; size fixes scale with the system font.
- British English in docs, commit messages and comments.
- Each bundled component keeps its own license files.
- The repository name contains a version on purpose (it will not move to another Kubuntu version).

## Layout
- `README.md` (what it is, disclaimer, fixes, screenshots), `INSTALL.md` (the guide)
- `README.upstream.md`, `INSTALL.upstream.md`: the original project's documents
- `repos/`: theme components; `apps/aero-dolphin/`: libAeroQt + Aero Dolphin
- `extras/librewolf/`: LibreWolf files; `tools/`: libplasma login check and hold script
- `docs/fork-notes.md`: decisions and history of this fork; the rest of `docs/` is upstream's

## State (2026-10-26)
Published 2026-10-24. Working on the maintainer's laptop and main PC setup; possible open item:
Aero Dolphin's tab close buttons don't scale with the font yet.
