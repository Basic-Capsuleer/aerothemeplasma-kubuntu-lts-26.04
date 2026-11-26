#!/bin/sh
# Puts every Ubuntu package that we replace with our own build, or that the theme is built against,
# on hold, so updates (apt and Discover) leave them alone until the theme is rebuilt for them. Run
# again after adding a package here; holding a package that is already on hold is harmless.
#   libplasma:   the Aero libplasma (built by install.sh)
#   KWin:        the theme's effects use KWin's internals and break when KWin changes
#   KDecoration: the SMOD window frames use KDecoration's private library
#   Dolphin:     Aero Dolphin (apps/aero-dolphin)
# To release one later: sudo apt-mark unhold <package>

sudo apt-mark hold \
    libplasma7 libplasmaquick7 libplasma-dev \
    kwin-wayland kwin-common kwin-data kwin-dev libkwin6 \
    libkdecorations3-6 libkdecorations3private2 libkdecorations3-dev \
    dolphin dolphin-data dolphin-doc libdolphinvcs6

apt-mark showhold
