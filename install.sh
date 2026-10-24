#!/bin/bash
CUR_DIR=${PWD}

# This fork includes every component (frozen at versions that work on Kubuntu 26.04 LTS with
# Plasma 6.6.6) in repos/, so nothing is downloaded: each part is built from its folder there.

SU_CMD=sudo
if [[ -z "$(command -v $SU_CMD)" ]]; then
    SU_CMD=doas
    if [[ -z "$(command -v $SU_CMD)" ]]; then
        echo "Neither sudo or doas were detected on the system."
        exit
    fi
fi


if [ -z $LIBEXEC_DIR ]; then
    LIBEXEC_DIR=lib
    UAC_LIBEXEC_DIR=lib
fi

if [[ "$(command -v dnf)" ]]; then # Automatically change for Fedora
    LIBEXEC_DIR=libexec
    UAC_LIBEXEC_DIR=libexec/kf6
fi

mkdir -p manifest
cd repos || exit 1

# libplasma last
cd libplasma || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/libplasma_install_manifest.txt"
cd "$CUR_DIR/repos"

# uac-polkit-agent
cd uac-polkit-agent || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBEXECDIR=$UAC_LIBEXEC_DIR -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/uac-polkit-agent_install_manifest.txt"
cd "$CUR_DIR/repos"

# SMOD
cd smod || exit 1
bash install.sh $@
cp build/install_manifest.txt "$CUR_DIR/manifest/smod_install_manifest.txt"
cp smodglow/build-wl/install_manifest.txt "$CUR_DIR/manifest/smodglow_install_manifest.txt"

if [[ ! "$*" == *"--skip-x11"* ]]
then
    cp smodglow/build/install_manifest.txt "$CUR_DIR/manifest/smodglow-x11_install_manifest.txt"
fi
cd "$CUR_DIR/repos"

# Aeroshell Workspace
cd aeroshell-workspace || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
$SU_CMD update-mime-database "/usr/local/share/mime"
cp build/install_manifest.txt "$CUR_DIR/manifest/aeroshell-workspace_install_manifest.txt"
cd "$CUR_DIR/repos"

# Aeroshell KWin
cd aeroshell-kwin-components || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKWIN_BUILD_WAYLAND=ON -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/aeroshell-kwin-components_install_manifest.txt"
if [[ ! "$*" == *"--skip-x11"* ]]
then
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DKWIN_BUILD_WAYLAND=OFF -DKWIN_INSTALL_MISC=OFF -B build_x11 . || exit 1
    cmake --build build_x11 || exit 1
    $SU_CMD cmake --install build_x11 || exit 1
    cp build_x11/install_manifest.txt "$CUR_DIR/manifest/aeroshell-kwin-components-x11_install_manifest.txt"
fi
cd "$CUR_DIR/repos"

# Aeroshell SDDM KCM
cd aeroshell-sddm-kcm || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/aeroshell-sddm-kcm_install_manifest.txt"
cd "$CUR_DIR/repos"

# Aerothemeplasma icons
cd aerothemeplasma-icons || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/icons_install_manifest.txt"
cd "$CUR_DIR/repos"

# Aerothemeplasma sounds
cd aerothemeplasma-sounds || exit 1
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/sounds_install_manifest.txt"
cd "$CUR_DIR/repos"

# Aerothemeplasma
cd "$CUR_DIR"
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBEXECDIR=$LIBEXEC_DIR -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1
cp build/install_manifest.txt "$CUR_DIR/manifest/aerothemeplasma_install_manifest.txt"
if [[ ! "$*" == *"--skip-x11"* ]]
then
    cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_LIBEXECDIR=$LIBEXEC_DIR -DINSTALL_X11_COMPONENTS=ON -B build_x11 . || exit 1
    cmake --build build_x11 || exit 1
    $SU_CMD cmake --install build_x11 || exit 1
    cp build_x11/install_manifest.txt "$CUR_DIR/manifest/aerothemeplasma-x11_install_manifest.txt"
fi
cd "$CUR_DIR/repos"


echo "Done."
