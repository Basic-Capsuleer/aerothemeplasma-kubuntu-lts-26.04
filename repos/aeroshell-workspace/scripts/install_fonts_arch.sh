#!/bin/bash

LACKS_DEPS=
if [[ -z "$(command -v 7z)" ]]; then
    echo "7z not found."
    LACKS_DEPS=1
fi
if [[ -z "$(command -v makepkg)" ]]; then
    echo "makepkg not found."
    LACKS_DEPS=1
fi
if [[ -z "$(command -v git)" ]]; then
    echo "git not found."
    LACKS_DEPS=1
fi

if [[ ! -z $LACKS_DEPS ]]; then
    echo "One or more dependencies were not found. Stopping."
    exit
fi


if [ -z $1 ]; then
	echo "Missing argument!"
	echo "$ ./install.sh [path to valid Windows 7 ISO]"
	exit
fi

ISO_PATH=$(realpath $1)
rm -rf /tmp/win7fonts
mkdir -p /tmp/win7fonts
cd /tmp/win7fonts

echo "Extracting install.wim..."
7z x $ISO_PATH sources/install.wim
echo "Cloning ttf-win7-fonts..."
git clone https://aur.archlinux.org/ttf-win7-fonts.git
echo "Extracting fonts..."
7z x sources/install.wim 1/Windows/Fonts

for filename in 1/Windows/Fonts/*; do
	mv $filename ttf-win7-fonts/
done
# hacks for calibril.ttf calibrili.ttf 
cd ttf-win7-fonts
cp calibri.ttf calibril.ttf
cp calibri.ttf calibrili.ttf 

makepkg -si 
