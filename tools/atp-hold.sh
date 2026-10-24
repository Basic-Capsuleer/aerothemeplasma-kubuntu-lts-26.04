#!/bin/sh
# Puts every Ubuntu package that we replace with our own build on hold, so updates (apt and
# Discover) leave our versions alone. Run again after adding a package here; holding a package
# that is already on hold is harmless.
#   libplasma:  the Aero libplasma (built by install.sh)
#   Dolphin:    Aero Dolphin (apps/aero-dolphin)
# To release one later: sudo apt-mark unhold <package>

sudo apt-mark hold \
    libplasma7 libplasmaquick7 libplasma-dev \
    dolphin dolphin-data dolphin-doc libdolphinvcs6

apt-mark showhold
